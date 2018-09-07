/*
 * 2016/10/02: R. Sato
 *   modify to study the behavior GPS receiver protocol.
 *   This program just send a command to the receiver and
 *   see the output from the receiver.
 * Use as: <program> [CMD1 CMD2 [CMD3] [CMD4] ...]
 *   where CMD1 is the first byte of hexdecimal value of the command
 *      CMD2 is the second and so on.
        But without "@@" and the check sum and tail.
 *   For example: to send the command "@@Ham<CR><LF>" with m=1,
 *    one should write as: <program> 48 61 1
 *    since the letter "H" is 0x48 in the ASCII table and
 *    "a" is 0x61
 *  Modified from the original code from R. Halliday.
 */
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

char xorchk(char* message, int bytes){
  //Takes the Xor checksum of 'bytes' number of entries in a char array
  char chksum=0;
  int i;
  for(i=0;i<bytes;i++){
    chksum^=message[i];
  }
  return chksum;
}
int gen_msg(char *cmd,int ncmd,char *msg)
{
  int n;
  msg[0]='@';
  msg[1]='@';
  n=2;
  memcpy(msg+n,cmd,ncmd);
  n+=ncmd;
  *(msg+n)=xorchk(cmd,ncmd);
  n++;
  *(msg+n)=0x0D;
  n++;
  *(msg+n)=0x0A;
  return(n+1);
}

int main(int argc, char *argv[])
{
  int gpsuart,nloop;
  int buffincr=0;
  int buffnow,messbeg,messend,i,j;
  char buff[1024],cmd[10],rr[32];
  
  char Hn[154];
  char Bb[92];
  char Ha[78];

  FILE *fd;
  struct termios newtio;
  fd_set readfds,aux;
  struct timeval timeout;
  
  printf("Enter !!!\n");
  
  gpsuart=open("/dev/ttyUL1", O_RDWR | O_NOCTTY );
  fd=fopen("output","w");
  if(gpsuart<0){
    printf("Not possible to open the gps device /dev/ttyUL1   %d\n",gpsuart);
    return(0);
  }
  tcgetattr(gpsuart, &newtio);
  newtio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
			  | INLCR | IGNCR | ICRNL | IXON);
  newtio.c_oflag &= ~OPOST;
  newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  newtio.c_cflag &= ~(CSIZE | PARENB);
  newtio.c_cflag |= CS8;

  tcflush(gpsuart, TCIFLUSH);
  tcsetattr(gpsuart,TCSANOW,&newtio);

  FD_ZERO(&readfds);
  FD_SET(gpsuart,&readfds);
  
  for(i=1;i<argc;i++){
    sscanf(argv[i],"%x",&(cmd[i-1]));
  }

  if(argc>2){
    buffnow=gen_msg(cmd,argc-1,buff);
    for(i=0;i<buffnow;i++){
      printf("%02x ",buff[i]);
    }
    printf("\n");
    while (1)  // Debug!!!
      {
    write(gpsuart,buff,buffnow);
    fsync(gpsuart);
  } // Debug !!!
  }
  nloop=0;
  while(nloop<50){
    aux=readfds;
    timeout.tv_sec=60;
    timeout.tv_usec=0;
    select(gpsuart+1,&aux,NULL,NULL,&timeout);
    gettimeofday(&timeout,NULL);
    printf("time: %d.%06d\n",timeout.tv_sec,timeout.tv_usec);
    usleep(200000);
    if (FD_ISSET(gpsuart,&aux)){
      buffnow=read(gpsuart,buff,1024);
      printf("read: %d\n",buffnow);
      
      fwrite("===>",1,4,fd);
      fwrite(buff,1,buffnow,fd);
      fwrite("<===",1,4,fd);
      fflush(fd);
      j=0;
      for(i=0;i<buffnow;i++){
	if(i-j == 16){
	  rr[16]='\0';
	  printf("===>%16s<====\n",rr);
	  j+=16;
	}
	printf("%02x ",buff[i]);
	if(0x20<=buff[i] && buff[i]<=0x7F){
	  rr[i-j]=buff[i];
	} else
	  rr[i-j]='.';

      }
      
      if(i-j>0){
	rr[i-j]='\0';
	while(i-j<16){
	  printf("   ");
	  i++;
	}
	printf("===>%s<====",rr);
      }
      printf("\n============\n");
      fflush(stdout);
    }
    nloop++;
  }
  
  close(gpsuart);
  fclose(fd);
}

