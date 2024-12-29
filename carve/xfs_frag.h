#ifndef XFS_FRAGMENTATION
#define XFS_FRAGMENTATION

#include "common.h"
#include "xfs.h"
#include "carve.h"

const unsigned char XFS_BMAP[] = {0x42, 0x4d, 0x41, 0x33};

void find_bmap(xfs_fskey key, xfs_superblock &sb, std::ifstream &fs);

#endif // !XFS_FRAGMENTATION
