/*
 * brhyp1_dst.h
 *
 *  Created on: Apr 13, 2010
 *      Author: elliottb
 */

#ifndef BRHYP1_DST_H_
#define BRHYP1_DST_H_

#include "hyp1_dst.h"

#define BRHYP1_BANKID		13301
#define BRHYP1_BANKVERSION	000

#ifdef __cplusplus
extern "C" {
#endif
integer4 brhyp1_common_to_bank_();
integer4 brhyp1_bank_to_dst_(integer4 *NumUnit);
integer4 brhyp1_common_to_dst_(integer4 *NumUnit);	// combines above 2
integer4 brhyp1_bank_to_common_(integer1 *bank);
integer4 brhyp1_common_to_dump_(integer4 *opt1) ;
integer4 brhyp1_common_to_dumpf_(FILE* fp,integer4 *opt2);
/* get (packed) buffer pointer and size */
integer1* brhyp1_bank_buffer_ (integer4* brhyp1_bank_buffer_size);
#ifdef __cplusplus
} //end extern "C"
#endif


typedef hyp1_dst_common brhyp1_dst_common;
extern brhyp1_dst_common brhyp1_;

#endif /* BRHYP1_DST_H_ */
