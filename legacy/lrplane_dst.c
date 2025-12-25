// Created 2008/03/16 LMS
// Modified to use fdplane 2008/09/23 DRB

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "fdplane_dst.h"
#include "lrplane_dst.h"

lrplane_dst_common lrplane_;
static fdplane_dst_common* lrplane = &lrplane_;

//static integer4 lrplane_blen;
static integer4 lrplane_maxlen = sizeof(integer4) * 2 + sizeof(lrplane_dst_common);
static integer1 *lrplane_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* lrplane_bank_buffer_ (integer4* lrplane_bank_buffer_size)
{
  (*lrplane_bank_buffer_size) = fdplane_blen;
  return lrplane_bank;
}



static void lrplane_bank_init() {
  lrplane_bank = (integer1 *)calloc(lrplane_maxlen, sizeof(integer1));
  if (lrplane_bank==NULL) {
      fprintf (stderr,"lrplane_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 lrplane_common_to_bank_() {
  if (lrplane_bank == NULL) lrplane_bank_init();
  return fdplane_struct_to_abank_(lrplane, &lrplane_bank, LRPLANE_BANKID, LRPLANE_BANKVERSION);
}

integer4 lrplane_bank_to_dst_ (integer4 *unit) {
  return fdplane_abank_to_dst_(lrplane_bank, unit);
}

integer4 lrplane_common_to_dst_(integer4 *unit) {
  if (lrplane_bank == NULL) lrplane_bank_init();
  return fdplane_struct_to_dst_(lrplane, lrplane_bank, unit, LRPLANE_BANKID, LRPLANE_BANKVERSION);
}

integer4 lrplane_bank_to_common_(integer1 *bank) {
  return fdplane_abank_to_struct_(bank, lrplane);
}

integer4 lrplane_common_to_dump_(integer4 *opt) {
  return fdplane_struct_to_dumpf_(lrplane, stdout, opt);
}

integer4 lrplane_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdplane_struct_to_dumpf_(lrplane, fp, opt);
}
