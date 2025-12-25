/* Created 2010/04 LMS */

#ifndef _STTUBEPROFILE_DST_
#define _STTUBEPROFILE_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "stplane_dst.h"

#define STTUBEPROFILE_BANKID        20002
#define STTUBEPROFILE_BANKVERSION   001

#define STTUBEPROF_MAXTUBE          (STPLANE_MAXTUBE)

#ifdef __cplusplus
extern "C" {
#endif
integer4 sttubeprofile_common_to_bank_();
integer4 sttubeprofile_bank_to_dst_(integer4 *unit);
integer4 sttubeprofile_common_to_dst_(integer4 *unit); // combines above 2
integer4 sttubeprofile_bank_to_common_(integer1 *bank);
integer4 sttubeprofile_common_to_dump_(integer4 *opt);
integer4 sttubeprofile_common_to_dumpf_(FILE *fp, integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* sttubeprofile_bank_buffer_ (integer4* sttubeprofile_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct {
  integer4 ntube[2];                          // total number of tubes
  integer4 ngtube;                            // number of good tubes
  integer4 status;                            // (-2 if bank is not filled
                                              //  -1 if bad geometry fit
                                              //   0 if bad profile fit
                                              //   1 if good profile fit)

  real8 rp[2];                                // Impact parameter (meters)
  real8 psi[2];                               // Shower-detector plane angle (radians)
  real8 t0[2];                                // Detection time at Rp, less Rp travel time (ns)

  real8 Xmax;                                 // Shower maximum (g/cm2)
  real8 eXmax;                                // uncertainty on xmax
  real8 Nmax;                                 // Number of charged particles at shower maximum
  real8 eNmax;                                // uncertainty on nmax
  real8 Energy;                               // Initial cosmic-ray energy
  real8 eEnergy;                              // uncertainty on energy
  real8 chi2;                                 // Total chi2 of fit

  real8 x[2][STTUBEPROF_MAXTUBE];             // slant depth at middle of tube (g/cm2)

  real8 npe[2][STTUBEPROF_MAXTUBE];           // number of photo-electrons in tube
  real8 enpe[2][STTUBEPROF_MAXTUBE];          // uncertainty on NPE, including uncertainty from acceptance
  real8 eacptfrac[2][STTUBEPROF_MAXTUBE];     // fraction of uncertainty due to acceptance.

  real8 acpt[2][STTUBEPROF_MAXTUBE];          // PMT acceptance
  real8 eacpt[2][STTUBEPROF_MAXTUBE];         // binomial uncertainty on acceptance

  real8 flux[2][STTUBEPROF_MAXTUBE];          // flux at the mirror [detectable npe / (m2 * radian)]
  real8 eflux[2][STTUBEPROF_MAXTUBE];         // uncertainty on flux

  real8 simnpe[2][STTUBEPROF_MAXTUBE];        // simulated photo-electrons in tube

  real8 nfl[2][STTUBEPROF_MAXTUBE];           // Flux of simulated fluorescence photons
  real8 ncvdir[2][STTUBEPROF_MAXTUBE];        // Flux of simulated direct cerenkov photons
  real8 ncvmie[2][STTUBEPROF_MAXTUBE];        // Flux of simulated Mie scattered cerenkov photons
  real8 ncvray[2][STTUBEPROF_MAXTUBE];	      // Flux of simulated Rayleigh scattered cerenkov photons
  real8 simflux[2][STTUBEPROF_MAXTUBE];	      // Total flux of simluated photons

  real8 ne[2][STTUBEPROF_MAXTUBE];	      // Number of charged particles
  real8 ene[2][STTUBEPROF_MAXTUBE];	      // uncertainty on ne

  real8 tres[2][STTUBEPROF_MAXTUBE];	      // Time-slice fit residual
  real8 tchi2[2][STTUBEPROF_MAXTUBE];	      // Time-slice fit chi2 contribution

  integer4 camera[2][STTUBEPROF_MAXTUBE];     // Camera number for this tube
  integer4 tube[2][STTUBEPROF_MAXTUBE];       // Tube ID
  integer4 tube_qual[2][STTUBEPROF_MAXTUBE];  // tube quality (good = 1, bad = 0)

  integer4 mc;                                // [0 = don't use trumpmc bank info, 1 = use trumpmc 
  
  // new in bank version 1
  real8 X0;
  real8 eX0;
  real8 Lambda;
  real8 eLambda;
  
  int siteid[2];
  
} sttubeprofile_dst_common;

extern sttubeprofile_dst_common sttubeprofile_;

integer4 sttubeprofile_struct_to_abank_(sttubeprofile_dst_common *sttubeprofile, integer1 *(*pbank), integer4 id, integer4 ver);
integer4 sttubeprofile_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 sttubeprofile_struct_to_dst_(sttubeprofile_dst_common *sttubeprofile, integer1 *bank, integer4 *unit, integer4 id, integer4 ver);
integer4 sttubeprofile_abank_to_struct_(integer1 *bank, sttubeprofile_dst_common *sttubeprofile);
integer4 sttubeprofile_struct_to_dump_(sttubeprofile_dst_common *sttubeprofile, integer4 *opt);
integer4 sttubeprofile_struct_to_dumpf_(sttubeprofile_dst_common *sttubeprofile, FILE *fp, integer4 *opt);

#endif
