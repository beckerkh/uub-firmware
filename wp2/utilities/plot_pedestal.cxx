void plot_shwr_traces(char *filename)
{
#define _GNU_SOURCE

  // Plot shower traces from minicom capture file

  char line[132];
  int status, in_event, event_num, nentries;
  size_t len = 0;
  ssize_t read;
  int adc[10], flags, filt_adc[3], ix, i;
  int hi[10], lo[10];
  double x;
  int lobin = 580;
  int hibin = 870;
  int nbins = hibin - lobin;

  FILE *inpfile = fopen(filename,"r");
  if (inpfile == 0) {
    printf("Unable to open input file %s\n",filename);
    return;
  }
   
  gStyle->SetPaperSize(27.94,21.59);
  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(3);
  gStyle->SetTitleSize(0.06,"xy");
  gStyle->SetLabelSize(0.06,"xy");
  gStyle->SetTitleOffset(.75,"xy");
  gStyle->SetNdivisions(110,"y");

  TCanvas *shower_canv = new TCanvas("shower_canv",
				      "Showers",
				      100,10,2200,1000);
  TH1D *hADC0H = new TH1D("hADC0H",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC1H = new TH1D("hADC1H",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC2H = new TH1D("hADC2H",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC0L = new TH1D("hADC0L",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC1L = new TH1D("hADC1L",";Bin;ADC value",nbins,lobin,hibin);
  TH1D *hADC2L = new TH1D("hADC2L",";Bin;ADC value",nbins,lobin,hibin);

  TPad *padadc0 = new TPad("padadc0","ADC",0.02,0.02,0.48,0.48,0);
  TPad *padadc1 = new TPad("padadc1","ADC",0.52,0.02,0.98,0.48,0);
  TPad *padadc2 = new TPad("padadc2","ADC",0.02,0.52,0.48,0.98,0);
  TPad *padadc3 = new TPad("padadc3","ADC",0.52,0.52,0.98,0.98,0);

  shower_canv->Draw();
  padadc0->Draw();
  padadc1->Draw();
  padadc2->Draw();
  padadc3->Draw();

  event_num = 0;
  printf("Type Enter to continue: ");
  gets(line);

  while (fgets(line,132,inpfile)) {
    if (strncmp(line,">>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>",40) == 0)
      {
	for (i=0; i<10; i++)
	  {
	  hi[i] = 0;
	  lo[i] = 4095;
	  }

	in_event = 1;
	hADC0H->Reset();
        hADC0H->SetLineWidth(3);
	hADC0H->SetLineColor(kBlack);

	hADC1H->Reset();
        hADC1H->SetLineWidth(3);
	hADC1H->SetLineColor(kBlack);

	hADC2H->Reset();
        hADC2H->SetLineWidth(3);
	hADC2H->SetLineColor(kBlack);

	hADC0L->Reset();
        hADC0L->SetLineWidth(3);
	hADC0L->SetLineColor(kBlack);

	hADC1L->Reset();
        hADC1L->SetLineWidth(3);
	hADC1L->SetLineColor(kBlack);

	hADC2L->Reset();
        hADC2L->SetLineWidth(3);
	hADC2L->SetLineColor(kBlack);
	//	hINT->SetMinimum(0.);
	//	hINT->SetMaximum(4000.);
	//	hADCD->SetMinimum(310.);
	//	hADCD->SetMaximum(340.);
        //	hADCD->SetAxisRange(650, 750, "X");

	// Get header information for the event
        //	for(i=0; i<10; i++)
	//  {
	//    fgets(line,132,inpfile);
	//    sscanf(line,"%d %d %d %d %d %d", 
	//	   &ix, &saturated[i], &base[i], &peak[i], &area[i], &rnum);
	    //	    printf("ix=%d\n",ix);
	//  }
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
	sprintf(line,"event%4.4d.eps\000",event_num);
	shower_canv->SaveAs(line);
	printf("Low values = %d %d   High values = %d %d\n",
	       lo[1], lo[3], hi[1], hi[3]);
	printf("Plotted event %d.  Type Enter to continue or q to quit: ",
	       event_num);
        nentries = 0;
	gets(line);
	if (strncmp(line,"q",1) == 0) return;

      }
    else if (in_event)
      {
	sscanf(line,"%x %x %x %x %x %x %x %x %x",
	       &ix,&flags,&adc[0],&adc[1],&adc[2],&adc[3],&adc[4],
               &adc[5],&adc[6]);
	nentries++;
	x = ix+.5;
	hADC0L->Fill(x,double(adc[0]));
	hADC1L->Fill(x,double(adc[2]));
	hADC0H->Fill(x,double(adc[1]));
	hADC1H->Fill(x,double(adc[3]));

	for (i=0; i<10; i++) 
	  {
	    if (adc[i] > hi[i]) hi[i] = adc[i];
	    if (adc[i] < lo[i]) lo[i] = adc[i];
	  }
        
      }

  }

  fclose(inpfile);
}

