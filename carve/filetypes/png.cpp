#include "png.h"

uint64_t png_recovered = 0;

void make_png(uint64_t start, uint64_t end, std::ifstream &fs) {
        if (end < start) {
                std::cout << "You Fucked Up" << std::endl;
                return;
        }

        png_recovered++;
        std::string filename = "output/" + std::to_string(png_recovered) + ".png";

        std::ofstream out(filename, std::ios::binary);
        if (!out) {
                std::cerr << "Failed to create output file: " << filename << std::endl;
                return;
        }

        uint64_t size = end - start;
        fs.seekg(start);
        std::vector<char> buffer(size);
        fs.read(buffer.data(), size);

        out.write(buffer.data(), size);
        out.close();

        std::cout << "Recovered PNG: " << filename << " (" << size << " bytes)" << std::endl;
}
