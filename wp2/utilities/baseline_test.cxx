#include "TStyle.h"
#include "TH1D.h"
#include "TF1.h"

double tmin;
double dt;
double baseline_initial;
double r1, r2;
int nbins;

double fitf(double *t, double *par);

void baseline_test()
{
  //#define _GNU_SOURCE

#include <unistd.h>


  double rc_decay1, rc_decay2;
  double xmin = 0.;
  double xmax = 50.e-6;
  double max_ph = 1000.;
  double width = 10.e-6;
  double delay = 0.1e-6;
  double baseline_sag1 = 0.;
  double baseline_sag2 = 0.;
  double baseline_computed1 = 0.;
  double baseline_computed2 = 0.;
  double input, input1, input2, output1, output2;
  double inferred_input1, inferred_input2;
  double t, rc1, rc2;
  int i;

  r1 = 100.;
  r2 = 100.;
  double c1 = 100.e-9;
  double c2 = 470.e-9;

  nbins = 100;
  baseline_initial = 1000.;
  dt = (xmax-xmin)/nbins;

  rc1 = r1*c1;
  rc2 = r2*c2;

  gStyle->SetPaperSize(27.94,21.59);
  gStyle->SetLineWidth(3);
  gStyle->SetOptFit(1111);

  TH1D *hInput = new TH1D("hInput",";Time;Pulse height",nbins,xmin,xmax);
  TH1D *hInfInp1 = new TH1D("hInfInp1",";Time;Pulse height",nbins,xmin,xmax);
  TH1D *hInfInp2 = new TH1D("hInfInp2",";Time;Pulse height",nbins,xmin,xmax);
  TH1D *hBaseline = new TH1D("hBaseline",";Time;Pulse height",nbins,xmin,xmax);
  TH1D *hOutput1 = new TH1D("hOutput1",";Time;Pulse height",nbins,xmin,xmax);
  TH1D *hOutput2 = new TH1D("hOutput2",";Time;Pulse height",nbins,xmin,xmax);

  hInput->SetMinimum(0.);
  hInfInp1->SetLineColor(kGreen);
  hInfInp2->SetLineColor(kMagenta);
  hOutput1->SetLineColor(kRed);
  hOutput2->SetLineColor(kCyan);
  hBaseline->SetLineColor(kBlue);

  for (i=0; i<nbins; i++)
    {
      t = (i+.1)*(xmax-xmin)/nbins + xmin;
      if (t < delay)
	input = baseline_initial;
      else if (t < delay+width)
	input = baseline_initial + max_ph;
      else
	input = baseline_initial;

      rc_decay1 = dt/rc1;
      input1 = input - baseline_initial;
      output1 = input1 - baseline_sag1;
      baseline_sag1 = baseline_sag1 + output1*rc_decay1;
      baseline_computed1 = baseline_initial - baseline_sag1;
      inferred_input1 = output1 + baseline_sag1 + baseline_initial;

      rc_decay2 = dt/rc2;
      input2 = output1;
      output2 = input2 - baseline_sag2;
      baseline_sag2 = baseline_sag2 + output2*rc_decay2;
      baseline_computed2 = baseline_computed1 - baseline_sag2;
      inferred_input2 = output2 + +baseline_sag1 + 
	baseline_sag2 + baseline_initial;

      hInput->Fill(t, input);
      hOutput1->Fill(t, output1+baseline_initial);
      hOutput2->Fill(t, output2+baseline_initial);
      hBaseline->Fill(t+dt, baseline_computed2);
      hInfInp1->Fill(t, inferred_input1);
      hInfInp2->Fill(t, inferred_input2);

    }

  hInput->Draw();
  hOutput1->Draw("same");
  hOutput2->Draw("same");
  hBaseline->Draw("same");
  hInfInp1->Draw("same");
  hInfInp2->Draw("same");

  // Now fit the final histogram to try to recover RC1 & RC2.
  tmin = width+delay+dt/2.;
  TF1 *f = new TF1("f",fitf,tmin,xmax,3);
  f->SetParameters(1/.1,1/.470,762.85);
  f->SetParNames ("c1","c2","Normalization");
  f->SetParLimits(0,1.,20.);
  f->SetParLimits(1,1.,20.);
  f->SetParLimits(2,762.85,762.85);

  ROOT::Fit::FitConfig::SetMinimizer("Simplex"); 
  hBaseline->Fit("f","RVW");
}

// Fit to 2 rc constants
double fitf(double *t, double *par)
{
  // Somehow we need to know dt, and also the first t.  Get these from
  // global variables.

  double dtrc1, dtrc2;
  double c1, c2;
  double baseline_sag1, baseline_sag2;
  double baseline_computed1, baseline_computed2;
  double output1, output2;
  double input1, input2;
  int ibins;
  int i;

  c1 = 1./par[0]*1.e-6;
  c2 = 1./par[1]*1.e-6;
  dtrc1 = dt/(r1*c1);
  dtrc2 = dt/(r2*c2);
  baseline_sag1 = par[2];
  ibins = (t[0] - tmin)/dt;

  for (i=0; i<=ibins; i++)
    {
      input1 = 0;
      output1 = input1 - baseline_sag1;
      baseline_sag1 = baseline_sag1 + output1*dtrc1;
      baseline_computed1 = baseline_initial - baseline_sag1;

      input2 = output1;
      output2 = input2 - baseline_sag2;
      baseline_sag2 = baseline_sag2 + output2*dtrc2;
      baseline_computed2 = baseline_computed1 - baseline_sag2;

    }
  //  printf("ibins=%d dt=%g c1=%g c2=%g norm=%g baseline=%g\n",
  //	 ibins,dt,par[0],par[1],par[2], baseline_computed2);
  
  return baseline_computed2;
}
