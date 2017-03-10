// Header file to set options for for test routine to interact with the
// sde_trigger PL.  This file will be different for Linux & Standalone
// versions, allowing trigger_test.h to be common.
//
// 06-Feb-2017 DFN Initial version extracted from trigger_test.h

//#define STAND_ALONE
#define MAX_EVENTS 100

//#define ADJUST_BASELINE
//#define TOGGLE_WATCHDOG
//#define USE_FAKE_MUON
#define USE_FAKE_SIGNAL
#define USE_FAKE_GPS
//#define DO_LED_PULSE
//#define DO_LED_NOW
#define LED_DELAY0 50
#define LED_DELAY1 100
#define LED_PULSWID0 10
#define LED_PULSWID1 20

#define TRIG_THR0 1000
#define TRIG_THR1 1000
#define TRIG_THR2 1000
#define TRIG_SSD 2040
#define SHWR_TRIGGERS

#ifdef SHWR_TRIGGERS 
  #define COMPAT_SB_TRIGGER
//  #define SB_TRIGGER
//  #define EXT_TRIGGER
//  #define PRESCALE_EXT_TRIGGER
//  #define PRESCALE_COMPAT_SB_TRIGGER
  #define LED_TRIGGER
#endif
//#define MUON_TRIGGERS

// TRIGGGER_POLLED checks sde_trigger module status register for buffers
// to be read in a polling loop.  TRIGGER_INTERRUPT used an interrupt routine
// to process any full buffers.  TRIGGER_POLLED is the only option that
// works in Linux currently.
#define TRIGGER_POLLED
//#define TRIGGER_INTERRUPT

// PDT mode. Read shower/muon buffers using simple memory mapped access.
// This is the only option that works in Linux currently.
#define PDT

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






