/* Created 2010/05/04 ELB
 *
 * Using the fdtubeprofile bank to do profile fitting using the
 * fdtubeprofile codes
 */
#ifndef _HYTUBEPROFILE_DST_
#define _HYTUBEPROFILE_DST_

#include "fdtubeprofile_dst.h"

#define HYTUBEPROFILE_BANKID		13313
#define HYTUBEPROFILE_BANKVERSION	000

#ifdef __cplusplus
extern "C" {
#endif
integer4 hytubeprofile_common_to_bank_();
integer4 hytubeprofile_bank_to_dst_(integer4 *NumUnit);
integer4 hytubeprofile_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 hytubeprofile_bank_to_common_(integer1 *bank);
integer4 hytubeprofile_common_to_dump_(integer4 *opt1) ;
integer4 hytubeprofile_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* hytubeprofile_bank_buffer_ (integer4* hytubeprofile_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdtubeprofile_dst_common hytubeprofile_dst_common;
extern hytubeprofile_dst_common hytubeprofile_;

#endif
