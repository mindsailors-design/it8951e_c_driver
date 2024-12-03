#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <stdbool.h>

#define SPI_CHAN 0
#define SPI_SPEED 500000
#define BUFFER_SIZE 4
    // const uint8_t BUFFER_SIZE = 4;

// #define CS 10
#define HST_RDY 5

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
    /*
    BUG
    obsluga chip select jest calkowicie realizowana przez wiringpi, wiec cala obsluge softwareowa trzeba usunac
    dlatego tez caly payload do jednorazowego wyslania trzeba spakowac w jedna tablice i wyslac tylko jeden raz
    opisane to jest w programming quidzie
    dlatego prototypowo recznie zapakowalem preamble i komende do jednej tablicy i wyslalem recznie, dzieki czemu chip select sie zgadza i dziala poprawnie
    zostalo jeszcze rozkminienie jak powinien dzialac ten HST_RDY i odkomentowanie LCDWaitForReady()
    */

    unsigned char data_to_send[4];
    data_to_send[0] = 0x60;
    data_to_send[1] = 0x00;

    data_to_send[2] = command >> 8;
    data_to_send[3] = command;


    printf("0x%02x\n", data_to_send[0]);
    printf("0x%02x\n", data_to_send[1]);
    printf("0x%02x\n", data_to_send[2]);
    printf("0x%02x\n", data_to_send[3]);

    LCDWaitForReady();

    wiringPiSPIDataRW(SPI_CHAN, data_to_send, sizeof(data_to_send));
}

void SPIWriteData(uint16_t data)
{
    unsigned char data_to_send[4];
    data_to_send[0] = 0x00;
    data_to_send[1] = 0x00;
    data_to_send[2] = data >> 8;
    data_to_send[3] = data;

    printf("0x%02x\n", data_to_send[0]);
    printf("0x%02x\n", data_to_send[1]);
    printf("0x%02x\n", data_to_send[2]);
    printf("0x%02x\n", data_to_send[3]);

    LCDWaitForReady();

    wiringPiSPIDataRW(SPI_CHAN, data_to_send, sizeof(data_to_send));
}

void SPIReadData(uint16_t address, uint8_t* data)
{
    // read_preamble = 0x10, read_preamble = 0x00, dummy = 0x00, dummy = 0x00
    data[0] = 0x10;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;

    wiringPiSPIDataRW(SPI_CHAN, data, BUFFER_SIZE);
    
    printf("Data received: ");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("0x%02x ", data[i]); 
    }
    printf("\n");
}

// Host Commands
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
// readreg
void IT8951ReadRegister(uint16_t address)
{
    SPIWriteCommand(IT8951_TCON_REG_RD);
    SPIWriteData(address);

    uint8_t data[BUFFER_SIZE] = {0};
    SPIReadData(address, data);
    printf("%p\n", (void*)data);
    
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

int main(void) {
    printf("RaspberryPi SPI test\n");

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

    IT8951SystemRun();
    // IT8951SystemStandby();
    IT8951SystemSleep();

    IT8951WriteRegister(0x1100, 0x0506);
    // IT8951ReadRegister(0x1100);
 
    return 0;
}