
#pragma once

#include <string>




void saveToBitmap(std::string filename, int width, int height, const unsigned char* imageData);
void loadFromBitmap(std::string filename, int* width, int* height, unsigned char **imageData);
void luminanceToRGB(const unsigned char* luminanceData, unsigned char* rgbData, int width, int height);
void RGBToLuminance(const unsigned char* rgbData, unsigned char* luminanceData, int width, int height);
void RGBToRGBA(const unsigned char* rgbData, unsigned char* rgbaData, int width, int height);
void RGBAToRGB(const unsigned char* rgbaData, unsigned char* rgbData, int width, int height);
