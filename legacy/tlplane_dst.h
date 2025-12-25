/* Created 2008/03/15 LMS
 * Modified to use fdplane 2008/09/23 DRB
 */

#ifndef _TLPLANE_DST_
#define _TLPLANE_DST_

#include "fdplane_dst.h"

#define TLPLANE_BANKID		12503
#define TLPLANE_BANKVERSION	000

#ifdef __cplusplus
extern "C" {
#endif
integer4 tlplane_common_to_bank_();
integer4 tlplane_bank_to_dst_(integer4 *NumUnit);
integer4 tlplane_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 tlplane_bank_to_common_(integer1 *bank);
integer4 tlplane_common_to_dump_(integer4 *opt1) ;
integer4 tlplane_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* tlplane_bank_buffer_ (integer4* tlplane_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdplane_dst_common tlplane_dst_common;
extern tlplane_dst_common tlplane_;

#endif
