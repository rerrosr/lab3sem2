#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef LAB3SEM2_FUNCTIONS_H
#define LAB3SEM2_FUNCTIONS_H

#pragma pack(push, 1)
typedef struct {
    unsigned char ID[2];
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;
} BMPHeader;

typedef struct {
    unsigned int size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int compimageSize;
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int colorsImportant;
} BMPInfoHeader;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} BMPColor;
#pragma pack(pop)

void convertToNegative(BMPInfoHeader infoHeader, BMPColor* pixels);
void convertToBlackAndWhite(BMPInfoHeader infoHeader, BMPColor* pixels);
char* inputStr();
double my_pow(double base, double exponent);
int median(unsigned char arr[], int n);
void medianFilter(BMPInfoHeader infoHeader, BMPColor* pixels, int filterSize);
void gammaCorrection(BMPColor *color, float gammaValue);
void menu(BMPInfoHeader infoHeader, BMPColor* pixels);
FILE* openAndReadBMPFile(const char* filename, BMPHeader* header, BMPInfoHeader* infoHeader, BMPColor** pixels);
void writeFile(const char* filename, BMPHeader header, BMPInfoHeader infoHeader, BMPColor* pixels, int pixelArraySize);
#endif LAB3SEM2_FUNCTIONS_H
