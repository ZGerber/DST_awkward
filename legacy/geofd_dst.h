/* 2011-08-30: Version 001 is introduced
 * Some structure members are changed, and GEOFD_MAXMIR is increased from 12 to 52
 * (at this time, no FD has more than 14 cameras, but this makes it possible to
 * create a single GEOFD bank containing all four FD sites BR/LR/MD/TL)
 *
 * - Tom Stroman
 */

/* 
 * Locations and pointing directions for FADC cameras and tubes
 * All azimuthal angle are east = 0, north = 90.
 * X-axis to the east, Y-axis to the north
 * geofd_dst.h LMS - 2008/10/06
 */

#ifndef _GEOFD_DST_
#define _GEOFD_DST_

#include <stdio.h>
#include "dst_std_types.h"

#define GEOFD_BANKID    12091
#define GEOFD_BANKVERSION 003

#define GEOFD_MAXMIR    52
#define GEOFD_MIRTUBE   256
#define GEOFD_MAXTUBE   (GEOFD_MAXMIR * GEOFD_MIRTUBE)

#define GEOFD_ROW       16      // Number of rows of PMTs
#define GEOFD_COL       16      // Number of columns of PMTs
#define GEOFD_SEGMENT   38      // Number of mirror segments

// #define HIRES_CLOVER_GAP     0.00635  // Gap between clover mirror segments (meters)
// #define HIRES_CLOVER_BUTTON  0.02540  // Radius of button at center of clover mirrors (meters)
// #define HIRES_CLOVER_SEGMENT 4        // Number of clover mirror segments
#define MD_CLOVER_GAP 0.00635
#define MD_CLOVER_SEGMENT 4
#define GEOFD_TA    0
#define GEOFD_MD    1
#define GEOFD_TALE  2

#define GEOFD_UNIQDEFAULT -1
#define GEOFD_UNIQBRLRSCOTT 0
#define GEOFD_UNIQBRLRTOKUNO 1320019200
#define GEOFD_UNIQBRLRTHOMAS 1362958681
#define GEOFD_UNIQBRLRSSPTHOMAS 1363040147

// deprecated:
#define GEOFD_UNIQBRSTAN 1349465175
#define GEOFD_UNIQLRSTAN 1349465298
#define GEOFD_UNIQBRSTANSSP 1361982296
#define GEOFD_UNIQLRSTANSSP 1361982939

#ifdef __cplusplus
extern "C" {
#endif
integer4 geofd_common_to_bank_();
integer4 geofd_bank_to_dst_(integer4 *unit);
integer4 geofd_common_to_dst_(integer4 *unit); // combines above 2
integer4 geofd_bank_to_common_(integer1 *bank);
integer4 geofd_common_to_dump_(integer4 *opt) ;
integer4 geofd_common_to_dumpf_(FILE* fp, integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* geofd_bank_buffer_ (integer4* geofd_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef struct {
  integer4 uniqID;  // Uniq ID (ex, creation date), added by D.IKEDA 31/Oct/2011 
  
  /* The location of "a site" is defined in version 0 and 1 as the back corner
   * of the mirror building at roof level.
   * 
   * In version 2, it is the location of the lowest-numbered mirror.
   * 
   * In version 3, it is calculated as the third vertex of 
   * an equilateral triangle with its other two vertices located at
   * the highest- and lowest-numbered mirrors, at a height equal to the
   * midpoint between the heights of those two mirrors, and located behind
   * (rather than in front of) the mirrors. This is a compromise between
   * GPS-measured quantities and the desire to minimize parallax. */


  real8 latitude;     // Site latitude in radians
  real8 longitude;      // Site longitude in radians
  real8 altitude;     // Site altitude in meters above WGS84 reference ellipsoid

  real8 vclf[3];      // vector to vertical axis at CLF relative to center of earth (meters)
  real8 vsite[3];     // vector to vertical axis at site relative to center of earth (meters)
  
  
  real8 local_vsite[3];     // position of local site origin relative to CLF (east, north, relative altitude in meters)

  real8 site2earth[3][3];   // Rotation matrix to rotate from site coordinates to earth coordinates
  real8 site2clf[3][3];     // Rotation matrix to rotate from site coordinates to CLF coordinates

  
  
  /* The location of "a mirror" is the intersection of the mirror axis
   * with the spherical mirror surface, though there is no physical reflective
   * surface at that location. */
  real8 local_vmir[12][3];  // vector to mirror from site origin
  
  
  
  real8 local_vcam[12][3]; // vector to camera-face center from site origin (not used?)
  real8 vmir[12][3];    // Mirror pointing directions relative to local site origin

  real8 mir_lat[12];  // Mirror latitude (radians)
  real8 mir_lon[12];  // Mirror longitude (radians)
  real8 mir_alt[12];  // Mirror altitude above sea level (meters)

  real8 mir_the[12];  // Zenith angle of mirror pointing direction relative to local site origin (radians)
  real8 mir_phi[12];  // Azimuthal angle of mirror pointing direction relative to local site origin (radians)
  real8 rcurve[12]; // effective mirror radius of curvature (meters)

  real8 sep[12];  // effective mirror-camera separation (meters)
                              // interpreted by TRUMP as distance between mirror point 
                              // and PMT face behind the BG3 filter
  real8 site2cam[12][3][3]; // Rotation matrix to rotate vmir to (0, 0, 1)

  /* x and y coordinates of tubes in camera box (meters)
   * (origin at center, when facing camera box, +x is to the LEFT (new), +y is up)
   */
  real8 xtube[GEOFD_MIRTUBE];
  real8 ytube[GEOFD_MIRTUBE];

  /* mean direction vectors of tubes relative to local site origin */
  real8 vtube[12][GEOFD_MIRTUBE][3];

  /* unit vectors to mirror segment centers relative to center of curvature of mirror */ /* deprecated in v3! */
  real8 vseg[18][3];

  real8 diameter;   // largest-distance diameter of mirror (meters)

  real8 cam_width;  // Width of camera box (meters)
  real8 cam_height; // Height of camera box (meters)
  real8 cam_depth;  // Depth of camera box (meters)

//   real8 uvled_width[GEOFD_MAXMIR];  // Width of UVLED housing (meters)
//   real8 uvled_height[GEOFD_MAXMIR]; // Height of UVLED housing (meters)
//   real8 uvled_depth[GEOFD_MAXMIR];  // Depth of UVLED housing (meters)
//   real8 uvled_sep[GEOFD_MAXMIR];

  real8 pmt_flat2flat;  // flat-to-flat distance on PMT (meters)
  real8 pmt_point2point;  // point-to-point distance on PMT (meters)

  /* The following segment distances are along the chord of the sphere (two dimensional distance) */
  real8 seg_flat2flat;  // mirror segment 2D flat-to-flat distance (meters)
  real8 seg_point2point;  // mirror segment 2D point-to-point distance (meters)

  integer4 ring[12];  // mirror ring number
  integer4 siteid;              // site id (BR = 0, LR = 1)

  // Everything that follows is new to Version 3
  integer4 nmir;                  // Number of mirrors / cameras for this site
  
  integer4 nseg[GEOFD_MAXMIR];                  // Number of mirror segments per telescope
  integer4 ring3[GEOFD_MAXMIR];  // mirror ring number
  real8 diameters[GEOFD_MAXMIR];
  
  real8 local_vmir3[GEOFD_MAXMIR][3];
  real8 local_vcam3[GEOFD_MAXMIR][3];
  real8 vmir3[GEOFD_MAXMIR][3];
  
  real8 mir_lat3[GEOFD_MAXMIR]; // Mirror latitude
  real8 mir_lon3[GEOFD_MAXMIR];  // Mirror longitude (radians)
  real8 mir_alt3[GEOFD_MAXMIR];  // Mirror altitude above sea level (meters)

  real8 mir_the3[GEOFD_MAXMIR];  // Zenith angle of mirror pointing direction relative to local site origin (radians)
  real8 mir_phi3[GEOFD_MAXMIR];  // Azimuthal angle of mirror pointing direction relative to local site origin (radians)
  real8 rcurve3[GEOFD_MAXMIR]; // effective mirror radius of curvature (meters)

  real8 sep3[GEOFD_MAXMIR];  // effective mirror-camera separation (meters)
                              // interpreted by TRUMP as distance between mirror point 
                              // and PMT face behind the BG3 filter
  real8 site2cam3[GEOFD_MAXMIR][3][3]; // Rotation matrix to rotate vmir to (0, 0, 1)
  
  real8 vtube3[GEOFD_MAXMIR][GEOFD_MIRTUBE][3];
  
  
  
  integer4 camtype[GEOFD_MAXMIR];   // (0 = HIRES, 1 = TA, 2 = TALE)
  
  
  real8 vseg3[GEOFD_MAXMIR][GEOFD_SEGMENT][3]; // unit vector from center of
  // curvature to segment face center
  
  real8 seg_center[GEOFD_MAXMIR][GEOFD_SEGMENT][3]; // position of mirror segment curvature center relative to mirror curvature center
//   real8 seg_axis[GEOFD_MAXMIR][GEOFD_SEGMENT][3]; // unit vector from segment center toward SEGMENT center of curvature
  
  real8 rotation[GEOFD_MAXMIR]; // rotation of the camera about mirror axis

  real8 seg_rcurve[GEOFD_MAXMIR][GEOFD_SEGMENT]; // segment spherical radius of curvature in meters
  real8 seg_spot[GEOFD_MAXMIR][GEOFD_SEGMENT]; // width parameter of 2D Gaussian, in degrees
  real8 seg_orient[GEOFD_MAXMIR][GEOFD_SEGMENT]; // orientation of segment x-axis, radians relative to camera
  
  /* ellipsoidal radii of curvature in meters, with segment x- and y-axes defined by
   * seg_orient -- not yet used */
  real8 seg_rcurvex[GEOFD_MAXMIR][GEOFD_SEGMENT];
  real8 seg_rcurvey[GEOFD_MAXMIR][GEOFD_SEGMENT]; 
  real8 seg_spotx[GEOFD_MAXMIR][GEOFD_SEGMENT];
  real8 seg_spoty[GEOFD_MAXMIR][GEOFD_SEGMENT];
  
} geofd_dst_common;

extern geofd_dst_common geofd_;
extern integer4 geofd_blen; /* needs to be accessed by the c files of the derived banks */ 

integer4 geofd_struct_to_abank_(geofd_dst_common *geofd, integer1 *(*pbank), integer4 id, integer4 ver);
integer4 geofd_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 geofd_struct_to_dst_(geofd_dst_common *geofd, integer1 *bank, integer4 *unit, integer4 id, integer4 ver);
integer4 geofd_abank_to_struct_(integer1 *bank, geofd_dst_common *geofd);
integer4 geofd_struct_to_dump_(geofd_dst_common *geofd, integer4 *opt);
integer4 geofd_struct_to_dumpf_(geofd_dst_common *geofd, FILE *fp, integer4 *opt);

#endif
