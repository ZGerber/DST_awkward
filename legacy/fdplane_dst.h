/* Created 2008/09/23 LMS DRB */

#ifndef _FDPLANE_DST_
#define _FDPLANE_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "geofd_dst.h"

#define FDPLANE_BANKID		12093
#define FDPLANE_BANKVERSION	002

#define FDPLANE_MAXTUBE         2000

#define DOWN_EVENT      2
#define UP_EVENT        3
#define INTIME_EVENT    4
#define NOISE_EVENT     5

#ifdef __cplusplus
extern "C" {
#endif
integer4 fdplane_common_to_bank_();
integer4 fdplane_bank_to_dst_(integer4 *unit);
integer4 fdplane_common_to_dst_(integer4 *unit); // combines above 2
integer4 fdplane_bank_to_common_(integer1 *bank);
integer4 fdplane_common_to_dump_(integer4 *opt);
integer4 fdplane_common_to_dumpf_(FILE *fp, integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* fdplane_bank_buffer_ (integer4* fdplane_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct {
  integer4 part;			// part number
  integer4 event_num;			// event number
  integer4 julian;			// run start day
  integer4 jsecond;			// run start second (from start of julian)
  integer4 jsecfrac;			// run start nanosecond (from start of jsecond)
  integer4 second;			// event start second (from run start)
  integer4 secfrac;			// event start nanosecond (from start of second)
  integer4 ntube;			// number of tubes in event
  
                                        // New to bank version 001: uniqID and fmode
  integer4 uniqID;                      // Unique ID of GEOFD bank used in plane fit
  integer4 fmode;                 // Fmode used: 0=default (triangle)
                                        //             1=MCRU
  
  real8 npe[GEOFD_MAXTUBE];		// integrated pulse above pedestal in NPE
  real8 adc[GEOFD_MAXTUBE];             // integrated pulse above pedestal in FADC counts
  real8 ped[GEOFD_MAXTUBE];             // pedestal value under the pulse in FADC counts
  real8 time[GEOFD_MAXTUBE];		// weighted average pulse time
  real8 time_rms[GEOFD_MAXTUBE];	// weighted average pulse time rms
  real8 sigma[GEOFD_MAXTUBE];		// tube significance

  real8 sdp_n[3];			// shower-detector plane normal (SDPN)
  real8 sdp_en[3];			// uncertainty on SDPN fit
  real8 sdp_n_cov[3][3];		// covariance matrix of SDPN fit
  real8 sdp_the;			// shower-detector plane theta angle
  real8 sdp_phi;			// shower-detector plane phi angle
  real8 sdp_chi2;			// SDPN fit chi2

  real8 alt[GEOFD_MAXTUBE];		// altitude of tube
  real8 azm[GEOFD_MAXTUBE];		// azimuth of tube
  real8 plane_alt[GEOFD_MAXTUBE];	// altitude of tube rotated into SDP coordinate system
  real8 plane_azm[GEOFD_MAXTUBE];	// azimuth of tube rotated into SDP coordinate system

  real8 linefit_slope;			// linear fit to time vs. angle slope (ns / degree)
  real8 linefit_eslope;			// linear fit to time vs. angle slope uncertainty (ns / degree)
  real8 linefit_int;			// linear fit to time vs. angle intercept (ns)
  real8 linefit_eint;			// linear fit to time vs. angle intercept uncertainty (ns)
  real8 linefit_chi2;			// linear fit chi2
  real8 linefit_cov[2][2];		// linear fit covariance
  real8 linefit_res[GEOFD_MAXTUBE];	// linear fit tube residual (ns)
  real8 linefit_tchi2[GEOFD_MAXTUBE];	// linear fit tube chi2 contribution

  real8 ptanfit_rp;			// pseudo-tangent fit rp (meters)
  real8 ptanfit_erp;			// pseudo-tangent fit rp uncertainty (meters)
  real8 ptanfit_t0;			// pseudo-tangent fit t0 (ns)
  real8 ptanfit_et0;			// pseudo-tangent fit t0 uncertainty (ns)
  real8 ptanfit_chi2;			// pseudo-tangent fit chi2
  real8 ptanfit_cov[2][2];		// pseudo-tangent fit covariance
  real8 ptanfit_res[GEOFD_MAXTUBE];	// pseudo-tangent fit tube residual contribution (ns)
  real8 ptanfit_tchi2[GEOFD_MAXTUBE];	// pseudo-tangent fit tube chi2 contribution

  real8 rp;				// tangent-fit rp (meters)
  real8 erp;				// tangent-fit rp uncertainty (meters)
  real8 psi;				// tangent-fit psi (radians)
  real8 epsi;				// tangent-fit psi uncertainty (radians)
  real8 t0;				// tangent-fit t0 (ns)
  real8 et0;				// tangent-fit t0 uncertainty (ns)
  real8 tanfit_chi2;			// tangent-fit chi2
  real8 tanfit_cov[3][3];		// pseudo-tangent fit covariance
  real8 tanfit_res[GEOFD_MAXTUBE];	// pseudo-tangent fit tube residual (ns)
  real8 tanfit_tchi2[GEOFD_MAXTUBE];	// pseudo-tangent fit tube chi2 contribution

  real8 azm_extent;			// azimuthal extent of good tubes rotated into SDP coordinate system (radians)
  real8 time_extent;			// time extent of good tubes (ns)

  real8 shower_zen;			// Shower zenith angle (radians)
  real8 shower_azm;			// Shower azimuthal angle (pointing back to source, radians, E=0, N=PI/2)
  real8 shower_axis[3];			// Shower axis vector (along direction of shower propagation)
  real8 rpuv[3];			// Rp unit vector
  real8 core[3];			// Shower core location (meters)

  integer4 camera[GEOFD_MAXTUBE];	// camera number
  integer4 tube[GEOFD_MAXTUBE];		// tube number
  integer4 it0[GEOFD_MAXTUBE];		// FADC index of start of pulse
  integer4 it1[GEOFD_MAXTUBE];		// FADC index of end of pulse
  integer4 knex_qual[GEOFD_MAXTUBE];	// 1 = good connectivity, 0 = bad connectivity
  integer4 tube_qual[GEOFD_MAXTUBE];	// total tube quality
					// good = 1
      // bad  = decimal (-[bad_peak][bad_saturated][bad_knex][bad_sdpn][bad_tvsa])

  integer4 ngtube;			// number of good tubes in event
  integer4 seed;			// original knex seed
  integer4 type;			// type of event (down=2, up=3, intime=4, noise=5)
  integer4 status;			// decimal time fit status ([good linear][good pseudotan][good tangent])
  integer4 siteid;			// site ID (BR = 0, LR = 1)

} fdplane_dst_common;

extern fdplane_dst_common fdplane_;
extern integer4 fdplane_blen; /* needs to be accessed by the c files of the derived banks */ 

integer4 fdplane_struct_to_abank_(fdplane_dst_common *fdplane, integer1 *(*pbank), integer4 id, integer4 ver);
integer4 fdplane_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 fdplane_struct_to_dst_(fdplane_dst_common *fdplane, integer1 *bank, integer4 *unit, integer4 id, integer4 ver);
integer4 fdplane_abank_to_struct_(integer1 *bank, fdplane_dst_common *fdplane);
integer4 fdplane_struct_to_dump_(fdplane_dst_common *fdplane, integer4 *opt);
integer4 fdplane_struct_to_dumpf_(fdplane_dst_common *fdplane, FILE *fp, integer4 *opt);

#endif
