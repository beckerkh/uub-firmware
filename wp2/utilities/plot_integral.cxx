void plot_integral(char *filename)
{
#define _GNU_SOURCE
#define INTEGRAL_FRAC_BITS 11

  // Plot showers from minicom capture file

  char line[132];
  int status, in_event, event_num;
  size_t len = 0;
  ssize_t read;
  int adcraw[5];
  int ix, adc[2],adcd[2],baseline[2],integral[2],sintegral[2], iv;
  int nbins = 2048;
  int i, j, pk, nentries, rnum;
  int baseline3;
  double x, baseline_sag, baseline1, baseline2;
  int saturated[10], base[10], peak[10], area[10];
  int ichan = 0;

  FILE *inpfile = fopen(filename,"r");
  if (inpfile == 0) {
    printf("Unable to open input file %s\n",filename);
    return;
  }
   
  // Test computation of baseline sag
  int amp = 10000;
  //  int sag1 = (amp>>9) - (amp>>11) - (amp>>14);
  int sag1 = (amp>>10) - (amp>>12);
  int sag2 = amp*0.0008;
  printf("amp=%d  sag1=%d  sag2=%d\n",amp,sag1,sag2);

  gStyle->SetPaperSize(27.94,21.59);
  //gStyle->SetOptStat(0);
  gStyle->SetLineWidth(3);
  //  gStyle->SetTitleSize(0.09,"xy");
  //gStyle->SetLabelSize(0.09,"xy");
  //gStyle->SetTitleOffset(.45,"xy");
  //gStyle->SetNdivisions(110,"y");

  TCanvas *shower_canv = new TCanvas("shower_canv",
				     "Showers",
				     100,10,2200,1000);
  TH1D *hADC0 = new TH1D("hADC0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADC1 = new TH1D("hADC1",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADCD = new TH1D("hADCD",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hPEAK = new TH1D("hPEAK",";Bin;ADC peak value",nbins,0.,nbins);
  TH1D *hBASE0 = new TH1D("hBASE0","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hBASE1 = new TH1D("hBASE1","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hBASE3 = new TH1D("hBASE3","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hINT0 = new TH1D("hINT0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hINT1 = new TH1D("hINT1",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hINT2 = new TH1D("hINT2","Offline integral;Bin;ADC value",
			 nbins,0.,nbins);
  TPad *padadc0 = new TPad("padadc0","ADC",0.02,0.02,0.48,0.48,0);
  TPad *padadc1 = new TPad("padadc1","ADC",0.52,0.02,0.98,0.48,0);
  TPad *padint0 = new TPad("padint0","Integral",0.02,0.52,0.48,0.98,0);
  TPad *padint1 = new TPad("padint1","Integral",0.52,0.52,0.98,0.98,0);

  shower_canv->Draw();
  padadc0->Draw();
  padadc1->Draw();
  padint0->Draw();
  padint1->Draw();

  event_num = 0;
  printf("Type Enter to continue: ");
  gets(line);

  while (fgets(line,132,inpfile)) {
    if (strncmp(line,">>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>",40) == 0)
      {
	in_event = 1;
	sintegral[0] = 0;
	sintegral[1] = 0;

	hADC0->Reset();
	hADC0->SetLineWidth(3);
	hADC0->SetLineColor(kBlack);
	hADC0->SetAxisRange(650, 750, "X");

	hADC1->Reset();
	hADC1->SetLineWidth(3);
	hADC1->SetLineColor(kBlack);
	hADC1->SetAxisRange(650, 750, "X");

	hADCD->Reset();
        hADCD->SetLineWidth(3);
	hADCD->SetLineColor(kBlack);

	hPEAK->Reset();
        hPEAK->SetLineWidth(3);
	hPEAK->SetLineColor(kBlue);

	hBASE0->Reset();
        hBASE0->SetLineWidth(3);
	hBASE0->SetLineColor(kRed);
	hBASE0->SetMinimum(250.);
	hBASE0->SetMaximum(350.);

	hBASE1->Reset();
        hBASE1->SetLineWidth(3);
	hBASE1->SetLineColor(kRed);
	hBASE1->SetMinimum(250.);
	hBASE1->SetMaximum(350.);

	hBASE3->Reset();
        hBASE3->SetLineWidth(3);
	hBASE3->SetLineColor(kMagenta);

	hINT0->Reset();
        hINT0->SetLineWidth(3);
	hINT0->SetLineColor(kRed);
	hINT0->SetAxisRange(650, 750, "X");

	hINT1->Reset();
        hINT1->SetLineWidth(3);
	hINT1->SetLineColor(kRed);
	hINT1->SetAxisRange(650, 750, "X");

	hINT2->Reset();
        hINT2->SetLineWidth(3);
	hINT2->SetLineColor(kBlack);

	//	hINT->SetMinimum(0.);
	//	hINT->SetMaximum(4000.);

	baseline_sag = 0;
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
	hADC0->Draw();
	hBASE0->Draw("same");
	//	hBASE2->Draw("same");
	//hBASE3->Draw("same");
	//hPEAK->Draw("same");

	padadc1->cd();
	hADC1->Draw();
	hBASE1->Draw("same");
	//	hBASE2->Draw("same");
	//hBASE3->Draw("same");
	//hPEAK->Draw("same");

	padint0->cd();
	hINT0->Draw();
	// hINT2->Draw("same");

	padint1->cd();
	hINT1->Draw();
	// hINT2->Draw("same");

        shower_canv->Modified();
	shower_canv->Update();
	sprintf(line,"event%4.4d.eps\000",event_num);
	shower_canv->SaveAs(line);
	printf("Saturated=%d  Baseline=%7.3lf  Peak=%d  Area=%d  entries=%d  rnum=%d\n",
	       saturated[3], double(base[3])/16., peak[3], area[3], nentries, rnum);
	printf("Plotted event %d.  Type Enter to continue or q to quit: ",
	       event_num);
        nentries = 0;
	gets(line);
	if (strncmp(line,"q",1) == 0) return;

      }
    else if (in_event)
      {
	sscanf(line,"%x %x %x %x %x %x",
	       &ix,&adcraw[0], &adcraw[1], &adcraw[2], &adcraw[3], &adcraw[4]);
	nentries++;
	//	if (nentries < 10)
	//  printf("nentries=%d  ix=%d  adc=%d\n", nentries, ix, adc);

	x = ix+.5;
	adc[0] = (adcraw[0] >> 16) & 0xfff;
	adc[1] = (adcraw[1] >> 16) & 0xfff;
	baseline[0] = double(adcraw[2] & 0xffff)/16.;
	baseline[1] = double((adcraw[2] >> 16) & 0xffff)/16.;
	integral[0] = adcraw[3] & 0x7ffff;
	integral[1] = adcraw[4] & 0x7ffff;
	//baseline_sag = baseline_sag + (adcd-baseline1)*0.0008;
        //baseline2 = baseline1 - baseline_sag;
	//	integral = iv & 0x7ffff;
	//	pk = (iv >> 19) & 0xfff;
	hADC0->Fill(x,double(adc[0]));
	hADC1->Fill(x,double(adc[1]));
	//hADCD->Fill(x,double(adcd));
	//hPEAK->Fill(x-1.,baseline1+double(pk));
	hBASE0->Fill(x-19.,baseline[0]);
	hBASE1->Fill(x-19.,baseline[1]);
	//	hBASE3->Fill(x-4.,double(baseline3)/16.);
	hINT0->Fill(x-19.,double(integral[0]));
	hINT1->Fill(x-19.,double(integral[1]));
	//if ((x > 680.) && (x < 740.)) sintegral = sintegral+adcd-baseline2;
	//hINT2->Fill(x,double(sintegral));
      }

  }

  fclose(inpfile);
}

