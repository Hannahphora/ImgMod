#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#define NUM_CHNS 3

void Invert(uint8_t* img, int w, int h);
void Convolve(uint8_t* in, uint8_t* out, int w, int h, float* kernel, int kW, int kH);
void RGBtoGrayscale(uint8_t* img, int w, int h);
void CalcGaussianKernel(float* k, int w, int h, float sigma);

int main(int argc, char** argv) {
    
    //if (argc < 3) {
    //    std::cerr << "Usage: " << argv[0] << " <input> <output>\n";
    //    return -1;
    //}
    //const char* inPath = argv[1];
    //const char* outPath = argv[2];

    const char* inPath = "res/space.jpg";
    const char* outPath = "res/space_out.png";

    int w, h, comp;
    uint8_t* img = stbi_load(inPath, &w, &h, &comp, NUM_CHNS);
    if (!img) {
        std::cerr << "Failed to load image.\n";
        return -1;
    }

    // grayscale conversion
    RGBtoGrayscale(img, w, h);

    { // gaussian convolve
        const int kS = 20;
        float k[kS * kS];
        CalcGaussianKernel(k, kS, kS, 1.f);
        uint8_t* tempBuf = new uint8_t[w * h * NUM_CHNS];
        Convolve(img, tempBuf, w, h, k, kS, kS);
        delete[] img;
        img = tempBuf;
    }

    { // laplacian convolve
        float k[] = { -1, -1, -1, -1,  8, -1, -1, -1, -1 };
        uint8_t* tempBuf = new uint8_t[w * h * NUM_CHNS];
        Convolve(img, tempBuf, w, h, k, 3, 3);
        delete[] img;
        img = tempBuf;
    }

    // Invert(img, w, h);

    // write output & cleanup
    stbi_write_png(outPath, w, h, NUM_CHNS, img, w * NUM_CHNS);
    stbi_image_free(img);
	return 0;
}

void Invert(uint8_t* img, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < 3; c++) {
                img[NUM_CHNS * (y * w + x) + c] = 255 - img[NUM_CHNS * (y * w + x) + c];
            }
        }
    }
}

void RGBtoGrayscale(uint8_t* img, int w, int h) {
    // iterate pxls in img
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // grayscale conversion values
            float rMod = 0.299, gMod = 0.587, bMod = 0.114;

            // calculate vals for each channel
            float rt = ((float)img[NUM_CHNS * (y * w + x)] / 255.f) * rMod;
            float gt = ((float)img[NUM_CHNS * (y * w + x) + 1] / 255.f) * gMod;
            float bt = ((float)img[NUM_CHNS * (y * w + x) + 2] / 255.f) * bMod;

            // sum calculated values and write to current pixel
            uint8_t fVal = (uint8_t)std::min(std::max((int)((rt + gt + bt) * 255), 0), 255);
            for (int c = 0; c < NUM_CHNS; c++) img[NUM_CHNS * (y * w + x) + c] = fVal;
        }
    }
}

void Convolve(uint8_t* in, uint8_t* out, int w, int h, float* kernel, int kW, int kH) {
    int dx = kW / 2, dy = kH / 2;
    // iterate pxls in img
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // iterate channels
            for (int c = 0; c < NUM_CHNS; c++) {
                float sum = 0.f;
                // iterate over kernel, convolving kernel with img into sum
                for (int ky = -dy; ky <= dy; ky++) {
                    for (int kx = -dx; kx <= dx; kx++) {
                        int ny = std::min(std::max(y + ky, 0), h - 1);
                        int nx = std::min(std::max(x + kx, 0), w - 1);
                        sum += in[NUM_CHNS * (ny * w + nx) + c] * kernel[(ky + dy) * kW + (kx + dx)];
                    }
                }
                // set val in output buf to sum, for current pixel and channel
                out[NUM_CHNS * (y * w + x) + c] = std::min(std::max((int)sum, 0), 255);
            }
        }
    }
}

void CalcGaussianKernel(float* k, int w, int h, float sigma) {
    int dx = w / 2, dy = h / 2;
    float s = 2.f * sigma * sigma, sum = 0.f;
    // generate kernel
    for (int y = -dy; y <= dy; y++) {
        for (int x = -dx; x <= dx; x++) {
            float r = sqrt(x * x + y * y);
            k[(y + dy) * w + (x + dx)] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += k[(y + dy) * w + (x + dx)];
        }
    }
    // normalise kernel
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            k[y * w + x] /= sum;
        }
    }
}