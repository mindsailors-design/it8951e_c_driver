#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "it8951.h"
#include "read_png_file.h"

int main(int argc, char *argv[])
{
        const char *filename = "sleeping_penguin.png";
        unsigned char *image_buffer;
        int width, height, channels;

        read_png_file(filename, &image_buffer, &width, &height, &channels);

        if (image_buffer) {
                printf("Loaded PNG: %dx%d, Channels: %d\n", width, height, channels);

                // Use the image buffer (e.g., pass it to another function)

                // Free buffer after use
                free(image_buffer);
        } else {
                printf("Failed to load PNG image\n");
        }

        bool init_result = IT8951Init();
        if (init_result)
        {
                printf("IT8951 initialized\n");
        }
        else
        {
                printf("IT8951 initialization failed\n");
                return 1;
        }

        IT8951GetVcom();

        // uint8_t imageBuffer[IMAGE_BUFFER_SIZE] = {0};
        // memset(imageBuffer, 0x69, IMAGE_BUFFER_SIZE);

        // IT8951LoadImgInfo loadImgInfo;
        // loadImgInfo.EndianType = IT8951_LDIMG_B_ENDIAN;
        // loadImgInfo.PixelFormat = IT8951_4BPP;
        // loadImgInfo.Rotate = IT8951_ROTATE_0;
        // loadImgInfo.SourceBufferAddr = (uint32_t*)imageBuffer;
        // loadImgInfo.TargetMemoryAddr = 0;

        // printf("SourceBufferAddr: 0x%08x\n", (uint32_t*)imageBuffer);
        // printf("ImageBuffer Size: %d\n", sizeof(imageBuffer));

        // IT8951LoadImageStart(&loadImgInfo);
        // LCDWaitForReady();
        // // TODO dodac funkcje do ladowania obrazu do bufora drivera przez spi
        // const int chunkSize = 1024;
        // int result = 0;
        // for (int i = 0; i < IMAGE_BUFFER_SIZE; i += chunkSize)
        // {
        // int size = (i + chunkSize > IMAGE_BUFFER_SIZE) ? IMAGE_BUFFER_SIZE - i : chunkSize;
        // LCDWaitForReady();
        // result = wiringPiSPIDataRW(SPI_CHAN, &imageBuffer[i], size);
        // if (result == -1)
        // {
        //         perror("SPI communication failed");
        // }
        // else
        // {
        //         printf("SPI result: %d\n", result);
        // }
        // }

        // IT8951LoadImageEnd();


        return 0;
}