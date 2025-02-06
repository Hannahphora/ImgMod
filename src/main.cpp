#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include "../include/utility.h"

#define NUM_CHNS 3

void _Invert(uint8_t* img, int w, int h);
void _Convolve(uint8_t* img, uint8_t* out, int w, int h, float* kernel, int kW, int kH);
void _RGBtoGrayscale(uint8_t* img, int w, int h);
void _CalcGaussianKernel(float* k, int w, int h, float sigma);
void _Diff(uint8_t* img1, uint8_t* img2, uint8_t* out, int w, int h);

void Invert(State& s);
void RGBtoGrayscale(State& s);
void ConvSharpen(State& s);
void ConvLaplacian(State& s);
void ConvGaussian(State& s);
void DiffOfGaussians(State& s);
void Save(State& s);
void Exit(State& s) {}

int main(int argc, char** argv) {
    
    // create state
    State s("res/brisket.jpg", "res/brisket_out.png");
    
    // load img
    s.img = stbi_load(s.inPath, &s.w, &s.h, &s.comp, NUM_CHNS);
    if (!s.img) {
        std::cerr << "Failed to load image.\n";
        return -1;
    }

    // init menu
    Menu::Opts mOpts[] = {
        FN_DEF(Invert, true)
        FN_DEF(RGBtoGrayscale, true)
        FN_DEF(ConvSharpen, true)
        FN_DEF(ConvGaussian, true)
        FN_DEF(ConvLaplacian, true)
        FN_DEF(DiffOfGaussians, true)
        FN_DEF(Save, false)
        FN_DEF(Exit, false)
    };
    Menu menu = Menu(mOpts, 8, s);
    menu.Run();

	return 0;
}

void Save(State& s) { stbi_write_png(s.outPath, s.w, s.h, NUM_CHNS, s.img, s.w * NUM_CHNS); }

void Invert(State& s) { _Invert(s.img, s.w, s.h); }
void _Invert(uint8_t* img, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < 3; c++) {
                img[NUM_CHNS * (y * w + x) + c] = 255 - img[NUM_CHNS * (y * w + x) + c];
            }
        }
    }
}

void RGBtoGrayscale(State& s) { _RGBtoGrayscale(s.img, s.w, s.h); }
void _RGBtoGrayscale(uint8_t* img, int w, int h) {
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

void ConvGaussian(State& s) {
    printf("|| Gaussian Convolution\n\n");

    const int kS = 15;
    float sigma = 1.f;
    printf("Kernel size: %d\n", kS);
    printf("Kernel weight: %f\n", sigma);
    printf("\nGenerating kernel...\n");
    float* k = new float[kS * kS];
    _CalcGaussianKernel(k, kS, kS, sigma);

    uint8_t* tempBuf = new uint8_t[s.w * s.h * NUM_CHNS];
    printf("Convolving image...\n\n");
    _Convolve(s.img, tempBuf, s.w, s.h, k, kS, kS);
    delete[] s.img, k;
    s.img = tempBuf;
}

void ConvLaplacian(State& s) {
    printf("|| Laplacian Convolution\n\n");
    float k[] = { -1, -1, -1, -1,  8, -1, -1, -1, -1 };
    uint8_t* tempBuf = new uint8_t[s.w * s.h * NUM_CHNS];
    printf("Convolving image...\n\n");
    _Convolve(s.img, tempBuf, s.w, s.h, k, 3, 3);
    delete[] s.img;
    s.img = tempBuf;
}

void ConvSharpen(State& s) {
    printf("|| Sharpen Convolution\n\n");
    float k[] = { 0, -1, 0, -1,  5, -1, 0, -1, 0 };
    uint8_t* tempBuf = new uint8_t[s.w * s.h * NUM_CHNS];
    printf("Convolving image...\n\n");
    _Convolve(s.img, tempBuf, s.w, s.h, k, 3, 3);
    delete[] s.img;
    s.img = tempBuf;
}

void DiffOfGaussians(State& s) {
    printf("|| Difference Of Gaussians\n\n");

    const int k1Size = 5, k2Size = 25;
    float k1Sigma = 0.3f, k2Sigma = 1.0f;
    printf("Kernel 1 size: %d\n", k1Size);
    printf("Kernel 1 weight: %.1f\n", k1Sigma);
    printf("Kernel 2 size: %d\n", k2Size);
    printf("Kernel 1 weight: %.1f\n", k2Sigma);

    printf("\nGenerating kernels...\n");
    float* k1 = new float[k1Size * k1Size];
    float* k2 = new float[k2Size * k2Size];
    _CalcGaussianKernel(k1, k1Size, k1Size, k1Sigma);
    _CalcGaussianKernel(k2, k2Size, k2Size, k2Sigma);

    printf("Convolving images...\n\n");
    uint8_t* temp1 = new uint8_t[s.w * s.h * NUM_CHNS];
    uint8_t* temp2 = new uint8_t[s.w * s.h * NUM_CHNS];
    _Convolve(s.img, temp1, s.w, s.h, k1, k1Size, k1Size);
    _Convolve(s.img, temp2, s.w, s.h, k2, k2Size, k2Size);

    printf("Calculating difference...\n\n");
    _Diff(temp1, temp2, s.img, s.w, s.h);
    delete[] temp1, temp2, k1, k2;
}

void _Diff(uint8_t* img1, uint8_t* img2, uint8_t* out, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < 3; c++) {
                out[NUM_CHNS * (y * w + x) + c] = (uint8_t)std::min(std::max(img1[NUM_CHNS * (y * w + x) + c] - img2[NUM_CHNS * (y * w + x) + c], 0), 255);
            }
        }
    }
}

void _Convolve(uint8_t* img, uint8_t* out, int w, int h, float* kernel, int kW, int kH) {
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
                        sum += img[NUM_CHNS * (ny * w + nx) + c] * kernel[(ky + dy) * kW + (kx + dx)];
                    }
                }
                // set val in output buf to sum, for current pixel and channel
                out[NUM_CHNS * (y * w + x) + c] = std::min(std::max((int)sum, 0), 255);
            }
        }
    }
}

void _CalcGaussianKernel(float* k, int w, int h, float sigma) {
    int dx = w / 2, dy = h / 2;
    float s = 2.f * sigma * sigma, sum = 0.f;
    // generate
    for (int y = -dy; y <= dy; y++) {
        for (int x = -dx; x <= dx; x++) {
            float r = sqrt(x * x + y * y);
            k[(y + dy) * w + (x + dx)] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += k[(y + dy) * w + (x + dx)];
        }
    }
    // normalise
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            k[y * w + x] /= sum;
        }
    }
}