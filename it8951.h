#ifndef IT8951_H
#define IT8951_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

/*-----------------------------------------------------------------------
IT8951 Registers defines
------------------------------------------------------------------------*/
//Register Base Address
#define DISPLAY_REG_BASE 0x1000               //Register RW access

//Base Address of Basic LUT Registers
#define LUT0EWHR  (DISPLAY_REG_BASE + 0x00)   //LUT0 Engine Width Height Reg
#define LUT0XYR   (DISPLAY_REG_BASE + 0x40)   //LUT0 XY Reg
#define LUT0BADDR (DISPLAY_REG_BASE + 0x80)   //LUT0 Base Address Reg
#define LUT0MFN   (DISPLAY_REG_BASE + 0xC0)   //LUT0 Mode and Frame number Reg
#define LUT01AF   (DISPLAY_REG_BASE + 0x114)  //LUT0 and LUT1 Active Flag Reg

//Update Parameter Setting Register
#define UP0SR     (DISPLAY_REG_BASE + 0x134)  //Update Parameter0 Setting Reg
#define UP1SR     (DISPLAY_REG_BASE + 0x138)  //Update Parameter1 Setting Reg
#define LUT0ABFRV (DISPLAY_REG_BASE + 0x13C)  //LUT0 Alpha blend and Fill rectangle Value
#define UPBBADDR  (DISPLAY_REG_BASE + 0x17C)  //Update Buffer Base Address
#define LUT0IMXY  (DISPLAY_REG_BASE + 0x180)  //LUT0 Image buffer X/Y offset Reg
#define LUTAFSR   (DISPLAY_REG_BASE + 0x224)  //LUT Status Reg (status of All LUT Engines)
#define BGVR      (DISPLAY_REG_BASE + 0x250)  //Bitmap (1bpp) image color table

//System Registers
#define SYS_REG_BASE 0x0000

//Address of System Registers
#define I80CPCR (SYS_REG_BASE + 0x04)

//Endian Type
#define IT8951_LDIMG_L_ENDIAN   0
#define IT8951_LDIMG_B_ENDIAN   1

//Memory Converter Registers
#define MCSR_BASE_ADDR 0x0200
#define MCSR  (MCSR_BASE_ADDR + 0x0000)
#define LISAR (MCSR_BASE_ADDR + 0x0008)

// Display Modes
// these waveform modes are described here:
// http://www.waveshare.net/w/upload/c/c4/E-paper-mode-declaration.pdf
#define DSP_MD_INIT  0
#define DSP_MD_DU    1
#define DSP_MD_GC16  2
#define DSP_MD_GL16  3
#define DSP_MD_GLR16 4
#define DSP_MD_GLD16 5
#define DSP_MD_A2    6
#define DSP_MD_DU4   7

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
    uint16_t FWVersion[8];
    uint16_t LUTVersion[8];
} IT8951DevInfo;

bool GPIO_Init(void);
IT8951DevInfo IT8951Init(uint16_t VCOM);

void LCDWaitForReady();

void sendAndReceiveSPI(const unsigned char *dataToSend, unsigned char *dataReceived, int length);
void IT8951_WriteCommand(uint16_t command);
void IT8951_WriteData(uint16_t data);
void IT8951_ReadData(uint8_t* data);

void IT8951_WriteMultiArg(uint16_t command, uint16_t* arg_buff, uint16_t arg_num);
void IT8951_WriteMultiData(uint16_t* Data_Buff, uint32_t Length);
uint16_t IT8951ReadRegister(uint16_t address);
void IT8951WriteRegister(uint16_t address, uint16_t value);

void IT8951Reset();
void IT8951SystemRun();
void IT8951SystemStandby();
void IT8951SystemSleep();
void IT8951GetSystemInfo(void* Buf);

void IT8951SetVcom(uint16_t vcom);
uint16_t IT8951GetVcom();

void IT8951LoadImageStart(IT8951LoadImgInfo* LoadImageInfo);
void IT8951LoadImageAreaStart(IT8951LoadImgInfo* LoadImageInfo, IT8951AreaImgInfo* AreaImgInfo);
void IT8951LoadImageEnd();

void IT8951_ClearRefresh(IT8951DevInfo Dev_Info, uint32_t Target_Memory_Address, uint16_t Mode);

void IT8951DisplayImage_1bpp_Refresh(uint8_t* Frame_Buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t mode, uint32_t Target_Memory_Address, bool Packed_Write);
void IT8951DisplayImage_4bpp_Refresh(uint8_t* Frame_Buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t mode, uint32_t Target_Memory_Address, bool Packed_Write);

uint8_t Display_BMP_Example(uint16_t Panel_Width, uint16_t Panel_Height, uint32_t Init_Target_Memory_Addr, uint8_t BitsPerPixel);
#endif