/*
 * stps2_dst.h
 *
 * $Source:$
 * $Log:$
 *
 * A filter bank. Contains "interesting" values computed during the stps2 
 * filter.
 *
 */

#ifndef _STPS2_   /* Avoid redeclaration problems.. */
#define _STPS2_

#include <stdio.h>
#include "dst_std_types.h"

#define STPS2_BANKID 15042
#define STPS2_BANKVERSION 0 

#include "univ_dst.h"
#include "mc04_detector.h"

#ifdef __cplusplus
extern "C" {
#endif
integer4 stps2_common_to_bank_(void);
integer4 stps2_bank_to_dst_(integer4 *NumUnit);
integer4 stps2_common_to_dst_(integer4 *NumUnit); /* combines above 2 */
integer4 stps2_bank_to_common_(integer1 *bank);
integer4 stps2_common_to_dump_(integer4 *long_output);
integer4 stps2_common_to_dumpf_(FILE* fp, integer4 *long_output);
/* get (packed) buffer pointer and size */
integer1* stps2_bank_buffer_ (integer4* stps2_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif



typedef struct 
{
  /* 
     plog is the negative log10 probability (0.0 - 1.0) that an event is noise.
     It is calculated by the function rc1_ray0(), and equals: 
     rvec^2 / log10(npair), where rvec is the Rayleigh Vector computed by
     rc1_ray0() and npair is the number of neighboring tubes in an event.
  */
  real4 plog[MC04_MAXEYE];
  
  /* 
     rvec is, as describes above, the Rayleigh Vector magnitude calculated by
     the function rc1_ray0() for the event.
  */
  real4 rvec[MC04_MAXEYE];
  
  /* 
     rwalk is the Rayleigh Vector magnitude that would be due to a random 
     scattering of a given number of pairs of neighboring tubes (npair)..
  */
  real4 rwalk[MC04_MAXEYE];
  
  /* 
     ang is the angle between the y-axis and the Rayleigh Vector  =
     arccos( rvec_y / rvec_mag ). It is used to calculate the upward bit..
  */
  real4 ang[MC04_MAXEYE];
  
  /* 
     aveTime, and sigmaTime are the mean and standard deviation of the 
     calibrated trigger times of all the in-time tubes in an event. In-time
     tubes are described below. The calibrated values for the tubes are
     in the thcal1[] array of the hraw1 common block.
  */
  real4 aveTime  [MC04_MAXEYE];
  real4 sigmaTime[MC04_MAXEYE];
  
  /* 
     The mean and standard deviaiton of the calibrated photon count (prxf[])
     for all in-time tubes in the event.
  */
  real4 avePhot  [MC04_MAXEYE];
  real4 sigmaPhot[MC04_MAXEYE];

  /*
     inTimeTubes is the number of tubes whose standard deviation from the mean
     of all the tubes falls inside 3x the standard deviation for all the tubes.

     total_lifetime is the total amount of time between the first tube that
     fired and the last tube that fired.

     lifetime is the maximum in-time tube trigger time minus the minimum 
     in-time tube trigger time. It gives some idea of the temporal spread of
     the in-time tubes in an event.
  */
  real4    lifetime     [MC04_MAXEYE];
  real4    totalLifetime[MC04_MAXEYE];
  integer4 inTimeTubes  [MC04_MAXEYE];

  /*
    if ( if_eye[ieye] != 1) ignore site
   */
  integer4 if_eye[MC04_MAXEYE];
  integer4 maxeye;

  /* 
     upward is either 1 or 0 depending on whether the stps2 filter thought an
     event was upward going or downward going, respectively. 
  */
  integer1 upward[MC04_MAXEYE];

} stps2_dst_common;

extern stps2_dst_common stps2_ ; 

#endif

