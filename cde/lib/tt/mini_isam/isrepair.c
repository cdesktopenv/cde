/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $TOG: isrepair.c /main/5 1998/04/10 08:04:42 mgreess $ 			 				 */
/* @(#)isrepair.c	1.7 93/07/30
 * Copyright (c) 1988, 1993 by Sun Microsystems, Inc.
 */

/*
 * isrepair.c
 *
 * Description:
 *	Repair an ISAM file. 
 */


#include "isam_impl.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>


/*
 * err =  isrepair(isfname, verbose)
 *
 * isrepair repairs an ISAM file. 
 *
 * The algorithm used is as the following:
 *   1. Read the control page of the possibly damaged file. We assume
 *	that the control page is not damaged.
 *   2. Open a new ISAM file with ~ suffix.
 *   3. Scan .rec file (and .var file if variable length records file)
 *	retrieve all records not marked as deleted, and write them
 *	to the ~ ISAM file.
 *   4. Delete the old ISAM file.
 *   5. Rename ~ file to the original file name.
 *   6. Build all indexes.
 *
 *   verbose option (if set to nonzero) will print messages to stdout.
 */

Static	char *rp_readrecord_v(), *rp_readrecord_f();
Static int printkey(int, struct keydesc *, int (*)(const char *, ...));
Static void cmd_error(const char *, int (*)(const char *, ...));
Static int typeletter();
Static rp_readcntlpg();
static int  isnoprintf(const char *, ...);

isrepair(isfname, verbose)
    char	*isfname;
    int		verbose;
{
  extern      char *rp_readrecord_v(), *rp_readrecord_f();
  char	      cntlpg[ISCNTLSIZE];
  int	      datfd, indfd, varfd;
  int	      minreclen, maxreclen;
  int	      nrecords_fromcntl;
  int	      varflag;
  char	      nameBuf [MAXPATHLEN];
  char	      *namebuf;
  int	      isfd = -1;
  struct keydesc2	keydesc2;
  int	      i;
  long	      offset, recfile_end;
  char	      *prec;
  long	      recnum;
  int	      nrecords_found, diff;
  long        lastrecno;
  struct keydesc keydesc;
  int	      (*print)(const char *, ...);
  sigset_t    oldmask;
  sigset_t    allsignals;
  char	      Buffer[BUFSIZ];
  char	      *buffer;


  print = verbose ? printf : isnoprintf;
  datfd = indfd = varfd = -1;

  /*
   * Open UNIX files.
   */
  if (strlen(isfname) + 10 >= MAXPATHLEN)
    namebuf = (char*) malloc(strlen(isfname) + 10);
  else
    namebuf = nameBuf;

  (void)strcpy(namebuf, isfname);
  _makedat_isfname(namebuf);
  datfd = open(namebuf, O_RDONLY);
  if (datfd > -1) {
	  fcntl(datfd, F_SETFD, 1);
  }

  (void)strcpy(namebuf, isfname);
  _makeind_isfname(namebuf);
  indfd = open(namebuf, O_RDONLY);
  if (indfd > -1) {
	  fcntl(indfd, F_SETFD, 1);
  }

  (void)strcpy(namebuf, isfname);
  _makevar_isfname(namebuf);
  varfd = open(namebuf, O_RDONLY);
  if (varfd > -1) {
	  fcntl(varfd, F_SETFD, 1);
  }

  (void)print("Reading control page from %s.rec file...\n",
	      isfname);
  if (rp_readcntlpg(datfd, cntlpg) == ISERROR) {
    (void)print("Cannot read the control page\n");
    goto ERROR;
  }

  /*
   * Check magic. Repair only ISAM files!!!
   */

  if (strncmp(cntlpg + CP_MAGIC_OFF, ISMAGIC, strlen(ISMAGIC)) != 0) {
    (void)print("Bad magic in %s.rec\n", isfname);
    goto ERROR;
  }

  varflag = ldint(cntlpg + CP_VARFLAG_OFF);
  minreclen = ldint(cntlpg + CP_MINRECLEN_OFF);
  maxreclen = ldint(cntlpg + CP_MAXRECLEN_OFF);	

  /*
   *  Check for maxreclen field value of -1. This could have occured due to
   *  ISMAXRECLEN being incorrectly set to 65535 in an earlier version. If
   *  this field is -1 and it's a variable length record, reset to the new
   *  value of MAXRECLEN. This means that this field will be repaired when
   *  the control block is written back to disk.
   */

  if (maxreclen == -1 && varflag) {
    maxreclen = ISMAXRECLEN;
  }

  lastrecno = ldlong(cntlpg + CP_LASTRECNO_OFF);	
  nrecords_fromcntl = ldlong(cntlpg + CP_NRECORDS_OFF);

  /*
   * Open output file. Use ~ as suffix.
   */
  (void)sprintf(namebuf, "%s~", isfname);
  (void)print("Opening temporary ISAM file '%s'...\n",
	      namebuf);
  isreclen = minreclen;
  if ((isfd = isbuild(namebuf, maxreclen, nokey, ISINOUT + ISEXCLLOCK +
		      (varflag?ISVARLEN:0))) == ISERROR) {
    (void)print("Cannot open temporary ISAM file %s\n",
		namebuf);
    if (iserrno == EEXIST)
      (void)print("File %s already exists\n", namebuf);
    goto ERROR;
  }

  /*
   * Scan .rec file and read all undeleted records.
   */
  (void)print("Salvaging records from %s.rec%s file...\n",
	      isfname, varflag?" (and .var file)" : "");

  offset = ISCNTLSIZE;
  recfile_end = lseek(datfd, 0L, 2);
  recnum = 1;
  nrecords_found = 0;
    
  while (recnum <= lastrecno && offset < recfile_end - minreclen) {

    if (varflag) {
      prec = rp_readrecord_v(datfd, varfd, offset, minreclen, maxreclen);	
      offset += minreclen + LONGSIZE;
    }
    else {
      prec = rp_readrecord_f(datfd, offset, minreclen);
      offset += minreclen + 1;
    }
	
    if (prec != NULL) {
      if (iswrrec(isfd, recnum, prec) == ISERROR) {
	cmd_error("iswrrec", print);
	goto ERROR;
      }
      nrecords_found++;
    }
    recnum++;
  }

  diff = nrecords_found - nrecords_fromcntl;

  if (diff == 0)
    (void)print("All records found - total %d records\n",
		nrecords_found);
  else
    (void)print("%d records found - %d records %s than in header\n",
		nrecords_found, diff, diff > 0 ?
		"more" : "less");

  /*
   * Close all file descriptors.
   */
  (void)close(datfd);
  (void)close(indfd);
  (void)close(varfd);
  (void)isclose(isfd);

  (void) sigfillset(&allsignals);
  (void) sigprocmask(SIG_SETMASK, &allsignals, &oldmask);

  (void)print("Erasing ISAM file '%s'...\n", isfname);
  /*    if (iserase(isfname) != ISOK) {
	cmd_error("iserase", print);
	goto ERROR;
	}
	*/
  if (strlen(isfname) + 5 >= MAXPATHLEN)
    buffer = (char*) malloc(strlen(isfname) + 5);
  else
    buffer = Buffer;

  (void)sprintf(buffer,"%s.rec", isfname);
  (void)unlink(buffer);
  (void)sprintf(buffer,"%s.ind", isfname);
  (void)unlink(buffer);
  (void)sprintf(buffer,"%s.var", isfname);
  (void)unlink(buffer);

  (void)sprintf(namebuf, "%s~", isfname);
  (void)print("Renaming ISAM file '%s' to '%s'...\n",
	      namebuf, isfname);
  if (isrename(namebuf, isfname) != ISOK) {
    cmd_error("isrename", print);
    goto ERROR;
  }

  /*
   * Re-open the file and add keys.
   */
  if (ldshort(cntlpg + CP_NKEYS_OFF) > 0) {
    (void)print("Adding keys...\n");

    if ((isfd = isopen(isfname, ISEXCLLOCK + ISINOUT +
		       (varflag?ISVARLEN:0))) == ISERROR) {
      cmd_error("isopen", print);
      goto ERROR;
    }
	
    for (i = 0; i < ldshort(cntlpg + CP_NKEYS_OFF); i++) {
      ldkey(&keydesc2, cntlpg + CP_KEYS_OFF + i * K2_LEN);
      _iskey_itox(&keydesc2, &keydesc);

      if (keydesc.k_nparts == 0) /* special case for no primary */
	continue;

      printkey (i+1, &keydesc, print);

      if (i == 0) {
	if (isaddprimary(isfd, &keydesc) == ISERROR) {
	  cmd_error("isaddprimary", print);
	  (void)isclose(isfd);
	  goto ERROR;
	}
      }
      else {
	if (isaddindex(isfd, &keydesc) == ISERROR) {
	  cmd_error("isaddindex", print);
	  (void)isclose(isfd);
	  goto ERROR;
	}
      }
    }
  }
  (void)isclose(isfd);
  (void)sigprocmask(SIG_SETMASK, &oldmask, NULL);

  print("...File repaired\n");
  return (ISOK);

 ERROR:
  (void)print("\007Didn't repair ISAM file '%s'\n", isfname);
  (void)close(datfd);
  (void)close(indfd);
  (void)close(varfd);
  (void)isclose(isfd);
  if (namebuf != nameBuf) free(namebuf);
  if (buffer != Buffer) free(buffer);

  return (ISERROR);
}

/******* low level data access used by the 'repair' utility ******************/

static char	recordbuffer[ISMAXRECLEN + LONGSIZE];

/* rp_readcntlpg() - Read the control page */
Static 
rp_readcntlpg(datfd, cntlpg)
    int		datfd;
    char	*cntlpg;
{
    if (read (datfd, cntlpg, ISCNTLSIZE) != ISCNTLSIZE)
	return (ISERROR);

    return (ISOK);
}

/* rp_readrecord_f() - Read a record from .rec file */
Static char *
rp_readrecord_f(datfd, offset, reclen)
    int		datfd;
    long	offset;
    int		reclen;
{
    if (lseek(datfd, offset, 0) != offset)
	return ((char *) NULL);

    if (read(datfd, recordbuffer, reclen + 1) != (reclen + 1))
	return ((char *) NULL);

    if (recordbuffer[0] == FL_RECDELETED)
	return ((char *) NULL);

    return (recordbuffer + 1);
}

/* rp_readrecord_v() - Read a record from .rec file */
Static char *
rp_readrecord_v(datfd, varfd, offset, minreclen, maxreclen)
    int		datfd, varfd;
    long	offset;
    int		minreclen, maxreclen;
{
    long	tailoff;
    char	frameheadbuf [2 * SHORTSIZE];
    int		taillen;
    
    if (lseek(datfd, offset, 0) != offset)
	return ((char *) NULL);

    if (read(datfd, recordbuffer, minreclen + LONGSIZE) != (minreclen + LONGSIZE))
	return ((char *) NULL);

    if ((tailoff = ldlong(recordbuffer)) == VL_RECDELETED)
	return ((char *) NULL);

    isreclen = minreclen;

    /* Recover tail of the record */
    if (tailoff != VL_RECNOTAIL) {

	if (lseek(varfd, tailoff, 0) != tailoff)
	    goto OKEXIT;

	if (read(varfd, frameheadbuf, 2 * SHORTSIZE) != 2 * SHORTSIZE)
	    goto OKEXIT;

	taillen	 = (int) ldshort(frameheadbuf + VR_TAILLEN_OFF);

	if (taillen <= 0 || taillen + minreclen > maxreclen)
	    goto OKEXIT;

	if (read(varfd, recordbuffer + LONGSIZE + isreclen, taillen) != taillen)
	    goto OKEXIT;

	isreclen += taillen;
    }

 OKEXIT:
    return (recordbuffer + LONGSIZE);
}

static int
isnoprintf(const char *s, ...)
{
    return(0);
}

static int
printkey(int n, struct keydesc *pkdesc, int (*print)(const char *, ...))
{
    int		i;
    struct keypart *pk;

    if (pkdesc->k_nparts == 0) {
	print("%3d: --- NO PRIMARY KEY ---\n", n);
	return 0;
    }

    if (n == 1)
	print("P%3d: %s ", n, (pkdesc->k_flags&ISDUPS) ?
	      "DUPS  " : "NODUPS");
    else
	print(" %3d: %s ", n, (pkdesc->k_flags&ISDUPS) ?
	      "DUPS  " : "NODUPS");


    for (i = 0; i < pkdesc->k_nparts; i++) {
	pk = pkdesc->k_part + i;
	print(" %d%c%d%s", pk->kp_start, 
	       typeletter(pk->kp_type & ~ISDESC), pk->kp_leng,
	       (pk->kp_type & ISDESC)?"D":" ");
    }
    print("\n");
    return 1;
}

static void
cmd_error(const char *str, int (*print)(const char *, ...))
{
    (void)print("%s: ISAM error %d\n", str, iserrno);
}

static int
typeletter(type)
    int 	type;
{
    switch (type) {
    case INTTYPE:
	return 'I';
    case LONGTYPE:
	return 'L';
    case FLOATTYPE:
	return 'F';
    case DOUBLETYPE:
	return 'D';
    case CHARTYPE:
	return 'C';
    case BINTYPE:
	return 'B';
    default:
	assert(0);
    }
    /* NOTREACHED */
}
