#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gpstypes.h"
#include "gpsproto.h"
#include "gpsconfig.h"
#define DEBUG

extern int Debug ;

int InitNextLine(FILE *in, char *line){
  char *read, *sharp;

  do {
    if ((read=fgets(line, 128, in))){
      sharp=strchr(line,'#');
#ifdef DEBUG
      if ( Debug > 1 ) fprintf(stderr, "avant: %s",line);
#endif
      if (sharp)
*sharp=0;
#ifdef DEBUG
      if ( Debug > 1 ) fprintf(stderr, "apres: %s",line);
#endif
    }
  }
  while (!strlen(line) && read);
#ifdef DEBUG
  if ( Debug > 1 ) fprintf(stderr, "alafin: %s",line);
#endif
  return 0;
}

int InitReadConf(com_info *com, ioset *gps, ioset *counter) {
  char line[128];
  FILE *config;
  
  if ((config=fopen(CONFIGFILENAME,"r"))==NULL){
    fprintf(stderr, "InitReadConf: can't open config file %s\n");
    fprintf(stderr, "InitReadConf: the filename is defined in gpsconfig.h\n");
    return(0);
  }
  
  InitNextLine(config,line);
  sscanf(line, "%s", com->stat_name);
  
  InitNextLine(config,line);
  sscanf(line, "%s", com->ref_name);
  
  InitNextLine(config,line);
  if (!com->ppsoffset) /* read ppsoffset if it has not been specified in command line */
    sscanf(line, "%u", &(com->ppsoffset));
  
  InitNextLine(config,line);
  sscanf(line, "%32s", com->gpsobroot);
  
  InitNextLine(config,line);
  sscanf(line, "%32s", com->gpsobdata);
  
  InitNextLine(config,line);
  sscanf(line, "%32s", com->schedulefile);
  
  InitNextLine(config,line);
  if (!gps->id) /* command line -f option overrides config file entry */
    sscanf(line, "%32s", gps->dev);
  
  InitNextLine(config,line);
  
  InitNextLine(config,line);
  
  
  InitNextLine(config,line);
  
  fclose(config);
  return(1);
}

void InitGetCoordRad(point *stat, char str[8][32]){
  sscanf(str[1], "%lf", &(stat->geo.larad));
  sscanf(str[2], "%lf", &(stat->geo.lorad));
  sscanf(str[3], "%lf", &(stat->geo.alt));
  MathRadDeg(stat->geo.larad, &(stat->geo.ladeg));
  MathRadDeg(stat->geo.lorad, &(stat->geo.lodeg));
  MathRadMas(stat->geo.larad, &(stat->geo.lamas));
  MathRadMas(stat->geo.lorad, &(stat->geo.lomas));
  fprintf(stderr, "InitReadCoord: en mas : %ld %ld \n",
  stat->geo.lamas,
  stat->geo.lomas);
}

void InitGetCoordDeg(point *stat, char str[8][32]){
  sscanf(str[1], "%d", &(stat->geo.ladeg.deg));
  sscanf(str[2], "%d", &(stat->geo.ladeg.min));
  sscanf(str[3], "%lf", &(stat->geo.ladeg.sec));
  sscanf(str[4], "%d", &(stat->geo.lodeg.deg));
  sscanf(str[5], "%d", &(stat->geo.lodeg.min));
  sscanf(str[6], "%lf", &(stat->geo.lodeg.sec));
  sscanf(str[7], "%lf", &(stat->geo.alt));
  stat->geo.larad=(double)stat->geo.ladeg.deg + 
    (double)(sgn(stat->geo.ladeg.deg))*(
(double)stat->geo.ladeg.min
+ (double)stat->geo.ladeg.sec/60.)/60.;
  stat->geo.larad=stat->geo.larad*degrad;
  MathRadMas(stat->geo.larad, &(stat->geo.lamas));
  stat->geo.lorad=(double)stat->geo.lodeg.deg+(double)(sgn(stat->geo.lodeg.deg))*((double)stat->geo.lodeg.min
  +((double)stat->geo.lodeg.sec)/60.)/60.;
  stat->geo.lorad*=degrad;
  MathRadMas(stat->geo.lorad, &(stat->geo.lomas));
  if ( Debug > 1 ) {
    fprintf(stderr, "InitReadCoord: en mas : %ld %ld \n",
    stat->geo.lamas,
    stat->geo.lomas);
    fprintf(stderr, "InitReadConf: %d %d %lf %d %d %lf %lf %lf %lf %d %lf\n",
    stat->geo.ladeg.deg,
    stat->geo.ladeg.min,
    stat->geo.ladeg.sec,
    stat->geo.lodeg.deg,
    stat->geo.lodeg.min,
    stat->geo.lodeg.sec,
    stat->geo.alt,
    stat->geo.larad,
    stat->geo.lorad,
    sgn(stat->geo.lodeg.deg),
    (double)(sgn(stat->geo.lodeg.deg)));
  }
}

int InitSWriteCoord(char *name, coordg *stat) {
  char line[70];
  FILE *coord, *tempco;
  int i, done=0;
  char tempcoord[32]="/tmp/gps.coord";
  
  if ((coord=fopen(COORDFILENAME,"r"))==NULL){
    fprintf(stderr, "InitSWriteCoord: can't open coord file\n");
    return(0);
  }

  if ((tempco=fopen(tempcoord,"w+"))==NULL){
    fprintf(stderr, "InitSWriteCoord: can't open temp coord file\n");
    return(0);
  }
  while (fgets(line, 64, coord)!=NULL){
    i=strncmp(line, name, strlen(name));
    if (*line=='#' || i){
      fprintf(tempco,line);
    }
    if(!i){
      fprintf(tempco,"%s %d %d %lf %d %d %lf %lf\n", name, stat->ladeg.deg, stat->ladeg.min, stat->ladeg.sec,
      stat->lodeg.deg, stat->lodeg.min, stat->lodeg.sec,
      stat->alt);
      done=1;
    }
  }
  if (!done){
    fprintf(tempco,"%s %d %d %lf %d %d %lf %lf\n", name, stat->ladeg.deg, stat->ladeg.min, stat->ladeg.sec,
    stat->lodeg.deg, stat->lodeg.min, stat->lodeg.sec,
    stat->alt);
  }
  fclose(tempco);
  fclose(coord);
  sprintf(line, "mv %s %s", tempcoord, COORDFILENAME);
  system(line);
  return(1);
}

int InitSReadCoord(char *name, point *stat) {
  char line[70];
  char str[8][32];
  FILE *coord;
  int i;
  
  if ((coord=fopen(COORDFILENAME,"r"))==NULL){
    fprintf(stderr, "InitReadCoord: can't open coord file\n");
    return(0);
  }
  
  while (fgets(line, 64, coord)!=NULL){
    if (*line!='#'){
      switch ((i=sscanf(line, "%s %s %s %s %s %s %s %s", str[0], str[1], str[2], str[3],str[4],
str[5], str[6], str[7]))){
      case 4:
if (strcmp(str[0], name)==0) {
  InitGetCoordRad(stat, str);
}
break;

      case 8:
if (strcmp(str[0], name)==0) {
  InitGetCoordDeg(stat, str);
}
break;

      default:
fprintf(stderr, "InitReadCoord: wrong config file format\n");
fclose(coord);
return(0);
      }
    }
  }
  CoordGeoCar(stat->geo, &(stat->car));
  CoordGeoCar(stat->geo, &(stat->car));
  fprintf(stderr, "InitSReadCoord: %lf %lf %lf \n",
  stat->car.x,
  stat->car.y,
  stat->car.z);
  fclose(coord);
  return(1);
}
