#include <stdio.h>
#include <stdlib.h>

#include "LatLong-UTMConversion.h"

int main()
{
  char UTMZone[4];
  int RefEllipsoid = 23 ;
  FILE *fin ;
  double lat_input, lon_input,
    northing_output, easting_output ;

#if 0
  while( ifs >> loc >> la >> lo >> no >> ea )
    {
      codes.push_back(loc);
      lat_input.push_back(la);
      lon_input.push_back(lo);
      northing_input.push_back(no);
      easting_input.push_back(ea);
    }
  int nlines=codes.size();
  cout << "Nombre de lignes lues : " << nlines << endl;
  lon_output.resize(nlines);
  lat_output.resize(nlines);
  northing_output.resize(nlines);
  easting_output.resize(nlines);
  
  cout << setiosflags(ios::showpoint | ios::fixed) << setprecision(5);
#else
#endif

  for( ; ; ) {
    char lu[128] ;
    float la, lon ;

    printf( "Latitude et Longitude (en degres decimaux) :" ) ;
    fgets( lu, 128, stdin ) ;
    sscanf( lu, "%lg%lg", &lat_input, &lon_input ) ;
    LLtoUTM( RefEllipsoid, lat_input, lon_input,
     &northing_output, &easting_output, UTMZone);

    printf( "Lat: %f - Long: %f >> North: %f - East: %f - Zone: %s\n",
    lat_input, lon_input,
    northing_output, easting_output, UTMZone );
    UTMtoLL( RefEllipsoid, northing_output, easting_output, UTMZone,
     &lat_input, &lon_input ) ;
    printf( " North: %f - East: %f - Zone: %s >> Lat: %f - Long: %f\n",
    northing_output, easting_output,
    UTMZone,
    lat_input, lon_input );
  }
  exit( 0 ) ;
}
