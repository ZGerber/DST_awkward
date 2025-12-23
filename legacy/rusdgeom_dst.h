/*
 *     Bank for SD pass1 geometry fit information
 *     Dmitri Ivanov (dmiivanov@gmail.com)
 *     Nov 4, 2008
 
 *     Last modified: Nov 6, 2018
 
 */

#ifndef _RUSDGEOM_
#define _RUSDGEOM_

#include <stdio.h>
#include "dst_std_types.h"

#define RUSDGEOM_BANKID  13104
#define RUSDGEOM_BANKVERSION   001



#ifdef __cplusplus
extern "C" {
#endif
integer4 rusdgeom_common_to_bank_ ();
integer4 rusdgeom_bank_to_dst_ (integer4 * NumUnit);
integer4 rusdgeom_common_to_dst_ (integer4 * NumUnit);	/* combines above 2 */
integer4 rusdgeom_bank_to_common_ (integer1 * bank);
integer4 rusdgeom_common_to_dump_ (integer4 * opt1);
integer4 rusdgeom_common_to_dumpf_ (FILE * fp, integer4 * opt2);
/* get (packed) buffer pointer and size */
integer1* rusdgeom_bank_buffer_ (integer4* rusdgeom_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


#define RUSDGEOM_MSDS 0x200
#define RUSDGEOM_MSIG 100
#define RUSDGEOM_DST_GZ ".rusdgeom.dst.gz" /* output suffix */


// SD origin with respect to CLF origin in CLF frame, in [1200m] units
#define RUSDGEOM_ORIGIN_X_CLF -12.2435
#define RUSDGEOM_ORIGIN_Y_CLF -16.4406

/* For converting time in uS to distance in units of counter
   separation distance 1200m. Numerically, this is 
   c*(10^(-6)(S/uS))/1200m, where c is the speed of light in m/s */
#define RUSDGEOM_TIMDIST 0.249827048333


typedef struct
{

  real8 sdsigq[RUSDGEOM_MSDS][RUSDGEOM_MSIG];  /* charge in VEM of the given signal in the given counter */

  
  /* Relative time of each signal in each counter in [1200m] units. To convert this into time after midnight, do
     'time = tearliest + sdsigt[i][j]/RUSDGEOM_TIMDIST * (1e-6)' */
  real8 sdsigt[RUSDGEOM_MSDS][RUSDGEOM_MSIG];
  
  /* Time resolution each signal in each counter in [1200m] units. To convert this into time after midnight, do
     'time = tearliest + sdsigt[i][j]/RUSDGEOM_TIMDIST * (1e-6)' */
  real8 sdsigte[RUSDGEOM_MSDS][RUSDGEOM_MSIG];
  
  /* clf-frame xyz coordinates of each sd, [1200m] units, with respect to CLF origin */
  real8 xyzclf[RUSDGEOM_MSDS][3];
  
  /* The following two variables are using sd signals which are part of the event. If the sd itself is not a part of 
     the event (see igsd - variable), then these variables are calculated using the first signal seen by this sd */
  real8 pulsa[RUSDGEOM_MSDS];  /* charge of the i'th counter in VEM */

  /* To convert this to time after midnight in seconds, do 'time = tearliest + sdtime[i]/RUSDGEOM_TIMDIST * (1e-6)'   */
  real8 sdtime[RUSDGEOM_MSDS]; /* relative time of the i'th counter in [1200m] units */  
  
  real8 sdterr[RUSDGEOM_MSDS]; /* time resolution of the i'th counter, [1200m] units */
  /* 
     Results of geometry fits (double precision data)
     [0] - for plane fit
     [1] - for Modified Linsley's fit
     [2] - final values of the geometry fit
  */

  /* To find the core position in CLF frame in meters with respect to CLF origin, do
     coreX = (xcore+RUSDGEOM_ORIGIN_X_CLF)*1200.0, coreY = (ycore+RUSDGEOM_ORIGIN_Y_CLF)*1200.0
     CLF XY plane is used as 'SD ground plane', that's why zcore is absent. */
  real8 xcore[3]; /* core X and Y, in 1200m units, with respect to CLF, SD origin subtracted */
  real8 dxcore[3]; /* uncertainty on xcore */
  real8 ycore[3];
  real8 dycore[3];
  /* Time when the core hits the CLF plane, [1200m] unts, with respect to tearliest.
     To determine the time when the core hits the CLF plane in seconds since midnight, do:
     t_core_sec_since_midnight = rusdgeom_.tearliest + (1e-6) * rusdgeom_.t0[ifit]/RUSDGEOM_TIMDIST */
  real8 t0[3];
  real8 dt0[3];
  real8 theta[3]; /* event zenith angle, degrees */
  real8 dtheta[3];
  real8 phi[3];   /* event azimuthal angle, degrees */
  real8 dphi[3];
  real8 chi2[3];  /* chi2 of the fit */
  real8 a;        /* Curvature parameter in Linsley's formula */
  real8 da;
  
  /* Earliest signal time in the trigger in seconds after midnight. All other quoted times are relative to this time, 
     and are converted to [1200m] units for convenience. */
  real8 tearliest;   /* Earliest signal time in the trigger in seconds after midnight */

  
  /* igsig[sd_index][signal_index]:
     0 - given SD was not working properly
     1 - given SD is not a part of any clusters
     2 - given SD is a part of space cluster
     3 - given SD signal passed a roguh time pattern recognition
     4 - given SD signal is a part of the event
     5 - given SD signal saturates the counter
  */
  
  integer4 igsig[RUSDGEOM_MSDS][RUSDGEOM_MSIG];
  
  /* irufptn[sd_index][signal_index]: points to the signal in rufptn dst bank, 
     which is a signal-based list of variables */
  integer4 irufptn[RUSDGEOM_MSDS][RUSDGEOM_MSIG];


  
  /* igsd[sd index]:
     0 - sd was not working properly (bad 1MIP fit,etc)
     1 - sd was working but is none of its signals is a part of event
     2 - sd is a part of event
     3 - sd is saturated
  */
  integer4 igsd[RUSDGEOM_MSDS];
  integer4 xxyy[RUSDGEOM_MSDS];  /* sd position IDs */
  integer4 nsig[RUSDGEOM_MSDS];  /* number of independent signals (hits) in each SD */
  
  /* 
     For each counter that's a part of the event there is only one signal chosen.
     sdirufptn[sd_index] contains rufptn index (rufptn is a signal-based list of variables) of the chosen signal.  
     If this sd is not a part of the event (see igsd variable), then we quote here the rufptn index of the first 
     singnal seen by the sd.
   */
  integer4 sdirufptn[RUSDGEOM_MSDS];
  
  /* # of d.o.f. for geom. fitting, [0] - plane fitting, [1] - Modified Linsley fit, [3] - Final result
     Calculated as  (# of counters in the fit) - (# of fit parameters) */ 
  integer4 ndof[3];

  integer4 nsds;   /* number of sds in the trigger */  

} rusdgeom_dst_common;

extern rusdgeom_dst_common rusdgeom_;

#endif
