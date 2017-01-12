void plot_pedestal(char *filename, int lobin, int hibin, int max_pedbin)
{
#define _GNU_SOURCE

  // Plot shower traces from minicom capture file

  char line[132];
  int status, in_event, event_num, nentries;
  size_t len = 0;
  ssize_t read;
  int ix, i;
  int adcraw[5], adc[4];
  double x;
  //  int lobin = 250;
  //  int hibin = 600;
  //int lobin = 0;
  //int hibin = 4095;
  int nbins = hibin - lobin;
  //   int max_pedbin = 600;
  //  int max_pedbin = 1000;

  FILE *inpfile = fopen(filename,"r");
  if (inpfile == 0) {
    printf("Unable to open input file %s\n",filename);
    return;
  }
   
  gStyle->SetPaperSize(27.94,21.59);
  //  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(3);
  gStyle->SetTitleSize(0.06,"xy");
  gStyle->SetLabelSize(0.06,"xy");
  gStyle->SetTitleOffset(.75,"xy");
  gStyle->SetNdivisions(110,"y");

  TCanvas *shower_canv = new TCanvas("pedestal_canv",
				     "Pedestals",
				     100,10,2200,1000);
  TH1D *hADC0H = new TH1D("hADC0H",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC1H = new TH1D("hADC1H",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC0L = new TH1D("hADC0L",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC1L = new TH1D("hADC1L",";Bin;ADC value",nbins,lobin,hibin);

  TPad *padadc0 = new TPad("padadc0","ADC0H",0.02,0.02,0.48,0.48,0);
  TPad *padadc1 = new TPad("padadc1","ADC0L",0.52,0.02,0.98,0.48,0);
  TPad *padadc2 = new TPad("padadc2","ADC1H",0.02,0.52,0.48,0.98,0);
  TPad *padadc3 = new TPad("padadc3","ADC1L",0.52,0.52,0.98,0.98,0);

  shower_canv->Draw();
  padadc0->Draw();
  padadc1->Draw();
  padadc2->Draw();
  padadc3->Draw();

  hADC0H->SetLineWidth(3);
  hADC0H->SetLineColor(kBlack);

  hADC1H->SetLineWidth(3);
  hADC1H->SetLineColor(kBlack);

  hADC0L->SetLineWidth(3);
  hADC0L->SetLineColor(kBlack);

  hADC1L->SetLineWidth(3);
  hADC1L->SetLineColor(kBlack);

  event_num = 0;

  while (fgets(line,132,inpfile)) {
    if (strncmp(line,">>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>",40) == 0)
      {
	in_event = 1;
      }
    else if (strncmp(line,"<<<<<<<<<< END OF EVENT <<<<<<<<<<",34) == 0)
      {
	in_event = 0;
	event_num ++;

	// Draw all the histograms
	padadc0->cd();
	hADC0H->Draw();

	padadc1->cd();
	hADC0L->Draw();

	padadc2->cd();
	hADC1H->Draw();

	padadc3->cd();
	hADC1L->Draw();

        shower_canv->Modified();
	shower_canv->Update();
      }
    else if (in_event)
      {
	sscanf(line,"%x %x %x %x %x %x",
	       &ix,&adcraw[0], &adcraw[1], &adcraw[2], &adcraw[3], &adcraw[4]);
	if (ix < max_pedbin)
	  {
	    nentries++;
	    //	if (nentries < 10)
	    //  printf("nentries=%d  ix=%d  adc=%d\n", nentries, ix, adc);

	    x = ix+.5;
	    adc[0] = adcraw[0] & 0xfff;
	    adc[2] = adcraw[1] & 0xfff;
	    adc[1] = (adcraw[0] >> 16) & 0xfff;
	    adc[3] = (adcraw[1] >> 16) & 0xfff;
	    hADC0L->Fill(adc[0]);
	    hADC1L->Fill(adc[2]);
	    hADC0H->Fill(adc[1]);
	    hADC1H->Fill(adc[3]);
	    if ((adc[0] > 500) || (adc[1] > 500) || 
		(adc[2] > 500) || (adc[3] > 500))
	      {
		printf("Bad pedestal value %d %d %d %d  event %d  bin %d\n", 
		       adc[0],adc[1],adc[2],adc[3], event_num, ix);
	      }
	  }

      }
        
  }

fclose(inpfile);
}

