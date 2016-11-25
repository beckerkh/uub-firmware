/************************************************************************
   Changes 

   Aug 2013 - changed _os_ things to defs in compat.h
************************************************************************/
#if !defined(_OS9000)
#include <stdio.h>
#include <unistd.h>
#include <gpstypes.h>
#include <gpsproto.h>
#else
#include <stdio.h>
#include "gpstypes.h"
#include "gpsproto.h"
#endif
#define SUCCESS 0

extern int InitReceiver;
extern int Debug ;

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
#if 0
void chksum(comm *com)
{
  BYTE i, j=com->lng-3;
  unsigned test;

  com->dat[j]=com->dat[2];

  for (i=3;i<j;++i)
    com->dat[j]=com->dat[j]^com->dat[i];
  /*
  test=com->dat[j];
  test=com->dat[j];
  */
}
#else
void chksum( unsigned char *dat, int length )
{
  BYTE i, j = length - 3 ;
  unsigned test;

  dat[j]= dat[2];

  for (i=3;i<j;++i)
    dat[j] = dat[j]^dat[i];
}
#endif

unsigned commande(comm *com, BYTE *arg, int outcom)
{
  unsigned char buff[132] ;
  unsigned i;
  unsigned int nb ;
  int ok ;

  buff[0] = '\0' ;
  memcpy( buff, com->dat, 4 ) ;
  if ( InitReceiver ) {
    if ( arg ) memcpy( buff+4, arg+1, *arg ) ;
    else memset( buff+4, 0, com->lng-3 ) ;
    buff[com->lng-2] = 13 ;
    buff[com->lng-1] = 10 ;
#if 0
    chksum( (comm *)buffer ) ;
    ok = write( outcom, buff, com->lng ) ;
#else
    chksum( buff, com->lng ) ;
    nb = com->lng ;
    ok = compatWrite( outcom, buff, &nb ) ;
    if ( ok != SUCCESS ) fprintf( stderr, "\n***** Cant write to GPS\n" ) ;
#endif

    if ( Debug ) fprintf(stderr,"Sent: %s - %s\n", com->nom, com->dat );
    if ( Debug > 2 ) {
      for(i=0;i<4;++i)
fprintf(stderr, "-%c", buff[i] );
      for(i=4 ; i<com->lng; ++i)
fprintf(stderr,"-%d",buff[i]);
      fprintf( stderr, "\n" );
    }
    return ok ;
  }
  else
    return 0;
}


