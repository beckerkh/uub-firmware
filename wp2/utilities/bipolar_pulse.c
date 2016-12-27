// bipolar_pulse
//
// Generate .csv arbitray waveform file for Rigol 5152 pulser
// This writes to STDOUT.
//
//  25-Dec-2016 DFN Initial version

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define VPP 5.   // Peak to peak voltage from pulser
#define DOTS 10000 // Number of data points
#define MAX_PH (-0.4*VPP)
#define NSTEPS_RUNIN 5
#define NSTEPS_LEADING 100
#define NSTEPS_TRAILING 120
#define STEP_SIZE  (MAX_PH/NSTEPS_LEADING) // Vertical step per tick
#define PERIOD 10. // Period in microseconds
#define DT (PERIOD/DOTS*1.E-6)

int main(int argc, char *argv[])
{

  double ph, t;
  int i;

  printf("RIGOL:DG5:CSV DATA FILE\r\n");
  printf("TYPE:Arb\r\n");
  printf("AMP:%f Vpp\r\n", VPP);
  printf("PERIOD:%f us\r\n",PERIOD);
  printf("DOTS: %d\r\n", DOTS);
  printf("MODE: Normal\r\n");
  printf("AFG Frequency:1000000.00000\r\n");  //??
  printf("AWG N:0\r\n");
  printf("x,y[V]\r\n");

  printf(",0.0\r\n");
  ph = 0;
  t = 0;
  for (i=2; i<DOTS; i++)
    {
      if ((i < NSTEPS_LEADING+NSTEPS_RUNIN) && (i >= NSTEPS_RUNIN))
	  ph = ph + STEP_SIZE;
      if ((i < NSTEPS_LEADING+NSTEPS_RUNIN+NSTEPS_TRAILING) && 
	  (i >= NSTEPS_LEADING+NSTEPS_RUNIN))
	ph = ph - STEP_SIZE;
      if ((i < NSTEPS_RUNIN+2*NSTEPS_TRAILING) && 
	  (i >= NSTEPS_LEADING+NSTEPS_RUNIN+NSTEPS_TRAILING))
	ph = ph + STEP_SIZE;
      t = t + DT;
      printf(",%6e\r\n",ph);
    }
  t = t + DT;
  printf("%6e\r\n",0.);
  exit(0);
}	
