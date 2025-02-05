#include <cstdlib>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

void Convolve();

int main(int argC, char* argV[]) {
    
    // TODO: implement taking input img path from cmdline arguments
    // if (argC <= 1) std::cout << "Error: No input image specified\n";

    const int CHN = 3;
    const char* inPath = "res/input/test.jpg";
    const char* outPath = "res/output/test.png";

    int w, h, comp;
    uint8_t* img = stbi_load(inPath, &w, &h, &comp, CHN);

    // FORMULA FOR FLAT 2D ARRAY ACCESS:
    // array[y * width + x]

    const int kW = 3, kH = 3;
    const int kernel[] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };

    uint8_t* window[kW * kH];
    int dx = kW / 2, dy = kH / 2;

    for (int y = -dy; y <= dy; y++) {
        for (int x = -dx; x <= dx; x++) {
            window[y * kW + x] = img;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            img[CHN * (y * w + x)];
            img[CHN * (y * w + x) + 1];
            img[CHN * (y * w + x) + 2];


        }
    }

    stbi_write_png(outPath, w, h, CHN, img, w * CHN);
    stbi_image_free(img);
	return 0;
}