#ifndef CARVE
#define CARVE

#include "common.h"
#include "parser.h"
#include "signature.h"
#include "xfs.h"

int d_carve(xfs_fskey &key, std::vector<filerec_d> &fr, xfs_superblock &sb, std::ifstream &fs,
            std::vector<filetype> &ft);
uint64_t signature_match(const std::vector<char> &buffer, std::streamsize bytes_read,
                         uint64_t offset, const unsigned char *sign);
int parse_file(uint64_t result, uint64_t header, uint64_t footer, std::ifstream &fs);

#endif // !CARVE
