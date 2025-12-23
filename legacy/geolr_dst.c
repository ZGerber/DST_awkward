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
#include "geolr_dst.h"

geolr_dst_common geolr_;
static geofd_dst_common* geolr = &geolr_;

//static integer4 geolr_blen;
static integer4 geolr_maxlen = sizeof(integer4) * 2 + sizeof(geolr_dst_common);
static integer1 *geolr_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* geolr_bank_buffer_ (integer4* geolr_bank_buffer_size)
{
  (*geolr_bank_buffer_size) = geofd_blen;
  return geolr_bank;
}



static void geolr_bank_init() {
  geolr_bank = (integer1 *)calloc(geolr_maxlen, sizeof(integer1));
  if (geolr_bank==NULL) {
      fprintf (stderr,"geolr_bank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

integer4 geolr_common_to_bank_() {
  if (geolr_bank == NULL) geolr_bank_init();
  return geofd_struct_to_abank_(geolr, &geolr_bank, GEOLR_BANKID, GEOLR_BANKVERSION);
}

integer4 geolr_bank_to_dst_ (integer4 *unit) {
  return geofd_abank_to_dst_(geolr_bank, unit);
}

integer4 geolr_common_to_dst_(integer4 *unit) {
  if (geolr_bank == NULL) geolr_bank_init();
  return geofd_struct_to_dst_(geolr, geolr_bank, unit, GEOLR_BANKID, GEOLR_BANKVERSION);
}

integer4 geolr_bank_to_common_(integer1 *bank) {
  return geofd_abank_to_struct_(bank, geolr);
}

integer4 geolr_common_to_dump_(integer4 *opt) {
  return geofd_struct_to_dumpf_(geolr, stdout, opt);
}

integer4 geolr_common_to_dumpf_(FILE* fp, integer4 *opt) {
  return geofd_struct_to_dumpf_(geolr, fp, opt);
}
