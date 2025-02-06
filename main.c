#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "it8951.h"
#include "read_png_file.h"

uint16_t VCOM = 2140;

IT8951DevInfo Dev_Info;
uint16_t Panel_Width;
uint16_t Panel_Height;
uint32_t Target_Memory_Address;

uint8_t epd_mode = 0;

int main(int argc, char *argv[])
{
        const char *filename = "sleeping_penguin.png";
        unsigned char *image_buffer;
        int width, height, channels;

        read_png_file(filename, &image_buffer, &width, &height, &channels);

        if (image_buffer) {
                printf("Loaded PNG: %dx%d, Channels: %d\n", width, height, channels);

                // Use the image buffer (e.g., pass it to another function)

                
        } else {
                printf("Failed to load PNG image\n");
        }

        GPIO_Init();

        Dev_Info = IT8951Init(VCOM);

        Target_Memory_Address = Dev_Info.MemoryAddrL | (Dev_Info.MemoryAddrH << 16);

        IT8951_ClearRefresh(Dev_Info, Target_Memory_Address, DSP_MD_GC16);

        IT8951DisplayImage_1bpp_Refresh(image_buffer, 0, 0, width, height, DSP_MD_GC16, Target_Memory_Address, true);


        // Free buffer after use
        free(image_buffer);
        
        return 0;
}

/*
void IT8951HostAreaPackedPixelWrite(IT8951LdImgInfo* pstLdImgInfo, IT8951AreaImgInfo* pstAreaImgInfo)
{
	TDWord i,j;
	//Source buffer address of Host
	TWord* pusFrameBuf = (TWord*)pstLdImgInfo->ulStartFBAddr;
	
	//Set Image buffer(IT8951) Base address
	IT8951SetImgBufBaseAddr(pstLdImgInfo->ulImgBufBaseAddr);
	
	//Send Load Image start Cmd
	IT8951LoadImgAreaStart(pstLdImgInfo , pstAreaImgInfo);
	
	//Host Write Data
	for(j=0;j< pstAreaImgInfo->usHeight;j++)
	{
		#ifdef __SPI_2_I80_INF__ //{__SPI_2_I80_INF__
			//Write 1 Line for each SPI transfer
			LCDWriteNData(pusFrameBuf, pstAreaImgInfo->usWidth/2);
			pusFrameBuf += pstAreaImgInfo->usWidth/2;//Change to Next line of loaded image 
													 // (supposed the Continuous image content in hsot frame buffer )
		#else
	for(i=0;i< pstAreaImgInfo->usWidth/2;i++)
	{
		//Write a Word(2-Bytes) for each time
		LCDWriteData(*pusFrameBuf);
		pusFrameBuf++;
	}
	#endif//}__SPI_2_I80_INF__
	}
	//Send Load Img End Command
	IT8951LoadImgEnd();
}
*/