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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: dosync.c /main/6 1998/10/26 12:40:53 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           dosync.c
 *
 *
 *   DESCRIPTION:    Routines synchronizing directories
 *
 *   FUNCTIONS: CONFIRM
 *		CmpPred
 *		DTYPE
 *		DirCmp
 *		DirCmp2
 *		DumpDir
 *		ERROR_CHECK
 *		FreeDir
 *		GetDir
 *		GetDirEntry
 *		SortDir
 *		SyncDirRecur
 *		SyncDirectory
 *		SyncItem
 *		cmpSymlink
 *		dirEraseCallback
 *		dirErrorCallback
 *		doCopy
 *		doCopyLink
 *		doMkdir
 *		doSymlink
 *		doUnlink
 *		targetLink
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/file.h>
#include "fsrtns.h"
#ifdef PATRTNS
#include "patrtns.h"
#endif
#include "dosync.h"

/*--------------------------------------------------------------------
 * global variables
 *------------------------------------------------------------------*/

char *FileOpNames[] = {
  "synchronize",
  "opendir",
  "lstat",
  "stat",
  "readlink",
  "delete",
  "copy",
  "mkdir",
  "copy link",
  "make link",
  "working",
  "is a copy",
  "is a link"
};


int (*syncConfirmCallback)(FileOp op,
                           char *sname, int stype,
                           char *tname, int ttype,
                           char *link) = NULL;
int (*syncErrorCallback)(FileOp op, char *fname, int errnum) = NULL;


static SyncParams *SP;


/*--------------------------------------------------------------------
 * macros for invoking callback functions
 *------------------------------------------------------------------*/

#define DTYPE(delP) ((delP)? (delP)->ftype: ft_noent)

#define CONFIRM(op, sn, st, tn, tt, l, rc) \
  if (periodicCallback && (*periodicCallback)() != 0) return -1; \
  else if (syncConfirmCallback && \
      (rc = syncConfirmCallback(op, sn, st, tn, tt, l)) != 0) return rc; else

#define ERROR_CHECK(op, f, rc) \
  if (rc < 0 || rc && syncErrorCallback && syncErrorCallback(op, f, rc) < 0) \
    return -1; else


/*--------------------------------------------------------------------
 * reading directories
 *------------------------------------------------------------------*/

/* structure containing information anout a directory entry */
typedef struct de {
  char name[256];      /* file name */
  int rc, lrc;         /* return codes from stat, lstat */
  struct stat stat;    /* stat */
  int ftype;           /* file type flags (see #defines in dosync.h) */
  char exclude, skip;  /* exclude, skip flags */
  char *link;          /* value of symbolic link */
  struct de *teP;      /* source dir: points to correspond. target dir entry */
  struct de *next;     /* next directory entry */
} DirEntry;


/*
 * DumpDir: dump linked list of directory entries to stdout (debugging only)
 */
static void
DumpDir(DirEntry *dP)
{
  DirEntry *eP;

  for (eP = dP; eP; eP = eP->next)
    printf(" %-15s %2d  %c %c %c %c  %c  %s\n",
           eP->name,
           eP->rc,
           (eP->ftype & ft_isdir)? 'D': ' ',
           (eP->ftype & ft_islnk)? 'L': ' ',
           eP->exclude? 'X': ' ',
           eP->skip? 'S': ' ',
           eP->teP? 'F': ' ',
           eP->link? eP->link: "");
}


/*
 * DirCmp: compare function for sorting directory entries
 */
static int
DirCmp(const void *p1, const void *p2)
{
  return strcmp((*(DirEntry **)p1)->name, (*(DirEntry **)p2)->name);
}


/*
 * SortDir: sort linked list of directory entries by name
 */
static DirEntry *
SortDir(DirEntry *dP, int (*cmp)(const void*, const void*))
{
  int i, n;
  DirEntry *eP, **linkPP;
  DirEntry *da[1024], **daP;

  /* count number of directory entries */
  n = 0;
  for (eP = dP; eP; eP = eP->next)
    n++;
  if (n < 2)
    return dP;

  /* allocate pointer array */
  if (n <= 1024)
    daP = da;
  else
    daP = (DirEntry **)malloc(n * sizeof(DirEntry *));

  /* sort pointer array */
  for (eP = dP, i = 0; eP; eP = eP->next, i++)
    daP[i] = eP;
  qsort(daP, n, sizeof(DirEntry *), cmp);

  /* re-link the linked list */
  linkPP = &dP;
  for (i = 0; i < n; i++) {
    *linkPP = daP[i];
    linkPP = &(*linkPP)->next;
  }
  *linkPP = NULL;

  /* free pointer array */
  if (daP != da)
    free(daP);

  return dP;
}


/*
 * FreeDir: de-allocate linked list of directory entries
 */
static void
FreeDir(DirEntry *deP)
{
  DirEntry *nextP;

  while (deP) {
    nextP = deP->next;
    free(deP);
    deP = nextP;
  }
}


/*
 * GetDirEntry: Get information about one directory entry.
 * If an error occurs, returns the errno in *rc and the operation
 * that failed (op_lstat, op_stat, or op_readlink) in *op.
 */
static DirEntry *
GetDirEntry(char *fname, FileOp *op, int *rc)
{
  DirEntry *deP;
  char *p;
  char link[1024];
  int l;

  /* allocate new DirEntry */
  deP = (DirEntry *)malloc(sizeof(DirEntry));
  memset(deP, 0, sizeof(DirEntry));

  /* get file name */
  p = strrchr(fname, '/');
  if (p && p > fname)
    snprintf(deP->name, sizeof(deP->name), "%s", p + 1);
  else
    snprintf(deP->name, sizeof(deP->name), "%s", fname);

  /* assume everything is fine */
  *op = 0;
  *rc = 0;

  /* get information */
  deP->rc = lstat(fname, &deP->stat);
  if (deP->rc) {
    *op = op_lstat;
    *rc = errno;
  } else if ((deP->stat.st_mode & S_IFMT) == S_IFDIR)
    deP->ftype |= ft_isdir;
  else if ((deP->stat.st_mode & S_IFMT) == S_IFLNK) {
    deP->ftype |= ft_islnk;
    deP->lrc = stat(fname, &deP->stat);
    if (deP->lrc) {
      if (errno != ENOENT) {
        *op = op_stat;
        *rc = errno;
      }
    } else if ((deP->stat.st_mode & S_IFMT) == S_IFDIR)
      deP->ftype |= ft_isdir;
    l = readlink(fname, link, sizeof(link) - 1);
    if (l < 0) {
      *op = op_readlink;
      *rc = errno;
    } else {
      link[l] = 0;
      deP->link = strdup(link);
    }
  }

  return deP;
}


/*
 * GetDir: read a directory and return linked list of directory entries
 */
static int
GetDir(char *dirname, PatternList *xl, PatternList *sl, DirEntry **listPP)
{
  DIR *dirP = NULL;                      /* open directory */
  struct dirent *entryP;          /* directory entry */
  DirEntry *deP, *firstP, **linkPP;
  char fname[1024], *fnP;
  PatternList *pl;
  FileOp op;
  int rc;


  /* open directory */
  dirP = opendir(dirname);
  if (dirP == NULL) {
    rc = errno;
    if (syncErrorCallback &&
        syncErrorCallback(op_opendir, dirname, rc) < 0)
    {
      return -1;
    }
    return rc;
  }

  /* copy dirname to file name buffer */
  snprintf(fname, sizeof(fname), "%s", dirname);
  fnP = fname + strlen(fname);
  *fnP++ = '/';

  /* initialize linked list */
  firstP = NULL;
  linkPP = &firstP;

  /* read the directory */
  while ((entryP = readdir(dirP)) != NULL) {
    /* skip . and .. */
    if (strcmp(entryP->d_name, ".") == 0 || strcmp(entryP->d_name, "..") == 0)
      continue;

    /* get new DirEntry */
    strcpy(fnP, entryP->d_name);
    deP = GetDirEntry(fname, &op, &rc);

    /* add to linked list */
    *linkPP = deP;
    linkPP = &deP->next;

    /* if error occurred, call error callback function */
    if (rc != 0 && syncErrorCallback && syncErrorCallback(op, fname, rc) < 0) {
      closedir(dirP);
      FreeDir(firstP);
      return -1;
    }

    /* check exclude and skip list */
#ifdef PATRTNS
    for (pl = xl ; pl; pl = pl->next)
      if (MatchPattern(pl->pattern, fname))
        deP->exclude++;

    for (pl = sl; pl; pl = pl->next)
      if (MatchPattern(pl->pattern, fname))
        deP->skip++;
#endif
  }

  closedir(dirP);
  *listPP = SortDir(firstP, DirCmp);
  return 0;
}


/*--------------------------------------------------------------------
 * file operations
 *------------------------------------------------------------------*/

static int erase_rc;

/*
 * dirEraseCallback: used in doUnlink (below) for confirmation
 * during recursive directory deletes
 */
static int
dirEraseCallback(char *fname)
{
  erase_rc = syncConfirmCallback(op_delete, "", ft_noent, fname, 0, "");
  return erase_rc;
}

static int
dirErrorCallback(char *fname, int errnum)
{
  erase_rc = syncErrorCallback(op_delete, fname, errnum);
  return erase_rc;
}


/*
 * doUnlink: delete a file from the target directory
 */
static int
doUnlink(char *fname, DirEntry *fP, int confirm)
{
  int rc;

  if (confirm)
    CONFIRM(op_delete, "", ft_noent, fname, fP->ftype, "", rc);

  if (SP->dontdoit)
    rc = 0;

  else if (SP->keepold) {
    char newname[1024];

    snprintf(newname, sizeof(newname), "%s%s", fname, SP->keepold);
    fsMove(fname, newname, 1, &rc);

  } else if ((fP->ftype & ft_isdir) && !(fP->ftype & ft_islnk)) {
    /* arrange for syncConfirmCallback's during recursive directory deletes */
    erase_rc = 0;
    if (syncErrorCallback)
      errorCallback = dirErrorCallback;
    if (syncConfirmCallback)
      progressCallback = dirEraseCallback;

    fsErase(fname, &rc, 1);
    if (rc < 0 && erase_rc)
      rc = ENOTEMPTY;

    errorCallback = NULL;
    progressCallback = NULL;

  } else
    rc = (unlink(fname) != 0)? errno: 0;

  /* check for errors */
  ERROR_CHECK(op_delete, fname, rc);
  return rc;
}


/*
 * doCopy: copy a file from the source to target directory
 */
static int
doCopy(char *sname, DirEntry *sP, char *tname, DirEntry *delP)
{
  int rc;

  /* display message */
  CONFIRM(op_copy, sname, sP->ftype, tname, DTYPE(delP), "", rc);

  /* if necessary, delete target */
  if (delP && !SP->dontdoit)
    if ((rc = doUnlink(tname, delP, 0)) != 0)
      return rc;

  /* copy the file */
  if (SP->dontdoit)
    rc = 0;
  else
    fsCopy(sname, tname, 0, &rc);

  /* check for errors */
  ERROR_CHECK(op_copy, sname, rc);
  return rc;
}


/*
 * doMkdir: create a new sub directory in the target directory
 */
static int
doMkdir(char *sname, DirEntry *sP, char *tname, DirEntry *delP)
{
  int rc;

  /* display message */
  CONFIRM(op_mkdir, sname, sP->ftype, tname, DTYPE(delP), "", rc);

  /* if necessary, delete target */
  if (delP && !SP->dontdoit)
    if ((rc = doUnlink(tname, delP, 0)) != 0)
      return rc;

  /* make a new directory */
  rc = SP->dontdoit? 0: (mkdir(tname, sP->stat.st_mode & 07777) < 0)? errno: 0;

  /* check for errors */
  ERROR_CHECK(op_mkdir, tname, rc);
  return rc;
}


/*
 * targetLink: lookup symbloc link in the maplink list
 */
static void
targetLink(char *source, char *target, char **tlPP, char **msgPP)
/*
 *
 */
{
  static char buf[1024] = " -> ";
  MapList *ml;
  int l;

  for (ml = SP->maplink; ml; ml = ml->next) {
    l = strlen(ml->from);
    if (strncmp(ml->from, source, l) == 0
        && (source[l] == '/' || source[l] == 0))
    {
      strcpy(buf + 4, ml->to);
      if (source[l])
        strcat(buf, source +l);
      if (strcmp(buf + 4, target) != 0) {
        *tlPP = buf + 4;
        *msgPP = buf;
        return;
      }
    }
  }

  *tlPP = source;
  *msgPP = "";
}


/*
 * doCopyLink: copy a symbolic link from the source to target directory
 */
static int
doCopyLink(char *sname, DirEntry *sP, char *tname, DirEntry *delP)
{
  char *tl, *msg;
  int rc;

  /* determine where the link should point */
  targetLink(sP->link, tname, &tl, &msg);

  /* display message */
  CONFIRM(op_cplink, sname, sP->ftype, tname, DTYPE(delP), msg, rc);

  /* if necessary, delete target */
  if (delP && !SP->dontdoit)
    if ((rc = doUnlink(tname, delP, 0)) != 0)
      return rc;

  /* create link */
  rc = SP->dontdoit? 0: (symlink(tl, tname) != 0)? errno: 0;

  /* check for errors */
  ERROR_CHECK(op_cplink, tname, rc);
  return rc;
}


/*
 * doSymlink: create a symbolic link in the target directory to a file
 *            in the source directory
 */
static int
doSymlink(char *sname, DirEntry *sP, char *tname, DirEntry *delP)
{
  char *tl, *msg;
  int rc;

  /* determine where the link should point */
  targetLink(sname, tname, &tl, &msg);

  /* display message */
  CONFIRM(op_mklink, sname, sP->ftype, tname, DTYPE(delP), msg, rc);

  /* if necessary, delete target */
  if (delP && !SP->dontdoit)
    if ((rc = doUnlink(tname, delP, 0)) != 0)
      return rc;

  /* create link */
  targetLink(sname, tname, &tl, &msg);
  rc = SP->dontdoit? 0: (symlink(tl, tname) != 0)? errno: 0;

  /* check for errors */
  ERROR_CHECK(op_mklink, sname, rc);
  return rc;
}


/*
 * cmpSymlink: compare two symbolic links
 *             return values:
 *              -1  error reading one of the links
 *               0  links point to the same file
 *               1  tname link points to sname
 *               2  links point to different files
 */
static int
cmpSymlink(char *sname, DirEntry *seP, char *tname, DirEntry *teP)
{
  char *tl, *msg;

  /* return error if target link could not be read */
  if (!teP->link)
    return -1;

  /* check if target link points to source link */
  if (SP->keeplinks) {
    targetLink(sname, tname, &tl, &msg);
    if (strcmp(teP->link, tl) == 0)
      return 1;
  }

  /* if source not a symbolic link, target must be pointing to wrong file */
  if (!(seP->ftype & ft_islnk))
    return 2;

  /* compare the links */
  targetLink(seP->link, tname, &tl, &msg);
  if (strcmp(teP->link, tl) == 0)
    return 0;
  else
    return 2;
}


/*--------------------------------------------------------------------
 * sync directories
 *------------------------------------------------------------------*/

static int SyncDirRecur(char *sdir, char *tdir);


/*
 * SyncItem: copy or replace one item from the source to the target directory
 */
static int
SyncItem(char *sname, DirEntry *sP, char *tname, DirEntry *tP)
{
  int rc;

  if (periodicCallback && (*periodicCallback)() != 0)
    return -1;

  if (!tP) {
    /* target does not exist */

rpl_target:
    /*--------------------------------------------------------------------
     * target does not exist or should be replaced
     *------------------------------------------------------------------*/

    /* check if -dontreplace or -dontadd options apply */
    if (tP && SP->dontreplace || !tP && SP->dontadd)
      return 0;

    if (sP->ftype & ft_islnk) {
      /* source is a link */

      if ((sP->ftype & ft_isdir) && !SP->copydirs ||
          !(sP->ftype & ft_isdir) && !SP->copyfiles)
      {
        /* just copy the link */
        return doCopyLink(sname, sP, tname, tP);
      }

    } else {
      /* source is not a link */

      if ((sP->ftype & ft_isdir) && SP->linkdirs ||
          !(sP->ftype & ft_isdir) && SP->linkfiles)
      {
        /* don't copy; just create a link to the target */
        return doSymlink(sname, sP, tname, tP);
      }
    }

    if (sP->ftype & ft_isdir) {
      /* source is a dir: create target directory and do recursive sync */
      rc = doMkdir(sname, sP, tname, tP);
      if (rc || SP->dontdoit)
        return rc;
      else
        return SyncDirRecur(sname, tname);

    } else
      /* source is a regular file: copy the file */
      return doCopy(sname, sP, tname, tP);


  } else if (tP->ftype & ft_islnk) {
    /*--------------------------------------------------------------------
     * target is a symbolic link
     *------------------------------------------------------------------*/

    if ((tP->ftype & ft_isdir) == (sP->ftype & ft_isdir)) {
      /*
       * target points to the right type of object (dir vs regular file):
       * check if target points to the right place
       */
      rc = cmpSymlink(sname, sP, tname, tP);
      if (rc < 0)
        return 1; /* something is wrong */

      if (SP->listlinks && rc == 1)
        CONFIRM(op_info_link, sname, sP->ftype, tname, tP->ftype, "", rc);

      if (rc < 2)
        /* target is pointing to the right place */
        return 0;
    }

    /*
     * target points to the wrong type of object:
     * delete target link and try again
     */
    goto rpl_target;


  } else if (!(tP->ftype & ft_isdir)) {
    /*--------------------------------------------------------------------
     * target is regular file
     *------------------------------------------------------------------*/

    if (sP->ftype & ft_isdir)
      /* source is a dir: delete target file and try again */
      goto rpl_target;

    else {
      /* source is regular file or link to regular file */

      if (sP->ftype & ft_islnk) {
        /* source is just a link */

        if (!SP->keepcopies && !SP->copyfiles)
          /* replace target by a link */
          goto rpl_target;

        else if (SP->listcopies)
	  CONFIRM(op_info_copy, sname, sP->ftype, tname, tP->ftype, "", rc);
      }

      if (!SP->forcecopies &&
          tP->stat.st_mtime == sP->stat.st_mtime &&
          tP->stat.st_size  == sP->stat.st_size)
      {
        /* we assume the file hasn't changed: don't do anything */
        return 0;
      }

      if (SP->keepnew && tP->stat.st_mtime > sP->stat.st_mtime)
        /* we should keep the target because it is newer than the source */
        return 0;

      /* replace the target file */
      goto rpl_target;
    }


  } else if (!(tP->ftype & ft_islnk)) {
    /*--------------------------------------------------------------------
     * target is a directory
     *------------------------------------------------------------------*/

    if (sP->ftype & ft_isdir) {
      if (sP->ftype & ft_islnk) {
        /* source is just a link */

        if (!SP->keepcopies && !SP->copydirs)
          /* replace target by a link */
          goto rpl_target;

        else if (SP->listcopies)
	  CONFIRM(op_info_copy, sname, sP->ftype, tname, tP->ftype, "", rc);
      }

      /* source is dir: recursively sync source and target dirs */
      return SyncDirRecur(sname, tname);

    } else
      /* source is no dir: delete target dir and try again */
      goto rpl_target;
  }

  /* should never be reached */
  return -1;
}


/*
 * CmpPred: returns a precedence number that is used to determine in which
 * order source directory entries are processed:
 *   1  source is regular file
 *   2  source is symbolic link
 *   3  target is not a real directory
 *   4  target exists and is a real directory (not a sym link)
 */
static int
CmpPred(DirEntry *dP)
{
  if (!(dP->ftype & ft_isdir))
    return 1;

  else if (dP->ftype & ft_islnk)
    return 2;

  else if (!dP->teP ||
           !(dP->teP->ftype & ft_isdir) ||
           (dP->teP->ftype & ft_islnk))
  {
    return 3;

  } else
    return 4;
}


/*
 * DirCmp2: compare function for sorting directory entries
 */
static int
DirCmp2(const void *p1, const void *p2)
{
  int d = CmpPred(*(DirEntry **)p1) - CmpPred(*(DirEntry **)p2);
  if (d)
    return d;
  else
    return strcmp((*(DirEntry **)p1)->name, (*(DirEntry **)p2)->name);
}


/*
 * SyncDirRecur: recursively synchronize source and target directories
 */
static int
SyncDirRecur(char *sdir, char *tdir)
{
  DirEntry *sP, *tP, *seP, *teP;
  char *fsP, *ftP;
  int rc;

  /* informational callback */
  if (SP->verbose)
    CONFIRM(op_sync, sdir, ft_isdir, tdir, ft_isdir, "", rc);

  /* read source and target directories */
  if ((rc = GetDir(sdir, SP->exclude, SP->skip, &sP)) != 0)
    return rc;
  if ((rc = GetDir(tdir, NULL, SP->skip, &tP)) != 0) {
    FreeDir(sP);
    return rc;
  }

  /* append "/" to source and target directory names */
  fsP = sdir + strlen(sdir);
  ftP = tdir + strlen(tdir);
  *fsP++ = '/';
  *ftP++ = '/';

  /* first delete files in target dir that don't exist in source dir */
  seP = sP;
  teP = tP;
  while (teP) {
    /* compare directory entries */
    rc = (seP != NULL)? strcmp(seP->name, teP->name): 1;

    if ((seP != NULL) && seP->exclude) {
      /* skip excluded files */
      seP = seP->next;

    } else if (rc == 0) {
      /* file exists in both directories */
      seP->teP = teP;
      seP = seP->next;
      teP = teP->next;

    } else if (rc < 0) {
      /* source file not in target directory */
      seP = seP->next;

    } else { /* rc > 0 */
      /* target file not in source directory */
      if (!SP->dontdelete && !teP->skip) {
        strcpy(ftP, teP->name);
        if (doUnlink(tdir, teP, 1) < 0) {
          FreeDir(sP);
          FreeDir(tP);
          return -1;
        }
      }
      teP = teP->next;
    }
  }

  /* re-sort source dir so that regular files are processed before sub dirs */
  sP = SortDir(sP, DirCmp2);

  /* process all entires found in the source directory */
  for (seP = sP, rc = 0; seP && rc >= 0; seP = seP->next) {
    /* skip error entries */
    if (seP->rc)
      continue;

    /* skip excluded and to-be-skipped files */
    if (seP->exclude || seP->skip)
      continue;

    /* also skip sub directories if "don't recur" option is set */
    if (SP->dontrecur && (seP->ftype & ft_isdir))
      continue;

    /* construct source and target file names */
    strcpy(fsP, seP->name);
    strcpy(ftP, seP->name);

    /* sync directory item */
    rc = SyncItem(sdir, seP, tdir, seP->teP);
  }

  /* free directory lists */
  FreeDir(sP);
  FreeDir(tP);

  return (rc < 0)? -1: 0;
}


/*--------------------------------------------------------------------
 * external entry point
 *------------------------------------------------------------------*/

void
SyncDirectory(SyncParams *p)
{
  char sbuf[1024], tbuf[1024];
  DirEntry *sP, *tP;
  FileOp op;
  int rc;

  /* the lower-level error and progress callbacks are not used */
  errorCallback = NULL;
  progressCallback = NULL;

  /* save pointer to params; copy source & target names */
  SP = p;
  snprintf(sbuf, sizeof(sbuf), "%s", SP->source);
  snprintf(tbuf, sizeof(tbuf), "%s", SP->target);

  /* get info about the source */
  sP = GetDirEntry(sbuf, &op, &rc);
  if (rc) {
    if (syncErrorCallback)
      syncErrorCallback(op, sbuf, rc);
    FreeDir(sP);
    return;
  }

  /*
   * The "copytop" option means: if the top-level source is a symbolic
   * make a copy of the real thing.  To get this behavior we just switch
   * off the ft_islnk flag on the source.
   */
  if (SP->copytop)
    sP->ftype &= ~ft_islnk;

  /* get info about the target */
  tP = GetDirEntry(tbuf, &op, &rc);
  if (rc) {
    if (op == op_lstat && rc == ENOENT) {
      /* target does not yet exist; that's ok */
      FreeDir(tP);
      tP = NULL;

    } else {
      /* we'll try to procede anyway ... unless the error callback says no */
      if (syncErrorCallback && syncErrorCallback(op, tbuf, rc) < 0) {
        FreeDir(sP);
        FreeDir(tP);
        return;
      }
    }
  }

  /* now do the real work */
  SyncItem(sbuf, sP, tbuf, tP);

  /* free storage and return */
  FreeDir(sP);
  FreeDir(tP);
}

