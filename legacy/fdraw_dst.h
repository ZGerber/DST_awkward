/*
 * Genericized BRM/LR raw data.
 * DRB 2008/09/24
 *
 * New bank for BLACK ROCK MESA raw data
 * SRS - 3.12.08
 */

#ifndef _FDRAW_
#define _FDRAW_

#include <stdio.h>
#include "dst_std_types.h"

#define FDRAW_BANKID  12092
#define FDRAW_BANKVERSION   000

#ifdef __cplusplus
extern "C" {
#endif
integer4 fdraw_common_to_bank_();
integer4 fdraw_bank_to_dst_(integer4 *unit);
integer4 fdraw_common_to_dst_(integer4 *unit); /* combines above 2 */
integer4 fdraw_bank_to_common_(integer1 *bank);
integer4 fdraw_common_to_dump_(integer4 *opt) ;
integer4 fdraw_common_to_dumpf_(FILE* fp,integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* fdraw_bank_buffer_ (integer4* fdraw_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


#define fdraw_nmir_max 12             /* number of cameras per site        */
#define fdraw_nchan_mir 256           /* number of tubes per camera        */
#define fdraw_nt_chan_max 512         /* number of time bins per tube      */

typedef struct {
  integer2 event_code;                 /* 1=normal, 0=monte carlo           */ 
  integer2 part;                       /* = run_id % 100                    */
  integer4 num_mir;                    /* number of participating cameras   */
  integer4 event_num;                  /* trigger id number                 */

  /* 
   *  CTD trigger time 
   */
  integer4 julian;                     /* julian day                        */
  integer4 jsecond;                    /* second into julian day            */
  integer4 gps1pps_tick;               /* last 1pps tick from gps           */
  integer4 ctdclock;                   /* ctd 40MHz clock tick              */

  /*
   * Hardware version info
   */
  integer4 ctd_version;
  integer4 tf_version;
  integer4 sdf_version;

  /*
   *  selected TF data
   */
  integer4 trig_code[fdraw_nmir_max];  /* tf trigger code:                  *
					*   0 = not a primary trigger       *
					*   1 = primary trigger             *
					*   2 = joint trigger               *
					*   3, 4 = very large signals       */
  integer4 second[fdraw_nmir_max];     /* camera store time rel. to 0:00 UT */
  integer4 microsec[fdraw_nmir_max];   /*   microsec of store time          */
  integer4 clkcnt[fdraw_nmir_max];     /* camera 40 MHz clock tick          */

  integer2 mir_num[fdraw_nmir_max];    /* mirror id number (0-11)           */ 
  integer2 num_chan[fdraw_nmir_max];   /* number of channels with FADC data */

  integer4 tf_mode[fdraw_nmir_max];
  integer4 tf_mode2[fdraw_nmir_max];

  /* array of triggered tubes by camera (idx 0-255 are tubes, 256 is empty) */
  integer2 hit_pt[fdraw_nmir_max][fdraw_nchan_mir+1];

  /*
   *  selected SDF data
   */
  /* channel ID number */
  integer2 channel[fdraw_nmir_max][fdraw_nchan_mir];

  /* peak timing of input pulse */
  integer2 sdf_peak[fdraw_nmir_max][fdraw_nchan_mir];
  /* internal value for trigg */
  integer2 sdf_tmphit[fdraw_nmir_max][fdraw_nchan_mir];
  /* run mode */
  integer2 sdf_mode[fdraw_nmir_max][fdraw_nchan_mir];
  /* communication mode */
  integer2 sdf_ctrl[fdraw_nmir_max][fdraw_nchan_mir];
  /* for threshold and trigger mode */
  integer2 sdf_thre[fdraw_nmir_max][fdraw_nchan_mir];

  /* average of bkgnd. 0ms, 6ms, 53ms, and 79ms ago. */
  uinteger2 mean[fdraw_nmir_max][fdraw_nchan_mir][4];

  /* rms of bkgnd. 0ms, 6ms, 53ms, and 79ms ago. */
  uinteger2 disp[fdraw_nmir_max][fdraw_nchan_mir][4];

  /* raw 14-bit fadc data */
  integer2 m_fadc[fdraw_nmir_max][fdraw_nchan_mir][fdraw_nt_chan_max];

} fdraw_dst_common;

extern fdraw_dst_common fdraw_;
extern integer4 fdraw_blen; /* needs to be accessed by the c files of the derived banks */ 

integer4 fdraw_struct_to_abank_(fdraw_dst_common *fdraw, integer1* (*pbank), integer4 id, integer4 ver);
integer4 fdraw_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 fdraw_struct_to_dst_(fdraw_dst_common *fdraw, integer1* (*pbank), integer4 *unit, integer4 id, integer4 ver);
integer4 fdraw_abank_to_struct_(integer1 *bank, fdraw_dst_common *fdraw);
integer4 fdraw_struct_to_dump_(integer4 siteid, fdraw_dst_common *fdraw, integer4 *opt);
integer4 fdraw_struct_to_dumpf_(integer4 siteid, fdraw_dst_common *fdraw, FILE *fp, integer4 *opt);

#endif
