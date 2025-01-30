#ifndef READ_PNG_FILE_H
#define READ_PNG_FILE_H

#include <stdlib.h>
#include <png.h>

void read_png_file(const char *filename, unsigned char **image_buffer, int *width, int *height, int *channels);

#endif