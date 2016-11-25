/*
   $Author: guglielm $
   $Revision: 1.1.1.1 $
   $Date: 2007-09-11 09:55:00 $
   $Log: not supported by cvs2svn $
   Revision 1.1.1.1  2003/12/04 21:58:23  bertou
   Initial import of all software

   Revision 1.3  2001/11/13 16:13:38  guglielm
   Many changes:
     Separated path in and path out to /tgps
     Added several signals to set the receiver state
     No longer initialize the receiver in "non hold position" mode (let it as it is)
     Use lltoutm to convert Lat/Lon to UTM
     Uses GPS_STATUS structure in Status DM.
     Cosmetics

   Revision 1.1.1.1  2001/03/28 14:33:19  revenu
   new LS

   Revision 1.3  2001/03/01 13:14:15  os9
   no message

*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int *Histo ;
static char Title[64] ;
static int Entries = 0, Uflow = 0, Oflow = 0 ;
static int HistoMiddle, HistoSize, HistoSize2 ;
static time_t Ttt ;

HistInit( int middle, int size )
{
  int i ;

  Ttt = time( NULL ) ;
  HistoMiddle = middle ;
  HistoSize = size ;
  HistoSize2 = size/2 ;
  Histo = malloc( size ) ;
  for( i = 0 ; i<HistoSize ; i++ ) Histo[i] = 0 ;
  Uflow = 0 ; Oflow = 0 ; Entries = 0 ;
}

HistAdd( int value )
{
  int index ;

  index = value - HistoMiddle + HistoSize2 ;
  if ( index < 0 ) Uflow++ ;
  else if (index >= HistoSize ) Oflow++ ;
  else {
    Histo[index]++ ;
    Entries++ ;
  }
}

HistPrint( char *fname )
{
  int i = 0 ;
  FILE *fout ;

  fout =fopen( fname, "w" ) ;
  fprintf( fout, "GPS Altitude - %s", ctime( &Ttt ) ) ;
  fprintf( fout, "Minimum: %d - Maximum: %d - Bins: %d\n",
   HistoMiddle - HistoSize2, HistoMiddle + HistoSize2,
   HistoSize ) ;
  fprintf( fout, "Entries: %d - Underflow: %d - Overflow: %d\n",
  Entries, Uflow, Oflow ) ;
  for ( ; i <HistoSize ; i++ ) {
    if ( (i%10) == 0 ) fprintf( fout, "\n" ) ;
    fprintf( fout, "%d ", Histo[i] ) ;
  }
  fprintf( fout, "\n" ) ;
  fclose( fout ) ;
}
