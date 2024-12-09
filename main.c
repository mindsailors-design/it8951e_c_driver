#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <stdbool.h>

#define WIDTH 100
#define HEIGHT 80
#define BPP 4
#define IMAGE_BUFFER_SIZE (WIDTH * HEIGHT * BPP / 8)

#define SPI_CHAN 0
#define SPI_SPEED 500000
#define BUFFER_SIZE 4
    // const uint8_t BUFFER_SIZE = 4;

// #define CS 10
#define HST_RDY 5
#define RST 0

//Built in I80 Command Code
#define IT8951_TCON_SYS_RUN      0x0001
#define IT8951_TCON_STANDBY      0x0002
#define IT8951_TCON_SLEEP        0x0003
#define IT8951_TCON_REG_RD       0x0010
#define IT8951_TCON_REG_WR       0x0011
#define IT8951_TCON_MEM_BST_RD_T 0x0012
#define IT8951_TCON_MEM_BST_RD_S 0x0013
#define IT8951_TCON_MEM_BST_WR   0x0014
#define IT8951_TCON_MEM_BST_END  0x0015
#define IT8951_TCON_LD_IMG       0x0020
#define IT8951_TCON_LD_IMG_AREA  0x0021
#define IT8951_TCON_LD_IMG_END   0x0022

//I80 User defined command code
#define USDEF_I80_CMD_DPY_AREA     0x0034
#define USDEF_I80_CMD_GET_DEV_INFO 0x0302
#define USDEF_I80_CMD_DPY_BUF_AREA 0x0037
#define USDEF_I80_CMD_VCOM		   0x0039

// Rotate mode
#define IT8951_ROTATE_0     0
#define IT8951_ROTATE_90    1
#define IT8951_ROTATE_180   2
#define IT8951_ROTATE_270   3


// Pixel mode (Bit per Pixel)
#define IT8951_2BPP   0
#define IT8951_3BPP   1
#define IT8951_4BPP   2
#define IT8951_8BPP   3

//Endian Type
#define IT8951_LDIMG_L_ENDIAN   0
#define IT8951_LDIMG_B_ENDIAN   1

typedef struct IT8951LoadImgInfo
{
    uint16_t EndianType;
    uint16_t PixelFormat;
    uint16_t Rotate;
    uint32_t* SourceBufferAddr;
    uint32_t TargetMemoryAddr;
} IT8951LoadImgInfo;

typedef struct IT8951AreaImgInfo
{
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
} IT8951AreaImgInfo;

typedef struct IT8951DevInfo
{
    uint16_t PanelWidth;
    uint16_t PanelHeight;
    uint16_t ImgBugAddrL;
    uint16_t ImgBugAddrH;
    uint16_t FWVersion;
    uint16_t LUTVersion;
} IT8951DevInfo;

void LCDWaitForReady()
{
	bool ulData = digitalRead(HST_RDY);
	while(ulData == 0)
	{
		ulData = digitalRead(HST_RDY);
        printf("HST_RDY: %d\n", digitalRead(HST_RDY));
	}
}

void sendAndReceiveSPI(const unsigned char *dataToSend, unsigned char *dataReceived, int length)
{
    memcpy(dataReceived, dataToSend, length);
    int result = wiringPiSPIDataRW(SPI_CHAN, dataReceived, length);

    if (result == -1)
    {
        perror("SPI communication failed");
    }
    
}

void SPIWriteCommand(uint16_t command)
{
    unsigned char data_to_send[4];
    data_to_send[0] = 0x60;
    data_to_send[1] = 0x00;

    data_to_send[2] = command >> 8;
    data_to_send[3] = command;

    // printf("0x%02x\n", data_to_send[0]);
    // printf("0x%02x\n", data_to_send[1]);
    // printf("0x%02x\n", data_to_send[2]);
    // printf("0x%02x\n", data_to_send[3]);

    LCDWaitForReady();

    int result = wiringPiSPIDataRW(SPI_CHAN, data_to_send, sizeof(data_to_send));
    if (result == -1)
    {
        perror("SPI communication failed");
    }
    else
    {
        printf("SPI result: %d\n", result);
    }
}

void SPIWriteData(uint16_t data)
{
    unsigned char data_to_send[4];
    data_to_send[0] = 0x00;
    data_to_send[1] = 0x00;
    data_to_send[2] = data >> 8;
    data_to_send[3] = data;

    // printf("0x%02x\n", data_to_send[0]);
    // printf("0x%02x\n", data_to_send[1]);
    // printf("0x%02x\n", data_to_send[2]);
    // printf("0x%02x\n", data_to_send[3]);

    LCDWaitForReady();

    int result = wiringPiSPIDataRW(SPI_CHAN, data_to_send, sizeof(data_to_send));
    if (result == -1)
    {
        perror("SPI communication failed");
    }
    else
    {
        printf("SPI result: %d\n", result);
    }   
}

void SPIReadData(uint8_t* data)
{
    // read_preamble = 0x10, read_preamble = 0x00, dummy = 0x00, dummy = 0x00
    data[0] = 0x10;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;

    LCDWaitForReady();

    int result = wiringPiSPIDataRW(SPI_CHAN, data, BUFFER_SIZE);
    if (result == -1)
    {
        perror("SPI communication failed");
    }
    else
    {
        printf("SPI result: %d\n", result);
    } 
    
    printf("Data received: ");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("0x%02x ", data[i]); 
    }
    printf("\n");
}
// write multi arg
void IT8951_WriteMultiArg(uint16_t command, uint16_t* arg_buff, uint16_t arg_num)
{
    // send command
    SPIWriteCommand(command);
    // send data
    for (uint16_t i = 0; i < arg_num; i++)
    {
        SPIWriteData(arg_buff[i]);
    }
}

// readreg
void IT8951ReadRegister(uint16_t address)
{
    SPIWriteCommand(IT8951_TCON_REG_RD);
    SPIWriteData(address);

    uint8_t data[BUFFER_SIZE] = {0};
    SPIReadData(data);
    // printf("%p\n", (void*)data);
    
    printf("First byte: 0x%02x\n", data[2]);
    printf("Second byte: 0x%02x\n", data[3]);
}
// writereg
void IT8951WriteRegister(uint16_t address, uint16_t value)
{
    SPIWriteCommand(IT8951_TCON_REG_WR);
    SPIWriteData(address);
    SPIWriteData(value);
}

// Host Commands
// reset
void IT8951Reset()
{
    digitalWrite(RST, HIGH);
    delay(200);
    digitalWrite(RST, LOW);
    delay(200);
    digitalWrite(RST, HIGH);
}
// run
void IT8951SystemRun()
{
    SPIWriteCommand(IT8951_TCON_SYS_RUN);
}
// standby
void IT8951SystemStandby()
{
    SPIWriteCommand(IT8951_TCON_STANDBY);
}
// sleep 
void IT8951SystemSleep()
{
    SPIWriteCommand(IT8951_TCON_SLEEP);
}
// system info
void IT8951SystemInfo()
{
    SPIWriteCommand(USDEF_I80_CMD_GET_DEV_INFO);
}
// set vcom
void IT8951SetVcom(uint16_t vcom)
{
    SPIWriteCommand(USDEF_I80_CMD_VCOM);
    SPIWriteData(0x0001);
    printf("VCOM SET: %d\n", vcom);
    SPIWriteData(vcom);
}
// get vcom
uint16_t IT8951GetVcom()
{
    // BUG: vcom register must be read twice to get correct value
    // on the first read it returns previous value
    uint8_t buffer[BUFFER_SIZE] = {0};
    uint16_t Vcom = 0;
    SPIWriteCommand(USDEF_I80_CMD_VCOM);
    SPIWriteData(0x0000);
    SPIReadData(buffer);
    SPIReadData(buffer);
    Vcom = ((uint16_t)buffer[2]<<8) | buffer[3];
    // dodac jakas konwersje buffer na vcom
    printf("VCOM: %d\n", Vcom);
    return Vcom;
}
// load image start
void IT8951LoadImageStart(IT8951LoadImgInfo* LoadImageInfo)
{
    uint16_t Args;
    Args = (\
        LoadImageInfo->EndianType<<8 | \
        LoadImageInfo->PixelFormat<<4 | \
        LoadImageInfo->Rotate \
    );
    printf("Args: 0x%04x\n", Args);
    printf("EndianType: 0x%04x\n", LoadImageInfo->EndianType);
    printf("PixelFormat: 0x%04x\n", LoadImageInfo->PixelFormat);
    printf("Rotate: 0x%04x\n", LoadImageInfo->Rotate);
    printf("SourceBufferAddr: 0x%08x\n", LoadImageInfo->SourceBufferAddr);
    printf("TargetMemoryAddr: 0x%08x\n", LoadImageInfo->TargetMemoryAddr);

    SPIWriteCommand(IT8951_TCON_LD_IMG);
    SPIWriteData(Args);
}
// load image area start
void IT8951LoadImageAreaStart(IT8951LoadImgInfo* LoadImageInfo, IT8951AreaImgInfo* AreaImgInfo)
{
    uint16_t Args[5];
    Args[0] = (\
        LoadImageInfo->EndianType<<8 | \
        LoadImageInfo->PixelFormat<<4 | \
        LoadImageInfo->Rotate \
    );
    Args[1] = AreaImgInfo->X;
    Args[2] = AreaImgInfo->Y;
    Args[3] = AreaImgInfo->Width;
    Args[4] = AreaImgInfo->Height;

    IT8951_WriteMultiArg(IT8951_TCON_LD_IMG_AREA, Args, 5);
}
// load image end
void IT8951LoadImageEnd()
{
    SPIWriteCommand(IT8951_TCON_LD_IMG_END);
}

bool IT8951Init(void)
{
    int result = wiringPiSetup();
    printf("%d\n", result);
    if (result == -1)
    {
        printf("WiringPi setup failed\n");
        return 1;
    }

    result = wiringPiSPISetup(SPI_CHAN, SPI_SPEED);
    printf("%d\n", result);
    if (result == -1)
    {
        printf("SPI setup failed\n");
        return 1;
    }

    pinMode(HST_RDY, INPUT);
    pinMode(RST, OUTPUT);

    digitalWrite(RST, HIGH);

    return 1;
}
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