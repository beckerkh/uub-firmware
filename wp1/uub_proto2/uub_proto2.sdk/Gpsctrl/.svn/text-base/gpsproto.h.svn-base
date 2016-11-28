#include "gpsconfig.h"
#include "gpstypes.h"
#ifdef GTK
void gps_plot(void);
void gps_print(char *, char *, int);
void update_trackprogbar(gfloat);
void set_mask_adj(gfloat);
int init_gtk (int, char **);
/*argv[])*/
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#endif
#define SGN(x) ((x)>0 ? 1 : -1)

void io_gpib_sighandler(int);
void swab(const void *, void *, size_t );
long dswab(char *);
void SchedCont(com_info *com, double x) ;
int SchedSeq(com_info *, trk_info *, sat_info *, double ) ;
double SchedGetMJD(struct tm *) ;
int SchedGetChecksum(char* );
int SchedGetSingleCVTow(sat_info *, com_info *, int, trk_info *);
int SchedGetSingleCV(sat_info *sat, com_info *com, int tow, trk_info *trk);
int SchedGetMask(sat_info *, com_info *, int);
int SchedGetTowFromDate(struct tm *);
int SchedGetTowFromMJD(double mjd);
int SchedGetSatElev(sat_info *, com_info *, int, point *,int [33], int [33] );
int SchedCompar(const void *, const void *); 
int SchedMalargueSim(sat_info *, com_info *, int , point *) ;
int SchedBuildSchedule(com_info *com, trk_info *trk, sat_info *sat) ;
int SchedEndTrack(com_info *,trk_info *);
double SchedGoTrack(com_info *,trk_info *);

double MathNEqSpcRms(double *, double *, int, double *) ;
double MathEqSpcRms(double *, int, double *) ;
double MathEqSpcRms2(double *, int, double *) ;
void MathSecDeg(double , angled *) ;
void MathRadDeg(double , angled *) ;
void MathRadMas(double , long *) ;
void MathMasDeg(long, angled *);
int MathKepler(double , double , double *, double, int) ;

void ComNoEndOfCom(ioset *);
void reinitbuf(ioset *);
int init_stanford(void);
long dswab(char * );
void masdeg(long, angled *);
#if 0
void chksum(comm *);
#else
void chksum( unsigned char *, int ) ;
#endif
unsigned commande(comm *, BYTE *, int );

void CoordSetStat(point *station);
double CoordDist(coordc , coordc );
coordc *CoordPv(coordc , coordc , coordc *, double *);
double CoordPs(coordc *, coordc *) ;
coordc *CoordGeoCar(coordg , coordc *);
coordc *CoordSetNormale(point *, coordc *) ;
void CoordSetRepereLocal(point *) ;
double CoordGetElevation(sat_info *, com_info *, point *, double );
double CoordGetSatPos(sat_info *, com_info *, point *, double);
double CoordGetIono(sat_info *, com_info *, point, double) ;
double CoordGetTropo(sat_info *, point);
double CoordGetSag(sat_info *, point) ;

int InitReadConf(com_info *, ioset *, ioset *) ;
int InitReadCoord(com_info *) ;
int InitSWriteCoord(char *, coordg *);
int InitSReadCoord(char *, point *);
void InitGetCoordRad(point *, char str[8][32]);
void InitGetCoordDeg(point *, char str[8][32]);

int traitesfr(sat_info *, com_info *, subframe * , unsigned , unsigned);

void traistan(ioset *);
void mesure(ioset *);
int io_create_gpib_fifo(char * );
int io_gpib_init(ioset *);
void io_read(ioset *);
void io_write(ioset *);
int _setcanal(int,int,int);
void traite(ioset *);
void readcom(int);

#ifndef GTK
void _wait(void);
#else
void _wait(gpointer, gint, GdkInputCondition);
#endif
