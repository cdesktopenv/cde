/* $XConsortium: Options.h /main/3 1995/11/03 12:32:15 rswiston $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "cstring.h"

class Options {
 public:
  Options (unsigned int, char **);
 ~Options ();

  int       Verbose () const             { return (flags & 1); }
  int       CheckingUser () const        { return (flags & 2); }
  int       doingTooltalk () const       { return !doingICCCM(); }
  int       doingICCCM () const          { return (flags & 4); }
  int       dontOptimize () const        { return (flags & 8); }
  int       removeAutoMountPoint() const { return (flags & 16); }
  int       useCshEnv() const            { return (flags & 32); }
  int       useKshEnv() const            { return (flags & 64); }

  CString   getAutoMountPoint () const   { return *automountpoint; }
  CString * getUserID () const           { return user_id; }
  CString * getHomeDir () const          { return home_dir; }

  void      setUserID (const CString &);
  void      setAutoMountPoint (const CString &);

 private:
  unsigned char flags;
  CString *     user_id;
  CString *     home_dir;
  CString *     automountpoint;
};

#endif
