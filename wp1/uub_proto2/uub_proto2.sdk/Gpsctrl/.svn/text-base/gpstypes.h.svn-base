#ifndef TYPH
#define TYPH
#include <time.h>

/*********************************************************************

  $Author: guglielm $
  $Date: 2007-09-11 09:55:00 $
  $Revision: 1.1.1.1 $

  $Log: not supported by cvs2svn $
  Revision 1.1.1.1  2003/12/04 21:58:23  bertou
  Initial import of all software

  Revision 1.4  2001/11/13 16:13:38  guglielm
  Many changes:
    Separated path in and path out to /tgps
    Added several signals to set the receiver state
    No longer initialize the receiver in "non hold position" mode (let it as it is)
    Use lltoutm to convert Lat/Lon to UTM
    Uses GPS_STATUS structure in Status DM.
    Cosmetics

  Revision 1.2  2001/03/29 08:02:57  revenu
  Cosmetics


*********************************************************************/

#define IOTYPGPS 0x01
#define IOTYPSR620 0x02
#define IOTYPHP53132A 0x04
#define IOTYPFIFO 0x08
#define IOTYPRS232 0x10
#define IOTYPGPIB 0x20

#define CFGGPLOT 0x01
#define CFGFIFOIN 0x02
#define CFGFIFOOUT 0x04
#define CFGRECORDRAW 0x08
#define CFGGPSOUT 0x10
#define CFGGPSFROMFILE 0x20
#define CFGREADCOUNTER 0x40

#ifdef GTK
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#endif

#pragma pack (1)

typedef unsigned char  BYTE;          /* unsigned  8 bits */
#if !defined(_OS9000)
typedef unsigned short WORD;          /* unsigned 16 bits */
#endif
typedef unsigned long  LWORD;         /* unsigned 32 bits */

typedef struct dangle angled;

struct dangle {
  int deg;
  int min;
  double sec;
};

typedef enum {
  GPS_STATUS_OK,
  GPS_STATUS_ALMOST_OK,
  GPS_STATUS_BAD
} GPS_STATUS_STATUS ;

typedef struct {
  angled longitude, latitude ;
  double height ;
  struct tm temps ;
  unsigned int secondes ;
  int status ;
} GPS_STATUS ;


typedef struct ppoint point;
typedef struct ccoord coordc;
typedef struct gcoord coordg;

struct ccoord {
  double x;
  double y;
  double z;
};

struct gcoord {
  angled ladeg;
  long lamas;     /* en mas */
  double larad;     /* en rad */
  angled lodeg;
  long lomas;     /* en mas */
  double lorad;     /* en rad */
  double alt;
};

struct ppoint {
  coordc car;
  coordg geo;
};

typedef struct subf subframe;
struct subf {
        unsigned word[10];        };

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



typedef struct sat satv;
struct sat
        {
        BYTE id;
        WORD doppler;
        BYTE elev;
        WORD azim;
        BYTE health;
        };

typedef struct vs VIS_SAT;
struct vs
        {
        BYTE n_vis_sat; /* Nombre de satellite visibles */
        satv v_sat[12];
        };

/*
DEFINITION DU TYPE ttraim, INFORMATIONS SUR LE TimeRAIM */

typedef struct TRAIM ttraim;

#if defined(_OS9000)
struct TRAIM {
  BYTE rate ;
  BYTE algo ;
  WORD alarm ;
  BYTE ppsmode ;
  BYTE dummy[10] ;
  BYTE ppstatus ;
  BYTE ppsync ;
  BYTE traim_solution ;
  BYTE traim_status ;
  WORD one_sigma ;
  BYTE sawtooth ;
  BYTE satid ;
  unsigned int fract ;
} ;
#else
struct TRAIM
{
  BYTE rate;
  BYTE on;
  WORD alarm;
  BYTE mode;
  BYTE pps_rate[3];
  BYTE month;
  BYTE days;
  WORD year;
  BYTE heure;      /* Tout cela va de soi            */
  BYTE min;        /* Tout cela va de soi            */
  BYTE sec;        /* Tout cela va de soi            */
  BYTE raf2;    /* 3 non utilis~Bs                 */
  BYTE ppssync;    /* 3 non utilis~Bs                 */
  BYTE raf3;    /* 3 non utilis~Bs                 */
  BYTE status;        
  WORD sigma;               /* sigma estime */
  char neg_sawtooth;  /* negative sawtooth           */
  BYTE gpsst[8][5];
};

#endif

/***************************************************************************/


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

typedef struct sat_inf sat_info;
struct sat_inf
{
int status; /* voir definitions plus bas */
unsigned toc; /* time of clock data */
double af0;
double af1;
double af2;
double cic;
double cis;
double crc;
double crs;
double cuc;
double cus;
double dtn;
double ecc;
double ek;
double svt[16];
double svts[16];
double psd[16];
double rpsd[53];
double geom[53];
double iono[53];
double tropo[53];
double sagnac[53];
double dttr[53];
double tgd;
double gpssvt[53];
double refsv;
double srefsv[2]; 
double refgps;
double srefgps[2]; 
double mdio;
double smdi[2]; 
double mdtr;
double smdt[2]; 
point pos;
int initial_id;
int id;
double i0;
double idot;
unsigned iodc;
unsigned iode;
double m0;
double om;
double om0;
double omdot;
unsigned rcp;
double srcp;
double sqa;
unsigned sv_health;
unsigned framepage;
double elev;
double az;
};

typedef struct com_inf com_info;
struct com_inf
{
unsigned toe; /* time of ephemeris */
unsigned toa; /* time of almanach */
unsigned tow; /* time of week */
double mjd;
double tk;
double al[4];
double bt[4];
double a1;
double a0;
unsigned wn;
unsigned tot;
unsigned char wnt;
char dtls;
unsigned char wnlsf;
unsigned char dn;
char dtlsf;
int status;
unsigned alm_status;
struct tm date;/* actual date */
int timeofweek;/* actual TOW  */
double gpslt;
double utclt;
int heureutc;
int minutc;
int secutc;
double meas[16];
int lastneg;
int lastnegf;
int gplotfd;/* file handler for gnuplot fifo */
int his_length;/* # of samples to plot */
int config;/* see CFG**** above */
unsigned ppsoffset;
char ref_name[32];
char stat_name[32];
char gpsobroot[64];
char gpsobdata[64];
char schedulefile[64];
point stat_coord;
point ref_coord;
};

typedef struct trk_inf trk_info;
struct trk_inf {
int tempid; /* id */
int elev; /* tenth of degrees */
int az;   /* tenth of degrees */
int trackreldate;
int ltptr;
int stptr;
int mask;
int freqmaj;
double debtrack; /* MJD du début de traque */
int debtracktow; /* tow du début de traque */
double st[15];
double lt[52];
double coef[3];
double sigma;
int schedrefmjd;  /* reference mjd of the initial schedule */
int id[128];
double schedtime[128]; /* time of the beginning of the tracks (and then %1440)*/ 
int last_track;/* index of the last track of the current mjd */
int nb_track;/* number of tracks for the current mjd */
int track_index;/* index of the next track */
int tracking;
int mjdsched;
int schedule;/*The type of scheduling we are performing (see below) */
};

typedef struct io_set ioset;
        
struct io_set {
char *dev;
int fd;
int id;
int current;
int state;
int type;
BYTE command[32];
BYTE buffer[512];
        void (* process)(ioset *);
};

/*
DEFINITION DU TYPE pos, INFORMATIONS GENERALES */

typedef struct ppos pos;
struct ppos {
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
};

/*****************************************************************/
/* structure parametre geoides */
typedef struct DATUM ddatum;
struct DATUM
        {
        BYTE id;
        BYTE semimajor_int_part[3];
        WORD semimajor_fract_part;
        WORD inverseflat_int_part;
        LWORD inverseflat_fract_part;
        WORD delta_x;
        WORD delta_y;
        WORD delta_z;
        };

/*****************************************************************/
/* structure time_s */
typedef struct time_s ttime;
struct time_s
        {
        BYTE hours;
        BYTE minutes;
        BYTE secondes;
        };

/*****************************************************************/

/* structure date_s */
typedef struct date_s ddtate;
struct date_s
        {
        BYTE month;
        BYTE day;
        WORD year;
        };

#define ap ((double)6378137.)
#define f1 ((double)298.257223563)
#define pi ((double)3.141592653589793)
#define nsl ((double).299792458)
#define ee2 ((double)(2.-1./f1)/f1)

#define degrad ((double)pi/(double)180.)
#define MU ((double) 3.986005e14)
#define SOL ((double) 299792458)
#define omedot ((double) 7.2921151467e-5)
#define GRDF (-4.442807633e-10)
#define L1F0 (1575420000)
#define sgn(x) ((x)>0) ? (1) : (-(1))

#define ROLLOVER 1024

/* Definitions pour sat_info.status */

#define WNOK 0x01   /* le week number est acquis */
#define SF1 0x02/* subframe 1 acquise */
#define SF2 0x04
#define SF3 0x08
#define SF4 0x10
#define SF5 0x20
#define UTCOK 0x40
#define ALMOK 0x80
#define FULLALM 0x100
#define TIMEOK 0x200
#define MJDOK 0x400
#define SCHEDOK 0x800

/* Definitions pour trk_info.status */

#define TRKWAIT 0x01
#define TRKING 0x02

/* Typical duratio of one track */
#define TRKL (STTRK * LTTRK)
#define STTRK 15
#define LTTRK 52

/* Definitions pour le type de scheduling */
/* Definitions for the scheduling method  */
#define SINGLECV   1/* scheduling using almanach data and elevation information */
#define MELTINGPOT 2/* we follow the bipm schedule to perform simultaneous tracking */
#define MULTICHANNEL 3/* idem */
#define BIPM 3/* the same as multichannel */
#define CONTINUOUS 4/* Continuous tracking */

/* Macro pour determiner si une page de la subframe 4 est une page
   d'almanach de satellite ou pas;
   macro to determine whether  or not a given page of the subframe 4 
   is an almanach page */
#define isinsf4(x) ((((x)>1 && (x) <6) ||((x)>6 && (x) <11))  ? 1 : 0 )

#endif
