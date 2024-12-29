#include "parser.h"

using json = nlohmann::json;



// Helper function to convert hex string to bytes
std::vector<unsigned char> hexStringToBytes(const std::string &hexStr) {
        std::vector<unsigned char> bytes;
        std::string hex = hexStr;

        // Remove spaces from hex string
        hex.erase(remove_if(hex.begin(), hex.end(), isspace), hex.end());

        // Convert hex string to bytes
        for (size_t i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                unsigned char byte = (unsigned char)strtol(byteString.c_str(), nullptr, 16);
                bytes.push_back(byte);
        }

        return bytes;
}

std::vector<filetype> parseSignatures(const std::string &filename) {
        std::vector<filetype> fileTypes;

        try {
                // Read JSON file
                std::ifstream file(filename);
                if (!file.is_open()) {
                        throw std::runtime_error("Unable to open file: " + filename);
                }

                json j;
                file >> j;
                int i = 0;
                // Parse file signatures
                for (const auto &sig : j["filesigs"]) {
                        filetype ft;

                        // Parse basic information
                        ft.ext = sig["File extension"].get<std::string>();
                        ft.desc = sig["File description"].get<std::string>();
                        ft.index = i;
                        i += 1;
                        // Parse header
                        std::string headerHex = sig["Header (hex)"].get<std::string>();
                        if (!headerHex.empty() && headerHex != "(null)") {
                                ft.Header = hexStringToBytes(headerHex);
                                ft.Header_s = ft.Header.size();
                        } else {
                                ft.Header_s = 0;
                        }

                        // Parse footer
                        std::string footerHex = sig["Trailer (hex)"].get<std::string>();
                        if (!footerHex.empty() && footerHex != "(null)") {
                                ft.Footer = hexStringToBytes(footerHex);
                                ft.Footer_s = ft.Footer.size();
                        } else {
                                ft.Footer_s = 0;
                        }

                        fileTypes.push_back(ft);
                }

        } catch (const std::exception &e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }

        return fileTypes;
}


// Example usage
// int main() {
//         std::vector<filetype> signatures = parseSignatures("file_sigs.json");
//
//         // Print parsed signatures
//         for (const auto &ft : signatures) {
//                 std::cout << "Extension: " << ft.ext << std::endl;
//                 std::cout << "Description: " << ft.desc << std::endl;
//
//                 std::cout << "Header (" << ft.Header_s << " bytes): ";
//                 for (const auto &byte : ft.Header) {
//                         printf("%02X ", byte);
//                 }
//                 std::cout << std::endl;
//
//                 std::cout << "Footer (" << ft.Footer_s << " bytes): ";
//                 for (const auto &byte : ft.Footer) {
//                         printf("%02X ", byte);
//                 }
//                 std::cout << std::endl << std::endl;
//         }
//
//         return 0;
// }
