#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

uint8_t* Convolve(uint8_t* img, int w, int h, int chn, float* kernel, int kW, int kH);

int main(int argc, char** argv) {
    
    //if (argc < 3) {
    //    std::cerr << "Usage: " << argv[0] << " <input> <output>\n";
    //    return -1;
    //}
    //const char* inPath = argv[1];
    //const char* outPath = argv[2];

    const char* inPath = "res/miku_original.jpg";
    const char* outPath = "res/miku_out.png";

    const int chn = 3;
    int w, h, comp;
    uint8_t* img = stbi_load(inPath, &w, &h, &comp, chn);
    if (!img) {
        std::cerr << "Failed to load image.\n";
        return -1;
    }

    float laplacian[] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };

    float gaussian3[] = {
        1 / 16.f, 1 /  8.f, 1 / 16.f, 
        1 /  8.f, 1 /  4.f, 1 /  8.f, 
        1 / 16.f, 1 /  8.f, 1 / 16.f,
    };

    auto output = Convolve(img, w, h, chn, laplacian, 3, 3);

    // write output buffer
    stbi_write_png(outPath, w, h, chn, output, w * chn);
    
    // cleanup
    stbi_image_free(img);
    delete[] output;

	return 0;
}

uint8_t* Convolve(uint8_t* img, int w, int h, int chn, float* kernel, int kW, int kH) {
    
    uint8_t* output = new uint8_t[w * h * chn];
    int dx = kW / 2, dy = kH / 2;

    // iterate pxls in img
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            // iterate channels
            for (int c = 0; c < chn; c++) {

                float sum = 0.f;

                // iterate over kernel, convolving kernel with img into sum
                for (int ky = -dy; ky <= dy; ky++) {
                    for (int kx = -dx; kx <= dx; kx++) {

                        int ny = std::min(std::max(y + ky, 0), h - 1);
                        int nx = std::min(std::max(x + kx, 0), w - 1);

                        sum += img[chn * (ny * w + nx) + c] * kernel[(ky + dy) * kW + (kx + dx)];
                    }
                }

                // set val in output buf to sum, for current pixel and channel
                output[chn * (y * w + x) + c] = std::min(std::max((int)sum, 0), 255);
            }
        }
    }

    return output;
}