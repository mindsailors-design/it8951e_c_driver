#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <string.h>

#define SPI_CHAN 0
#define SPI_SPEED 500000
#define LED 0

void sendAndReceiveSPI(const unsigned char *dataToSend, unsigned char *dataReceived, int length)
{
    memcpy(dataReceived, dataToSend, length);
    int result = wiringPiSPIDataRW(SPI_CHAN, dataReceived, length);

    if (result == -1)
    {
        perror("SPI communication failed");
    }
    
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
    
    unsigned char dataToSend[] = {0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF, 0xDE, 0xAD, 0xEF};
    unsigned char dataReceived[sizeof(dataToSend)] = {0};

    printf("Sending data over SPI...\n");
    sendAndReceiveSPI(dataToSend, dataReceived, sizeof(dataToSend));

    printf("Sent: ");
    for (int i = 0; i < sizeof(dataToSend); i++)
    {
        printf("0x%02X ", dataToSend[i]);
    }
    printf("\n");
    
    printf("Received: ");
    for (int i = 0; i < sizeof(dataReceived); i++)
    {
        printf("0x%02X ", dataReceived[i]);
    }
    printf("\n");
    
    return 0;
}