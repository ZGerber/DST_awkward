/*
 *     Bank for SDMC inputs
 *     Benjamin Stokes (stokes@physics.rutgers.edu)
 *     Jan 22, 2009

 *     Last modified: Mar 6, 2019
 *     (D. Ivanov, dmiivanov@gmail.com)

*/
#ifndef _RUSDMC_
#define _RUSDMC_

#include <stdio.h>
#include "dst_std_types.h"

#define RUSDMC_BANKID  13105
#define RUSDMC_BANKVERSION   000

#ifdef __cplusplus
extern "C" {
#endif
integer4 rusdmc_common_to_bank_ ();
integer4 rusdmc_bank_to_dst_ (integer4 * NumUnit);
integer4 rusdmc_common_to_dst_ (integer4 * NumUnit);	/* combines above 2 */
integer4 rusdmc_bank_to_common_ (integer1 * bank);
integer4 rusdmc_common_to_dump_ (integer4 * opt1);
integer4 rusdmc_common_to_dumpf_ (FILE * fp, integer4 * opt2);
integer1* rusdmc_bank_buffer_ (integer4* rusdmc_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif

typedef struct
{
  integer4 event_num;	 /* event number */
  integer4 parttype;     /* Corsika particle code [proton=14, iron=5626,
			    for others, consult Corsika manual] */
  integer4 corecounter;  /* counter closest to core */
  integer4 tc;           /* clock count corresponding to shower front 
			    passing through core position*/
  real4 energy;          /* total energy of primary particle [EeV] */
  real4 height;          /* height of first interation [cm] */
  real4 theta;           /* zenith angle [rad] */
  real4 phi;             /* azimuthal angle (N of E) [rad] */
  real4 corexyz[3];      /* 3D core position in CLF reference frame [cm] */
} rusdmc_dst_common;

extern rusdmc_dst_common rusdmc_;

#endif
