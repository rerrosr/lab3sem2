#include "functions.h"

char* inputStr() {
    char* str = NULL;
    int pos = 0;
    int len = 0;
    char c = getchar();

    while (c != '\n') {
        if (pos == len) {
            len += 10;
            str = (char*) realloc(str, len * sizeof(char));
        }

        str[pos] = c;
        pos++;
        c = getchar();
    }

    if (pos == len) {
        len++;
        str = (char*) realloc(str, len * sizeof(char));
    }

    str[pos] = '\0';
    return str;
}

void writeFile(const char* filename, BMPHeader header, BMPInfoHeader infoHeader, BMPColor* pixels, int pixelArraySize) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error: Failed to create output file\n");
        exit(1);
    }

    fwrite(&header,sizeof(header),1,file);
    fwrite(&infoHeader,sizeof(infoHeader),1,file);
    fseek(file,header.offset-sizeof(header)-sizeof(infoHeader),SEEK_CUR);
    fwrite(pixels,pixelArraySize,1,file);

    fclose(file);
}

FILE* openAndReadBMPFile(const char* filename, BMPHeader* header, BMPInfoHeader* infoHeader, BMPColor** pixels) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: File not found\n");
        return NULL;
    }

    fread(header, sizeof(BMPHeader), 1, file);

    if (header->ID[0] != 'B' || header->ID[1] != 'M') {
        printf("Error: Invalid BMP file format\n");
        fclose(file);
        exit(1);
    }

    fread(infoHeader, sizeof(BMPInfoHeader), 1, file);

    if (infoHeader->bitsPerPixel != 24) {
        printf("Error: Only supports 24 bit BMP images\n");
        fclose(file);
        return NULL;
    }

    fseek(file, header->offset - sizeof(BMPHeader) - sizeof(BMPInfoHeader), SEEK_CUR);

    int rowSize = (infoHeader->bitsPerPixel * infoHeader->width + 31) / 32 * 4;
    int pixelArraySize = rowSize * infoHeader->height;
    *pixels = malloc(pixelArraySize);

    fread(*pixels, pixelArraySize, 1, file);

    fclose(file);

    return file;
}



void gammaCorrection(BMPColor *color, float gammaValue) {
    color->red   = (unsigned char)(pow(color->red   / 255.0f, gammaValue) * 255.0f);
    color->green = (unsigned char)(pow(color->green / 255.0f, gammaValue) * 255.0f);
    color->blue  = (unsigned char)(pow(color->blue  / 255.0f, gammaValue) * 255.0f);
}

void convertToNegative(BMPInfoHeader infoHeader, BMPColor* pixels) {
    for (int i = 0; i < infoHeader.height; i++) {
        for (int j = 0; j < infoHeader.width; j++) {
            int index = i * infoHeader.width + j;

            unsigned char red = pixels[index].red;  // получение значений компонент цвета (красный, зеленый, синий)
            unsigned char green = pixels[index].green;
            unsigned char blue = pixels[index].blue;

            pixels[index].red = 255 - red;  // инвертирование значений компонент цвета
            pixels[index].green = 255 - green;
            pixels[index].blue = 255 - blue;
        }
    }
}

void convertToBlackAndWhite(BMPInfoHeader infoHeader, BMPColor* pixels) {
    for (int i = 0; i < infoHeader.height; i++) {
        for (int j = 0; j < infoHeader.width; j++) {
            int index = i * infoHeader.width + j;

            unsigned char red = pixels[index].red; // получение значений компонент цвета (красный, зеленый, синий)
            unsigned char green = pixels[index].green;
            unsigned char blue = pixels[index].blue;
            unsigned char gray = (red + green + blue) / 3;   // вычисление среднего значения яркости
            pixels[index].red = gray; // Установка одинаковых значений компонент цвета для черно-белого изображения
            pixels[index].green = gray;
            pixels[index].blue = gray;
        }
    }
}

int median(unsigned char arr[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[i]) {
                unsigned char temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    if (n % 2 == 0) {
        return (arr[n / 2] + arr[n / 2 - 1]) / 2;
    } else {
        return arr[n / 2];
    }
}

void medianFilter(BMPInfoHeader infoHeader, BMPColor* pixels, int filterSize) {
    int radius = filterSize / 2;
    int rowSize = (infoHeader.bitsPerPixel * infoHeader.width + 31) / 32 * 4;
    BMPColor* oldPixels = malloc(rowSize * infoHeader.height);
    memcpy(oldPixels, pixels, rowSize * infoHeader.height);

    for (int y = radius; y < infoHeader.height - radius; y++) {
        for (int x = radius; x < infoHeader.width - radius; x++) {
            unsigned char red[filterSize * filterSize];
            unsigned char green[filterSize * filterSize];
            unsigned char blue[filterSize * filterSize];
            int idx = 0;
            for (int j = y - radius; j <= y + radius; j++) {
                for (int i = x - radius; i <= x + radius; i++) {
                    red[idx] = oldPixels[j * infoHeader.width + i].red;
                    green[idx] = oldPixels[j * infoHeader.width + i].green;
                    blue[idx] = oldPixels[j * infoHeader.width + i].blue;
                    idx++;
                }
            }
            pixels[y * infoHeader.width + x].red = (unsigned char)median(red, filterSize * filterSize);
            pixels[y * infoHeader.width + x].green = (unsigned char)median(green, filterSize * filterSize);
            pixels[y * infoHeader.width + x].blue = (unsigned char)median(blue, filterSize * filterSize);
        }
    }

    free(oldPixels);
}


void menu(BMPInfoHeader infoHeader, BMPColor* pixels)
{
    int choice;
    int filterSize;
    printf("Enter the operation :\n");
    printf("1. Negative\n");
    printf("2. Grayscale\n");
    printf("3. Median filter\n");
    printf("4. Gamma correction\n");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            convertToNegative(infoHeader, pixels);
            break;
        case 2:
            convertToBlackAndWhite(infoHeader, pixels);
            break;
        case 3:
            printf("Enter size of median filter (odd number): ");
            scanf("%d", &filterSize);
            medianFilter(infoHeader, pixels, filterSize);

            break;
        case 4: {
            float gammaValue;
            printf("Enter gamma value:\n");
            scanf("%f", &gammaValue);

            for (int i = 0; i < infoHeader.width * infoHeader.height; i++) {
                gammaCorrection(&pixels[i], gammaValue);
            }
            break;
        }
        default:
            printf("Incorrect input!\n");
            break;
    }
}


