/*
 * C functions for rusdraw
 * Dmitri Ivanov, ivanov@physics.rutgers.edu
 * Jun 17, 2008
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "dst_std_types.h"
#include "dst_bank_proto.h"
#include "dst_pack_proto.h"

#include "univ_dst.h"
#include "rusdraw_dst.h"



rusdraw_dst_common rusdraw_;	/* allocate memory to rusdraw_common */

static integer4 rusdraw_blen = 0;
static integer4 rusdraw_maxlen =
  sizeof (integer4) * 2 + sizeof (rusdraw_dst_common);
static integer1 *rusdraw_bank = NULL;

integer1* rusdraw_bank_buffer_ (integer4* rusdraw_bank_buffer_size)
{
  (*rusdraw_bank_buffer_size) = rusdraw_blen;
  return rusdraw_bank;
}

static void rusdraw_bank_init ()
{
  rusdraw_bank = (integer1 *) calloc (rusdraw_maxlen, sizeof (integer1));
  if (rusdraw_bank == NULL)
    {
      fprintf (stderr,
	       "rusdraw_bank_init: fail to assign memory to bank. Abort.\n");
      exit (0);
    }				/* else fprintf ( stderr,"rusdraw_bank allocated memory %d\n",rusdraw_maxlen); */
}

integer4
rusdraw_common_to_bank_ ()
{
  static integer4 id = RUSDRAW_BANKID, ver = RUSDRAW_BANKVERSION;
  integer4 rcode, nobj, i, j;

  if (rusdraw_bank == NULL)
    rusdraw_bank_init ();

  rcode =
    dst_initbank_ (&id, &ver, &rusdraw_blen, &rusdraw_maxlen, rusdraw_bank);
  /* Initialize test_blen, and pack the id and version to bank */

  nobj = 1;

  rcode +=
    dst_packi4_ (&rusdraw_.event_num, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen); 
  rcode +=
    dst_packi4_ (&rusdraw_.event_code, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen); 
  rcode +=
    dst_packi4_ (&rusdraw_.site, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);

  nobj=3;
  
  rcode +=
    dst_packi4_ (&rusdraw_.run_id[0], &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.trig_id[0], &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  nobj=1;
  
  rcode +=
    dst_packi4_ (&rusdraw_.errcode, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.yymmdd, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.hhmmss, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.usec, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.monyymmdd, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.monhhmmss, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.nofwf, &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  
  nobj = rusdraw_.nofwf;
  rcode +=
    dst_packi4_ (&rusdraw_.nretry[0], &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.wf_id[0], &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.trig_code[0], &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.xxyy[0], &nobj, rusdraw_bank, &rusdraw_blen,
		 &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.clkcnt[0], &nobj,
		 rusdraw_bank, &rusdraw_blen, &rusdraw_maxlen);
  rcode +=
    dst_packi4_ (&rusdraw_.mclkcnt[0], &nobj,
		 rusdraw_bank, &rusdraw_blen, &rusdraw_maxlen);

  for(i = 0;  i< rusdraw_.nofwf; i++)
    {
      
      nobj = 2;
      rcode +=
	dst_packi4_ (&rusdraw_.fadcti[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packi4_ (&rusdraw_.fadcav[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      nobj = 128;
      for (j = 0; j < 2; j++)
	{
	  rcode +=
	    dst_packi4_ (&rusdraw_.fadc[i][j][0], &nobj, rusdraw_bank,
			 &rusdraw_blen, &rusdraw_maxlen);
	}
      
      nobj=2;
      rcode +=
	dst_packi4_ (&rusdraw_.pchmip[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packi4_ (&rusdraw_.pchped[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packi4_ (&rusdraw_.lhpchmip[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packi4_ (&rusdraw_.lhpchped[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packi4_ (&rusdraw_.rhpchmip[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packi4_ (&rusdraw_.rhpchped[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packi4_ (&rusdraw_.mftndof[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packr8_ (&rusdraw_.mip[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_packr8_ (&rusdraw_.mftchi2[i][0], &nobj, rusdraw_bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      
      nobj=4;
      for(j=0;j<2;j++)
	{
	  rcode +=
	    dst_packr8_ (&rusdraw_.mftp[i][j][0], &nobj, rusdraw_bank, &rusdraw_blen,
			 &rusdraw_maxlen);
	  rcode +=
	    dst_packr8_ (&rusdraw_.mftpe[i][j][0], &nobj, rusdraw_bank, &rusdraw_blen,
			 &rusdraw_maxlen);
	}
      
    }
  
  return rcode;
}

integer4
rusdraw_bank_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  rcode = dst_write_bank_ (NumUnit, &rusdraw_blen, rusdraw_bank);
  free (rusdraw_bank);
  rusdraw_bank = NULL;
  return rcode;
}

integer4
rusdraw_common_to_dst_ (integer4 * NumUnit)
{
  integer4 rcode;
  if ((rcode = rusdraw_common_to_bank_ ()))
    {
      fprintf (stderr, "rusdraw_common_to_bank_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  if ((rcode = rusdraw_bank_to_dst_ (NumUnit)))
    {
      fprintf (stderr, "rusdraw_bank_to_dst_ ERROR : %ld\n", (long) rcode);
      exit (0);
    }
  return 0;
}

integer4
rusdraw_bank_to_common_ (integer1 * bank)
{
  integer4 rcode = 0;
  integer4 nobj, i, j;
  rusdraw_blen = 2 * sizeof (integer4);	/* skip id and version  */

  nobj = 1;

  rcode +=
    dst_unpacki4_ (&rusdraw_.event_num, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.event_code, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.site, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  nobj=3;
  rcode +=
    dst_unpacki4_ (&rusdraw_.run_id[0], &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.trig_id[0], &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  nobj=1;
  rcode +=
    dst_unpacki4_ (&rusdraw_.errcode, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.yymmdd, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.hhmmss, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.usec, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.monyymmdd, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.monhhmmss, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.nofwf, &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);

  nobj = rusdraw_.nofwf;

  rcode +=
    dst_unpacki4_ (&rusdraw_.nretry[0], &nobj, bank,
		   &rusdraw_blen, &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.wf_id[0], &nobj, bank,
		   &rusdraw_blen, &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.trig_code[0], &nobj, bank,
		   &rusdraw_blen, &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.xxyy[0], &nobj, bank,
		   &rusdraw_blen, &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.clkcnt[0], &nobj,
		   bank, &rusdraw_blen, &rusdraw_maxlen);
  rcode +=
    dst_unpacki4_ (&rusdraw_.mclkcnt[0], &nobj, bank,
		   &rusdraw_blen, &rusdraw_maxlen);

  for(i = 0;  i< rusdraw_.nofwf; i++)
    {
      nobj = 2;
      rcode +=
	dst_unpacki4_ (&rusdraw_.fadcti[i][0], &nobj, bank, &rusdraw_blen,
		       &rusdraw_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdraw_.fadcav[i][0], &nobj, bank, &rusdraw_blen,
		       &rusdraw_maxlen);
      nobj = 128;
      for (j = 0; j < 2; j++)
	{
	  rcode +=
	    dst_unpacki4_ (&rusdraw_.fadc[i][j][0], &nobj, bank,
			   &rusdraw_blen, &rusdraw_maxlen);
	}
      
      nobj=2;
      rcode +=
	dst_unpacki4_ (&rusdraw_.pchmip[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdraw_.pchped[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdraw_.lhpchmip[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdraw_.lhpchped[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdraw_.rhpchmip[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdraw_.rhpchped[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpacki4_ (&rusdraw_.mftndof[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpackr8_ (&rusdraw_.mip[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      rcode +=
	dst_unpackr8_ (&rusdraw_.mftchi2[i][0], &nobj, bank, &rusdraw_blen,
		     &rusdraw_maxlen);
      
      nobj=4;
      for(j=0;j<2;j++)
	{
	  rcode +=
	    dst_unpackr8_ (&rusdraw_.mftp[i][j][0], &nobj, bank, &rusdraw_blen,
			 &rusdraw_maxlen);
	  rcode +=
	    dst_unpackr8_ (&rusdraw_.mftpe[i][j][0], &nobj, bank, &rusdraw_blen,
			 &rusdraw_maxlen);
	}


    }

  return rcode;
}

integer4
rusdraw_common_to_dump_ (integer4 * long_output)
{
  return rusdraw_common_to_dumpf_ (stdout, long_output);
}

integer4
rusdraw_common_to_dumpf_ (FILE * fp, integer4 * long_output)
{
  integer4 i, j, k;
  integer4 yr, mo, day, hr, min, sec, usec, xy[2];
  
  fprintf (fp, "%s :\n","rusdraw");
  
  yr = rusdraw_.yymmdd / 10000;
  mo = (rusdraw_.yymmdd / 100) % 100;
  day = rusdraw_.yymmdd % 100;
  hr = rusdraw_.hhmmss / 10000;
  min = (rusdraw_.hhmmss / 100) % 100;
  sec = rusdraw_.hhmmss % 100;
  usec = rusdraw_.usec;
  fprintf (fp,"event_num %d event_code %d site ",
	   rusdraw_.event_num,rusdraw_.event_code);
  switch(rusdraw_.site)
    {
    case RUSDRAW_BR:
      fprintf(fp,"BR ");
      break;
    case RUSDRAW_LR:
      fprintf(fp,"LR ");
      break;
    case RUSDRAW_SK:
      fprintf(fp,"SK ");
      break;
    case RUSDRAW_BRLR:
      fprintf(fp,"BRLR ");
      break;
    case RUSDRAW_BRSK:
      fprintf(fp,"BRSK ");
      break;
    case RUSDRAW_LRSK:
      fprintf(fp,"LRSK ");
      break;
    case RUSDRAW_BRLRSK:
      fprintf(fp,"BRLRSK ");
      break;
    default:
      fprintf(fp,"%d ",rusdraw_.site);
      break;
    }
  fprintf(fp,"run_id: BR=%d LR=%d SK=%d trig_id: BR=%d LR=%d SK=%d\n",
	  rusdraw_.run_id[0],rusdraw_.run_id[1],rusdraw_.run_id[2],
	  rusdraw_.trig_id[0],rusdraw_.trig_id[1],rusdraw_.trig_id[2]);
  fprintf (fp,"errcode %d date %.02d/%.02d/%.02d %02d:%02d:%02d.%06d nofwf %d monyymmdd %06d monhhmmss %06d\n",
	   rusdraw_.errcode,mo, day, yr, hr, min,sec, usec,
	   rusdraw_.nofwf,rusdraw_.monyymmdd,rusdraw_.monhhmmss);
  

  if(*long_output ==0)
    {
      fprintf(fp,"%s\n",
	      "wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof");
      for(i=0;i<rusdraw_.nofwf;i++)
	{
	  xy[0] = rusdraw_.xxyy[i]/100;
	  xy[1] = rusdraw_.xxyy[i]%100;
	  fprintf(fp,"%02d %5.02d %4d %3d %10d %10d %8d %8d %5d %4d %6d %7d %5d %5d %8.1f %6.1f %6.1f %6.1f %5d %4d\n",
		  i,rusdraw_.wf_id[i],
		  xy[0],xy[1],rusdraw_.clkcnt[i],
		  rusdraw_.mclkcnt[i],rusdraw_.fadcti[i][0],rusdraw_.fadcti[i][1],
		  rusdraw_.fadcav[i][0],rusdraw_.fadcav[i][1],
		  rusdraw_.pchmip[i][0],rusdraw_.pchmip[i][1],
		  rusdraw_.pchped[i][0],rusdraw_.pchped[i][1],
		  rusdraw_.mip[i][0],rusdraw_.mip[i][1],
		  rusdraw_.mftchi2[i][0],rusdraw_.mftchi2[i][1],
		  rusdraw_.mftndof[i][0],rusdraw_.mftndof[i][1]);
	}
    }
  else if (*long_output == 1)
    {
      fprintf(fp,"%s\n",
	      "wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof");
      for(i=0;i<rusdraw_.nofwf;i++)
	{
	  xy[0] = rusdraw_.xxyy[i]/100;
	  xy[1] = rusdraw_.xxyy[i]%100;	  
	  fprintf(fp,"%02d %5.02d %4d %3d %10d %10d %8d %8d %5d %4d %6d %7d %5d %5d %8.1f %6.1f %6.1f %6.1f %5d %4d\n",
		  i,rusdraw_.wf_id[i],
		  xy[0],xy[1],rusdraw_.clkcnt[i],
		  rusdraw_.mclkcnt[i],rusdraw_.fadcti[i][0],rusdraw_.fadcti[i][1],
		  rusdraw_.fadcav[i][0],rusdraw_.fadcav[i][1],
		  rusdraw_.pchmip[i][0],rusdraw_.pchmip[i][1],
		  rusdraw_.pchped[i][0],rusdraw_.pchped[i][1],
		  rusdraw_.mip[i][0],rusdraw_.mip[i][1],
		  rusdraw_.mftchi2[i][0],rusdraw_.mftchi2[i][1],
		  rusdraw_.mftndof[i][0],rusdraw_.mftndof[i][1]);
	  fprintf(fp,"lower fadc\n");
	  k=0;
	  for(j=0; j<128; j++)
	    {
	      if(k==12)
		{
		  fprintf(fp,"\n");
		  k = 0;
		}
	      fprintf(fp,"%6d ",rusdraw_.fadc[i][0][j]);
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
	      fprintf(fp,"%6d ",rusdraw_.fadc[i][1][j]);
	      k++;
	    }
	  fprintf(fp,"\n");

	}
    }

  return 0;
}

integer4 rusdraw_site_from_bitflag(integer4 tower_bitflag)
{
  // calculate rusdraw site using information from the tower_bitflag variable
  switch ((integer4) (tower_bitflag & 7))
    {
    case 1:    // 001
      {
        return RUSDRAW_BR;
        break;
      }
    case 2:    // 010
      {
        return RUSDRAW_LR;
        break;
      }
    case 3:    // 011
      {
        return RUSDRAW_BRLR;
        break;
      }
    case 4:    // 100
      {
        return RUSDRAW_SK;
        break;
      }
    case 5:    // 101
      {
        return RUSDRAW_BRSK;
        break;
      }
    case 6:    // 110
      {
        return RUSDRAW_LRSK;
        break;
      }
    case 7:    // 111
      {
        return RUSDRAW_BRLRSK;
        break;
      }
    default:
      {
	// If the tower bitflag is neither of the above
	// then set the site to BR, LR, SK by default.
        return RUSDRAW_BRLRSK;
        break;
      }
    }
}

integer4 rusdraw_bitflag_from_site(integer4 rusdraw_site)
{
  integer4 tower_bitflag = 0;  // initialize
  if ((rusdraw_site == RUSDRAW_BR) ||  
      (rusdraw_site == RUSDRAW_BRLR) ||
      (rusdraw_site == RUSDRAW_BRSK) ||
      (rusdraw_site == RUSDRAW_BRLRSK))
    tower_bitflag |= 1; // bit x in **x is set
  if ((rusdraw_site == RUSDRAW_LR) ||  
      (rusdraw_site == RUSDRAW_BRLR) ||
      (rusdraw_site == RUSDRAW_LRSK) ||
      (rusdraw_site == RUSDRAW_BRLRSK))
    tower_bitflag |= 2; // bit x in *x* is set
  if ((rusdraw_site == RUSDRAW_SK) ||  
      (rusdraw_site == RUSDRAW_BRSK) ||
      (rusdraw_site == RUSDRAW_LRSK) ||
      (rusdraw_site == RUSDRAW_BRLRSK))
    tower_bitflag |= 4; // bit x in x** is set
  return tower_bitflag;
}
