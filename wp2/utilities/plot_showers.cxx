void plot_showers(char *filename)
{
      #define _GNU_SOURCE

  // Plot showers from minicom capture file

  char line[132];
  int status, in_event, event_num;
  size_t len = 0;
  ssize_t read;
  double x, y;
  int ix, adc[10], volts[10];
  int nbins = 100;
  double dt = 8.333333;
  double pedestal[10];
  int fir[21] = 
    {5,0,12,22,0,-61,-96,0,256,551,681,551,256,0,-96,-61,0,22,12,0,5};
  int nfircoef = 21;
  int i, j, iy;
  double low_gain = 2000./4096.;
  double high_gain = low_gain/30.;
 
  int plot_adcs = 1;  // Set to 1 to plot raw ADC values
  int plot_volts = 0; // Set to 1 to plot input voltages
  int plot_filt = 1;  // Set to 1 to plot unfiltered & filtered traces

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
				      200,10,1200,700);
  TH1D *hADC0 = new TH1D("hADC0",";Time (ns);ADC value",nbins,0.,dt*nbins);
  TH1D *hADC1 = new TH1D("hADC1",";Time (ns);ADC value",nbins,0.,dt*nbins);
  TH1D *hADC2 = new TH1D("hADC2",";Time (ns);ADC value",nbins,0.,dt*nbins);
  TH1D *hADC3 = new TH1D("hADC3",";Time (ns);ADC value",nbins,0.,dt*nbins);
  TH1D *hADC4 = new TH1D("hADC4",";Time (ns);ADC value",nbins,0.,dt*nbins);
  TH1D *hADC5 = new TH1D("hADC5",";Time (ns);ADC value",nbins,0.,dt*nbins);
  TH1D *hVolts0 = new TH1D("Volts0","PMT0 low gain;Time (ns);Signal (mV)",
			   nbins,0.,dt*nbins);
  TH1D *hVolts1 = new TH1D("Volts1","PMT0 high gain;Time (ns);Signal (mV)",
			   nbins,0.,dt*nbins);
  TH1D *hVolts2 = new TH1D("Volts2","PMT1 low gain;Time (ns);Signal (mV)",
			   nbins,0.,dt*nbins);
  TH1D *hVolts3 = new TH1D("Volts3","PMT1 high gain;Time (ns);Signal (mV)",
			   nbins,0.,dt*nbins);
  TH1D *hVolts4 = new TH1D("Volts4","PMT2 low gain;Time (ns);Signal (mV)",
			   nbins,0.,dt*nbins);
  TH1D *hVolts5 = new TH1D("Volts5","PMT2 high gain;Time (ns);Signal (mV)",
			   nbins,0.,dt*nbins);
  TH1D *hFilt0 = new TH1D("Filt0","PMT0 low gain;Time (ns);Signal",
			   nbins,0.,dt*nbins);
  TH1D *hFilt1 = new TH1D("Filt1","PMT0 high gain;Time (ns);Signal",
			   nbins,0.,dt*nbins);
  TH1D *hFilt2 = new TH1D("Filt2","PMT1 high gain;Time (ns);Signal",
			   nbins,0.,dt*nbins);

   TPad *pad0 = new TPad("pad0","ADC0",0.02,0.52,0.31,0.98,0);
   TPad *pad1 = new TPad("pad1","ADC1",0.33,0.52,0.64,0.98,0);
   TPad *pad2 = new TPad("pad2","ADC2",0.66,0.52,0.98,0.98,0);
   TPad *pad3 = new TPad("pad3","ADC3",0.02,0.02,0.31,0.48,0);
   TPad *pad4 = new TPad("pad4","ADC4",0.33,0.02,0.64,0.48,0);
   TPad *pad5 = new TPad("pad5","ADC5",0.66,0.02,0.98,0.48,0);
   pad0->Draw();
   pad1->Draw();
   pad2->Draw();
   pad3->Draw();
   pad4->Draw();
   pad5->Draw();
    
  event_num = 0;
  while (fgets(line,132,inpfile)) {
    //    printf("%s,  len=%d\n", line, strlen(line));
    if (strncmp(line,">>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>",40) == 0)
      {
	//	printf("Found beginning of event\n");
	in_event = 1;
	x = 0.;
	hADC0->Reset();
	hADC1->Reset();
	hADC2->Reset();
	hADC3->Reset();
	hADC4->Reset();
	hADC5->Reset();
	hVolts0->Reset();
	hVolts1->Reset();
	hVolts2->Reset();
	hVolts3->Reset();
	hVolts4->Reset();
	hVolts5->Reset();
	hFilt0->Reset();
	hFilt1->Reset();
	hFilt2->Reset();
      }
    if (strncmp(line,"<<<<<<<<<< END OF EVENT <<<<<<<<<<",34) == 0)
      {
	//	printf("Found end of event\n");
	in_event = 0;
	event_num ++;

        // Make filtered histograms
        if (plot_filt)
          x = dt+nfircoef/2;
          {
            for (i=21; i<nbins; i++)
              {
                x = x+dt;
                if (plot_adcs)
                  {
                    iy = 0;
                    for (j=0; j<nfircoef; j++)
                      {
                        iy = iy + 
                          hADC1->GetBinContent(i-nfircoef+j+1)*fir[j];
                      }
                    iy = iy/2048;
                    hFilt0->Fill(x,double(iy));
                  }
              }
          }

	// Draw all the histograms

	pad0->cd();
	if (plot_adcs) hADC0->Draw();
	if (plot_volts) hVolts0->Draw();
	pad1->cd();
	if (plot_adcs) hADC1->Draw();
	if (plot_volts) hVolts1->Draw();
        if (plot_filt) hFilt0->Draw("a");
	pad2->cd();
	if (plot_adcs) hADC2->Draw();
	if (plot_volts) hVolts2->Draw();
	pad3->cd();
	if (plot_adcs) hADC3->Draw();
	if (plot_volts) hVolts3->Draw();
	pad4->cd();
	if (plot_adcs) hADC4->Draw();
	if (plot_volts) hVolts4->Draw();
	pad5->cd();
	if (plot_adcs) hADC5->Draw();
	if (plot_volts) hVolts5->Draw();

	shower_canv->Modified();
	shower_canv->Update();
	printf("Plotted event %d.  Type Enter to continue: ", event_num);
	gets(line);
      }
    if (in_event)
      {
	sscanf(line,"%3x%4x%4x%4x%4x%4x%4x%4x%4x",
	       &ix,&adc[0],&adc[1],&adc[2],&adc[3],&adc[4],
	       &adc[5],&adc[6],&adc[7]);
	//	printf("%4x,%4x,%4x,%4x,%4x,%4x,%4x,%4x,%4x\n",
	//       ix,adc[0],adc[1],adc[2],adc[3],adc[4],
	//       adc[5],adc[6],adc[7]);

	// Convert from ADC counts to volts at input
	volts[0] = adc[0]*low_gain;
	volts[2] = adc[2]*low_gain;
	volts[4] = adc[4]*low_gain;
	volts[1] = adc[1]*high_gain;
	volts[3] = adc[3]*high_gain;
	volts[5] = adc[5]*high_gain;

	hADC0->Fill(x,double(adc[0]));
	hADC1->Fill(x,double(adc[1]));
	hADC2->Fill(x,double(adc[2]));
	hADC3->Fill(x,double(adc[3]));
	hADC4->Fill(x,double(adc[4]));
	hADC5->Fill(x,double(adc[5]));

	hVolts0->Fill(x,volts[0]);
	hVolts1->Fill(x,volts[1]);
	hVolts2->Fill(x,volts[2]);
	hVolts3->Fill(x,volts[3]);
	hVolts4->Fill(x,volts[4]);
	hVolts5->Fill(x,volts[5]);
	x = x+dt;
      }

  }

  fclose(inpfile);
}

