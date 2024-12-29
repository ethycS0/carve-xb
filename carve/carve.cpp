#include "carve.h"

uint64_t signature_match(const std::vector<char> &buffer, std::streamsize bytes_read,
                         uint64_t offset, const unsigned char *sign) {
        for (int j = 0; j < bytes_read - 8; j++) {
                if (std::memcmp(&buffer[j], sign, 8) == 0) {
                        std::cout << "Signature Match Found at position: " << offset + j
                                  << std::endl;
                        return j + offset;
                }
        }
        return 0;
}

int d_carve(xfs_fskey &key, xfs_superblock &sb, std::ifstream &fs) {

        uint64_t agblocks = ntohl(sb.sb_agblocks);
        uint64_t blocksize = ntohl(sb.sb_blocksize);

        uint64_t agno = key.k_agno;
        uint64_t start = key.k_start;
        uint64_t count = key.k_count;

        std::cout << "AGNO: " << agno << std::endl
                  << "AGBlocks: " << agblocks << std::endl
                  << "Blocksize: " << blocksize << std::endl
                  << "Start: " << start << std::endl
                  << "Count: " << count << std::endl;

        uint64_t blk_offset = agblk_offset(agno, sb);
        uint64_t offset = blk_offset + (start * blocksize);
        uint64_t end = ((agno * agblocks) + (start + count)) * blocksize;

        uint64_t free_size = end - offset;

        std::cout << "Offset: " << offset << std::endl
                  << "End: " << end << std::endl
                  << "Free Size: " << free_size << std::endl;

        fs.seekg(offset, fs.beg);

        uint64_t current_header = 0;
        bool header_found = false;

        for (uint64_t i = 0; i < free_size; i += blocksize) {
                fs.seekg(offset + i);
                std::vector<char> buffer(blocksize);
                fs.read(buffer.data(), blocksize);
                std::streamsize bytes_read = fs.gcount();

                for (uint64_t j = 0; j < D_ARR_SIZE; j++) {
                        if (!header_found) {
                                current_header =
                                    signature_match(buffer, bytes_read, i, D_HEADER[j]);
                                if (current_header != 0) {
                                        header_found = true;
                                        uint64_t header_offset = current_header + offset;
                                        std::cout << filetypes.at(j)
                                                  << " header found at offset: " << header_offset
                                                  << std::endl;
                                }
                        }

                        if (header_found) {
                                uint64_t current_footer =
                                    signature_match(buffer, bytes_read, i, D_FOOTER[j]);
                                if (current_footer != 0) {
                                        uint64_t footer_offset = current_footer + 8 + offset;
                                        if (footer_offset > (current_header + offset)) {
                                                std::cout
                                                    << filetypes.at(j)
                                                    << " footer found at offset: " << footer_offset
                                                    << std::endl;

                                                parse_file(j, current_header + offset,
                                                           footer_offset, fs);

                                                header_found = false;
                                                current_header = 0;
                                        } else {
                                                std::cout
                                                    << "Footer found before header. Ignoring..."
                                                    << std::endl;
                                        }
                                }
                        }
                }
        }

        return 0;
}

int parse_file(uint64_t result, uint64_t header, uint64_t footer, std::ifstream &fs) {
        switch (result) {
        case 0:
                make_png(header, footer, fs);
                return 0;
        default:
                std::cout << "Header/Footer not implemented" << std::endl;
                return -1;
        }
}
