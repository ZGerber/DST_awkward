/*
 * hctim_dst.h
 *
 * $Source: /hires_soft/cvsroot/bank/hctim_dst.h,v $
 * $Log: hctim_dst.h,v $
 * Revision 1.3  2001/06/30 02:14:47  wiencke
 * added documentation
 * no changes to functionality
 *
 * Revision 1.2  2000/11/13 06:31:20  reil
 * Fixed formatting of structure to allow automatic ntuple creation.
 * No changes to structure made.
 *
 * Revision 1.1  1999/06/28 21:05:31  tareq
 * Initial revision
 *
 *
 * Track geometry from profile constraint geometry fit
 * used for analysis of BigH data
 *
 * This bank is designed to work in conjunction with mjk's PRFC bank in
 * the context of the "profile constraint geometry fit".  The construction
 * of this bank mimics PRFC bank.
 * 
 */

/* Comments added 6-29-2001 LRW 
 *
 * The hctim bank contains a description of the shower geometry
 * It was originally used for monocular reconstruction.  At one point
 * reconstruction used timing between PMT's to determine the
 * distance between the detector and the shower, and the angle
 * the shower made in the shower detector plane.  Hence the tim
 * (short for time) in the bank name.
 * 
 * For showers seen in stereo, the location and direction of the shower
 * axis can be determined by from intersection between the two shower 
 * detector planes.  This works when the opening angle between the planes 
 * is not to close to 0 degrees or 180 degrees.  One can also do a global
 * fit to the triggered PMT's in an attempt to improve the reconstruction.
 * Finally one can use timing as well as the observed numbers of photoelectrons
 * to improve the fit.
 *
 * As of the writing of this note, for stereo data, the parameters in 
 * this bank were just the result of an intersection between the two
 * planes.
 *
 * Three angles are used to describe the shower direction.
 * mthe is the zenith angle. This means the angle between the shower
 *       and a vertical line. 
 * mspi is the angle between the horizontal and the shower axis *in* the
 *      detector shower plane
 * mphi is the angle measured counter clockwise from east to the projection
 *      of the shower axis on to the horizontal plane.
 *
 * mrp   is the distance of closest approach of the shower axis to the detector
 * mchi2 is the chi-square of the fit
 * mtkv  is a unit vector along the shower
 * mrpv  is the rp vector
 * mrpuv is a unit vector version of mrpv
 * mshwn is a unit vector perpendicular (normal) to the detector shower plane
 * mcore is the location where the shower axis intersects with the X,Y plane.
 *       (also called the shower core location)  It's close to the point the
 *       shower would hit the ground except that the XY plane (z=0) is typically
 *       above the ground since the origin is defined at a point on 5 mile hill. 
 *
 * Each variable has an index.  For the stereo analysis 0 corresponds to hr1,
 * 1 to hr2.  There may be other types of fits stored in higher indicies.
 * 
 * End of June 29 2001 comments */

#ifndef _HCTIM_
#define _HCTIM_

#include <stdio.h>
#include "dst_std_types.h"

#define HCTIM_BANKID 15006
#define HCTIM_BANKVERSION 0 

#define HCTIM_MAXFIT 16

#define HCTIM_TIMINFO_USED    1
#define HCTIM_TIMINFO_UNUSED  0

#define HCTIM_FIT_NOT_REQUESTED         1
#define HCTIM_NOT_IMPLEMENTED           2
#define HCTIM_REQUIRED_BANKS_MISSING    3
#define HCTIM_MISSING_TRAJECTORY_INFO   4
#define HCTIM_UPWARD_GOING_TRACK       10
#define HCTIM_TOO_FEW_GOOD_TUBES       11
#define HCTIM_FITTER_FAILURE           12
#define HCTIM_INSANE_TRAJECTORY        13

#define HCTIM_STAT_ERROR_FAILURE        1
#define HCTIM_RIGHT_ERROR_FAILURE       2
#define HCTIM_LEFT_ERROR_FAILURE        4

#ifdef __cplusplus
extern "C" {
#endif
integer4 hctim_common_to_bank_(void);
integer4 hctim_bank_to_dst_(integer4 *NumUnit);
integer4 hctim_common_to_dst_(integer4 *NumUnit);
integer4 hctim_bank_to_common_(integer1 *bank);
integer4 hctim_common_to_dump_(integer4 *long_output);
integer4 hctim_common_to_dumpf_(FILE* fp, integer4 *long_output);
/* get (packed) buffer pointer and size */
integer1* hctim_bank_buffer_ (integer4* hctim_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif



/* In profile constraint fit (PCF) main, right, and left trajectories are
 * the trajectories where the profile fit is minimum, and is 1 sigma from
 * minimum.
 * This bank stores the geometry parameters.  The chi2 values below are the
 * those of the time fit chi2, NOT the profile fit.
 */

typedef struct  {

  real8 mchi2 [HCTIM_MAXFIT], rchi2[HCTIM_MAXFIT], lchi2[HCTIM_MAXFIT];  
  real8 mrp   [HCTIM_MAXFIT], rrp  [HCTIM_MAXFIT], lrp  [HCTIM_MAXFIT];
  real8 mpsi  [HCTIM_MAXFIT], rpsi [HCTIM_MAXFIT], lpsi [HCTIM_MAXFIT];
  real8 mthe  [HCTIM_MAXFIT], rthe [HCTIM_MAXFIT], lthe [HCTIM_MAXFIT];
  real8 mphi  [HCTIM_MAXFIT], rphi [HCTIM_MAXFIT], lphi [HCTIM_MAXFIT];

  real8 mtkv [HCTIM_MAXFIT][3], rtkv [HCTIM_MAXFIT][3], ltkv [HCTIM_MAXFIT][3];
  real8 mrpv [HCTIM_MAXFIT][3], rrpv [HCTIM_MAXFIT][3], lrpv [HCTIM_MAXFIT][3];
  real8 mrpuv[HCTIM_MAXFIT][3], rrpuv[HCTIM_MAXFIT][3], lrpuv[HCTIM_MAXFIT][3];
  real8 mshwn[HCTIM_MAXFIT][3], rshwn[HCTIM_MAXFIT][3], lshwn[HCTIM_MAXFIT][3];
  real8 mcore[HCTIM_MAXFIT][3], rcore[HCTIM_MAXFIT][3], lcore[HCTIM_MAXFIT][3];

  /* tube/mir info. from main fit */

  real8 time   [HCTIM_MAXFIT][HR_UNIV_MAXTUBE]; /* tube time */
  real8 timefit[HCTIM_MAXFIT][HR_UNIV_MAXTUBE]; /* time from best fit */
  real8 thetb  [HCTIM_MAXFIT][HR_UNIV_MAXTUBE]; /* viewing angle */
  real8 sgmt   [HCTIM_MAXFIT][HR_UNIV_MAXTUBE]; /* sigma time */ 
  real8 asx    [HCTIM_MAXFIT][HR_UNIV_MAXTUBE];
  real8 asy    [HCTIM_MAXFIT][HR_UNIV_MAXTUBE];
  real8 asz    [HCTIM_MAXFIT][HR_UNIV_MAXTUBE];

  integer4 nmir    [HCTIM_MAXFIT];
  integer4 ntube   [HCTIM_MAXFIT];

  integer4 mir     [HCTIM_MAXFIT][HR_UNIV_MAXMIR]; /* mir number */
  integer4 mirntube[HCTIM_MAXFIT][HR_UNIV_MAXMIR];
  integer4 tubemir [HCTIM_MAXFIT][HR_UNIV_MAXTUBE]; 
  integer4 tube    [HCTIM_MAXFIT][HR_UNIV_MAXTUBE];  /* tube number */
  integer4 ig      [HCTIM_MAXFIT][HR_UNIV_MAXTUBE];  /* tube flag */

  integer4 failmode[HCTIM_MAXFIT];  
  integer4 timinfo [HCTIM_MAXFIT];

  integer4 jday[HCTIM_MAXFIT];   /* mean julian day - 2.44e6 */
  integer4 jsec[HCTIM_MAXFIT];   /* second into Julian day */
  integer4 msec[HCTIM_MAXFIT];   /* milli sec of julian day 
                                    (NOT since UT0:00) */

} hctim_dst_common ;

extern hctim_dst_common hctim_ ; 

#endif
