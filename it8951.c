#include "it8951.h"

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

bool IT8951DisplayImage()
{
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
}