// Created 2010/04 LMS

#ifndef _STPLANE_DST_
#define _STPLANE_DST_

#include <stdio.h>
#include "dst_std_types.h"

#define STPLANE_BANKID          20001
#define STPLANE_BANKVERSION   004

#define STPLANE_MAXSITES      4
#define STPLANE_MAXTUBE         2000

#ifdef __cplusplus
extern "C" {
#endif
integer4 stplane_common_to_bank_();
integer4 stplane_bank_to_dst_(integer4 *unit);
integer4 stplane_common_to_dst_(integer4 *unit); // combines above 2
integer4 stplane_bank_to_common_(integer1 *bank);
integer4 stplane_common_to_dump_(integer4 *opt);
integer4 stplane_common_to_dumpf_(FILE *fp, integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* stplane_bank_buffer_ (integer4* stplane_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct {
  real8 sdp_angle[(STPLANE_MAXSITES)*(STPLANE_MAXSITES-1)/2];      // Shower-detector plane opening angle (radians) [BR/LR][BR/MD][BR/TL][LR/MD][LR/TL][MD/TL]
  real8 showerVector[3];                  // Shower axis vector (CLF coordinate system)
  real8 impactPoint[3];                   // Shower core location (CLF coordinate system)
  real8 zenith;                           // Zenith angle (radians, CLF coordinate system)
  real8 azimuth;                          // Azimuth angle (radians, CLF coordinate system)

  real8 sdp_n[STPLANE_MAXSITES][3];       // Shower-detector plane normal (CLF coordinate system) for BR, LR, MD, TL

  real8 rpuv[STPLANE_MAXSITES][3];        // Stereo Rp unit vector for BR, LR, MD, TL(in local coordinate system)
  real8 shower_axis[STPLANE_MAXSITES][3]; // Stereo shower axis vector wrt BR, LR, MD, TL (local coordinate system)
  real8 core[STPLANE_MAXSITES][3];        // Stereo shower core location wrt BR, LR, MD, Tl (local coordinate system)
  real8 rp[STPLANE_MAXSITES];           // Stereo rp (meters) wrt BR, LR, MD, TL
  real8 psi[STPLANE_MAXSITES];            // Stereo psi (radians) for BR, LR, MD, TL
  real8 shower_zen[STPLANE_MAXSITES];     // Shower zenith angle (radians)
  real8 shower_azm[STPLANE_MAXSITES];     // Shower azimuthal angle (radians)

  integer4 part[STPLANE_MAXSITES];        // part number for BR, LR, MD, TL
  integer4 event_num[STPLANE_MAXSITES];   // event number for BR, LR, MD, TL

  integer4 sites[STPLANE_MAXSITES];       // Sites involved [BR][LR][MD][TL] [0/1][0/1][0/1][0/1]
  
  integer4 juliancore[STPLANE_MAXSITES];
  
  integer4 jseccore[STPLANE_MAXSITES];
  
  integer4 nanocore[STPLANE_MAXSITES];
  
  real8 track_length[STPLANE_MAXSITES];
  real8 expected_duration[STPLANE_MAXSITES]; // given Rp, psi, SDP, and track length, how long
                                             //should the shower last (nanoseconds)?
} stplane_dst_common;

extern stplane_dst_common stplane_;

integer4 stplane_struct_to_abank_(stplane_dst_common *stplane, integer1 *(*pbank), integer4 id, integer4 ver);
integer4 stplane_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 stplane_struct_to_dst_(stplane_dst_common *stplane, integer1 *bank, integer4 *unit, integer4 id, integer4 ver);
integer4 stplane_abank_to_struct_(integer1 *bank, stplane_dst_common *stplane);
integer4 stplane_struct_to_dump_(stplane_dst_common *stplane, integer4 *opt);
integer4 stplane_struct_to_dumpf_(stplane_dst_common *stplane, FILE *fp, integer4 *opt);

#endif
