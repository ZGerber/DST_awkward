/*
 * C functions for brraw
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
#include "brraw_dst.h"  

brraw_dst_common brraw_;  /* allocate memory to brraw_common */
static fdraw_dst_common* brraw = &brraw_;

//static integer4 brraw_blen; 
static integer4 brraw_maxlen = sizeof(integer4) * 2 + sizeof(brraw_dst_common);
static integer1 *brraw_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* brraw_bank_buffer_ (integer4* brraw_bank_buffer_size)
{
  (*brraw_bank_buffer_size) = fdraw_blen;
  return brraw_bank;
}



static void brraw_bank_init() {
  brraw_bank = (integer1 *)calloc(brraw_maxlen, sizeof(integer1));
  if (brraw_bank==NULL) {
      fprintf (stderr,"brraw_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 brraw_common_to_bank_() {
  if (brraw_bank == NULL) brraw_bank_init();
  return fdraw_struct_to_abank_(brraw, &brraw_bank, BRRAW_BANKID, BRRAW_BANKVERSION);
}

integer4 brraw_bank_to_dst_ (integer4 *unit) {
  return fdraw_abank_to_dst_(brraw_bank, unit);
}

integer4 brraw_common_to_dst_(integer4 *unit) {
  if (brraw_bank == NULL) brraw_bank_init();
  return fdraw_struct_to_dst_(brraw, &brraw_bank, unit, BRRAW_BANKID, BRRAW_BANKVERSION);
}

integer4 brraw_bank_to_common_(integer1 *bank) {
  return fdraw_abank_to_struct_(bank, brraw);
}

integer4 brraw_common_to_dump_(integer4 *opt) {
  return fdraw_struct_to_dumpf_(0, brraw, stdout, opt);
}

integer4 brraw_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdraw_struct_to_dumpf_(0, brraw, fp, opt);
}
