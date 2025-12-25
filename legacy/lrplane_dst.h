/* Created 2008/09/24 DRB */

#ifndef _LRPLANE_DST_
#define _LRPLANE_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "fdplane_dst.h"

#define LRPLANE_BANKID		12203
#define LRPLANE_BANKVERSION	002

#ifdef __cplusplus
extern "C" {
#endif
integer4 lrplane_common_to_bank_();
integer4 lrplane_bank_to_dst_(integer4 *NumUnit);
integer4 lrplane_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 lrplane_bank_to_common_(integer1 *bank);
integer4 lrplane_common_to_dump_(integer4 *opt1) ;
integer4 lrplane_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* lrplane_bank_buffer_ (integer4* lrplane_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdplane_dst_common lrplane_dst_common;
extern lrplane_dst_common lrplane_;

#endif
