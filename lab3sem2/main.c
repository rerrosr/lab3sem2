#include <string.h>
#include "functions.h"

int main() {
    printf("Enter file name:");
    char* filename = inputStr();
    strcat(filename, ".bmp");
    BMPHeader header;
    BMPInfoHeader infoHeader;
    BMPColor* pixels;
    FILE* file = openAndReadBMPFile(filename, &header, &infoHeader, &pixels);

    if (file == NULL) {
        return 1;
    }

    int rowSize = (infoHeader.bitsPerPixel * infoHeader.width + 31) / 32 * 4;
    int pixelArraySize = rowSize * infoHeader.height;

    menu(infoHeader, pixels);

    writeFile("output.bmp", header, infoHeader, pixels, pixelArraySize);

    free(pixels);
    return 0;
}