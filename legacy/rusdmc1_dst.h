/*
 *     Bank for variables calculated from rusdmc bank.  
 *     Usefull for applying cuts on thrown MC variables
 *     (e.g. when studying trigger/reconstruction efficiency) and resolution studies.
 *
 *     Dmitri Ivanov (ivanov@physics.rutgers.edu)
 *     Apr 11, 2009

 *     Last modified: Nov 19, 2009

*/
#ifndef _RUSDMC1_
#define _RUSDMC1_

#include <stdio.h>
#include "dst_std_types.h"

#define RUSDMC1_BANKID  13106
#define RUSDMC1_BANKVERSION   001

#define RUSDMC1_SD_ORIGIN_X_CLF -12.2435
#define RUSDMC1_SD_ORIGIN_Y_CLF -16.4406

#ifdef __cplusplus
extern "C" {
#endif
integer4 rusdmc1_common_to_bank_ ();
integer4 rusdmc1_bank_to_dst_ (integer4 * NumUnit);
integer4 rusdmc1_common_to_dst_ (integer4 * NumUnit);	/* combines above 2 */
integer4 rusdmc1_bank_to_common_ (integer1 * bank);
integer4 rusdmc1_common_to_dump_ (integer4 * opt1);
integer4 rusdmc1_common_to_dumpf_ (FILE * fp, integer4 * opt2);
/* get (packed) buffer pointer and size */
integer1* rusdmc1_bank_buffer_ (integer4* rusdmc1_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct
{  
  // Thrown MC core position, CLF frame, [1200m] units, SD origin subtracted off (RUSDMC1_SD_ORIGIN_(X,Y)_CLF)
  real8 xcore;
  real8 ycore;
  real8 t0;    // Time of the core hit, [1200m], with respect to earliest SD time in the event readout
  real8 bdist;   // Distance of the core from the edge of the array.If negative, then the core is outside.  
  real8 tdistbr; // Distance of the core position from BR T-Shaped boundary, negative if not in BR
  real8 tdistlr; // same for LR
  real8 tdistsk; // same for SK
  real8 tdist;   // Closest distance to any T-shaped boundary (BR,LR,SK)
} rusdmc1_dst_common;

extern rusdmc1_dst_common rusdmc1_;

#endif
