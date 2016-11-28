/* Change Log

   Aug 2013: Moved signal.h to compat.h
             changed _os_ things to defs in compat.h
            

**********************************************************/


#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cglob.h>
#include <modes.h>
#include <math.h>
#include <errno.h>

#include "postlib.h"
#include "compat.h"

MailboxId LsxMbxOut ;
char *LsxMbxName = "LsxGpsMbx" ;

main( int argc, char **argv )
{
  int *ptr ;
  u_int32 tm ;
  signal_code dummy ;
  
  PostLink( &LsxMbxOut, LsxMbxName ) ;

  /* PostPut( LsxMbxOut, &PrevStatus, sizeof( int ) ) ; */
  PostAlloc( LsxMbxOut, (void **)&ptr, sizeof( int ) ) ;
  *ptr = 2 ; /* Status BAD pour commencer */
  PostSend( LsxMbxOut, ptr ) ;

  /* On attend un peu au cas ou */
  tm = 0x80000100 ;
  compatSleep( &tm, &dummy ) ;

  PostAlloc( LsxMbxOut, (void **)&ptr, sizeof( int ) ) ;
  *ptr = 0 ; /* Status OK pour terminer */
  PostSend( LsxMbxOut, ptr ) ;

  for( ; ; ) {

    tm = 0 ;
    compatSleep( &tm, &dummy ) ;
  }

}
