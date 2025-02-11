#ifndef READ_PNG_FILE_H
#define READ_PNG_FILE_H

#include <stdlib.h>
#include <stdint.h>
#include <png.h>
#include "stb_image_resize.h"

uint8_t* read_png_file(const char *filename, int *width, int *height, int* bit_depth);
uint8_t* resize_image(uint8_t* image, uint16_t src_w, uint16_t src_h, uint16_t dest_w, uint16_t dest_h);

#endif