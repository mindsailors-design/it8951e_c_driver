#include "read_png_file.h"

void read_png_file(const char *filename, unsigned char **image_buffer, int *width, int *height, int *channels) {
        FILE *fp = fopen(filename, "rb");
        if (!fp)
        {
                perror("File opening failed");
                return;
        }

        // Validate PNG file signature
        unsigned char header[8];
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
        {
                printf("Not a valid PNG file\n");
        }
        
        // Create PNG structures
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png)
        {
                fclose(fp);
                return;
        }

        png_infop info = png_create_info_struct(png);
        if(!info)
        {
                png_destroy_read_struct(&png, NULL, NULL);
                fclose(fp);
                return;
        }

        // Error handling
        if (setjmp(png_jmpbuf(png)))
        {
                png_destroy_read_struct(&png, &info, NULL);
                fclose(fp);
                return;
        }
        
        // Initialize I/O and read PNG info
        png_init_io(png, fp);
        png_set_sig_bytes(png, 8);
        png_read_info(png, info);

        *width = png_get_image_width(png, info);
        *height = png_get_image_height(png, info);
        png_byte color_type = png_get_color_type(png, info);
        png_byte bit_depth = png_get_bit_depth(png, info);

        if (bit_depth == 16) png_set_strip_16(png);
        if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
        if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);
        
        png_read_update_info(png, info);

        // Get the number of channels (RBG = 3, RGBA = 4)
        *channels = png_get_channels(png, info);

        // Allocate memory for image buffer
        int rowbytes = png_get_rowbytes(png, info);
        *image_buffer = (unsigned char*)malloc(rowbytes * (*height));

        // Allocate row pointers
        png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * (*height));
        for (int y = 0; y < *height; y++) {
                row_pointers[y] = *image_buffer + y * rowbytes;
        }

        // Read image into buffer
        png_read_image(png, row_pointers);

        // Clean up
        free(row_pointers);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
}