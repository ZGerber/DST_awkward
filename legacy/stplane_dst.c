// Created 2008/09/23 DRB LMS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "stplane_dst.h"
#include "caldat.h"

stplane_dst_common stplane_;

static integer4 stplane_blen = 0;
static integer4 stplane_maxlen = sizeof(integer4) * 2 + sizeof(stplane_dst_common);
static integer1 *stplane_bank = NULL;

/* get (packed) buffer pointer and size */
integer1* stplane_bank_buffer_ (integer4* stplane_bank_buffer_size)
{
  (*stplane_bank_buffer_size) = stplane_blen;
  return stplane_bank;
}



static void stplane_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(stplane_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
      fprintf (stderr,"stplane_abank_init: fail to assign memory to bank. Abort.\n");
      exit(0);
  }
}

static void stplane_bank_init() {stplane_abank_init(&stplane_bank);}

integer4 stplane_common_to_bank_() {
  if (stplane_bank == NULL) stplane_bank_init();
  return stplane_struct_to_abank_(&stplane_, &stplane_bank, STPLANE_BANKID, STPLANE_BANKVERSION);
}
integer4 stplane_bank_to_dst_ (integer4 *unit) {return stplane_abank_to_dst_(stplane_bank, unit);}
integer4 stplane_common_to_dst_(integer4 *unit) {
  return stplane_struct_to_dst_(&stplane_, stplane_bank, unit, STPLANE_BANKID, STPLANE_BANKVERSION);
}
integer4 stplane_bank_to_common_(integer1 *bank) {return stplane_abank_to_struct_(bank, &stplane_);}
integer4 stplane_common_to_dump_(integer4 *opt) {return stplane_struct_to_dumpf_(&stplane_, stdout, opt);}
integer4 stplane_common_to_dumpf_(FILE* fp, integer4 *opt) {return stplane_struct_to_dumpf_(&stplane_, fp, opt);}

integer4 stplane_struct_to_abank_(stplane_dst_common *stplane, integer1 *(*pbank), integer4 id, integer4 ver) {
  int i;
  integer4 rcode, nobj;
  integer1 *bank;

  if (*pbank == NULL) stplane_abank_init(pbank);

  bank = *pbank;
  rcode = dst_initbank_(&id, &ver, &stplane_blen, &stplane_maxlen, bank);

// Initialize stplane_blen and pack the id and version to bank
  nobj = STPLANE_MAXSITES*(STPLANE_MAXSITES-1)/2;
  rcode += dst_packr8_(&stplane->sdp_angle[0],    &nobj, bank, &stplane_blen, &stplane_maxlen);

  nobj = 3;
  rcode += dst_packr8_(&stplane->showerVector[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_packr8_(&stplane->impactPoint[0],  &nobj, bank, &stplane_blen, &stplane_maxlen);

  nobj = 1;
  rcode += dst_packr8_(&stplane->zenith,   &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_packr8_(&stplane->azimuth,  &nobj, bank, &stplane_blen, &stplane_maxlen);

  for (i=0; i<STPLANE_MAXSITES; i++) {
    nobj = 3;
    rcode += dst_packr8_(&stplane->sdp_n[i][0],       &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_packr8_(&stplane->rpuv[i][0],        &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_packr8_(&stplane->shower_axis[i][0], &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_packr8_(&stplane->core[i][0],        &nobj, bank, &stplane_blen, &stplane_maxlen);
  }

  nobj = STPLANE_MAXSITES;
  rcode += dst_packr8_(&stplane->rp[0],         &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_packr8_(&stplane->psi[0],        &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_packr8_(&stplane->shower_zen[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_packr8_(&stplane->shower_azm[0], &nobj, bank, &stplane_blen, &stplane_maxlen);

  rcode += dst_packi4_(&stplane->part[0],       &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_packi4_(&stplane->event_num[0],  &nobj, bank, &stplane_blen, &stplane_maxlen);

  rcode += dst_packi4_(&stplane->sites[0],      &nobj, bank, &stplane_blen, &stplane_maxlen);

  rcode += dst_packi4_(&stplane->juliancore[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  
  rcode += dst_packi4_(&stplane->jseccore[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  
  rcode += dst_packi4_(&stplane->nanocore[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  
  rcode += dst_packr8_(&stplane->track_length[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  
  rcode += dst_packr8_(&stplane->expected_duration[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  
  return rcode;
}

integer4 stplane_abank_to_dst_(integer1 *bank, integer4 *unit) {
  return dst_write_bank_(unit, &stplane_blen, bank);
}

integer4 stplane_struct_to_dst_(stplane_dst_common *stplane, integer1 *bank, integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = stplane_struct_to_abank_(stplane, &bank, id, ver)) ) {
      fprintf(stderr, "stplane_struct_to_abank_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  if ( (rcode = stplane_abank_to_dst_(bank, unit)) ) {
      fprintf(stderr, "stplane_abank_to_dst_ ERROR : %ld\n", (long)rcode);
      exit(0);
  }
  return 0;
}

integer4 stplane_abank_to_struct_(integer1 *bank, stplane_dst_common *stplane) {
  int i;
  int maxsites;
  integer4 rcode = 0 ;
  integer4 nobj;
  integer4 version;
  stplane_blen = sizeof(integer4);   /* skip id */

  nobj = 1;
  rcode += dst_unpacki4_(&version, &nobj, bank, &stplane_blen, &stplane_maxlen);
  
  if (version == 0)
    maxsites = 3;
  else
    maxsites = STPLANE_MAXSITES;
  
  nobj = maxsites*(maxsites-1)/2;
  rcode += dst_unpackr8_(&stplane->sdp_angle[0],    &nobj, bank, &stplane_blen, &stplane_maxlen);

  nobj = 3;
  rcode += dst_unpackr8_(&stplane->showerVector[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_unpackr8_(&stplane->impactPoint[0],  &nobj, bank, &stplane_blen, &stplane_maxlen);

  nobj = 1;
  rcode += dst_unpackr8_(&stplane->zenith,   &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_unpackr8_(&stplane->azimuth,  &nobj, bank, &stplane_blen, &stplane_maxlen);

  for (i=0; i<maxsites; i++) {
    nobj = 3;
    rcode += dst_unpackr8_(&stplane->sdp_n[i][0],       &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_unpackr8_(&stplane->rpuv[i][0],        &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_unpackr8_(&stplane->shower_axis[i][0], &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_unpackr8_(&stplane->core[i][0],        &nobj, bank, &stplane_blen, &stplane_maxlen);
  }

  nobj = maxsites;
  rcode += dst_unpackr8_(&stplane->rp[0],         &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_unpackr8_(&stplane->psi[0],        &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_unpackr8_(&stplane->shower_zen[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_unpackr8_(&stplane->shower_azm[0], &nobj, bank, &stplane_blen, &stplane_maxlen);

  rcode += dst_unpacki4_(&stplane->part[0],       &nobj, bank, &stplane_blen, &stplane_maxlen);
  rcode += dst_unpacki4_(&stplane->event_num[0],  &nobj, bank, &stplane_blen, &stplane_maxlen);

  rcode += dst_unpacki4_(&stplane->sites[0],      &nobj, bank, &stplane_blen, &stplane_maxlen);

  if (version >= 2) {
    rcode += dst_unpacki4_(&stplane->juliancore[0],      &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_unpacki4_(&stplane->jseccore[0],      &nobj, bank, &stplane_blen, &stplane_maxlen);
    rcode += dst_unpacki4_(&stplane->nanocore[0],      &nobj, bank, &stplane_blen, &stplane_maxlen);
  }
  else 
    for (i=0; i<maxsites; i++)
      stplane->juliancore[i] = stplane->jseccore[i] = stplane->nanocore[i] = -1;
    
  if (version >= 3)
    rcode += dst_unpackr8_(&stplane->track_length[0],      &nobj, bank, &stplane_blen, &stplane_maxlen);
  else
    for (i=0; i<maxsites; i++)
      stplane->track_length[i] = 0;
  
  if (version >= 4)
    rcode += dst_unpackr8_(&stplane->expected_duration[0], &nobj, bank, &stplane_blen, &stplane_maxlen);
  else
    for (i=0; i<maxsites; i++)
      stplane->expected_duration[i] = 0;
    
  return rcode;
}

integer4 stplane_struct_to_dump_(stplane_dst_common *stplane, integer4 *long_output) {
  return stplane_struct_to_dumpf_(stplane, stdout, long_output);
}

integer4 stplane_struct_to_dumpf_(stplane_dst_common *stplane, FILE* fp, integer4 *long_output) {
  int site;
  
  integer4 yr,mo,day;
  integer4 hr, min, sec;
  int ymd, hms, nano;
  (void)(long_output);
  fprintf(fp, "STPLANE :\n");
  fprintf(fp, " Impact Point (m): ( %12.3f  %12.3f  %12.3f )\n",
          stplane->impactPoint[0],stplane->impactPoint[1],stplane->impactPoint[2]);
  fprintf(fp, " Shower Vector:    (%7.4f,%7.4f,%7.4f)\n",
          stplane->showerVector[0],stplane->showerVector[1],stplane->showerVector[2]);
  fprintf(fp, "    Zenith: %7.2f\n",acos(-stplane->showerVector[2]) * R2D);
  
  double z=atan2(-stplane->showerVector[1],-stplane->showerVector[0]);
    while (z < 0.)
      z += 2.*M_PI;
  
  fprintf(fp, "   Azimuth: %7.2f\n",z*R2D);
  if (stplane->sites[0] && stplane->sites[1]) 
    fprintf(fp, " BR-LR plane-crossing angle (deg.): %.3f\n",stplane->sdp_angle[0]*R2D);

  if (stplane->sites[0] && stplane->sites[2])
    fprintf(fp, " BR-MD plane-crossing angle (deg.): %.3f\n",stplane->sdp_angle[1]*R2D);

  if (stplane->sites[1] && stplane->sites[2])
    fprintf(fp, " LR-MD plane-crossing angle (deg.): %.3f\n",stplane->sdp_angle[3]*R2D); 

  
  for (site=0; site<STPLANE_MAXSITES; site++) {
    if (!stplane->sites[site])
      continue;
    
    if (stplane->nanocore[site] < 0) {
      ymd = 0;
      hms = 0;
      nano = 0;
    }
    else {
      hr = stplane->jseccore[site] / 3600 + 12;

      if (hr >= 24) {
        caldat((double)stplane->juliancore[site]+1., &mo, &day, &yr);
        hr -= 24;
      }
      else
        caldat((double)stplane->juliancore[site], &mo, &day, &yr);

      min = ( stplane->jseccore[site] / 60 ) % 60;
      sec = stplane->jseccore[site] % 60;
      ymd = 10000*yr + 100*mo + day;
      hms = 10000*hr + 100*min + sec;
      nano = stplane->nanocore[site];
    }
    
    
    fprintf(fp, "\n Geometry information for site %d\n", site);
    fprintf(fp, "Event: part %2d  trigger %6d\n",
            stplane->part[site],stplane->event_num[site]);
    
    fprintf(fp, " Core time (CLF) from T0 fit: %08d %06d.%09d\n",
            ymd,hms,nano);
    
    fprintf(fp, " Core position (site):  ( %12.3f %12.3f %12.3f )\n",
            stplane->core[site][0], 
            stplane->core[site][1],
            stplane->core[site][2]);
    
    fprintf(fp, " Rp unit vector (site):  ( %10.7f %10.7f %10.7f )\n",
            stplane->rpuv[site][0],stplane->rpuv[site][1],
            stplane->rpuv[site][2]);
    fprintf(fp, " magnitude(Rp) %12.3f ; psi %8.3f\n",stplane->rp[site],
            stplane->psi[site]*R2D);
    fprintf(fp, " SDP normal vector (CLF): %.8f %.8f %.8f\n",
            stplane->sdp_n[site][0],
            stplane->sdp_n[site][1],
            stplane->sdp_n[site][2]);
    fprintf(fp, " Track length (deg): %.3f\n",stplane->track_length[site]*R2D);
    fprintf(fp, " Expected duration (ns): %.3f\n",stplane->expected_duration[site]);
  }
//   if ( (*long_output) == 1)
//     fprintf(fp, "This will be filled someday...\n");

  return 0;
}
