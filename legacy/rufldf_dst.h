/*     Bank for SD pass2 data
 *     Dmitri Ivanov (ivanov@physics.rutgers.edu)
 *     Mar 14, 2009
 
 *     Last Modified: May 16, 2019
 */

#ifndef _RUFLDF_
#define _RUFLDF_

#include <stdio.h>
#include "dst_std_types.h"

#define RUFLDF_BANKID  13107
#define RUFLDF_BANKVERSION   001


#ifdef __cplusplus
extern "C" {
#endif
integer4 rufldf_common_to_bank_ ();
integer4 rufldf_bank_to_dst_ (integer4 * NumUnit);
integer4 rufldf_common_to_dst_ (integer4 * NumUnit);	/* combines above 2 */
integer4 rufldf_bank_to_common_ (integer1 * bank);
integer4 rufldf_common_to_dump_ (integer4 * opt1);
integer4 rufldf_common_to_dumpf_ (FILE * fp, integer4 * opt2);
/* get (packed) buffer pointer and size */
integer1* rufldf_bank_buffer_ (integer4* rufldf_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


#define RUFLDF_DST_GZ ".rufldf.dst.gz" /* output suffix */

typedef struct
{

  
  /* [0] - LDF alone fit, [1] - LDF + geom. fit */
  real8 xcore[2];  // Core position + uncertainty
  real8 dxcore[2];
  real8 ycore[2];
  real8 dycore[2];
  real8 sc[2];     // Scaling factor in fron of LDF + Minuit uncertainty on that
  real8 dsc[2];
  real8 s600[2];   // S600, VEM/m^2
  real8 s600_0[2];
  real8 s800[2];   // S800, VEM/m^2
  real8 s800_0[2];
  real8 aenergy[2]; /* Energy in EeV using AGASA formula */
  real8 energy[2];  /* Energy in EeV using Rutgers formula */
  real8 atmcor[2];  /* Energy atmopsheric correction factor that was applied */
  real8 chi2[2];
  
  /* These variables are from combined LDF/geom fit */
  real8 theta;
  real8 dtheta;
  real8 phi;
  real8 dphi;
  real8 t0;
  real8 dt0;
  
  
  // These variables are required for exposure/efficiency computations
  // Variables w/o mc prefix are reconstructed, with mc prefix are the thrown variables
  
  // Distance of the shower core from a closes SD array edge boundary.
  // If it is negative, then the core is outside of the array
  real8 bdist;
  
  
  // Distance of the shower core from the closest T-shape bounday for BR,LR,SK
  // At most only one such distance is non-negagtive, as the shower core can
  // hit only one of the subarrays.  If all distances are negative, this means that
  // the shower core either hits outside of the array or outside of BR,LR,SK subarrays
  real8 tdistbr;
  real8 tdistlr;
  real8 tdistsk;
  real8 tdist;    // Distance to a T-shape boundary  
  
  /* [0] - for LDF alone fit, [1] - for LDF+geom fit */
  integer4 ndof[2];
  
} rufldf_dst_common;

extern rufldf_dst_common rufldf_;

#endif
