#include "common.h"
#include "xfs.h"

int main() {
        xfs_superblock sb;
        xfs_agf ag;
        xfs_fsbtree bb;

        std::ifstream fs("../xfs_test.raw", std::ifstream::binary);
        // std::ifstream fs("/dev/sdb2", std::ifstream::binary);
        if (!fs) {
                std::cerr << "Failed to open file system image!" << std::endl;
                return -1;
        }

        if (mkdir("output", 0777) == -1) {
        } else {
                std::cout << " Output Directory created" << std::endl;
        }
        fs.seekg(0, fs.end);
        uint64_t file_size = fs.tellg();
        fs.seekg(0, fs.beg);

        std::cout << "File system size: " << file_size << " bytes" << std::endl;

        uint64_t result = read_superblock(sb, fs);
        if (result != 0) {
                return -1;
        }
        print_xfs_superblock(&sb);

        uint64_t agno = ntohl(sb.sb_agcount);
        int cnt = 0;
        std::vector<xfs_fskey> keys;
        for (uint64_t i = 0; i < agno; i++) {

                result = read_agf(ag, sb, fs, i);
                if (result != 0) {
                        return -1;
                }
                print_xfs_agf(&ag);

                int len = read_bnobtree(bb, ag, sb, fs);
                if (len < 1) {
                        return -1;
                }
                keys.resize(keys.size() + len);
                result = append_keys(bb, ag, len, cnt, ntohl(ag.agf_bnolevel), keys);
                if (result == 0) {
                        std::cout << "No free space in Allocation Group " << ag.agf_seqno
                                  << std::endl;
                }
                cnt = cnt + result;
        }

        for (int j = 0; j < cnt; j++) {
                std::cout << std::dec << (keys[j].k_agno) << ":" << std::dec << (keys[j].k_start)
                          << ":" << std::dec << (keys[j].k_count) << std::endl;
        }
        std::cout << std::endl;

        return 0;
}
