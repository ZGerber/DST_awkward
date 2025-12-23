/*
 *     New Bank for raw fadc 1 values
 *     MRM July 18
*/

#ifndef _FRAW1_
#define _FRAW1_

#include <stdio.h>
#include "dst_std_types.h"

#define FRAW1_BANKID  12001
#define FRAW1_BANKVERSION   000

#ifdef __cplusplus
extern "C" {
#endif
integer4 fraw1_common_to_bank_();
integer4 fraw1_bank_to_dst_(integer4 *NumUnit);
integer4 fraw1_common_to_dst_(integer4 *NumUnit); /* combines above 2 */
integer4 fraw1_bank_to_common_(integer1 *bank);
integer4 fraw1_common_to_dump_(integer4 *opt1) ;
integer4 fraw1_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* fraw1_bank_buffer_ (integer4* fraw1_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif

integer4 fraw1_time_print_(integer4 *second, integer4 *clkcnt);
integer4 fraw1_time_fprint(FILE* fp, integer4 *second, integer4 *clkcnt);

#define fraw1_nmir_max 20           /*  max number of mirrors per site */
#define fraw1_nchan_mir 320         /*  320 channels of FADC per mirror
				     *    1-256 = High-gain
				     *  257-272 =  Trigger, cols 1-16
				     *  273-288 = Low-gain, cols 1-16
				     *  289-304 =  Trigger, rows 1-16
				     *  305-320 = Low-gain, rows 1-16 */
#define fraw1_nt_chan_max 2048       /*  max number of time bins per channel */

typedef struct
{
  integer2 event_code;  /* 0=messages, 1= normal, 2= special readout, 
                           3=snapshot, 4=bigshot, 5= intersite trig. */

  integer2 site,part;
  integer2 num_mir;                 /*  number of participating mirrors */

  integer4 event_num;
                     /* run start in day,second,nsec */
  integer4 julian;     
  integer4 jsecond;
  integer4 jclkcnt; /* this number is nanoseconds */

  /* next two variables are mirror store start time measured since run start */

  integer4 second[fraw1_nmir_max];      /* mirror store start time */
  integer4 clkcnt[fraw1_nmir_max];      /* this number is 60 MhZ clock counts */

  integer2 mir_num[fraw1_nmir_max];      /*  mirror number (1-63) */ 
  integer2 num_chan[fraw1_nmir_max];     /*  number of channels with raw FADC data
				    *  stored in mirror */
  
  integer2 channel[fraw1_nmir_max][fraw1_nchan_mir];  /*  channel number */
  integer2 it0_chan[fraw1_nmir_max][fraw1_nchan_mir]; /*  m2addr of readout start  */
  integer2 nt_chan[fraw1_nmir_max][fraw1_nchan_mir];  /*  number of digitizations recorded
					  *  for each channel */
  integer1 m_fadc[fraw1_nmir_max][fraw1_nchan_mir][fraw1_nt_chan_max]; /*  raw 8-bit fadc data */
} fraw1_dst_common;

extern fraw1_dst_common fraw1_;

#endif











