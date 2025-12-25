// Created 2010/01 LMS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "fdtubeprofile_dst.h"
#include "lrtubeprofile_dst.h"

lrtubeprofile_dst_common lrtubeprofile_;
static fdtubeprofile_dst_common* lrtubeprofile = &lrtubeprofile_;

//static integer4 lrtubeprofile_blen;
static integer4 lrtubeprofile_maxlen = sizeof(integer4) * 2 + sizeof(lrtubeprofile_dst_common);
static integer1 *lrtubeprofile_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* lrtubeprofile_bank_buffer_ (integer4* lrtubeprofile_bank_buffer_size)
{
  (*lrtubeprofile_bank_buffer_size) = fdtubeprofile_blen;
  return lrtubeprofile_bank;
}



static void lrtubeprofile_bank_init() {
  lrtubeprofile_bank = (integer1 *)calloc(lrtubeprofile_maxlen, sizeof(integer1));
  if (lrtubeprofile_bank==NULL) {
      fprintf (stderr,"lrtubeprofile_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 lrtubeprofile_common_to_bank_() {
  if (lrtubeprofile_bank == NULL) lrtubeprofile_bank_init();
  return fdtubeprofile_struct_to_abank_(lrtubeprofile, &lrtubeprofile_bank, LRTUBEPROFILE_BANKID, LRTUBEPROFILE_BANKVERSION);
}

integer4 lrtubeprofile_bank_to_dst_ (integer4 *unit) {
  return fdtubeprofile_abank_to_dst_(lrtubeprofile_bank, unit);
}

integer4 lrtubeprofile_common_to_dst_(integer4 *unit) {
  if (lrtubeprofile_bank == NULL) lrtubeprofile_bank_init();
  return fdtubeprofile_struct_to_dst_(lrtubeprofile, lrtubeprofile_bank, unit, LRTUBEPROFILE_BANKID, LRTUBEPROFILE_BANKVERSION);
}

integer4 lrtubeprofile_bank_to_common_(integer1 *bank) {
  return fdtubeprofile_abank_to_struct_(bank, lrtubeprofile);
}

integer4 lrtubeprofile_common_to_dump_(integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(lrtubeprofile, stdout, opt);
}

integer4 lrtubeprofile_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(lrtubeprofile, fp, opt);
}
