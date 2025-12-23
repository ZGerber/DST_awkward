// See geofd_dst.h for more information

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "geofd_dst.h"
#include "geobr_dst.h"

geobr_dst_common geobr_;
static geofd_dst_common* geobr = &geobr_;

//static integer4 geobr_blen;
static integer4 geobr_maxlen = sizeof(integer4) * 2 + sizeof(geobr_dst_common);
static integer1 *geobr_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* geobr_bank_buffer_ (integer4* geobr_bank_buffer_size)
{
  (*geobr_bank_buffer_size) = geofd_blen;
  return geobr_bank;
}



static void geobr_bank_init() {
  geobr_bank = (integer1 *)calloc(geobr_maxlen, sizeof(integer1));
  if (geobr_bank==NULL) {
      fprintf (stderr,"geobr_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 geobr_common_to_bank_() {
  if (geobr_bank == NULL) geobr_bank_init();
  return geofd_struct_to_abank_(geobr, &geobr_bank, GEOBR_BANKID, GEOBR_BANKVERSION);
}

integer4 geobr_bank_to_dst_ (integer4 *unit) {
  return geofd_abank_to_dst_(geobr_bank, unit);
}

integer4 geobr_common_to_dst_(integer4 *unit) {
  if (geobr_bank == NULL) geobr_bank_init();
  return geofd_struct_to_dst_(geobr, geobr_bank, unit, GEOBR_BANKID, GEOBR_BANKVERSION);
}

integer4 geobr_bank_to_common_(integer1 *bank) {
  return geofd_abank_to_struct_(bank, geobr);
}

integer4 geobr_common_to_dump_(integer4 *opt) {
  return geofd_struct_to_dumpf_(geobr, stdout, opt);
}

integer4 geobr_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return geofd_struct_to_dumpf_(geobr, fp, opt);
}
