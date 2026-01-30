/*
 * GENERIC TALE SD DST BANK
 * Added: 20101109, DI
 * Last Modified: 20210513, DI <dmiivanov@gmail.com>
 */

#ifndef _TALEX00_DST_
#define _TALEX00_DST_

#include <stdio.h>
#include "dst_std_types.h"

#define TALEX00_BANKID        13201
#define TALEX00_BANKVERSION   002

#ifdef __cplusplus
extern "C" {
#endif
integer4 talex00_common_to_bank_();
integer4 talex00_bank_to_dst_(integer4 *unit);
integer4 talex00_common_to_dst_(integer4 *unit); // combines above 2
integer4 talex00_bank_to_common_(integer1 *bank);
integer4 talex00_common_to_dump_(integer4 *opt);
integer4 talex00_common_to_dumpf_(FILE *fp, integer4 *opt);
integer1* talex00_bank_buffer_ (integer4* talex00_bank_buffer_size);
/* find tower ID from tower's string name, not case sensitive. returns
   some TALEX00_TOWER enum value (TALEX00_TOWER_UNDEFINED if no
   tower exists with the name provided) */
integer4 talex00_tower_id_from_name(const char* tower_name);
/* finds tower name when the tower ID is given.  returns "??" if
   the tower ID was not valid */
const integer1* talex00_tower_name_from_id(integer4 tower_id);
/* list tower names as string depending on which bits are set in tower_bit_flag:
     least significant bit = lower tower ID, most signficant bit = highest tower ID */
const integer1* talex00_list_towers(integer4 tower_bit_flag);

/* fills TA SD main specific information into rusdraw bank using talex00 information  */
void talex00_fill_rusdraw();
  
#ifdef __cplusplus
} //end extern "C"
#endif

#define TALEX00MWF 0x1000                               /*  max number of waveforms          */
#define talex00_nchan_sd 128                            /*  128 fadc channels per SD counter */

/* To o keep track of the ground array communication towers (CTs) */
enum TALEX00_TOWER
  {
    TALEX00_TOWER_UNDEFINED = -1, /* if function talex00_tower_id_from_name can't find tower ID */ 
    BRCT = 0, LRCT, SKCT,/* Main TA SD array towers */
    BFCT, DMCT, KMCT, SCCT, SNCT, SRCT, /* TAX4 SD towers */
    MDCT,       /* TALE infill array tower */
    TALEX00_NCT /* Total number of supported communication towers */
  };


/* To name the ground array towers */
static const char TALEX00_TOWER_NAME[TALEX00_NCT][3] =
  { "BR", "LR", "SK", /* main TASD towers */ 
    "BF", "DM", "KM", "SC", "SN", "SR",/* TAX4SD towers */
    "MD" /* TALE SD infill array tower */
  };

typedef struct 
{
  integer4 event_num;		                        /* event number */
  integer4 event_code;                                  /* 1=data, 0=Monte Carlo */
  integer4 site;                                        /* site bitflag index (bit0=BR,1=LR,2=SK,[3-8]=[BF,DM,KM,SC,SN,SR],bit9=MD */
  integer4 run_id[TALEX00_NCT];                         /* run IDs of the raw data files from each communication tower, -1 if irreleveant */
  integer4 trig_id[TALEX00_NCT];                        /* trigger IDs for each communication tower, -1 if irrelevant */
  integer4 errcode;                                     /* should be zero if there were no readout problems */
  integer4 yymmdd;		                        /* event year, month, day */
  integer4 hhmmss;		                        /* event hour minut second */
  integer4 usec;		                        /* event micro second */
  integer4 monyymmdd;                                   /* yymmdd at the beginning of the mon. cycle used in this event */
  integer4 monhhmmss;                                   /* hhmmss at the beginning of the mon. cycle used in this event */

  /* TODO: what does the internal trigger mean?  */
  integer4 level2_trig_code;                            /* level-2 trigger code: 1 self-triggered, 2 - hybrid, 0 is "internal" */
  
  integer4 nofwf;		                        /* number of waveforms in the event */



  /* These arrays contain the waveform information */
  integer4 nretry[TALEX00MWF];                          /* number of retries to get the waveform */
  integer4 wf_id[TALEX00MWF];                           /* waveform id in the trigger */
  integer4 trig_code[TALEX00MWF];                       /* level 1 trigger code */
  integer4 xxyy[TALEX00MWF];	                        /* detector that was hit (XXYY) */
  integer4 clkcnt[TALEX00MWF];	                        /* Clock count at the waveform beginning */
  integer4 mclkcnt[TALEX00MWF];	                        /* max clock count for detector, around 50E6 */ 
  /* 2nd index: [0] - lower, [1] - upper layers */
  integer4 fadcti[TALEX00MWF][2];	                /* fadc trace integral, for upper and lower */
  integer4 fadcav[TALEX00MWF][2];                       /* FADC average */
  integer4 fadc[TALEX00MWF][2][talex00_nchan_sd];	/* fadc trace for upper and lower */

  /* Useful calibration information  */
  integer4 pchmip[TALEX00MWF][2];     /* peak channel of 1MIP histograms */
  integer4 pchped[TALEX00MWF][2];     /* peak channel of pedestal histograms */
  integer4 lhpchmip[TALEX00MWF][2];   /* left half-peak channel for 1mip histogram */
  integer4 lhpchped[TALEX00MWF][2];   /* left half-peak channel of pedestal histogram */
  integer4 rhpchmip[TALEX00MWF][2];   /* right half-peak channel for 1mip histogram */
  integer4 rhpchped[TALEX00MWF][2];   /* right half-peak channel of pedestal histograms */

  /* Results from fitting 1MIP histograms */
  integer4 mftndof[TALEX00MWF][2];    /* number of degrees of freedom in 1MIP fit */
  real8    mip[TALEX00MWF][2];        /* 1MIP value (ped. subtracted) */
  real8    mftchi2[TALEX00MWF][2];    /* chi2 of the 1MIP fit */
  
  /* 
     1MIP Fit function: 
     [3]*(1+[2]*(x-[0]))*exp(-(x-[0])*(x-[0])/2/[1]/[1])/sqrt(2*PI)/[1]
     4 fit parameters:
     [0]=Gauss Mean
     [1]=Gauss Sigma
     [2]=Linear Coefficient
     [3]=Overall Scalling Factor
  */
  real8 mftp[TALEX00MWF][2][4];     /* 1MIP fit parameters */
  real8 mftpe[TALEX00MWF][2][4];    /* Errors on 1MIP fit parameters */
  
  
  real8 lat_lon_alt[TALEX00MWF][3]; /* GPS coordinates: latitude, longitude, altitude
				       [0] - latitude in degrees,  positive = North
				       [1] - longitude in degrees, positive = East
				       [2] - altitude is in meters */
  
  real8 xyz_cor_clf[TALEX00MWF][3];     /* XYZ coordinates in CLF frame:
					   origin=CLF, X=East,Y=North,Z=Up, [meters] */
  
} talex00_dst_common;

extern talex00_dst_common talex00_;

integer4 talex00_struct_to_abank_(talex00_dst_common *talex00, integer1 *(*pbank), integer4 id, integer4 ver);
integer4 talex00_abank_to_dst_(integer1 *bank, integer4 *unit);
integer4 talex00_struct_to_dst_(talex00_dst_common *talex00, integer1 *bank, integer4 *unit, integer4 id, integer4 ver);
integer4 talex00_abank_to_struct_(integer1 *bank, talex00_dst_common *talex00);
integer4 talex00_struct_to_dump_(talex00_dst_common *talex00, integer4 *opt);
integer4 talex00_struct_to_dumpf_(talex00_dst_common *talex00, FILE *fp, integer4 *opt);

#endif
