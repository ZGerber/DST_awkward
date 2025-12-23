// Created 2008/09/23 DRB LMS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_err_codes.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "geofd_dst.h"
#include "fdcalib_util.h"
#include "geofd_tokuno_spotsize.h"

geofd_dst_common geofd_;

integer4 geofd_blen = 0; /* not static because it needs to be accessed by the c files of the derived banks */
static integer4 geofd_maxlen = sizeof(integer4) * 2 + sizeof(geofd_dst_common);
static integer1 *geofd_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* geofd_bank_buffer_ (integer4* geofd_bank_buffer_size)
{
  (*geofd_bank_buffer_size) = geofd_blen;
  return geofd_bank;
}



static void geofd_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(geofd_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
      fprintf (stderr,"geofd_abank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

static void geofd_bank_init() {geofd_abank_init(&geofd_bank);}

integer4 geofd_common_to_bank_() {
  if (geofd_bank == NULL) geofd_bank_init();
  return geofd_struct_to_abank_(&geofd_, &geofd_bank, GEOFD_BANKID, GEOFD_BANKVERSION);
}
integer4 geofd_bank_to_dst_ (integer4 *unit) {return geofd_abank_to_dst_(geofd_bank, unit);}
integer4 geofd_common_to_dst_(integer4 *unit) {
  if (geofd_bank == NULL) geofd_bank_init();
  return geofd_struct_to_dst_(&geofd_, geofd_bank, unit, GEOFD_BANKID, GEOFD_BANKVERSION);
}
integer4 geofd_bank_to_common_(integer1 *bank) {return geofd_abank_to_struct_(bank, &geofd_);}
integer4 geofd_common_to_dump_(integer4 *opt) {return geofd_struct_to_dumpf_(&geofd_, stdout, opt);}
integer4 geofd_common_to_dumpf_(FILE* fp, integer4 *opt) {return geofd_struct_to_dumpf_(&geofd_, fp, opt);}

integer4 geofd_struct_to_abank_(geofd_dst_common *geofd, integer1 *(*pbank), integer4 id, integer4 ver) {
  integer4 rcode, nobj, i, j;
  integer1 *bank;

  if (*pbank == NULL) geofd_abank_init(pbank);



// Initialize geofd_blen and pack the id and version to bank

  bank = *pbank;
  rcode = dst_initbank_(&id, &ver, &geofd_blen, &geofd_maxlen, bank);
// new in version 001
  nobj = 1;
  rcode += dst_packi4_(&geofd->uniqID, &nobj, bank, &geofd_blen, &geofd_maxlen);
// end of v001 addition  
  
  // no longer here in version 003
// // new in version 002
//   nobj = 1;
//   rcode += dst_packi4_(&geofd->nmir,  &nobj, bank, &geofd_blen, &geofd_maxlen);
//   
//   nobj = 1;
//   rcode += dst_packi4_(&geofd->nseg,  &nobj, bank, &geofd_blen, &geofd_maxlen);
// 
//   nobj = geofd->nmir;
//   rcode += dst_packi4_(&geofd->camtype[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
// // end of v002 addition
  // back to v001 compatibility
  nobj = 1;
  rcode += dst_packr8_(&geofd->latitude,  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->longitude, &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->altitude,  &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  rcode += dst_packr8_(&geofd->vclf[0],   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->vsite[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->local_vsite[0],       &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_packr8_(&geofd->site2earth[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_packr8_(&geofd->site2clf[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<12; i++)
    rcode += dst_packr8_(&geofd->local_vmir[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  // removed in v002, restored in v003
  nobj = 3;
  for (i=0; i<12; i++)
    rcode += dst_packr8_(&geofd->local_vcam[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  // back to v001 layout
  nobj = 3;
  for (i=0; i<12; i++)
    rcode += dst_packr8_(&geofd->vmir[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 12;
  rcode += dst_packr8_(&geofd->mir_lat[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->mir_lon[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->mir_alt[0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  rcode += dst_packr8_(&geofd->mir_the[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->mir_phi[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->rcurve[0],   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->sep[0],      &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<12; i++)
    for (j=0; j<3; j++)
      rcode += dst_packr8_(&geofd->site2cam[i][j][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = GEOFD_MIRTUBE;
  rcode += dst_packr8_(&geofd->xtube[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->ytube[0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<12; i++)
    for (j=0; j<GEOFD_MIRTUBE; j++)
      rcode += dst_packr8_(&geofd->vtube[i][j][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<18; i++)
    rcode += dst_packr8_(&geofd->vseg[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);


  nobj = 1;
  rcode += dst_packr8_(&geofd->diameter,   &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  nobj = 1;
  rcode += dst_packr8_(&geofd->cam_width,  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->cam_height, &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->cam_depth,  &nobj, bank, &geofd_blen, &geofd_maxlen);

  rcode += dst_packr8_(&geofd->pmt_flat2flat,   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->pmt_point2point, &nobj, bank, &geofd_blen, &geofd_maxlen);

  rcode += dst_packr8_(&geofd->seg_flat2flat,   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_packr8_(&geofd->seg_point2point, &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 12;
  rcode += dst_packi4_(&geofd->ring[0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 1;
  rcode += dst_packi4_(&geofd->siteid, &nobj, bank, &geofd_blen, &geofd_maxlen);

  // Here begins version 3 extension.
  nobj = 1;
  rcode += dst_packi4_(&geofd->nmir, &nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  nobj = geofd->nmir;
  rcode += dst_packi4_(&geofd->nseg[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  rcode += dst_packi4_(&geofd->ring3[0], &nobj, bank, &geofd_blen,          
                         &geofd_maxlen);
  rcode += dst_packr8_(&geofd->diameters[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    rcode += dst_packr8_(&geofd->local_vmir3[i][0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  for (i=0; i<geofd->nmir; i++)
    rcode += dst_packr8_(&geofd->local_vcam3[i][0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  for (i=0; i<geofd->nmir; i++)
    rcode += dst_packr8_(&geofd->vmir3[i][0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  
  nobj = geofd->nmir;
  
  rcode += dst_packr8_(&geofd->mir_lat3[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  rcode += dst_packr8_(&geofd->mir_lon3[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  rcode += dst_packr8_(&geofd->mir_alt3[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  rcode += dst_packr8_(&geofd->mir_the3[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  rcode += dst_packr8_(&geofd->mir_phi3[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  rcode += dst_packr8_(&geofd->rcurve3[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  rcode += dst_packr8_(&geofd->sep3[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<3; j++)
      rcode += dst_packr8_(&geofd->site2cam3[i][j][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<GEOFD_MIRTUBE; j++)
      rcode += dst_packr8_(&geofd->vtube3[i][j][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
    
  nobj = geofd->nmir;
  rcode += dst_packi4_(&geofd->camtype[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<geofd->nseg[i]; j++)
      rcode += dst_packr8_(&geofd->vseg3[i][j][0],&nobj, bank, 
                           &geofd_blen,&geofd_maxlen);
  
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<geofd->nseg[i]; j++)
      rcode += dst_packr8_(&geofd->seg_center[i][j][0],&nobj, bank, 
                           &geofd_blen,&geofd_maxlen);
  
//   nobj = 3;
//   for (i=0; i<geofd->nmir; i++)
//     for (j=0; j<geofd->nseg[i]; j++)
//       rcode += dst_packr8_(&geofd->seg_axis[i][j][0],&nobj, bank, 
//                            &geofd_blen,&geofd_maxlen);
  nobj = geofd->nmir;
  rcode += dst_packr8_(&geofd->rotation[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  
  nobj = 1;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<geofd->nseg[i]; j++) {
      rcode += dst_packr8_(&geofd->seg_rcurve[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_packr8_(&geofd->seg_spot[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_packr8_(&geofd->seg_orient[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_packr8_(&geofd->seg_rcurvex[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_packr8_(&geofd->seg_rcurvey[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_packr8_(&geofd->seg_spotx[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_packr8_(&geofd->seg_spoty[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);      
    }
  
  
  return rcode;
}

integer4 geofd_abank_to_dst_(integer1 *bank, integer4 *unit) {
  return dst_write_bank_(unit, &geofd_blen, bank);
}

integer4 geofd_struct_to_dst_(geofd_dst_common *geofd, integer1 *bank, integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = geofd_struct_to_abank_(geofd, &bank, id, ver)) ) {
      fprintf(stderr, "geofd_struct_to_abank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  if ( (rcode = geofd_abank_to_dst_(bank, unit)) ) {
      fprintf(stderr, "geofd_abank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  return 0;
}

integer4 geofd_abank_to_struct_(integer1 *bank, geofd_dst_common *geofd) {
  integer4 rcode = 0 ;
  integer4 nobj, i, j, k;
//   geofd_blen = 2 * sizeof(integer4);   /* skip id and version  */
  integer4 version, maxmir;
  real8 local_vcam[12][3];
  real8 dummy[3];
  
  geofd_blen = sizeof(integer4); /* skip ID */
  nobj = 1;
  rcode += dst_unpacki4_(&version, &nobj, bank, &geofd_blen, &geofd_maxlen);

  geofd->uniqID = 0;

  if (version >= 1) {
    nobj = 1;
    rcode += dst_unpacki4_(&geofd->uniqID, &nobj, bank, &geofd_blen, &geofd_maxlen); 
  }
  switch (version) {
    case 0:
    case 1:
      fprintf(stderr,"%s (%d): Warning: old GEOFD bank version (%03d)\n",__FILE__,__LINE__,version);
      maxmir = 12; 

      break;
    case 2:
      maxmir = GEOFD_MAXMIR;
      nobj = 1;
      rcode += dst_unpacki4_(&geofd->nmir,  &nobj, bank, &geofd_blen, &geofd_maxlen);      
      rcode += dst_unpacki4_(&geofd->nseg[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
      for (i=1; i<geofd->nmir; i++)
        geofd->nseg[i] = geofd->nseg[0];
      nobj = geofd->nmir;
      rcode += dst_unpacki4_(&geofd->camtype[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);

      break;
    case 3:
    default:
      maxmir = 12; // for now; will change it farther down
      


  
  }
  nobj = 1;
  rcode += dst_unpackr8_(&geofd->latitude,  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->longitude, &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->altitude,  &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  rcode += dst_unpackr8_(&geofd->vclf[0],   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->vsite[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->local_vsite[0],       &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_unpackr8_(&geofd->site2earth[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<3; i++)
    rcode += dst_unpackr8_(&geofd->site2clf[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<12; i++)
    rcode += dst_unpackr8_(&geofd->local_vmir[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  if (version == 2) {
    for (i=12; i<maxmir; i++)
      rcode += dst_unpackr8_(&dummy[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  }
  if (version != 2) {
    nobj = 3;
    for (i=0; i<maxmir; i++)
      rcode += dst_unpackr8_(&local_vcam[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  }
  
  nobj = 3;
  for (i=0; i<12; i++)
    rcode += dst_unpackr8_(&geofd->vmir[i][0],       &nobj, bank, &geofd_blen, &geofd_maxlen);

  if (version == 2) {

    for (i=12; i<maxmir; i++)
      rcode += dst_unpackr8_(&dummy[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
    
    
    nobj = geofd->nmir;
  }
  else {
    nobj = maxmir;
  }
//   nobj = GEOFD_MAXMIR;
  rcode += dst_unpackr8_(&geofd->mir_lat[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->mir_lon[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->mir_alt[0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  rcode += dst_unpackr8_(&geofd->mir_the[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->mir_phi[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->rcurve[0],   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->sep[0],      &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 3;
  for (i=0; i<12; i++)
    for (j=0; j<3; j++)
      rcode += dst_unpackr8_(&geofd->site2cam[i][j][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
    
  if (version == 2) {
    for (i=12; i<maxmir; i++)
      for (j=0; j<3; j++)
        rcode += dst_unpackr8_(&dummy[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  }

  nobj = GEOFD_MIRTUBE;
  rcode += dst_unpackr8_(&geofd->xtube[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->ytube[0], &nobj, bank, &geofd_blen, &geofd_maxlen);

//   if (version <= 1)
//     for (i=0; i<GEOFD_MIRTUBE; i++)
//       geofd->ytube[i] *= -1.0;
  
  nobj = 3;
  for (i=0; i<12; i++)
    for (j=0; j<GEOFD_MIRTUBE; j++)
      rcode += dst_unpackr8_(&geofd->vtube[i][j][0], &nobj, bank, &geofd_blen, &geofd_maxlen);

  if (version == 2) {
    for (i=12; i<maxmir; i++)
      for (j=0; j<GEOFD_MIRTUBE; j++)
        rcode += dst_unpackr8_(&dummy[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  }
  nobj = 3;
  if (version == 2) {
    for (j=0; j<12; j++) {
      for (i=0; i<18; i++)
        rcode += dst_unpackr8_(&geofd->vseg3[j][i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
    }
    for (j=12; j<maxmir; j++) {
      for (i=0; i<18; i++)
        rcode += dst_unpackr8_(&dummy[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
    }
    for (i=0; i<geofd->nseg[0]; i++)
      for (j=0; j<3; j++)
        geofd->vseg[i][j] = geofd->vseg3[0][i][j];
  }
  else {
    for (i=0; i<18; i++)
        rcode += dst_unpackr8_(&geofd->vseg[i][0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  }

  if (version == 2) {
    nobj = geofd->nmir;
    rcode += dst_unpackr8_(&geofd->diameters[0], &nobj, bank, &geofd_blen,
                           &geofd_maxlen);
    geofd->diameter = geofd->diameters[0];
  }
  else {
    nobj = 1;
    rcode += dst_unpackr8_(&geofd->diameter,   &nobj, bank, &geofd_blen, &geofd_maxlen);
  }
  
  nobj = 1;
  rcode += dst_unpackr8_(&geofd->cam_width,  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->cam_height, &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->cam_depth,  &nobj, bank, &geofd_blen, &geofd_maxlen);

  rcode += dst_unpackr8_(&geofd->pmt_flat2flat,   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->pmt_point2point, &nobj, bank, &geofd_blen, &geofd_maxlen);

  rcode += dst_unpackr8_(&geofd->seg_flat2flat,   &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->seg_point2point, &nobj, bank, &geofd_blen, &geofd_maxlen);

  nobj = 12;
  rcode += dst_unpacki4_(&geofd->ring[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  if (version==2) {
    nobj = 1;
    for (i=12; i<maxmir; i++)
      rcode += dst_unpacki4_(&j, &nobj, bank, &geofd_blen, &geofd_maxlen);
  }
  nobj = 1;
  rcode += dst_unpacki4_(&geofd->siteid, &nobj, bank, &geofd_blen, &geofd_maxlen);

  // now for version-3 extensions
  if(version <= 2)
    {
      if(version <= 1)
	{
	  geofd->nmir = 12;
	  for (i=0; i<geofd->nmir; i++) 
	    {
	      geofd->nseg[i] = 18;
	      geofd->camtype[i] = GEOFD_TA;
	      geofd->diameters[i] = geofd->diameter;
	    }
	  for (i=0; i<geofd->nmir; i++)
	    for (j=0; j<geofd->nseg[i]; j++)
	      for (k=0; k<3; k++)
		geofd->vseg3[i][j][k] = geofd->vseg[j][k];
	}
      for (i=0; i<geofd->nmir; i++) 
	{
	  geofd->ring3[i] = geofd->ring[i];
	  geofd->mir_lat3[i] = geofd->mir_lat[i];
	  geofd->mir_lon3[i] = geofd->mir_lon[i];
	  geofd->mir_alt3[i] = geofd->mir_alt[i];
	  geofd->mir_the3[i] = geofd->mir_the[i];
	  geofd->mir_phi3[i] = geofd->mir_phi[i];
	  geofd->rcurve3[i] = geofd->rcurve[i];
	  geofd->sep3[i] = geofd->sep[i];
	  for (j=0; j<3; j++) 
	    {
	      geofd->local_vmir3[i][j] = geofd->local_vmir[i][j];
	      geofd->vmir3[i][j] = geofd->vmir[i][j];
	      for (k=0; k<3; k++)
		geofd->site2cam3[i][j][k] = geofd->site2cam[i][j][k];
	    }
	  for (j=0; j<GEOFD_MIRTUBE; j++)
	    for (k=0; k<3; k++)
	      geofd->vtube3[i][j][k] = geofd->vtube[i][j][k];
	  for (j=0; j<geofd->nseg[i]; j++) {
	    for (k=0; k<3; k++) {
	      
	      geofd->seg_center[i][j][k] = 0; // change this!
	      //             geofd->seg_axis[i][j][k] = (k==2)?1:0; // this too!
	    }
	    geofd->seg_rcurve[i][j] = geofd->rcurve3[i];
	    switch (geofd->uniqID) {
	    case GEOFD_UNIQBRLRSCOTT:        
	      geofd->seg_spot[i][j] = 0.195; // deflection in deg at 
	      break;
	    case GEOFD_UNIQBRLRTOKUNO:
	    case GEOFD_UNIQBRSTANSSP:
	    case GEOFD_UNIQLRSTANSSP:
	    case GEOFD_UNIQBRLRSSPTHOMAS:
	      geofd->seg_spot[i][j] = geofd_tokuno_ssp[geofd->siteid][i];
	      break;
	    case GEOFD_UNIQBRSTAN:
	    case GEOFD_UNIQLRSTAN:
	    case GEOFD_UNIQBRLRTHOMAS:
	      geofd->seg_spot[i][j]  = 0.0285;
	      break;
	    default:
	      geofd->seg_spot[i][j]  =  0.0285;
	    }
	    geofd->seg_orient[i][j] = 0;
	    geofd->seg_rcurvex[i][j] = geofd->rcurve3[i];
	    geofd->seg_rcurvey[i][j] = geofd->rcurve3[i];
	  }
	}
      return rcode;
    }
  
  
  nobj = 1;
  rcode += dst_unpacki4_(&geofd->nmir, &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  nobj = geofd->nmir;
  rcode += dst_unpacki4_(&geofd->nseg[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpacki4_(&geofd->ring3[0], &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->diameters[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    rcode += dst_unpackr8_(&geofd->local_vmir3[i][0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  for (i=0; i<geofd->nmir; i++)
    rcode += dst_unpackr8_(&geofd->local_vcam3[i][0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  for (i=0; i<geofd->nmir; i++)
    rcode += dst_unpackr8_(&geofd->vmir3[i][0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
  
  nobj = geofd->nmir;
  
  rcode += dst_unpackr8_(&geofd->mir_lat3[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->mir_lon3[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->mir_alt3[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  rcode += dst_unpackr8_(&geofd->mir_the3[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->mir_phi3[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  rcode += dst_unpackr8_(&geofd->rcurve3[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  rcode += dst_unpackr8_(&geofd->sep3[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  nobj = 3;
  for (i=0; i<geofd->nmir; i++) 
    for (j=0; j<3; j++)
      rcode += dst_unpackr8_(&geofd->site2cam3[i][j][0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<GEOFD_MIRTUBE; j++)
      rcode += dst_unpackr8_(&geofd->vtube3[i][j][0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  nobj = geofd->nmir;
  rcode += dst_unpacki4_(&geofd->camtype[0],  &nobj, bank, &geofd_blen, &geofd_maxlen);
  
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<geofd->nseg[i]; j++)
      rcode += dst_unpackr8_(&geofd->vseg3[i][j][0],&nobj, bank, 
                           &geofd_blen,&geofd_maxlen);
  
  nobj = 3;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<geofd->nseg[i]; j++)
      rcode += dst_unpackr8_(&geofd->seg_center[i][j][0],&nobj, bank, 
                           &geofd_blen,&geofd_maxlen);
  
//   nobj = 3;
//   for (i=0; i<geofd->nmir; i++)
//     for (j=0; j<geofd->nseg[i]; j++)
//       rcode += dst_unpackr8_(&geofd->seg_axis[i][j][0],&nobj, bank, 
//                            &geofd_blen,&geofd_maxlen);
  nobj = geofd->nmir;
  rcode += dst_unpackr8_(&geofd->rotation[0],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);  
  
    nobj = 1;
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<geofd->nseg[i]; j++) {
      rcode += dst_unpackr8_(&geofd->seg_rcurve[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_unpackr8_(&geofd->seg_spot[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_unpackr8_(&geofd->seg_orient[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_unpackr8_(&geofd->seg_rcurvex[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_unpackr8_(&geofd->seg_rcurvey[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_unpackr8_(&geofd->seg_spotx[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);
      rcode += dst_unpackr8_(&geofd->seg_spoty[i][j],&nobj, bank, &geofd_blen,
                       &geofd_maxlen);      
    }
  return rcode;
}

integer4 geofd_struct_to_dump_(geofd_dst_common *geofd, integer4 *long_output) {
  return geofd_struct_to_dumpf_(geofd, stdout, long_output);
}

integer4 geofd_struct_to_dumpf_(geofd_dst_common *geofd, FILE* fp, integer4 *long_output) {
  int i, j;
  integer4 numseg, numcam;

  (void)(long_output);
  numseg = 18;
  numcam = 12;
  
//   if (geofd->siteid == BR)
//     fprintf (fp, "\nGEOBR bank (geometry information for Black Rock Mesa FD)\n\n");
//   else if (geofd->siteid == LR)
//     fprintf (fp, "\nGEOLR bank (geometry information for Long Ridge FD)\n\n");
  switch (geofd->siteid) {
    case BR:
      fprintf (fp, "\nGEOBR bank (geometry information for Black Rock Mesa FD)\n\n");
      break;
    case LR:
      fprintf (fp, "\nGEOLR bank (geometry information for Long Ridge FD)\n\n");
      break;
    case MD:
      fprintf (fp, "\nGEOMD bank (geometry information for Middle Drum FD)\n\n");
      break;
    case TL:
      fprintf (fp, "\nGEOTL bank (geometry information for TALE FD)\n\n");
      break;
    default:
      fprintf (fp, "\nGEOFD bank (geometry information for unknown/generic FD)\n\n");
  }

  fprintf(fp, "uniq ID: %d",geofd->uniqID);
  if (geofd->uniqID != 0) {
    char dateLine[32];
    convertSec2DateLine(abs(geofd->uniqID),dateLine);
    fprintf(fp," (%s UTC)",dateLine);
  }
  fprintf(fp,"\n\n");
    
  fprintf (fp, "Central Laser Facility lat, lon, alt      : %12.8f %12.8f %12.8f\n",
    CLF_LATITUDE*R2D, CLF_LONGITUDE*R2D, CLF_ALTITUDE);
  fprintf (fp, "Site origin lat, lon, WGS84alt                 : %12.8f %12.8f %12.8f\n\n",
    geofd->latitude*R2D, geofd->longitude*R2D, geofd->altitude);

  fprintf (fp, "CLF location (rel. to center of earth in meters)         : %9.1f %9.1f %9.1f\n",
    geofd->vclf[0], geofd->vclf[1], geofd->vclf[2]);
  fprintf (fp, "Site origin location (rel. to center of earth in meters) : %9.1f %9.1f %9.1f\n",
    geofd->vsite[0], geofd->vsite[1], geofd->vsite[2]);
  fprintf (fp, "Position of site relative to CLF (meters)                : %9.3f %9.3f %9.3f\n\n",
    geofd->local_vsite[0], geofd->local_vsite[1], geofd->local_vsite[2]);

  fprintf (fp, "Site to Earth rotation matrix\n");
  for (i=0; i<3; i++)
    fprintf(fp, "  %9.6f %9.6f %9.6f\n",
      geofd->site2earth[i][0], geofd->site2earth[i][1], geofd->site2earth[i][2]);
  fprintf (fp, "\n");

  fprintf (fp, "Site to CLF rotation matrix\n");
  for (i=0; i<3; i++)
    fprintf(fp, "  %9.6f %9.6f %9.6f\n",
      geofd->site2clf[i][0], geofd->site2clf[i][1], geofd->site2clf[i][2]);
  fprintf (fp, "\n");

  fprintf (fp, "Camera dimensions (meters)                      : %8.4f %8.4f %8.4f\n",
    geofd->cam_width, geofd->cam_height, geofd->cam_depth);
  fprintf (fp, "PMT flat-to-flat distance (meters)              : %8.4f\n", geofd->pmt_flat2flat);
  fprintf (fp, "PMT point-to-point distance (meters)            : %8.4f\n", geofd->pmt_point2point);
  fprintf (fp, "Mirror segment flat-to-flat distance (meters)   : %8.4f\n", geofd->seg_flat2flat);
  fprintf (fp, "Mirror segment point-to-point distance (meters) : %8.4f\n", geofd->seg_point2point);
  fprintf (fp, "Mirror diameter (meters)                        : %8.4f\n\n", geofd->diameter);
  
  


  fprintf (fp, "Unit vectors to mirror segments (from center of curvature, z-axis along mirror axis)\n");
 
  for (i=0; i<numseg; i++) {
    fprintf (fp, "  Segment %2d : %9.6f %9.6f %9.6f\n",
      i, geofd->vseg[i][0], geofd->vseg[i][1], geofd->vseg[i][2]);
  }

  fprintf (fp, "\nMirror locations relative to site origin (meters)\n");
  for (i=0; i<numcam; i++)
    fprintf (fp, "  Mirror %2d : %8.4f %8.4f %8.4f\n",
      i, geofd->local_vmir[i][0], geofd->local_vmir[i][1], geofd->local_vmir[i][2]);
  
//   if (version == 0) {
  fprintf (fp, "\nCamera locations relative to site origin: not used\n");
  for (i=0; i<numcam; i++)
    fprintf (fp, "  Mirror %2d : %8.4f %8.4f %8.4f\n",
      i, geofd->local_vcam[i][0], geofd->local_vcam[i][1], geofd->local_vcam[i][2]);
//   }

  fprintf (fp, "\nMirror locations (lat, lon, alt)\n");
  for (i=0; i<numcam; i++)
    fprintf (fp, "  Mirror %2d : %12.8f %12.8f %8.4f\n",
      i, geofd->mir_lat[i]*R2D, geofd->mir_lon[i]*R2D, geofd->mir_alt[i]);      
      
  fprintf (fp, "\nCamera radii of curvature, mirror-camera separation\n");
  for (i=0; i<numcam; i++)
    fprintf (fp, "  Mirror %2d : R %8.4f S %8.4f\n",
      i, geofd->rcurve[i], geofd->sep[i]);

  fprintf (fp, "\nMirror pointing directions (site coordinate system)\n");
  for (i=0; i<numcam; i++)
    fprintf (fp, "  Mirror %2d : %12.9f %12.9f %12.9f [ zen %9.6f azm %11.6f : ring %d]\n",
      i, geofd->vmir[i][0], geofd->vmir[i][1], geofd->vmir[i][2], 
         geofd->mir_the[i]*R2D, geofd->mir_phi[i]*R2D, geofd->ring[i]);

  fprintf (fp, "\nSite to Camera rotation matrices\n");
  for (i=0; i<numcam; i++) {
    fprintf (fp, "  Site to Camera %2d :\n", i);
    for (j=0; j<3; j++)
      fprintf (fp, "    %9.6f %9.6f %9.6f\n",
        geofd->site2cam[i][j][0], geofd->site2cam[i][j][1], geofd->site2cam[i][j][2]);
    fprintf (fp, "\n");
  }

  fprintf (fp, "\nTube locations on camera / pointing directions\n");
  for (i=0; i<numcam; i++)
    for (j=0; j<GEOFD_MIRTUBE; j++)
      fprintf (fp, "  Camera %2d Tube %3d : x %8.4f y %8.4f   %9.6f %9.6f %9.6f\n",
        i, j, geofd->xtube[j], geofd->ytube[j], geofd->vtube[i][j][0], geofd->vtube[i][j][1], geofd->vtube[i][j][2]);

  fprintf (fp, "\n\n");

  fprintf(fp,"Now for GEOFD v3 extensions.\n\n");
  fprintf(fp,"Number of mirrors: %d\n",geofd->nmir);
  fprintf(fp,"Camera type, diameter (m), mir-cam separation, number of segments per mirror:\n");
  for (i=0; i<geofd->nmir; i++)
    fprintf(fp,"  Camera %2d : %d %f %f %d\n",i,geofd->camtype[i],geofd->diameters[i],geofd->sep3[i],geofd->nseg[i]);
  fprintf(fp,"\n\n");

  fprintf(fp,"Mirror GPS lat/lon/alt(WGS84 m) (axis+sphere intersection):\n");
  for (i=0; i<geofd->nmir; i++)
    fprintf(fp,"  Mirror %2d : %12.8f %12.8f %8.3f\n",i,geofd->mir_lat3[i]*R2D, geofd->mir_lon3[i]*R2D, geofd->mir_alt3[i]);
  fprintf(fp,"\n\n");
  
  fprintf(fp,"Mirror locations relative to site origin (m):\n");
  for (i=0; i<geofd->nmir; i++)
    fprintf (fp, "  Mirror %2d : %8.4f %8.4f %8.4f\n",
      i, geofd->local_vmir3[i][0], geofd->local_vmir3[i][1], geofd->local_vmir3[i][2]);
  
  fprintf (fp, "\nCamera locations relative to site origin: not used\n");
  for (i=0; i<geofd->nmir; i++)
    fprintf (fp, "  Mirror %2d : %8.4f %8.4f %8.4f\n",
      i, geofd->local_vcam3[i][0], geofd->local_vcam3[i][1], geofd->local_vcam3[i][2]);
    
  fprintf(fp, "\nMirror segments: curvature center offsets (x y z), radius, spot size (deg):\n");
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<geofd->nseg[i]; j++)
      fprintf(fp,"  M %2d S %2d : %f %f %f %f %f\n",i,j,geofd->seg_center[i][j][0],
              geofd->seg_center[i][j][1],geofd->seg_center[i][j][2],
              geofd->seg_rcurve[i][j],geofd->seg_spot[i][j]);
  fprintf(fp,"\n\n");
    fprintf (fp, "\nTube3 locations on camera / pointing directions\n");
  for (i=0; i<geofd->nmir; i++)
    for (j=0; j<GEOFD_MIRTUBE; j++)
      fprintf (fp, "  Camera %2d Tube %3d : x %8.4f y %8.4f   %9.6f %9.6f %9.6f (elev %.2f  az %.2f ccwe)\n",
        i, j, geofd->xtube[j], geofd->ytube[j], geofd->vtube3[i][j][0], geofd->vtube3[i][j][1], geofd->vtube3[i][j][2],asin(geofd->vtube3[i][j][2])*R2D,atan2(geofd->vtube3[i][j][1],geofd->vtube3[i][j][0])*R2D);

  fprintf (fp, "\n\n");
  fprintf (fp, "Further quantities defined in GEOFD version 3 will be added here soon.\n");
  fprintf (fp, "\n\n");
  return SUCCESS;
}

