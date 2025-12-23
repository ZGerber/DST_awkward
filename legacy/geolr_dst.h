/* geolr_dst.h
 * Created 2008/10.06 LMS
 * see geofd_dst.h for more information
 */

#ifndef _GEOLR_DST_
#define _GEOLR_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "geofd_dst.h"

#define GEOLR_BANKID          12202
#define GEOLR_BANKVERSION     003

#define GEOLR_MAXMIR    12
#define GEOLR_MIRTUBE   256

#define GEOLR_ROW 16    // Number of rows of PMTs
#define GEOLR_COL 16    // Number of columns of PMTs
#define GEOLR_SEGMENT   18    // Number of mirror segments

#define GEOLR_PMT_GAP   0.002 // GAP between flat sides of PMTs (cm)

/* Official values */
// This is the position of mirror 0.
#define LR_LATITUDE     0.68430834808004
#define LR_LONGITUDE    -1.9743417170361
// #define LR_LATITUDE       0.684308363
// #define LR_LONGITUDE     -1.974341665
#define LR_ALTITUDE       1544.665

#define GEOLR_LATITUDE  LR_LATITUDE
#define GEOLR_LONGITUDE LR_LONGITUDE
#define GEOLR_ALTITUDE  LR_ALTITUDE

/* prior to 2011-04 survey
#define GEOLR_LATITUDE   0.6843072969     //   39.20792 degrees
#define GEOLR_LONGITUDE -1.97434211 // -113.12147 degrees
#define GEOLR_ALTITUDE  1554.0            // meters
*/

/* Measured in August 2008 on top of building
 * #define GEOLR_LATITUDE   0.68430695    //   39.20790 degrees
 * #define GEOLR_LONGITUDE -1.97434385    // -113.12157 degrees
 * #define GEOLR_ALTITUDE  1554.0   // meters
 */

#ifdef __cplusplus
extern "C" {
#endif
integer4 geolr_common_to_bank_();
integer4 geolr_bank_to_dst_(integer4 *NumUnit);
integer4 geolr_common_to_dst_(integer4 *NumUnit); // combines above 2
integer4 geolr_bank_to_common_(integer1 *bank);
integer4 geolr_common_to_dump_(integer4 *opt1) ;
integer4 geolr_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* geolr_bank_buffer_ (integer4* geolr_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef geofd_dst_common geolr_dst_common;
extern geolr_dst_common geolr_;

#endif

