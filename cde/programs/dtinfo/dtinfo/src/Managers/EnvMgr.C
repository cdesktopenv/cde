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
/*
 * $TOG: EnvMgr.C /main/18 1998/04/17 11:52:44 mgreess $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 *
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 *
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 *
 */

#include <string.h>
#include <iostream>
using namespace std;
#include <sys/stat.h>

#include <lib/DtSvc/DtUtil2/LocaleXlate.h>

// xList, NodeViewInfo, NodeWindowAgent & NodeMgr used only for
// getting sections from NodeMgr

#define C_NodeViewInfo
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_HelpAgent
#define C_NodeWindowAgent
#define L_Agents

#define C_xList
#define L_Support

#define C_EnvMgr
#define C_MessageMgr
#define C_NodeMgr
#ifdef UseSessionMgmt
#define C_SessionMgr
#endif
#define C_GraphicsMgr
#define L_Managers

#include "Managers/CatMgr.hh"
#include "Prelude.h"

LONG_LIVED_CC(EnvMgr, env);

// a legacy global moved from WindowSystemMotif.C
bool g_debug;

// infolib string-list separator
#define ILSEP	':'
// section string-list separator
#define SLSEP	','


EnvMgr::EnvMgr() : f_argv(NULL),
                   f_argc(0),
		   f_lang(NULL),
                   f_secondary(False),
                   f_verbose(False),
                   f_debug(False)
{
  const char* lang;
  if ((lang = getenv("LC_ALL")) == NULL)
    if ((lang = getenv("LC_CTYPE")) == NULL)
      if ((lang = getenv("LANG")) == NULL)
	lang = "C";

  _DtXlateDb db = NULL;
  char platform[_DtPLATFORM_MAX_LEN + 1];
  int execver, compver;

  if (_DtLcxOpenAllDbs(&db) == 0)
  {
    if (_DtXlateGetXlateEnv(db, platform, &execver, &compver) == 0) {

      char* std_locale = NULL;

      _DtLcxXlateOpToStd(db, platform, compver, DtLCX_OPER_SETLOCALE,
					lang, &std_locale, NULL, NULL, NULL);
      if (std_locale) {
#ifdef LCX_DEBUG
	fprintf(stderr, "(DEBUG) standard locale=\"%s\"\n", std_locale);
#endif
	f_lang = XtNewString(std_locale);
	free(std_locale);
      }
    }

    _DtLcxCloseDb(&db);
    db = NULL;
  }

  // If OpToStd conversion failed, use non-std name
  if (f_lang == NULL)
    f_lang = XtNewString(lang);

  // tell mmdb info_lib to load info_base only if it matches to f_lang
  static char mmdb_lang[_POSIX_PATH_MAX];
  sprintf(mmdb_lang, "MMDB_LANG=%s", f_lang);
  putenv (mmdb_lang);

  f_home = XtNewString( getenv("HOME") );

  char dirname[256];
  sprintf (dirname, "%s/.dt/dtinfo/%s", f_home, f_lang);
  f_user_path = XtNewString(dirname);


  // if $HOME/.dt/dtinfo/$LANG does not exist, create it,
  // display auto help.
  if(!check_user_path())
  {
    create_user_path();
    help_agent().display_help ((char*)"doc_list_help");
  }

  UAS_Collection::request(
        (UAS_Receiver<UAS_CollectionRetrievedMsg> *)this) ;
  UAS_Common::request(
        (UAS_Receiver<UAS_LibraryDestroyedMsg> *)this) ;
}

EnvMgr::~EnvMgr()
{
    // free our generated/abbreviated arguments array
    for (int i=0; i < f_argc; i++)  XtFree(f_argv[i]);
    XtFree((char*)f_argv);
}


// init()
//
// initialize the environment class
//
// place as much as possible in the constructor for the class,
// reserving for here those things that have window system dependencies.
// parse_cmdline is here only because it might be re-written to use
// Xrm Options handling, and because there is a use of XtResolvePathname
// for one arg which needs the "display" value...
//
int
EnvMgr::init(int argc_i, char** argv_i)
{
  if( parse_cmdline(argc_i, argv_i) >= 0)
  {
    // See if any infolibs entered on command line. If not,
    // check environment for the default infolib.
    //
    if (f_infolibsStr.length() == 0)
    {
      char *tmp = infolibDefault();

      if( !(tmp && *tmp) )
      {
         // finally, try back door, which works better for non-CDE users
         // displaying dtinfo from a CDE system to a different desktop,
         // since all the Dt searchpath stuff won't be much help...
         const char *directpath = getenv ("DTINFO_INFOLIB_PATH");

         if (directpath != NULL && isInfolib(directpath))
         {
           f_infolibsStr = directpath ;	// UAS_String promotion
         }
         else
         {
            // invalid infolib--display error message
            message_mgr().error_dialog (
                 (char*)UAS_String(CATGETS(Set_AddLibraryAgent, 7,
                 "No infolibs found or specified.")));
            return -1;
         }
      }
      else f_infolibsStr = tmp;

      XtFree(tmp);
    }
  }
  else
  {
    usage();
    return -1;
  }
  {
    // link up indirect paths to mmdb code... ;-)
    static char buffer[256];
    sprintf (buffer, "MMDB_PATH=%s", (char *)f_infolibsStr );
    putenv (buffer);

    char *where = getenv ("DTINFO_MARKSPECPATH");
    if (where == NULL)
    {
       static char markref[256];
       sprintf (markref, "DTINFO_MARKSPECPATH=%s", "/usr/dt/infolib/etc" );
       putenv (markref);
    }
  }
  if (f_sectionsArg.length() == 0)
  {
    const char *locator = getenv ("DTINFO_FIRSTDOC");
    if (locator != NULL)
    {
       f_sectionsArg = UAS_String(locator) ;	// UAS_String promotion
    }
  }

  if (f_verbose) echoEnvState();
  return 1;
}


// infolibStringToList()
//
// parse a string of colon-separated infolib paths and return as a list.
//
UAS_List<UAS_String>
EnvMgr::infolibStringToList( const UAS_String & s_infolibs )
{
  return s_infolibs.splitFields (ILSEP);
}


UAS_List<UAS_String>
EnvMgr::sectionStringToList( const UAS_String & s_sections )
{
  // ... currently only handles a comma-separated list
  // ... for section "range" specifiers, need to make second pass
  //     on results of following, which will require some kind of
  //     dive into the UAS to interpret what a "range" of sections
  //     means, at the moment. This was to only apply with "-print",
  //     though.
  return s_sections.splitFields (SLSEP);

//   UAS_List initial_cut = s_sections.splitFields (SLSEP);
//   UAS_String  doc_locator, start_doc, end_doc ;
// 
//   for (int i = 0; i < docs.length(); i++)
//   {
//     (*(initial_cut[i])).split( '-', start_doc, end_doc ) ;
//     initial_cut.insert( start_doc.length() ? start_doc : end_doc ) ;
//     ... duely process rest of any range here to build "expanded_list" ...
//   }
//   return expanded_list;
}


// parse_cmdline()
//
// Extracts info from command line and sets up internal flags.
// -1 returned on error. Innocuous problems ignored, like if no arg
// provided where we might have a default or environment variable
// specified. Caller should do "usage()" on error return, as this
// method flags rather than performs it. 
//
int
EnvMgr::parse_cmdline( int     argc_i,
                       char ** argv_i )
{
    for (int i = 1; i < argc_i; i++)
    {	
      if(strcmp(argv_i[i], "-help") == 0 ||
         strcmp(argv_i[i], "-h") == 0)
      {
	// Print a summary of the command's syntax.
        return -1;
      }
      else if (strcmp(argv_i[i], "-l") == 0 ||
               strcmp(argv_i[i], "-lib") == 0 )
      {
        // get infolibs from command line
        i++;
        if( ( i < argc_i ) && ( argv_i[i][0] != '-' ) )
        {
          // This arg gives an absolute file path to an information
          // library, or the short name of the library (which will be
          // used for substitution into the search path specified by
          // DTINFOLIBSEARCHPATH in order to locate the infolib).
          // If the -l option is not provided, the browser displays
          // the default information library by path obtained from
          // substituting DTINFOLIBDEFAULT into DTINFOLIBSEARCHPATH.
          // [DTINFOLIBDEFAULT should be a single library short name.]
          // The "-l" option may be specified more than once.

          UAS_String pathname;

          if( !strchr( argv_i[i], ':' ) )
          {
            char *tmp = infolibStringToPath(argv_i[i]);
            if( tmp )
            {
              pathname = UAS_String(tmp);
              if( tmp != argv_i[i] )  XtFree(tmp);	// clean-up case
            }
          }
          else   // defer validation if arg contains colon separators
            pathname = UAS_String(argv_i[i]);  

          if( pathname.length() != 0 )
            if( f_infolibsStr.length() != 0 )
            {
              // need to concatenate
              f_infolibsStr = f_infolibsStr + ":" + pathname ;
            }
            else
            {
              // init string
              f_infolibsStr = pathname ;
            }
          else
          {
            // invalid infolib--display error message, but not fatal
            message_mgr().error_dialog (
                  (char*)UAS_String(CATGETS(Set_AddLibraryAgent, 5,
                  "Infolib specification format error.")));
          }
        }
      //else ... ignore this one
      }
      else if (strcmp(argv_i[i], "-sect") == 0 ||
               strcmp(argv_i[i], "-section") == 0 )
      {
        // get a list of sections from the command line
        i++;
        if( ( i < argc_i ) && ( argv_i[i][0] != '-' ) )
        {
	  // This arg specifies the infolib section (or a comma-separated
          // list of sections) via their unique locator IDs to either
          // display or print. 
	  // Sections can be specified using the hyphen character to
          // represent a range of sections, although it is up the print
          // interface to resolve what such a range means. Note that a
          // section "range" is only predictable within the same version
          // of an infolib from which it was mapped.
          // If the -print option is specified, the sections are printed;
          // otherwise, they are displayed (if a range is specified without
          // "-print", only the first ID in the range will be processed).
          // If a specified location ID is not at the top of a section,
          // the section containing the location is printed.

          f_sectionsArg = UAS_String(argv_i[i]);
          // (we actually build the sections "list" later, based
          //  on document windows actually displayed)
        }
      //else ... ignore this one
      }
      else if (strcmp(argv_i[i], "-secondary") == 0)
      {
        // Run this dtinfo instance without tooltalk session participation.
	// Secondary instances do not respond to ToolTalk messages. 
        f_secondary = True;
      }
      else if (strcmp(argv_i[i], "-verbose") == 0 ||
               strcmp(argv_i[i], "-v") == 0)
      {
        // make verbose a boolean flag
        f_verbose = True;
      }
#ifdef UseSessionMgmt
      else if (strcmp(argv_i[i], "-session") == 0)
      {
        // Get special session save-state file name
        // This option will usually result only from programmatic insertion
        // for purpose of session restart.
        i++;
        if(argv_i[i][0] != '-')
        {
          session().file( argv_i[i] ) ;
        }
      //else, ignore any errors for this arg, which is usually transparent
#endif
      }
      else if (strcmp(argv_i[i], "-Debug") == 0 ||
               strcmp(argv_i[i], "-D") == 0)
      {
        // Undocumented flag:  turn on debugging
        g_debug = TRUE;
        f_debug = TRUE;
        cerr << "\nInternal debugging activated." << endl;
      }
      else
      {
        cerr << CATGETS(Set_EnvMgr, 2, "Invalid argument") << endl;
        return -1;
      }
   }

   // do any argument consistency/cross-validation here

    // make sure that if print option was specified that some sections 
    // to print were also specified.
    if ( (window_system().videoShell()->print_only) && (f_sectionsArg == (UAS_String)NULL)) {
	cerr << CATGETS(Set_EnvMgr, 3, "ERROR: The -sect option must be specified with the -print option.") << endl;
	cerr << endl;
	usage();
	exit(1);
    }
   return 0;
}


void
EnvMgr::usage()
{
    cerr <<  CATGETS(Set_EnvMgr, 4, "Usage: dtinfo") << endl;
    cerr << "   [-help]" << endl;
    cerr << "   [-l infolib1] [-l infolib2] [...]" << endl;
    cerr << "   [-sect section[-section][,section[-section]]]" << endl;
    cerr << "   [-secondary]" << endl;
    cerr << "   [-verbose]" << endl;
    cerr << "   [[-print] [[-copies number]" << endl;
    cerr << "              [-hierarchy]" << endl;
    cerr << "              [-outputFile]" << endl;
    cerr << "              [-paperSize]" << endl;
    cerr << "              [-printer x_print_server]]]" << endl;
    cerr << endl;
    cerr << CATGETS(Set_EnvMgr, 5, 
		    "This application understands all standard X Toolkit command-line options.") << endl;
    
}


void
EnvMgr::echoEnvState()
{
  // need to display environment variables and
  // other information that would be useful
  cerr << endl;
  cerr << "   -l = " << (char*)f_infolibsStr << endl;
  cerr << "-sect = " << (char*)f_sectionsArg << endl;
  cerr << endl;
  if (getenv("DTINFOLIBSEARCHPATH") == NULL)
    cerr << "DTINFOLIBSEARCHPATH = " << endl;
  else
    cerr << "DTINFOLIBSEARCHPATH = " << getenv("DTINFOLIBSEARCHPATH") << endl;

  if (getenv("DTINFOLIBDEFAULT") == NULL)
    cerr << "   DTINFOLIBDEFAULT = " << endl;
  else
    cerr << "   DTINFOLIBDEFAULT = " << getenv("DTINFOLIBDEFAULT") << endl;

  if (getenv("DTHELPSEARCHPATH") == NULL)
    cerr << "   DTHELPSEARCHPATH = " << endl;
  else
    cerr << "   DTHELPSEARCHPATH = " << getenv("DTHELPSEARCHPATH") << endl;

  if (getenv("XUSERFILESEARCHPATH") == NULL)
    cerr << "XUSERFILESEARCHPATH = " << endl;
  else
    cerr << "XUSERFILESEARCHPATH = " << getenv("XUSERFILESEARCHPATH") << endl;

  if (getenv("LANG") == NULL)
    cerr << "               LANG = " << endl;
  else
    cerr << "               LANG = " << getenv("LANG") << endl;
  cerr << "          LANG(CDE) = " << lang() << endl;

  if(debug())
  {
    cerr << endl;
    if (getenv("XAPPLRESDIR") == NULL)
      cerr << "        XAPPLRESDIR = " << endl;
    else
      cerr << "        XAPPLRESDIR = " << getenv("XAPPLRESDIR") << endl;

    if (getenv("XFILESEARCHPATH") == NULL)
      cerr << "    XFILESEARCHPATH = " << endl;
    else
      cerr << "    XFILESEARCHPATH = " << getenv("XFILESEARCHPATH") << endl;

    if (getenv("NLSPATH") == NULL)
      cerr << "            NLSPATH = " << endl;
    else
      cerr << "            NLSPATH = " << getenv("NLSPATH") << endl;
  }
}


// isDir()
//
// Function to pass to the XtResolvePathname call in place of
// the default predicate, which does not allow for directories.
//
static bool
isDir(char* dirname)
{
  struct stat status;

  // just check to see if it's a directory
  if(access(dirname, R_OK) == 0)
  {
    stat(dirname, &status);
    return (S_ISDIR(status.st_mode));
  }
 else
    return False;
}


// isInfolib()
//
// determine whether an [assumed] absolute path specification
// matches basic file layout criteria for an infolib installation.
// (ignores search engine and index files, which would be more
// appropriately tested by a separate function, according to the
// search engine type)
//
bool
EnvMgr::isInfolib( const char* path )
{
    if (path == NULL || *path == '\0')
        return False;

    struct stat stat_buf;
    if (stat(path, &stat_buf) < 0)
        return False;

    if ((stat_buf.st_mode & S_IFMT & S_IFDIR) == 0)
        return False;

    if (access((char*)path, R_OK) < 0)
        return False;

    UAS_String map = path;
    map = map + "/" + "bookcase.map";

    if (access((char*)map, R_OK) < 0)
        return False;

    return True;
}


// get the default infolib per the environemnt variables
// DTINFOLIBDEFAULT and DTINFOLIBSEARCHPATH.
// Neither are tested for validity, which is assumed.
//
// Caller must free any returned string with XtFree()
//
char*
EnvMgr::infolibDefault()
{
   return infolibNameToPath( getenv("DTINFOLIBDEFAULT") );
}


// infolibNameToPath()
//
// Take the "short name" of an infolib (like "cde" in its dir name
// "cde.dti", and return the full path to library, if found in the
// default infolib search path set. Else, return NULL for no match.
//
// Caller must free any returned string with XtFree(),
// per XtResolvePathName
//
char*
EnvMgr::infolibNameToPath(char* path)
{
  SubstitutionRec subs[1];
  char* FileName;

  subs[0].match = 'I';
  subs[0].substitution = path;
  FileName = XtResolvePathname(
            window_system().display(),
            NULL, NULL, NULL,
            getenv("DTINFOLIBSEARCHPATH"),
            subs, XtNumber(subs),
            (XtFilePredicate)isDir);

  return FileName;
}


// infolibPathIsLocale()
//
// Attempt to match the immediate parent dir to the current locale or
// cde-normalized locale.
// Return false if no match, but caller must realize that this alone is
// inconclusive evidence.
bool
EnvMgr::infolibPathIsLocale(char* ilpath)
{
  // ... implement check of path head
  return False ;
}


// infolibStringToPath()
//
// Takes string representing a single infolib and attempts to convert
// it to an absolute infolib file path.
//
// Uses function isInfolib() for testing whether a result actually
// meets infolib criteria.
//
// Caller should test for NULL since valid infolib may not be found.
// Caller should free the result with XtFree() if returned pointer is
// not null AND not equal to the pointer supplied.
//
char*
EnvMgr::infolibStringToPath(char* path)
{
  char* filename;

  if( *path == '/' && isDir(path) )
  {
    filename = path ;		// assume input already absolute path
  }
  else if ( strchr( path, '/' ) )
  {
    // relative path... could get curr dir and build an absolute path
    filename = NULL ;
  }
  else		// assume a name and do path look-up
  {
    filename = infolibNameToPath(path) ;
  }

  if( !isInfolib( filename ) ) filename = NULL ;

  return filename;
}


// UAS_DocumentRetrievedMsg message handler
// ...
// void
// EnvMgr::receive (UAS_DocumentRetrievedMsg &msg,
//               void *	client_data)
// {
  // manipulate msg.fDoc ==> UAS_Pointer<UAS_Common> type
// }


// UAS_CollectionRetrievedMsg message handler
void
EnvMgr::receive (UAS_CollectionRetrievedMsg &msg,
                 void *	client_data)
{
  infolibAdd( fqlToFilePath(msg.fCol->root()->locator()) );
}


// UAS_LibraryDestroyedMsg message handler
void
EnvMgr::receive (UAS_LibraryDestroyedMsg &msg,
              void *    client_data)
{
  infolibRemove( fqlToFilePath(msg.fLib->locator()) );
}


// infolibAdd()
//
// Maintains state of browser in terms of infolibs actually loaded.
// The intent is for this function to be called only if the infolib
// has been successfully loaded (and thus appears in the booklist)
//
void
EnvMgr::infolibAdd( UAS_String pathname )
{
  if ( f_infolibsStr.length() != 0 )
  {
    // this is the init case, where the first infolib has actually been
    // loaded successfully by the browser. The init str is now superfluous.
    f_infolibsStr = "" ;
  }
  f_infolibs.insert_item(new UAS_String(pathname));
}


// infolibRemove()
//
// Maintains state of browser in terms of infolibs actually loaded.
// The intent is for this function to be called only if the infolib
// has been removed, once previously and successfully loaded
//
void
EnvMgr::infolibRemove( UAS_String pathname )
{
  if ( f_infolibsStr.length() != 0 )
  {
    // this entry case should never occur, so the remove is ignored
    cerr << "Invalid argument" << endl;
  }
  f_infolibs.remove_item(new UAS_String(pathname));
}


// infolibs()
//
// Return a list of infolibs known to browser.
// Any call to this function prior to the first infolib actually being
// loaded will get the list of infolibs initially requested at startup.
// Subsequent calls will be returned a list of the infolibs loaded at
// that time.
//
// The list entries are in the form of absolute path (as resolved and
// validated) to the infolib, and will be used for any session restart
// states saved with XSetCommand, as "-l" options to dtinfo.
//
// Prerequisite: command line args must have been processed (which
// will always have been the case for any call, since EnvMgr is "LongLived").
// 
UAS_List<UAS_String>
EnvMgr::infolibs()
{
  // after initialization, there is always at least one infolib loaded
  if( infolibCount() == 0 )
    return infolibStringToList( f_infolibsStr ) ;
  else
    return f_infolibs;
}


// get current infolib list as arg value string
//
UAS_String
EnvMgr::infolibsArg()
{
  if( ( infolibCount() == 0 ) && ( f_infolibsStr.length() != 0 ) )
  {
    return f_infolibsStr ;
  }
  else		// convert current list of sections to an arg
  {
    return cvtListToString( f_infolibs, ":", True ) ;
  }
}


// sections()
//
// Return a list of sections/nodes currently on display by the browser.
// Any call to this function prior to the first section actually displayed
// will get the list of sections initially requested at startup.
// Subsequent calls will be returned a list of the sections loaded at
// that time.
//
// The list entries are in the form of vaildated unique ids, and
// will be used for any session restart states saved with XSetCommand,
// as the argument to the "-sect" option to dtinfo.
//
// Prerequisite: command line args must have been processed (which
// will always have been the case for any call, since Env is "LongLived").
// 
UAS_List<UAS_String>
EnvMgr::sections()
{
  // check infolibCount to determine if in post-init phase...
  if( infolibCount() == 0 )
    return sectionStringToList( f_sectionsArg ) ;
  else					// post-init phase
  {
    return getCurrentSections() ;
  }
}


// get current section list as arg string
//
UAS_String
EnvMgr::sectionsArg()
{
  if( infolibCount() == 0 )
    return f_sectionsArg;
  else			// convert current list of sections to an arg
    return cvtListToString( getCurrentSections(), ",", True ) ;
}


// getCurrentSections()
//
// get the sections actually on display [right now] in all known
// node view windows, in their unique id format (i.e., not fully-qualified)
//
UAS_List<UAS_String>
EnvMgr::getCurrentSections()
{
  UAS_List<UAS_String> f_sections ;

  // get current section IDs from each node view visible
  xList<NodeWindowAgent *> &nwl = node_mgr().windows() ;

  List_Iterator<NodeWindowAgent *> nwli (nwl) ;
  for ( ; nwli; nwli ++)
  {
    f_sections.insert_item(
         new UAS_String(nwli.item()->node_view_info()->node_ptr()->id()) ) ;
  }
  return f_sections ;
}


// fqlToFilePath()
//
// convert a fully qualified locator to the infolib's absolute path,
// as resolved during the load phase. This function is expecting only
// fully qualified forms which include the "INFOLIB=<path>" part, such
// as are returned by the UAS_Common method "locator()", and serves to
// extract the "path".
//
// Caution: An fql does not have to have the "INFOLIB=" part...
//          Could add insurance check for that here to be more robust.
//
UAS_String
EnvMgr::fqlToFilePath( UAS_String fulloc )
{
  UAS_String discard;

  fulloc.split ('=', discard, fulloc);
  fulloc.split ('&', fulloc, discard);
  return fulloc ;
}


// arglist()
//
// construct and return an arglist which could restart the browser
// in its current state.
//
const char**
EnvMgr::arglist()
{
  char	*tmp ;
  int p_argc = 5 ;	// always have an arg0, a "-l <path>", and a
			// "-sect <list>" (even though list may be empty)

  if( !f_argv )
  {
    // augment count for any other necessary options here
    if( secondary() )  ++p_argc ;

    f_argv = (char **)XtMalloc( p_argc*sizeof(char *) ) ;

    int ia = 0 ;
    f_argv[ia++] = XtNewString( "dtinfo" ) ;
    if( secondary() )
    {
      f_argv[ia++] = XtNewString( "-secondary" ) ;
    }

    f_argv[ia++] = XtNewString( "-l" ) ;
    tmp = (char *)(infolibsArg()) ;
    f_argv[ia++] = XtNewString( tmp ) ;

    // always insert the -sect key and its arg, even if the arg
    // is zero length. This saves us from any re-malloc and copy.
    f_argv[ia++] = XtNewString( "-sect" ) ;
    tmp = (char *)(sectionsArg()) ;	// can be empty here
    f_argv[ia++] = XtNewString( tmp ) ;

    f_argc = p_argc ;
  }
  else
  {
    int il=0, is=0 ;

    // free then reset the infolibs/sections arg values per current state
    for( int i=1; i < f_argc && (il == 0 || is == 0) ; i++ )
    {
      if( strcmp(f_argv[i], "-l") == 0 )
      {
        XtFree( f_argv[++i] ) ;
        tmp = (char *)(infolibsArg()) ;
        f_argv[i] = XtNewString( tmp ) ;
        il = i ;
      }
      else if( strcmp(f_argv[i], "-sect") == 0 )
      {
        XtFree( f_argv[++i] ) ;
        tmp = (char *)(sectionsArg()) ;
        f_argv[i] = XtNewString( tmp ) ;
        is = i ;
      }
    }
  }

  return (const char **)f_argv ;
}


const char*
EnvMgr::home()
{
  return f_home;
}

const char*
EnvMgr::lang()
{
  return f_lang;
}

bool
EnvMgr::mkdirs(char *pathname)
{
  char buffer[256];
  char *c;
  strcpy(buffer, pathname);

  if(mkdir(buffer, 0777) == -1)
  {
    c = strrchr(buffer, '/');
    if (c != NULL)
      *c = 0;
    (void)mkdirs(buffer);
    if (c != NULL)
      *c = '/';
    if(mkdir(buffer, 0777) == -1)
      return False;
  }
  return True;
}

bool
EnvMgr::check_user_path()
{
  struct stat file_info;

  int status = stat (f_user_path, &file_info);
  if (status == -1)
    return False; 

  if (!S_ISDIR (file_info.st_mode))
    throw (CASTEXCEPT Exception());

  return True;
}

void
EnvMgr::create_user_path()
{
  if (mkdirs (f_user_path) == False)
    throw (CASTEXCEPT Exception());
}

const char*
EnvMgr::user_path()
{
  if(!check_user_path())
    create_user_path();
  return f_user_path;
}


// each element of theList is concatenated into a single string using
// the "delimiter" character for separation. Delimiter is treated as
// a prefix if "infix" is False, in which case it initiates the string.
//
UAS_String
EnvMgr::cvtListToString( UAS_List<UAS_String> theList,
                         UAS_String           delimiter,
                         bool                 infix )
{
  UAS_String  aggregate ;

  int len = theList.length();
  if( len != 0 )
  {
    if( infix ) aggregate = *(theList[0]);
    else        aggregate = delimiter + *(theList[0]);  // prefix
    for (int i = 1; i < len; i++)
    {
      // concatenate each additional item using the separator
      aggregate = aggregate + delimiter + *(theList[i]) ;
    }
  }
  return aggregate ;
}

