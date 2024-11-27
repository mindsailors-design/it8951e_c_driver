# Quick notes about how IT8951E works and how to talk to it via SPI.
Based on datasheet and programming guide.


1. Firstly, it is a e-paper display driver, that works over I80, SPI and I2C buses, however using SPI or I2C it is based on I80 commands that are sent via these different interfaces.
2. Code based on [repository](https://github.com/waveshare/IT8951) done by Waveshare.
3. List Host Commands (page 6):

| Value      | Name |
| ---------- | ----- |
| 0x0001    | SYS_RUN |
| 0x0003    | SLEEP |
| 0x0010    | REG_RD |
| 0x0011    | REG_WR |
| 0x0012    | MEM_BST_RD_T |
| 0x0013    | MEM_BST_RD_S |
| 0x0014    | MEM_BST_WR |
| 0x0015    | MEM_BST_END |
| 0x0020    | LD_IMG |
| 0x0021    | LD_IMG_AREA |
| 0x0022    | LD_IMG_END |

4. Preprogrammed User Defined Commands (page 10):

| Value      | Name |
| ---------- | ----- |
| 0x0302   | GET_DEVICE_SYSTEM_INFO |
| 0x0034    | DISPLAY_AREA |
| 0x0038    | POWER_ON_OFF_SEQUENCE |
| 0x0039    | SET_VCOM_VALUE |

5. Datasheet doesn't say anything about HARDWARE_READY signal (pin) when using SPI but programming guide says that because SPI is just translated I80, you need to wait for that signal to be valid to start SPI transmission.
6. Every SPI transmission needs to start with one of correct preambules.

| Name       | Value |
| ---------- | ----- |
| Command    | 0x0000 |
| Write Data | 0x6000 |
| Read Data  | 0x1000 |
7. 