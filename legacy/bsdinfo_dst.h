/*
 *     Bank for detailed information on SDs that are either completely out 
 *     or not working properly during the event readout
 *     Dmitri Ivanov (dmiivanov@gmail.com)
 *     May 16, 2017
 *     Last modified: Mar 6, 2019
*/

#ifndef _BSDINFO_
#define _BSDINFO_

#include <stdio.h>
#include "dst_std_types.h"

#define BSDINFO_BANKID  13112
#define BSDINFO_BANKVERSION   002

#ifdef __cplusplus
extern "C" {
#endif
integer4 bsdinfo_common_to_bank_ ();
integer4 bsdinfo_bank_to_dst_ (integer4 * NumUnit);
integer4 bsdinfo_common_to_dst_ (integer4 * NumUnit);	/* combines above 2 */
integer4 bsdinfo_bank_to_common_ (integer1 * bank);
integer4 bsdinfo_common_to_dump_ (integer4 * opt1);
integer4 bsdinfo_common_to_dumpf_ (FILE * fp, integer4 * opt2);
integer1* bsdinfo_bank_buffer_ (integer4* bsdinfo_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif

#define BSDINFO_NBSDS 1024 /* maximum number of SDs in the event readout */
#define BSDINFO_NBITS 16   /* number of bits to describe what's wrong with the SD */
#define BSDINFO_DST_GZ ".bsdinfo.dst.gz" /* output suffix */


typedef struct
{
  int yymmdd;               /* date, YYMMDD format */
  int hhmmss;               /* time, HHMMSS format */
  int usec;                 /* micro second */
  int nbsds;                /* number of SDs that are part of event readout but not working properly */
  int xxyy[BSDINFO_NBSDS];  /* position IDs of bad SDs */
  int bitf[BSDINFO_NBSDS];  /* bit flag that describes what's wrong with the SD, 
			       if either of the 16 bits is set, there is a problem: */
  /* Checks that are done during TA SD Monte Carlo generation.  If either
     of these conditions failed. then the SD is not used in the Monte Carlo.
     bit 0:  ICRR calibration issue, failed ICRR don't use criteria
     bit 1:  ICRR calibration issue, Mev2pe problem
     bit 2:  ICRR calibration issue, Mev2cnt problem
     bit 3:  ICRR calibration issue, bad pedestal mean values
     bit 4:  ICRR calibration issue, bad pedestal standard deviation
     bit 5:  ICRR calibration issue, saturation information not available  
     bit 6:  Rutgers calibration issue, bad mip values
     bit 7:  Rutgers calibration issue, bad pedestal peak channel
     bit 8:  Rutgers calibration issue, bad pedestal right half peak channel
     bit 9:  Rutgers calibration issue, bad 1-MIP peak fit number of degrees of freedom
     bit 10: Rutgers calibration issue, bad 1-MIP peak fit chi2
     Checks done the during event reconstruction.  If either of these fail then the counter
     is not used in the event reconstruction.
     bit 11: Rutgers calibration issue, peak channel of pedestal histogram
     bit 12: Rutgers calibration issue, peak channel of 1-MIP histogram
     bit 13: Rutgers calibration issue, 1-MIP histogram fit number of degrees of freedom
     bit 14: Rutgers calibration issue, 1-MIP histogram chi2 / dof
     bit 15: Rutgers calibration issue, FADC counts per VEM */
  int nsdsout;                /* number of SDs either completely out (absent in the live detector list during event) or not working properly */
  int xxyyout[BSDINFO_NBSDS]; /* SDs that are completely out (can't participate in event readout) */
  int bitfout[BSDINFO_NBSDS]; /* Bit flags of SDs that are considered as completely out, if not available then 0xFFFF */ 
} bsdinfo_dst_common;

extern bsdinfo_dst_common bsdinfo_;

#endif
