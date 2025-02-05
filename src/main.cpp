#include <cstdlib>
#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

int main(int argC, char* argV[]) {
    
    // TODO: implement taking input file from cmdline arguments
    // if (argC <= 1) std::cout << "Error: No input image specified\n";

    const int CHN = 3;
    const char* inPath = "res/input/test.jpg";
    const char* outPath = "res/output/test.png";

    int w, h, comp;
    uint8_t* img = stbi_load(inPath, &w, &h, &comp, CHN);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // convert values
            int r = img[CHN * (y * w + x)];
            int g = img[CHN * (y * w + x) + 1];
            int b = img[CHN * (y * w + x) + 2];



            // convert values back
            img[CHN * (y * w + x)] = (uint8_t)r;
            img[CHN * (y * w + x) + 1] = (uint8_t)g;
            img[CHN * (y * w + x) + 2] = (uint8_t)b;
        }
    }

    stbi_write_png(outPath, w, h, CHN, img, w * CHN);
	return 0;
}