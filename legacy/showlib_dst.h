// DST bank definition for shower library entries.
// Based on shower library entries originally created by Andreas Zech for HiRes.
// The Gaisser-Hillas parameterization of a hadronic shower is
//    n(x) = nmax * ((x-x0)/(xmax-x0))^((xmax-x0)/lambda) * exp((xmax-x)/lambda)
//
// showlib_dst.h DRB - 2009/01/07

#ifndef _SHOWLIB_
#define _SHOWLIB_

#include <stdio.h>
#include "dst_std_types.h"

#define SHOWLIB_BANKID      12811
#define SHOWLIB_BANKVERSION   000

#ifdef __cplusplus
extern "C" {
#endif
integer4 showlib_common_to_bank_();
integer4 showlib_bank_to_dst_(integer4 *NumUnit);
integer4 showlib_common_to_dst_(integer4 *NumUnit); // combines above 2
integer4 showlib_bank_to_common_(integer1 *bank);
integer4 showlib_common_to_dump_(integer4 *opt1) ;
integer4 showlib_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* showlib_bank_buffer_ (integer4* showlib_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct {
  integer4 code; 
  /* first number  = species (1 for proton, 2 for iron)
   * second number = energy (second number of log(energy): 7 = 10^17, 0 = 10^20 ....)
   * third number  = not used (for finer energy grid) 
   * fourth+fifth  = zenith angle in degree (rounded value)
   * last number   = hadronic interaction code
   *                 0 = QGSJET
   *                 1 = SIBYLL 1.6
   *                 2 = SIBYLL 2.1
   */       
  integer4  number;    // Number of shower in CORSIKA run
  real4     angle;     // Generation angle (radians)
  integer4  particle;  // Primary particle (CORSIKA convention)
  real4     energy;    // Energy of primary particle in GeV
  real4     first;     // (Slant) Depth of first (actual) interaction in g/cm^2
  real4     nmax;      // GH fit parameter, maximum shower size divided by 1e9
  real4     x0;        // GH fit parameter, shower starting depth in g/cm^2
  real4     xmax;      // GH fit parameter, depth of shower max in g/cm^2
  real4     lambda;    // GH fit parameter, shower development rate in g/cm^2
  real4     chi2;      // Chi-square of CORSIKA's fit of the actual shower to the GH profile
} showlib_dst_common;

extern showlib_dst_common showlib_;

#endif
