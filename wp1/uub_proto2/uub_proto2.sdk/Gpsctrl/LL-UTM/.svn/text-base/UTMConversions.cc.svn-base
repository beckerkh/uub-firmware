#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "LatLong-UTMConversion.h"

using namespace std;

int main()
{
  char UTMZone[4];
  int RefEllipsoid = 23;//WGS-84. See list with file "LatLong- UTM conversion.cpp" for id numbers
  string loc;
  double lo,la,no,ea;
  vector<string> codes, UTMzones;
  vector<double> lon_input,lat_input,northing_input,easting_input;
  vector<double> lon_output,lat_output,northing_output,easting_output;

  ifstream ifs("coordinates.txt");
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
  for( int i=0; i<nlines; i++ )
    {
      LLtoUTM(RefEllipsoid, lat_input[i], lon_input[i], northing_output[i], easting_output[i], UTMZone);
      UTMzones.push_back((string)UTMZone);
      UTMtoLL(RefEllipsoid, northing_input[i], easting_input[i], UTMZone, lat_output[i], lon_output[i]);
    }

  for( int i=0; i<nlines; i++ )
    {
      cout << northing_input[i] << "  " << northing_output[i] << " --- "
	   << easting_input[i] << "  " << easting_output[i] << " ++++++ "
	   << lon_input[i] << "  " << lon_output[i] << " --- "
	   << lat_input[i] << "  " << lat_output[i] << " " << UTMzones[i] << endl;
    }

  return 0;
}
