/* Created 2011/10 TAS */

#ifndef _TLTUBEPROFILE_DST_
#define _TLTUBEPROFILE_DST_

#include "fdtubeprofile_dst.h"

#define TLTUBEPROFILE_BANKID		12506
#define TLTUBEPROFILE_BANKVERSION	FDTUBEPROFILE_BANKVERSION

#ifdef __cplusplus
extern "C" {
#endif
integer4 tltubeprofile_common_to_bank_();
integer4 tltubeprofile_bank_to_dst_(integer4 *NumUnit);
integer4 tltubeprofile_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 tltubeprofile_bank_to_common_(integer1 *bank);
integer4 tltubeprofile_common_to_dump_(integer4 *opt1) ;
integer4 tltubeprofile_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* tltubeprofile_bank_buffer_ (integer4* tltubeprofile_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef fdtubeprofile_dst_common tltubeprofile_dst_common;
extern tltubeprofile_dst_common tltubeprofile_;

#endif
