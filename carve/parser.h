#ifndef PARSER
#define PARSER

#include "common.h"

struct FileSignature {
        std::string fileDescription;
        std::string headerHex;
        std::string fileExtension;
        std::string fileClass;
        std::string headerOffset;
        std::string trailerHex;
};

class filetype {
      public:
        std::string ext;
        std::string desc;
        int index;
        std::vector<unsigned char> Header;
        std::vector<unsigned char> Footer;

        size_t Header_s;
        size_t Footer_s;
};

std::vector<FileSignature> parseJsonFile(const std::string &filePath); 
std::vector<filetype> parseSignatures(const std::string &filename); 


#endif // !PARSER


