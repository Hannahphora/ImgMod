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

void Invert(State& s);
void RGBtoGrayscale(State& s);
void Save(State& s);
void ConvLaplacian(State& s);
void ConvGaussian(State& s);
void Exit(State& s) {}

int main(int argc, char** argv) {
    
    //if (argc < 3) {
    //    std::cerr << "Usage: " << argv[0] << " <input> <output>\n";
    //    return -1;
    //}
    //const char* inPath = argv[1];
    //const char* outPath = argv[2];

    // create state
    State s("res/space.jpg", "res/space_out.png");
    
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
        FN_DEF(ConvGaussian, true)
        FN_DEF(ConvLaplacian, true)
        FN_DEF(Save, false)
        FN_DEF(Exit, false)
    };
    Menu menu = Menu(mOpts, 6, s);
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
    printf("Kernel weight: %f\n\n", sigma);
    system("pause");
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