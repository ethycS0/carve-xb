#ifndef RECOVERY
#define RECOVERY

#include "carve.h"
#include "common.h"
#include "xfs.h"

uint64_t find_footer(std::vector<filetype> &ft, xfs_superblock &sb, std::ifstream &fs,
                     filerec_d &header, xfs_fskey &key);
void make_file(uint64_t start, uint64_t end, std::ifstream &fs, filetype &ft); 

#endif // !RECOVERY
