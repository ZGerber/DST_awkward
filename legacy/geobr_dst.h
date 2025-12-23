/* geobr_dst.h
 * Created 2008/10.06 LMS
 * see geofd_dst.h for more information
 *
 * Updated 2011/08/30 by TAS to v. 001
 */

#ifndef _GEOBR_DST_
#define _GEOBR_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "geofd_dst.h"

#define GEOBR_BANKID    12101
#define GEOBR_BANKVERSION 003

#define GEOBR_MAXMIR  12
#define GEOBR_MIRTUBE 256

#define GEOBR_ROW 16  // Number of rows of PMTs
#define GEOBR_COL 16  // Number of columns of PMTs
#define GEOBR_SEGMENT 18  // Number of mirror segments

#define GEOBR_PMT_GAP 0.002 // GAP between flat sides of PMTs (cm)

/* Official values */
// this is the position of Mirror 0.
#define BR_LATITUDE 0.68396494432012 // 39.188304644444
#define BR_LONGITUDE -1.9671927094723 // -112.71183974167 degrees
// #define BR_LATITUDE       0.683964949
// #define BR_LONGITUDE     -1.967192675
#define BR_ALTITUDE       1396.632

#define GEOBR_LATITUDE  BR_LATITUDE
#define GEOBR_LONGITUDE BR_LONGITUDE
#define GEOBR_ALTITUDE  BR_ALTITUDE

/* prior to 2011-04 survey
#define GEOBR_LATITUDE   0.68396486 //   39.18830 degrees
#define GEOBR_LONGITUDE -1.96719027 // -112.71170 degrees
#define GEOBR_ALTITUDE  1404.0    // meters
*/
/* Measured in August 2008 on top of building
 * #define GEOBR_LATITUDE   0.68396451  //   39.18828 degrees
 * #define GEOBR_LONGITUDE -1.96719062  // -112.71172 degrees
 * #define GEOBR_ALTITUDE  1404.0 // meters
 */

#ifdef __cplusplus
extern "C" {
#endif
integer4 geobr_common_to_bank_();
integer4 geobr_bank_to_dst_(integer4 *NumUnit);
integer4 geobr_common_to_dst_(integer4 *NumUnit); // combines above 2
integer4 geobr_bank_to_common_(integer1 *bank);
integer4 geobr_common_to_dump_(integer4 *opt1) ;
integer4 geobr_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* geobr_bank_buffer_ (integer4* geobr_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef geofd_dst_common geobr_dst_common;
extern geobr_dst_common geobr_;

#endif
