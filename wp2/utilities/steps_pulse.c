// steps_pulse
//
// Generate .csv arbitray waveform file for Rigol 5152 pulser
// This writes to STDOUT.
//
//  08-Jul-2016 DFN Initial version

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define VPP 5.   // Peak to peak voltage from pulser
#define DOTS 1000 // Number of data points
#define MAX_PH (-0.4*VPP)
#define STEP_WIDTH 40 // Width of step in ns
#define NUM_STEPS 5 // Number of steps
#define PERIOD 1. // Period in microseconds
#define DT (PERIOD/DOTS*1.E-6)

int main(int argc, char *argv[])
{

  double ph, t;
  int i, istep, jstep;

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
  ph = MAX_PH;
  t = 0;
  istep = 0;
  jstep = 0;
  for (i=2; i<DOTS; i++)
    {
      if (jstep >= STEP_WIDTH)
        {
          jstep = 0;
          istep = istep+1;
        }
      if (istep < NUM_STEPS)
          ph = (istep+1)*MAX_PH/NUM_STEPS;
      else
        ph = 0;
      jstep = jstep+1;
      t = t + DT;
      printf(",%6e\r\n",ph);
    }
  t = t + DT;
  printf("%6e\r\n",0.);
  exit(0);
}	
