#include "xfs_frag.h"

void find_bmap(xfs_fskey key, xfs_superblock &sb, std::ifstream &fs) {
        uint64_t offset = agblk_offset((key.k_agno), sb);
        uint64_t actual_offset = offset + (key.k_start);

        uint64_t size = (key.k_count);
        uint64_t blocksize = ntohl(sb.sb_blocksize);

        fs.seekg(actual_offset);

        std::cout << "Trying to find fragmented files at : " << actual_offset
                  << " in Allocation Group: " << ntohl(key.k_agno) << std::endl;

        for (uint64_t i = 0; i < size; i++) {
                fs.seekg(actual_offset + (i * blocksize));
                if (fs.fail()) {
                        std::cerr << "Seek failed at offset: " << actual_offset + (i * blocksize)
                                  << std::endl;
                }
                std::vector<char> buffer(blocksize);
                fs.read(buffer.data(), blocksize);
                std::streamsize bytes_read = fs.gcount();

                signature_match(buffer, bytes_read, (actual_offset + (i * blocksize)), XFS_BMAP);
        }
}
