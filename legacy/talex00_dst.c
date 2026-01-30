
// Added 2010/11/22, DI 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "talex00_dst.h"
#include "rusdraw_dst.h"

talex00_dst_common talex00_;

static integer4 talex00_blen;
static integer4 talex00_maxlen = sizeof(integer4) * 2 + sizeof(talex00_dst_common);
static integer1 *talex00_bank = NULL;


// to produce lists of towers that are present in a bit flag
static char talex00_tower_list[sizeof(TALEX00_TOWER_NAME)];


integer1* talex00_bank_buffer_ (integer4* talex00_bank_buffer_size)
{
  (*talex00_bank_buffer_size) = talex00_blen;
  return talex00_bank;
}



static void talex00_abank_init(integer1* (*pbank) ) {
  *pbank = (integer1 *)calloc(talex00_maxlen, sizeof(integer1));
  if (*pbank==NULL) {
    fprintf (stderr,"talex00_abank_init: fail to assign memory to bank. Abort.\n");
    exit(0);
  }
}

static void talex00_bank_init() {talex00_abank_init(&talex00_bank);}

integer4 talex00_common_to_bank_() {
  if (talex00_bank == NULL) talex00_bank_init();
  return talex00_struct_to_abank_(&talex00_, &talex00_bank, TALEX00_BANKID, TALEX00_BANKVERSION);
}
integer4 talex00_bank_to_dst_ (integer4 *unit) {return talex00_abank_to_dst_(talex00_bank, unit);}
integer4 talex00_common_to_dst_(integer4 *unit) {
  if (talex00_bank == NULL) talex00_bank_init();
  return talex00_struct_to_dst_(&talex00_, talex00_bank, unit, TALEX00_BANKID, TALEX00_BANKVERSION);
}
integer4 talex00_bank_to_common_(integer1 *bank) {return talex00_abank_to_struct_(bank, &talex00_);}
integer4 talex00_common_to_dump_(integer4 *opt) {return talex00_struct_to_dumpf_(&talex00_, stdout, opt);}
integer4 talex00_common_to_dumpf_(FILE* fp, integer4 *opt) {return talex00_struct_to_dumpf_(&talex00_, fp, opt);}

integer4 talex00_struct_to_abank_(talex00_dst_common *talex00, integer1 *(*pbank), integer4 id, integer4 ver) {
  integer4 rcode=0;
  integer4 nobj, i, j;
  integer1 *bank;
  
  if (*pbank == NULL) talex00_abank_init(pbank);
  bank = *pbank;
  rcode = dst_initbank_(&id, &ver, &talex00_blen, &talex00_maxlen, bank);  
  
  nobj = 1;
  rcode += dst_packi4_(&talex00->event_num, &nobj, bank, &talex00_blen,&talex00_maxlen); 
  rcode += dst_packi4_(&talex00->event_code, &nobj, bank, &talex00_blen,&talex00_maxlen); 
  rcode += dst_packi4_(&talex00->site, &nobj, bank, &talex00_blen,&talex00_maxlen);
  
  nobj=TALEX00_NCT;
  rcode += dst_packi4_(&talex00->run_id[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->trig_id[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  
  nobj=1;
  rcode += dst_packi4_(&talex00->errcode, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->yymmdd, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->hhmmss, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->usec, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->monyymmdd, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->monhhmmss, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->level2_trig_code, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->nofwf, &nobj, bank, &talex00_blen,&talex00_maxlen);
  
  nobj = talex00->nofwf;
  rcode += dst_packi4_(&talex00->nretry[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->wf_id[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->trig_code[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->xxyy[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_packi4_(&talex00->clkcnt[0], &nobj,bank, &talex00_blen, &talex00_maxlen);
  rcode += dst_packi4_(&talex00->mclkcnt[0], &nobj,bank, &talex00_blen, &talex00_maxlen);

  for(i = 0;  i< talex00->nofwf; i++)
    {
      
      nobj = 2;
      rcode += dst_packi4_(&talex00->fadcti[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packi4_(&talex00->fadcav[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      
      nobj = 128;
      for (j = 0; j < 2; j++)
	rcode += dst_packi4_(&talex00->fadc[i][j][0], &nobj, bank,&talex00_blen, &talex00_maxlen);
      
      nobj=2;
      rcode += dst_packi4_(&talex00->pchmip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packi4_(&talex00->pchped[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packi4_(&talex00->lhpchmip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packi4_(&talex00->lhpchped[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packi4_(&talex00->rhpchmip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packi4_(&talex00->rhpchped[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packi4_(&talex00->mftndof[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packr8_(&talex00->mip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packr8_(&talex00->mftchi2[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      
      nobj=4;
      for(j=0;j<2;j++)
	{
	  rcode += dst_packr8_(&talex00->mftp[i][j][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
	  rcode += dst_packr8_(&talex00->mftpe[i][j][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
	}
      
      nobj=3;
      rcode += dst_packr8_(&talex00->lat_lon_alt[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_packr8_(&talex00->xyz_cor_clf[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      
    }
  
  return rcode;
}

integer4 talex00_abank_to_dst_(integer1 *bank, integer4 *unit) {
  return dst_write_bank_(unit, &talex00_blen, bank);
}

integer4 talex00_struct_to_dst_(talex00_dst_common *talex00, integer1 *bank, integer4 *unit, integer4 id, integer4 ver) {
  integer4 rcode;
  if ( (rcode = talex00_struct_to_abank_(talex00, &bank, id, ver)) ) {
    fprintf(stderr, "talex00_struct_to_abank_ ERROR : %ld\n", (long)rcode);
    exit(0);
  }
  if ( (rcode = talex00_abank_to_dst_(bank, unit)) ) {
    fprintf(stderr, "talex00_abank_to_dst_ ERROR : %ld\n", (long)rcode);
    exit(0);
  }
  return 0;
}

integer4 talex00_abank_to_struct_(integer1 *bank, talex00_dst_common *talex00) {
  
  integer4 i = 0, j = 0;
  integer4 rcode  = 0;
  integer4 nobj   = 0;
  integer4 bankid = 0, bankversion = 0;
  
  talex00_blen = 0; /* do not want to skip id and version */

  nobj = 1;
  
  // unpack the id and version
  rcode += dst_unpacki4_(&bankid,&nobj,bank,&talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&bankversion,&nobj,bank,&talex00_blen,&talex00_maxlen);
  
  
  nobj = 1;
  rcode += dst_unpacki4_(&talex00->event_num, &nobj, bank, &talex00_blen,&talex00_maxlen); 
  rcode += dst_unpacki4_(&talex00->event_code, &nobj, bank, &talex00_blen,&talex00_maxlen); 
  rcode += dst_unpacki4_(&talex00->site, &nobj, bank, &talex00_blen,&talex00_maxlen);
  
  nobj= (bankversion >= 1 ? TALEX00_NCT : 3);
  rcode += dst_unpacki4_(&talex00->run_id[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->trig_id[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  
  nobj=1;
  rcode += dst_unpacki4_(&talex00->errcode, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->yymmdd, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->hhmmss, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->usec, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->monyymmdd, &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->monhhmmss, &nobj, bank, &talex00_blen,&talex00_maxlen);
  if (bankversion >= 2) {
    rcode += dst_unpacki4_(&talex00->level2_trig_code, &nobj, bank, &talex00_blen,&talex00_maxlen);
  }
  rcode += dst_unpacki4_(&talex00->nofwf, &nobj, bank, &talex00_blen,&talex00_maxlen);
  
  nobj = talex00->nofwf;
  rcode += dst_unpacki4_(&talex00->nretry[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->wf_id[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->trig_code[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->xxyy[0], &nobj, bank, &talex00_blen,&talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->clkcnt[0], &nobj,bank, &talex00_blen, &talex00_maxlen);
  rcode += dst_unpacki4_(&talex00->mclkcnt[0], &nobj,bank, &talex00_blen, &talex00_maxlen);

  for(i = 0;  i< talex00->nofwf; i++)
    {
      
      nobj = 2;
      rcode += dst_unpacki4_(&talex00->fadcti[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpacki4_(&talex00->fadcav[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      
      nobj = 128;
      for (j = 0; j < 2; j++)
	rcode += dst_unpacki4_(&talex00->fadc[i][j][0], &nobj, bank,&talex00_blen, &talex00_maxlen);
      
      nobj=2;
      rcode += dst_unpacki4_(&talex00->pchmip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpacki4_(&talex00->pchped[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpacki4_(&talex00->lhpchmip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpacki4_(&talex00->lhpchped[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpacki4_(&talex00->rhpchmip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpacki4_(&talex00->rhpchped[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpacki4_(&talex00->mftndof[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpackr8_(&talex00->mip[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpackr8_(&talex00->mftchi2[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      
      nobj=4;
      for(j=0;j<2;j++)
	{
	  rcode += dst_unpackr8_(&talex00->mftp[i][j][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
	  rcode += dst_unpackr8_(&talex00->mftpe[i][j][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
	}
      
      nobj=3;
      rcode += dst_unpackr8_(&talex00->lat_lon_alt[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      rcode += dst_unpackr8_(&talex00->xyz_cor_clf[i][0], &nobj, bank, &talex00_blen,&talex00_maxlen);
      
    }
  
  
  return rcode;
}

integer4 talex00_struct_to_dump_(talex00_dst_common *talex00, integer4 *long_output) {
  return talex00_struct_to_dumpf_(talex00, stdout, long_output);
}

integer4 talex00_struct_to_dumpf_(talex00_dst_common *talex00, FILE* fp, integer4 *long_output) 
{
  integer4 i, j, k;
  integer4 yr, mo, day, hr, min, sec, usec, xy[2];
  
  // dislay the bank name
  fprintf(fp,"%s :\n", "talex00");
  
  yr = talex00->yymmdd / 10000;
  mo = (talex00->yymmdd / 100) % 100;
  day = talex00->yymmdd % 100;
  hr = talex00->hhmmss / 10000;
  min = (talex00->hhmmss / 100) % 100;
  sec = talex00->hhmmss % 100;
  usec = talex00->usec;
  fprintf (fp,"event_num %d event_code %d site %s ",
	   talex00->event_num,talex00->event_code,talex00_list_towers(talex00->site));
  fprintf (fp,"errcode %d date %.02d/%.02d/%.02d %02d:%02d:%02d.%06d nofwf %d monyymmdd %06d monhhmmss %06d\n",
	   talex00->errcode,mo, day, yr, hr, min,sec, usec,
	   talex00->nofwf,talex00->monyymmdd,talex00->monhhmmss);
  

  if(*long_output ==0)
    {
      fprintf(fp,"%s\n",
	      "wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof");
      for(i=0;i<talex00->nofwf;i++)
	{
	  xy[0] = talex00->xxyy[i]/100;
	  xy[1] = talex00->xxyy[i]%100;
	  fprintf(fp,"%02d %5.02d %4d %3d %10d %10d %8d %8d %5d %4d %6d %7d %5d %5d %8.1f %6.1f %6.1f %6.1f %5d %4d\n",
		  i,talex00->wf_id[i],
		  xy[0],xy[1],talex00->clkcnt[i],
		  talex00->mclkcnt[i],talex00->fadcti[i][0],talex00->fadcti[i][1],
		  talex00->fadcav[i][0],talex00->fadcav[i][1],
		  talex00->pchmip[i][0],talex00->pchmip[i][1],
		  talex00->pchped[i][0],talex00->pchped[i][1],
		  talex00->mip[i][0],talex00->mip[i][1],
		  talex00->mftchi2[i][0],talex00->mftchi2[i][1],
		  talex00->mftndof[i][0],talex00->mftndof[i][1]);
	}
    }
  else if (*long_output == 1)
    {
      for(i=0;i<talex00->nofwf;i++)
	{
	  fprintf(fp,"%s\n",
		  "wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof lat_lon_alt xyz_coor_clf");
	  xy[0] = talex00->xxyy[i]/100;
	  xy[1] = talex00->xxyy[i]%100;	  
	  fprintf(fp,"%02d %5.02d %4d %3d %10d %10d %8d %8d %5d %4d %6d %7d %5d %5d %8.1f %6.1f %6.1f %6.1f %5d %4d %.2f %.2f %.1f %.1f %.1f %.1f \n",
		  i,talex00->wf_id[i],
		  xy[0],xy[1],talex00->clkcnt[i],
		  talex00->mclkcnt[i],talex00->fadcti[i][0],talex00->fadcti[i][1],
		  talex00->fadcav[i][0],talex00->fadcav[i][1],
		  talex00->pchmip[i][0],talex00->pchmip[i][1],
		  talex00->pchped[i][0],talex00->pchped[i][1],
		  talex00->mip[i][0],talex00->mip[i][1],
		  talex00->mftchi2[i][0],talex00->mftchi2[i][1],
		  talex00->mftndof[i][0],talex00->mftndof[i][1],
		  talex00->lat_lon_alt[i][0],talex00->lat_lon_alt[i][1],talex00->lat_lon_alt[i][2],
		  talex00->xyz_cor_clf[i][0],talex00->xyz_cor_clf[i][1],talex00->xyz_cor_clf[i][2]);
	  fprintf(fp,"lower fadc\n");
	  k=0;
	  for(j=0; j<128; j++)
	    {
	      if(k==12)
		{
		  fprintf(fp,"\n");
		  k = 0;
		}
	      fprintf(fp,"%6d ",talex00->fadc[i][0][j]);
	      k++;
	    }
	  fprintf(fp,"\nupper fadc\n");
	  k=0;
	  for(j=0; j<128; j++)
	    {
	      if(k==12)
		{
		  fprintf(fp,"\n");
		  k = 0;
		}
	      fprintf(fp,"%6d ",talex00->fadc[i][1][j]);
	      k++;
	    }
	  fprintf(fp,"\n");

	}
    }


  
  return 0;
}


const integer1* talex00_list_towers(integer4 tower_bit_flag)
{
  int itower = 0;
  talex00_tower_list[0] = 0;
  for (itower = 0; itower < TALEX00_NCT; itower++)
    {
      if((tower_bit_flag & (1 << itower)))
	memcpy(&talex00_tower_list[strlen(talex00_tower_list)],
	       talex00_tower_name_from_id(itower),strlen(talex00_tower_name_from_id(itower))+1);
    }
  if(!talex00_tower_list[0])
    sprintf(talex00_tower_list,"??");
  return (const char*)talex00_tower_list;
}

const integer1* talex00_tower_name_from_id(integer4 tower_id)
{
  if(tower_id < 0 || tower_id > TALEX00_NCT - 1)
    return "??";
  return (const char*)&TALEX00_TOWER_NAME[tower_id][0]; 
}


integer4 talex00_tower_id_from_name(const integer1* tower_name)
{
  integer4 tower_id = TALEX00_TOWER_UNDEFINED, i = 0;
  integer1* str = (integer1*)malloc(strlen(tower_name)+1);
  memcpy(str,tower_name,strlen(tower_name)+1);
  for (i=0; i < (integer4)strlen(tower_name); i++)
    str[i] = toupper(str[i]);
  for (i=0; i < TALEX00_NCT; i++)
    {
      if(strcmp(str,&TALEX00_TOWER_NAME[i][0]) == 0)
	{
	  tower_id = i;
	  break;
	}
    }
  free(str);
  return tower_id;
}

void talex00_fill_rusdraw()
{
  integer4 i = 0, iwf = 0, j = 0;
  integer4 talex00_tower_ind[3]={BRCT,LRCT,SKCT};
  rusdraw_.event_num = talex00_.event_num;
  rusdraw_.event_code = talex00_.event_code;
  rusdraw_.site = rusdraw_site_from_bitflag(talex00_.site);
  rusdraw_.errcode = talex00_.errcode;
  rusdraw_.yymmdd = talex00_.yymmdd;
  rusdraw_.hhmmss = talex00_.hhmmss;
  rusdraw_.usec = talex00_.usec;
  rusdraw_.monyymmdd = talex00_.monyymmdd;
  rusdraw_.monhhmmss = talex00_.monhhmmss;  
  for (i = 0; i < 3; i++)
    {
      rusdraw_.run_id[i] = talex00_.run_id[talex00_tower_ind[i]];
      rusdraw_.trig_id[i] = talex00_.trig_id[talex00_tower_ind[i]];
    }
  rusdraw_.nofwf = 0;
  for (iwf = 0; iwf < talex00_.nofwf; iwf++)
    {
      rusdraw_.nretry[rusdraw_.nofwf] = talex00_.nretry[iwf];
      rusdraw_.wf_id[rusdraw_.nofwf] = talex00_.wf_id[iwf];
      rusdraw_.trig_code[rusdraw_.nofwf] = talex00_.trig_code[iwf];
      rusdraw_.xxyy[rusdraw_.nofwf] = talex00_.xxyy[iwf];
      rusdraw_.clkcnt[rusdraw_.nofwf] = talex00_.clkcnt[iwf];
      rusdraw_.mclkcnt[rusdraw_.nofwf] = talex00_.mclkcnt[iwf];
      for (i = 0; i < 2; i++)
	{
	  rusdraw_.fadcti[rusdraw_.nofwf][i] = talex00_.fadcti[iwf][i];
	  rusdraw_.fadcav[rusdraw_.nofwf][i] = talex00_.fadcav[iwf][i];
	  for (j = 0; j < rusdraw_nchan_sd; j++)
	    rusdraw_.fadc[rusdraw_.nofwf][i][j] = talex00_.fadc[iwf][i][j];
	  rusdraw_.pchmip[rusdraw_.nofwf][i] = talex00_.pchmip[iwf][i];
	  rusdraw_.pchped[rusdraw_.nofwf][i] = talex00_.pchped[iwf][i];
	  rusdraw_.lhpchmip[rusdraw_.nofwf][i] = talex00_.lhpchmip[iwf][i];
	  rusdraw_.lhpchped[rusdraw_.nofwf][i] = talex00_.lhpchped[iwf][i];
	  rusdraw_.rhpchmip[rusdraw_.nofwf][i] = talex00_.rhpchmip[iwf][i];
	  rusdraw_.rhpchped[rusdraw_.nofwf][i] = talex00_.rhpchped[iwf][i];
	  rusdraw_.mftndof[rusdraw_.nofwf][i] = talex00_.mftndof[iwf][i];
	  rusdraw_.mip[rusdraw_.nofwf][i] = talex00_.mip[iwf][i];
	  rusdraw_.mftchi2[rusdraw_.nofwf][i] = talex00_.mftchi2[iwf][i];
	  for (j = 0; j < 4; j++)
	    {
	      rusdraw_.mftp[rusdraw_.nofwf][i][j] = talex00_.mftp[iwf][i][j];
	      rusdraw_.mftpe[rusdraw_.nofwf][i][j] = talex00_.mftpe[iwf][i][j];
	    }
	}
      rusdraw_.nofwf++;
    }
}
