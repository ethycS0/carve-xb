#include "xfs.h"

int read_superblock(xfs_superblock &sb, std::ifstream &fs) {
        fs.seekg(0, fs.beg);
        fs.read(reinterpret_cast<char *>(&sb), SECTORLEN);

        if (!fs) {
                std::cout << "Failed to read superblock. Read: " << fs.gcount() << std::endl;
                return -1;
        }

        if (htobe32(sb.sb_magicnum) != XFS_MAGICNO) {
                std::cout << "Invalid Magic No." << std::endl;
                return -1;
        }

        std::cout << "Successfully read superblock." << std::endl;
        return 0;
}

int read_agf(xfs_agf &ag, xfs_superblock &sb, std::ifstream &fs, uint64_t agno) {
        uint64_t offset = (agno * ntohl(sb.sb_blocksize) * ntohl(sb.sb_agblocks)) + 512;

        fs.seekg(offset, fs.beg);
        fs.read(reinterpret_cast<char *>(&ag), SECTORLEN);

        if (!fs) {
                std::cout << "Failed to read Allocation group. Read: " << fs.gcount() << std::endl;
                return -1;
        }

        if (htobe32(ag.agf_magicnum) != XFS_AGMAGICNO) {
                std::cout << "Invalid Magic No." << std::endl;
                return -1;
        }

        std::cout << "Successfully read Allocation Group: " << agno << std::endl;
        return 0;
}

bool is_error_key(xfs_fskey key) {
        return key.k_start == err_key.k_start && key.k_count == err_key.k_count;
}

uint64_t agblk_offset(uint32_t agno, xfs_superblock &sb) {

        uint64_t offset = static_cast<uint64_t>(agno) * static_cast<uint64_t>(ntohl(sb.sb_blocksize)) *
                          static_cast<uint64_t>(ntohl(sb.sb_agblocks));

        return offset;
}

int read_bnobtree(xfs_fsbtree &bb, xfs_agf &ag, xfs_superblock &sb, std::ifstream &fs) {

        uint64_t root = ntohl(ag.agf_bnoroots);
        uint64_t level = ntohl(ag.agf_bnolevel);
        uint64_t offset = agblk_offset(ntohl(ag.agf_seqno), sb);

        std::cout << "Found Block Number B+Tree root at: " << root << " with level: " << level
                  << std::endl;

        uint64_t actual_offset = offset + (4096 * root);
        std::cout << "Found BnoB+Tree at actual offset " << std::dec << actual_offset << std::endl;

        fs.seekg(actual_offset, fs.beg);
        fs.read(reinterpret_cast<char *>(&bb), SECTORLEN);
        if (!fs) {
                std::cout << "Failed to read block number b+tree. Read: " << fs.gcount()
                          << std::endl;
                return -1;
        }

        if (ntohl(bb.bb_magicno) != XFS_BNO[0] && ntohl(bb.bb_magicno) != XFS_BNO[1]) {
                std::cout << "Invalid Magic No." << std::endl;
                return -1;
        }

        uint64_t locations = htobe16(bb.bb_numrecs);
        std::cout << "Found " << locations << " locations of free space." << std::endl;

        return locations;
}

uint64_t append_keys(xfs_fsbtree &bb, xfs_agf &ag, uint64_t locations, int cnt, uint64_t level,
                     std::vector<xfs_fskey> &key) {
        int count = 0;
        if (level == 1) {

                uint64_t index = 0;
                for (uint64_t i = 0; i < locations; i++) {
                        key[i + cnt].k_agno = ntohl(ag.agf_seqno);
                        key[i + cnt].k_start = ntohl(bb.bb_data[SH_OFFSET + index]);
                        key[i + cnt].k_count = ntohl(bb.bb_data[SH_OFFSET + index + 1]);
                        index += 2;
                        count++;
                }
        } else {
                // Crawl B+tree to reach Leaf Nodes
        }
        return count;
}

void print_xfs_superblock(xfs_superblock *sb) {
        std::cout << "sb_magicnum: 0x" << std::hex << ntohl(sb->sb_magicnum) << std::endl;
        std::cout << "sb_blocksize: " << std::dec << ntohl(sb->sb_blocksize) << std::endl;
        std::cout << "sb_dblocks: " << be64toh(sb->sb_dblocks) << std::endl;
        std::cout << "sb_rblocks: " << be64toh(sb->sb_rblocks) << std::endl;
        std::cout << "sb_rextents: " << be64toh(sb->sb_rextents) << std::endl;
        std::cout << "sb_uuid: " << std::hex << be64toh(sb->sb_uuid[0]) << be64toh(sb->sb_uuid[1])
                  << std::endl;
        std::cout << "sb_logstart: " << be64toh(sb->sb_logstart) << std::endl;
        std::cout << "sb_rootino: " << be64toh(sb->sb_rootino) << std::endl;
        std::cout << "sb_rbmino: " << be64toh(sb->sb_rbmino) << std::endl;
        std::cout << "sb_rsumino: " << be64toh(sb->sb_rsumino) << std::endl;
        std::cout << "sb_rextsize: " << ntohl(sb->sb_rextsize) << std::endl;
        std::cout << "sb_agblocks: " << std::dec << ntohl(sb->sb_agblocks) << std::endl;
        std::cout << "sb_agcount: " << ntohl(sb->sb_agcount) << std::endl;
        std::cout << "sb_rbmblocks: " << ntohl(sb->sb_rbmblocks) << std::endl;
        std::cout << "sb_logblocks: " << ntohl(sb->sb_logblocks) << std::endl;
        std::cout << "sb_versionnum: 0x" << std::hex << ntohs(sb->sb_versionnum) << std::endl;
        std::cout << "sb_sectsize: " << ntohs(sb->sb_sectsize) << std::endl;
        std::cout << "sb_inodesize: " << ntohs(sb->sb_inodesize) << std::endl;
        std::cout << "sb_inopblock: " << ntohs(sb->sb_inopblock) << std::endl;
        std::cout << "sb_fname: " << be64toh(sb->sb_fname[0]) << be64toh(sb->sb_fname[1])
                  << std::endl;
        std::cout << "sb_blocklog: " << (uint64_t)sb->sb_blocklog << std::endl;
        std::cout << "sb_sectlog: " << (uint64_t)sb->sb_sectlog << std::endl;
        std::cout << "sb_inodelog: " << (uint64_t)sb->sb_inodelog << std::endl;
        std::cout << "sb_inopblog: " << (uint64_t)sb->sb_inopblog << std::endl;
        std::cout << "sb_agblklog: " << (uint64_t)sb->sb_agblklog << std::endl;
        std::cout << "sb_rextslog: " << (uint64_t)sb->sb_rextslog << std::endl;
        std::cout << "sb_inprogress: " << (uint64_t)sb->sb_inprogress << std::endl;
        std::cout << "sb_imax_pct: " << (uint64_t)sb->sb_imax_pct << std::endl;
        std::cout << "sb_icount: " << be64toh(sb->sb_icount) << std::endl;
        std::cout << "sb_ifree: " << be64toh(sb->sb_ifree) << std::endl;
        std::cout << "sb_fdblocks: " << be64toh(sb->sb_fdblocks) << std::endl;
        std::cout << "sb_frextents: " << be64toh(sb->sb_frextents) << std::endl;
        std::cout << "sb_crc: 0x" << std::hex << ntohl(sb->sb_crc) << std::endl;
        std::cout << std::endl;
}

void print_xfs_agf(const xfs_agf *agf) {
        std::cout << "agf_magicnum: 0x" << std::hex << ntohl(agf->agf_magicnum) << std::endl;
        std::cout << "agf_versionnum: " << ntohl(agf->agf_versionnum) << std::endl;
        std::cout << "agf_seqno: " << ntohl(agf->agf_seqno) << std::endl;
        std::cout << "agf_length: " << std::dec << ntohl(agf->agf_length) << std::endl;
        std::cout << "agf_bnoroots: " << ntohl(agf->agf_bnoroots) << std::endl;
        std::cout << "agf_cntroots: " << ntohl(agf->agf_cntroots) << std::endl;
        std::cout << "agf_rmaproots: " << ntohl(agf->agf_rmaproots) << std::endl;
        std::cout << "agf_bnolevel: " << ntohl(agf->agf_bnolevel) << std::endl;
        std::cout << "agf_cntlevel: " << ntohl(agf->agf_cntlevel) << std::endl;
        std::cout << "agf_rmaplevel: " << ntohl(agf->agf_rmaplevel) << std::endl;
        std::cout << "agf_flfirst: " << ntohl(agf->agf_flfirst) << std::endl;
        std::cout << "agf_fllast: " << ntohl(agf->agf_fllast) << std::endl;
        std::cout << "agf_flcount: " << ntohl(agf->agf_flcount) << std::endl;
        std::cout << "agf_freeblks: " << ntohl(agf->agf_freeblks) << std::endl;
        std::cout << "agf_longest: " << ntohl(agf->agf_longest) << std::endl;
        std::cout << "agf_btreeblks: " << ntohl(agf->agf_btreeblks) << std::endl;
        std::cout << "agf_uuid: " << std::hex << be64toh(agf->agf_uuid[0])
                  << be64toh(agf->agf_uuid[1]) << std::endl;
        std::cout << "agf_rmap_blocks: " << ntohl(agf->agf_rmap_blocks) << std::endl;
        std::cout << "agf_refcount_blocks: " << ntohl(agf->agf_refcount_blocks) << std::endl;
        std::cout << "agf_refcount_root: " << ntohl(agf->agf_refcount_root) << std::endl;
        std::cout << "agf_refcount_level: " << ntohl(agf->agf_refcount_level) << std::endl;
        std::cout << std::endl;
}
