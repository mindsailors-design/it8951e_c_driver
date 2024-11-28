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
    unsigned char command_preamble[2];
    command_preamble[0] = 0x60;
    command_preamble[1] = 0x00;

    uint8_t command_char[2];
    printf("0x%04x\n", command);

    command_char[0] = (command >> 8) & 0xFF;
    command_char[1] = (command & 0xFF);

    printf("0x%02x\n", command_char[0]);
    printf("0x%02x\n", command_char[1]);

    // LCDWaitForReady();
    
    digitalWrite(CS, LOW);

    wiringPiSPIDataRW(SPI_CHAN, command_preamble, sizeof(command));
    // wiringPiSPIDataRW(SPI_CHAN, command_preamble, sizeof(command));

    // LCDWaitForReady();

    wiringPiSPIDataRW(SPI_CHAN, command_char, sizeof(command));
    // wiringPiSPIDataRW(SPI_CHAN, command, sizeof(command));

    digitalWrite(CS, HIGH);

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
    pinMode(HST_RDY, OUTPUT);

    // default CS needs to be HIGH
    digitalWrite(CS, HIGH);

    // SPIWriteCommand(USDEF_I80_CMD_GET_DEV_INFO);
    // SPIWriteCommand(IT8951_TCON_STANDBY);
    // SPIWriteCommand(IT8951_TCON_MEM_BST_RD_S);
    // SPIWriteCommand(IT8951_TCON_LD_IMG_AREA);

    // SPIWriteData(USDEF_I80_CMD_GET_DEV_INFO);
    // SPIWriteData(IT8951_TCON_STANDBY);
    // SPIWriteData(IT8951_TCON_MEM_BST_RD_S);
    // SPIWriteData(IT8951_TCON_LD_IMG_AREA);

    SPIReadData();


    // digitalWrite (CS, HIGH) ;	// On
    // delay (500) ;		// mS
    // digitalWrite (CS, LOW) ;	// Off
    // delay (500) ;
  

    // printf("Sending data over SPI...\n");
    // sendAndReceiveSPI(dataToSend, dataReceived, sizeof(dataToSend));

    // printf("Sent: ");
    // for (int i = 0; i < sizeof(dataToSend); i++)
    // {
    //     printf("0x%02X ", dataToSend[i]);
    // }
    // printf("\n");
    
    // printf("Received: ");
    // for (int i = 0; i < sizeof(dataReceived); i++)
    // {
    //     printf("0x%02X ", dataReceived[i]);
    // }
    // printf("\n");
    
    return 0;
}