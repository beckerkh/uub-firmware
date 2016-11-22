void plot_cal(char *filename)
{
#define _GNU_SOURCE
#define MUON_BUF_SIPM_CAL 0x20
  //#include "sde_trigger_defs.h"

  // Plot sipm calibration historgrams

  char line[132];
  int status, in_buf, buf_num, cal;
  size_t len = 0;
  int muon_num, ix, adc0, adc1, adc2, adc3, trigs;
  int nbins = 70;
  int i;
  double x, scale;

  FILE *inpfile = fopen(filename,"r");
  if (inpfile == 0) {
    printf("Unable to open input file %s\n",filename);
    return;
  }
   
  gStyle->SetPaperSize(27.94,21.59);
  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);

  TCanvas *muon_canv = new TCanvas("muon_canv",
				   "Muons",
				   100,10,2200,1000);
  TH1D *hADC0 = new TH1D("hADC0",";Bin;ADC value",nbins,0.,double(nbins));
  TH1D *hADC1 = new TH1D("hADC1",";Bin;ADC value",nbins,0.,double(nbins));
  TPad *padadc0 = new TPad("padadc0","High gain",0.02,0.02,0.98,0.48,0);
  TPad *padadc1 = new TPad("padadc1","Cal. gain",0.02,0.52,0.98,0.98,0);

  hADC0->SetLineWidth(2);
  hADC1->SetLineWidth(2);
  hADC0->SetLineColor(kBlack);
  hADC1->SetLineColor(kRed);

  muon_canv->Draw();
  padadc0->Draw();
  padadc1->Draw();

  buf_num = 0;
  muon_num = 0;
  while (fgets(line,132,inpfile)) {
    if (strncmp(line,">>>>>>>>>> BEGINNING OF MUON BUFFER >>>>>>>>>>",46) == 0)
      {
	printf("Found beginning of muon buffer\n");
	in_buf = 1;
	continue;
      }
    if (strncmp(line,"<<<<<<<<<< END OF MUON BUFFER <<<<<<<<<<",40) == 0)
      {
	printf("Found end of muon buffer \n");
	in_buf = 0;
	buf_num ++;
      }
    if (in_buf)
      {
	sscanf(line,"%d %d %x %x %x %x %x",
	       &muon_num,&ix,&adc0,&adc1,&adc2,&adc3,&trigs);

	cal = trigs & MUON_BUF_SIPM_CAL;
	x = double(ix) + 0.5;

	if (cal) 
	  hADC1->Fill(x,double(adc3));
	else
	  hADC0->Fill(x,double(adc3));
      }
  }

  fclose(inpfile);
  
  scale = 2./(buf_num*muon_num);

  padadc0->cd();
  hADC0->Scale(scale);
  hADC0->Draw();

  padadc1->cd();
  hADC1->Scale(scale);
  hADC1->Draw();

  muon_canv->Modified();
  muon_canv->Update();
}

