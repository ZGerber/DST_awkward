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
#include "brtubeprofile_dst.h"

brtubeprofile_dst_common brtubeprofile_;
static fdtubeprofile_dst_common* brtubeprofile = &brtubeprofile_;

//static integer4 brtubeprofile_blen;
static integer4 brtubeprofile_maxlen = sizeof(integer4) * 2 + sizeof(brtubeprofile_dst_common);
static integer1 *brtubeprofile_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* brtubeprofile_bank_buffer_ (integer4* brtubeprofile_bank_buffer_size)
{
  (*brtubeprofile_bank_buffer_size) = fdtubeprofile_blen;
  return brtubeprofile_bank;
}



static void brtubeprofile_bank_init() {
  brtubeprofile_bank = (integer1 *)calloc(brtubeprofile_maxlen, sizeof(integer1));
  if (brtubeprofile_bank==NULL) {
      fprintf (stderr,"brtubeprofile_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 brtubeprofile_common_to_bank_() {
  if (brtubeprofile_bank == NULL) brtubeprofile_bank_init();
  return fdtubeprofile_struct_to_abank_(brtubeprofile, &brtubeprofile_bank, BRTUBEPROFILE_BANKID, BRTUBEPROFILE_BANKVERSION);
}

integer4 brtubeprofile_bank_to_dst_ (integer4 *unit) {
  return fdtubeprofile_abank_to_dst_(brtubeprofile_bank, unit);
}

integer4 brtubeprofile_common_to_dst_(integer4 *unit) {
  if (brtubeprofile_bank == NULL) brtubeprofile_bank_init();
  return fdtubeprofile_struct_to_dst_(brtubeprofile, brtubeprofile_bank, unit, BRTUBEPROFILE_BANKID, BRTUBEPROFILE_BANKVERSION);
}

integer4 brtubeprofile_bank_to_common_(integer1 *bank) {
  return fdtubeprofile_abank_to_struct_(bank, brtubeprofile);
}

integer4 brtubeprofile_common_to_dump_(integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(brtubeprofile, stdout, opt);
}

integer4 brtubeprofile_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(brtubeprofile, fp, opt);
}
