#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cglob.h>
#include <modes.h>
#include <math.h>
#include <errno.h>
#include <UNIX/os9def.h>


#include "compat.h"
#include "versions.h"

#include "augererr.h"

#include "msgdefs.h"
#include "dmlib.h"
#include "run_config.h"

#include "msgdefs.h"
#include "gpslib.h"

#include "gpsdefs.h"
#include "gpscommandes.h"
#include "gpsctrl.h"

#include "logfile.h"


#include "lltoutm.h"

#define GPSCTRL_DEBUG 

extern STATUS *st;
extern CONFIG *cf;

unsigned int gps_seconds( int yy, int mm, int dd, int hh, int mn, int ss ) ;
void ComNoEndOfCom() ;

char *UtmModName = LL_UTM_MOD_NAME ;
LL_UTM_CONV *UtmData ;
mh_com *UtmHeader ;

/*********************************************************************

  Aug 2013 - Moved signal.h into compat.h
             Moved _os_ things to defs in compat.h

  $Author: guglielm $
  $Date: 2007-10-08 12:40:11 $
  $Revision: 1.2 $

  $Log: not supported by cvs2svn $
  Revision 1.1.1.1  2007/09/11 09:55:00  guglielm
  First import Src

  Revision 1.4.2.2  2006/04/01 17:27:23  guglielm
  Fixed error in generic message sending (case in future/past)

  Revision 1.4.2.1  2006/04/01 13:54:00  guglielm
  Better handling of single shot LS in future/past

  Revision 1.4  2006/03/10 19:57:29  barawn
  Import of v0r9b4p0: added Cyclone compatibility, CLF handling,
  T3Already/NotFound FD bug

  Revision 1.3  2005/08/31 16:12:30  bertou
  v0r9b3p1
  resetting TT if GPS!=TT
  'GRB' scaler is now difference of scaler 1 and 2
  Scalers read on 1 PPS and no longer on 1st T1
  putting S/N info in sn.dat

   Modified Files:
   	Defs/calibx.h Defs/evbdefs.h Defs/fedefs.h Defs/feevts.h
   	Defs/fev3_defs.h Defs/logfile.h Defs/monitor.h
   	Defs/run_config.h Defs/sigauger.h Defs/status.h Defs/svrdm.h
   	Defs/versions.h Src/Control/control.c Src/Gpsctrl/gpsprocess.c
   	Src/Gpsirq/gpsirq.c Src/MiniSpy/minispy.c
   	Src/MsgSvrIn/msgsvrin.c Src/Trigger2/trigger2.c
   	Src/Trigger2/xbtrig.c

  Revision 1.1.1.1  2003/12/04 21:58:23  bertou
  Initial import of all software

  Revision 1.5  2002/04/11 09:02:23  guglielm
  Several changes:
  Fake message "LS in the future" should not longer occur
  Added possibility of setting GPS in hold position (see run_config.h)
  Cleanup
  Cosmetics

  Revision 1.4  2001/11/13 16:13:38  guglielm
  Many changes:
    Separated path in and path out to /tgps
    Added several signals to set the receiver state
    No longer initialize the receiver in "non hold position" mode (let it as it is)
    Use lltoutm to convert Lat/Lon to UTM
    Uses GPS_STATUS structure in Status DM.
    Cosmetics

  Revision 1.2  2001/04/17 09:04:32  guglielm
  Good version with FAKE_GPS (but needs a real receiver !)
  Still a problem when sending Ea command to the receiver.

  Revision 1.1  2001/04/12 10:53:01  guglielm
  Gpsctrl version for LS.
  Works well with LS software


*********************************************************************/

/* #define GPSCTRL_DEBUG  */

#if defined(_OS9000)
#define swab(x,y,n) memcpy(y,x,n)
#endif

extern char StrLog[256] ;
extern PROCESS *Pproc ;

/* Special for LS future or past */
static int isInPast = 0, isInFuture = 0 ;
extern int simulFututre ;

#if 0
void MathSecDeg(double angles, LAT_LONG *deg) {
  int i;

  i=sgn(angles);
  angles = fabs(angles);
  deg->deg = i*(int)(angles/3600.);
  deg->min = (((int)angles)%3600)/60;
  deg->sec = angles-(double)(3600*abs(deg->deg))-(double)(60*deg->min) ;
}

void MathMasDeg(long mas, LAT_LONG *deg) {
  double angles;

  angles=((double)mas)/1000.;
  MathSecDeg(angles, deg);
}
#endif

void
AddSawTooth( unsigned char value )
{
  int index ;

  index = *GpsCurrentSec & ( SAWTOOTH_MASK ) ;
  SawTooth->table[index].sawtooth = value ;
}

void traiteA()
{  
#if defined(GPSCTRL_DEBUG)
  if ( Debug ) fprintf( stderr, "Traite Ax\n" ) ;
  if ( Debug ) SendGenericMsg( "GPS: Got A msg", Pproc->mbx_out, 0 ) ;
#endif
  IOCurrent=0;
  IOState=0;
}

void traiteB()
{
  
#if defined(GPSCTRL_DEBUG)
  if ( Debug ) fprintf( stderr, "Traite B\n" ) ;
  if ( Debug ) SendGenericMsg( "GPS: Got B msg", Pproc->mbx_out, 0 ) ;
#endif
  IOCurrent=0;
  IOState=0;
}

const char *gps_serial_string = "SERIAL #   ";
const char *gps_manuf_string = "MANUFACTUR DATE ";

void traiteC() {
  /*
   *  PSA: Change for serial number output to gps.sn
   */
  FILE *fout;

#if defined(GPSCTRL_DEBUG)
  if ( Debug ) fprintf( stderr, "Traite C\n" ) ;
  if ( IOBuffer[3] == 'j' ) {
    int i = 0 ;
    char *tempbuff, *snptr, *mnptr;

    if (!(fout = fopen("gps.sn", "w")))
      {
	PrintLog(LOG_ERROR, " Can't open gps.sn for writing!\n");
	IOCurrent=0;
	IOState=0;
	return;
      }
    /*
    for( ; i < IOCurrent ; i++ ) fputc( IOBuffer[i], stderr ) ;
    */
    for(;i<IOCurrent;i++) fputc(IOBuffer[i], fout);
    fclose(fout);
    tempbuff = (char *) malloc(sizeof(char)*IOCurrent+1);
    memcpy(tempbuff, IOBuffer, IOCurrent);
    tempbuff[IOCurrent] = 0x0; /* Null terminate so strstr has a string */
    snptr = strstr(tempbuff, gps_serial_string);
    if (snptr)
      {
        FILE *f;
	mnptr = strstr(tempbuff,gps_manuf_string);
	snptr += strlen(gps_serial_string);
	snptr[NUM_GPS_SN_CHARS] = 0x0;
	strncpy(CurrGPSSn, snptr, NUM_GPS_SN_CHARS);
	PrintLog(LOG_INFO, "  GPS Serial Number is %s\n", CurrGPSSn);
	if (mnptr) {
          mnptr += strlen(gps_manuf_string);
	  mnptr[9] = 0x0;
	  f=fopen("sn.dat","a");
	  fprintf(f,"GPS Serial Number is %s %s\n", CurrGPSSn,mnptr);
	  fclose(f);
	}
      }
    else
      {
	PrintLog(LOG_ERROR, "  Couldn't find GPS serial no. in string!\n");
      }
    free(tempbuff);
  }
  /*
  if ( Debug ) SendGenericMsg( "Gpsctrl: Got C msg", Pproc->mbx_out, 0 ) ;
  */
#endif
  IOCurrent = 0 ;
  IOState = 0 ;
}

void Convert2UTM( )
{
#if GPSCTRL_VERSION<5
  char UTMZone[4] ;
  double north, east ;
  double lat, lon ;

  lat = (double)GpsPos.poslat/3600000. ;
  lon = (double)GpsPos.poslon/3600000. ;
  /* Convertir */
  strcpy( UTMZone, "19H" ) ;
  LLtoUTM( cf->GpsParams.Posit_h.Htype,
   lat, lon, &north, &east, UTMZone ) ;
  *GpsPosNorth = north*100 ;
  *GpsPosEast = east*100 ;
#else
  /* This piece of code is necessary because we dont have enough room in the
     flash eprom. The ll to UTM conversion needs almost 20 kbytes !
     It is now put in an other process "lltoutm", flashed in the other
     part of the flash
  */
  static int lltoutm_init = 0 ;

  if ( lltoutm_init == 0 ) {

    lltoutm_init = 1 ;
    /* Launch lltoutm */
    dm_init( UtmModName, sizeof( LL_UTM_CONV ),
	     (void **)&UtmData, &UtmHeader) ;
    system( "lltoutm &" ) ;

    while ( UtmData->owner == 0 ) {
      u_int32 tm = 2 ;
      signal_code dum0 ;

      compatSleep( &tm, &dum0 ) ;
    }
  }

  /* Lock the data module */
  semaP( &UtmData->exclusion ) ;
  /* Prepare data to be converted */
  UtmData->Htype = cf->GpsParams.Posit_h.Htype ;
  UtmData->LatMas = GpsPos.poslat ;
  UtmData->LonMas = GpsPos.poslon ;
  /* Start conversion */
  UtmData->rendez_vous = 0 ;
  sendSignal( UtmData->owner, SIG_LL_TO_UTM ) ;
  /* Wait till finished */
  while ( UtmData->rendez_vous == 0 ) {
    u_int32 tm = 2 ;
    signal_code dum1 ;

    compatSleep( &tm, &dum1 ) ;
  }
  /* Unlock the data module */
  semaV( &UtmData->exclusion ) ;
  *GpsPosNorth = UtmData->PosNorth ;
  *GpsPosEast = UtmData->PosEast ;
#if defined(GPSCTRL_DEBUG)
  if ( Debug > 1 ) PrintLog( LOG_INFO, " LL: %d %d / UTM: %d %d\n",
			 GpsPos.poslat, GpsPos.poslon,
			 *GpsPosNorth, *GpsPosEast ) ;
#endif

#endif
}

void traiteE()
{
  BYTE arg[32];
  int count=IOCurrent;
  int cursec ;

  switch(IOBuffer[3]) {
  case 'a':
    if (count == C_pos8.lngin) {
#if defined(GPSCTRL_DEBUG)
      if ( Debug ) PrintLog( LOG_INFO, " Got Pos-8\n" ) ;

      if ( Debug > 2 ) {
	int i = 0 ;
	fprintf( stderr, "\nLongueur: %d\n", count ) ;
	for( ; i<count ; i++ ) {
	  if ( (i % 16 ) == 0) fprintf( stderr, "\n" ) ;
	  fprintf( stderr, "%02x ", IOBuffer[i] ) ;
	}
	fprintf( stderr, "\n" ) ;
      }
#endif
      IOCurrent=0;
      IOState=0;
      memcpy( &(GpsPos), IOBuffer+4, 7 ) ;
      memcpy( &(GpsPos.fract_sec), IOBuffer+11, 26 ) ;
      memcpy( &(GpsPos.DOP_type), IOBuffer+37, 36 ) ;
#if GPSCTRL_VERSION>4
      /* Set position validity according to receiver status */
      /* GpsPos.poslat, GpsPos.poslon, GpsPos.alt_gps )
	 transform MAS into Northing/easting
	 and fill the relevant structure in status data module
      */
      Convert2UTM() ;
      *GpsPosHeight = GpsPos.alt_gps ;
#if STATUS_MODULE_VERSION>0
      if ( ( GpsPos.rec_status & GPS_POSITION_VALID_MASK ) == 0 )
	*GpsPosValid = 1 ;
      else *GpsPosValid = 0 ;
#endif
#endif
      GpsTime->tm_sec=GpsPos.sec;
      GpsTime->tm_min=GpsPos.min;
      GpsTime->tm_hour=GpsPos.heure;
      GpsTime->tm_mday=GpsPos.jour;
      GpsTime->tm_mon=GpsPos.mois-1;
      GpsTime->tm_year=GpsPos.annee-1900;

      cursec = gps_seconds( 
			   GpsTime->tm_year + 1900,
			   GpsTime->tm_mon+1,
			   GpsTime->tm_mday,
			   GpsTime->tm_hour,
			   GpsTime->tm_min,
			   GpsTime->tm_sec ) ;
#if defined(GPSCTRL_DEBUG)
      if ( Debug ) {
	PrintLog( LOG_INFO, " Time %d (%d) - Vis: %d - Trk: %d\n",
		  cursec, GpsOKCount,
		  GpsPos.sat_vis, GpsPos.sat_trac ) ;
	PrintLog( LOG_INFO, " ll: %d - LL: %d - Hh: %d\n",
		 GpsPos.poslat, GpsPos.poslon, GpsPos.alt_gps ) ;

#if 0
	fprintf( stderr, "Gpsctrl: Time: %d (%d/%d/%d %d:%d:%d\n\t - Vis %d - Tracked %d\n",
		 cursec, GpsTime->tm_year+1900,
		 GpsTime->tm_mon+1, GpsTime->tm_mday,
		 GpsTime->tm_hour, GpsTime->tm_min, GpsTime->tm_sec,
		 GpsPos.sat_vis, GpsPos.sat_trac ) ;
	fprintf( stderr, "       : Lat: %d - Long: %d - Height: %d\n",
		 GpsPos.poslat, GpsPos.poslon, GpsPos.alt_gps ) ;
#endif
      }
#endif
      if ( SetCurrentTime ) {
	SetCurrentTime = 0 ;
	*GpsSeconds = cursec ;
	*GpsCurrentSec = *GpsSeconds ;
	/* Now reset the TTAG board, to make sure that the seconds counter
	   is at 0
	*/
	GPS_reset() ; /* Reset here - LGG 21/10/2001 */
	sendSignal( IrqGpsPid, SIG_GPS_ENABLE_PPS ) ;
	PrintLog( LOG_INFO, " Set up time %d (%d/%d/%d %d:%d:%d)\n",
		  *GpsSeconds, GpsTime->tm_year+1900,
		  GpsTime->tm_mon+1, GpsTime->tm_mday,
		  GpsTime->tm_hour, GpsTime->tm_min, GpsTime->tm_sec ) ;
	  /* Wait one more second, and Send a signal to Control.
	     Control should then send an M_READY Message to Cdas
	  */
	sprintf( StrLog, "setime %02d%02d%02d%02d%02d%02d",
		 GpsTime->tm_year % 100, GpsTime->tm_mon+1, GpsTime->tm_mday,
		 GpsTime->tm_hour, GpsTime->tm_min, GpsTime->tm_sec ) ;
	system( StrLog ) ;
	/*PrintLog( LOG_INFO, " %s\n", StrLog ) ;*/
	if ( !st->GpsBadSec ) sendSignal( getppid(), SIG_GPS_READY ) ;
	else sendSignal( getppid(), SIG_GPS_READY_AGAIN ) ;
      }
      else if ( PrevStatus == GPS_STATUS_OK &&
		*GpsStatus == GPS_STATUS_OK ) {
	/* Check any discrepancy between Gps time and GpsCurrentSec */
	if ( cursec > GpsLocalSec ) {
	  isInPast++ ;
	  if ( (isInPast % 20) == 2 ) {
	    /* send message only SECOND time (because it can be
	       cleared the next second if it's not "real".
	       Then every 20 secondes 
	    */
	    SendGenericMsg( "GPS: LS in past", Pproc->mbx_out, 0 ) ;
	    PrintLog( LOG_WARNING,
		      "LS in Past, %d < %d\n", *GpsCurrentSec, cursec ) ;
	  }
	  /* Something wrong, return to bad (high) frequency
	     Only the first time is necessary */
	  if ( isInPast == 1 ) {
	    arg[0]=1;
	    arg[1]= Pos8FrequencyBAD ;
	    commande(&C_pos8, arg );
	  }
	}
	else if ( cursec < GpsLocalSec ) {
	  isInFuture++ ;
	  if ( (isInFuture % 20 ) == 2 ) {
	    /* send message only SECOND time (because it can be
	       cleared the next second if it's not "real".
	       Then every 20 secondes 
	    */
	    SendGenericMsgStr( Pproc->mbx_out, 0, "GPS: LS in future %d-%d",
			       cursec, GpsLocalSec ) ;
	    PrintLog( LOG_WARNING, " LS in Future, %d > %d\n",
		      *GpsCurrentSec, cursec ) ;
	  }
	  /* Something wrong, return to bad (high) frequency
	     Only the first time is necessary */
	  if ( isInFuture == 1 ) {
	    arg[0]=1;
	    arg[1]= Pos8FrequencyBAD ;
	    commande(&C_pos8, arg );
	  }
	}
	else {
	  char *str = "" ;

	  if ( isInPast != 0 || isInFuture != 0 ) {
	    if ( isInPast != 0 ) str = ", Was in Past" ;
	    else if ( isInFuture != 0 ) str = ", Was in Future" ;
	    sprintf( StrLog, "GPS: LS OK %d%s", cursec, str ) ;
	    SendGenericMsg( StrLog, Pproc->mbx_out, 0 ) ;
	    PrintLog( LOG_INFO, " LS Ok %d%s\n", cursec, str ) ;
	    /* Everything back OK, return to normal (low) frequency */
	    arg[0]=1;
	    arg[1]= Pos8FrequencyOK ;
	    commande(&C_pos8, arg );
	  }
	  else if ( SendWarnings ) {
	    sprintf( StrLog, "GPS: LS OK %d%s", cursec, str ) ;
	    SendGenericMsg( StrLog, Pproc->mbx_out, 0 ) ;
	  }
	  isInPast = 0 ;
	  isInFuture = 0 ;
	}
      }
    }
    else IOState=2; 
    break;
  case 'k':
    if (count==C_extpos8.lngin){
      IOCurrent=0;
      IOState=0;
    }
    else IOState=2; 
    break;
  case 'g':
    if ( count==C_satrange8.lngin ) {
      IOCurrent=0 ;
      IOState=0 ;
    }
    else IOState = 2 ; 
    break ;
  case 'n':
    /* Trame TRAIM */
    if (count == C_Traim8.lngin) {
      IOCurrent=0;
      IOState=0;
      memcpy( &GpsTraim, IOBuffer+4, 19 ) ;
      memcpy( &GpsTraim.one_sigma, IOBuffer + 23, 43 ) ;
#if defined(GPSCTRL_DEBUG)
      if ( Debug ) {
	fprintf( stderr, " 1PPS mode: %d - 1PPS status: %d - 1PPS sync: %d\n",
		 GpsTraim.ppsmode, GpsTraim.ppstatus, GpsTraim.ppsync ) ;
	fprintf( stderr, " TRAIM solution: %d - TRAIM status: %d\n",
		 GpsTraim.traim_solution, GpsTraim.traim_status ) ;
	fprintf( stderr, " Saw Tooth: %d\n", (char)GpsTraim.sawtooth ) ;
	if ( Debug > 1 ) fprintf(stderr,"\n N  ID  M  S/N  St   Elev\n\n"); 
      }
      if ( Debug ) {
	if ( GpsTraim.traim_solution == 0 )
	  fprintf( stderr, "    GPS OK\n" ) ;
	else if ( GpsTraim.traim_solution == 1 )
	  fprintf( stderr, "GPS Almost OK\n" ) ;
	else fprintf( stderr, "    GPS Bad\n" ) ;
      }
#endif
#if GPSCTRL_VERSION>4
#if STATUS_MODULE_VERSION>0
      /* Save Time validity according to One Sigma */
      if ( GpsTraim.one_sigma <= GPS_TIME_VALIDITY ) *GpsTimeValid = 1 ;
      else *GpsTimeValid = 0 ;
#endif
#endif

      if ( StartedByHand ) GpsTraim.traim_solution = GpsTestStatus ;

      if ( GpsTraim.traim_solution == GPS_STATUS_OK &&
	   PrevStatus != GPS_STATUS_OK ) {
	/* Le GPS devient OK.
	   Attendre encore un coup avant de le faire savoir.
	*/
	GpsOKCount = 0 ;
	PrevStatus = GpsTraim.traim_solution ;
#if defined(GPSCTRL_DEBUG)
	PrintLog( LOG_INFO, "GPS Becoming OK\n" ) ;
#endif
      }
      else if ( GpsTraim.traim_solution != GPS_STATUS_OK &&
		PrevStatus == GPS_STATUS_OK ) {
	/* Le GPS devient BAD.
	   Changer la frequence des trames POS8
	*/
	*GpsStatus = GpsTraim.traim_solution ;
	PrevStatus = *GpsStatus ;
	arg[0]=1;
	arg[1]= Pos8FrequencyBAD ;
	commande(&C_pos8, arg );
	PrintLog( LOG_INFO, " GPS Bad at %d\n", *GpsCurrentSec ) ;
	st ->GpsBadSec = *GpsCurrentSec ;
      }
      else if ( GpsTraim.traim_solution == GPS_STATUS_OK &&
		PrevStatus == GPS_STATUS_OK ) {
	/* Ranger le sawtooth au bon endroit */
	if ( GpsOKCount == GPS_OK_COUNT_MIN ) {
	  /* AddSawTooth( GpsTraim.sawtooth ) ; Not useful here */
	  arg[0]=1;
	  arg[1]= Pos8FrequencyOK ;
	  commande( &C_pos8, arg ) ;
	  PrintLog( LOG_INFO, "Setting Frequency at %d\n", arg[1] ) ;
	  GpsOKCount++ ;
	  SetCurrentTime = 1 ;
	  PrintLog( LOG_INFO, " Gps says OK (%d)\n", GpsOKCount ) ;
#if GPSCTRL_VERSION > 7
      PrintLog( LOG_INFO, " Gps offset is %d\n", cf->GpsParams.GPSoffset);
#endif
	}
	else if ( GpsOKCount > GPS_OK_COUNT_MIN ) {
	  *GpsStatus = GpsTraim.traim_solution ;
	  PrevStatus = *GpsStatus ;
	  AddSawTooth( GpsTraim.sawtooth ) ;
	}
	else {
#if defined(GPSCTRL_DEBUG)
	  PrintLog( LOG_INFO, " Gps almost OK (%d)\n", GpsOKCount ) ;
#endif
	  PrevStatus = GpsTraim.traim_solution ;
	  GpsOKCount++ ;
	}
      }
    }
    else {
      if ( Debug > 1 ) fprintf( stderr, "   Bad count\n" ) ;
      IOState=2; 
    }
    break;
  case 's':
    if (count==C_measure.lngin){
      IOCurrent=0;
      IOState=0;
      /*traistan(io);           ********* Put your  measurement processing here ***** */
    }
    else ComNoEndOfCom();
    break;
  }
}


void traiteH()
{
  BYTE arg[32];
  int count=IOCurrent;
  int cursec ;

  switch(IOBuffer[3]) {
  case 'a':
    if (count == C_pos12.lngin) {
#if defined(GPSCTRL_DEBUG)
      if ( Debug ) PrintLog( LOG_INFO, " Got Pos-8\n" ) ;

      if ( Debug > 2 ) {
	int i = 0 ;
	fprintf( stderr, "\nLongueur: %d\n", count ) ;
	for( ; i<count ; i++ ) {
	  if ( (i % 16 ) == 0) fprintf( stderr, "\n" ) ;
	  fprintf( stderr, "%02x ", IOBuffer[i] ) ;
	}
	fprintf( stderr, "\n" ) ;
      }
#endif
      IOCurrent=0;
      IOState=0;
      memcpy( &(GpsPos), IOBuffer+4, 7 ) ;
      memcpy( &(GpsPos.fract_sec), IOBuffer+11, 26 ) ;
      memcpy( &(GpsPos.cDOP), IOBuffer+37, 117 ) ;  //we could probably do without some of these bytes. elephant.
#if GPSCTRL_VERSION>4
      /* Set position validity according to receiver status */
      /* GpsPos.poslat, GpsPos.poslon, GpsPos.alt_gps )
	 transform MAS into Northing/easting
	 and fill the relevant structure in status data module
      */
      Convert2UTM() ;
      *GpsPosHeight = GpsPos.alt_gps ;
#if STATUS_MODULE_VERSION>0
      if ( ( GpsPos.rec_status & GPS_POSITION_VALID_MASK ) == 0 )
	*GpsPosValid = 1 ;
      else *GpsPosValid = 0 ;
#endif
#endif
      GpsTime->tm_sec=GpsPos.sec;
      GpsTime->tm_min=GpsPos.min;
      GpsTime->tm_hour=GpsPos.heure;
      GpsTime->tm_mday=GpsPos.jour;
      GpsTime->tm_mon=GpsPos.mois-1;
      GpsTime->tm_year=GpsPos.annee-1900;

      cursec = gps_seconds(
			   GpsTime->tm_year + 1900,
			   GpsTime->tm_mon+1,
			   GpsTime->tm_mday,
			   GpsTime->tm_hour,
			   GpsTime->tm_min,
			   GpsTime->tm_sec ) ;
#if defined(GPSCTRL_DEBUG)
      if ( Debug ) {
	PrintLog( LOG_INFO, " Time %d (%d) - Vis: %d - Trk: %d\n",
		  cursec, GpsOKCount,
		  GpsPos.sat_vis, GpsPos.sat_trac ) ;
	PrintLog( LOG_INFO, " ll: %d - LL: %d - Hh: %d\n",
		 GpsPos.poslat, GpsPos.poslon, GpsPos.alt_gps ) ;

#if 0
	fprintf( stderr, "Gpsctrl: Time: %d (%d/%d/%d %d:%d:%d\n\t - Vis %d - Tracked %d\n",
		 cursec, GpsTime->tm_year+1900,
		 GpsTime->tm_mon+1, GpsTime->tm_mday,
		 GpsTime->tm_hour, GpsTime->tm_min, GpsTime->tm_sec,
		 GpsPos.sat_vis, GpsPos.sat_trac ) ;
	fprintf( stderr, "       : Lat: %d - Long: %d - Height: %d\n",
		 GpsPos.poslat, GpsPos.poslon, GpsPos.alt_gps ) ;
#endif
      }
#endif
      if ( SetCurrentTime ) {
	SetCurrentTime = 0 ;
	*GpsSeconds = cursec ;
	*GpsCurrentSec = *GpsSeconds ;
	/* Now reset the TTAG board, to make sure that the seconds counter
	   is at 0
	*/
	GPS_reset() ; /* Reset here - LGG 21/10/2001 */
	sendSignal( IrqGpsPid, SIG_GPS_ENABLE_PPS ) ;
	PrintLog( LOG_INFO, " Set up time %d (%d/%d/%d %d:%d:%d)\n",
		  *GpsSeconds, GpsTime->tm_year+1900,
		  GpsTime->tm_mon+1, GpsTime->tm_mday,
		  GpsTime->tm_hour, GpsTime->tm_min, GpsTime->tm_sec ) ;
	  /* Wait one more second, and Send a signal to Control.
	     Control should then send an M_READY Message to Cdas
	  */
	sprintf( StrLog, "setime %02d%02d%02d%02d%02d%02d",
		 GpsTime->tm_year % 100, GpsTime->tm_mon+1, GpsTime->tm_mday,
		 GpsTime->tm_hour, GpsTime->tm_min, GpsTime->tm_sec ) ;
	system( StrLog ) ;
	/*PrintLog( LOG_INFO, " %s\n", StrLog ) ;*/
	if ( !st->GpsBadSec ) sendSignal( getppid(), SIG_GPS_READY ) ;
	else sendSignal( getppid(), SIG_GPS_READY_AGAIN ) ;
      }
      else if ( PrevStatus == GPS_STATUS_OK &&
		*GpsStatus == GPS_STATUS_OK ) {
	/* Check any discrepancy between Gps time and GpsCurrentSec */
	if ( cursec > GpsLocalSec ) {
	  isInPast++ ;
	  if ( (isInPast % 20) == 2 ) {
	    /* send message only SECOND time (because it can be
	       cleared the next second if it's not "real".
	       Then every 20 secondes
	    */
	    SendGenericMsg( "GPS: LS in past", Pproc->mbx_out, 0 ) ;
	    PrintLog( LOG_WARNING,
		      "LS in Past, %d < %d\n", *GpsCurrentSec, cursec ) ;
	  }
	  /* Something wrong, return to bad (high) frequency
	     Only the first time is necessary */
	  if ( isInPast == 1 ) {
	    arg[0]=1;
	    arg[1]= Pos8FrequencyBAD ;
	    commande(&C_pos12, arg );
	  }
	}
	else if ( cursec < GpsLocalSec ) {
	  isInFuture++ ;
	  if ( (isInFuture % 20 ) == 2 ) {
	    /* send message only SECOND time (because it can be
	       cleared the next second if it's not "real".
	       Then every 20 secondes
	    */
	    SendGenericMsgStr( Pproc->mbx_out, 0, "GPS: LS in future %d-%d",
			       cursec, GpsLocalSec ) ;
	    PrintLog( LOG_WARNING, " LS in Future, %d > %d\n",
		      *GpsCurrentSec, cursec ) ;
	  }
	  /* Something wrong, return to bad (high) frequency
	     Only the first time is necessary */
	  if ( isInFuture == 1 ) {
	    arg[0]=1;
	    arg[1]= Pos8FrequencyBAD ;
	    commande(&C_pos12, arg );
	  }
	}
	else {
	  char *str = "" ;

	  if ( isInPast != 0 || isInFuture != 0 ) {
	    if ( isInPast != 0 ) str = ", Was in Past" ;
	    else if ( isInFuture != 0 ) str = ", Was in Future" ;
	    sprintf( StrLog, "GPS: LS OK %d%s", cursec, str ) ;
	    SendGenericMsg( StrLog, Pproc->mbx_out, 0 ) ;
	    PrintLog( LOG_INFO, " LS Ok %d%s\n", cursec, str ) ;
	    /* Everything back OK, return to normal (low) frequency */
	    arg[0]=1;
	    arg[1]= Pos8FrequencyOK ;
	    commande(&C_pos12, arg );
	  }
	  else if ( SendWarnings ) {
	    sprintf( StrLog, "GPS: LS OK %d%s", cursec, str ) ;
	    SendGenericMsg( StrLog, Pproc->mbx_out, 0 ) ;
	  }
	  isInPast = 0 ;
	  isInFuture = 0 ;
	}
      }
    }
    else IOState=2;
    break;
  case 'k':
    if (count==C_extpos8.lngin){
      IOCurrent=0;
      IOState=0;
    }
    else IOState=2;
    break;
  case 'g':
    if ( count==C_satrange8.lngin ) {
      IOCurrent=0 ;
      IOState=0 ;
    }
    else IOState = 2 ;
    break ;
  case 'n':
    /* Trame TRAIM */
    if (count == C_Traim8.lngin) {
      IOCurrent=0;
      IOState=0;
      memcpy( &GpsTraim, IOBuffer+4, 71 ) ;
//      memcpy( &GpsTraim.one_sigma, IOBuffer + 23, 43 ) ;
#if defined(GPSCTRL_DEBUG)
      if ( Debug ) {
	fprintf( stderr, " 1PPS status: %d - 1PPS sync: %d\n",
		 GpsTraim.ppstatus, GpsTraim.ppsync ) ;
	fprintf( stderr, " TRAIM solution: %d - TRAIM status: %d\n",
		 GpsTraim.traim_solution, GpsTraim.traim_status ) ;
	fprintf( stderr, " Saw Tooth: %d\n", (char)GpsTraim.sawtooth ) ;
	if ( Debug > 1 ) fprintf(stderr,"\n N  ID  M  S/N  St   Elev\n\n");
      }
      if ( Debug ) {
	if ( GpsTraim.traim_solution == 0 )
	  fprintf( stderr, "    GPS OK\n" ) ;
	else if ( GpsTraim.traim_solution == 1 )
	  fprintf( stderr, "GPS Almost OK\n" ) ;
	else fprintf( stderr, "    GPS Bad\n" ) ;
      }
#endif
#if GPSCTRL_VERSION>4
#if STATUS_MODULE_VERSION>0
      /* Save Time validity according to One Sigma */
      if ( GpsTraim.one_sigma <= GPS_TIME_VALIDITY ) *GpsTimeValid = 1 ;
      else *GpsTimeValid = 0 ;
#endif
#endif

      if ( StartedByHand ) GpsTraim.traim_solution = GpsTestStatus ;

      if ( GpsTraim.traim_solution == GPS_STATUS_OK &&
	   PrevStatus != GPS_STATUS_OK ) {
	/* Le GPS devient OK.
	   Attendre encore un coup avant de le faire savoir.
	*/
	GpsOKCount = 0 ;
	PrevStatus = GpsTraim.traim_solution ;
#if defined(GPSCTRL_DEBUG)
	PrintLog( LOG_INFO, "GPS Becoming OK\n" ) ;
#endif
      }
      else if ( GpsTraim.traim_solution != GPS_STATUS_OK &&
		PrevStatus == GPS_STATUS_OK ) {
	/* Le GPS devient BAD.
	   Changer la frequence des trames POS8
	*/
	*GpsStatus = GpsTraim.traim_solution ;
	PrevStatus = *GpsStatus ;
	arg[0]=1;
	arg[1]= Pos8FrequencyBAD ;
	commande(&C_pos8, arg );
	PrintLog( LOG_INFO, " GPS Bad at %d\n", *GpsCurrentSec ) ;
	st ->GpsBadSec = *GpsCurrentSec ;
      }
      else if ( GpsTraim.traim_solution == GPS_STATUS_OK &&
		PrevStatus == GPS_STATUS_OK ) {
	/* Ranger le sawtooth au bon endroit */
	if ( GpsOKCount == GPS_OK_COUNT_MIN ) {
	  /* AddSawTooth( GpsTraim.sawtooth ) ; Not useful here */
	  arg[0]=1;
	  arg[1]= Pos8FrequencyOK ;
	  commande( &C_pos8, arg ) ;
	  PrintLog( LOG_INFO, "Setting Frequency at %d\n", arg[1] ) ;
	  GpsOKCount++ ;
	  SetCurrentTime = 1 ;
	  PrintLog( LOG_INFO, " Gps says OK (%d)\n", GpsOKCount ) ;
#if GPSCTRL_VERSION > 7
      PrintLog( LOG_INFO, " Gps offset is %d\n", cf->GpsParams.GPSoffset);
#endif
	}
	else if ( GpsOKCount > GPS_OK_COUNT_MIN ) {
	  *GpsStatus = GpsTraim.traim_solution ;
	  PrevStatus = *GpsStatus ;
	  AddSawTooth( GpsTraim.sawtooth ) ;
	}
	else {
#if defined(GPSCTRL_DEBUG)
	  PrintLog( LOG_INFO, " Gps almost OK (%d)\n", GpsOKCount ) ;
#endif
	  PrevStatus = GpsTraim.traim_solution ;
	  GpsOKCount++ ;
	}
      }
    }
    else {
      if ( Debug > 1 ) fprintf( stderr, "   Bad count\n" ) ;
      IOState=2;
    }
    break;
  case 's':
    if (count==C_measure.lngin){
      IOCurrent=0;
      IOState=0;
      /*traistan(io);           ********* Put your  measurement processing here ***** */
    }
    else ComNoEndOfCom();
    break;
  }
}


void GpsProcess() {
  /*  int out;
      int count = IOCurrent;
  */

#if defined(GPSCTRL_DEBUG)
  if ( Debug ) fprintf(stderr, "GpsProcess: %c%c - %d char\n",
		       IOBuffer[2],IOBuffer[3],
		       IOCurrent );
#endif
  switch(IOBuffer[2]) {
  case 'A':
    traiteA();
    break;
  case 'B':
    traiteB();
    break;
  case 'C':
    traiteC();
    break;
  case 'E':
    traiteE();
    break;
  case 'H':
    traiteH();
    break;
    
  }
  
#if defined(GPSCTRL_DEBUG)
  if(!IOState) {
    if ( Debug ) fprintf(stderr,"@@%c%c\n ",IOBuffer[2],IOBuffer[3]);
  }
#endif
}
