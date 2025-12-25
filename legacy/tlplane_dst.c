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
#include "tlplane_dst.h"

tlplane_dst_common tlplane_;
static fdplane_dst_common* tlplane = &tlplane_;

//static integer4 tlplane_blen;
static integer4 tlplane_maxlen = sizeof(integer4) * 2 + sizeof(tlplane_dst_common);
static integer1 *tlplane_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* tlplane_bank_buffer_ (integer4* tlplane_bank_buffer_size)
{
  (*tlplane_bank_buffer_size) = fdplane_blen;
  return tlplane_bank;
}



static void tlplane_bank_init() {
  tlplane_bank = (integer1 *)calloc(tlplane_maxlen, sizeof(integer1));
  if (tlplane_bank==NULL) {
      fprintf (stderr,"tlplane_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 tlplane_common_to_bank_() {
  if (tlplane_bank == NULL) tlplane_bank_init();
  return fdplane_struct_to_abank_(tlplane, &tlplane_bank, TLPLANE_BANKID, TLPLANE_BANKVERSION);
}

integer4 tlplane_bank_to_dst_ (integer4 *unit) {
  return fdplane_abank_to_dst_(tlplane_bank, unit);
}

integer4 tlplane_common_to_dst_(integer4 *unit) {
  if (tlplane_bank == NULL) tlplane_bank_init();
  return fdplane_struct_to_dst_(tlplane, tlplane_bank, unit, TLPLANE_BANKID, TLPLANE_BANKVERSION);
}

integer4 tlplane_bank_to_common_(integer1 *bank) {
  return fdplane_abank_to_struct_(bank, tlplane);
}

integer4 tlplane_common_to_dump_(integer4 *opt) {
  return fdplane_struct_to_dumpf_(tlplane, stdout, opt);
}

integer4 tlplane_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdplane_struct_to_dumpf_(tlplane, fp, opt);
}
