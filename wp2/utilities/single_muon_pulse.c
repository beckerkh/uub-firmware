// single_muon_pulse
//
// Generate .csv arbitray waveform file for Rigol 5152 pulser
// This writes to STDOUT.
//
//  08-Jul-2016 DFN Initial version

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define VPP 5.   // Peak to peak voltage from pulser
#define DOTS 10000 // Number of data points
#define MAX_PH (-0.4*VPP)
#define TAU 60.e-9
#define PERIOD 1. // Period in microseconds
#define DT (1.E-9)

int main(int argc, char *argv[])
{

  double ph;
  int i;

  printf("RIGOL:DG5:CSV DATA FILE\r\n");
  printf("TYPE:Arb\r\n");
  printf("AMP:%f Vpp\r\n", VPP);
  printf("PERIOD:%f uS\r\n",PERIOD);
  printf("DOTS: %d\r\n", DOTS);
  printf("MODE: Normal\r\n");
  printf("AFG Frequency:1000000.00000\r\n");  //??
  printf("AWG N:0\r\n");
  printf("x,y[V]\r\n");

  printf(",0.0\r\n");
  ph = MAX_PH/2.;
  printf(",%6e\r\n",ph);
  ph = MAX_PH;
  printf(",%6e\r\n",ph);
  for (i=4; i<DOTS; i++)
    {
      ph = ph * exp(-DT/TAU);
      printf(",%6e\r\n",ph);
    }
  printf("%6e\r\n",0.);
  exit(0);
}	
