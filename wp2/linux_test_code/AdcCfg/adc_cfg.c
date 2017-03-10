
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
int init_device(int fd)
{
  int mode=0;
  uint32_t speed=500000;
  uint8_t bits=8;
  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1){
    printf("SPI mode  for write- set problem \n");
    return(1);
  }
  if (ioctl(fd, SPI_IOC_RD_MODE, &mode) == -1){
    printf("SPI mode for read - set problem \n");
    return(2);
  }
     
  // bits per word
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1){
    printf("bit number - set problem - write\n");
    return(3);
  }
  if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1){
    printf("but number - set problem - read\n");
    return(4);
  }
  
  // max speed hz
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1){
    printf("SPI max speed problem - Write\n");
    return(5);
  }

  if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1){
    printf("SPI max speed problem - Read\n");
    return(6);
  }
  return(0);
}

int main (int argc,char *argv[])
{
  
  int i, fd;
  int ret = 0;
  int addr,val;
  char cmd[3],adc;
  char out[3];
  char devname[50];

  struct spi_ioc_transfer transf[2];

  
  int nd;
  if(argc<3){
    printf("xxuse as: \n%s <ADC> <address> [val] \n",argv[0]);
    printf("<ADC> which adc is to be check/configure: 0, ..., 4)\n"
	   "<address> which address to be checked \n",
	   "[val] if is to write the parameter \n");
    return(0);
  }
  adc=atoi(argv[1]);
  sscanf(argv[2],"%x",&addr);
  if(argc==4){
    sscanf(argv[3],"%x",&val);
    cmd[0]=0x00;
  } else {
    val=0;
    cmd[0]=0x80;
  }
  cmd[0] |= (addr >> 8)&0x1F;
  cmd[1] = addr & 0xFF;
  cmd[2] = val & 0xFF;
  if(adc<0 || 5<adc){
    printf("adc need to be 0,...,4\n");
    return(0);
  }
  
  sprintf(devname,"/dev/spidev32766.%d",adc);
  
  fd = open(devname, O_RDWR);

  if (fd < 0){
    printf("Problem to open the device: %s\n",devname);
    return(1);
  }
  if(init_device(fd)!=0){
    return(1);
  }
  memset(&(transf[0]),0,sizeof(transf[0]));
  memset(&(transf[1]),0,sizeof(transf[1]));
  
  transf[0].tx_buf = (unsigned long) cmd;
  transf[0].rx_buf = (unsigned long)out;
  transf[0].len =3;
  transf[0].cs_change=1;
  transf[1].tx_buf = (unsigned long)NULL;
  transf[1].rx_buf = (unsigned long) out;
  transf[1].len =3;
  transf[1].cs_change=1;
  
  if((ret=ioctl(fd,SPI_IOC_MESSAGE(1), transf))!=-1){
    for(i=0;i<3;i++){
      printf("%02x %02x \n",cmd[i],out[i]);
    }
  } else {
    printf("error %d  %d!!\n",errno,ret);
  }
  close(fd);
  return(0);

  /*
    addr val 
     14   A0 ADC bus configuration LVDS, interleave
     14   A4 ADC bus config. LVDS, interleave and inverted
     00   3C ADC reset - it is strange ...
     18   04 Vref setting
     08   00 PWRD - reset normal setting
     08   03 PWRD reset setting
     17   25 ADC delay
     05   03 - select both channel
     0D   .. - test mode
     19   ... user pattern 
     1A   ... user pattern
  */
}

