/*
 * lrhyp1_dst.h
 *
 *  Created on: Apr 14, 2010
 *      Author: elliottb
 */

#ifndef LRHYP1_DST_H_
#define LRHYP1_DST_H_

#include "hyp1_dst.h"

#define LRHYP1_BANKID		13302
#define LRHYP1_BANKVERSION	000

#ifdef __cplusplus
extern "C" {
#endif
integer4 lrhyp1_common_to_bank_();
integer4 lrhyp1_bank_to_dst_(integer4 *NumUnit);
integer4 lrhyp1_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 lrhyp1_bank_to_common_(integer1 *bank);
integer4 lrhyp1_common_to_dump_(integer4 *opt1) ;
integer4 lrhyp1_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* lrhyp1_bank_buffer_ (integer4* lrhyp1_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef hyp1_dst_common lrhyp1_dst_common;
extern lrhyp1_dst_common lrhyp1_;

#endif /* LRHYP1_DST_H_ */
