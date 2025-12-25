/* Created 2008/03/15 LMS
 * Modified to use fdplane 2008/09/23 DRB
 */

#ifndef _BRPLANE_DST_
#define _BRPLANE_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "fdplane_dst.h"

#define BRPLANE_BANKID		12103
#define BRPLANE_BANKVERSION	002

#ifdef __cplusplus
extern "C" {
#endif
integer4 brplane_common_to_bank_();
integer4 brplane_bank_to_dst_(integer4 *NumUnit);
integer4 brplane_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 brplane_bank_to_common_(integer1 *bank);
integer4 brplane_common_to_dump_(integer4 *opt1) ;
integer4 brplane_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* brplane_bank_buffer_ (integer4* brplane_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdplane_dst_common brplane_dst_common;
extern brplane_dst_common brplane_;

#endif
