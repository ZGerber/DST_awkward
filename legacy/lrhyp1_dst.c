/*
 * lrhyp1_dst.h
 *
 *  Created on: Apr 14, 2010
 *      Author: elliottb
 */

#include <stdio.h>
#include <stdlib.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "hyp1_dst.h"
#include "lrhyp1_dst.h"

hyp1_dst_common lrhyp1_;
static hyp1_dst_common* lrhyp1 = &lrhyp1_;

static integer4 hyp1_maxlen = sizeof(integer4) * 2 + sizeof(lrhyp1_dst_common);
static integer1 *lrhyp1_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* lrhyp1_bank_buffer_ (integer4* lrhyp1_bank_buffer_size)
{
  (*lrhyp1_bank_buffer_size) = hyp1_blen;
  return lrhyp1_bank;
}



static void lrhyp1_bank_init() {
  lrhyp1_bank = (integer1 *)calloc(hyp1_maxlen, sizeof(integer1));
  if (lrhyp1_bank==NULL) {
      fprintf (stderr,"lrhyp1_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 lrhyp1_common_to_bank_() {
  if (lrhyp1_bank == NULL) lrhyp1_bank_init();
  return hyp1_struct_to_abank_(lrhyp1, &lrhyp1_bank, LRHYP1_BANKID, LRHYP1_BANKVERSION);
}

integer4 lrhyp1_bank_to_dst_ (integer4 *unit) {
  return hyp1_abank_to_dst_(lrhyp1_bank, unit);
}

integer4 lrhyp1_common_to_dst_(integer4 *unit) {
  if (lrhyp1_bank == NULL) lrhyp1_bank_init();
  return hyp1_struct_to_dst_(lrhyp1, lrhyp1_bank, unit, LRHYP1_BANKID, LRHYP1_BANKVERSION);
}

integer4 lrhyp1_bank_to_common_(integer1 *bank) {
  return hyp1_abank_to_struct_(bank, lrhyp1);
}

integer4 lrhyp1_common_to_dump_(integer4 *opt) {
  return hyp1_common_to_dumpf_(stdout, opt);
}

integer4 lrhyp1_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return hyp1_struct_to_dumpf_(lrhyp1, fp, opt);
}

