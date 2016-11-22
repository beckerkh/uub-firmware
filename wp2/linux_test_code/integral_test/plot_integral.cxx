void plot_integral(char *filename)
{
#define _GNU_SOURCE
#define INTEGRAL_FRAC_BITS 11

  // Plot showers from minicom capture file

  char line[132];
  int status, in_event, event_num;
  size_t len = 0;
  ssize_t read;
  int ix, adc,baseline,integral,integral1,sintegral;
  int nbins = 2048;
  int i, j;
  double x;
  int saturated[10], base[10], peak[10], area[10];

  FILE *inpfile = fopen(filename,"r");
  if (inpfile == 0) {
    printf("Unable to open input file %s\n",filename);
    return;
  }
   
  gStyle->SetPaperSize(27.94,21.59);
  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(2);

  TCanvas *shower_canv = new TCanvas("shower_canv",
				      "Showers",
				      100,10,2200,1000);
  TH1D *hADC = new TH1D("hADC",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hBASE = new TH1D("hBASE","FPGA baseline;Bin;ADC value",
			 nbins,0.,nbins);
  TH1D *hINT = new TH1D("hINT","FPGA integral;Bin;ADC value",nbins,0.,nbins);
  TH1D *hINT2 = new TH1D("hINT2","Offline integral;Bin;ADC value",
			 nbins,0.,nbins);
  TPad *padadc = new TPad("padadc","ADC",0.02,0.02,0.98,0.48,0);
  TPad *padint = new TPad("padint","Integral",0.02,0.52,0.98,0.98,0);

  shower_canv->Draw();
  padadc->Draw();
  padint->Draw();

  event_num = 0;
  printf("Type Enter to continue: ");
  gets(line);

  while (fgets(line,132,inpfile)) {
    if (strncmp(line,">>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>",40) == 0)
      {
	in_event = 1;
	sintegral = 0;
	hADC->Reset();
	hBASE->Reset();
	hINT->Reset();
	hINT2->Reset();
        hADC->SetLineWidth(2);
        hBASE->SetLineWidth(2);
        hINT->SetLineWidth(2);
        hINT2->SetLineWidth(2);
	hADC->SetLineColor(kBlack);
	hBASE->SetLineColor(kRed);
	hINT->SetLineColor(kRed);
	hINT2->SetLineColor(kBlack);
	hINT->SetMinimum(0.);
	hINT->SetMaximum(4000.);
	// Get header information for the event
	for(i=0; i<10; i++)
	  {
	    fscanf(inpfile,"%d %d %d %d %d", 
		   &ix, &saturated[i], &base[i], &peak[i], &area[i]);
	  }
	continue;
      }
    if (strncmp(line,"<<<<<<<<<< END OF EVENT <<<<<<<<<<",34) == 0)
      {
	in_event = 0;
	event_num ++;

	// Draw all the histograms
	padadc->cd();
	hADC->Draw();
	hBASE->Draw("same");

	padint->cd();
	hINT->Draw();
	hINT2->Draw("same");

        shower_canv->Modified();
	shower_canv->Update();
	printf("Saturated=%d  Baseline=%d  Peak=%d  Area=%d\n",
	       saturated[3], base[3], peak[3], area[3]);
	printf("Plotted event %d.  Type Enter to continue: ", event_num);
	gets(line);
      }
    if (in_event)
      {
	sscanf(line,"%d %d %d %d %d",
	       &ix,&adc,&integral1,&baseline,&integral);
	x = ix+.5;

	hADC->Fill(x,double(adc));
	hBASE->Fill(x,double(baseline));
	hINT->Fill(x,double(integral));
	sintegral = sintegral+adc-baseline;
	hINT2->Fill(x,double(sintegral));
      }

  }

  fclose(inpfile);
}

