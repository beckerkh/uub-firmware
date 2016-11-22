// Utility routines for reading, unpacking, checking muon buffers.
//
// 21-Nov-2016 DFN Initial version derived from stand alone code.

#include "uub_sipm_cal.h"

  // Read muon memory buffers from PL memory into PS memory via PDT
  void read_muon_buffers()
  {
    volatile u32 *mem_ptr; 
    int i;

    mu_word_count = read_trig(MUON_BUF_WORD_COUNT_ADDR);

    // Muon buffer 0
    mem_ptr = muon_buf0 + toread_muon_buf_num * MUON_MEM_WORDS;
    for (i=0; i<mu_word_count; i++)
      {
        muon_mem0[i] = *mem_ptr;
        mem_ptr++;
      }

    // Muon buffer 1
    mem_ptr = muon_buf1 + toread_muon_buf_num * MUON_MEM_WORDS;
    for (i=0; i<mu_word_count; i++)
      {
        muon_mem1[i] = *mem_ptr;
        mem_ptr++;
      }
  }

  // Unpack muon memory buffers 
  void unpack_muon_buffers()
  {
    int i, j, k;

    j = 0;
    k = 0;

    for (i=0; i<mu_word_count; i++)
      {
        // Burst time tag?
        if ((muon_mem0[i] & 0x80000000) != 0)
      {
        muon_burst_start[j] = muon_mem0[i] & 0x7fffffff;
        muon_trig_tags[j] = muon_mem1[i] & 0x7fffffff;
        j++;
        k = 0;
      } else {
        muon_adc[0][k][j-1] = muon_mem0[i] & ADC_MASK;
        muon_adc[1][k][j-1] = (muon_mem0[i]>>16) & ADC_MASK;
        muon_adc[2][k][j-1] = muon_mem1[i] & ADC_MASK;
        muon_adc[3][k][j-1] = (muon_mem1[i]>>16) & ADC_MASK;
        muon_seq_num[k][j-1] = ((muon_mem1[i]>>12) & 0xf) 
          | (((muon_mem1[i]>>28) & 0x7) << 4);
        k++;
        }
      }
  }

  void check_muon_buffers()
  {
    int i, j, nmuons, corrupt;

    nmuons = mu_word_count/(MUON_BURST_LEN);

   for (i=0; i<nmuons; i++)
      {
        corrupt = 0;
        for (j=0; j<MUON_BURST_LEN-1; j++)
          {
            if (muon_seq_num[j][i]-1 != j)
              corrupt = 1;
          }
        if (corrupt != 0)
          {   
          printf("trigger_test: Corrupt muon buffer for muon %d\n",i);
          j = 0;
            printf("Muon number %3d  expected sequence num= %2d  Actual seq. num = %d\n",
                   i, j+1, muon_seq_num[j][i]);
            j = MUON_BURST_LEN-3;
              printf("Muon number %3d  expected sequence num= %2d  Actual seq. num = %d\n",
                     i, j+1, muon_seq_num[j][i]);
              j = MUON_BURST_LEN-2;
                printf("Muon number %3d  expected sequence num= %2d  Actual seq. num = %d\n",
                       i, j+1, muon_seq_num[j][i]);
         }
      }
  }

void print_muon_buffers()
{
  int i, j, nmuons;

  nmuons = mu_word_count/(MUON_BURST_LEN);
  printf("\n>>>>>>>>>> BEGINNING OF MUON BUFFER >>>>>>>>>>\n");
  for (i=0; i<nmuons; i++)
    {
      for (j=0; j<MUON_BURST_LEN-1; j++) {
        printf("%3d %2d %3x %3x %3x %3x %2x\n",
               i, j+1, muon_adc[0][j][i], muon_adc[1][j][i],
               muon_adc[2][j][i],  muon_adc[3][j][i], 
               muon_trig_tags[i]);
      }
    }
  printf("<<<<<<<<<< END OF MUON BUFFER <<<<<<<<<<\n\n");
}




