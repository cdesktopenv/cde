/*
 * $XConsortium: EnvMgr.hh /main/11 1996/10/04 14:00:12 rcs $
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

#include "UAS.hh"


class EnvMgr : public Long_Lived,
               public UAS_Receiver<UAS_CollectionRetrievedMsg>,
               public UAS_Receiver<UAS_LibraryDestroyedMsg>
{
public:

  EnvMgr();
  ~EnvMgr();
  int    init( int argc, char **argv );

  UAS_List<UAS_String> sections();	// list of sections on display
  UAS_String           sectionsArg();	// current section list as arg string

  int    infolibCount()			// number of Loaded infolibs
		{ return f_infolibs.length(); } // post-init Only
  UAS_List<UAS_String> infolibs();	// list of paths to loaded infolibs
  UAS_String           infolibsArg();	// current infolib list as arg string
  char*  infolibDefault();		// resolve path to default infolib

           // infolib identification or path extraction methods

  char*  infolibNameToPath( char* il_base_name ); // find path for short name
  bool   infolibPathIsLocale( char* path );	// ... future
  char*  infolibStringToPath( char* may_be_il );  // rtrn full path for il arg

  UAS_String  fqlToFilePath( UAS_String path_qualified_locator ) ;
  UAS_List<UAS_String> infolibStringToList( const UAS_String & s_infolibs );
  static bool isInfolib( const char* path ) ;	// detail check


  const char**  arglist();		// "equiv" arg list for state
  int    arglistCount() { return f_argc; }

  const char*   lang() ;		// returns language for this instance 
  const char*   home() ;		// returns HOME for this instance 
  const char*   user_path() ;		// returns $HOME/.dt/dtinfo/$LANG path
  void   usage();
  bool   verbose() { return f_verbose; }	// True if want confirmations 
  bool   secondary() { return f_secondary; }	// True if no tooltalk
  bool   debug() { return f_debug; }		// True for extra info
  bool   autohelp() { return f_autohelp; }	// True for first time help

protected:

  UAS_List<UAS_String>	f_infolibs;	// list of infolib paths

  void   infolibAdd( UAS_String pathname );	// add to "infolibs"
  void   infolibRemove( UAS_String pathname );	// remove from "infolibs"

  UAS_List<UAS_String> sectionStringToList( const UAS_String & s_sections );
  UAS_List<UAS_String> getCurrentSections();	// get current doc locators


private:   // methods

  int    parse_cmdline( int argc_i,	// interpret arg values & set flags
                        char **argv_i);
  void   echoEnvState();		// if verbose, for environment
  UAS_String cvtListToString( UAS_List<UAS_String> slst,
                              UAS_String      delimiter,
                              bool            infix );

  void   receive( UAS_CollectionRetrievedMsg &msg, void* client_data );
  void   receive( UAS_LibraryDestroyedMsg &msg, void* client_data );

  bool   check_user_path();
  void   create_user_path();

  bool   mkdirs(char*); // recursively create directory path

private:   // data

  int                   f_argc;
  char                **f_argv;
  char                 *f_lang;
  char                 *f_home;
  char                 *f_user_path;

    // option flags and data
  UAS_String            f_infolibsStr;
  UAS_String            f_sectionsArg;
  UAS_String		f_first_doc;
  bool                  f_secondary;
  bool                  f_verbose;
  bool                  f_debug;
  bool                  f_autohelp;
    
private:
  LONG_LIVED_HH( EnvMgr, env );
};

LONG_LIVED_HH2( EnvMgr, env ) ;
