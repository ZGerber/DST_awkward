/*
 *     Bank for SD pass1 data
 *     Dmitri Ivanov (dmiivanov@gmail.com)
 *     Jul 5, 2008
 
 *     Last modified: Nov 6, 2018
 
 */

#ifndef _RUFPTN_
#define _RUFPTN_

#include <stdio.h>
#include "dst_std_types.h"

#define RUFPTN_BANKID  13103
#define RUFPTN_BANKVERSION   001


#ifdef __cplusplus
extern "C" {
#endif
integer4 rufptn_common_to_bank_ ();
integer4 rufptn_bank_to_dst_ (integer4 * NumUnit);
integer4 rufptn_common_to_dst_ (integer4 * NumUnit);	/* combines above 2 */
integer4 rufptn_bank_to_common_ (integer1 * bank);
integer4 rufptn_common_to_dump_ (integer4 * opt1);
integer4 rufptn_common_to_dumpf_ (FILE * fp, integer4 * opt2);
/* get (packed) buffer pointer and size */
integer1* rufptn_bank_buffer_ (integer4* rufptn_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


#define RUFPTNMH 0x400   /* maximum number of hits */
#define RUFPTN_DST_GZ ".rufptn.dst.gz" /* output suffix */


// SD origin with respect to CLF origin in CLF frame, in [1200m] units
#define RUFPTN_ORIGIN_X_CLF -12.2435
#define RUFPTN_ORIGIN_Y_CLF -16.4406

/* For converting time in uS to distance in units of counter
   separation distance 1200m. Numerically, this is 
   c*(10^(-6)(S/uS))/1200m, where c is the speed of light in m/s */
#define RUFPTN_TIMDIST 0.249827048333


typedef struct
{

  /* fadc trace and monitoring data parsing, integer data */
  
  integer4 nhits;     /* number of independent signals (hits) in the trigger */
  integer4 nsclust;   /* number of hits in the largest space cluster */
  
  /* Only one hit from each SD can be a part of space-time cluster */
  integer4 nstclust;  /* number of SDs in the space-time cluster */
  
  /* number of SDs that are a part of the space-time cluster and lie on the border of the array */
  integer4 nborder;
  
  /*
    isgood - variable:
    isgood[i] = 0 : the counter to which i'th hit corresonds was not working properly
    isgood[i] = 1 : i'th hit is not a part of any clusters
    isgood[i] = 2 : i'th hit is a part of space cluster
    isgood[i] = 3:  i'th hit passed rough time pattern recognition
    isgood[i] = 4:  i'th hit is a part of the event
    isgood[i] = 5:  i'th hit corresponds to a saturated counter
   */
  integer4 isgood[RUFPTNMH];
  integer4 wfindex [RUFPTNMH]; /* indicate to what 1st waveform in rusdraw each hit correponds */
  integer4 xxyy [RUFPTNMH];    /* position of the hit */
  integer4 nfold [RUFPTNMH];   /* foldedness of the hit (over how many 128 fadc widnows this signal extends) */

  /* Here,2nd index is interpreted as follows:
   * [*][0]: for lower counters
   * [*][1]: for upper counters
   */
  integer4 sstart [RUFPTNMH][2]; /* channel where the signal starts */
  integer4 sstop [RUFPTNMH][2]; /* channel where the signal stops */

  /* Channel since signal start after which fadc makes biggest jump (signal point of inflection) */
  integer4 lderiv [RUFPTNMH][2]; /* Channel after which FADC makes a big jump */

  /* Record the channel, since the first point of inflection, 
     after which derivative is negative. */
  integer4 zderiv [RUFPTNMH][2]; // channel after which derivative first goes negative since the signal start

 
  /* fadc trace and monitoring, double precision data */

  /* SD coordinates with respect to CLF frame of reference, in units of [1200m] */
  real8 xyzclf[RUFPTNMH][3];
     
  real8 qtot[2]; /* Total charge in the event (sum over counters in space-time cluster) */
  
  /* Time of the earliest waveform in the trigger in seconds since midnight.
     To find time in seconds since midnight of any hit, do
     tearliest + (reltime of a given hit) * 1200m / (c*t). */
  real8 tearliest [2];

  real8 reltime [RUFPTNMH][2];   /* hit time, relative to to EARLIEST hit, in units of counter sep. dist */
  real8 timeerr [RUFPTNMH][2];   /* error on time, in counter separation units */
  real8 fadcpa [RUFPTNMH][2];    /* pulse area, in fadc counts, peds subtracted */
  real8 fadcpaerr [RUFPTNMH][2]; /* errror on (pulse area - peds) in fadc counts */
  real8 pulsa [RUFPTNMH][2];     /* pulse area in VEM (pedestals subtracted) */
  real8 pulsaerr [RUFPTNMH][2];  /* error on pulse area in VEM (pedestals subtracted) */
  real8 ped [RUFPTNMH][2];       /* pedestals taken from monitoring  */
  real8 pederr [RUFPTNMH][2];    /* pedestal errors computed from the monitoring information (FWHM/2.33) */
  real8 vem [RUFPTNMH][2];       /* FADC counts / VEM, from monitoring */
  real8 vemerr [RUFPTNMH][2];    /* FADC counts/VEM (FWHM/2.33), using monitoring */

  /* Tyro geometry reconstruction (double precision data) */
  
  /* first index interpreted as follows:
   * [0][*]: using lower counters
   * [1][*]: using upper counters
   * [2][*]: using upper and lower counters (avaraged over upper/lower)
   */
  real8 tyro_cdist [3][RUFPTNMH]; /* distances from the core for all counters that were hit */
  
  /*
    [*][0-1]: <x>,<y> in CLF frame with SD origin subtracted off, [1200m] units.
    [*][2]: <x^2> about the core
    [*][3]: <xy> about the core
    [*][4]: <y^2> about the core
  */
  real8 tyro_xymoments [3][5];
  real8 tyro_xypmoments [3][2]; /* principal moments (eigenvalues) */
  real8 tyro_u [3][2]; /* long axis, corresponding to larger eigenvalue */
  real8 tyro_v [3][2]; /* short axis, corresponding to smaller eigenvalue */
  /* Time fit to a straight line of a t (rel. time) vs u plot, for points
     in the st-cluster such that t<u (demand physicsally plausible timing)
     [][0]-constant offset, [][1]-slope */
  real8 tyro_tfitpars [3][2];
  real8 tyro_chi2 [3];   /* chi2 value for T vs U fit */
  real8 tyro_ndof [3];   /* # of d.o.f. for T vs U fit */
  real8 tyro_theta [3];  /* event zenith angle */
  real8 tyro_phi [3];    /* event azimuthal angle */

} rufptn_dst_common;

extern rufptn_dst_common rufptn_;

#endif
