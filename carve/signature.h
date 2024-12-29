#ifndef SIGNATURE
#define SIGNATURE

#include "common.h"
#include "xfs.h"
#include "filetypes/png.h"

extern const unsigned char *D_HEADER[];
extern const unsigned char *D_FOOTER[];

extern const size_t D_HEADER_SIZE[];
extern const size_t D_FOOTER_SIZE[];

extern const size_t D_HEADER_ARR_SIZE;
extern const size_t D_FOOTER_ARR_SIZE;

extern const size_t H_ARR_SIZE;
extern const size_t F_ARR_SIZE;

const std::map<uint64_t, std::string> filetypes = {{0, "png"}};

struct filerec_d {
        int type;
        int sign_type;  // HEADER = 0 FOOTER = 1
        uint64_t location;
};

#endif // !SIGNATURE
