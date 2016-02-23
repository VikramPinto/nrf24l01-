#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "nrf_g.h"

void pabort(const char *s)
{
        perror(s);
        abort();
}

void spi_init(void)
{	
	int ret;
	fd = open(device, O_RDWR);
        if (fd < 0)
                pabort("can't open device");

        /*
         * spi mode
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1)
                pabort("can't set spi mode");

        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1)
                pabort("can't get spi mode");

        /*
         * bits per word
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't set bits per word");

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't get bits per word");
        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't set max speed hz");

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't get max speed hz");

        printf("\nspi mode: %d\n", mode);
        printf("bits per word: %d\n", bits);
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}


void pinCE_init(void)
{
	fd3 = fopen("/sys/class/gpio/export", "w");
	j = fwrite("106",sizeof(char), 4, fd3);
//	printf("%d export    : bytes written return val=%d\n", fd3, j);
	fclose(fd3);

	fd4 = fopen("/sys/class/gpio/gpio106/direction", "w");
	j = fwrite("out",sizeof(char), 4, fd4);
//	printf("%d direction : bytes written return val=%d\n", fd4, j);
	fclose(fd4);
//	system("echo 106 > /sys/class/gpio/export");
//	system("echo out > /sys/class/gpio/gpio106/direction");
}

void pinCE_low(void)
{	
//	char* zero = '0';
	fd2 = open("/sys/class/gpio/gpio106/value", 777);
	j = write(fd2, "0", 3);
//	printf("%d direction: bytes written j=%d\n", fd2, j);
	close(fd2);
//system("echo 1   > /sys/class/gpio/gpio106/value");
}

void pinCE_high(void)
{	
//	char *one = '1';
	fd2 = open("/sys/class/gpio/gpio106/value", 777);
	j = write(fd2, "1", 3);
//	printf("%d direction: bytes written j=%d\n", fd2, j);
	close(fd2);
//system("echo 1   > /sys/class/gpio/gpio106/value");
}

void spi_send1(char d0, char d1)
{
	int ret;
        tx[0]=d0;
        tx[1]=d1;
 	struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0)
                pabort("spi_send1->can't send spi message");

/*        for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
                if (!(ret % 6))
                        puts("");
                printf("%.2X ", rx[ret]);
        }
        puts("");
*/}

void spi_send5(char d0, char d1, char d2, char d3, char d4, char d5)
{
        int ret;
        txx[0] = d0;
        txx[1] = d1;
        txx[2] = d2;
        txx[3] = d3;
        txx[4] = d4;
        txx[5] = d5;

        struct spi_ioc_transfer trr = {
                .tx_buf = (unsigned long)txx,
                .rx_buf = (unsigned long)rxx,
                .len = ARRAY_SIZE(txx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &trr);
        if (ret < 0)
                pabort("spi_send5-> can't send spi message");

/*        for (ret = 0; ret < ARRAY_SIZE(txx); ret++) {
                if (!(ret % 6))
                        puts("");
                printf("%.2X ", rxx[ret]);
        }
        puts("");
*/}


void spi_send16(char cmd, char d[])
{
        int ret, i=0;
        txxx[0] = cmd;
        for(i=0;i<16;i++)
        txxx[1+i] = d[i];
       struct spi_ioc_transfer trrr = {
                .tx_buf = (unsigned long)txxx,
                .rx_buf = (unsigned long)rxxx,
                .len = 16,// ARRAY_SIZE(txxx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &trrr);
        if (ret < 0)
                pabort("spi_send16-> can't send spi message");

        for (ret = 0; ret < ARRAY_SIZE(txxx); ret++) {
//                if (!(ret % 8))
//                        puts("");
                       printf("%c", rxxx[ret]);
        }
        puts("");
}

void nrftx_init(void)
{
//	pinCE_init();
	printf("configuring nRF24L01 registers for PTX mode\n");
        spi_send1(0xe1, 0xe1); //flush tx FIFOs
        spi_send5(0x30, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7);
        spi_send5(0x2a, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7);

        spi_send1(0x20, 0x0e); //write reg CONFIG<00> PWR_UP, ~PRIM_RX, CRC0
        spi_send1(0x21, 0x00); //write reg EN_AA Disable all AUTO_ACK
        spi_send1(0x22, 0x01); //write reg EN_ADDR enble data pipe0
        spi_send1(0X23, 0X03); //write reg SETUP_AWK<03> AW->11
        spi_send1(0x24, 0x00); //write reg SETUP_RETR<04> ARC-> 0       
        spi_send1(0x25, ch_no); //write reg RF_CH<05> select channel 2
        spi_send1(0x26, 0x03); //write reg RF_SETUP<06> RF_DR_HIGH->01, RF_PWR->11 {2Mbps} 
        spi_send1(0x27, 0xff); //write STATUS<07> to clear all flags
        spi_send1(0x31, 0x05); //write reg RX_PW_P0<11> 5 bytes
	spi_send1(0xe1, 0xe1); //flush TX FIFOs
	printf("streaming data on 0x%x channel\n",ch_no);
}

void nrfrx_init(void)
{
//	pinCE_init();
	printf("configuring nRF24L01 registers for PRX mode");
        spi_send1(0xe2, 0xe2); //flush RX FIFOs
        spi_send5(0x2a, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7);//
        spi_send1(0x20, 0x0f); //write reg CONFIG<00> PWR_UP, PRIM_RX, CRC0
        spi_send1(0x21, 0x00); //write reg EN_AA Disable all AUTO_ACK
        spi_send1(0x22, 0xff); //write reg EN_RXADDR enable all data pipes
        spi_send1(0X23, 0X03); //write reg SETUP_AWK<03> AW->11        
        spi_send1(0xe2, 0xe2); //flush RX FIFOs
        spi_send1(0x25, ch_no); //write reg RF_CH<05> select channel 2 
        spi_send1(0x26, 0x03); //write reg RF_SETUP<06> RF_DR_HIGH->01,RF_PWR->11 {2Mbps} 
        spi_send1(0x31, 0x0f); //write reg RX_PW_P0<11> 16 bytes
        spi_send1(0x3D, 0x00); //write reg FEATURE<1D> Disable  
        printf("clearing status reg\n");
        spi_send1(0x07, 0xff); //write reg STATUS<07> to clear all flags
	printf("listening data on 0x%x channel\n",ch_no);
}

void nrftx_data(char d[])
{
	pinCE_low();
//        printf("sending %s \n",dp);
        spi_send16(0xa0, d);

        pinCE_high();
        usleep(1000);//1mS pusle
        pinCE_low();
//        puts("status check below");
        spi_send1(0x07, 0xff); //check STATUS if byte transmitted
        spi_send1(0x27, 0xff); //clear STATUS
        spi_send1(0x27, 0Xff); // " "   "
//	spi_send1(0xe1, 0xe1); //flush TX FIFOs
	printf(">");
}

void nrfrx_data(void)
{
        pinCE_high();
        spi_send1(0x09, 0xff); //read reg CD Carrier detect
        printf("polling for carrier detect\n");
        while(1)
        {
                spi_send1(0x09, 0xff);
                if(rx[0] & 0xf0) //checking if RX flag set
                {
                        pinCE_low();
                        printf("The data received is...... ");
                        spi_send16(0x61, "random_data_push");
                        spi_send1(0x27, 0xff); //clear STATUS
//                      	spi_send1(0xe2, 0xe2); //flush RX FIFOs
			pinCE_high();
        	}
//	usleep(50000);
	}
}

int get_ch_no(char *j)
{
	unsigned char data=0, val=0;
		data = *j;
			if(data <= 0x39)
				val = (data - 0x30) << 4; //MSB
			else
				val = (data - 0x37) << 4;
//			printf("yo %x\n",val); 
		data = *(j+1);
			if(data <= '9')
				val |= (data - 0x30);
			else 
				val |= (data - 0x37);
//			printf("yo %x\n", val);
	return val;	
}
