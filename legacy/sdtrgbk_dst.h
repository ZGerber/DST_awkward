/*
 *     Bank for SD trigger backup
 *     Dmitri Ivanov (ivanov@physics.rutgers.edu)
 *     Jan 12, 2009
 *     Last Modified: Feb 20, 2010

 */

#ifndef _SDTRGBK_
#define _SDTRGBK_

#include <stdio.h>
#include "dst_std_types.h"

#define SDTRGBK_BANKID  13109
#define SDTRGBK_BANKVERSION   001

#ifdef __cplusplus
extern "C" {
#endif
integer4 sdtrgbk_common_to_bank_();
integer4 sdtrgbk_bank_to_dst_(integer4 * NumUnit);
integer4 sdtrgbk_common_to_dst_(integer4 * NumUnit); /* combines above 2 */
integer4 sdtrgbk_bank_to_common_(integer1 * bank);
integer4 sdtrgbk_common_to_dump_(integer4 * opt1);
integer4 sdtrgbk_common_to_dumpf_(FILE * fp, integer4 * opt2);
/* get (packed) buffer pointer and size */
integer1* sdtrgbk_bank_buffer_ (integer4* sdtrgbk_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


/*
  -------- LEVEL-X TRIGGER NOTATION: ---------------------
  LEVEL-0 TRIGGER  - 15 FADC COUNTS ABOVE PEDESTAL
  LEVEL-1 TRIGGER  - 150 FADC COUNTS ABOVE PEDESTAL
  LEVEL-2 TRIGGER (EVENT) - 3 LEVEL-1 TRIGGER SDS IN A CERTAIN PATTENR IN CERTAIN TIME WINDOW
*/

#define SDTRGBK_NSD 256
#define SDTRGBK_NSIGPSD 1280

typedef struct
{

  // second fractions for the level-1 signals
  // (corresponds to the left edge of the sliding window)
  // [S]
  real8 secf[SDTRGBK_NSD][SDTRGBK_NSIGPSD];

  // time limits for each SD,
  // [0] - minimum possible time,
  // [1] - maximum possible time [S]
  real8 tlim[SDTRGBK_NSD][2];

  // BANK ID of the raw SD data bank used
  // Two possibilities:
  // RUSDRAW_BANKID
  // TASDCALIBEV_BANKID
  integer4 raw_bankid;

  // FADC time slices for the level-1 signals
  // (left edge of the sliding window)
  integer2 ich[SDTRGBK_NSD][SDTRGBK_NSIGPSD];

  // signal size (FADC counts) for the level-1 signals,
  // [0] = lower, [1] - upper
  integer2 q[SDTRGBK_NSD][SDTRGBK_NSIGPSD][2];

  // Waveform index inside the raw SD data bank
  // to tell from what waveform a given level-1 trigger
  // signal comes.
  integer2 l1sig_wfindex[SDTRGBK_NSD][SDTRGBK_NSIGPSD];

  integer2 xxyy[SDTRGBK_NSD]; // counter position ID

  // index inside raw SD data bank
  // of the waveform that was used for
  // calibrating this SD
  integer2 wfindex_cal[SDTRGBK_NSD];

  // number of (level-1) signals that are above 150 FADC counts
  integer2 nl1[SDTRGBK_NSD];

  integer2 nsd; // number of SDs

  integer2 n_bad_ped; // number of SDs with bad pedestals

  integer2 n_spat_cont; // number of spatially contiguous SDs

  integer2 n_isol; // number of isolated SDs - SDs not taking part in space trigger patterns

  // number of SDs that take part in space trigger pattern
  // and that have waveforms which connect in time with
  // parts of waveforms in other SDs in a space trigger pattern
  integer2 n_pot_st_cont;

  // number of SDs out of n_pot_spat_time_cont that also have level-1 trigger signals in them
  integer2 n_l1_tg;

  // by how much pedestal inside
  // the sliding window had to be decreased
  // to get the level-2 trigger
  integer2 dec_ped;
  
  // If the event triggers fine with dec_ped=0,
  // then this variable will tell by 
  // how much one can reaise the pedestal 
  // inside the sliding window and still
  // have the event trigger
  integer2 inc_ped;

  integer2 il2sd[3]; // indices of SDs that caused level-2 trigger (within this bank)
  integer2 il2sd_sig[3]; // indices of signals in each SD causing level-2 trigger

  // SD goodness flag.
  //    0 - SD has bad pedestals
  //    1 - SD has good pedestals but doesn't participate in space trigger patterns
  //    2 - SD participates in spatial trigger patterns
  //    3 - SD participates in spatial trigger patterns and parts of its waveforms 
  //        connect with parts of waveforms in other SDs in a space trigger pattern 
  //        with a given SD
  //    4 - SD satisfies the ig[isd] = 3 criteria and has level-1 signals in it
  //    5 - SD satisfies the ig[isd] = 4 criteria and happened to be picked for the event trigger
  //    6 - SD satisfies the ig[isd] = 4 criteria and happened to be picjed for the event trigger
  //        with raised pedestals but was not chosen in the event trigger 
  //        w/o raising the pedestal (ig[isd] = 5 criteria)
  //    7 - SD satisfies the ig[isd] = 5 criteria and also participates in the
  //        event trigger with raised pedestals
  integer1 ig[SDTRGBK_NSD];

  // trigger pattern     
  //   IF TRIGGERED:
  //      0 - triangle
  //      1 - line
  //   IF DID NOT TRIGGER:
  //      0 - don't have at least 3 space contiguous SDs with good pedestals
  //      1 - don't have at least 3 SDs in the set of space-contiguous SDs 
  //          with waveforms whose parts connect in time with parts of waveforms in other
  //          SDs that formed a spatial trigger pattern with a given SD
  //      2 - don't have at least 3 level-1 signal SDs in the set of potentially space-time
  //          contiguous SDs
  //      3 - don't have at least 3 SDs with level-1 signals that are in time
  //          (in the set of potentially space-time contiguous SDs)
  integer1 trigp;
  
  // event goodness flag:
  //   0 - event doesn't pass SD trigger backup even with lowered pedestals
  //   1 - event passes SD trigger backup with lowered pedestals
  //   2 - event passes SD trigger backup without lowering the pedestals
  //   3 - event passes SD trigger backup with raised pedestals
  integer1 igevent; 


} sdtrgbk_dst_common;

extern sdtrgbk_dst_common sdtrgbk_;

#endif
