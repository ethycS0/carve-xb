#include "carve.h"

#define CUDA_BLOCKS 56
#define THREADS_PER_BLOCK 256

__global__ void signature_match_h(unsigned char *buffer, filerec_d *d_results, uint64_t offset,
                                  uint64_t blocksize, unsigned char *d_header,
                                  size_t *d_header_offsets, size_t *d_header_size,
                                  int *d_header_type, ssize_t ARR_SIZE, uint64_t num_blocks,
                                  uint64_t processed, int *global_counter) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= num_blocks) {
                return;
        }

        uint64_t already_processed = (processed * blocksize);
        uint64_t block_start = idx * blocksize;

        for (size_t i = 0; i < blocksize; i++) {
                uint64_t current_pos = block_start + i;

                for (size_t j = 0; j < ARR_SIZE; j++) {
                        uint64_t d_strt = d_header_offsets[j];
                        size_t pattern_size = d_header_size[j];
                        bool match = true;

                        if (current_pos + pattern_size > (idx + 1) * blocksize) {
                                continue;
                        }
                        for (size_t k = 0; k < pattern_size; k++) {
                                if (buffer[current_pos + k] != d_header[d_strt + k]) {
                                        match = false;
                                        break;
                                }
                        }

                        if (match) {
                                int result_idx = atomicAdd(global_counter, 1);

                                d_results[result_idx].type = d_header_type[j];
                                d_results[result_idx].location =
                                    current_pos + offset + already_processed;
                                d_results[result_idx].sign_type = 1;
                        }
                }
        }
}

__global__ void signature_match_f(unsigned char *buffer, filerec_d *d_results, uint64_t offset,
                                  uint64_t blocksize, unsigned char *d_footer,
                                  size_t *d_footer_offsets, size_t *d_footer_size,
                                  int *d_footer_type, ssize_t ARR_SIZE, uint64_t num_blocks,
                                  uint64_t processed, int *global_counter) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= num_blocks) {
                return;
        }

        uint64_t already_processed = (processed * blocksize);
        uint64_t block_start = idx * blocksize;

        for (size_t i = 0; i < blocksize; i++) {
                uint64_t current_pos = block_start + i;

                for (size_t j = 0; j < ARR_SIZE; j++) {
                        if(d_footer_size[j] == 0) {
                                continue;
                        } 
                        uint64_t d_strt = d_footer_offsets[j];
                        size_t pattern_size = d_footer_size[j];
                        bool match = true;

                        if (current_pos + pattern_size > (idx + 1) * blocksize) {
                                continue;
                        }
                        for (size_t k = 0; k < pattern_size; k++) {
                                if (buffer[current_pos + k] != d_footer[d_strt + k]) {
                                        match = false;
                                        break;
                                }
                        }

                        if (match) {
                                int result_idx = atomicAdd(global_counter, 1);

                                d_results[result_idx].type = d_footer_type[j];
                                d_results[result_idx].location =
                                    current_pos + offset + already_processed;
                                d_results[result_idx].sign_type = 2;
                        }
                }
        }
}

void CudaErr(cudaError_t error) {
        if (error != cudaSuccess) {
                std::cout << "CUDA error: " << cudaGetErrorString(error) << std::endl;
                exit(EXIT_FAILURE);
        }
}

int d_carve(xfs_fskey &key, std::vector<filerec_d> &fr, xfs_superblock &sb, std::ifstream &fs,
            std::vector<filetype> &ft) {
        // Check Cuda
        cudaError_t cudaStatus;
        cudaStatus = cudaSetDevice(0);
        if (cudaStatus != cudaSuccess) {
                std::cout << "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?"
                          << std::endl;
        }

        cudaError_t err;

        // Initialize Useful Variables
        uint64_t agblocks = ntohl(sb.sb_agblocks);
        uint64_t blocksize = ntohl(sb.sb_blocksize);

        uint64_t agno = key.k_agno;
        uint64_t start = key.k_start;
        uint64_t count = key.k_count;

        uint64_t blk_offset = agblk_offset(agno, sb);
        uint64_t offset = blk_offset + (start * blocksize);
        uint64_t total_blocks = count;
        const uint64_t batch_size = CUDA_BLOCKS * THREADS_PER_BLOCK;

        fs.seekg(0, fs.end);
        size_t eof = fs.tellg();

        //  Counter
        int *d_header_counter;
        cudaMalloc(&d_header_counter, sizeof(int));
        cudaMemset(d_header_counter, 0, sizeof(int));

        int *d_footer_counter;
        cudaMalloc(&d_footer_counter, sizeof(int));
        cudaMemset(d_footer_counter, 0, sizeof(int));

        // Creating Header Arrays
        size_t total_header_size = 0;
        size_t h_offset = 0;
        std::vector<size_t> host_header_sizes(ft.size());
        std::vector<int> host_header_types(ft.size());
        for (size_t i = 0; i < ft.size(); ++i) {
                if (ft[i].Header_s != 0) {
                        total_header_size += ft[i].Header_s;
                        host_header_sizes[i] = ft[i].Header_s;
                        host_header_types[i] = ft[i].index;
                }
        }

        unsigned char *h_flat_headers = new unsigned char[total_header_size];
        size_t header_offsets[ft.size() * sizeof(size_t)];

        for (size_t i = 0; i < ft.size(); ++i) {
                if (ft[i].Header_s != 0) {
                        header_offsets[i] = h_offset;
                        std::memcpy(h_flat_headers + h_offset, ft[i].Header.data(), ft[i].Header_s);
                        h_offset += ft[i].Header_s;
                }
        }

        // Creating Footer Arrays
        size_t total_footer_size = 0;
        size_t f_offset = 0;
        std::vector<size_t> host_footer_sizes(ft.size());
        std::vector<int> host_footer_types(ft.size());
        for (size_t i = 0; i < ft.size(); ++i) {
                if (ft[i].Footer_s != 0) {
                        total_footer_size += ft[i].Footer_s;
                        host_footer_sizes[i] = ft[i].Footer_s;
                        host_footer_types[i] = ft[i].index;
                }
        }

        unsigned char *h_flat_footer = new unsigned char[total_footer_size];
        size_t footer_offsets[ft.size() * sizeof(size_t)];

        for (size_t i = 0; i < ft.size(); ++i) {
                if (ft[i].Footer_s != 0) {
                        footer_offsets[i] = f_offset;
                        std::memcpy(h_flat_footer + f_offset, ft[i].Footer.data(), ft[i].Footer_s);
                        f_offset += ft[i].Footer_s;
                }
        }

        // Device buffers
        unsigned char *d_buffer;
        filerec_d *h_results;
        filerec_d *f_results;

        unsigned char *d_flat_headers;
        size_t *d_header_offsets;
        size_t *d_header_size;
        int *d_header_type;

        unsigned char *d_flat_footers;
        size_t *d_footer_offsets;
        size_t *d_footer_size;
        int *d_footer_type;

        // Allocating Device Buffers
        err = cudaMalloc(&d_buffer, (batch_size * blocksize));
        CudaErr(err);

        size_t max_results = total_blocks * ft.size();
        err = cudaMalloc(&h_results, max_results * sizeof(filerec_d));
        CudaErr(err);
        err = cudaMalloc(&f_results, max_results * sizeof(filerec_d));
        CudaErr(err);
        err = cudaMalloc(&d_flat_headers, total_header_size);
        CudaErr(err);
        err = cudaMalloc(&d_header_offsets, ft.size() * sizeof(size_t));
        CudaErr(err);
        err = cudaMalloc(&d_header_size, ft.size() * sizeof(size_t));
        CudaErr(err);
        err = cudaMalloc(&d_header_type, ft.size() * sizeof(int));
        CudaErr(err);

        err = cudaMalloc(&d_flat_footers, total_footer_size);
        CudaErr(err);
        err = cudaMalloc(&d_footer_offsets, ft.size() * sizeof(size_t));
        CudaErr(err);
        err = cudaMalloc(&d_footer_size, ft.size() * sizeof(size_t));
        CudaErr(err);
        err = cudaMalloc(&d_footer_type, ft.size() * sizeof(int));
        CudaErr(err);

        // Host -> Device Copying
        err = cudaMemcpy(d_flat_headers, h_flat_headers, total_header_size, cudaMemcpyHostToDevice);
        CudaErr(err);
        err = cudaMemcpy(d_header_size, host_header_sizes.data(), ft.size() * sizeof(size_t),
                         cudaMemcpyHostToDevice);
        CudaErr(err);
        err = cudaMemcpy(d_header_type, host_header_types.data(), ft.size() * sizeof(int),
                         cudaMemcpyHostToDevice);
        CudaErr(err);
        err = cudaMemcpy(d_header_offsets, header_offsets, ft.size() * sizeof(size_t),
                         cudaMemcpyHostToDevice);
        CudaErr(err);

        err = cudaMemcpy(d_flat_footers, h_flat_footer, total_footer_size, cudaMemcpyHostToDevice);
        CudaErr(err);
        err = cudaMemcpy(d_footer_size, host_footer_sizes.data(), ft.size() * sizeof(size_t),
                         cudaMemcpyHostToDevice);
        CudaErr(err);
        err = cudaMemcpy(d_footer_type, host_footer_types.data(), ft.size() * sizeof(int),
                         cudaMemcpyHostToDevice);
        CudaErr(err);
        err = cudaMemcpy(d_footer_offsets, footer_offsets, ft.size() * sizeof(size_t),
                         cudaMemcpyHostToDevice);
        CudaErr(err);

        err = cudaMemset(h_results, 0, total_blocks * sizeof(filerec_d));
        CudaErr(err);
        err = cudaMemset(f_results, 0, total_blocks * sizeof(filerec_d));
        CudaErr(err);

        // Main Host Buffer
        char *host_buffer = new char[blocksize * batch_size];

        // Loop analyze batch_size over free space
        for (uint64_t processed_blocks = 0; processed_blocks < total_blocks;
             processed_blocks += batch_size) {
                uint64_t current_batch_size = batch_size;
                if (processed_blocks + batch_size > total_blocks) {
                        current_batch_size = total_blocks - processed_blocks;
                }
                uint64_t r_size;

                // Filesystem Read to host buffer
                int64_t seek_pos = offset + (processed_blocks * blocksize);
                fs.seekg(seek_pos, fs.beg);
                if (offset + (processed_blocks * blocksize) + (blocksize * current_batch_size) >
                    eof) {
                        r_size = eof - (offset + (processed_blocks * blocksize));
                } else {
                        r_size = (blocksize * current_batch_size);
                }

                fs.read(host_buffer, r_size);
                if (!fs.good()) {
                        std::cout << "Error: Read Error" << std::endl;
                        return -1;
                }

                // Host -> Device main buffer
                err = cudaMemcpy(d_buffer, host_buffer, r_size, cudaMemcpyHostToDevice);
                CudaErr(err);

                // Kernel
                signature_match_h<<<CUDA_BLOCKS, THREADS_PER_BLOCK>>>(
                    d_buffer, h_results, offset, blocksize, d_flat_headers, d_header_offsets,
                    d_header_size, d_header_type, ft.size(), current_batch_size, processed_blocks,
                    d_header_counter);
                cudaDeviceSynchronize();
                // signature_match_f<<<CUDA_BLOCKS, THREADS_PER_BLOCK>>>(
                //     d_buffer, f_results, offset, blocksize, d_flat_footers, d_footer_offsets,
                //     d_footer_size, d_footer_type, ft.size(), current_batch_size, processed_blocks,
                //     d_footer_counter);
                // cudaDeviceSynchronize();

                // Host Vector to store results
                int header_count, footer_count;
                cudaMemcpy(&header_count, d_header_counter, sizeof(int), cudaMemcpyDeviceToHost);
                // cudaMemcpy(&footer_count, d_footer_counter, sizeof(int), cudaMemcpyDeviceToHost);

                std::vector<filerec_d> header_results(header_count);
                // std::vector<filerec_d> /* footer_results */(footer_count);

                err = cudaMemcpy(header_results.data(), h_results, header_count * sizeof(filerec_d),
                                 cudaMemcpyDeviceToHost);
                CudaErr(err);
                // err = cudaMemcpy(footer_results.data(), f_results, footer_count * sizeof(filerec_d),
                                 // cudaMemcpyDeviceToHost);
                // CudaErr(err);
                for (int i = 0; i < header_count; i++) {
                        fr.push_back(header_results[i]);
                }

                // for (int i = 0; i < footer_count; i++) {
                        // fr.push_back(footer_results[i]);
                // }
        }
        cudaFree(d_buffer);
        cudaFree(h_results);
        cudaFree(f_results);
        cudaFree(d_flat_headers);
        cudaFree(d_header_offsets);
        cudaFree(d_header_size);
        cudaFree(d_header_type);
        cudaFree(d_flat_footers);
        cudaFree(d_footer_offsets);
        cudaFree(d_footer_size);
        cudaFree(d_footer_type);
        cudaFree(d_header_counter);
        cudaFree(d_footer_counter);
        delete[] h_flat_headers;
        delete[] h_flat_footer;
        delete[] host_buffer;

        return 0;
}
