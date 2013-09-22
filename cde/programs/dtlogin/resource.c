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
/* $TOG: resource.c /main/7 1997/03/14 13:45:09 barstow $ */
/* (c) Copyright 1997 The Open Group */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 * xdm - display manager daemon
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/*
 * resource.c
 */

# include "dm.h"
# include "vgmsg.h"
# include <X11/Xresource.h>
# include <X11/Xmu/CharSet.h>
# include <string.h>


  void CleanUpName( char *src, char *dst, int len) ;
  static char * getFullFileName(char *name, int special, char * lang);
  extern char * qualifyWithFirst(char *, char *);

/* XtOffset() hack for ibmrt BandAidCompiler */

int session_set = False;
char	*config;

char	*servers;
int	request_port;
int	debugLevel;
char	*errorLogFile;
int	errorLogSize;
int	daemonMode;
int	quiet;
char	*pidFile;
int	lockPidFile;
char	*authDir;
int	autoRescan;
int	removeDomainname;
char	*keyFile;
char	*accessFile;
char    **exportList;    /* List of all export env vars */
char	*sysParmsFile;
char	*timeZone;
char	*fpHead = NULL;
char	*fpTail = NULL;
int	wakeupInterval;
int 	langListTimeout;
#ifdef DEF_NETWORK_DEV
char    *networkDev;
#endif
#if defined(__osf__)
char	*ignoreLocales;
#endif

/*
 * Dtlogin will accept both Dtlogin and XDM resources. The string variable
 * "AppName" contains the proper application name to use in looking up
 * resources.
 */

# define DISPLAYMANAGER	    "DisplayManager"
# define DTLOGIN	    "Dtlogin"

static	char	AppName[16] = DTLOGIN;


# define DM_STRING	0
# define DM_INT		1
# define DM_BOOL	2
# define DM_ARGV	3

#ifdef BLS
#  define DEF_XDM_CONFIG CDE_INSTALLATION_TOP "/lib/X11/Dtlogin/Xconfig"
#  define DEF_AUTH_DIR   CDE_INSTALLATION_TOP "/lib/X11/Dtlogin"
#  define DEF_KEY_FILE	 CDE_INSTALLATION_TOP "/lib/X11/Dtlogin/Xkeys"
#endif


/*
 * the following constants are supposed to be set in the makefile from
 * parameters set util/imake.includes/site.def (or *.macros in that directory
 * if it is server-specific).  DO NOT CHANGE THESE DEFINITIONS!
 */
#ifndef DEF_SERVER_LINE 
#  ifdef sun 
#define DEF_SERVER_LINE ":0 local /usr/openwin/bin/X :0"
#  else
#define DEF_SERVER_LINE ":0 local /usr/bin/X11/X :0"
#  endif /* sun */
#endif
#ifndef XRDB_PROGRAM
#  ifdef sun 
#define XRDB_PROGRAM "/usr/openwin/bin/xrdb"
#  else
#define XRDB_PROGRAM "/usr/bin/X11/xrdb"
#  endif /* sun */
#endif
#ifndef DEF_SESSION
#define DEF_SESSION CDE_INSTALLATION_TOP "/bin/Xsession"
#endif

#ifndef DEF_USER_PATH
#  ifdef sun 
#    define DEF_USER_PATH "/usr/openwin/bin:/bin:/usr/bin:/usr/contrib/bin:/usr/local/bin:."
#  else
#    define DEF_USER_PATH "/usr/bin/X11:/bin:/usr/bin:/usr/contrib/bin:/usr/local/bin"
#  endif /* sun */
#endif

#ifndef DEF_SYSTEM_PATH
#  ifdef sun 
#    define DEF_SYSTEM_PATH "/usr/openwin/bin:/etc:/bin:/usr/bin"
#  else
#    define DEF_SYSTEM_PATH "/usr/bin/X11:/etc:/bin:/usr/bin"
#  endif /* sun */
#endif

#ifndef DEF_SYSTEM_SHELL
#define DEF_SYSTEM_SHELL "/bin/sh"
#endif

#ifndef DEF_FAILSAFE_CLIENT
#  ifdef sun 
#define DEF_FAILSAFE_CLIENT "/usr/openwin/bin/xterm"
#  else
#define DEF_FAILSAFE_CLIENT "/usr/bin/X11/xterm"
#  endif /* sun */
#endif

#ifndef DEF_XDM_CONFIG
#define DEF_XDM_CONFIG "Xconfig"
#endif

#ifndef DEF_CHOOSER
#define DEF_CHOOSER CDE_INSTALLATION_TOP "/bin/chooser"
#endif

#ifndef CPP_PROGRAM
#define CPP_PROGRAM "/lib/cpp"
#endif
#ifndef DEF_XDM_AUTH_GEN
#define DEF_XDM_AUTH_GEN CDE_CONFIGURATION_TOP "/xdmauthgen"
#endif
#ifndef DEF_AUTH_NAME
#define DEF_AUTH_NAME	"MIT-MAGIC-COOKIE-1"
#endif
#ifndef DEF_AUTH_DIR
#define DEF_AUTH_DIR CDE_CONFIGURATION_TOP
#endif
#ifndef DEF_USER_AUTH_DIR
#define DEF_USER_AUTH_DIR	"/tmp"
#endif
#ifndef DEF_KEY_FILE
#define DEF_KEY_FILE	CDE_CONFIGURATION_TOP "/Xkeys"
#endif
#ifndef DEF_ACCESS_FILE
#define DEF_ACCESS_FILE	""
#endif
#ifndef DEF_TIMEZONE
#define DEF_TIMEZONE	"MST7MDT"
#endif
#ifndef DEF_SYS_PARMS_FILE
#  if defined( __hpux )
#    define DEF_SYS_PARMS_FILE	"/etc/src.sh"
#  else
#    define DEF_SYS_PARMS_FILE	"/etc/TIMEZONE"
#  endif
#endif
#define DEF_UDP_PORT	"177"	    /* registered XDMCP port, don't change */

#ifndef DEF_ENV
#  ifdef sun	/* need to set the environment for Sun OpenWindows */
#    define DEF_ENV	"OPENWINHOME=/usr/openwin"
#  else
#    define DEF_ENV	""
#  endif
#endif

#ifndef DEF_LANG	/* LANG default settings for various architectures */
#  ifdef __apollo
#    define DEF_LANG	"C"
#  endif
#  ifdef __osf__
#    define DEF_LANG    "C"
#  endif
#  ifdef __hp_osf
#    define DEF_LANG	"en_US.88591"
#  endif
#  ifdef sun	/* default language under Solaris */
#    define DEF_LANG    "C" 
#  endif
#endif

#ifndef DEF_LANG	/* set LANG to NULL if not already defined	   */
#define DEF_LANG	""
#endif

struct dmResources {
	char	*name, *class;
	int	type;
	char	**dm_value;
	char	*default_value;
} DmResources[] = {
"servers",	"Servers", 	DM_STRING,	&servers,
				DEF_SERVER_LINE,
"requestPort",	"RequestPort",	DM_INT,		(char **) &request_port,
				DEF_UDP_PORT,
"debugLevel",	"DebugLevel",	DM_INT,		(char **) &debugLevel,
				"0",
"errorLogFile",	"ErrorLogFile",	DM_STRING,	&errorLogFile,
				"",
"errorLogSize",	"ErrorLogSize",	DM_INT,		(char **) &errorLogSize,
				"50",
"daemonMode",	"DaemonMode",	DM_BOOL,	(char **) &daemonMode,
				"false",
"quiet",	"quiet",	DM_BOOL,	(char **) &quiet,
				"false",
"pidFile",	"PidFile",	DM_STRING,	&pidFile,
				"",
"lockPidFile",	"LockPidFile",	DM_BOOL,	(char **) &lockPidFile,
				"true",
"authDir",	"AuthDir",	DM_STRING,	&authDir,
				DEF_AUTH_DIR,
"autoRescan",	"AutoRescan",	DM_BOOL,	(char **) &autoRescan,
				"true",
"removeDomainname","RemoveDomainname",DM_BOOL,	(char **) &removeDomainname,
				"true",
"keyFile",	"KeyFile",	DM_STRING,	&keyFile,
				DEF_KEY_FILE,
"accessFile",	"AccessFile",	DM_STRING,	&accessFile,
				DEF_ACCESS_FILE,
/* exportList env var resource    RK   08.17.93  */
"exportList",   "ExportList",   DM_ARGV,        (char **) &exportList,
                                "",
"timeZone",	"TimeZone",	DM_STRING,	&timeZone,
				"",
"fontPathHead",	"FontPathHead",	DM_STRING,	&fpHead,
				"",
"fontPathTail",	"FontPathTail",	DM_STRING,	&fpTail,
				"",
"sysParmsFile",	"SysParmsFile",	DM_STRING,	&sysParmsFile,
				DEF_SYS_PARMS_FILE,
"wakeupInterval","WakeupInterval",DM_INT,	(char **) &wakeupInterval,
				"10",
"langListTimeout","langListTimeout",DM_INT,	(char **) &langListTimeout,
				"30",
#ifdef DEF_NETWORK_DEV
"networkDevice","NetworkDevice",DM_STRING,      &networkDev,
                                DEF_NETWORK_DEV,
#endif
#if defined(__osf__)
"ignoreLocales", "IgnoreLocales", DM_STRING,    &ignoreLocales,
				"",
#endif
};

# define NUM_DM_RESOURCES	(sizeof DmResources / sizeof DmResources[0])

# define boffset(f)	((char *) &(((struct display *) 0)->f) - (char *) 0)

struct displayResources {
	char	*name, *class;
	int	type;
	int	offset;
	char	*default_value;
} DisplayResources[] = {


/* 
 * resources for managing the server...
 */
 
"serverAttempts","ServerAttempts", DM_INT,	boffset(serverAttempts),
				"1",
"openDelay",	"OpenDelay",	DM_INT,		boffset(openDelay),
				"5",
"openRepeat",	"OpenRepeat",	DM_INT,		boffset(openRepeat),
				"5",
"openTimeout",	"OpenTimeout",	DM_INT,		boffset(openTimeout),
				"30",
"startAttempts","StartAttempts",DM_INT,		boffset(startAttempts),
				"4",
"pingInterval",	"PingInterval",	DM_INT,		boffset(pingInterval),
				"5",
"pingTimeout",	"PingTimeout",	DM_INT,		boffset(pingTimeout),
				"5",
"terminateServer","TerminateServer",DM_BOOL,	boffset(terminateServer),
				"false",
"grabServer",	"GrabServer",	DM_BOOL,	boffset(grabServer),
				"true",
"grabTimeout",	"GrabTimeout",	DM_INT,		boffset(grabTimeout),
				"3",
"resetSignal",	"Signal",	DM_INT,		boffset(resetSignal),
				"1",	/* SIGHUP */
"termSignal",	"Signal",	DM_INT,		boffset(termSignal),
				"15",	/* SIGTERM */
"resetForAuth",	"ResetForAuth",	DM_BOOL,	boffset(resetForAuth),
				"false",
"authorize",	"Authorize",	DM_BOOL,	boffset(authorize),
				"true",
"authName",	"AuthName",	DM_ARGV,        boffset(authNames),
				DEF_AUTH_NAME,
"authFile",	"AuthFile",	DM_STRING,	boffset(authFile),
				"",
#if 0
"gettyLine",	"GettyLine",	DM_STRING,	boffset(gettyLine),
				"",
"gettySpeed",	"GettySpeed",	DM_STRING,	boffset(gettySpeed),
				"9600",
#endif
/*
 *  resources which control the session behavior...
 */

"resources",	"Resources",	DM_STRING,	boffset(resources),
				"",
"xrdb",		"Xrdb",		DM_STRING,	boffset(xrdb),
				XRDB_PROGRAM,
"cpp",		"Cpp",		DM_STRING,	boffset(cpp),
				CPP_PROGRAM,
"setup",	"Setup",	DM_STRING,	boffset(setup),
				"",
"startup",	"Startup",	DM_STRING,	boffset(startup),
				"",
"reset",	"Reset",	DM_STRING,	boffset(reset),
				"",
"session",	"Session",	DM_STRING,	boffset(session),
				DEF_SESSION,
"userPath",	"Path",		DM_STRING,	boffset(userPath),
				DEF_USER_PATH,
"systemPath",	"Path",		DM_STRING,	boffset(systemPath),
				DEF_SYSTEM_PATH,
"systemShell",	"Shell",	DM_STRING,	boffset(systemShell),
				DEF_SYSTEM_SHELL,
"failsafeClient","FailsafeClient",	DM_STRING,	boffset(failsafeClient),
				DEF_FAILSAFE_CLIENT,
"userAuthDir",	"UserAuthDir",	DM_STRING,	boffset(userAuthDir),
				DEF_AUTH_DIR,
"chooser",      "Chooser",      DM_STRING,      boffset(chooser),
                                DEF_CHOOSER,
"language",	"Language",	DM_STRING,	boffset(language),
				DEF_LANG,
"languageList",	"LanguageList",	DM_STRING,	boffset(langList),
				"",
"environment",	"Environment",	DM_STRING,	boffset(environStr),
				DEF_ENV,
"dtlite",	"Dtlite",	DM_BOOL,	boffset(dtlite),
				"false",
"xdmMode",	"XdmMode",	DM_BOOL,	boffset(xdmMode),
				"false",
"authenticationName","AuthenticationName", DM_STRING,	boffset(verifyName),
				"",
"pmSearchPath", "PmSearchPath", DM_STRING,      boffset(pmSearchPath),
                                DEF_PM_SEARCH_PATH,
"bmSearchPath", "bmSearchPath", DM_STRING,      boffset(bmSearchPath), 
                                DEF_BM_SEARCH_PATH,
};

# define NUM_DISPLAY_RESOURCES	(sizeof DisplayResources/sizeof DisplayResources[0])

XrmDatabase	DmResourceDB;

int 
GetResource( char *name, char *class, int valueType, char **valuep,
	     char *default_value )
{
    char	*type;
    XrmValue	value;
    char	*string, *new_string;
    char	str_buf[50];
    int	len;


    if (DmResourceDB && XrmGetResource (DmResourceDB,
	name, class,
	&type, &value))
    {
	string = value.addr;
	len = value.size;
    }
    else
    {
	string = default_value;
	len = (string == NULL ? 0 : strlen (string));
    }

    Debug ("%s/%s value %*.*s\n", name, class, len, len, string);


    /*
     *  check if new resource is the same as old...
     */
     
     if (valueType == DM_STRING && *valuep) {
 	if (string != NULL && strlen (*valuep) == len && !strncmp (*valuep, string, len))
 	    return;
 	else {
 	    free (*valuep);
	    *(valuep) = NULL;
	}
     }
 

    switch (valueType) {
    case DM_STRING:
	if ( len > 0 ) {
	    new_string = malloc ((unsigned) (len+1));
	    if (!new_string) {
		LogOutOfMem(
		  ReadCatalog(MC_LOG_SET,MC_LOG_GET_RSC,MC_DEF_LOG_GET_RSC));
		return;
	    }
	    strncpy (new_string, string, len);
	    new_string[len] = '\0';
	    *(valuep) = new_string;
	}
	break;
    case DM_INT:
	strncpy (str_buf, string, sizeof (str_buf));
	str_buf[sizeof (str_buf)-1] = '\0';
	*((int *) valuep) = atoi (str_buf);
	break;
    case DM_BOOL:
	strncpy (str_buf, string, sizeof (str_buf));
	str_buf[sizeof (str_buf)-1] = '\0';
	XmuCopyISOLatin1Lowered (str_buf, str_buf);
	if (!strcmp (str_buf, "true") ||
	    !strcmp (str_buf, "on") ||
	    !strcmp (str_buf, "yes"))
		*((int *) valuep) = 1;
	else if (!strcmp (str_buf, "false") ||
		 !strcmp (str_buf, "off") ||
		 !strcmp (str_buf, "no"))
		*((int *) valuep) = 0;
	break;
    case DM_ARGV:
        *((char ***) valuep) = parseArgs ((char **) 0, string);
        break;

    }
}

XrmOptionDescRec configTable [] = {
{"-server",	NULL,			XrmoptionSkipArg,	(caddr_t) NULL },
{"-udpPort",	NULL,			XrmoptionSkipArg,	(caddr_t) NULL },
{"-error",	NULL,			XrmoptionSkipArg,	(caddr_t) NULL },
{"-resources",	NULL,			XrmoptionSkipArg,	(caddr_t) NULL },
{"-session",	NULL,			XrmoptionSkipArg,	(caddr_t) NULL },
{"-debug",	NULL,			XrmoptionSkipArg,	(caddr_t) NULL },
{"-xrm",	NULL,			XrmoptionSkipArg,	(caddr_t) NULL },
{"-config",	".configFile",		XrmoptionSepArg,	(caddr_t) NULL }
};

XrmOptionDescRec optionTable [] = {
{"-server",	".servers",		XrmoptionSepArg,	(caddr_t) NULL },
{"-udpPort",	".requestPort",		XrmoptionSepArg,	(caddr_t) NULL },
{"-error",	".errorLogFile",	XrmoptionSepArg,	(caddr_t) NULL },
{"-resources",	"*resources",		XrmoptionSepArg,	(caddr_t) NULL },
{"-session",	"*session",		XrmoptionSepArg,	(caddr_t) NULL },
{"-debug",	"*debugLevel",		XrmoptionSepArg,	(caddr_t) NULL },
{"-xrm",	NULL,			XrmoptionResArg,	(caddr_t) NULL },
{"-daemon",	".daemonMode",		XrmoptionNoArg,		"true"         },
{"-nodaemon",	".daemonMode",		XrmoptionNoArg,		"false"        },
{"-quiet",	".quiet",		XrmoptionNoArg,		"true"        }
};

static int	originalArgc;
static char	**originalArgv;

void 
InitResources( int argc, char **argv )
{
	XrmInitialize ();
	originalArgc = argc;
	originalArgv = argv;
	ReinitResources ();
}

void 
ReinitResources( void )
{
    int	argc;
    char	**a;
    char	**argv;
    XrmDatabase newDB;
    char * configFileName;
    char	*type;
    XrmValue	value;

    argv = (char **) malloc ((originalArgc + 1) * sizeof (char *));
    if (!argv)
	LogOutOfMem(
		ReadCatalog(MC_LOG_SET,MC_LOG_NO_SPACE,MC_DEF_LOG_NO_SPACE));
    for (argc = 0; argc < originalArgc; argc++)
	argv[argc] = originalArgv[argc];
    argv[argc] = 0;
    if (DmResourceDB)
	XrmDestroyDatabase (DmResourceDB);
    DmResourceDB = XrmGetStringDatabase ("");
    /* pre-parse the command line to get the -config option, if any */
    XrmParseCommand (&DmResourceDB, configTable,
		     sizeof (configTable) / sizeof (configTable[0]),
		     "Dtlogin", &argc, argv);
    
    configFileName = qualifyWithFirst(DEF_XDM_CONFIG,
				      CDE_CONFIGURATION_TOP "/config:"
				      CDE_INSTALLATION_TOP "/config");

    GetResource ("Dtlogin.configFile", "Dtlogin.ConfigFile",
		 DM_STRING, &config, configFileName);

    free(configFileName);


    
    newDB = XrmGetFileDatabase ( config );
    if (newDB)
    {

	if (DmResourceDB)
	    XrmDestroyDatabase (DmResourceDB);
	DmResourceDB = newDB;
    }
    else if (argc != originalArgc)
	LogError(
		ReadCatalog(MC_LOG_SET,MC_LOG_NO_OPENCFG,MC_DEF_LOG_NO_OPENCFG),
		config );



    /*
     *  scan the resource database to set the application name...
     */
    SetAppName();	
     
    XrmParseCommand (&DmResourceDB, optionTable,
		     sizeof (optionTable) / sizeof (optionTable[0]),
		     AppName, &argc, argv);

     /* 
      * test to see if the session variable is set.
      * for enabling the toggle in the options menu, if it is set..
      */
    if((XrmGetResource (DmResourceDB, "Dtlogin*session", "Dtlogin*Session", &type, &value)) == True)
    	session_set = True;

    if (argc > 1)
    {
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_EXTRA_ARG,MC_DEF_LOG_EXTRA_ARG));
	for (a = argv + 1; *a; a++)
		LogError ((unsigned char *)" \"%s\"", *a);
	LogError ((unsigned char *)"\n");
    }
    free (argv);
}

void 
LoadDMResources( void )
{
	int	i;
	char	name[1024], class[1024];
	FILE	*fp;

	if (servers) {free(servers); servers=NULL;}
	if (keyFile) {free(keyFile); keyFile=NULL;}
	if (accessFile) {free(accessFile); accessFile=NULL;}

	for (i = 0; i < NUM_DM_RESOURCES; i++) {
		sprintf (name, "%s.%s", AppName, DmResources[i].name);
		sprintf (class, "%s.%s", AppName, DmResources[i].class);
		GetResource (name, class, DmResources[i].type,
			      (char **) DmResources[i].dm_value,
			      DmResources[i].default_value);
	}

	servers = getFullFileName(servers, 1, NULL);
	keyFile = getFullFileName(keyFile, 0, NULL);
	accessFile = getFullFileName(accessFile, 0, NULL);

#ifndef __apollo	/* set a TZ default for all OS except Domain */
        if (timeZone == NULL || strlen(timeZone) == 0)
        {
	 /*
          *  dynamically determine the timeZone resource default value...
          */
          GetSysParms(&timeZone,0,0);
        }
#endif

}

void
CleanUpName( char *src, char *dst, int len )
{
    while (*src) {
	if (--len <= 0)
		break;
	switch (*src)
	{
	case ':':
	case '.':
	    *dst++ = '_';
	    break;
	default:
	    *dst++ = *src;
	}
	++src;
    }
    *dst = '\0';
}

void
LoadDisplayResources( struct display *d )
{
	int	i;
	char	name[1024], class[1024];
	char	dpyName[512], dpyClass[512];
	char 	*lang;

	Debug("Loading display resources for %s\n", d->name);
	
	if (d->resources) {free(d->resources); d->resources=NULL;}
	if (d->setup)     {free(d->setup); d->setup=NULL;}
	if (d->startup)   {free(d->startup); d->startup=NULL;}
	if (d->reset)     {free(d->reset); d->reset=NULL;}
	if (d->session)   {free(d->session); d->session=NULL;}
	if (d->failsafeClient) {free(d->failsafeClient); d->failsafeClient=NULL;}

	CleanUpName (d->name, dpyName, sizeof (dpyName));
	CleanUpName (d->class ? d->class : d->name, dpyClass, sizeof (dpyClass));

	if((lang = getenv("LANG")) != NULL)
	   for (i = NUM_DISPLAY_RESOURCES - 1; i >= 0; i--)  {
		if(strcmp(DisplayResources[i].name,"language") == 0){
		  DisplayResources[i].default_value = lang;
		  break;
		}
	   }


	for (i = 0; i < NUM_DISPLAY_RESOURCES; i++) {
		sprintf (name, "%s.%s.%s", AppName,
			dpyName, DisplayResources[i].name);
		sprintf (class, "%s.%s.%s", AppName,
			dpyClass, DisplayResources[i].class);
		GetResource (name, class, DisplayResources[i].type,
			      (char **) (((char *) d) + DisplayResources[i].offset), DisplayResources[i].default_value);
	}

#ifdef _AIX
    if(d->language == NULL || strlen(d->language) == 0)
        SetDefaultLanguage(d);
#endif /* _AIX */

	d->resources = getFullFileName(d->resources, 2, d->language);
	d->setup = getFullFileName(d->setup, 0, NULL);
	d->startup = getFullFileName(d->startup, 0, NULL);
	d->reset = getFullFileName(d->reset, 0, NULL);
	d->session = getFullFileName(d->session, 0, NULL);
	d->failsafeClient = getFullFileName(d->failsafeClient, 0, NULL);

}


	
/***************************************************************************
 *
 *  SetAppName
 *
 *  Probe the resource database to see whether the config file is using
 *  "Dtlogin" or "DisplayManager" as the application name.
 *
 *  If it cannot be determined, "Dtlogin" is used.
 ***************************************************************************/

void
SetAppName( void )
{
    int		i;
    char	name[1024], class[1024];
    char	*type;
    XrmValue	value;

    for (i = 0; i < NUM_DM_RESOURCES; i++) {

        /*
         *  try "Dtlogin" (default) ...
         */

        sprintf (name, "%s.%s", DTLOGIN, DmResources[i].name);
        sprintf (class, "%s.%s", DTLOGIN, DmResources[i].class);

        if (DmResourceDB && XrmGetResource (DmResourceDB, name, class,
	    &type, &value)) {

	    strcpy(AppName, DTLOGIN);
	    break;
        }


        /*
         *  try "DisplayManager" ...
         */

        sprintf (name, "%s.%s", DISPLAYMANAGER, DmResources[i].name);
        sprintf (class, "%s.%s", DISPLAYMANAGER, DmResources[i].class);

        if (DmResourceDB && XrmGetResource (DmResourceDB, name, class,
	    &type, &value)) {

	    strcpy(AppName, DISPLAYMANAGER);
	    break;
        }
        
    }
}


/***************************************************************************
 *
 *  getFullFileName
 *
 * Try to produce a fully qualified file name by prepending
 * /etc/dt/config or /usr/dt/config to a resource file name.
 * There are 2 special cases:  servers - which can be a command, and
 *                             resources - which can an imbeded %L.
 ***************************************************************************/
extern char *_ExpandLang( char *string, char *lang );

static char *
getFullFileName(char *name, int special, char *lang)
{

char * newname;

    if (name == NULL)
        return(NULL);

    /** if the file already starts with a '/' **/
    /** then just return a copy of it         **/
    if (name[0] == '/') {
        if ((newname = (char *)malloc((strlen(name)+1)*sizeof(char))) == NULL) {
            return (NULL);
        } else {
            strcpy(newname, name);
            return(newname);
        }
    }

    switch (special) {
                 /*********************/
        case 0:  /** no special case **/
                 /*********************/
            newname = qualifyWithFirst(name,
                                       CDE_CONFIGURATION_TOP "/config:"
                                       CDE_INSTALLATION_TOP "/config");
            return (newname);

                 /***************************************/
        case 1:  /** special handling for servers file **/
                 /***************************************/
            if (strchr(name, ':') != NULL) {
                /** This is probably a command and not a file name **/
                /** so just return a copy of it.                   **/
                if ((newname = (char *)malloc((strlen(name)+1)*sizeof(char)))
                    == NULL) {
                    return (NULL);
                } else {
                    strcpy(newname, name);
                    return(newname);
                }
            } else {
                newname = qualifyWithFirst(name,
                                           CDE_CONFIGURATION_TOP "/config:"
                                           CDE_INSTALLATION_TOP "/config");
                return (newname);
            }

                 /********************************************/
        case 2:  /** special handling for d->resources file **/
                 /********************************************/
            if (strchr(name, '%') == NULL) {
                /** no special processing needed **/
                newname = qualifyWithFirst(name,
                                           CDE_CONFIGURATION_TOP "/config:"
                                           CDE_INSTALLATION_TOP "/config");
                return (newname);
            } else {
                char *langString;
                char *tempName;
                int tempLen;

                /** need to remember the %L **/
                if (lang == NULL)
                    lang = "C";
                langString = _ExpandLang(name, lang);
                if ( (tempName = qualifyWithFirst(langString,
                                           CDE_CONFIGURATION_TOP "/config:"
                                           CDE_INSTALLATION_TOP "/config") )
                    == NULL) {
                    free(langString);
                    langString = _ExpandLang(name, "C");
                    if ( (tempName = qualifyWithFirst(langString,
                                           CDE_CONFIGURATION_TOP "/config:"
                                           CDE_INSTALLATION_TOP "/config") )
                        == NULL) {
                        free(langString);
                        return(NULL);
                    }
                }
                /** We have a fully qualified and expanded file name **/
                /** but we need to return a fully qualified but NOT  **/
                /** expanded file name.                              **/
                tempLen = strlen(tempName) - strlen(langString)
                             + strlen(name) + 1;
                if ((newname = (char *)malloc(tempLen * sizeof(char))) == NULL){
                    free(langString);
                    return(tempName);
                } else {
                    tempLen = strlen(tempName) - strlen(langString);
                    strncpy(newname, tempName, tempLen);
                    strcpy(newname+tempLen, name);
                    free (langString);
                    return(newname);
                }
            }
    }
}

