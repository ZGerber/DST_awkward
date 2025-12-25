// Created 2010/05/04 ELB

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "fdtubeprofile_dst.h"
#include "hytubeprofile_dst.h"

hytubeprofile_dst_common hytubeprofile_;
static fdtubeprofile_dst_common* hytubeprofile = &hytubeprofile_;

//static integer4 hytubeprofile_blen;
static integer4 hytubeprofile_maxlen = sizeof(integer4) * 2 + sizeof(hytubeprofile_dst_common);
static integer1 *hytubeprofile_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* hytubeprofile_bank_buffer_ (integer4* hytubeprofile_bank_buffer_size)
{
  (*hytubeprofile_bank_buffer_size) = fdtubeprofile_blen;
  return hytubeprofile_bank;
}



static void hytubeprofile_bank_init() {
  hytubeprofile_bank = (integer1 *)calloc(hytubeprofile_maxlen, sizeof(integer1));
  if (hytubeprofile_bank==NULL) {
      fprintf (stderr,"hytubeprofile_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 hytubeprofile_common_to_bank_() {
  if (hytubeprofile_bank == NULL) hytubeprofile_bank_init();
  return fdtubeprofile_struct_to_abank_(hytubeprofile, &hytubeprofile_bank, HYTUBEPROFILE_BANKID, HYTUBEPROFILE_BANKVERSION);
}

integer4 hytubeprofile_bank_to_dst_ (integer4 *unit) {
  return fdtubeprofile_abank_to_dst_(hytubeprofile_bank, unit);
}

integer4 hytubeprofile_common_to_dst_(integer4 *unit) {
  if (hytubeprofile_bank == NULL) hytubeprofile_bank_init();
  return fdtubeprofile_struct_to_dst_(hytubeprofile, hytubeprofile_bank, unit, HYTUBEPROFILE_BANKID, HYTUBEPROFILE_BANKVERSION);
}

integer4 hytubeprofile_bank_to_common_(integer1 *bank) {
  return fdtubeprofile_abank_to_struct_(bank, hytubeprofile);
}

integer4 hytubeprofile_common_to_dump_(integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(hytubeprofile, stdout, opt);
}

integer4 hytubeprofile_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return fdtubeprofile_struct_to_dumpf_(hytubeprofile, fp, opt);
}
