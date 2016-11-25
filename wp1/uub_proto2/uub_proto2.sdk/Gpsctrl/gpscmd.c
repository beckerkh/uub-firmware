#include <stdio.h>
#include <stdlib.h>
#include <modes.h>


#include "gpsdefs.h"
#include "gpsctrl.h"

#include "logfile.h"
#include "compat.h"

#define SUCCESS 0

void short_to_bytes( unsigned char *pb, unsigned short val )
{
  *pb++ = (val >> 8 ) & 0xFF ;
  *pb++ = val & 0xFF ;
}

void
int_to_bytes( unsigned char *pb, unsigned int val )
{
  *pb++ = (val >> 24) & 0xFF ;
  *pb++ = (val >> 16) & 0xFF ;
  *pb++ = (val >> 8 ) & 0xFF ;
  *pb++ = val & 0xFF ;
}

bytes_to_int( unsigned int *value, unsigned char *pb )
{
  *value = (*pb << 24) | (*(pb+1)<<16) | (*(pb+2)<<8) | *(pb+3) ;
}

long dswab( char *from )
{
  long value ;

  value = (*from <<24) | (*(from+1)<<16) | (*(from+2)<< 8) | *(from+3) ;
  return value ;
}

void chksum( unsigned char *dat, int length )
{
  BYTE i, j = length - 3 ;

  dat[j]= dat[2];

  for ( i=3 ; i<j ; ++i )
    dat[j] = dat[j]^dat[i];
}

unsigned commande(comm *com, BYTE *arg )
{
  unsigned char buff[132] ;
  unsigned i;
  unsigned int nb ;
  int ok ;

  memset( buff, 0, com->lng ) ;
  memcpy( buff, com->dat, 4 ) ;

  if ( arg ) memcpy( buff+4, arg+1, *arg ) ;
  else memset( buff+4, 0, com->lng-3 ) ;
  buff[com->lng-2] = 13 ;
  buff[com->lng-1] = 10 ;

  chksum( buff, com->lng ) ;
  nb = com->lng ;
  ok = compatWrite( GpsPathOut, buff, &nb ) ;
  if ( ok != SUCCESS ) PrintLog( LOG_ERROR, " Cant write to GPS\n" ) ;

#if 0
    fprintf(stderr,"Sent (%d) ", nb ) ;
    for(i=0;i<4;++i)
      fprintf(stderr, "-%c", buff[i] );
    for(i=4 ; i<com->lng; ++i)
      fprintf(stderr,"-%02x",buff[i]);
    fprintf( stderr, "\n" );
#endif
    return ok ;
}


