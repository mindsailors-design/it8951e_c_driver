#include "it8951.h"

int main(void) {
    printf("RaspberryPi SPI test\n");

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
    
    IT8951Reset();
    IT8951SystemRun();

    uint8_t imageBuffer[IMAGE_BUFFER_SIZE] = {0};
    memset(imageBuffer, 0x69, IMAGE_BUFFER_SIZE);

    IT8951LoadImgInfo loadImgInfo;
    loadImgInfo.EndianType = IT8951_LDIMG_B_ENDIAN;
    loadImgInfo.PixelFormat = IT8951_4BPP;
    loadImgInfo.Rotate = IT8951_ROTATE_0;
    loadImgInfo.SourceBufferAddr = (uint32_t*)imageBuffer;
    loadImgInfo.TargetMemoryAddr = 0;

    printf("SourceBufferAddr: 0x%08x\n", (uint32_t*)imageBuffer);
    printf("ImageBuffer Size: %d\n", sizeof(imageBuffer));

    IT8951LoadImageStart(&loadImgInfo);
    LCDWaitForReady();
    // TODO dodac funkcje do ladowania obrazu do bufora drivera przez spi
    const int chunkSize = 1024;
    int result = 0;
    for (int i = 0; i < IMAGE_BUFFER_SIZE; i += chunkSize)
    {
        int size = (i + chunkSize > IMAGE_BUFFER_SIZE) ? IMAGE_BUFFER_SIZE - i : chunkSize;
        LCDWaitForReady();
        result = wiringPiSPIDataRW(SPI_CHAN, &imageBuffer[i], size);
        if (result == -1)
        {
            perror("SPI communication failed");
        }
        else
        {
            printf("SPI result: %d\n", result);
        }
    }
    
    IT8951LoadImageEnd();
 
    return 0;
}