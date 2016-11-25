#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <modes.h>
#include <math.h>
#include <errno.h>

#include "versions.h"
#include "gpsdefs.h"
#include "gpscommandes.h"
#include "gpsctrl.h"

#include "logfile.h"

int 
#if GPSCTRL_VERSION<6
SetGpsState(void)
#else
SetGpsState( int hold )
#endif
 {

  BYTE arg[32];
  time_t system_time;
  static struct tm *system_date;

#if GPSCTRL_VERSION<5
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
#endif

  arg[0]=4;

  int_to_bytes( arg+1, PpsOffset ) ;

  dswab(arg+1);
  commande(&C_pps_time_offset, arg );

  arg[0]=1;
  arg[1]=0;
  commande(&C_almoutout,arg );

  arg[0]=1;
  arg[1]= MaskAngle ;
  commande( &C_mask_angle, arg );
  
#ifdef(OS9000)
  arg[0]=5;
  arg[1] = TraimFrequency ; /* Frequence d'envoi de TRAIM (1 par sec) */
  arg[2] = 1 ; /* Algorithm ON */
  arg[3] = 0 ; /* Alarm limit */
  arg[4] = 100 ; /* 100x100 nanos */
  arg[5] = 1 ; /* 1PPS All the time */
  commande(&C_Traim8, arg );
#else
  arg[0]=1;
  arg[1] = TraimFrequency ; /* Frequence d'envoi de TRAIM (1 par sec) */
  commande(&C_Traim12, arg );

  arg[0]=1;
  arg[1] = 1 ; /* Algorithm ON */
  commande(&C_TraimEnable, arg );

  arg[0]=2;
  arg[1] = 0 ; /* Alarm limit */
  arg[2] = 100 ; /* 100 nanoseconds */
  commande(&C_TraimAlarm);

  arg[0] = 1;
  arg[1] = 1 ; /* 1PPS All the time */
  commande(&C_PPSEnable, arg );
#endif
  
  arg[0]=1;
  arg[1] = 0 ; /* Don't send satellite visibility changes */
  commande(&C_visiblesat, arg );
  
  
#if GPSCTRL_VERSION<5
  /* Unset Position */
  arg[0] = 1 ;
  arg[1] = 0 ;
  commande(&C_position_hold, arg );
#elif GPSCTRL_VERSION>6
  /* go to hold position mode if the config says that */
  if ( hold ) {
    arg[0] = 13 ;
    int_to_bytes( arg+1, LatMas ) ;
    int_to_bytes( arg+5, LongMas ) ;
    int_to_bytes( arg+9, HeightMas ) ;
    arg[13] = 0 ;
    commande( &C_position_holdp, arg ) ;

    /* 2nd enable hold position mode */
    arg[0] = 1 ;
    arg[1] = 1 ;
    commande(&C_position_hold, arg );
    PrintLog( LOG_INFO, "Set Hold Position: %d %d %d\n",
	      LatMas, LongMas, HeightMas ) ;
  }
  else {
    /* Unset Hold Position Mode */
    arg[0] = 1 ;
    arg[1] = 0 ;
    commande(&C_position_hold, arg );
    PrintLog( LOG_INFO, "UnSet Hold Position: %d %d %d\n",
	      LatMas, LongMas, HeightMas ) ;
  }
#endif

  /* Set use GPS time (not UTC) */
  arg[0] = 1 ;
  arg[1] = 0 ;
  commande( &C_corr_utc, arg ) ;

  arg[0] = 1 ;
  arg[1]= Pos8FrequencyBAD ; /* Frequence d'envoi de la position (1 par sec) */
#ifdef(OS9000)
  commande(&C_pos8, arg );
#else
  commande(&C_pos12, arg );
#endif
  return 0 ;
}
