#if !defined(_GPSCTRL_H_)

#define _GPSCTRL_H_

#include "sigauger.h"

#include "latlong.h"
#include "gpsstatus.h"

#define CHAR_LF 0x0A
#define CHAR_CR 0x0D

#define GPS_OK_COUNT_MIN 10

#define ALMANACH_DATA_FILE "/dd/almanach.bin"

#if defined(MAIN)

#define EXTERN 

char *GpsDevice = "/tgps" ;
int Pos8FrequencyBAD = 1 ;
int Pos8FrequencyOK = 20 ;
int TraimFrequency = 1 ;
int PpsOffset = 0 ;
int SendWarnings = 0 ;
int HoldPosition = 0 ;
int GpsOKCount = 0 ;
int SetCurrentTime = 0 ;
int Debug = 0 ;
int StartedByHand = 0 ;
int DoHisto = 0 ;
int TestMode = 1 ;
int Mute = 0 ;
int WasOkOnce = 0 ;

#else

#define EXTERN extern

EXTERN char *GpsDevice ; /* /tgps */
EXTERN int Pos8FrequencyOK ;
EXTERN int Pos8FrequencyBAD ;
EXTERN int TraimFrequency ;
EXTERN int PpsOffset ;
EXTERN int SendWarnings ;
EXTERN int HoldPosition ;
EXTERN int GpsOKCount ;
EXTERN int SetCurrentTime ;
EXTERN int Debug ;
EXTERN int StartedByHand ;
EXTERN int DoHisto ;
EXTERN int TestMode ;
EXTERN int Mute ;
EXTERN int WasOkOnce ;

#endif


EXTERN int LongMas, LatMas, HeightMas ;

EXTERN path_id GpsPath, GpsPathOut ;
EXTERN int IOState, IOCurrent ;
EXTERN unsigned char IOBuffer[512] ;

EXTERN int *GpsLatitude, *GpsLongitude ; /* Pointers to lat/long,height
 in the Config data module */
EXTERN int *GpsHeight ;

EXTERN float UTMNorth, UTMEast ;

EXTERN int *GpsStatus,  /* Pointer to gps state in Status data module */
  PrevStatus,
  GpsTestStatus ;
EXTERN int MaskAngle ;

EXTERN TRAIM GpsTraim ;
EXTERN GPS_POSITION GpsPos ;
EXTERN SAT_STATUS GpsSatStat ;

/* GPS serial number is up to 15 digits, and is typically bigger than an
   unsigned int, so we store it simply as characters */
#define NUM_GPS_SN_CHARS 15
EXTERN char CurrGPSSn[NUM_GPS_SN_CHARS]; /* The GPS Serial number */

EXTERN struct tm *GpsTime ;
EXTERN unsigned int *GpsSeconds ; /* Pointer to time in seconds in Status */
EXTERN unsigned int *GpsCurrentSec ;
#if GPSCTRL_VERSION>5
EXTERN unsigned int GpsLocalSec ;
#endif
EXTERN SAWTOOTH_TABLE *SawTooth ; /* Pointer to sawtooth structure in Status */

EXTERN process_id IrqGpsPid;      /* extirq pid for GPS 1 pps interrupt */
#if GPSCTRL_VERSION>4
EXTERN unsigned char *GpsTimeValid ;
EXTERN unsigned char *GpsPosValid ;
EXTERN int *GpsPosNorth ;
EXTERN int *GpsPosEast ;
EXTERN int *GpsPosHeight ;
#endif

#endif
