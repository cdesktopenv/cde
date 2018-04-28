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
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

/*****************************************************************************
 *  File:              dtsearchpath
 *
 *  Purpose:           Set up the application search paths for 
 *                     the desktop components.
 *
 *  Description:       This client is invoked by the Xsession file
 *                     at user login.  It sets the local and remote locations
 *                     that the desktop components should look, for application
 *                     icons, help files, and other application configuration
 *                     information on behalf of the user.
 *
 *  Invoked by:        Xsession after the scripts in /etc/dt/Xsession.d
 *                     are sourced in.
 *
 *  Product:           @(#)Common Desktop Environment 1.0          
 *
 *  Revision:          $XConsortium: dtsearchpath.C /main/9 1996/07/10 16:38:27 rswiston $
 *
 ****************************************************************************/

/****************************************************************************
 *
 *  DTMOUNTPOINT 
 *     Description:  The directory where this system mounts remote discs.
 *                   Used for constructing pathnames to remote files, for
 *                   systems that are not using a global file name space.
 *     Default:      Vendor specific:  /nfs
 *
 *  DTAPPSEARCHPATH
 *     Description:  The directories where we will look for applications.
 *     Default:      $HOME/.dt/appmanager,
 *                   /etc/dt/appconfig/appmanager,
 *                   /usr/dt/appconfig/appmanager
 *
 *  DTDATABASESEARCHPATH
 *     Description:  The directories where we will look for actions/filetypes.
 *     Default:      Derived from DTAPPSEARCHPATH:
 *                       $HOME/.dt/types,
 *                       /etc/dt/appconfig/types/%L,
 *                       /usr/dt/appconfig/types/%L
 *
 *  DTHELPSEARCHPATH
 *     Description:  The directories where we will look for online help.
 *     Default:      Derived from $DTAPPSEARCHPATH:
 *                       $HOME/.dt/help/[%H | %H.hv]
 *                       /etc/dt/appconfig/help/%L/[%H | %H.sdl | %H.hv],
 *                       /usr/dt/appconfig/help/%L/[%H | %H.sdl | %H.hv]
 *
 *  DTINFOLIBSEARCHPATH
 *     Description:  The directories where we will look for online information.
 *     Default:      Derived from $DTAPPSEARCHPATH:
 *                       $HOME/.dt/infolib/%L/%I.dti,
 *                       /etc/dt/appconfig/infolib/%L/%I.dti,
 *                       /usr/dt/appconfig/infolib/%L/%I.dti
 *
 *  DTICONSEARCHPATH
 *     Description:  The directories where we will look for icons.
 *     Default:      Derived from $DTAPPSEARCHPATH:
 *                       $HOME/.dt/icons/%B,
 *                       /etc/dt/appconfig/icons/%L/[%B | %B.pm | %B.bm]
 *                       /usr/dt/appconfig/icons/%L/[%B | %B.pm | %B.bm]
 ****************************************************************************/

/****************************************************************************
 *    If you specify a value for one of the search paths, then that value
 *    will be added to the default search path after the sys-admin config-
 *    uration location but before the factory-default location.  For ex-
 *    ample, if you specify a value for the DTSPSYSAPPHOSTS of /foo, then
 *    the search path will be:
 *
 *      $HOME/.dt:/etc/dt/appconfig:/foo:/usr/dt/appconfig
 *
 *    Remember that you can also set the USER equivalents of these
 *    search paths.  They are useful for specifying locations other
 *    than the defaults.   Their names are:
 *
 *  DTSPUSERAPPHOSTS
 *     Default:  <null>
 *
 *  DTSPUSERDATABASEHOSTS
 *     Default:  <null>
 *
 *  DTSPUSERHELP
 *     Default:  <null>
 *
 *  DTSPUSERINFOLIB
 *     Default:  <null>
 *
 *  DTSPUSERICON
 *     Default:  <null>
 *
 *    These values will be prepended to the system paths, and hence will
 *    will take precedence.
 *
 **************************************************************************/

#include <stdlib.h>
#include "SearchPath.h"
#include "Options.h"
#include "Environ.h"

const char * DTINFOLIBDEFAULT_NAME = "DTINFOLIBDEFAULT";
const char * DTINFOLIBDEFAULT_VALUE = "cde";

Options * options;

int main (int argc, char **argv)
{
  CString newShell("SHELL="); // Keep in scope for duration of program.

  options = new Options(argc, argv);

  if ( options->useKshEnv() ) {
    newShell += UnixEnvironment::kshString();
    putenv(newShell.data());
  }
  else if ( options->useCshEnv() ) {
    newShell += UnixEnvironment::cshString();
    putenv(newShell.data());
  }

  UnixEnvironment * os = new UnixEnvironment;

  CDEEnvironment * user = new CDEEnvironment(options->getHomeDir(), os);
  user->CreateHomeAppconfigDir();

  if (options->removeAutoMountPoint())
    options->setAutoMountPoint(os->getEnvironmentVariable("DTAUTOMOUNTPOINT"));

  InfoLibSearchPath infolib (user);

  AppSearchPath app (user);
  DatabaseSearchPath database (user);

  user->setDTAPPSP(app.GetSearchPath());

  IconSearchPath icon (user);
  IconSearchPath iconbm (user, "XMICONBM", ".bm", ".pm");

  HelpSearchPath help (user);

  ManSearchPath man(user);

  app.FixUp();

  if (options->Verbose() || options->CheckingUser()) {
    CString mp(user->OS()->MountPoint());
#ifdef IOSTREAMSWORKS
    cout << "DTMOUNTPOINT: " << mp << "\n" << endl;
    cout << app << endl;
    cout << database << endl;
    cout << icon << endl;
    cout << iconbm << endl;
    cout << help << endl;
    cout << infolib << endl;
    cout << DTINFOLIBDEFAULT_NAME << ": " << DTINFOLIBDEFAULT_VALUE << "\n" << endl;
#else
    printf("DTMOUNTPOINT: %s\n",mp.data());
    app.Print();
    database.Print();
    icon.Print();
    iconbm.Print();
    help.Print();
    infolib.Print();
    printf ("%s: %s\n", DTINFOLIBDEFAULT_NAME, DTINFOLIBDEFAULT_VALUE);
#endif
    man.Print();
  }

  if (!options->CheckingUser() && !options->Verbose()) {
    app.ExportPath();
    database.ExportPath();
    icon.ExportPath();
    iconbm.ExportPath();
    help.ExportPath();
    infolib.ExportPath();

#ifdef IOSTREAMSWORKS
    if ( os->cshFormat ) {
      cout << "setenv " << DTINFOLIBDEFAULT_NAME << " " << DTINFOLIBDEFAULT_VALUE << endl;
    } else {
      cout << DTINFOLIBDEFAULT_NAME << "=" << DTINFOLIBDEFAULT_VALUE << endl;
      cout << "export " << DTINFOLIBDEFAULT_NAME << ";" << endl;
    }
#else
    if ( os->cshFormat ) {
      printf("setenv %s %s;\n", DTINFOLIBDEFAULT_NAME, DTINFOLIBDEFAULT_VALUE);
    } else {
      printf("%s=%s;\n", DTINFOLIBDEFAULT_NAME, DTINFOLIBDEFAULT_VALUE);
      printf("export %s;\n", DTINFOLIBDEFAULT_NAME);
    }
#endif /* IOSTREAMSWORKS */

    man.ExportPath();

  }

  return 0;
}
