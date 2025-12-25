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
#include "brplane_dst.h"

brplane_dst_common brplane_;
static fdplane_dst_common* brplane = &brplane_;

//static integer4 brplane_blen;
static integer4 brplane_maxlen = sizeof(integer4) * 2 + sizeof(brplane_dst_common);
static integer1 *brplane_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* brplane_bank_buffer_ (integer4* brplane_bank_buffer_size)
{
  (*brplane_bank_buffer_size) = fdplane_blen;
  return brplane_bank;
}



static void brplane_bank_init() {
  brplane_bank = (integer1 *)calloc(brplane_maxlen, sizeof(integer1));
  if (brplane_bank==NULL) {
      fprintf (stderr,"brplane_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 brplane_common_to_bank_() {
  if (brplane_bank == NULL) brplane_bank_init();
  return fdplane_struct_to_abank_(brplane, &brplane_bank, BRPLANE_BANKID, BRPLANE_BANKVERSION);
}

integer4 brplane_bank_to_dst_ (integer4 *unit) {
  return fdplane_abank_to_dst_(brplane_bank, unit);
}

integer4 brplane_common_to_dst_(integer4 *unit) {
  if (brplane_bank == NULL) brplane_bank_init();
  return fdplane_struct_to_dst_(brplane, brplane_bank, unit, BRPLANE_BANKID, BRPLANE_BANKVERSION);
}

integer4 brplane_bank_to_common_(integer1 *bank) {
  return fdplane_abank_to_struct_(bank, brplane);
}

integer4 brplane_common_to_dump_(integer4 *opt) {
  return fdplane_struct_to_dumpf_(brplane, stdout, opt);
}

integer4 brplane_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdplane_struct_to_dumpf_(brplane, fp, opt);
}
