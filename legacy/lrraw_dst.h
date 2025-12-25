/*
 * New bank for LONG RIDGE raw data
 * SRS - 3.12.08
 *
 * Modified to use fdraw
 * DRB 2008/09/23
 */

#ifndef _LRRAW_
#define _LRRAW_

#include <stdio.h>
#include "dst_std_types.h"

#include "fdraw_dst.h"

#define lrraw_nmir_max fdraw_nmir_max         /* number of cameras per site        */
#define lrraw_nchan_mir fdraw_nchan_mir       /* number of tubes per camera        */
#define lrraw_nt_chan_max fdraw_nt_chan_max   /* number of time bins per tube      */

//#define LRRAW_BANKID  12202
#define LRRAW_BANKID  12201
#define LRRAW_BANKVERSION   001

#ifdef __cplusplus
extern "C" {
#endif
integer4 lrraw_common_to_bank_();
integer4 lrraw_bank_to_dst_(integer4 *unit);
integer4 lrraw_common_to_dst_(integer4 *unit); /* combines above 2 */
integer4 lrraw_bank_to_common_(integer1 *bank);
integer4 lrraw_common_to_dump_(integer4 *opt) ;
integer4 lrraw_common_to_dumpf_(FILE* fp,integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* lrraw_bank_buffer_ (integer4* lrraw_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdraw_dst_common lrraw_dst_common;
extern lrraw_dst_common lrraw_;

#endif
