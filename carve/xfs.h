#ifndef XFS
#define XFS

#include "common.h"

const uint64_t SECTORLEN = 512;

const uint32_t XFS_MAGICNO = {0x58465342};
const uint32_t XFS_AGMAGICNO = {0x58414746};

const uint32_t XFS_BNO[2] = {0x41425442, 0x41423342};

const uint64_t SH_OFFSET = 12;

struct xfs_superblock {
        uint32_t sb_magicnum;   // Magic number (0x58465342 or 'XFSB')
        uint32_t sb_blocksize;  // Filesystem block size
        uint64_t sb_dblocks;    // Total number of data blocks
        uint64_t sb_rblocks;    // Total number of realtime blocks
        uint64_t sb_rextents;   // Total number of realtime extents
        uint64_t sb_uuid[2];    // Filesystem UUID (128 bits)
        uint64_t sb_logstart;   // Starting block of the log
        uint64_t sb_rootino;    // Root inode number
        uint64_t sb_rbmino;     // Realtime extents bitmap inode number
        uint64_t sb_rsumino;    // Realtime summary inode number
        uint32_t sb_rextsize;   // Realtime extent size (in blocks)
        uint32_t sb_agblocks;   // Number of blocks per AG
        uint32_t sb_agcount;    // Number of AGs
        uint32_t sb_rbmblocks;  // Number of blocks in the realtime bitmap
        uint32_t sb_logblocks;  // Number of log blocks
        uint16_t sb_versionnum; // Version number
        uint16_t sb_sectsize;   // Sector size
        uint16_t sb_inodesize;  // Inode size
        uint16_t sb_inopblock;  // Inodes per block
        char sb_fname[12];      // Filesystem name (12 characters)
        uint8_t sb_blocklog;    // log2 of block size
        uint8_t sb_sectlog;     // log2 of sector size
        uint8_t sb_inodelog;    // log2 of inode size
        uint8_t sb_inopblog;    // log2 of inodes per block
        uint8_t sb_agblklog;    // log2 of blocks per AG
        uint8_t sb_rextslog;    // log2 of realtime extents
        uint8_t sb_inprogress;  // Filesystem is being made
        uint8_t sb_imax_pct;    // Max % of inodes allocated
        uint64_t sb_icount;     /* allocated inodes */
        uint64_t sb_ifree;      /* free inodes */
        uint64_t sb_fdblocks;   /* free data blocks */
        uint64_t sb_frextents;  /* free realtime extents */
        uint8_t sb_pad1[64];    // Padding to align structure
        uint32_t sb_crc;        // CRC32 checksum for superblock
        uint8_t sb_pad2[279];   // Padding to align structure
};

struct xfs_agf {
        uint32_t agf_magicnum;   // Magic number (0x58414746 or 'XAGF')
        uint32_t agf_versionnum; // Version number
        uint32_t agf_seqno;      // Allocation group sequence number
        uint32_t agf_length;     // Length of this structure in bytes

        uint32_t agf_bnoroots;  // bnobt Root inode number
        uint32_t agf_cntroots;  // cntbt Root inode number
        uint32_t agf_rmaproots; // rmapbt Root inode number

        uint32_t agf_bnolevel;  // bnobt Level of the allocation group free space tree
        uint32_t agf_cntlevel;  // cntbt Level of the allocation group free space tree
        uint32_t agf_rmaplevel; // rmapbt Level of the allocation group free space tree

        uint32_t agf_flfirst;  // First freelist block's index
        uint32_t agf_fllast;   // Last freelist block's index
        uint32_t agf_flcount;  // Count of blocks in freelist
        uint32_t agf_freeblks; // Total free blocks

        uint32_t agf_longest;   // Longest free space
        uint32_t agf_btreeblks; // # of blocks held in AGF B-trees
        uint64_t agf_uuid[2];   // UUID of filesystem (16 bytes)

        uint32_t agf_rmap_blocks;     // Rmapbt blocks used
        uint32_t agf_refcount_blocks; // Refcountbt blocks used

        uint32_t agf_refcount_root;  // Refcount tree root block
        uint32_t agf_refcount_level; // Refcount B-tree levels
        uint8_t agf_pad[424];        // Padding to align structure
};

struct xfs_fsbtree {
        uint32_t bb_magicno;
        uint16_t bb_level;
        uint16_t bb_numrecs;
        uint32_t bb_data[126];
};

struct xfs_fskey {
        int k_agno;
        uint32_t k_start;
        uint32_t k_count;
};

static const xfs_fskey err_key = {0, 0};

int read_superblock(xfs_superblock &sb, std::ifstream &fs);
int read_agf(xfs_agf &ag, xfs_superblock &sb, std::ifstream &fs, uint64_t agno);
int read_bnobtree(xfs_fsbtree &bb, xfs_agf &ag, xfs_superblock &sb, std::ifstream &fs);

uint64_t append_keys(xfs_fsbtree &bb, xfs_agf &ag, uint64_t locations, int cnt, uint64_t level, std::vector<xfs_fskey> &key); 
uint64_t agblk_offset(uint32_t agno, xfs_superblock &sb); 
bool is_error_key(xfs_fskey key);

void print_xfs_superblock(xfs_superblock *sb);
void print_xfs_agf(const xfs_agf *agf);

#endif // !XFS
