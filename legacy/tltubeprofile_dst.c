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
#include "tltubeprofile_dst.h"

tltubeprofile_dst_common tltubeprofile_;
static fdtubeprofile_dst_common* tltubeprofile = &tltubeprofile_;

//static integer4 tltubeprofile_blen;
static integer4 tltubeprofile_maxlen = sizeof(integer4) * 2 + sizeof(tltubeprofile_dst_common);
static integer1 *tltubeprofile_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* tltubeprofile_bank_buffer_ (integer4* tltubeprofile_bank_buffer_size)
{
  (*tltubeprofile_bank_buffer_size) = fdtubeprofile_blen;
  return tltubeprofile_bank;
}



static void tltubeprofile_bank_init() {
  tltubeprofile_bank = (integer1 *)calloc(tltubeprofile_maxlen, sizeof(integer1));
  if (tltubeprofile_bank==NULL) {
      fprintf (stderr,"tltubeprofile_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 tltubeprofile_common_to_bank_() {
  if (tltubeprofile_bank == NULL) tltubeprofile_bank_init();
  return fdtubeprofile_struct_to_abank_(tltubeprofile, &tltubeprofile_bank, TLTUBEPROFILE_BANKID, TLTUBEPROFILE_BANKVERSION);
}

integer4 tltubeprofile_bank_to_dst_ (integer4 *unit) {
  return fdtubeprofile_abank_to_dst_(tltubeprofile_bank, unit);
}

integer4 tltubeprofile_common_to_dst_(integer4 *unit) {
  if (tltubeprofile_bank == NULL) tltubeprofile_bank_init();
  return fdtubeprofile_struct_to_dst_(tltubeprofile, tltubeprofile_bank, unit, TLTUBEPROFILE_BANKID, TLTUBEPROFILE_BANKVERSION);
}

integer4 tltubeprofile_bank_to_common_(integer1 *bank) {
  return fdtubeprofile_abank_to_struct_(bank, tltubeprofile);
}

integer4 tltubeprofile_common_to_dump_(integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(tltubeprofile, stdout, opt);
}

integer4 tltubeprofile_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(tltubeprofile, fp, opt);
}
