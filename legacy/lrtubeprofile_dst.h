/* Created 2010/01 LMS */

#ifndef _LRTUBEPROFILE_DST_
#define _LRTUBEPROFILE_DST_

#include <stdio.h>
#include "dst_std_types.h"

#include "fdtubeprofile_dst.h"

#define LRTUBEPROFILE_BANKID		12206
#define LRTUBEPROFILE_BANKVERSION	FDTUBEPROFILE_BANKVERSION

#ifdef __cplusplus
extern "C" {
#endif
integer4 lrtubeprofile_common_to_bank_();
integer4 lrtubeprofile_bank_to_dst_(integer4 *NumUnit);
integer4 lrtubeprofile_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 lrtubeprofile_bank_to_common_(integer1 *bank);
integer4 lrtubeprofile_common_to_dump_(integer4 *opt1) ;
integer4 lrtubeprofile_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* lrtubeprofile_bank_buffer_ (integer4* lrtubeprofile_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdtubeprofile_dst_common lrtubeprofile_dst_common;
extern lrtubeprofile_dst_common lrtubeprofile_;

#endif
