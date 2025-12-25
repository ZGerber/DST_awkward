/* Created 2010/01 LMS */
/* 
 * Added X0 and Lambda fits and changed version number to '2'; set
 *   BANKVERSION macros in brtubeprofile_dst.h and lrtubeprofile_dst.h
 *   to point back to this one to avoid ever having a mis-match.
 *   (SS 22-Sep-2011)
 */

#ifndef _FDTUBEPROFILE_DST_
#define _FDTUBEPROFILE_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "fdplane_dst.h"

#define FDTUBEPROFILE_BANKID        12096
#define FDTUBEPROFILE_BANKVERSION   3

#define FDTUBEPROF_MAXTUBE          (FDPLANE_MAXTUBE)

#ifdef __cplusplus
extern "C" {
#endif
integer4 fdtubeprofile_common_to_bank_();
integer4 fdtubeprofile_bank_to_dst_(integer4 *unit);
integer4 fdtubeprofile_common_to_dst_(integer4 *unit); // combines above 2
integer4 fdtubeprofile_bank_to_common_(integer1 *bank);
integer4 fdtubeprofile_common_to_dump_(integer4 *opt);
integer4 fdtubeprofile_common_to_dumpf_(FILE *fp, integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* fdtubeprofile_bank_buffer_ (integer4* fdtubeprofile_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct {
  integer4 ntube;                             // total number of tubes
  integer4 ngtube[3];                         // number of good tubes

  real8 rp[3];                                // Impact parameter (meters)
  real8 psi[3];                               // Shower-detector plane angle (radians)
  real8 t0[3];                                // Detection time at Rp, less Rp travel time (ns)

  real8 Xmax[3];                              // Shower maximum (g/cm2)
  real8 eXmax[3];                             // uncertainty on xmax
  real8 Nmax[3];                              // Number of charged particles at shower maximum
  real8 eNmax[3];                             // uncertainty on nmax
  real8 Energy[3];                            // Initial cosmic-ray energy
  real8 eEnergy[3];                           // uncertainty on energy
  real8 chi2[3];                              // Total chi2 of fit

  real8 X0[3];                                // effective depth of 1st inter.
  real8 eX0[3];                               // uncertainty in X0
  real8 Lambda[3];                            // profile width parameter
  real8 eLambda[3];                           // uncertainty in lambda

  real8 x[3][FDTUBEPROF_MAXTUBE];             // slant depth at middle of tube (g/cm2)

  real8 npe[3][FDTUBEPROF_MAXTUBE];           // number of photo-electrons in tube
  real8 enpe[3][FDTUBEPROF_MAXTUBE];          // uncertainty on NPE, including uncertainty from acceptance
  real8 eacptfrac[3][FDTUBEPROF_MAXTUBE];     // fraction of uncertainty due to acceptance.

  real8 acpt[3][FDTUBEPROF_MAXTUBE];          // PMT acceptance
  real8 eacpt[3][FDTUBEPROF_MAXTUBE];         // binomial uncertainty on acceptance

  real8 flux[3][FDTUBEPROF_MAXTUBE];          // flux at the mirror [detectable npe / (m2 * radian)]
  real8 eflux[3][FDTUBEPROF_MAXTUBE];         // uncertainty on flux

  real8 simnpe[3][FDTUBEPROF_MAXTUBE];        // simulated photo-electrons in tube

  real8 nfl[3][FDTUBEPROF_MAXTUBE];           // Flux of simulated fluorescence photons
  real8 ncvdir[3][FDTUBEPROF_MAXTUBE];        // Flux of simulated direct cerenkov photons
  real8 ncvmie[3][FDTUBEPROF_MAXTUBE];        // Flux of simulated Mie scattered cerenkov photons
  real8 ncvray[3][FDTUBEPROF_MAXTUBE];	      // Flux of simulated Rayleigh scattered cerenkov photons
  real8 simflux[3][FDTUBEPROF_MAXTUBE];	      // Total flux of simluated photons

  real8 ne[3][FDTUBEPROF_MAXTUBE];	      // Number of charged particles
  real8 ene[3][FDTUBEPROF_MAXTUBE];	      // uncertainty on ne

  real8 tres[3][FDTUBEPROF_MAXTUBE];	      // Time-slice fit residual
  real8 tchi2[3][FDTUBEPROF_MAXTUBE];	      // Time-slice fit chi2 contribution

  integer4 camera[FDTUBEPROF_MAXTUBE];        // Camera number for this tube
  integer4 tube[FDTUBEPROF_MAXTUBE];          // Tube ID
  integer4 tube_qual[3][FDTUBEPROF_MAXTUBE];  // tube quality (good = 1, bad = 0, added = copy of fdplane tube status (EXPERIMENTAL, TENTATIVE))
  integer4 status[3];                         // status[0] is for fdplane_.psi
                                              // status[1] is for fdplane_.psi - fdplane_.epsi
                                              // status[2] is for fdplane_.psi + fdplane_.epsi
                                              // (-2 if bank is not filled                    
                                              //  -1 if bad geometry fit
                                              //   0 if bad profile fit
                                              //   1 if good profile fit)


  integer4 siteid;                            // site ID (BR = 0, LR = 1, MD = 2, TL = 3)
  integer4 mc;                                // [0 = don't use trumpmc bank info, 1 = use trumpmc 
  real8 simtime[3][FDTUBEPROF_MAXTUBE];       // time of simulated signal from waveform
  real8 simtrms[3][FDTUBEPROF_MAXTUBE];       // RMS of time
  real8 simtres[3][FDTUBEPROF_MAXTUBE];       // waveform time residual with fdplane
  real8 timechi2[3][FDTUBEPROF_MAXTUBE];      // chi2 of above qty with fdplane
} fdtubeprofile_dst_common;

extern fdtubeprofile_dst_common fdtubeprofile_;
extern integer4 fdtubeprofile_blen; /* needs to be accessed by the c files of the derived banks */ 

integer4 fdtubeprofile_struct_to_abank_(fdtubeprofile_dst_common *fdtubeprofile, integer1 *(*pbank), integer4 id, integer4 ver);
integer4 fdtubeprofile_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 fdtubeprofile_struct_to_dst_(fdtubeprofile_dst_common *fdtubeprofile, integer1 *bank, integer4 *unit, integer4 id, integer4 ver);
integer4 fdtubeprofile_abank_to_struct_(integer1 *bank, fdtubeprofile_dst_common *fdtubeprofile);
integer4 fdtubeprofile_struct_to_dump_(fdtubeprofile_dst_common *fdtubeprofile, integer4 *opt);
integer4 fdtubeprofile_struct_to_dumpf_(fdtubeprofile_dst_common *fdtubeprofile, FILE *fp, integer4 *opt);

#endif
