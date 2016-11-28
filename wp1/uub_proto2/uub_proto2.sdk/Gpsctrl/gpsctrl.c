#if !defined(_OS9000)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#else
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cglob.h>
#include <modes.h>
#include <math.h>
#include <errno.h>
#endif

/* Pour inclusion dans le schema general de l'acquisition */

#include "msgdefs.h"
#include "ready_acq.h"
#include "augererr.h"
#define _STATUS_MAIN_
#define _CONFIG_MAIN_
#define _MONITOR_MAIN_
#include "run_config.h"
#include "svrdm.h"
#include "fakes.h"

#include "postlib.h"

#include "gpsconfig.h"
#include "gpsdefs.h"
#include "gpscommandes.h"
#include "gpsctrl.h"
#include "compat.h"

/*********************************************************************

  Aug 2013:
  moved signal.h header into compat.h, 
  changed _os_ things to defs in compat.h

  $Author: guglielm $
  $Date: 2007-09-11 09:55:00 $
  $Revision: 1.1.1.1 $

  $Log: not supported by cvs2svn $
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

  Revision 1.7  2001/11/13 16:13:37  guglielm
  Many changes:
    Separated path in and path out to /tgps
    Added several signals to set the receiver state
    No longer initialize the receiver in "non hold position" mode (let it as it is)
    Use lltoutm to convert Lat/Lon to UTM
    Uses GPS_STATUS structure in Status DM.
    Cosmetics

  Revision 1.5  2001/04/17 09:04:20  guglielm
  Good version with FAKE_GPS (but needs a real receiver !)
  Still a problem when sending Ea command to the receiver.

  Revision 1.4  2001/03/29 08:20:45  guglielm
  Cosmetics (and test of CVS)


*********************************************************************/

#define NBLS (13)    /* Nombre de leap sec entre GPS et UTC pour approximer UTC quand on
n'a pas encore chopé la page 18 de la subframe 4 */
#define DEBGPS (44244)

#if defined(_OS9000)
#define swab(x,y,n) memcpy(y,x,n)
#define SIG_GPS_CHAR 12345
#endif

char isinsf4[25]={0,0,25,26,27,28,0,29,30,31,32};
/* les pages 2-5 et 7-10 de subframe 4 contiennent des almanachs */


subframe sf; /* 32 pages d'almanach, 1 par satellite. les 24 premières viennent de 
subframe 5, p1-24, les 8 dernieres de subframe 4, p 2-5 et 7-10
On commence à 1. sf[0] n'est pas utilisé */


int Debug = 0 ;
int ResetReceiver = 0 ;
int HoldPosition = 0 ;
int InitReceiver=1;
int shifted=0;
int Survey=0;
int last;
com_info cominfo;
sat_info sat[33];
trk_info trk;
int mapchannel[33];

int elev[12];
int azim[12];
int masquelocal;
int  lines, cols;

BYTE visinfo=0;

FILE *erreur, *SORTIE, *sortie, *data, *rawdata; 

char buffer[512];

ioset gps; /* File descripteurs pour les gps */ 

int    maxfd;     /* maximum file desciptor used */

ttraim traim;
pos POSITION;
VIS_SAT vis_sat;
point pos_rec;

GPS_STATUS GpsStatus ;
int LongMas, LatMas, HeightMas ;
unsigned char MaskAngle = 15, PpsOffset = 13 ;
static int Histo = 0, SaveHisto = 0 ;
int Listen = 1 ; /* Just listening to the GPS, no command sent */
int PrevStatus = GPS_STATUS_BAD ;
int Lsx = 0 ; /* If 1, means launched by the LSX. In that case try to link
 to the GpsCtrlMbx. If success send a message each time
 the Traim solution changes.
      */
int Frequency = 5 ; /* Frequency of @@Ea answers */
int VisibleReq = 1 ; /* Send @@Bb on request/at any change */

char HFname[132] ;
FILE *Hfout = NULL ;

/******************************* special Acq ***********************/

CONFIG *cf ;
STATUS  *st ;

void
InitAcq( void )
{
  error_code err;
  int n;
  unsigned int ready_mask;
  PROCESS *pproc;
  signal_code sig;
  u_int32 nt, ndata;

  setSignalHandler( sig_hand, _glob_data );
                                       /* Link to Data Modules */

  if( dm_init( StatModName, sizeof( STATUS), (void **)&StData, &StHeader))
    exit(_errmsg(errno, "Gpsctrl : Can't create STATUS data module!\n"));
  if( dm_init( ConfigModName, sizeof( CONFIG), (void **)&CfData, &CfHeader))
    exit(_errmsg(errno, "Gpsctrl : Can't create CONFIG data module!\n"));

  st= StData;
  cf= CfData;

  if(( err= ReadyInit( &ReadyEvId, ReadyEvName))) {
    sprintf( StrLog, "Gpsctrl : Error %d calling ReadyInit\n", err);
    PrtLog( StrLog, 1);
    byebye( err);
  }

  /******** Create the mailbox *********/

  if(( err= ReadyToGo( ReadyEvId, READY_GPS))) {
    sprintf( StrLog, "Gpsctrl: Call ReadyToGo(): err= %d\n", err);
    PrtLog( StrLog, 1);
    byebye( err);
  }
  ready_mask= READY_CONTROL ;
  if(( err= ReadyAll( ReadyEvId, &sig, ready_mask)) != E_SUCCESS) {
    sprintf( StrLog, "Gpsctrl : Unexpect sig %d call ReadyAll() : err= %d\n",
     sig, err);
    PrtLog( StrLog, 1);
    byebye( err);
  }
                                       /* Intercept Signals and mask them */
  err= ReadyFinish( ReadyEvId, ReadyEvName);
  printf( "Gpsctrl : init ended...\n"); fflush( stdout);
}

void show_status()
{
  fprintf( stderr, " Date: %d/%d/%d %d:%d:%d\n",
   GpsStatus.temps.tm_mday,
   GpsStatus.temps.tm_mon+1,
   GpsStatus.temps.tm_year,
   GpsStatus.temps.tm_hour,
   GpsStatus.temps.tm_min,
   GpsStatus.temps.tm_sec ) ;
  fprintf( stderr, " Secondes GPS: %d\n", GpsStatus.secondes ) ;
  fprintf( stderr, " Longitude: %d:%d:%6.2f\n",
   GpsStatus.longitude.deg,
   GpsStatus.longitude.min,
   GpsStatus.longitude.sec ) ;
  fprintf( stderr, " Latitude : %d:%d:%6.2f\n",
   GpsStatus.latitude.deg,
   GpsStatus.latitude.min,
   GpsStatus.latitude.sec ) ;
  fprintf( stderr, " Altitude: %8.2f\n", GpsStatus.height ) ;
  fprintf( stderr, " Sat Vis: %d - Tracked: %d\n",
   POSITION.sat_vis, POSITION.sat_trac ) ;
}

#if 0
int MathKepler(double mk, double e, double *ek, double eps, int max){
  double ek0;
  int compteur=0;

  *ek=mk;
  while(fabs(*ek-ek0)>eps && compteur <max) {
    ek0=*ek;
    *ek=mk+e*sin(ek0);
    ++compteur;
  }
  if (compteur<max) 
    return(1);
  else 
    return (0);
}
#endif
 
void MathSecDeg(double angles, angled *deg) {
  int i;

  i=sgn(angles);
  angles = fabs(angles);
  deg->deg = i*(int)(angles/3600.);
  deg->min = (((int)angles)%3600)/60;
  deg->sec = angles-(double)(3600*abs(deg->deg))-(double)(60*deg->min) ;
}

void MathRadDeg(double rad, angled *deg) {
  double angles;

  angles=rad/degrad;
  MathSecDeg(angles, deg);
}

void MathRadMas(double rad, long *mas) {
  double angles;

  *mas=(long)((double)rad/(double)degrad*(double)3600000.);
}

void MathMasDeg(long mas, angled *deg) {
  double angles;

  angles=((double)mas)/1000.;
  MathSecDeg(angles, deg);
}

int SchedGetTowFromDate(struct tm *date) {
  struct tm dateref={0,0,0,22,7,99}; /* origine pour gpstime */
  time_t t0, t1;
  int result;
  
  t1=mktime(date);
  t0=mktime(&dateref);
  result=((int)difftime(t1, t0))%604800;
  return(result);
}

int SchedGetTowFromMJD(double mjd) {
  int mjdref=51412; /* origine pour gpstime */
  double result;
  
  result=(double)(mjd-(int)mjd) ; 
  result+=((int)mjd-mjdref)%7;
  result*=86400.;
  return((int)result);
}

double SchedGetMJD(struct tm *date) {
  struct tm dateref={0,0,0,22,7,99}; /* origine pour gpstime */
  time_t t0, t1;
  double mjdref=51412.;
  double result;
  
  t1=mktime(date);
  t0=mktime(&dateref);
  
  result=mjdref+difftime(t1, t0)/86400.;
  return(result);
}

void _wait(void) {
  BYTE arg[32];

  while (1) {
    unsigned int tm = 0 ;
    signal_code dummy ;

    signalMask( 1 ) ;
    signalSend( gps.fd, SIG_GPS_CHAR ) ;
    compatSleep( &tm, &dummy ) ;
    if ( SaveHisto ) HistPrint( HFname ) ;
    else io_read( &gps ) ;
  }
}

void traiteA(ioset *io) {
  long dum;
  
  int count=io->current;
  
  if ( Debug ) fprintf( stderr, "Traite Ax\n" ) ;
  io->current=0;
  io->state=0;
}

void traiteB(ioset *io) {
  int i;
  
  if ( Debug ) fprintf( stderr, "Traite B\n" ) ;
  io->current=0;
  io->state=0;
}

void traiteC(ioset *io) {
  int i, j, ind, npage, nsubfr, framepage, epage;
  BYTE *curwor;
  int count=io->current;

  if ( Debug ) fprintf( stderr, "Traite C\n" ) ;
  io->current=0;
  io->state=0;
}

void traiteE(ioset * io) {
  int i, j;
  unsigned tempint;
  BYTE arg[32];
  int count=io->current;
  
  switch(io->buffer[3]) {
  case 'a':
    if (count==C_pos8.lngin) {
      if ( Debug ) fprintf(stderr,"Pos-8\n");
      if ( Debug > 2 ) {
int i = 0 ;
fprintf( stderr, "\nLongueur: %d\n", count ) ;
for( ; i<count ; i++ ) {
  if ( (i % 16 ) == 0) fprintf( stderr, "\n" ) ;
  fprintf( stderr, "%02x ", io->buffer[i] ) ;
}
fprintf( stderr, "\n" ) ;
      }
      io->current=0;
      io->state=0;
      memcpy( &(POSITION), io->buffer+4, 7 ) ;
      memcpy( &(POSITION.fract_sec), io->buffer+11, 62 ) ;

      MathMasDeg(POSITION.poslat, &(GpsStatus.latitude));
      MathMasDeg(POSITION.poslon, &(GpsStatus.longitude));

      GpsStatus.height = (double)POSITION.alt_gps/100.;
      if ( Histo ) HistAdd( (int)(GpsStatus.height*100) ) ;
      GpsStatus.temps.tm_sec=POSITION.sec;
      GpsStatus.temps.tm_min=POSITION.min;
      GpsStatus.temps.tm_hour=POSITION.heure;
      GpsStatus.temps.tm_mday=POSITION.jour;
      GpsStatus.temps.tm_mon=POSITION.mois-1;
      GpsStatus.temps.tm_year=POSITION.annee-1900;
      GpsStatus.secondes =
gps_seconds( 
    GpsStatus.temps.tm_year + 1900,
    GpsStatus.temps.tm_mon+1,
    GpsStatus.temps.tm_mday,
    GpsStatus.temps.tm_hour,
    GpsStatus.temps.tm_min,
    GpsStatus.temps.tm_sec ) ;
      if ( Debug ) show_status() ;
    }
    else io->state=2; 
    break;
  case 'k':
    if (count==C_extpos8.lngin){
      io->current=0;
      io->state=0;
    }
    else io->state=2; 
    break;
  case 'g':
    if (count==C_satrange8.lngin){
      io->current=0;
      io->state=0;
    }
    else io->state=2; 
    break;
  case 'n':
    if (count==C_Traim8.lngin){
      if ( Debug ) fprintf(stderr,"TRaim_os_\n");
      io->current=0;
      io->state=0;
      memcpy( &traim, io->buffer+4, 19 ) ;
      memcpy( &traim.one_sigma, io->buffer + 23, 43 ) ;
      if ( !Lsx ) {
FILE *fsaw = fopen( "/dd/sawtooth.txt", "w" ) ;
fprintf( fsaw, "%d\n", traim.sawtooth ) ;
fclose( fsaw ) ;
      }
      if ( Debug ) {
fprintf( stderr, " 1PPS mode: %d - 1PPS status: %d - 1PPS sync: %d\n",
 traim.ppsmode, traim.ppstatus, traim.ppsync ) ;
fprintf( stderr, " TRAIM solution: %d - TRAIM status: %d\n",
 traim.traim_solution, traim.traim_status ) ;
fprintf( stderr, " Saw Tooth: %d\n", traim.sawtooth ) ;
fprintf(stderr,"\n N  ID  M  S/N  St   Elev\n\n"); 
      }
      if ( Debug ) {
if ( traim.traim_solution == 0 )
  fprintf( stderr, "    GPS OK\n" ) ;
else if ( traim.traim_solution == 1 )
  fprintf( stderr, "GPS Almost OK\n" ) ;
else fprintf( stderr, "    GPS Bad\n" ) ;
      }
      GpsStatus.status = traim.traim_solution ;
      if ( Lsx && PrevStatus != GpsStatus.status ) {
PostPut( LsxMbxOut, &GpsStatus.status, 4 ) ;
PrevStatus = GpsStatus.status ;
      }
    }
    else {
      if ( Debug ) fprintf( stderr, "   Bad count\n" ) ;
      io->state=2; 
    }
    break;
  case 's':
    if (count==C_measure.lngin){
      io->current=0;
      io->state=0;
      /*traistan(io);           ********* Put your  measurement processing here ***** */
    }
    else ComNoEndOfCom(io);
    break;
  }
}

void traite(ioset *io) {
  int out;
  int count=io->current;
  if ( Debug ) fprintf(stderr, "%c%c - %d char\n",
       io->buffer[2],io->buffer[3],
       io->current );
  switch(io->buffer[2]) {
  case 'A':
    traiteA(io);
    break;
  case 'B':
    traiteB(io);
    break;
  case 'C':
    traiteC(io);
    break;
  case 'E':
    traiteE(io);
    break;
    
  }
  
  if(!io->state) {
    if ( Debug ) fprintf(stderr,"@@%c%c\n ",io->buffer[2],io->buffer[3]);
  }
}

int init_receiver(void) {
  BYTE arg[32];
  time_t system_time;
  static struct tm *system_date;

  if ( Debug ) fprintf( stderr, "Init_receiver : %d\n", InitReceiver ) ;

  if (ResetReceiver==1) {
    commande(&C_set_default,0, gps.fd);
    time( &system_time );
    system_date=gmtime(&system_time);
    arg[0]=3;
    arg[1]=system_date->tm_hour;
    arg[2]=system_date->tm_min;
    arg[3]=system_date->tm_sec;
    commande( &C_heure,arg, gps.fd );
    
    arg[0]=4;
    arg[1]=system_date->tm_mon+1;
    arg[2]=system_date->tm_mday;
    arg[3]=system_date->tm_year%256;
    arg[4]=system_date->tm_year/256;
    commande(&C_date,arg, gps.fd);
  }
  
  arg[0]=1;
  arg[1]= Frequency ;
  if ( Debug ) fprintf( stderr, "Commande 1\n" ) ;
  commande(&C_pos8, arg, gps.fd);

  if (Survey){
    arg[0]=1;
    arg[1]=2;
    if ( Debug ) fprintf( stderr, "Commande Survey\n" ) ;
    commande(&C_position_hold, arg, gps.fd);
  }
  /* Parametres de position */
  else if ( !Listen ) {
    if ( Debug ) fprintf( stderr, "Commande pas survey\n" ) ;
    arg[0]=13;
    int_to_bytes( arg+1, LatMas ) ;
    int_to_bytes( arg+5, LongMas ) ;
    int_to_bytes( arg+9, HeightMas ) ;
    arg[13]=0;
    if ( Debug ) fprintf( stderr, "Commande 3\n" ) ;
    commande(&C_position_holdp, arg, gps.fd);

    arg[0]=1;
    arg[1]=1;
    if ( Debug ) fprintf( stderr, "Commande 4\n" ) ;
    commande(&C_position_hold,arg,gps.fd);
  }
  if ( !Listen ) {
    arg[0]=4;
    
    int_to_bytes( arg+1, PpsOffset ) ;
    
    dswab(arg+1);
    commande(&C_pps_time_offset, arg, gps.fd);
    
    commande(&C_id, 0, gps.fd);
    
    arg[0]=1;
    arg[1]=0;
    commande(&C_almoutout,arg,gps.fd);
  } 
  if ( !Listen ) {
    
    arg[0]=1;
    arg[1]= MaskAngle ;
    commande( &C_mask_angle, arg, gps.fd);
    
    arg[0]=5;
    arg[1]=1;
    arg[2]=1;
    arg[3]=1;
    arg[4]=1;
    arg[5]=1;
    commande(&C_Traim8,arg,gps.fd);
    
    arg[0]=1;
    arg[1] = VisibleReq ;
    commande(&C_visiblesat, arg, gps.fd);
    
    arg[0]=1;
    arg[1]=4;
    commande(&C_ap_type,arg,gps.fd);
    
    if ( !HoldPosition ) {
      /* Unset Position */
      arg[0] = 1 ;
      arg[1] = 0 ;
      commande(&C_position_hold,arg,gps.fd);
    }
    /* Set use GPS time (not UTC) */
    arg[0] = 1 ;
    arg[1] = 0 ;
    commande( &C_corr_utc, arg, gps.fd ) ;
  }
  return(0);
}

int initialisation(int argc, char **argv) {
  if ( Debug ) fprintf(stderr, "Doing initialisation...\n"); 
  maxfd=0;
  
  gps.current=gps.state=0;
  gps.type=IOTYPGPS;
  gps.id=0;
  gps.dev = "/tgps" ;
  gps.process = &traite;

  if ( Debug ) fprintf(stderr, "Opening gps com..."); 

  if ( compatOpen( gps.dev, S_IREAD | S_IWRITE, &gps.fd ) != 0 ) {
    if ( Debug ) fprintf(stderr,"Erreur ouverture %s (%d)",gps.dev, errno );
    exit(0);
  }
  else { 
    if (gps.fd>maxfd) 
      maxfd=gps.fd;
  }
  if ( Debug ) fprintf(stderr, " Premier OK.\n"); 
  
  GpsStatus.longitude.deg = -68 ;
  GpsStatus.longitude.min = 0 ;
  GpsStatus.longitude.sec = 0.0 ;

  GpsStatus.latitude.deg = -35 ;
  GpsStatus.latitude.min = 0 ;
  GpsStatus.latitude.sec = 0. ;

  GpsStatus.height = 1200 ;
  if ( Histo ) HistInit( 145000, 200 ) ;

  {
    int signe ;
    int deg ;

    /* Convert to mas */
    signe = sgn( GpsStatus.longitude.deg ) ;
    deg = abs( GpsStatus.longitude.deg ) ;
    LongMas = ((double)deg * 3600. +
       (double)GpsStatus.longitude.min*60. +
       GpsStatus.longitude.sec ) * 1000. * signe ;
    signe = sgn( GpsStatus.latitude.deg ) ;
    deg = abs( GpsStatus.latitude.deg ) ;
    LatMas = (deg * 3600. +
      (double)GpsStatus.latitude.min*60. +
      GpsStatus.latitude.sec ) * 1000. ;
    HeightMas = GpsStatus.height * 100 ;
  }

  if ( Debug ) {
    fprintf(stderr, " Deuxieme OK.\n"); 
    fprintf(stderr, "initialising receiver..."); 
  }

  init_receiver();
  
  if ( Debug ) fprintf(stderr, " Troiseme OK.\n"); 
  return(0);
}

byebye()
{
  PostClose( &LsxMbxOut ) ;
  exit( 0 ) ;
}

void sig_hand( int sig )
{
  if ( sig == SIGINT ) SaveHisto = 1 ;
  else if ( sig == SIGQUIT ) byebye() ;
  else if ( sig != SIG_GPS_CHAR ) exit( 1 ) ;
  signalHandlerFinish() ;
}

help()
{
  printf( "GpsCtrl - %s - %s\n", __DATE__, __TIME__ ) ;
  puts( "gpsctrl [<options>]" ) ;
  puts( "Options:" ) ;
  puts( "  -d      : Increase the debugging (verbose) level" ) ;
  puts( "  -p      : Set Position Hold (default is NO)" ) ;
  puts( "  -i      : Reset the receiver" ) ;
  puts( "  -x      : Link to a mailbox created by LSX and send the status" ) ;
  puts( "  -f <nn> : Frequency for sending position packet" ) ;
  exit( 1 ) ;
}

int main(int argc, char **argv) {
  char c;
  extern char *optarg;
  extern int opterr;

  while ( (c = getopt( argc, argv, "f:H:xlvidp?" )) != EOF ) {
    switch( c ) {
    case 'l': Listen = 0 ; break ;
    case 'H': strcpy( HFname, optarg ) ; break ;
    case 'd': Debug++ ; break ;
    case 'i':  ResetReceiver=1 ; Listen = 0 ;
      /* setting this to 1 will reset the receiver -
 5 minutes at least to get it up again */
      break ;
    case 'f': sscanf( optarg, "%d", &Frequency ) ; break ;
    case 'v': VisibleReq = 0 ; break ;
    case 'p': HoldPosition = 1 ; Listen = 0 ;
      break ;
    case 'x': Lsx = 1 ; break ;
    default: help() ;
    }
  }
  setSignalHandler( sig_hand, _glob_data ) ;
  opterr=0;

  initialisation(argc,argv);
  if ( Debug ) show_status() ;
  do {
    _wait();
  } while (1);
  
  return 0;
}
