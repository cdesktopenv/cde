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
/*
 *+SNOTICE
 *
 *      $TOG: DetermineFileLocality.C /main/9 1997/12/22 16:30:59 bill $
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *       
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#if defined(SVR4)
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/mnttab.h>
#include <sys/mntent.h>
#include <sys/param.h>
#endif

#include <DtMail/DtMail.hh>

/*
 * Returns: length of second argument if it is a prefix of the
 * first argument, otherwise zero.
 */
static int
preflen(char * str, char * pref)
{
  int len;

  assert(str != NULL);
  assert(pref != NULL);

  len = strlen(pref);
  if (strncmp(str, pref, len) == 0)
    return (len);
  return (0);
}

// DetermineFileLocality -- determine if specified path object is local
//		or remote to the current system
// Arguments:
//  const char * path	-- -> path to object whose locality is to be determined
// Outputs:
//  None.
// Returns:
//  (DtmFileLocality) -- determined locality of the specified path object:
//	FL_UNKNOWN	-- locality of object cannot be determined
//	FL_LOCAL	-- the path object is local to this system
//	FL_REMOTE	-- the path object is remote to this system
//			-- (such as an NFS mounted file system object)
//	FL_LOCAL_AND_REMOTE -- the path object is locally available, yet
//			-- it resides remotely (such as a CACHEFS object)
//

enum DtmFileLocality
DtMail::DetermineFileLocality(const char * path)
{
#if defined(SVR4)
  FILE *mfp;
  struct mnttab *mnt;
  struct mnttab sysvmnt;
  int fsTypeLen = 0;
  int len;
  char realPath[MAXPATHLEN];		// resolved symbolic link
  char linkname[MAXPATHLEN];		// for symbolic link chasing
  char fsType[MAXPATHLEN];		// fs type for name put here
  struct statvfs statbuf;
  int statvfs_successful = 0;


  // Make sure the inputs to this function are correct
  //
  assert(path);			// path must be specified
  assert(strlen(path));		// cannot have a zero length


  // Resolve all symbolic links in path so that we have a handle
  // on the name of the actual file to check up on
  //
  (void) strncpy(realPath, path, MAXPATHLEN-1);
  while (1) {
    if ((len = readlink(realPath, linkname, sizeof (linkname)-1)) < 0)
	break;
    linkname[len] = '\0';
    (void) strncpy(realPath, linkname, MAXPATHLEN-1);
  }

  // Try and determine where this file is located relative to here
  //

  for (int i = 0; ; i++)
  {
    fsTypeLen = 0;

    switch (i)
    {
    case 0:			// first instance -- try statvfs	    
      statbuf.f_files = 0;
      if (statvfs (realPath, &statbuf) == -1)
	  continue;
      statvfs_successful++;
      fsTypeLen = strlen(statbuf.f_basetype);
      (void) strncpy(fsType, statbuf.f_basetype, MAXPATHLEN-1);
      break;

    case 1:			// second instance -- scan mount table
      if ( (mfp = fopen(MNTTAB, "r")) == NULL )
	  continue;
      mnt = &sysvmnt;
      while ((getmntent(mfp, mnt)) >= 0) {
	len = preflen(realPath, mnt->mnt_mountp);
	if (len >= fsTypeLen) {
	  fsTypeLen = len;
	  (void) strncpy(
			fsType,
			mnt->mnt_fstype ? mnt->mnt_fstype : "unknown-fstype",
			MAXPATHLEN-1);
	}
      }
      fclose(mfp);
      break;

    default:			// run out of major options
      if (statvfs_successful)	// if statvfs successful, infer results
	return(statbuf.f_files == -1 ? Dtm_FL_REMOTE : Dtm_FL_LOCAL);
      return(Dtm_FL_UNKNOWN);
    }

    // At this point, if fsTypeLen > 0 then we have found a match.
    // Return the proper Dtm_FL_xxx code according to the type of file system
    // path object is located on
    //
    if (fsTypeLen == 0)			// nothing found??
      continue;				// nope - try next pass
    if (!strcmp(fsType, "cachefs"))	// cached from anonymous source?
      return(Dtm_FL_LOCAL_AND_REMOTE);	// yes: file is local and remote
    else if (!strcmp(fsType, "nfs"))	// nfs resource?
      return (Dtm_FL_REMOTE);		// yes: file is remote only
    else if (!strcmp(fsType, "ufs"))	// ufs type file system?
      return (Dtm_FL_LOCAL);		// yes: file is local only
    else if (!strcmp(fsType, "tmpfs"))	// tmpfs local but transient file?
      return (Dtm_FL_LOCAL);		// yes: file is local
    else if (!strcmp(fsType, "autofs"))	// automounter mounting point??
      return (Dtm_FL_REMOTE);		// yes: file is remote

    // no type that we recognize - try next pass
    //
  }
#else
  int len = (int) strlen(path);
  return(Dtm_FL_UNKNOWN);
#endif
}
