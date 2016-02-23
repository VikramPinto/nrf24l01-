#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "nrf_e.h"

int main(int argc, char *argv[])
{ 
	ch_no = get_ch_no(argv[2]);	//ascii to HEX convertion
	spi_init();
/*	pinCE_init();
	pinCE_high();
	sleep(1);
	pinCE_low();
*/
	if((*argv[1] == 'T')||(*argv[1] == 't'))
	{	
		nrftx_init();
		txdata = argv[3];
//		strcpy(txdata, "*****Hello*****");
//		ptxdata = txdata;
//		txdata = "*****Hello*****" ; 	//transmit data string 4th arg
		printf("Enter the string_message to be streamed\n>");
		while(1) 	  	//window to chat
		{
		nrftx_data(txdata);
		scanf("%s",txdata);
		for(i=strlen(txdata); i<16; i++)
		txdata[i]='_';
		txdata[15]='\0';		
		}
	}
	else
	{
		nrfrx_init();
//		while(1)
		nrfrx_data();
	}
	
	close(fd);
	return 0;
	
}
