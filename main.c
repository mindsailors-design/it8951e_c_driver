#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "it8951.h"
#include "read_png_file.h"
#include "stb_image_resize.h"

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
	} else {
		printf("Failed to load PNG image\n");
	}

	printf("Original: %dx%d\n", width, height);

	// target resolution
	int new_width = 1448, new_height = 1072;

	// resize
	uint8_t *scaled = resize_image(image, width, height, new_width, new_height);
	if (!scaled)
	{
		free(image);
		return -1;
	}

	printf("Resized: %dx%d\n", new_width, new_height);

	GPIO_Init();

	Dev_Info = IT8951Init(VCOM);

	Target_Memory_Address = Dev_Info.MemoryAddrL | (Dev_Info.MemoryAddrH << 16);

	IT8951_ClearRefresh(Dev_Info, Target_Memory_Address, DSP_MD_INIT);

	IT8951DisplayImage_1bpp_Refresh(scaled, 0, 0, new_width, new_height, DSP_MD_GC16, Target_Memory_Address, true);


	// Free buffer after use
	free(scaled);
	
	return 0;
}