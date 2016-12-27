void plot_shwr_traces(char *filename)
{
#define _GNU_SOURCE

  // Plot shower traces from minicom capture file

  char line[132];
  int status, in_event, event_num;
  size_t len = 0;
  ssize_t read;
  int adc[9], flags, filt_adc[3], ix;
  double x;

  FILE *inpfile = fopen(filename,"r");
  if (inpfile == 0) {
    printf("Unable to open input file %s\n",filename);
    return;
  }
   
  gStyle->SetPaperSize(27.94,21.59);
  gStyle->SetOptStat(0);
  gStyle->SetLineWidth(3);
  gStyle->SetTitleSize(0.09,"xy");
  gStyle->SetLabelSize(0.09,"xy");
  gStyle->SetTitleOffset(.45,"xy");
  gStyle->SetNdivisions(110,"y");

  TCanvas *shower_canv = new TCanvas("shower_canv",
				      "Showers",
				      100,10,2200,1000);
  TH1D *hADC0H = new TH1D("hADC0H",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADC1H = new TH1D("hADC1H",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADC2H = new TH1D("hADC2H",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADC0L = new TH1D("hADC0L",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADC1L = new TH1D("hADC1L",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADC2L = new TH1D("hADC2L",";Bin;ADC value",nbins,0.,nbins);

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
	in_event = 1;
	hADC0->Reset();
        hADC0->SetLineWidth(3);
	hADC0->SetLineColor(kBlack);

	hADC1->Reset();
        hADC1->SetLineWidth(3);
	hADC1->SetLineColor(kBlack);

	hADC2->Reset();
        hADC2->SetLineWidth(3);
	hADC2->SetLineColor(kBlack);

	hADC3->Reset();
        hADC3->SetLineWidth(3);
	hADC3->SetLineColor(kBlack);
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
	printf("Plotted event %d.  Type Enter to continue or q to quit: ",
	       event_num);
        nentries = 0;
	gets(line);
	if (strncmp(line,"q",1) == 0) return;

      }
    else if (in_event)
      {
	sscanf(line,"%x %x %x %x %x %x %x %x %x %x %x %x %x %x",
	       &ix,&flags,&adc[0].&adc[1],&adc[2],&adc[3],&adc[4],
               &adc[5], &adc[6],&adc[7],&adc[8],&adc[9],&filt_adc[0],
               &filt_adc[1],&filt_adc[2]);
	nentries++;
	x = ix+.5;
	hADC0L->Fill(x,double(adc[0]));
	hADC1L->Fill(x,double(adc[2]));
	hADC0H->Fill(x,double(adc[1]));
	hADC1H->Fill(x,double(adc[3]));
      }

  }

  fclose(inpfile);
}

