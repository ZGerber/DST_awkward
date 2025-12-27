/*
 * brhyp1_dst.c
 *
 *  Created on: Apr 13, 2010
 *      Author: elliottb
 */

#include <stdio.h>
#include <stdlib.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "hyp1_dst.h"
#include "brhyp1_dst.h"

hyp1_dst_common brhyp1_;
static hyp1_dst_common* brhyp1 = &brhyp1_;

static integer4 hyp1_maxlen = sizeof(integer4) * 2 + sizeof(brhyp1_dst_common);
static integer1 *brhyp1_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* brhyp1_bank_buffer_ (integer4* brhyp1_bank_buffer_size)
{
  (*brhyp1_bank_buffer_size) = hyp1_blen;
  return brhyp1_bank;
}



static void brhyp1_bank_init() {
  brhyp1_bank = (integer1 *)calloc(hyp1_maxlen, sizeof(integer1));
  if (brhyp1_bank==NULL) {
      fprintf (stderr,"brhyp1_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 brhyp1_common_to_bank_() {
  if (brhyp1_bank == NULL) brhyp1_bank_init();
  return hyp1_struct_to_abank_(brhyp1, &brhyp1_bank, BRHYP1_BANKID, BRHYP1_BANKVERSION);
}

integer4 brhyp1_bank_to_dst_ (integer4 *unit) {
  return hyp1_abank_to_dst_(brhyp1_bank, unit);
}

integer4 brhyp1_common_to_dst_(integer4 *unit) {
  if (brhyp1_bank == NULL) brhyp1_bank_init();
  return hyp1_struct_to_dst_(brhyp1, brhyp1_bank, unit, BRHYP1_BANKID, BRHYP1_BANKVERSION);
}

integer4 brhyp1_bank_to_common_(integer1 *bank) {
  return hyp1_abank_to_struct_(bank, brhyp1);
}

integer4 brhyp1_common_to_dump_(integer4 *opt) {
  return hyp1_common_to_dumpf_(stdout, opt);
}

integer4 brhyp1_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return hyp1_struct_to_dumpf_(brhyp1, fp, opt);
}

