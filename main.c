#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <stdbool.h>

#define SPI_CHAN 0
#define SPI_SPEED 500000

#define CS 10
#define HST_RDY 24

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

    unsigned char command_preamble[4];
    command_preamble[0] = 0x60;
    command_preamble[1] = 0x00;

    command_preamble[2] = 0x11;
    command_preamble[3] = 0x00;

    uint8_t command_char[2];
    printf("0x%04x\n", command);

    command_char[0] = (command >> 8) & 0xFF;
    command_char[1] = (command & 0xFF);

    printf("0x%02x\n", command_preamble[0]);
    printf("0x%02x\n", command_preamble[1]);
    printf("0x%02x\n", command_preamble[2]);
    printf("0x%02x\n", command_preamble[3]);

    // LCDWaitForReady();
    
    // digitalWrite(CS, LOW);

    wiringPiSPIDataRW(SPI_CHAN, command_preamble, sizeof(command_preamble));
    // wiringPiSPIDataRW(SPI_CHAN, command_preamble, sizeof(command));

    // LCDWaitForReady();

    // wiringPiSPIDataRW(SPI_CHAN, command_char, sizeof(command));
    // wiringPiSPIDataRW(SPI_CHAN, command, sizeof(command));

    // digitalWrite(CS, HIGH);
}

void SPIWriteData(uint16_t data)
{
    unsigned char write_preamble[2];
    write_preamble[0] = 0x00;
    write_preamble[1] = 0x00;

    unsigned char data_char[2];
    data_char[0] = (data >> 8) & 0xFF;
    data_char[1] = data & 0xFF;

    // LCDWaitForReady();

    digitalWrite(CS, LOW);

    // wiringPiSPIDataRW(SPI_CHAN, write_preamble >> 8, sizeof(data));
    wiringPiSPIDataRW(SPI_CHAN, write_preamble, sizeof(write_preamble));

    // LCDWaitForReady();

    wiringPiSPIDataRW(SPI_CHAN, data_char, sizeof(data_char));
    // wiringPiSPIDataRW(SPI_CHAN, data, sizeof(data));

    digitalWrite(CS, HIGH);
}

uint16_t SPIReadData()
{
    unsigned char data_received[2] = {0};
    unsigned char read_preamble[2];
    read_preamble[0] = 0x10;
    read_preamble[1] = 0x00;

    
    // LCDWaitForReady();

    digitalWrite(CS, LOW);

    // wiringPiSPIDataRW(SPI_CHAN, read_preamble >> 8, sizeof(read_preamble));
    printf("0x%02x\n", read_preamble[0]);
    printf("0x%02x\n", read_preamble[1]);
    wiringPiSPIDataRW(SPI_CHAN, read_preamble, sizeof(read_preamble));
    printf("0x%02x\n", read_preamble[0]);
    printf("0x%02x\n", read_preamble[1]);

    // LCDWaitForReady();

    wiringPiSPIDataRW(SPI_CHAN, data_received, sizeof(data_received)); // dummy
    printf("0x%02x\n", data_received[0]);
    printf("0x%02x\n", data_received[1]);
    // wiringPiSPIDataRW(SPI_CHAN, data_received, sizeof(data_received)); // dummy

    // LCDWaitForReady();

    data_received[1] = wiringPiSPIDataRW(SPI_CHAN, data_received, sizeof(data_received)) << 8;
    data_received[0] = wiringPiSPIDataRW(SPI_CHAN, data_received, sizeof(data_received));
    printf("0x%02x\n", data_received[0]);
    printf("0x%02x\n", data_received[1]);

    digitalWrite(CS, HIGH);

    uint16_t result = (data_received[1] << 8) | data_received[0];
    printf("0x%04x\n", result);

    return result;
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
uint16_t IT8951ReadRegister(uint16_t address)
{
    uint16_t data;
    SPIWriteCommand(IT8951_TCON_REG_RD);
    SPIWriteData(address);
    data = SPIReadData();
    return data;
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
    
    unsigned char dataToSend[] = {0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF, 0xFF, 0xFF};
    unsigned char dataReceived[sizeof(dataToSend)] = {0};

    pinMode(CS, OUTPUT);
    pinMode(HST_RDY, INPUT);

    // default CS needs to be HIGH
    digitalWrite(CS, HIGH);

    // IT8951SystemRun();
    // IT8951SystemStandby();
    // IT8951SystemSleep();

    // IT8951ReadRegister(0x1234);
    IT8951WriteRegister(0x1100, 0x0506);


    // SPIReadData();    
    return 0;
}