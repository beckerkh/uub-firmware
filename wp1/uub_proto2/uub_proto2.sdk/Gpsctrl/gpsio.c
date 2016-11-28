/* Changes

   Aug 2013 - Moved signal.h to compat.h
              Moved _os_ things to defs in compat.h

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <const.h>

#include "compat.h"
#include "gpsdefs.h"
#include "gpsctrl.h"
#include "logfile.h"


#define INBUFF_SIZE 256

static int CharInBuff = 0 ;
static unsigned char InBuff[INBUFF_SIZE + 32], *CurInBuff ;

static int
ReadChars()
{
  error_code err ;
  unsigned int nb ;
  unsigned int tm ;
  signal_code dummy ;

  CurInBuff = InBuff ;
  _os_gs_ready( GpsPath, &nb ) ;
  /*  fprintf( stderr, "ReadChars: %d chars available\n", nb ) ; */
  if ( nb > 0 ) {
    if ( nb > INBUFF_SIZE ) nb = INBUFF_SIZE ;
    err = compatRead( GpsPath, InBuff, &nb ) ;
    if ( err != SUCCESS ) {
      /*      fprintf( stderr, "       Read error %d\n", err) ;
      PrintLog( LOG_WARNING, " Read Error %d\n", err) ;
      */
     return -1 ;
    }
    CharInBuff = nb ;
    /*    fprintf( stderr, "       %d chars read\n", nb ) ; */
    return SUCCESS ;
   }
  else return -1 ;
}

static int
GetChars( unsigned char *p, int nb )
{
  /*  fprintf( stderr, ">>>> Getchars %d, %x, %x\n", nb, p, CurInBuff ) ; */
  for( ; ; )
    if ( CharInBuff >= nb ) {
      memcpy( p, CurInBuff, nb ) ;
      CurInBuff += nb ;
      CharInBuff -= nb ;
      /*      fprintf( stderr, "<<<< Getchars: %d\n", nb ) ; */
      return nb ;
    }
    else {
      if ( CharInBuff != 0 ) {
	memcpy( p, CurInBuff, CharInBuff ) ;
	p += CharInBuff ;
	nb -= CharInBuff ;
	/*	fprintf( stderr, "In Getchars: %d\n", nb ) ; */
      }
      if ( ReadChars() != SUCCESS ) return -1 ;
    }
}

void ComNoEndOfCom( ){ 
  /* got CHAR_CR CHAR_LF inside a command, though not completed  */

  IOState=2;
  if ( Debug > 2 )
    fprintf(stderr,
	    "NoEndOfCom:@@%c%c %d\n",
	    IOBuffer[2],IOBuffer[3],IOCurrent);
  
}

void reinitbuf() { /* On a perdu le fil, on reinitialise tout
		      et on dumpe le buffer*/

  IOCurrent = IOState = 0;
  if ( Debug > 2 )
    fprintf(stderr,"\nioset.c: aborting. %d %d %s\n",
	    IOBuffer[2], IOBuffer[3], GpsDevice);
    
}

void io_read() {
  int i, j, k;
  double valeur;

  if ( Debug > 2 ) fprintf(stderr,"\n>>>> ioread.c- %d [%x, %d] \n",
			       IOCurrent, CurInBuff, CharInBuff );
  i=IOCurrent;
  j=i;
#if 0
  while ( read( GpsPath, IOBuffer+i, 1 ) == 1 ){
#else
    while( GetChars( IOBuffer+i, 1 ) == 1 ) {
#endif
    if ( Debug > 2 ) {
      fprintf( stderr, "%02x ", *(IOBuffer+i) ) ;
      fflush( stderr ) ;
    }
    switch (IOState) {
    case 0:
      if (IOBuffer[i]== '@' )
	IOState=1;
      break;

    case 1:
      if(IOBuffer[i]== '@' )
	IOState=2;
      else
	IOState=0;
      break;

    case 2:
      if(IOBuffer[i]==CHAR_CR )
	IOState=3;
      break;

    case 3:
      if(IOBuffer[i]==CHAR_LF)
	IOState=4;
      else if(IOBuffer[i]==CHAR_CR)
	IOState=3;
      else
	IOState=2;
      break;
    }

    if (IOState) ++IOCurrent;

    if ( IOState==4 ) {
      /* PSA - added because Motorola's protocol is awful - the gps sn contains CR/LF pairs
	 everywhere. You just need to know that it's 294 bytes */

#define GPS_SN_CUTOFF 290

      if (IOBuffer[2] == 'C' && IOBuffer[3] == 'j')
	{
	  if (IOCurrent < GPS_SN_CUTOFF)
	    {
	      IOState = 2;
	      i = IOCurrent;
	      continue;
	    }
	}
      /*fprintf( stderr, "<<<<< ioread: found after %d [%x,%d]\n",
	IOCurrent, CurInBuff, CharInBuff ) ;*/
      GpsProcess() ;
      /*fprintf( stderr, "<<<< GpsProcess [%x, %d]\n", CurInBuff, CharInBuff ) ;*/
      IOState = 0 ;
      IOCurrent = 0 ;
      return ;
    }

    if (IOCurrent>300) {
      reinitbuf();
      /* On a perdu le fil, on remet tout a 0 */
      PrintLog( LOG_WARNING, " On a perdu le fil\n" ) ;
    }
    i = IOCurrent;
  }
    
    if ( Debug > 2 ) fprintf(stderr,"<<<<< ioset.c: not finished\n");
  return;

}
