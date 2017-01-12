void convert_file(char *ifilename, char *ofilename)
{
#define _GNU_SOURCE

  // Convert minicom capture file to work with latest capture code.

  char line[132];
  int status, in_event, event_num, nentries;
  size_t len = 0;
  ssize_t read;
  int adc[10], flags, filt_adc[3], ix, i;
  int adcraw[5];
  double x;

  FILE *inpfile = fopen(ifilename,"r");
  if (inpfile == 0) {
    printf("Unable to open input file %s\n",ifilename);
    return;
  }
  FILE *outfile = fopen(ofilename,"w");
  if (outfile == 0) {
    printf("Unable to open output file %s\n",ofilename);
    return;
  }


  while (fgets(line,132,inpfile)) {
    if (strncmp(line,">>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>",40) == 0)
      {
	in_event = 1;
	fprintf(outfile,"%s",line);
      }
    else if (strncmp(line,"<<<<<<<<<< END OF EVENT <<<<<<<<<<",34) == 0)
      {
	in_event = 0;
	fprintf(outfile,"%s",line);
      }
    else if (in_event)
      {
	sscanf(line,"%x %x %x %x %x %x %x %x %x",
	       &ix,&flags,&adc[0],&adc[1],&adc[2],&adc[3],&adc[4],
               &adc[5],&adc[6]);
	adcraw[0] = adc[0] | (adc[1] << 16);
	adcraw[1] = adc[2] | (adc[3] << 16);
        adcraw[2] = adc[4] | (adc[5] << 16);
	adcraw[3] = 0;
	adcraw[4] = 0;
	fprintf(outfile,"%x %x %x %x %x %x\n",
		ix,adcraw[0], adcraw[1], adcraw[2], adcraw[3], adcraw[4]);
      }
    else
      {
	fprintf(outfile,"%s",line);
      }
  }

  fclose(inpfile);
  fclose(outfile);
}

