#include "recovery.h"

uint64_t find_footer(std::vector<filetype> &ft, xfs_superblock &sb, std::ifstream &fs,
                     filerec_d &header, xfs_fskey &key) {
        uint64_t blocksize = ntohl(sb.sb_blocksize);

        uint64_t agno = key.k_agno;
        uint64_t u_start = key.k_start;
        uint64_t count = key.k_count;

        uint64_t blk_offset = agblk_offset(agno, sb);
        uint64_t start = blk_offset + (u_start * blocksize);
        uint64_t end = start + (count * blocksize);
        uint64_t bytes = count * blocksize;

        uint64_t header_location = header.location;
        int type = header.type;

        int footer_type = -1;
        for (size_t i = 0; i < ft.size(); i++) {
                if (type == ft[i].index) {
                        footer_type = ft[i].index;
                }
        

        if (footer_type == -1) {
                std::cout << "Error: footer not found" << std::endl;
                return 0;
        }

        size_t footlen = ft[footer_type].Footer_s;
        std::vector<unsigned char> footer = ft[footer_type].Footer;

        if (header_location < start || header_location > end) {
                std::cout << "Error: Header Memory Issue" << std::endl;
                return 0;
        }

        fs.seekg(header_location);

        const uint64_t batchsize = 4096;
        char buffer[batchsize];

        for (uint64_t i = 0; i < bytes; i += batchsize) {
                fs.read(buffer, batchsize);
                for (uint64_t j = 0; j < batchsize; j++) {
                        bool found = true;

                        if (j + footlen > batchsize) {
                                continue;
                        }

                        for (uint64_t k = 0; k < footlen; k++) {
                                if (buffer[j + k] != footer[k]) {
                                        found = false;
                                        break;
                                }
                        }
                        if (found == true) {
                                return (header_location + i + j + footlen);
                        }
                }
        }
        return 0;
}

void make_file(uint64_t start, uint64_t end, std::ifstream &fs, filetype &ft) {
        if (end < start) {
                std::cout << "You Fucked Up" << std::endl;
                return;
        }

        std::string filename = "output/" + std::to_string(std::rand()) + "." + ft.ext;

        std::ofstream out(filename, std::ios::binary);
        if (!out) {
                std::cerr << "Failed to create output file: " << filename << std::endl;
                return;
        }

        uint64_t size = end - start;
        fs.seekg(start);
        std::vector<char> buffer(size);
        fs.read(buffer.data(), size);

        out.write(buffer.data(), size);
        out.close();

        std::cout << "Recovered " << ft.desc << ": " << filename << " (" << size << " bytes)"
                  << std::endl;
}
