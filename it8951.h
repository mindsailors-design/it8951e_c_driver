#ifndef IT8951_H
#define IT8951_H

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

#define CS 10
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
    uint16_t MemoryAddrL;
    uint16_t MemoryAddrH;
    uint16_t FWVersion;
    uint16_t LUTVersion;
} IT8951DevInfo;

bool IT8951Init(void);

void LCDWaitForReady();

void sendAndReceiveSPI(const unsigned char *dataToSend, unsigned char *dataReceived, int length);
void IT8951_WriteCommand(uint16_t command);
void IT8951_WriteData(uint16_t data);
void IT8951_ReadData(uint8_t* data);

void IT8951_WriteMultiArg(uint16_t command, uint16_t* arg_buff, uint16_t arg_num);
void IT8951ReadRegister(uint16_t address);
void IT8951WriteRegister(uint16_t address, uint16_t value);

void IT8951Reset();
void IT8951SystemRun();
void IT8951SystemStandby();
void IT8951SystemSleep();
void IT8951GetSystemInfo();

void IT8951SetVcom(uint16_t vcom);
uint16_t IT8951GetVcom();

void IT8951LoadImageStart(IT8951LoadImgInfo* LoadImageInfo);
void IT8951LoadImageAreaStart(IT8951LoadImgInfo* LoadImageInfo, IT8951AreaImgInfo* AreaImgInfo);
void IT8951LoadImageEnd();

bool IT8951DisplayImage();
#endif