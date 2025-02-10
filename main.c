#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "it8951.h"
#include "read_png_file.h"

uint16_t VCOM = 2140;

IT8951DevInfo Dev_Info;
uint16_t Panel_Width;
uint16_t Panel_Height;
uint32_t Target_Memory_Address;

uint8_t epd_mode = 0;

int main(int argc, char *argv[])
{
	int width, height, bit_depth;
	// unsigned char *image_buffer;
	// const char *filename = "sleeping_penguin.png";
	uint8_t *image = read_png_file("sleeping_penguin.png", &width, &height, &bit_depth);

	// read_png_file(filename, &image_buffer, &width, &height, &channels);

	if (image) {
		printf("Loaded PNG: %dx%d, Bit depth: %d\n", width, height, bit_depth);

		// Use the image buffer (e.g., pass it to another function)

	} else {
		printf("Failed to load PNG image\n");
	}

	GPIO_Init();

	Dev_Info = IT8951Init(VCOM);

	IT8951_ClearRefresh(Dev_Info, Target_Memory_Address, DSP_MD_INIT);

	IT8951DisplayImage_1bpp_Refresh(image, 0, 0, width, height, DSP_MD_GC16, Target_Memory_Address, true);


	// Free buffer after use
	free(image);
	
	return 0;
}