/*
 * prfc_dst.h
 *
 * $Source: /hires_soft/cvsroot/bank/prfc_dst.h,v $
 * $Log: prfc_dst.h,v $
 * Revision 1.3  2001/06/29 22:19:38  wiencke
 * added several paragraphs of documentation about this bank
 *
 * Revision 1.2  1996/07/02 22:30:09  mjk
 * Added PRFC_INSANE_TRAJECTORY failmode.
 *
 * Revision 1.1  1996/05/03  22:25:52  mjk
 * Initial revision
 *
 *
 * bank PRFC Contains profile information from the group profile
 * program, pfl. pfl is Dai's original pfgh fitting program after
 * substantial additions by MJK.
 *
 * The PRFC bank is strict superset of the PRFA bank and is
 * intended as its replacement. (N.B.: PRFB was already claimed
 * by jTang) 
 *   
 * All hires eyes are handled in one bank.
 *
 * It is expected that alternative profiling banks will be named
 * prfd, prfe, ...
 *
*/
/*-----------*/
/* PRFC bank */
/*-----------*/
/* Comments added 6-29-2001 lrw
 * This bank contains results of fitting shower profiles. 
 *
 * The shower profile, ie the number of particles along the shower axis,
 * is described by a 4 parameter function.  As of 6-29-01, the function is
 * a Gieser-Hillas function. The four parameters are
 * szmx, xm, x0, lambda.  These are described later.  From these the shower energy,
 * eng, is estimated.
 * Each parameter is actually a 1 dimensional array.
 * For stereo analysis an index of 0 indicates the measurement was
 * made by HiRes1 (for example prfc_.eng[0] ), and index of 1 indicates the 
 * measurement wsa made by HiRes2.  Higher indicies correspond to other fit
 * that may or not be filled depending on the program used to fill this bank.
 * As of 6-29-01, these profiles 0,1 were determined independently at each site using
 * a geometry determined a stereo fit. 
 *
 * Also provided is the profile of light along the shower axis
 * that would correspond to the profile of light measured by the detector.
 * The light profile measured by the detector is not in this bank.  It is
 * typically found in the hcbin bank.
 * Note that the profile of light at the shower and the profile of light at the
 * detector are very different, due primarily to geometrical and atmospheric effects!
 *
 * The light profile at the shower is separated into 4 components.  scin is the profile of
 * scintillation light, rayl the profile of cherenkov light scattered by the
 * molecular component of the atmosphere, aero the component scattered by the
 * aerosol part of the atmosphere, and crnk the direct (unscattered) part of
 * the cherenkov beam that would reach the detector.
 * The light profiles are divided into "nbin" bins.  The atmospheric depth
 * of the center of each bin is given by dep (slant grammage) and gm (vertical grammage)
 * End of Comments added 6-29-2001 lrw
 */
#ifndef _PRFC_
#define _PRFC_

#include <stdio.h>
#include "dst_std_types.h"

#define PRFC_BANKID 30002
#define PRFC_BANKVERSION 0 

#define PRFC_MAXFIT 16   /* hr1, hr2, combined hires1/hires2 +13 spare */
#define PRFC_MAXBIN 300
#define PRFC_MAXMEL 10   /* Maximum number of error matrix elements */

/* Define codes describing what part of each fit is used:
     1.  Profile results (Xmax, X0, ...) 
     2.  Bin information (i.e. light at each bin)
     3.  Error matrix

   Before packing this bank, the user MUST set the values for 
   prfc_.pflinfo[], prfc_.bininfo[], prfc_.mtxinfo[], and 
   prfc_.nbin[] */

#define PRFC_PFLINFO_USED    1
#define PRFC_PFLINFO_UNUSED  0

#define PRFC_BININFO_USED    1
#define PRFC_BININFO_UNUSED  0

#define PRFC_MTXINFO_USED    1
#define PRFC_MTXINFO_UNUSED  0

/* 
   Define several status codes. Each failmode[] should be filled 
   with SUCCESS or one of these defined value. More definitions may 
   be added as needed.  Packed as an integer2.
   
   PRFC_FIT_NOT_REQUESTED  - Reserved for marking fits that user
     did not want computed, even though necessary banks may have
     been present.
   PRFC_NOT_IMPLEMENTED - Reserved for marking fits that are not
     yet implemented in the code, for example, hr1+hr2 combined
     fit or the spares. The spare fits can be handy for specialized
     code which does the same fit under different physics assumptions.
   PRFC_REQUIRED_BANKS_MISSING - Indicates that the basic banks
     required for the fit are not available or have failed. For 
     example, the hr1 fit requires BIN1, and PLN1.
   PRFC_MISSING_TRAJECTORY_INFO - Indicates that trajectory info
     is not available. It is possible that some source of trajectory
     information is available (say TIM1), but it was not the source
     the user specified.
   PRFC_UPWARD_GOING_TRACK - Indicates track was upward going. Such
     tracks are not fitted.
   PRFC_TOO_FEW_BINS - Indicates there were not enough good bins
     for the fit. Bins may be lost due to the Cherenkov cut in the
     profile fitter program or earlier during the binning process.
   PRFC_FITTER_FAILURE - Indicates the fitter failed. Usually this
     means the fitter ran to an edge, i.e. x0 went to its extreme
     negative value.
   PRFC_INSANE_TRAJECTORY - The core location and/or track direction
     is so unreasonable that the event can not be processed.
*/

#define PRFC_FIT_NOT_REQUESTED         1
#define PRFC_NOT_IMPLEMENTED           2
#define PRFC_REQUIRED_BANKS_MISSING    3
#define PRFC_MISSING_TRAJECTORY_INFO   4
#define PRFC_UPWARD_GOING_TRACK       10
#define PRFC_TOO_FEW_GOOD_BINS        11
#define PRFC_FITTER_FAILURE           12
#define PRFC_INSANE_TRAJECTORY        13


/* Define codes for failure to compute parts of error. If errstat[]
   is not SUCCESS, it will be the sum of some set of these. "left"
   and "right" errors may fail if the fitter result for one/both of 
   these trajectories fails. Packed as an integer2 */

#define PRFC_STAT_ERROR_FAILURE        1
#define PRFC_RIGHT_ERROR_FAILURE       2
#define PRFC_LEFT_ERROR_FAILURE        4
#define PRFC_GEOM_ERROR_FAILURE        8
#define PRFC_GEOM_ERROR_INCOMPLETE    16

/* Define bin status codes. Codes are generally inherited by the 
   profile program from bin_.ig[]. However profile program can
   knock out bins, for example, because of Chrenkov contamination. 
   Packed as an integer2 */

#define PRFC_IG_GOODBIN                1
#define PRFC_IG_OVERCORRECTED          0
#define PRFC_IG_SICKPLNFIT            -1
#define PRFC_IG_CHERENKOV_CUT         -2


/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif
integer4 prfc_common_to_bank_(void);
integer4 prfc_bank_to_dst_(integer4 *NumUnit);
integer4 prfc_common_to_dst_(integer4 *NumUnit); /* combines above 2 */
integer4 prfc_bank_to_common_(integer1 *bank);
integer4 prfc_common_to_dump_(integer4 *long_output);
integer4 prfc_common_to_dumpf_(FILE* fp, integer4 *long_output);
/* get (packed) buffer pointer and size */
integer1* prfc_bank_buffer_ (integer4* prfc_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif



/* ==================================================================== */


typedef struct 
{
  /* There are several arrays ( fit[], bininfo[], nbin[] ) at the end 
     that the user must set correctly. I have put them at the end to 
     avoid fortran common alignment problems */

  /* Nonreduced chi2 value */
  real8 chi2    [PRFC_MAXFIT];

  /* Profile parameters from each eye. The dparam is the statistical
     error from the fit to the profile. The rparam and lparam are the
     results assuming the 1 sigma away geometry on either side of the
     best fit. tparam is a total geometrical error. This applies when
     the geometry fit is so well constrained that it is no longer
     reasonable to talk about "left" and "right" trajectories in the
     Rp, Psi chi^2 "trench" */

  real8 szmx	[PRFC_MAXFIT];   /* Shower size. Number of charged */
  real8 dszmx	[PRFC_MAXFIT];   /* particles at Shower Maximum */
  real8 rszmx	[PRFC_MAXFIT];   
  real8 lszmx	[PRFC_MAXFIT];   
  real8 tszmx	[PRFC_MAXFIT];   

  real8 xm	[PRFC_MAXFIT];   /* Xmax = Shower Maximum   g/cm^2 */
  real8 dxm	[PRFC_MAXFIT];
  real8 rxm	[PRFC_MAXFIT];
  real8 lxm	[PRFC_MAXFIT];
  real8 txm	[PRFC_MAXFIT];

  real8 x0	[PRFC_MAXFIT];   /* X0   = Shower initial point  g/cm^2 */
  real8 dx0	[PRFC_MAXFIT];
  real8 rx0	[PRFC_MAXFIT];
  real8 lx0	[PRFC_MAXFIT];
  real8 tx0	[PRFC_MAXFIT];

  real8 lambda	[PRFC_MAXFIT];   /* Elongation parameter  g/cm^2 */
  real8 dlambda	[PRFC_MAXFIT];
  real8 rlambda	[PRFC_MAXFIT];
  real8 llambda	[PRFC_MAXFIT];
  real8 tlambda	[PRFC_MAXFIT];

  real8 eng     [PRFC_MAXFIT];   /* Shower energy  EeV  */
  real8 deng    [PRFC_MAXFIT];
  real8 reng    [PRFC_MAXFIT];
  real8 leng    [PRFC_MAXFIT];
  real8 teng    [PRFC_MAXFIT];


  /* Information about the grammage at each bin along the shower 
     trajectory. Keep this information because it depends on the
     atmospheric model used. The BIN1 bank gives the unit vector 
     for the bin centers */

  real8 dep	[PRFC_MAXFIT][PRFC_MAXBIN];  /* slant grammage g/cm^2 */
  real8 gm	[PRFC_MAXFIT][PRFC_MAXBIN];  /* vertical grammage g/cm^2 */


  /* Computed light contributions from each source along the shower 
     trajectory. This is the unmormalized result in photoelectrons 
     received at the phototube. Actual result is obtained by normalizing
     by the shower size, szmx.  Note: The BIN1 bank gives the unit vectors 
     for the bin centers. */

  real8 scin	[PRFC_MAXFIT][PRFC_MAXBIN];   /* Scintillation */
  real8 rayl	[PRFC_MAXFIT][PRFC_MAXBIN];   /* Rayleigh Scattered */
  real8 aero	[PRFC_MAXFIT][PRFC_MAXBIN];   /* Aerosol Scattered */
  real8 crnk	[PRFC_MAXFIT][PRFC_MAXBIN];   /* Direct Cherenkov */
  real8 sigmc	[PRFC_MAXFIT][PRFC_MAXBIN];   /* Total MC signal */
  real8 sig	[PRFC_MAXFIT][PRFC_MAXBIN];   /* Signal measured */


  /* The idea here is since the error matrix is symmetric only 
     mor * (mor + 1) /2 elements need to be packed, where mor
     is the matrix order. Both nel and mor are packed as integer2 */

  real8 mxel    [PRFC_MAXFIT][PRFC_MAXMEL];   /* Error Matrix Elements */
  integer4 nel  [PRFC_MAXFIT];                /* Number of Matrix Elements */
  integer4 mor  [PRFC_MAXFIT];                /* Matix order */

  integer4 ig	[PRFC_MAXFIT][PRFC_MAXBIN];   /* flag of the bin */



/* 
   User must set these. Each fit:

   pflinfo[]  - should be set to PRFC_PFLINFO_USED or PRFC_PFLINFO_UNUSED
                depending on whether or not the fit contains valid profile 
		results. N.B. The fit contains still contains valid
		fit information when failmode!=SUCCESS, i.e. it contains
		information about how the fit failed.
        
   bininfo[]  - should be set to PRFC_BININFO_USED or PRFC_BININFO_UNUSED
                depending on whether or not the fit contains profile bin
		information, i.e. the light contributions in each bin.

   mtxinfo[]  - should be set to PRFC_MTXINFO_USED or PRFC_MTXINFO_UNUSED
                depending on whether or not the fit contains an error
		matrix.

   N.B. It is possible for a member of bininfo[] == PRFC_BININFO_UNUSED
        while the corresponding member of pflinfo[] == PRFC_PFLINFO_USED
	This may occur when different physics assumptions are being
	tested and one is only checking for differences in quantities
	like Xmax and Energy.


   failmode[] - Must be set if corresponding pflinfo[] == PRFC_PFLINFO_USED
   nbin[]     - Must be set if corresponding bininfo[] == PRFC_BININFO_USED
                This will normally be equal to the number of bins in 
		the source bank, i.e. BIN1 or presumably BIN2 when the 
		hires2 analysis gets that far. This value must not
		exceed PRFC_MAXBIN.
*/
 
  integer4 pflinfo [PRFC_MAXFIT];
  integer4 bininfo [PRFC_MAXFIT];
  integer4 mtxinfo [PRFC_MAXFIT];

  integer4 failmode[PRFC_MAXFIT];
  integer4 nbin    [PRFC_MAXFIT]; 


  /* Trajectory source. This should be filled with the bankid
     of the bank used as the trajectory source */

  integer4 traj_source[PRFC_MAXFIT];

  /* Status of errors. Usually this will be filled with SUCCESS,
     but if there is some problem computing the "left" or "right"
     trajectories, an errstat[] value could be fill with something
     like PRFC_LEFT_ERROR_FAILURE */

  integer4 errstat[PRFC_MAXFIT]; 
  
  /* Number of degrees of freedom for the chi2 fit. Reduced chi2
     is chi2[]/ndf[] */

  integer4 ndf[PRFC_MAXFIT];

} prfc_dst_common ;

extern prfc_dst_common prfc_ ; 

#endif




