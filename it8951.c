#include "it8951.h"

uint8_t* Refresh_Frame_Buf = NULL;

uint8_t* Panel_Frame_Buff = NULL;
uint8_t* Panel_Area_Frame_Buff = NULL;

bool Four_Byte_Align = false;

void LCDWaitForReady()
{
	bool ulData = digitalRead(HST_RDY);
	while(ulData == 0)
	{
		ulData = digitalRead(HST_RDY);
        // printf("HST_RDY: %d\n", digitalRead(HST_RDY));
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

void IT8951_WriteCommand(uint16_t command)
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

    digitalWrite(CS, LOW);
    int result = wiringPiSPIDataRW(SPI_CHAN, data_to_send, sizeof(data_to_send));
    digitalWrite(CS, HIGH);
    if (result == -1)
    {
        perror("SPI communication failed");
    }
    else
    {
        printf("SPI result: %d\n", result);
    }
}

void IT8951_WriteData(uint16_t data)
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

    digitalWrite(CS, LOW);
    int result = wiringPiSPIDataRW(SPI_CHAN, data_to_send, sizeof(data_to_send));
    digitalWrite(CS, HIGH);
    if (result == -1)
    {
        perror("SPI communication failed");
    }
    else
    {
        printf("SPI result: %d\n", result);
    }   
}

void IT8951_ReadData(uint8_t* data)
{
    // read_preamble = 0x10, read_preamble = 0x00, dummy = 0x00, dummy = 0x00
    data[0] = 0x10;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;

    LCDWaitForReady();

    digitalWrite(CS, LOW);
    int result = wiringPiSPIDataRW(SPI_CHAN, data, BUFFER_SIZE);
    digitalWrite(CS, HIGH);
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

void IT8951_ReadMultiData(uint16_t *Data_Buf, uint32_t Length)
{
    uint8_t data[BUFFER_SIZE];
    uint8_t rxBuffer[2];

    data[0] = 0x10;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;

    LCDWaitForReady();

    digitalWrite(CS, LOW);
    int result = wiringPiSPIDataRW(SPI_CHAN, data, BUFFER_SIZE);
    if (result == -1)
    {
        perror("SPI communication failed");
    }
    else
    {
        printf("SPI result: %d\n", result);
    }
    
    // read two dummy words first, then usefull data

    // read multi data from it8951
    for (uint32_t i = 0; i < Length; i++)
    {
        wiringPiSPIDataRW(SPI_CHAN, rxBuffer, 2);
        Data_Buf[i] = rxBuffer[0] << 8 | rxBuffer[1];
    }
    
    digitalWrite(CS, HIGH);
}

// write multi arg
void IT8951_WriteMultiArg(uint16_t command, uint16_t* arg_buff, uint16_t arg_num)
{
    // send command
    IT8951_WriteCommand(command);
    // send data
    for (uint16_t i = 0; i < arg_num; i++)
    {
        IT8951_WriteData(arg_buff[i]);
    }
}

void IT8951_WriteMultiData(uint16_t* Data_Buff, uint32_t Length)
{
    uint8_t data[BUFFER_SIZE];
    uint8_t rxBuffer[2];

    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;

    LCDWaitForReady();

    digitalWrite(CS, LOW);
    int result = wiringPiSPIDataRW(SPI_CHAN, data, BUFFER_SIZE);
    if (result == -1)
    {
        perror("SPI communication failed");
    }
    // else
    // {
    //     printf("SPI result: %d\n", result);
    // }

    LCDWaitForReady();

    uint8_t txBuffer[2];

    for (uint32_t i = 0; i < Length; i++)
    {
        // printf("i: %d\n", i);
        txBuffer[0] = (Data_Buff[i] >> 8) & 0xFF;
        txBuffer[1] = (Data_Buff[i]) & 0xFF;
        result = wiringPiSPIDataRW(SPI_CHAN, txBuffer, 2);
        if (result == -1)
            perror("SPI communication failed");
    }
    digitalWrite(CS, HIGH);
    
}

// readreg
uint16_t IT8951ReadRegister(uint16_t address)
{
    IT8951_WriteCommand(IT8951_TCON_REG_RD);
    IT8951_WriteData(address);

    uint8_t data[BUFFER_SIZE] = {0};
    IT8951_ReadData(data);
    // printf("%p\n", (void*)data);
    
    printf("First byte: 0x%02x\n", data[2]);
    printf("Second byte: 0x%02x\n", data[3]);
}
// writereg
void IT8951WriteRegister(uint16_t address, uint16_t value)
{
    IT8951_WriteCommand(IT8951_TCON_REG_WR);
    IT8951_WriteData(address);
    IT8951_WriteData(value);
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
    IT8951_WriteCommand(IT8951_TCON_SYS_RUN);
}
// standby
void IT8951SystemStandby()
{
    IT8951_WriteCommand(IT8951_TCON_STANDBY);
}
// sleep 
void IT8951SystemSleep()
{
    IT8951_WriteCommand(IT8951_TCON_SLEEP);
}
// system info
void IT8951GetSystemInfo(void* Buf)
{
    // TODO: make a dev_info struct
    IT8951DevInfo *DevInfo;

    IT8951_WriteCommand(USDEF_I80_CMD_GET_DEV_INFO);

    LCDWaitForReady();

    IT8951_ReadMultiData((uint16_t*) Buf, sizeof(IT8951DevInfo)/2);

    // TODO: parse dev info to dev_info struct
    DevInfo = (IT8951DevInfo*)Buf;

    printf("Panel (W,H) = (%d, %d) \r\n", DevInfo->PanelWidth, DevInfo->PanelHeight);
    printf("Memory Address = %X\n", DevInfo->MemoryAddrL | DevInfo->MemoryAddrH << 16);
    
    char firmware_version[9];
    char lut_version[9];

    for (int8_t i = 0; i < 4; i++)
    {
        firmware_version[i * 2] = (char)(DevInfo->FWVersion[i] >> 8);
        firmware_version[i * 2 + 1] = (char)(DevInfo->FWVersion[i] & 0xFF);
    }
    firmware_version[8] = '\0';
    
    printf("FW Version: %s\n", firmware_version);
    
    for (int8_t i = 0; i < 4; i++)
    {
        lut_version[i * 2] = (char)(DevInfo->LUTVersion[i] >> 8);
        lut_version[i * 2 + 1] = (char)(DevInfo->LUTVersion[i] & 0xFF);
    }
    lut_version[8] = '\0';

    printf("LUT Version: %s\n", lut_version);
}

void IT8951_SetTargetMemoryAddress(uint32_t TargetMemoryAddress)
{
    uint16_t WordH = (uint16_t)((TargetMemoryAddress >> 16) & 0x0000FFFF);
    uint16_t WordL = (uint16_t)(TargetMemoryAddress & 0x0000FFFF);

    IT8951WriteRegister(LISAR+2, WordH);
    IT8951WriteRegister(LISAR, WordL);
}

// set vcom
void IT8951SetVcom(uint16_t vcom)
{
    IT8951_WriteCommand(USDEF_I80_CMD_VCOM);
    IT8951_WriteData(0x0001);
    printf("VCOM SET: %d\n", vcom);
    IT8951_WriteData(vcom);
}
// get vcom
uint16_t IT8951GetVcom()
{
    // BUG: vcom register must be read twice to get correct value
    // on the first read it returns previous value
    uint8_t buffer[BUFFER_SIZE] = {0};
    uint16_t Vcom = 0;
    IT8951_WriteCommand(USDEF_I80_CMD_VCOM);
    IT8951_WriteData(0x0000);
    IT8951_ReadData(buffer);
    IT8951_ReadData(buffer);
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

    IT8951_WriteCommand(IT8951_TCON_LD_IMG);
    IT8951_WriteData(Args);
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
    IT8951_WriteCommand(IT8951_TCON_LD_IMG_END);
}

bool GPIO_Init(void)
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
    pinMode(CS, OUTPUT);

    digitalWrite(RST, HIGH);
    digitalWrite(CS, HIGH);
}

IT8951DevInfo IT8951Init(uint16_t VCOM)
{
    IT8951DevInfo DevInfo;

    IT8951Reset();
    IT8951SystemRun();

    IT8951GetSystemInfo(&DevInfo);

    uint32_t Target_Memory_Address = DevInfo.MemoryAddrL | (DevInfo.MemoryAddrH << 16);

    IT8951_SetTargetMemoryAddress(Target_Memory_Address);

    // Enable pack write
    IT8951WriteRegister(I80CPCR, 0x0001);

    // set vcom if stored is different
    if (VCOM != IT8951GetVcom())
    {
        IT8951SetVcom(VCOM);
        printf("Vcom = -%.02fV\n", (float)IT8951GetVcom()/1000);
    }

    return DevInfo;
}

void IT8951DisplayArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t mode){
    uint16_t Args[5];
    Args[0] = x;
    Args[1] = y;
    Args[2] = w;
    Args[3] = h;
    Args[4] = mode;

    IT8951_WriteMultiArg(USDEF_I80_CMD_DPY_AREA, Args, 5);
}

void IT8951DisplayAreaBuf(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t mode, uint32_t Target_Memory_Address){
    uint16_t Args[7];
    Args[0] = x;
    Args[1] = y;
    Args[2] = w;
    Args[3] = h;
    Args[4] = mode;
    Args[5] = (uint16_t)Target_Memory_Address;
    Args[6] = (uint16_t)(Target_Memory_Address>>16);

    IT8951_WriteMultiArg(USDEF_I80_CMD_DPY_BUF_AREA, Args, 7);
}

void IT8951DisplayImage_1bpp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t mode, uint32_t Target_Memory_Address, uint8_t Back_Gray_Val, uint8_t Front_Gray_Val)
{
    IT8951WriteRegister(UP1SR+2, IT8951ReadRegister(UP1SR+2) | (1<<2));
    IT8951WriteRegister(BGVR, (Front_Gray_Val<<8) | Back_Gray_Val);

    if (Target_Memory_Address == 0)
    {
        // it8951_display_area(x,y,w,h,mode);
        IT8951DisplayArea(x, y, w, h, mode);
    }
    else
    {
        // it8951_display_areaBuf(x,y,w,h,mode,Target_Memory_Address);
        IT8951DisplayAreaBuf(x, y, w, h, mode, Target_Memory_Address);
    }
    
    LCDWaitForReady();

    IT8951WriteRegister(UP1SR+2, IT8951ReadRegister(UP1SR+2) & ~(1<<2));

}

void IT8951_HostAreaPackedPixelWrite_1bp(IT8951LoadImgInfo* LoadImageInfo, IT8951AreaImgInfo* AreaImgInfo, bool Packed_Write)
{
    uint16_t Source_Buffer_Width, Source_Buffer_Height, Source_Buffer_Length;

    uint16_t* Source_Buffer = (uint16_t*)LoadImageInfo->SourceBufferAddr;
    IT8951_SetTargetMemoryAddress(LoadImageInfo->TargetMemoryAddr);
    IT8951LoadImageAreaStart(LoadImageInfo, AreaImgInfo);

    // from byte to word
    // use 8bpp to display 1bpp, so here, divide by 2, because every byte has full bit
    Source_Buffer_Width = AreaImgInfo->Width/2;
    Source_Buffer_Height - AreaImgInfo->Height;
    Source_Buffer_Length = Source_Buffer_Width * Source_Buffer_Height;

    if (Packed_Write == true)
    {
        IT8951_WriteMultiData(Source_Buffer, Source_Buffer_Length);
    }
    else
    {
        for (uint32_t i = 0; i < Source_Buffer_Height; i++)
        {
            for (uint32_t j = 0; j < Source_Buffer_Width; j++)
            {
                IT8951_WriteData(*Source_Buffer);
                Source_Buffer++;
            }
        }
    }

    IT8951LoadImageEnd();
}

void IT8951_HostAreaPackedPixelWrite_4bpp(IT8951LoadImgInfo* LoadImageInfo, IT8951AreaImgInfo* AreaImgInfo, bool Packed_Write)
{
    uint16_t Source_Buffer_Width, Source_Buffer_Height, Source_Buffer_Length;

    uint16_t* Source_Buffer = (uint16_t*)LoadImageInfo->SourceBufferAddr;
    IT8951LoadImageAreaStart(LoadImageInfo, AreaImgInfo);

    // from byte to word
    Source_Buffer_Width = (AreaImgInfo->Width*4/8)/2;
    Source_Buffer_Height - AreaImgInfo->Height;
    Source_Buffer_Length = Source_Buffer_Width * Source_Buffer_Height;

    if (Packed_Write == true)
    {
        IT8951_WriteMultiData(Source_Buffer, Source_Buffer_Length);
    }
    else
    {
        for (uint32_t i = 0; i < Source_Buffer_Height; i++)
        {
            for (uint32_t j = 0; j < Source_Buffer_Width; j++)
            {
                if (Source_Buffer == NULL)
                {
                    fprintf(stderr, "Error: Source_Buffer became NULL at i=%u, j=%u\n", i, j);
                    return;
                }
                
                IT8951_WriteData(*Source_Buffer);
                Source_Buffer++;
            }   
        }   
    }

    IT8951LoadImageEnd();
}

void IT8951_ClearRefresh(IT8951DevInfo Dev_Info, uint32_t Target_Memory_Address, uint16_t Mode)
{
    uint32_t Image_Size = ((Dev_Info.PanelWidth * 4 % 8 ==0)? (Dev_Info.PanelWidth * 4 / 8): (Dev_Info.PanelWidth * 4 / 8 + 1)) * Dev_Info.PanelHeight;
    uint16_t* Frame_Buf = malloc(Image_Size);
    memset(Frame_Buf, 0xFF, Image_Size);

    IT8951LoadImgInfo Load_Img_Info;
    IT8951AreaImgInfo Area_Img_Info;

    LCDWaitForReady();

    Load_Img_Info.SourceBufferAddr = Frame_Buf;
    Load_Img_Info.EndianType = IT8951_LDIMG_L_ENDIAN;
    Load_Img_Info.PixelFormat =  IT8951_4BPP;
    Load_Img_Info.Rotate = IT8951_ROTATE_0;
    Load_Img_Info.TargetMemoryAddr = Target_Memory_Address;

    Area_Img_Info.X = 0;
    Area_Img_Info.Y = 0;
    Area_Img_Info.Width = Dev_Info.PanelWidth;
    Area_Img_Info.Height = Dev_Info.PanelHeight;

    IT8951_HostAreaPackedPixelWrite_4bpp(&Load_Img_Info, &Area_Img_Info, true);

    IT8951DisplayArea(0, 0, Dev_Info.PanelWidth, Dev_Info.PanelHeight, Mode);

    free(Frame_Buf);
    Frame_Buf = NULL;
    
}

void IT8951DisplayImage_1bpp_Refresh(uint8_t* Frame_Buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t mode, uint32_t Target_Memory_Address, bool Packed_Write)
{
    uint32_t Image_Size = ((w * 4 % 8 ==0)? (w * 4 / 8): (w * 4 / 8 + 1)) * h;
    uint16_t* Dummy_Buf = malloc(Image_Size);
    memset(Dummy_Buf, 0x11, Image_Size);

    IT8951LoadImgInfo LoadImgInfo;
    IT8951AreaImgInfo AreaImgInfo;

    LCDWaitForReady();

    LoadImgInfo.SourceBufferAddr = Frame_Buf;
    LoadImgInfo.EndianType = IT8951_LDIMG_L_ENDIAN;
    LoadImgInfo.PixelFormat = IT8951_8BPP;
    LoadImgInfo.Rotate = IT8951_ROTATE_0;
    LoadImgInfo.TargetMemoryAddr = Target_Memory_Address;

    AreaImgInfo.X = x/8;
    AreaImgInfo.Y = y;
    AreaImgInfo.Width = w/8;
    AreaImgInfo.Height = h;

    IT8951_HostAreaPackedPixelWrite_4bpp(&LoadImgInfo, &AreaImgInfo, Packed_Write);

    IT8951DisplayImage_1bpp(x, y, w, h, mode, Target_Memory_Address, 0xF0, 0x00);
} 

// uint8_t Display_BMP_Example(uint16_t Panel_Width, uint16_t Panel_Height, uint32_t Init_Target_Memory_Addr, uint8_t BitsPerPixel)
// {
//     uint16_t Width;
//     if (Four_Byte_Align == true)
//     {
//         Width = Panel_Width - (Panel_Width % 32);
//     }
//     else
//     {
//         Width = Panel_Width;
//     }

//     uint16_t Height = Panel_Height;
    
//     uint32_t Imagesize;

//     Imagesize = ((Width * BitsPerPixel % 8 == 0)? (Width * BitsPerPixel / 8): (Width * BitsPerPixel / 8 + 1)) * Height;
//     if ((Refresh_Frame_Buf = (uint8_t *)malloc(Imagesize)) == NULL)
//     {
//         printf("Failed to apply for black memory... \r\n");
//         return -1;
//     }


    
// }
