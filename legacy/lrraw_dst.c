/*
 * C functions for lrraw
 * SRS - 3.12.08
 *
 * Modified to use fdraw
 * DRB 2008/09/23
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "fdraw_dst.h"  
#include "lrraw_dst.h"  

lrraw_dst_common lrraw_;  /* allocate memory to lrraw_common */
static fdraw_dst_common* lrraw = &lrraw_;

//static integer4 lrraw_blen; 
static integer4 lrraw_maxlen = sizeof(integer4) * 2 + sizeof(lrraw_dst_common);
static integer1 *lrraw_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* lrraw_bank_buffer_ (integer4* lrraw_bank_buffer_size)
{
  (*lrraw_bank_buffer_size) = fdraw_blen;
  return lrraw_bank;
}



static void lrraw_bank_init() {
  lrraw_bank = (integer1 *)calloc(lrraw_maxlen, sizeof(integer1));
  if (lrraw_bank==NULL) {
      fprintf (stderr,"lrraw_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 lrraw_common_to_bank_() {
  if (lrraw_bank == NULL) lrraw_bank_init();
  return fdraw_struct_to_abank_(lrraw, &lrraw_bank, LRRAW_BANKID, LRRAW_BANKVERSION);
}

integer4 lrraw_bank_to_dst_ (integer4 *unit) {
  return fdraw_abank_to_dst_(lrraw_bank, unit);
}

integer4 lrraw_common_to_dst_(integer4 *unit) {
  if (lrraw_bank == NULL) lrraw_bank_init();
  return fdraw_struct_to_dst_(lrraw, &lrraw_bank, unit, LRRAW_BANKID, LRRAW_BANKVERSION);
}

integer4 lrraw_bank_to_common_(integer1 *bank) {
  return fdraw_abank_to_struct_(bank, lrraw);
}

integer4 lrraw_common_to_dump_(integer4 *opt) {
  return fdraw_struct_to_dumpf_(1, lrraw, stdout, opt);
}

integer4 lrraw_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdraw_struct_to_dumpf_(1, lrraw, fp, opt);
}
