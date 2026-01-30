/*
 * $Source:$
 * $Log:$
*/

#ifndef _STPLN_
#define _STPLN_

#include <stdio.h>
#include "dst_std_types.h"

#include "univ_dst.h"
#include "mc04_detector.h"

#define STPLN_BANKID 15043 
#define STPLN_BANKVERSION 2 
#define STPLN_BAD_THCAL1 -1000.0
#define STPLN_BAD_PRXF -100000.0

#ifdef __cplusplus
extern "C" {
#endif
integer4 stpln_common_to_bank_();
integer4 stpln_bank_to_dst_(integer4 *NumUnit);
integer4 stpln_common_to_dst_(integer4 *NumUnit); /* combines above 2 */
integer4 stpln_bank_to_common_(integer1 *bank);
integer4 stpln_common_to_dump_(integer4 *long_output);
integer4 stpln_common_to_dumpf_(FILE* fp, integer4 *long_output);
/* get (packed) buffer pointer and size */
integer1* stpln_bank_buffer_ (integer4* stpln_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


#define STPLN_TUBE_NOT_SATURATED            0
#define STPLN_SAH_TUBE_SATURATED            1
#define STPLN_TALE_TUBE_SATURATED_NOT_FIXED 3
#define STPLN_TALE_TUBE_SATURATED_FIXED     33



typedef struct
{
  /* 2440000 subtracted from the julian day to give room for millisecond 
     precision.
     checks: "1/1/1985,0.0hr UT" gives 6066 + 0.5;
     jday=0 gives 1968 May 23.5 UT
  */
  integer4 jday;        /* mean julian day - 2.44e6 */
  integer4 jsec;        /* second into Julian day */
  integer4 msec;        /* milli sec of julian day (NOT since UT0:00) */

  integer4 neye;        /* number of sites triggered */
  integer4 nmir;        /* number of mirrors for this event */
  integer4 ntube;       /* total number of tubes for this event */
	
  /*
    if ( if_eye[ieye] != 1) ignore site
   */
  integer4 maxeye;
  integer4 if_eye      [MC04_MAXEYE];

  /* -------------- eye info  ---------------*/

  integer4 eyeid       [MC04_MAXEYE];
  integer4 eye_nmir    [MC04_MAXEYE];        /* number of mirrors for this event */
  integer4 eye_ngmir   [MC04_MAXEYE];        /* number of mirrors for this event */
  integer4 eye_ntube   [MC04_MAXEYE];       /* total number of tubes for this event */
  integer4 eye_ngtube  [MC04_MAXEYE];

  real4    n_ampwt     [MC04_MAXEYE][3];/* amplitude weighted plane normal */
  real4    errn_ampwt  [MC04_MAXEYE][6];/* error in n_ampwt[]              */

  real4    rmsdevpln   [MC04_MAXEYE];   /* rms deviation in offplane angle (rad)  */
  real4    rmsdevtim   [MC04_MAXEYE];   /* rms deviation in tube trigger time from
                                         time fit to a quadratic (microseconds) */

  real4    tracklength [MC04_MAXEYE];  /* tracklength in degrees */
  real4    crossingtime[MC04_MAXEYE];  /* time difference between last and first
                                          good tubes to trigger (microseconds) */
  real4    ph_per_gtube[MC04_MAXEYE];  /* average number of photons per good tube */

  /* -------------- mir info, one for each of nmir mirrors  ---------------*/

  integer4 mirid     [MC04_MAXMIR];  /* mirror # (id), saved as short */
  integer4 mir_eye   [MC04_MAXMIR];  /* eye # (id), saved as short */
  integer4 mir_type  [MC04_MAXMIR];  /* Hires/TA/ToP */
  integer4 mir_ngtube[MC04_MAXMIR];  /* # of tubes for that mir */
  integer4 mirtime_ns[MC04_MAXMIR];  /* time of mir. holdoff in ns from sec. */

  /* -------------- tube info, one for each of ntube tubes  ---------------*/
  integer4 ig        [MC04_MAXTUBE];  /* ig = 1 is for good tubes, 0 for noise tubes */
  integer4 tube_eye  [MC04_MAXTUBE];  /* eye #, saved with tube as short */
  /* STPLN VERSION 2 VARIABLES, added by DI on 02/23/2016 */
  integer4 saturated[MC04_MAXTUBE];   /* tube saturation flag */
  integer4 mir_tube_id[MC04_MAXTUBE]; /* mir_tube_id = (mirror_id*1000+tube_id) */

} stpln_dst_common ;

extern stpln_dst_common stpln_ ; 

#endif
