#if !defined(_OS9000)
#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gpstypes.h>
#include <gpsproto.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "gpstypes.h"
#include "gpsproto.h"
#include <errno.h>
#endif
#include "compat.h"

extern int Debug ;

#define MAXGPIBMSGLENGTH 30

void ComNoEndOfCom(ioset *io){ /* got 13 10 inside a command, though not completed  */
io->state=2;
if ( Debug ) fprintf(stderr,
"NoEndOfCom:@@%c%c %d\n",
io->buffer[2],io->buffer[3],io->current);

}

void reinitbuf(ioset *io) { /* On a perdu le fil, on reinitialise tout
       et on dumpe le buffer*/
  int i;

  io->current=io->state=0;
  if ( Debug ) fprintf(stderr,"\nioset.c: aborting. %d %d %s\n",
  io->buffer[2], io->buffer[3], io->dev);
}

void io_write(ioset *io) {
  int i,j;
  
  j=strlen(io->command);
  i=write(io->fd,io->command,j);
  if (i!=j) 
    perror("ioset.c: write error to port");
  if ( Debug )
    fprintf(stderr,
    "\nioset.c: wrote %s (%d bytes) to %s fd %d.\n",
    io->command,i, io->dev,io->fd);
  fflush(NULL);
}

void io_read(ioset *io) {
  int i, j, k;
  double valeur;
  
  if (io->type&IOTYPGPS){
    if ( Debug > 1 ) fprintf(stderr,"\nioset.c: entering io_read GPS \n");
    i=io->current;
    j=i;
    while (read( io->fd, io->buffer+i, 1) == 1 ){
      if ( Debug > 2 ) fprintf( stderr, "%02x ", *(io->buffer+i) ) ; fflush( stderr ) ;
      switch (io->state) {
      case 0:
if (io->buffer[i]== '@' )
  io->state=1;
break;

      case 1:
if(io->buffer[i]== '@' )
  io->state=2;
else
  io->state=0;
break;

      case 2:
if(io->buffer[i]==13)
  io->state=3;
break;

      case 3:
if(io->buffer[i]==10)
  io->state=4;
else
  if(io->buffer[i]==13)
    io->state=3;
  else
    io->state=2;
break;
      }
      
      if (io->state)
++io->current;
      
      if (io->state==4) 
io->process(io);
      
      if (io->current>300) {
reinitbuf(io);  
/* On a perdu le fil, on remet tout a 0 */
if ( Debug ) fprintf( stderr, "Lost in the messages\n" ) ;
      }
      i=io->current;
    }

    if ( Debug > 1 )fprintf(stderr,"\nioset.c: returning from ioset GPS \n");
    return;
  }
  if (io->type&IOTYPSR620){
    i=io->current;
    while (read(io->fd,io->buffer+i,1)==1){
      switch (io->state) {
      case 0:
if(io->buffer[i]==13)
  io->state=1;
break;

      case 1:
if(io->buffer[i]==10)
  io->state=2;
else
  if(io->buffer[i]==13)
    io->state=1;
  else
    io->state=0;
break;
      }
      
      ++io->current;
      
      if (io->state==2){
io->process(io);
io->state=io->current=0;
      }
      
      if (io->current>300)
reinitbuf(io);   
/* On a perdu le fil, on remet tout a 0 */
      
      i=io->current;
    }
    return ;
  }
  if(io->type&IOTYPHP53132A){
    i=io->current;
    while (read(io->fd,io->buffer+i,1)==1){
      if ( Debug > 2 ) fprintf(stderr,"io->state:%d\n", io->state);
      switch (io->state) {
      case 0:
if (isalpha(io->buffer[i])&& io->buffer[i]!='E' && io->buffer[i]!='e'){
  io->state=io->current=0;
  if ( Debug > 2 ) fprintf(stderr,"%d\n", io->buffer[i]);
  return;
}

if(io->buffer[i]==10 && i > 2){
  io->state=1;
  if ( Debug > 2 )
    fprintf(stderr,"Yet another measurement %d %s\n",i,io->buffer);
}
break;

      }
      
      ++io->current;
      if ( Debug > 2 ) fprintf(stderr,"about to going to traistan\n");
      
      if (io->state==1){
if ( Debug > 2 ) fprintf(stderr,"Ok, going to traistan\n");
io->process(io);
io->state=io->current=0;
      }
      
      if (io->current>300)
reinitbuf(io);   
/* On a perdu le fil, on remet tout a 0 */
      
      i=io->current;
    }
    return ;
  }

  if (io->type&IOTYPFIFO){
    i=io->current;
    while (read(io->fd,io->buffer+i,1)==1){
      ++io->current;
      
      if(io->buffer[i]=='\n'){
io->process(io);
io->state=io->current=0;
      }
      
      if (io->current>300)
reinitbuf(io);   
/* On a perdu le fil, on remet tout a 0 */
      
      i=io->current;
    }
    return;      
  }
  if ( Debug > 1 ) fprintf(stderr, "ioset.c: unknown ioset type %d\n", io->type);
}
