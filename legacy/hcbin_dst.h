/*
 * hcbin_dst.h
 *
 * $Source: /hires_soft/cvsroot/bank/hcbin_dst.h,v $
 * $Log: hcbin_dst.h,v $
 * Revision 1.3  2001/10/18 00:34:24  wiencke
 * modified comments, no functional changes
 *
 * Revision 1.2  2001/06/29 22:55:13  wiencke
 * added documentation only
 * no functional changes of any kind
 *
 * Revision 1.1  1999/06/28 21:04:21  tareq
 * Initial revision
 *
 *
 * light flux bin information
 * used for analysis of BigH data
 *
 * This bank is designed to work in conjunction with mjk's PRFC bank in
 * the context of the "profile constraint geometry fit".  The construction
 * of this bank mimics PRFC bank.
 * 
 */

/* Comments added 6-29-2001 , modified 10-17-10 by lrw */
/*
 * This bank contains samples of the light profile of an event just before the
 * light reaches the detector.  It does not include corrections of atmospheric
 * effects or distance to the shower.
 *
 * The samples correspond to points along the shower detector plane.  The
 * exact nature of the samples depend on the program that generated them.
 * 
 * Historically this has been called the "bin signal".  It is given in units of
 * photoelectrons/m^2/degree.
 *
 * The "bin signal" and other parameters have two indicies.  The first index
 * corresponds to different types of fits and/or measurements.  In the case of stereo
 * analysis index 0 corresponds to a fit to measure hires1 data.  Index 1 corresponds
 * to a fit to measured hires2 data.  The second index corresponds to the bin number
 * along the track.
 *
 * End comments of 6-29-2001 */

#ifndef _HCBIN_
#define _HCBIN_

#include <stdio.h>
#include "dst_std_types.h"

#define HCBIN_BANKID 15007
#define HCBIN_BANKVERSION 0 

#define HCBIN_MAXFIT 16   /* cross check with PRFC_MAXFIT in prfc_dst.h */
#define HCBIN_MAXBIN 300  /* cross check with PRFC_MAXFIT in prfc_dst.h */

#define HCBIN_BININFO_USED    1
#define HCBIN_BININFO_UNUSED  0

/* status/error codes defined in parallel to those in prfc_dst.h */

#define HCBIN_FIT_NOT_REQUESTED         1
#define HCBIN_NOT_IMPLEMENTED           2
#define HCBIN_REQUIRED_BANKS_MISSING    3
#define HCBIN_MISSING_TRAJECTORY_INFO   4
#define HCBIN_UPWARD_GOING_TRACK       10
#define HCBIN_TOO_FEW_GOOD_BINS        11
#define HCBIN_FITTER_FAILURE           12
#define HCBIN_INSANE_TRAJECTORY        13

#define HCBIN_STAT_ERROR_FAILURE        1
#define HCBIN_RIGHT_ERROR_FAILURE       2
#define HCBIN_LEFT_ERROR_FAILURE        4

#define HCBIN_IG_GOODBIN                1
#define HCBIN_IG_OVERCORRECTED          0
#define HCBIN_IG_SICKPLNFIT            -1
#define HCBIN_IG_CHERENKOV_CUT         -2

#ifdef __cplusplus
extern "C" {
#endif
integer4 hcbin_common_to_bank_(void);
integer4 hcbin_bank_to_dst_(integer4 *NumUnit);
integer4 hcbin_common_to_dst_(integer4 *NumUnit); /* combines above 2 */
integer4 hcbin_bank_to_common_(integer1 *bank);
integer4 hcbin_common_to_dump_(integer4 *long_output);
integer4 hcbin_common_to_dumpf_(FILE* fp, integer4 *long_output);
/* get (packed) buffer pointer and size */
integer1* hcbin_bank_buffer_ (integer4* hcbin_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif



typedef struct 
{

  real8 bvx   [HCBIN_MAXFIT] [HCBIN_MAXBIN];
  real8 bvy   [HCBIN_MAXFIT] [HCBIN_MAXBIN];
  real8 bvz   [HCBIN_MAXFIT] [HCBIN_MAXBIN];
  real8 bsz   [HCBIN_MAXFIT] [HCBIN_MAXBIN]; /* bin size in degrees */   
  real8 sig   [HCBIN_MAXFIT] [HCBIN_MAXBIN]; /* signal in pe/degree/m^2 */
  real8 sigerr[HCBIN_MAXFIT] [HCBIN_MAXBIN]; /* error on the signal */
  real8 cfc   [HCBIN_MAXFIT] [HCBIN_MAXBIN]; /* correction factor or exposure
                                                of the bin in degree*MRAREA */
  integer4 ig      [HCBIN_MAXFIT] [HCBIN_MAXBIN];   /*  ig=  1: good bin */
  integer4 nbin    [HCBIN_MAXFIT];                  /* number of bins */
  integer4 failmode[HCBIN_MAXFIT];                  /* 0 ==> Success */
  integer4 bininfo [HCBIN_MAXFIT];

  integer4 jday[HCBIN_MAXFIT];   /* mean julian day - 2.44e6 */
  integer4 jsec[HCBIN_MAXFIT];   /* second into Julian day */
  integer4 msec[HCBIN_MAXFIT];   /* milli sec of julian day 
                                    (NOT since UT0:00) */
} hcbin_dst_common ;
extern hcbin_dst_common hcbin_ ; 

#endif


















