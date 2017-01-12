void plot_integral(char *filename, double xmin, double xmax)
{
#define _GNU_SOURCE
#define INTEGRAL_FRAC_BITS 11

#include <unistd.h>


  // Plot showers from minicom capture file

  char line[132];
  int status, in_event, event_num;
  size_t len = 0;
  ssize_t read;
  int adcraw[5];
  int ix, adc[2],adcd[2],baseline[2],integral[2],integral_computed[2], iv;
  int nbins = 4095;
  int i, j, pk, nentries, rnum;
  double x, baseline_sag[2], baseline_computed[2], baseline_latched[2];
  int saturated[10], base[10], peak[10], area[10];
  int ichan = 0;
  int average_mode = 0;
  int timed_mode = 0;

  double ymin = 300;
  double ymax = 400;
  int baseline_latch_point = 640;
  double rc_decay = 0.0008;
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
				     100,10,2000,1200);
  TH1D *hADC0 = new TH1D("hADC0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADC1 = new TH1D("hADC1",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hADCD = new TH1D("hADCD",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hBASE0 = new TH1D("hBASE0","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hBASE1 = new TH1D("hBASE1","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hBASEC0 = new TH1D("hBASEC0","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hBASEC1 = new TH1D("hBASEC1","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hINT0 = new TH1D("hINT0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hINT1 = new TH1D("hINT1",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hINTC0 = new TH1D("hINTC0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hINTC1 = new TH1D("hINTC1",";Bin;ADC value",nbins,0.,nbins);

  TH1D *hsADC0 = new TH1D("hsADC0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hsADC1 = new TH1D("hsADC1",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hsADCD = new TH1D("hsADCD",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hsBASE0 = new TH1D("hsBASE0","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hsBASE1 = new TH1D("hsBASE1","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hsBASEC0 = new TH1D("hsBASEC0","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hsBASEC1 = new TH1D("hsBASEC1","FPGA baseline;Bin;ADC value",
			  nbins,0.,nbins);
  TH1D *hsINT0 = new TH1D("hsINT0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hsINT1 = new TH1D("hsINT1",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hsINTC0 = new TH1D("hsINTC0",";Bin;ADC value",nbins,0.,nbins);
  TH1D *hsINTC1 = new TH1D("hsINTC1",";Bin;ADC value",nbins,0.,nbins);

  TPad *padadc0 = new TPad("padadc0","ADC",0.02,0.34,0.48,0.66,0);
  TPad *padadc1 = new TPad("padadc1","ADC",0.52,0.34,0.98,0.66,0);
  TPad *padadczm0 = new TPad("padadczm0","ADC",0.02,0.02,0.48,0.32,0);
  TPad *padadczm1 = new TPad("padadczm1","ADC",0.52,0.02,0.98,0.32,0);
  TPad *padint0 = new TPad("padint0","Integral",0.02,0.68,0.48,1.00,0);
  TPad *padint1 = new TPad("padint1","Integral",0.52,0.68,0.98,1.00,0);

  shower_canv->Draw();
  padadc0->Draw();
  padadc1->Draw();
  padint0->Draw();
  padadczm0->Draw();
  padadczm1->Draw();
  padint1->Draw();

  event_num = 0;
  printf("Enter s for single traces, t for timed update, a for average mode, q to quit.\n");
  gets(line);
	if (strncmp(line,"q",1) == 0) return;
	if (strncmp(line,"a",1) == 0) average_mode = 1;
	if (strncmp(line,"t",1) == 0) timed_mode = 1;

  while (fgets(line,132,inpfile)) {
    if (strncmp(line,">>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>",40) == 0)
      {
	in_event = 1;
	integral_computed[0] = 0;
	integral_computed[1] = 0;

	hADC0->Reset();
	hADC0->SetLineWidth(3);
	hADC0->SetLineColor(kBlack);
	hADC0->SetAxisRange(xmin, xmax, "X");

	hADC1->Reset();
	hADC1->SetLineWidth(3);
	hADC1->SetLineColor(kBlack);
	hADC1->SetAxisRange(xmin, xmax, "X");

	hBASE0->Reset();
        hBASE0->SetLineWidth(3);
	hBASE0->SetLineColor(kGreen);
	hBASE0->SetMinimum(ymin);
	hBASE0->SetMaximum(ymax);
	hBASE0->SetAxisRange(xmin, xmax, "X");

	hBASE1->Reset();
        hBASE1->SetLineWidth(3);
	hBASE1->SetLineColor(kGreen);
	hBASE1->SetMinimum(ymin);
	hBASE1->SetMaximum(ymax);
	hBASE1->SetAxisRange(xmin, xmax, "X");

	hBASEC0->Reset();
        hBASEC0->SetLineWidth(3);
	hBASEC0->SetLineColor(kBlue);
	hBASEC0->SetMinimum(ymin);
	hBASEC0->SetMaximum(ymax);
	hBASEC0->SetAxisRange(xmin, xmax, "X");

	hBASEC1->Reset();
        hBASEC1->SetLineWidth(3);
	hBASEC1->SetLineColor(kBlue);
	hBASEC1->SetMinimum(ymin);
	hBASEC1->SetMaximum(ymax);
	hBASEC1->SetAxisRange(xmin, xmax, "X");

	hINT0->Reset();
        hINT0->SetLineWidth(3);
	hINT0->SetLineColor(kRed);
	hINT0->SetAxisRange(xmin, xmax, "X");

	hINT1->Reset();
        hINT1->SetLineWidth(3);
	hINT1->SetLineColor(kRed);
	hINT1->SetAxisRange(xmin, xmax, "X");

	hINTC0->Reset();
        hINTC0->SetLineWidth(3);
	hINTC0->SetLineColor(kCyan);
	hINTC0->SetAxisRange(xmin, xmax, "X");

	hINTC1->Reset();
        hINTC1->SetLineWidth(3);
	hINTC1->SetLineColor(kCyan);
	hINTC1->SetAxisRange(xmin, xmax, "X");

	hsADC0->SetLineWidth(3);
	hsADC0->SetLineColor(kBlack);
	hsADC0->SetAxisRange(xmin, xmax, "X");

	hsADC1->SetLineWidth(3);
	hsADC1->SetLineColor(kBlack);
	hsADC1->SetAxisRange(xmin, xmax, "X");

        hsBASE0->SetLineWidth(3);
	hsBASE0->SetLineColor(kGreen);
	hsBASE0->SetMinimum(ymin);
	hsBASE0->SetMaximum(ymax);
	hsBASE0->SetAxisRange(xmin, xmax, "X");

        hsBASE1->SetLineWidth(3);
	hsBASE1->SetLineColor(kGreen);
	hsBASE1->SetMinimum(ymin);
	hsBASE1->SetMaximum(ymax);
	hsBASE1->SetAxisRange(xmin, xmax, "X");

        hsBASEC0->SetLineWidth(3);
	hsBASEC0->SetLineColor(kBlue);
	hsBASEC0->SetMinimum(ymin);
	hsBASEC0->SetMaximum(ymax);
	hsBASEC0->SetAxisRange(xmin, xmax, "X");

        hsBASEC1->SetLineWidth(3);
	hsBASEC1->SetLineColor(kBlue);
	hsBASEC1->SetMinimum(ymin);
	hsBASEC1->SetMaximum(ymax);
	hsBASEC1->SetAxisRange(xmin, xmax, "X");

        hsINT0->SetLineWidth(3);
	hsINT0->SetLineColor(kRed);
	hsINT0->SetAxisRange(xmin, xmax, "X");

        hsINT1->SetLineWidth(3);
	hsINT1->SetLineColor(kRed);
	hsINT1->SetAxisRange(xmin, xmax, "X");

        hsINTC0->SetLineWidth(3);
	hsINTC0->SetLineColor(kCyan);
	hsINTC0->SetAxisRange(xmin, xmax, "X");

        hsINTC1->SetLineWidth(3);
	hsINTC1->SetLineColor(kCyan);
	hsINTC1->SetAxisRange(xmin, xmax, "X");

	baseline_sag[0] = 0.;
	baseline_sag[1] = 0.;

	// Get header information for the event
	// Need to add reading/writing this to trigger_test
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

	if (average_mode == 0) {
	padadczm0->cd();
	hBASE0->Draw();
	hBASEC0->Draw("same");
	hADC0->Draw("same");
	hINT0->Draw("same");
	hINTC0->Draw("same");

	padadczm1->cd();
	hBASE1->Draw();
	hBASEC1->Draw("same");
	hADC1->Draw("same");
	hINT1->Draw("same");
	hINTC1->Draw("same");

	padadc0->cd();
	hADC0->Draw();
	hBASE0->Draw("same");
	hBASEC0->Draw("same");
	hINT0->Draw("same");
	hINTC0->Draw("same");

	padadc1->cd();
	hADC1->Draw();
	hBASE1->Draw("same");
	hBASEC1->Draw("same");
	hINT1->Draw("same");
	hINTC1->Draw("same");

	padint0->cd();
	hINT0->Draw();
	hINTC0->Draw("same");

	padint1->cd();
	hINT1->Draw();
	hINTC1->Draw("same");
	}

	if (average_mode == 1) {
	padadczm0->cd();

	TH1D *haBASE0 = (TH1D*)(hsBASE0->Clone("haBASE0"));
	haBASE0->Scale(1./event_num);
	haBASE0->SetMinimum(ymin);
	haBASE0->SetMaximum(ymax);
	haBASE0->Draw();

	TH1D *haBASEC0 = (TH1D*)(hsBASEC0->Clone("haBASEC0"));
	haBASEC0->Scale(1./event_num);
	haBASEC0->SetMinimum(ymin);
	haBASEC0->SetMaximum(ymax);
	haBASEC0->Draw("same");

	TH1D *haADC0 = (TH1D*)(hsADC0->Clone("haADC0"));
	haADC0->Scale(1./event_num);
	haADC0->Draw("same");

	TH1D *haINT0 = (TH1D*)(hsINT0->Clone("haINT0"));
	haINT0->Scale(1./event_num);
	haINT0->Draw("same");

	TH1D *haINTC0 = (TH1D*)(hsINTC0->Clone("haINTC0"));
	haINTC0->Scale(1./event_num);
	haINTC0->Draw("same");

	padadczm1->cd();
	TH1D *haBASE1 = (TH1D*)(hsBASE1->Clone("haBASE1"));
	haBASE1->Scale(1./event_num);
	haBASE1->SetMinimum(ymin);
	haBASE1->SetMaximum(ymax);
	haBASE1->Draw();

	TH1D *haBASEC1 = (TH1D*)(hsBASEC1->Clone("haBASEC1"));
	haBASEC1->Scale(1./event_num);
	haBASEC1->SetMinimum(ymin);
	haBASEC1->SetMaximum(ymax);
	haBASEC1->Draw();

	TH1D *haADC1 = (TH1D*)(hsADC1->Clone("haADC1"));
	haADC1->Scale(1./event_num);
	haADC1->Draw("same");

	TH1D *haINT1 = (TH1D*)(hsINT1->Clone("haINT1"));
	haINT1->Scale(1./event_num);
	haINT1->Draw("same");

	TH1D *haINTC1 = (TH1D*)(hsINTC1->Clone("haINTC1"));
	haINTC1->Scale(1./event_num);
	haINTC1->Draw("same");

	padadc0->cd();
	haADC0->Draw();
	haBASE0->Draw("same");
	haBASEC0->Draw("same");
	haINT0->Draw("same");
	haINTC0->Draw("same");

	padadc1->cd();
	haADC1->Draw();
	haBASE1->Draw("same");
	haBASEC1->Draw("same");
	haINT1->Draw("same");
	haINTC1->Draw("same");

	padint0->cd();
	haINT0->Draw();
	haINTC0->Draw("same");

	padint1->cd();
	haINT1->Draw();
	haINTC1->Draw("same");
	}

        shower_canv->Modified();
	shower_canv->Update();
	//	sprintf(line,"event%4.4d.eps\000",event_num);
	// shower_canv->SaveAs(line);
	//	printf("Saturated=%d  Baseline=%7.3lf  Peak=%d  Area=%d  entries=%d  rnum=%d\n",
	//       saturated[3], double(base[3])/16., peak[3], area[3], nentries, rnum);
	printf("Plotted event %d. ", event_num);

	if (timed_mode == 1) 
	  {
	    sleep(3);
	  } else if (average_mode == 0) {
	printf("Type Enter to continue or q to quit: ",
	       event_num);
        nentries = 0;
	gets(line);
	if (strncmp(line,"q",1) == 0) return;
	}
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
	if (ix <= baseline_latch_point)
	  {
	    baseline_latched[0] = baseline[0];
	    baseline_latched[1] = baseline[1];
	    baseline_computed[0] = baseline[0];
	    baseline_computed[1] = baseline[1];
	  } else {
	  baseline_sag[0] = baseline_sag[0] + 
	    (adc[0] - baseline_computed[0])*rc_decay;
	  baseline_computed[0] = baseline_latched[0] - baseline_sag[0];
	  baseline_sag[1] = baseline_sag[1] + 
	    (adc[1] - baseline_computed[1])*rc_decay;
	  baseline_computed[1] = baseline_latched[1] - baseline_sag[1];
	}

	integral[0] = adcraw[3] & 0x7ffff;
	integral[1] = adcraw[4] & 0x7ffff;

	//	pk = (iv >> 19) & 0xfff;
	hADC0->Fill(x,double(adc[0]));
	hADC1->Fill(x,double(adc[1]));
	hBASE0->Fill(x-25.,baseline[0]);
	hBASE1->Fill(x-25.,baseline[1]);
	hINT0->Fill(x-25.,double(integral[0]));
	hINT1->Fill(x-25.,double(integral[1]));
	hBASEC0->Fill(x,baseline_computed[0]);
	hBASEC1->Fill(x,baseline_computed[1]);

	hsADC0->Fill(x,double(adc[0]));
	hsADC1->Fill(x,double(adc[1]));
	hsBASE0->Fill(x-25.,baseline[0]);
	hsBASE1->Fill(x-25.,baseline[1]);
	hsINT0->Fill(x-25.,double(integral[0]));
	hsINT1->Fill(x-25.,double(integral[1]));
	hsBASEC0->Fill(x,baseline_computed[0]);
	hsBASEC1->Fill(x,baseline_computed[1]);

	if (x > baseline_latch_point) {
	  integral_computed[0] = 
	    integral_computed[0]+adc[0]-baseline_computed[0];
	  integral_computed[1] = 
	    integral_computed[1]+adc[1]-baseline_computed[1];
	}
	hINTC0->Fill(x,integral_computed[0]);
	hINTC1->Fill(x,integral_computed[1]);
	hsINTC0->Fill(x,integral_computed[0]);
	hsINTC1->Fill(x,integral_computed[1]);
      }

  }

  fclose(inpfile);
}

