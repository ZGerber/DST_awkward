#ifndef HYP1_DST_H_
#define HYP1_DST_H_

#include "math.h"

#include "univ_dst.h"
#include "fdraw_dst.h"

#define HYP1_BANKID 13300
#define HYP1_BANKVERSION 002
#define SDRAWMWF 0x100
#define FDPLANE_MAXTUBE 2000
#define HYP1NFIT 4 // total possible fits
#define NCHI2COMP 4 // number different chi2 components

#ifdef __cplusplus
extern "C" {
#endif
integer4 hyp1_common_to_bank_();
integer4 hyp1_bank_to_dst_(integer4 *unit);
integer4 hyp1_common_to_dst_(integer4 *unit); // combines above 2 
integer4 hyp1_bank_to_common_(integer1 *bank);
integer4 hyp1_common_to_dump_(integer4 *opt) ;
integer4 hyp1_common_to_dumpf_(FILE* fp,integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* hyp1_bank_buffer_ (integer4* hyp1_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif

//integer4 hyp1_struct_to_abank_(hyp1_dst_common *hyp1, integer1 *(*pbank), integer4 id, integer4 ver);

//static void hyp1_abank_init(integer1* (*pbank) );

typedef struct {
  integer2 eventCode; // 1=data, 0=montecarlo 
  
  real8 tref; // ns after the second for event.  Other time values are wrt tref
              // tref comes from rufptn->tearliest
  real8 offset; // offset between FD and SD [ns]
  /*
   * Time and event ID values were stored from both the FD and
   * SD banks.  This is really not needed, but shouldn't change
   * this now, as there are many DST files around containing hyp1
   * banks
   */
  integer4 fdEventNum; // trigger id number
  integer4 fdsiteid; // 0-BR, 1-LR
  integer4 julian; // julian day
  integer4 jsecond; // second into julian day
  integer4 jsecfrac; // time after sec that event starts for BRM/LR fraction of second in nanosec
  
  integer4 sdEventNum;
  integer4 sdsiteid; // 0-BR, 1-LR, 2-SK, 3-BRLR,4-BRSK,5-LRSK,6-BRLRSK
  integer4 yymmdd; //sd timing included for extra not getting confused ;)
  integer4 hhmmss;
  integer4 usec;  

  integer4 nfit; // number of fits in current analysis

  // fd quantities
  integer4 ngtube;
  real8 tubeVector[FDPLANE_MAXTUBE][3]; // tube vectors in CLF
  real8 tubeAlt[FDPLANE_MAXTUBE]; // tube alt in CLF frame
  real8 tubeAzm[FDPLANE_MAXTUBE]; // tube azm in CLF frame
  real8 tubeSigma[FDPLANE_MAXTUBE]; // sigma that this tube is above night sky background
  integer4 fdplaneIndex[FDPLANE_MAXTUBE]; // index from the fdplane bank
  real8 npe[FDPLANE_MAXTUBE]; // npe calculated in fdplane bank
  integer4 fdTime[FDPLANE_MAXTUBE]; // tube trigger time ns after jsecfrac
  real8 fdTimeRMS[FDPLANE_MAXTUBE]; // RMS of the signal region of FD waveform
  real8 planeAlt[HYP1NFIT][FDPLANE_MAXTUBE]; // tube alt, rotated into sdp from ith fit
  real8 planeAzm[HYP1NFIT][FDPLANE_MAXTUBE]; // tube alt, rotated into sdp from ith fit
  
  //sd quantities
  integer4 nhits;
  real8 xyz[SDRAWMWF][3]; // GPS position of SD in meters from rufptn bank
  real8 rho[SDRAWMWF]; // charge density of SD vem/m^2
  real8 sdTime[HYP1NFIT][SDRAWMWF]; // tube time including t_d delay function [ns]
  real8 sdTimeSigma[HYP1NFIT][SDRAWMWF]; // sigma from t_s formula [ns]
  real8 sdPlaneAlt[HYP1NFIT][SDRAWMWF]; // equivalent 'tube' alt in sdp from ith fit
  real8 sdPlaneAzm[HYP1NFIT][SDRAWMWF]; // equivalent 'tube' azm in sdp from ith fit
  integer4 rufptnIndex[SDRAWMWF]; // SD's index in the rufptn bank

  /*
   * Fit Quantities
   *
   * This bank will be allowed to expand to a variety of fitting styles.
   * Hybrid seems easily expandable to fitting the data in different ways.
   * If I feel like trying a new options this should make things easier.
   *
   * Here is a map of how things are set up now.
   * 0 -> 4-comp fit.
   * 1 -> fit using SDP from FD
   */

  integer1 fitType[HYP1NFIT][128]; // character string describing ith fit
  real8 sdp[HYP1NFIT][3];
  real8 rp[HYP1NFIT]; // meters
  real8 dRp[HYP1NFIT]; // meters
  real8 psi[HYP1NFIT]; // rad
  real8 dPsi[HYP1NFIT]; // rad
  real8 t0[HYP1NFIT]; // time crossing rp, ns
  real8 dT0[HYP1NFIT]; // ns

  real8 xcore[HYP1NFIT]; // meters
  real8 dXcore[HYP1NFIT]; // meters
  real8 ycore[HYP1NFIT]; // meters
  real8 dYcore[HYP1NFIT]; // meters
  real8 zen[HYP1NFIT]; // rad
  real8 dZen[HYP1NFIT]; // rad
  real8 azm[HYP1NFIT]; // rad
  real8 dAzm[HYP1NFIT]; // rad
  real8 tc[HYP1NFIT]; // time crossing CLF_z == 0, ns
  real8 dTc[HYP1NFIT];

  integer4 nComp[HYP1NFIT]; // number of chi2 components for ith fit
  /*
   * Outline of chi2Comp:
   * [0][0] -> FD timing
   *    [1] -> SD timing
   *    [2] -> SDP
   *    [3] -> center of charge
   *
   * Note: if this chi2 component is not relevant to the
   * particular fit, the value will be zero
   */
  real8 chi2Comp[HYP1NFIT][NCHI2COMP]; // chi2 from difference components (up to four)
  integer4 nparam[HYP1NFIT]; // number of fit params for reduced chi2
  real8 chi2[HYP1NFIT];

  /*
   * time residuals residuals for each tube or SD
   * in ns.
   * fdResidual applies to FD timing chi2 component
   * sdResidual applies to SD timing chi2 component
   */
  real8 fdResidual[HYP1NFIT][FDPLANE_MAXTUBE];
  real8 sdResidual[HYP1NFIT][SDRAWMWF];
} hyp1_dst_common;

integer4 hyp1_struct_to_abank_(hyp1_dst_common *hyp1, integer1 *(*pbank), integer4 id, integer4 ver);
integer4 hyp1_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 hyp1_struct_to_dst_(hyp1_dst_common *hyp1, integer1 *bank, integer4 *unit, integer4 id, integer4 ver);
integer4 hyp1_abank_to_struct_(integer1 *bank, hyp1_dst_common *hyp1);
integer4 hyp1_struct_to_dumpf_(hyp1_dst_common *hyp1, FILE * fp, integer4 * long_output);

extern hyp1_dst_common hyp1_;
extern integer4 hyp1_blen; /* needs to be accessed by the c files of the derived banks */

#endif /*HYP1_DST_H_*/
