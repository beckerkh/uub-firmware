#ifndef _GPSDEFS_H_

#define _GPSDEFS_H_

/*********************************************************************

  $Author: guglielm $
  $Date: 2007-09-11 09:55:00 $
  $Revision: 1.1.1.1 $

  $Log: not supported by cvs2svn $
  Revision 1.1.1.1  2003/12/04 21:58:23  bertou
  Initial import of all software

  Revision 1.3  2001/11/13 16:13:37  guglielm
  Many changes:
    Separated path in and path out to /tgps
    Added several signals to set the receiver state
    No longer initialize the receiver in "non hold position" mode (let it as it is)
    Use lltoutm to convert Lat/Lon to UTM
    Uses GPS_STATUS structure in Status DM.
    Cosmetics

  Revision 1.2  2001/07/29 16:30:19  guglielm
  Various bugs fixes

  Revision 1.1  2001/04/12 10:52:52  guglielm
  Gpsctrl version for LS.
  Works well with LS software


*********************************************************************/

#define sgn(x) ((x)>0) ? (1) : (-(1))

typedef unsigned char BYTE ;
#if !defined(_OS9000)
typedef unsigned short WORD ;          /* unsigned 16 bits */
#endif
typedef unsigned long  LWORD ;         /* unsigned 32 bits */

#if GPSCTRL_VERSION>4
/* If bits 6 and 0 of receiver status are at 0, position should be OK */
#define GPS_POSITION_VALID_MASK 0x41
/* If one-sigma from receiver < GPS_TIME_VALID, everything is OK */
#define GPS_TIME_VALIDITY 200
#endif

/*
DEFINITION DU TYPE comm POUR MANIPULER AISEMENT
LES COMMANDES (MOTOROLA BINARY FORMAT) */

typedef struct com comm;
struct com
{
  char *nom; /* NOM DE LA COMMANDE (ON Y MET CE QU'ON VEUT) */
  char *dat;/* LA COMMANDE ELLE-MEME */
  BYTE lng;     /* LA LONGUEUR DE LA COMMANDE en sorite du PC */
  WORD lngin;     /* LA LONGUEUR DE LA COMMANDE en entree du PC */
};

/*
DEFINITION DU TYPE ttraim, INFORMATIONS SUR LE TimeRAIM */
typedef struct {
  BYTE satid ;
  unsigned int fract ;
} TRAIM_SAT ;
#ifdef(OS9000)
typedef struct {
  BYTE rate ;
  BYTE algo ;
  WORD alarm ;
  BYTE ppsmode ;
  BYTE dummy[10] ;
  BYTE ppstatus ;
  BYTE ppsync ;
  BYTE traim_solution ;
  BYTE traim_status ; /*19*/
  WORD one_sigma ;
  BYTE sawtooth ;
  TRAIM_SAT sat[8] ;
} TRAIM ;
#else
typedef struct {
  BYTE ppstatus ;
  BYTE ppsync ;
  BYTE traim_solution ;
  BYTE traim_status ; /*19*/
  LWORD svids;  //svids removed by traim, 32 bit field
  WORD one_sigma ;
  BYTE sawtooth ;
  TRAIM_SAT sat[12] ;
} TRAIM ;
#endif
/*
DEFINITION DU TYPE channel, STATUS D'1 CANAL DONNE */

typedef struct canal channel;
struct canal
{
  BYTE id;              /* PRN DU SATELLITE */
  BYTE trac_mode;       /* TRACKING MODE */
  BYTE sig_strgth;      /* PUISSANCE DU SIGNAL */
  BYTE chan_status;     /* ETAT DU CANAL */
};

/*
DEFINITION DU TYPE pos, INFORMATIONS GENERALES */
#ifdef(OS9000) //For M12M upgrade
typedef struct {
  BYTE mois;                      /* mois */
  BYTE jour;                      /* jour */
  WORD annee;
  BYTE heure;                     /* heure */
  BYTE  min;              /* minutes */
  BYTE  sec;              /* secondes */
  LWORD fract_sec;
  long poslat;          /* latitude */
  long poslon;         /* longitude */
  LWORD alt_gps;          /* altitude referencee a GPS */
  LWORD alt_msl;          /* altitude referencee a MSL */
  WORD velocity;          /* vitesse */
  WORD heading;
  WORD cDOP;
  BYTE DOP_type;
  BYTE sat_vis;         /* nombre de stallites visibles */
  BYTE sat_trac;        /* nombre de stallites suivis */
  channel can[8];       /* status des 8 canaux */
  BYTE rec_status;      /* status du recepteur */
  BYTE checksum;
} GPS_POSITION ;
#else  //following is for 12 channel receivers
typedef struct {
  BYTE mois;                      /* mois */
  BYTE jour;                      /* jour */
  WORD annee;
  BYTE heure;                     /* heure */
  BYTE  min;              /* minutes */
  BYTE  sec;              /* secondes */
  LWORD fract_sec;
  long poslat;          /* latitude */
  long poslon;         /* longitude */
  LWORD alt_gps;          /* altitude referencee a GPS */
  LWORD alt_msl;          /* altitude referencee a MSL */
  WORD velocit3D;			//3D velocity, new on M12M
  WORD velocity;          /* vitesse */
  WORD heading;
  WORD cDOP;
//  BYTE DOP_type;
  BYTE sat_vis;         /* nombre de stallites visibles */
  BYTE sat_trac;        /* nombre de stallites suivis */
  channel can[12];       /* status des 8 canaux */
  WORD rec_status;      /* status du recepteur */ //changed for M12M
  BYTE m12mstuff[20];     //Various new data that are given by m12m. See the m12m users manual if you think you may have a use for this. Might be 19 bytes
  BYTE checksum;
} GPS_POSITION ;
#endif

typedef struct {
  BYTE id ;
  WORD doppler ;
  BYTE elevation ;
  WORD azimuth ;
  BYTE health ;
} SAT_DATA ;

typedef struct {
  BYTE nsat ;
  SAT_DATA sat[12] ;
} SAT_STATUS ;

unsigned commande(comm *com, BYTE *arg ) ;

#endif
