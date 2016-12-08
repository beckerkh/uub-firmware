// Header file for simple standalone test routine to interact with the
// sde_trigger PL.
//
// 25-May-2016 DFN Initial version extracted from trigger_test.c
// 05-Dec-2016 DFN Add TOGGLE_WATCHDOG switch


#include <stdio.h>
#include "xparameters.h"  // Peripheral parameters (GPIO addresses, etc.)
#include "xil_types.h"
#include "xil_printf.h"
#include "sde_trigger_defs.h"
#include "time_tagging_defs.h"
#include "time_tagging.h"
#include "xaxicdma.h"
#include "xdebug.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xpseudo_asm_gcc.h"
#include "xreg_cortexa9.h"
#include "test_periph.h"
#include "interface_uub_dfn3.h" 

#define TOGGLE_WATCHDOG
#define USE_FAKE_MUON
//#define USE_FAKE_SIGNAL
#define USE_FAKE_GPS
//#define DO_LED_PULSE
//#define DO_LED_NOW
#define LED_DELAY0 50
#define LED_DELAY1 100
#define LED_PULSWID0 10
#define LED_PULSWID1 20

#define COMPAT_MV_PER_CHAN (2000./(1024.*30.))
#define COMPAT_CHAN_PER_MV (1/COMPAT_MV_PER_CHAN)
//#define TRIG_THR0 50. * COMPAT_CHAN_PER_MV // Set approx 50 mv threshold
#define TRIG_THR0 2040
#define TRIG_THR1 2040
#define TRIG_THR2 2040
#define TRIG_SSD 2040
//#define TRIG_THR1 50. * COMPAT_CHAN_PER_MV // Set approx 50 mv threshold
//#define TRIG_THR2 50. * COMPAT_CHAN_PER_MV // Set approx 50 mv threshold
//#define TRIG_SSD 50. * COMPAT_CHAN_PER_MV // Set ?? threshold

#define MUONS_PER_BUF (MUON_MEM_WORDS/MUON_BURST_LEN)

//#define SHWR_TRIGGERS
#ifdef SHWR_TRIGGERS 
//  #define COMPAT_SB_TRIGGER
  #define SB_TRIGGER
//  #define EXT_TRIGGER
//  #define PRESCALE_EXT_TRIGGER
//  #define PRESCALE_COMPAT_SB_TRIGGER
#endif
#define MUON_TRIGGERS

// PDT mode. Read shower/muon buffers
// using simple memory mapped access.
#define PDT

// TRIGGGER_POLLED checks sde_trigger module status register for buffers
// to be read in a polling loop.  TRIGGER_INTERRUPT used an interrupt routine
// to process any full buffers.
#define TRIGGER_POLLED
//#define TRIGGER_INTERRUPT

// If DMA & SIMPLE are defined then readout polls for completion of read from 
// each of the memory blocks before continuing.  This is sufficient to test
// DMA operatation but it not intended as a model for use.  DMA_INTERRUPT
// is not implemented in this program for the simple DMA mode.
//
// If DMA && SCATTER_GATHER are defined then all of the memory blocks for an
// event are read in one DMA operation.  If in addition, DMA_INTERRUPT is
// is defined, the DMA completion invokes an interrupt routine to finish
// processing the event, otherwise the readout polls for completion of the
// the DMA operation.
//#define DMA
//#define SIMPLE
//#define SCATTER_GATHER
//#define DMA_INTERRUPT

#define ADC_MASK ((1<<ADC_WIDTH)-1)

void enable_trigger_intr();
void disable_trigger_intr();
void read_shw_buffers();
void unpack_shw_buffers();
void check_shw_buffers();
void print_shw_buffers();
void read_muon_buffers();
void unpack_muon_buffers();
void check_muon_buffers();
void print_muon_buffers();
#if defined(DMA) && defined(SIMPLE)
int do_simple_polled_dma(u32 *src, u32 *dest, int length);
#endif
void sde_trigger_handler(void *CallbackRef);

#ifdef SCATTER_GATHER
#define NUM_SHWR_BDS_TO_TRANSFER	5
#define NUM_MUON_BDS_TO_TRANSFER	2
int check_scatter_gather_completion();
int do_scatter_gather_polled_shwr_dma();
int do_scatter_gather_polled_muon_dma();
#endif

// Macros
#define write_trig(RegNumber, Data)					\
  SDE_TRIGGER_mWriteReg(SDE_TRIGGER_BASE, 4*RegNumber, Data)

#define read_trig(RegNumber)						\
  SDE_TRIGGER_mReadReg(SDE_TRIGGER_BASE, 4*RegNumber)

#ifndef TIME_TAGGING_BASE
  #define TIME_TAGGING_BASE XPAR_TIME_TAGGING_0_S00_AXI_BASEADDR
#endif

#define write_ttag(RegNumber, Data)					\
  TIME_TAGGING_mWriteReg(TIME_TAGGING_BASE, 4*RegNumber, Data)

#define read_ttag(RegNumber)						\
  TIME_TAGGING_mReadReg(TIME_TAGGING_BASE, 4*RegNumber)






