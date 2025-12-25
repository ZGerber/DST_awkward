/*
 * New bank for BLACK ROCK MESA raw data
 * SRS - 3.12.08
 *
 * Modified to use fdraw
 * DRB 2008/09/23
 */

#ifndef _BRRAW_
#define _BRRAW_

#include <stdio.h>
#include "dst_std_types.h"

#include "fdraw_dst.h"

#define brraw_nmir_max fdraw_nmir_max         /* number of cameras per site        */
#define brraw_nchan_mir fdraw_nchan_mir       /* number of tubes per camera        */
#define brraw_nt_chan_max fdraw_nt_chan_max   /* number of time bins per tube      */

#define BRRAW_BANKID  12102
#define BRRAW_BANKVERSION   001

#ifdef __cplusplus
extern "C" {
#endif
integer4 brraw_common_to_bank_();
integer4 brraw_bank_to_dst_(integer4 *unit);
integer4 brraw_common_to_dst_(integer4 *unit); /* combines above 2 */
integer4 brraw_bank_to_common_(integer1 *bank);
integer4 brraw_common_to_dump_(integer4 *opt) ;
integer4 brraw_common_to_dumpf_(FILE* fp,integer4 *opt);
/* get (packed) buffer pointer and size */
integer1* brraw_bank_buffer_ (integer4* brraw_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdraw_dst_common brraw_dst_common;
extern brraw_dst_common brraw_;

#endif
