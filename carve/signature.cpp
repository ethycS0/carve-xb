#include "signature.h"

const unsigned char *D_HEADER[] = {
    PNG_HEAD,
};

const size_t D_HEADER_SIZE[] = {
    sizeof(PNG_HEAD),
};

const size_t D_FOOTER_SIZE[] = {
    sizeof(PNG_FOOTER),
};

const unsigned char *D_FOOTER[] = {
    PNG_FOOTER,
};

const size_t H_ARR_SIZE = sizeof(D_HEADER) / sizeof(D_HEADER[0]);
const size_t F_ARR_SIZE = sizeof(D_FOOTER) / sizeof(D_FOOTER[0]);

const size_t D_HEADER_ARR_SIZE = sizeof(D_HEADER);
const size_t D_FOOTER_ARR_SIZE = sizeof(D_FOOTER);
