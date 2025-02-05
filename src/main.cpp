#include <cstdlib>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

int main(int argC, char* argV[]) {



    uint8_t* img = (uint8_t*)new char[160 * 60 * 3];

    for (int y = 0; y < 60; y++) {
        for (int x = 0; x < 160; x++) {
            img[3 * (y * 160 + x)] = 128;
            img[3 * (y * 160 + x) + 1] = 128;
            img[3 * (y * 160 + x) + 2] = 128;
        }
    }

    // stbi_write_png(path, width, height, CHANNEL_NUM, pixels, width * CHANNEL_NUM);
    stbi_write_png("res/output/test.png", 160, 60, 3, img, 160 * 3);

	return 0;
}