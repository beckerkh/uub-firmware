#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cglob.h>
#include <modes.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sg_codes.h>
#include <types.h>

/* Pour inclusion dans le schema general de l'acquisition */
#define MAIN
#include "compat.h"
#include "versions.h"

#include "ready_acq.h"
#include "msgdefs.h"
#include "dmlib.h"

#include "augererr.h"

/*#define _STATUS_MAIN_*/
/*#define _CONFIG_MAIN_*/
/*#define _MONITOR_MAIN_ */
#include "run_config.h"
/*#include "svrdm.h"*/
/*#include "fakes.h"*/

#include "siglib.h"
#include "logfile.h"

#include "gpsdefs.h"
#include "gpscommandes.h"
#include "gpsctrl.h"

/*********************************************************************

  Aug 2013: Moved signal.h to compat.h
            Moved _os_ things to compat.h

  $Author: guglielm $
  $Date: 2007-10-11 14:57:04 $
  $Revision: 1.6 $

  $Log: not supported by cvs2svn $
  Revision 1.5  2007/10/08 12:40:11  guglielm
  Cleanup

  Revision 1.4  2007/09/19 09:26:28  guglielm
  Changes in glitch info output and generic messages

  Revision 1.3  2007/09/12 14:31:35  guglielm
  Added gpsctrl PID in call to gpsirq
  Handle signal to send GLITCH info to CDAS.

  Revision 1.1.1.1  2007/09/11 09:55:00  guglielm
  First import Src

  Revision 1.2.2.3  2006/04/01 17:27:23  guglielm
  Fixed error in generic message sending (case in future/past)

  Revision 1.2.2.2  2006/04/01 13:52:17  guglielm
  Cleanup

  Revision 1.3  2006/03/25 19:27:57  guglielm
  Send a generic message when a PPS glitch has been detected (by gpsirq).
  Should no longer send 'LS in Future'

  Revision 1.2.2.1  2006/03/24 20:54:50  guglielm
  Send generic message when 1PPS glitch has been detected.
  Should no longer send 'LS in future'

  Revision 1.2  2005/08/31 16:03:04  bertou
  v0r9b3p0
  Lots of cleaning by Laurent.
  From versions.h:
  Fixed bug in msgsvrout (sending twice the same packet)
  This happens when the SU sends a message before
  the "normal" ACK. Now msgsvrout resend the messages if
  there is no ACK after a 500 millis timeout.
  Suppressed spurious generic msg in evtsvr.
  Lots of cleanup: no more LS_EA, LIMIT_SIZE.
  No more references to old VERSIONs.
  More and More Doxygen comments.
  Adapted to new Pld version:
  Scalers
  Second TOT trigger (FAST1_B)
  Baseline handling
  Added several tags in run_config.h
  Stuff for FAST1_b, Scalers
  Enables for dble bin trigger, GRB handling, base line handling.
  Modified Files:
  wmakeub Augerirq/augerirq.c Buildconfig/buildconfig.c
  Buildconfig/buildconfig.h Buildconfig/modconf.c
  Buildconfig/modconf.h Control/config2flash.c Control/control.c
  Control/do_pld.c Control/start_t1.c EvtSvr/evtsvr.c
  Gpsctrl/gps_main.c Gpsctrl/gpsctrl.c Gpsctrl/gpsprocess.c
  Gpsirq/gpsirq.c Lib/Calbuff/calbuff.c Lib/Crc/crc32.c
  Lib/Dmalib/dmaasm.c Lib/Dmalib/dmalib.c Lib/Dmalib/dmaprt.c
  Lib/FeV3lib/fev3_lib.c Lib/Felib/fe_lib.c Lib/Gpslib/gpslib.c
  Lib/Msglib/msglib.c Monitor/monitor.c MsgSvrIn/msgsvrin.c
  MsgSvrOut/msgsvrout.c MuFill/mufill.c Trigger2/ledana.c
  Trigger2/trigger2.c Trigger2/xbtrig.c
  Added Files:
  Cf2Rcf/buildconfig.h Cf2Rcf/cf2rcf.c Cf2Rcf/dowine
  Cf2Rcf/modconf.c Cf2Rcf/modconf.h Cf2Rcf/run_config.h

  Revision 1.1.1.1  2003/12/04 21:58:23  bertou
  Initial import of all software

  Revision 1.6  2002/04/11 09:02:23  guglielm
  Several changes:
  Fake message "LS in the future" should not longer occur
  Added possibility of setting GPS in hold position (see run_config.h)
  Cleanup
  Cosmetics

  Revision 1.5  2001/11/13 16:13:37  guglielm
  Many changes:
  Separated path in and path out to /tgps
  Added several signals to set the receiver state
  No longer initialize the receiver in "non hold position" mode (let it as it is)
  Use lltoutm to convert Lat/Lon to UTM
  Uses GPS_STATUS structure in Status DM.
  Cosmetics

  Revision 1.3  2001/04/20 12:41:16  guglielm
  Added signal 3004 to start gpsirq independently of the receiver
  Necessary if no receiver installed.

  Revision 1.2  2001/04/17 09:04:14  guglielm
  Good version with FAKE_GPS (but needs a real receiver !)
  Still a problem when sending Ea command to the receiver.

  Revision 1.1  2001/04/12 10:51:32  guglielm
  Main for gpsctrl.
  Works well with LS software


*********************************************************************/

/****************************************************
    Version 3 : use variable StartedByHand to simulate GPS ready (instead
    of symbol FAKE_GPS
    Use symbol FAKE_GPS in case of no GPS at all.

**************************************************/

/* #define GPSCTRL_VERSION 3 */

PROG_VERSION(GPSCTRL_VERSION);

#define MYNAME GPS

PROCESS *Pproc = NULL ;

char StrLog[256];

CONFIG *cf = NULL ;
STATUS  *st = NULL ;

#if defined(FAKE_GPS)
static char *argblk[] = { "gpsirqfake", "-sGps1pps", NULL } ;
#else
static char MyPid[16] ;
static char *argblk[] = { "gpsirq", "-sGps1pps", MyPid, NULL } ;
#endif


int SetGpsState( int hold ) ;
void io_read() ;

/*
  pps Glitch
*/
int ppsGlitchNb = 0 ;
int ppsSpurIntNb = 0;
int ppsJumpsNb = 0 ;
int bErrSendMsg = 0;
PPS_GLITCH_STATUS* PpsGlitchSt = NULL;
mh_com   *PpsGlitchStH = NULL ;


void byebye( error_code err )
{
  error_code ok ;

  if ( Pproc && Pproc->mbx_in ) {
    if ( (ok = PostClose( &Pproc->mbx_in )) != E_SUCCESS )
      PrintLog( LOG_ERROR, " Cant Close Mailbox in %s [%x] (%d)\n",
		Pproc->pnamein,Pproc->mbx_in, ok ) ;
  }
  if ( Pproc && Pproc->mbx_out ) {
    if ( (ok = PostClose( &Pproc->mbx_out)) != E_SUCCESS )
      PrintLog( LOG_ERROR, " Cant Close Mailbox out %s [%x] (%d)\n",
		Pproc->pnameout, Pproc->mbx_out, ok ) ;
  }
  /* Kill gpsirq */
  sendSignal( IrqGpsPid, SIGQUIT ) ;
  /* Unlink Data Module       */
  if ( st )  err= statuslib_Exit();  /*dm_finish( &StHeader ) ;*/
	if ( cf ) err = configlib_Exit(); /*dm_finish( &CfHeader ) ;*/
  if ( PpsGlitchSt ) dm_finish( &PpsGlitchStH );
  PrintLog( LOG_ERROR, " Ended with error= %d\n", err);
  exit( err ) ;
}

void sig_hand( int sig )
{
  PushSig( sig ) ;
  signalHandlerFinish() ;
}

void
ll2utm()
{
  /* Calculate UTMNorth and UTMEast from GpsLat and GpsLong */
}

int InitGps()
{

  if ( Debug > 1 ) fprintf(stderr, "Doing initialisation...\n"); 

  IOState = 0 ;

  if ( Debug > 1 ) fprintf(stderr, "Opening gps com...");

  if ( compatOpen( GpsDevice, S_IREAD, &GpsPath ) != 0 ) {
    PrintLog( LOG_ERROR, "Erreur ouverture %s (%d)",
	      GpsDevice, errno );
    exit(0);
  }
  if ( compatOpen( GpsDevice, S_IWRITE, &GpsPathOut ) != 0 ) {
    PrintLog( LOG_ERROR, "Erreur ouverture %s (%d)",
	      GpsDevice, errno );
    exit(0);
  }
#if defined(GPSCTRL_DEBUG)  
  PrintLog( LOG_INFO, " /tgps opened: %d in, %d out\n",
            GpsPath, GpsPathOut ); 
#endif
  LatMas = cf->GpsParams.Posit_h.Lat ;
  LongMas = cf->GpsParams.Posit_h.Long ;
  HeightMas = cf->GpsParams.Posit_h.Height ;

  if ( Debug > 1 ) {
    fprintf(stderr, " Deuxieme OK.\n"); 
    fprintf(stderr, "initialising receiver..."); 
  }

  if ( Debug > 1 ) fprintf(stderr, " Troiseme OK.\n"); 
  return(0);
}

void launch_gpsirq()
{
  error_code err ;
  u_int16 prior=0, paths=3;  /* prioriy and nb of paths to inherit */
  u_int32 edata= 0;          /* No Extra Data Space */
  char orphan= 0;            /* make this child a normal child */
  u_int16 type_lang= mktypelang( MT_PROGRAM, ML_OBJECT);

  sprintf( MyPid, "-f%d", my_pid() ) ;
  if( (err= compatExec(prior,argblk[0], argblk,&IrqGpsPid)) != SUCCESS) {
    PrintLog( LOG_ERROR, " problem forking %s\n", argblk[0]);
    exit( _errmsg( errno, StrLog));
  }
  else {
    PrintLog( LOG_INFO, " %s forked : Pid= %d\n",
	      argblk[0],IrqGpsPid );
  }
}

void
InitAcq( void )
{
  error_code err;
  int n;
  unsigned int ready_mask;
  signal_code sig;
  u_int32 nt, ndata;

  setSignalHandler( sig_hand, _glob_data );
  launch_gpsirq() ;
  /* give gpsirq a chance to get ready */
  {
    unsigned int tm = 2 ;

    compatSleep( &tm, &sig ) ;
  }
  /* Link to Data Modules */
  st = statuslib_Initialize();
  if (st == NULL) {
	  PrintLog(LOG_ERROR, "Gpsctrl : Can't create STATUS data module!\n");
	  exit(1);
  }
	cf = configlib_Initialize();
	if (cf == NULL) {
		PrintLog(LOG_ERROR,"Gpsctrl : Can't create CONFIG data module!\n");
	}
	/*
  if( dm_init( StatModName, sizeof( STATUS), (void **)&st, &StHeader))
    exit(_errmsg(errno, "Gpsctrl : Can't create STATUS data module!\n"));
	 */
	/*
  if( dm_init( ConfigModName, sizeof( CONFIG), (void **)&cf, &CfHeader))
    exit(_errmsg(errno, "Gpsctrl : Can't create CONFIG data module!\n"));
	 */
  if( dm_init( PPS_GLITCH_MODNAME, sizeof( PPS_GLITCH_STATUS ), (void **)&PpsGlitchSt, &PpsGlitchStH)) {
    PrintLog( LOG_ERROR, " Can't initialize Pps data module\n" ) ;
    exit( 1 ) ;
  }
  

  GpsSeconds = &(st->GpsSeconds) ;
  GpsCurrentSec = &(st->GpsCurrentSec) ;
  GpsTime = malloc( sizeof(struct tm) ) ;
  SawTooth = &(st->SawTooth) ;

  GpsStatus = &st->GpsState ;
  *GpsStatus = GPS_STATUS_BAD ;
  GpsTestStatus = GPS_STATUS_BAD ;
  PrevStatus = GPS_STATUS_BAD ;

  MaskAngle = cf->GpsParams.Mask ;
  GpsHeight = &cf->GpsParams.Posit_h.Height ;
  GpsLatitude = &cf->GpsParams.Posit_h.Lat ;
  GpsLongitude = &cf->GpsParams.Posit_h.Long ;

  GpsPosNorth = &st->GpsStatus.Northing ;
  *GpsPosNorth = cf->GpsParams.Posit_h.Northing ;
  GpsPosEast = &st->GpsStatus.Easting ;
  *GpsPosEast = cf->GpsParams.Posit_h.Easting ;
  GpsPosHeight = &st->GpsStatus.Height ;
  *GpsPosHeight = cf->GpsParams.Posit_h.Height ;

  GpsPosValid = &st->GpsStatus.GpsPositionValid ;
  GpsTimeValid =  &st->GpsStatus.GpsTimeValid ;

  st->GpsBadSec = 0 ;

  if(( Pproc= myprocess( MYNAME)) == NULL) {
    PrintLog( LOG_ERROR, " error calling myprocess\n", 1);
    exit( 0);
  }

  if(( err= ReadyInit( &ReadyEvId, ReadyEvName))) {
    PrintLog( LOG_ERROR, " Error %d calling ReadyInit\n", err);
    byebye( err);
  }

  /******** Create the receive mailbox *********/
  if((err= PostCreate( &Pproc->mbx_in, Pproc->pnamein, Pproc->Msize_in,
		       Pproc->notify)) &&
     (err =  PostLink( &Pproc->mbx_in, Pproc->pnamein )) ) {
    PrintLog( LOG_ERROR, " Can't Create/Link MBX %s : %d\n", 
	      Pproc->pnamein, err ) ;
      byebye( err);
  }
#if defined(GPSCTRL_DEBUG)
  else {
    PrintLog( LOG_INFO, " MBX %s Created (&%x)\n",
	      Pproc->pnamein, Pproc->mbx_in ) ;
  }
#endif  
  /******** Inform others that i am ready*******/

  if(( err= ReadyToGo( ReadyEvId, READY_GPS))) {
    PrintLog( LOG_ERROR, " Call ReadyToGo(): err= %d\n", err);
    byebye( err);
  }

  /******** Wait until Msgsvrout is ready *********/
  ready_mask= READY_MSGSVROUT ;
  if(( err= ReadyAll( ReadyEvId, &sig, ready_mask)) != E_SUCCESS) {
    PrintLog( LOG_ERROR," Unexpect sig %d call ReadyAll() : err= %d\n",
	      sig, err);
    byebye( err);
  }

  if ( (err = PostLink( &Pproc->mbx_out, Pproc->pnameout ))) {
    PrintLog( LOG_ERROR, "Can't link to MBX %s: %d\n",
	      Pproc->pnameout , err ) ;
    byebye( err);
  }
#if defined(GPSCTRL_DEBUG)
  else {
    PrintLog( LOG_ERROR, " MBX %s linked (&%x)\n",
	      Pproc->pnameout, Pproc->mbx_out ) ;
  }
#endif
  /* Intercept Signals and mask them */
  err= ReadyFinish( ReadyEvId, ReadyEvName);
  PrintLog( LOG_INFO, " Ready\n" );
}

void help()
{
  printf( "Gpsctrl V%d.%d - %s %s\n", GPSCTRL_VERSION/10,
	  GPSCTRL_VERSION % 10, __DATE__, __TIME__ ) ;
  puts( "Options:" ) ;
  puts( "  -d    : Debugging" ) ;
  puts( "          Several -d options are possible" ) ;
  exit( 1 ) ;
}

/*************************************************
  LGG 21/10/2001
  Set time for crazy GPS receiver from LS time
  Launched by signal SIG_GPS_SET_TIME
  The LS time should be set via an M_OS9_CMD message
**************************************************/
void SetGpsTime()
{
  BYTE arg[64];
  struct tm ttm ;
  time_t ttt = time( NULL ) ;

  ttm = *gmtime( &ttt ) ;
  arg[0] = 4 ;
  arg[1] = ttm.tm_mon + 1 ;
  arg[2] = ttm.tm_mday ;
  short_to_bytes( &arg[3], ttm.tm_year + 1900 ) ;
  commande( &C_date, arg ) ;
  arg[0] = 3 ;
  arg[1] = ttm.tm_hour ;
  arg[2] = ttm.tm_min ;
  arg[3] = ttm.tm_sec ;
  commande( &C_heure, arg ) ;
  PrintLog( LOG_INFO, " Set time %d/%d/%d %d:%d:%d\n",
            ttm.tm_mday, ttm.tm_mon + 1, ttm.tm_year + 1900,
            ttm.tm_hour, ttm.tm_min, ttm.tm_sec ) ;
}

/*************************************************
  LGG 21/10/2001
  Reset the GPS receiver
**************************************************/
void ResetGpsReceiver()
{
  BYTE arg[64];

  PrintLog( LOG_WARNING, " Reset Gps Receiver\n" ) ;
  arg[0] = 0 ;
  commande( &C_set_default, arg ) ;
  if ( Debug ) fprintf( stderr, "Gpsctrl: Receiver reset\n" ) ;
}

/*************************************************
  LGG 21/10/2001
  Set the GPS receiver in hold position mode
**************************************************/
void SetGpsHoldPosition()
{
  BYTE arg[64];

  arg[0] = 13 ;
  int_to_bytes( arg+1, GpsPos.poslat ) ;
  int_to_bytes( arg+5, GpsPos.poslon ) ;
  int_to_bytes( arg+9, *GpsPosHeight /*GpsPos.alt_gps*/ ) ;
  arg[13] = 0 ;
  commande( &C_position_holdp, arg ) ;

  /* 2nd enable hold position mode */
  arg[0] = 1 ;
  arg[1] = 1 ;
  commande(&C_position_hold, arg );
}

/*************************************************
  LGG 11/12/2001
  UnSet the GPS receiver in hold position mode
**************************************************/
void UnSetGpsHoldPosition()
{
  BYTE arg[64];

  /* 2nd enable hold position mode */
  arg[0] = 1 ;
  arg[1] = 0 ;
  commande(&C_position_hold, arg );
}

/*************************************************
  LGG 21/10/2001
  Set the GPS receiver position
**************************************************/
void SetGpsPosition()
{
  BYTE arg[64];

  PrintLog( LOG_INFO, "Northing: %d, Easting: %d\n",
	    cf->GpsParams.Posit_h.Northing,
	    cf->GpsParams.Posit_h.Easting ) ;
  PrintLog( LOG_INFO, "Lat: %d - Long: %d - Height: %d\n",
            LatMas, LongMas, HeightMas ) ;

  arg[0] = 4 ;
  /* Set estimated Latitude */
  int_to_bytes( arg+1, LatMas ) ;
  commande( &C_lat, arg ) ;

  /* Set estimated Longitude */
  int_to_bytes( arg+1, LongMas ) ;
  commande( &C_lon, arg ) ;

  /* Set estimated Height */
  arg[0] = 5 ;
  int_to_bytes( arg+1, HeightMas ) ;
  commande( &C_height, arg ) ;
}

/**************************
  LGG - 23/10/2001
  Send the almanach to the receiver
  Almanach data is in /dd/almanach.dat
**************************/
void SendAlmanach()
{
  FILE *fin ;
  unsigned char paquet[33] ;
  error_code ok ;
  int nb ;

  if ( access( ALMANACH_DATA_FILE, 0 ) != 0 ) {
    PrintLog( LOG_WARNING, " No almanach\n" ) ;
    return ;
  }
  PrintLog( LOG_INFO, " Sending almanach\n" ) ;
  fin = fopen( ALMANACH_DATA_FILE, "r" ) ;
  while( fread( paquet, 1, 33, fin ) == 33 ) {
    nb = 33 ;
    ok = compatWrite( GpsPathOut, paquet, &nb ) ;
    if ( ok != SUCCESS ) {
      PrintLog( LOG_ERROR, "Cant write to GPS\n" ) ;
      break ;
    }
  }
  return ;
}

/*
 * This is the message that needs to be sent to the
 * GPS to request the serial number, in hex.
 * There's no need to constantly recalculate it
 * since it's the same, once and always.
 */

u_int8 sn_msg[] = {0x40,0x40,0x43,0x6A,0x29,0xD,0xA,'\0'};
u_int32 sn_msg_len = 7;

/**************************
  PSA - 25/10/2003
  Request GPS serial number and write it to gps.sn
**************************/

void DumpGpsSerial()
{
  error_code ok;
  PrintLog(LOG_INFO, " Requesting GPS serial number.\n");
  *CurrGPSSn = 0x0;
  ok = compatWrite(GpsPathOut, sn_msg, &sn_msg_len);
  if (ok != SUCCESS ) {
    PrintLog(LOG_ERROR, "Cant write to GPS\n" );
  }
}

/**************************
  PSA - 25/10/2003
  Send GPS serial to CDAS
**************************/

#define GPS_SN_STR_LEN 32

void SendGpsSerial()
{
  char *gpssnstring;

  gpssnstring = (char *) malloc(sizeof(char)*GPS_SN_STR_LEN);

  sprintf(gpssnstring, "GPS SN: %s", CurrGPSSn);
  SendGenericMsg(gpssnstring, Pproc->mbx_out, GENERIC_MSG_VERSION);
  PrintLog(LOG_INFO, "Sending GPS SN: %s\n", CurrGPSSn);
}

/* Just for a test */
int simulFuture = 0 ;

static void sendPpsGlitchInfo()
{
  if ( PpsGlitchSt->error ){
    int i;   
    if ( !bErrSendMsg ){
      FILE* errfile;
      SendGenericMsgStr( Pproc->mbx_out, CONTROL_VERSION,
			 "PPS Glitch: 2 or more detected in sec %d. It could be a problem. We reboot",
			 PpsGlitchSt->error);
      PrintTimedLog( LOG_WARNING,
		     "PPS Glitch: 2 or more detected. It could be a problem. We reboot\n",
		     PpsGlitchSt->error);
      if ( (errfile = fopen( "errtocdas", "a+")) ){
	if ( st->ppsSpuriousInt )
	  fprintf( errfile, " %d pps spurious interrupt. Last at %d 0x%x. Pps at 0x%x. %d\n",
		   st->ppsSpuriousInt,
		   st->ppsIntLastSec,
		   st->ppsIntLast80MhzCpt, 
		   st->ppsIntPrecPps80MhzCpt,
		   (st->ppsIntLast80MhzCpt - st->ppsIntPrecPps80MhzCpt)/80 ); 

	for( i = 1; i <= PpsGlitchSt->count; i++ ){
	  fprintf( errfile, "PPS Glitch %d at %d - %d was %d [%d %d] [0x%x 0x%x %d]\n",
		   i,  PpsGlitchSt->glitches[i].sec, 
		   PpsGlitchSt->glitches[i].nano, 
		   PpsGlitchSt->glitches[i].prevnano,
		   PpsGlitchSt->glitches[i].calib, 
		   PpsGlitchSt->glitches[i].oldcalib,
		   PpsGlitchSt->glitches[i].cpt80mhz,
		   PpsGlitchSt->glitches[i].oldcpt80mhz,
		   (PpsGlitchSt->glitches[i].cpt80mhz - PpsGlitchSt->glitches[i].oldcpt80mhz)/80) ;
	}
	fclose( errfile );
      }	
      bErrSendMsg = 1;
    }
  }
  else {
    if ( ppsGlitchNb < PpsGlitchSt->count ) {
      PPS_GLITCH* pg;
      if ( PpsGlitchSt->count > PPS_MAX_SAV_GLITCH ){
	ppsGlitchNb++;
	pg = PpsGlitchSt->last;
	SendGenericMsgStr( Pproc->mbx_out, CONTROL_VERSION,
			   "PPS Glitch %d at %d - %d was %d [%d %d] [0x%x 0x%x %d]",
			   ppsGlitchNb, pg->sec, pg->nano, pg->prevnano,
			   pg->calib, pg->oldcalib, pg->cpt80mhz, pg->oldcpt80mhz, 
			   (pg->cpt80mhz - pg->oldcpt80mhz)/80 ) ;
	PrintTimedLog( LOG_WARNING,
		       "PPS Glitch %d at %d - %d was %d [%d %d] [0x%x 0x%x %d]\n",
		       ppsGlitchNb, pg->sec, pg->nano, pg->prevnano,
		       pg->calib, pg->oldcalib, pg->cpt80mhz, pg->oldcpt80mhz, 
		       (pg->cpt80mhz - pg->oldcpt80mhz)/80 ) ;
      }
      else{
	while( ppsGlitchNb < PpsGlitchSt->count ){
	  ppsGlitchNb++;
	  pg= &PpsGlitchSt->glitches[ppsGlitchNb];
	  SendGenericMsgStr( Pproc->mbx_out, CONTROL_VERSION,
			     "PPS Glitch %d at %d - %d was %d [%d %d] [0x%x 0x%x %d]",
			     ppsGlitchNb, pg->sec, pg->nano, pg->prevnano,
			     pg->calib, pg->oldcalib, pg->cpt80mhz, pg->oldcpt80mhz, 
			     (pg->cpt80mhz - pg->oldcpt80mhz)/80 ) ;
	  PrintTimedLog( LOG_WARNING,
			 "PPS Glitch %d at %d - %d was %d [%d %d] [0x%x 0x%x %d]\n",
			 ppsGlitchNb, pg->sec, pg->nano, pg->prevnano,
			 pg->calib, pg->oldcalib, pg->cpt80mhz, pg->oldcpt80mhz, 
			 (pg->cpt80mhz - pg->oldcpt80mhz)/80 ) ;  
	}
      }
    }
    else PrintLog( LOG_INFO, "No more glitch (was %d)\n", ppsGlitchNb ) ;

    if ( ppsSpurIntNb < st->ppsSpuriousInt ){
      ppsSpurIntNb++;
      SendGenericMsgStr( Pproc->mbx_out, CONTROL_VERSION,
			 " %d pps spurious interrupt. Last at %d 0x%x. Pps at 0x%x. %d",
			 st->ppsSpuriousInt,
			 st->ppsIntLastSec,
			 st->ppsIntLast80MhzCpt, 
			 st->ppsIntPrecPps80MhzCpt,
			 (st->ppsIntLast80MhzCpt -
			  st->ppsIntPrecPps80MhzCpt)/80 );
      PrintLog( LOG_WARNING,
		" %d pps spurious interrupt. Last at %d 0x%x. Pps at 0x%x. %d\n",
		st->ppsSpuriousInt,
		st->ppsIntLastSec,
		st->ppsIntLast80MhzCpt, 
		st->ppsIntPrecPps80MhzCpt,
		(st->ppsIntLast80MhzCpt - st->ppsIntPrecPps80MhzCpt)/80 );
    }
    else PrintLog( LOG_INFO, "No more spurious irq (was %d)\n",
		   ppsSpurIntNb ) ;

    if ( ppsJumpsNb != PpsGlitchSt->jumps ) {
      ppsJumpsNb = PpsGlitchSt->jumps ;
      SendGenericMsgStr( Pproc->mbx_out, CONTROL_VERSION,
			 " %d pps Jump, last at %d", ppsJumpsNb,
			 PpsGlitchSt->last_jump ) ;
      PrintLog( LOG_WARNING, " %d pps Jump, last at %d\n", ppsJumpsNb,
		PpsGlitchSt->last_jump ) ;
    }
    else PrintLog( LOG_INFO, "No more jumps (was %d)\n", ppsJumpsNb ) ;
  }
}

main( int argc, char **argv )
{
  int opt ;

  LogProgName = "Gpsctrl" ;

  while( (opt = getopt( argc, argv, "dh" )) != EOF )
    switch( opt ) {
    case 'd': Debug++ ; break ;
    default: 
      help() ;
    }

  PrintLog( LOG_INFO, " Version %d, Compiled %s %s\n",
            GPSCTRL_VERSION, __DATE__, __TIME__ ) ;

  InitAcq() ;  /* Initialize the Data Acquisition part */

  InitGps() ;
  /*
   * PSA v0r9b2p4. Dump serial number on boot, and when requested
   * via SIG_GPS_SN. Writes to "gps.sn".
   */
  DumpGpsSerial();
  SetGpsState( cf->GpsParams.HoldPosition ) ;
  InitTimeLog( &st->GpsCurrentSec ) ;

  signalSend( GpsPath, SIG_GPS_CHAR ) ;

  for( ; ; ) {
    unsigned int tm = 0 ;
    signal_code dummy, sig ;
    int nsig ;
    unsigned char *pmsg ;
    int size ;

    while( (sig = PopSig( &nsig )) != 0 ) {
      if ( sig != SIG_GPS_CHAR ) PrintLog( LOG_INFO, "Got signal %d\n", sig ) ;
      if ( sig == (signal_code)(Pproc->notify->param1) ) {
	/* Un message vient d'arriver, le traiter.
	   Pour l'instant on se contente de le jeter !
	*/
	PostReceive( Pproc->mbx_in, &pmsg, &size ) ;
	PostCancel( Pproc->mbx_in, pmsg ) ;
      }
      else switch( sig ) {
	/* Just for a test */
      case 10001:
	PrintLog( LOG_INFO, "Simulate in Future 1\n" ) ;
	simulFuture = 1 ; break ;
      case 10002:
	PrintLog( LOG_INFO, "Simulate in Future 2\n" ) ;
	simulFuture = 2 ; break ;

      case SIG_GPS_CHAR:
	GpsLocalSec = *GpsCurrentSec ;
	io_read() ;
	signalSend( GpsPath, SIG_GPS_CHAR ) ;
	break ;

      case SIG_GPS_SEND_GLITCH_INFO:
	/* pps Glitch */
	sendPpsGlitchInfo();
	PrintLog( LOG_INFO, "Glitch Info Sent\n" ) ;
	break ;

      case SIG_GPS_SEND_WARNINGS:
	SendWarnings = (SendWarnings + 1) % 2 ;
	PrintLog( LOG_INFO, " SendWarnings = %d\n", SendWarnings ) ;
	break ;

      case SIG_GPS_NO_DEBUG: Debug = 0 ;
	break ;
      case SIG_GPS_DEBUG: Debug++ ;
	break ;
      case SIG_GPS_START_FAKE:
	*GpsStatus = GPS_STATUS_OK ;
	*GpsSeconds = 1 ;
	*GpsCurrentSec = *GpsSeconds ;
	StartedByHand = 1 ;
	sendSignal( IrqGpsPid, SIG_GPS_ENABLE_PPS ) ;
	PrintLog( LOG_INFO, " Start Fake GPS %d\n", *GpsSeconds ) ;
	break ;
      case SIG_GPS_SET_OK:
	GpsTestStatus = GPS_STATUS_OK ;
	StartedByHand = 1 ;
	PrintLog( LOG_WARNING, " GPS set OK by hand\n" ) ;
	break ;
      case SIG_GPS_SET_BAD:
	GpsTestStatus = GPS_STATUS_BAD ;
	PrintLog( LOG_INFO, " Stop fake GPS (but 1PPS continues)\n" ) ;
	break ;
      case SIG_GPS_SET_TIME: SetGpsTime() ;
	break ;
      case SIG_GPS_SET_POSITION: SetGpsPosition() ;
	break ;
      case SIG_GPS_SEND_ALMANACH: SendAlmanach() ;
	break ;
      case SIG_GPS_RESET: ResetGpsReceiver() ;
	break ;
      case SIG_GPS_RESTART:
	SetGpsState( cf->GpsParams.HoldPosition ) ;
	break ;
      case SIG_GPS_SET_HOLD_POSITION:
	SetGpsHoldPosition() ;
	SendGenericMsg( "GPS in Hold Position",
			Pproc->mbx_out, GENERIC_MSG_VERSION ) ;
	PrintLog( LOG_INFO, " Hold Position: %d, %d, %d\n",
		  GpsPos.poslat, GpsPos.poslon, GpsPos.alt_gps ) ;
	break ;
      case SIG_GPS_UNSET_HOLD_POSITION:
	UnSetGpsHoldPosition() ;
	SendGenericMsg( "GPS in NON Hold Position",
			Pproc->mbx_out, GENERIC_MSG_VERSION ) ;
	PrintLog( LOG_INFO, " Unset Hold Position\n" ) ;
	break ;
      case SIG_GPS_SEND_POSITION:
	sprintf( StrLog, "GPS_POS %d %d %d %d",
		 *GpsPosNorth, *GpsPosEast, *GpsPosHeight, *GpsPosValid ) ;
	SendGenericMsg( StrLog, Pproc->mbx_out, GENERIC_MSG_VERSION ) ;
	PrintLog( LOG_INFO, " %s\n", StrLog ) ;
	break ;
      case SIG_GENERIC_MSG_SEND:
	sprintf( StrLog, "GPS_TIME  %d", *GpsCurrentSec ) ;
	SendGenericMsg( StrLog,
			Pproc->mbx_out, GENERIC_MSG_VERSION ) ;
	PrintLog( LOG_INFO, " %s\n", StrLog ) ;
	break ;
      case SIGQUIT:
      case SIGINT: byebye( sig ) ;
	break ; 
      case SIG_GPS_SN_GET:
	/*
	 * PSA v0r9b2p4. Dump serial number on boot, and when requested
	 * via SIG_GPS_SN_GET. Writes to "gps.sn".
	 * CDAS can request the GPS SN via SIG_GPS_SN_SEND.
	 */
	DumpGpsSerial();
	break;
      case SIG_GPS_SN_SEND:
	SendGpsSerial();
	break;
      default: PrintLog( LOG_INFO, " Unexpected signal %d\n", sig ) ;
      }
    }
    compatSleep( &tm, &dummy ) ;
  }
}
