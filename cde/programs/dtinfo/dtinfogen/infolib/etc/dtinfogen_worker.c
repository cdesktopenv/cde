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
/* $XConsortium: dtinfogen_worker.c /main/24 1996/11/26 12:35:07 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#ifndef __osf__
# include <libgen.h> /* for dirname(); DEC has prototype in string.h */
#endif
#include <ctype.h>
#include <signal.h>
#if !defined(CSRG_BASED)
#include <sys/sysmacros.h>
#endif
#include <sys/param.h>
#include <sys/stat.h>
#include <X11/Intrinsic.h>
#include <Dt/Utility.h>

#include <lib/DtSvc/DtUtil2/LocaleXlate.h>

#define LANG_COMMON	"C"		/* default os language */
#define CLANG_COMMON	"C.ISO-8859-1"	/* default canonical language */

#define XtsNewString(str) \
    ((str) != NULL ? (char *)(memcpy(XtMalloc((unsigned)strlen(str) + 1), \
     str, (unsigned)strlen(str) + 1)) : NULL)

typedef enum
{
    FSTAT_EXISTS        = 1 << 0,
    FSTAT_IS_FILE       = 1 << 1,
    FSTAT_IS_DIR        = 1 << 2,
    FSTAT_IS_READABLE   = 1 << 3,
    FSTAT_IS_WRITABLE   = 1 << 4,
    FSTAT_IS_EXECUTABLE = 1 << 5
} FileStatFlags;

typedef enum
{
    INTEGER_CODE = 4,
    STRING_CODE = 6,
    OID_CODE = 7,
    COMPRESSED_STRING_CODE = 11,
    SHORT_LIST_CODE = 105,
    OID_LIST_CODE = 106,
    NODE_META = 401
} MMDBConstants;

typedef struct _GlobalsStruct
{
    char *pathEnv;
    char *ldLibraryPathEnv;
    char *libPathEnv;
    char *shlibPathEnv;
    char *lcxPathEnv;
    char *dtInfoHomeEnv;
    char *dtInfoBinEnv;
    char *tmpDirEnv;
    char *sgmlPathEnv;
    char *sgmlSearchPathEnv;
    char *sgmlCatFilesEnv;
    char *sgmlCatFiles;
    int sgmlCatFilesLen;
    int sgmlCatFilesMaxLen;
    char *mmdbPathEnv;

    char *install;
    char *arch;
    char *lang;		/* os-specific language (value of env) */
    char *clang;	/* canonical language */
    int   dtsridx;      /* dtsearch: index into langtbl */
    char *sgml;
    char *decl;
    char *styleProlog;
    char *tmpDir;
    char *spec;
    int dirMode;
    char *searchEngine;
    char *parser;

    Boolean verbose;
    Boolean keepWorkDir;
    char *workDir;

    char *library;
    char *libDesc;
    char *bookCase;
    char *libName;
    char *outFile;
    char *saveESIS;
    Boolean loadESIS;
    int tocElemIndex;
    char *id;
    char *title;

    char *dtsrlib;
    char *dbdfile;
    char *keytypes;
} GlobalsStruct;

typedef struct _TOCEntry
{
    char *ord;
    char *entry;
} TOCEntry;

typedef struct _TOCRecord
{
    int code;
    char *loc;
    char *file;
    char *line;
    char *ord;
    char *title;
} TOCRecord;

#define TOC_REC_COLS 6

static char *TOCElems[] =
{
    "TOCPart",
    "TOCChap",
    "TOClevel1",
    "TOClevel2",
    "TOClevel3",
    "TOClevel4",
    "TOClevel5"
};

static char *emptyString = "";

#define STR(s) ((s) ? (s) : emptyString)

#define EXEC_NAME "dtinfogen"

typedef struct
{
  const char* name;
  int	      dtsrlang;
  const char* sfx;
  const char* stp;
  const char* knj;
} t_entry;

static
t_entry langtbl[] =
{
  { "C.ISO-8859-1",	1,	"eng.sfx",	"eng.stp",	NULL },
  { "es_ES.ISO-8859-1",	2,	"esp.sfx",	"esp.stp",	NULL },
  { "fr_FR.ISO-8859-1",	3,	"fra.sfx",	"fra.stp",	NULL },
  { "it_IT.ISO-8859-1",	4,	"ita.sfx",	"ita.stp",	NULL },
  { "de_DE.ISO-8859-1",	5,	"deu.sfx",	"deu.stp",	NULL },
  { "ja_JP.EUC-JP",	7,	NULL,		NULL,		"jpn.knj" },
  { NULL,		0,	NULL,		NULL,		NULL }
};

static char *usageMsg1 = "USAGE:\n\
  " EXEC_NAME " -h\n\
  " EXEC_NAME " admin\n\
  " EXEC_NAME " build [-h] [-T <tmpdir>] [-m <catalog>] [-d <library description>]\n\
           [-n <library short name>] -l <library> <bookcase-doc>...\n\
  " EXEC_NAME " tocgen [-h] [-T <tmpdir>] [-m <catalog>] -f <tocfile> [-id <tocid>]\n\
            [-title <toctitle>] <document>...\n\
  " EXEC_NAME " update [-h] [-m <catalog>] -l <library> -b <bookcase> <stylesheet>\n\
  " EXEC_NAME " validate [-h] [-T <tmpdir>] [-m <catalog>] <document>...\n"
  "\n"
  "options:\n";

static char *usageMsg2 = "\
    -T <tmpdir>          directory for intermediate processing files\n\
    -h                   help: show usage\n\
    -v                   verbose: more diagnostic output\n";

static GlobalsStruct *gStruct;

static void printUsage(char *preMsg, int exitCode);
static void dieRWD(int exitCode, char *format, ...);
static void die(int exitCode, char *format, ...);
static void sigHandler(int sig);
static void touchFile(char *fileName);
static Boolean checkStat(char *fileName, unsigned int flags);
static void checkDir(char *dirName);
static void checkExec(char *execName);
static char *makeTmpFile(void);
static char *makeWorkDir(void);
static void removeWorkDir(void);
static void runShellCmd(char *cmd);
static Boolean doAdmin(int argc, char *argv[]);
static Boolean doBuild(int argc, char *argv[]);
static Boolean doTocgen(int argc, char *argv[]);
static Boolean doUpdate(int argc, char *argv[]);
static Boolean doValidate(int argc, char *argv[]);
static Boolean doHelp(int argc, char *argv[]);
static void appendStr(char **str, int *curLen, int *maxLen, char *strToAppend);
static char *makeAbsPathEnv(char *var);
static char *makeAbsPathStr(char *str);
static char *addToEnv(char *var, char *addMe, Boolean prepend);
static char *buildPath(char *format, ...);
static char *buildSGML(void);
static char *buildDecl(void);
static char *buildStyleProlog(void);
static char *buildSpec(void);
static void defaultGlobals(void);
static void checkGlobals(void);
static int parseArgs(int argc, char *argv[]);
static char *parseDocument(Boolean runCmd, ...);
static void buildBookcase(char *cmdSrc, char *dirName);
static char *storeBookCase(char *cmdSrc, char *tocOpt, char *dbName,
			   char *dirName);
static Boolean findBookCaseNameAndDesc(char *tmpFile, char *bookCaseName,
				       char *bookCaseDesc);
static void validateBookCaseName(char *bookCaseName);
static void validateBookCase(char *mapName, char *bookCaseName);
static void editMapFile(char *bookCaseName, char *bookCaseMap);
static void buildTOC(int argc, char *argv[]);
static char *tocBookcase(int argc, char *argv[]);
static void makeTOC(char *id, char *title);
static char *sgmlData(char *inData);
static char *replaceData(char *inData, char *replaceMe, char *replacement);
static void addTOCEntry(TOCEntry **tocEntries, int *curSize, int *maxSize,
			TOCEntry *newEntry);
static int compareTOCEntries(const void *entry1, const void *entry2);
static void sortTOCEntries(TOCEntry *tocEntries, int nTOCEntries);
static void freeTOCEntries(TOCEntry *tocEntries, int nTOCEntries);
static TOCRecord *getTOCRecord(FILE *fp);
static void freeTOCRecord(TOCRecord *);
static char *getTOCField(FILE *fp);

static void
printUsage(char *preMsg, int exitCode)
{
    if (preMsg)
	fputs(preMsg, stderr);

    fputs(usageMsg1, stderr);
    fputs(usageMsg2, stderr);

    exit(exitCode);
}

static void
dieRWD(int exitCode, char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);

    if (!gStruct->keepWorkDir)
	removeWorkDir();

    exit(exitCode);
}

static void
die(int exitCode, char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);

    exit(exitCode);
}

static void
sigHandler(int sig)
{
    gStruct->keepWorkDir = False;	/* Always clean up on signal. */
    dieRWD(-1, "Received signal; exiting...\n");
}

static void
touchFile(char *fileName)
{
    FILE *fp;

    if ((fp = fopen(fileName, "w")) == (FILE *)NULL)
	dieRWD(-1, "%s: %s: %s\n",
	       EXEC_NAME, fileName, strerror(errno));

    fprintf(fp, "%s\n",
	    "$XConsortium: dtinfogen_worker.c /main/24 1996/11/26 12:35:07 cde-hal $");

    fclose(fp);
}

static Boolean
checkStat(char *fileName, unsigned int flags)
{
    struct stat statBuf;
    Boolean userOwnsFile;
    Boolean userInGroup;

    if (!fileName)
	return False;

    errno = 0;

    if (stat(fileName, &statBuf) != 0)
	return False;

    if ((flags & FSTAT_IS_FILE) &&
	(!S_ISREG(statBuf.st_mode)))
    {
	errno = ENOENT;
	return False;
    }

    if ((flags & FSTAT_IS_DIR) &&
	(!S_ISDIR(statBuf.st_mode)))
    {
	errno = ENOTDIR;
	return False;
    }

    userOwnsFile = (getuid() == statBuf.st_uid);
    userInGroup = (getgid() == statBuf.st_gid);

    errno = EACCES;
    if (flags & FSTAT_IS_READABLE)
    {
	if (userOwnsFile)
	{
	    if (!(statBuf.st_mode & S_IRUSR))
		return False;
	}
	else if (userInGroup)
	{
	    if (!(statBuf.st_mode & S_IRGRP))
		return False;
	}
	else if (!(statBuf.st_mode & S_IROTH))
	    return False;
    }

    if (flags & FSTAT_IS_WRITABLE)
    {
	if (userOwnsFile)
	{
	    if (!(statBuf.st_mode & S_IWUSR))
		return False;
	}
	else if (userInGroup)
	{
	    if (!(statBuf.st_mode & S_IWGRP))
		return False;
	}
	else if (!(statBuf.st_mode & S_IWOTH))
	    return False;
    }

    if (flags & FSTAT_IS_EXECUTABLE)
    {
	if (userOwnsFile)
	{
	    if (!(statBuf.st_mode & S_IXUSR))
		return False;
	}
	else if (userInGroup)
	{
	    if (!(statBuf.st_mode & S_IXGRP))
		return False;
	}
	else if (!(statBuf.st_mode & S_IXOTH))
	    return False;
    }

    errno = 0;
    return True;
}

static void
checkDir(char *dirName)
{
    char cmdBuf[MAXPATHLEN + 10];

    if (!dirName)
	return;

    if (checkStat(dirName, FSTAT_IS_DIR))
    {
	if (checkStat(dirName, FSTAT_IS_WRITABLE))
	    return;

	dieRWD(-1, "%s: %s not writable\n", EXEC_NAME, dirName);
    }

    snprintf(cmdBuf, sizeof(cmdBuf), "mkdir -p %s", dirName);
    runShellCmd(cmdBuf);
}

static Boolean
testExec(char *execName, Boolean tellIt)
{
    char *path;
    char **pathVector;
    int i;
    char execBuf[MAXPATHLEN + 1];
    Boolean found;

    if (!execName)
	return True;

    if ((path = getenv("PATH")) == (char *)NULL)
	return True;

    path = XtsNewString(path);
    pathVector = _DtVectorizeInPlace(path, ':');

    for (i = 0; pathVector[i] != (char *)NULL; i++)
    {
	snprintf(execBuf, sizeof(execBuf), "%s/%s", pathVector[i], execName);
	if (checkStat(execBuf, FSTAT_IS_EXECUTABLE))
	    break;
    }

    found = (pathVector[i] != (char *)NULL);

    XtFree((char *)pathVector);
    XtFree(path);

    if (found && tellIt)
	fprintf(stderr, "%s ==> %s\n", execName, execBuf);

    return found;
}

static void
checkExec(char *execName)
{
    if (!testExec(execName, gStruct->verbose))
	die(-1, "%s: %s not found\n", EXEC_NAME, execName);
}

static char *
makeTmpFile(void)
{
    int i;
    char *tmpFile;
    char *workDir;

    workDir = makeWorkDir();
    for (i = 1; i != 0; i++)
    {
	tmpFile = buildPath("%s/tmp.%d", workDir, i);
	if (!checkStat(tmpFile, FSTAT_EXISTS))
	    return tmpFile;

	XtFree(tmpFile);
    }

    dieRWD(-1, "%s: could not create tmp file.\n", EXEC_NAME);

    /* NOTREACHED */
    return (char *)NULL;
}

/* The return from this function should NOT be freed. */
static char *
makeWorkDir(void)
{
    char *workDir;
    char cmdBuf[MAXPATHLEN + 10];
    int i;

    if (gStruct->workDir)
	return gStruct->workDir;

    if (!checkStat(gStruct->tmpDir, FSTAT_IS_DIR | FSTAT_IS_WRITABLE))
	dieRWD(-1, "%s: %s: %s\n",
	       EXEC_NAME, gStruct->tmpDir, strerror(errno));

    /* Start with suffix "1"; if we wrap to "0" */
    /* (that's LOTS of directories), die. */
    for (i = 1; i != 0; i++)
    {
	workDir = buildPath("%s/otk.%d", gStruct->tmpDir, i);
	if (!checkStat(workDir, FSTAT_EXISTS))
	{
	    snprintf(cmdBuf, sizeof(cmdBuf), "mkdir -p %s", workDir);
	    runShellCmd(cmdBuf);
	    gStruct->workDir = workDir;

	    return workDir;
	}

	XtFree(workDir);
    }

    dieRWD(-1, "%s: could not create work directory.\n", EXEC_NAME);

    /* NOTREACHED */
    return (char *)NULL;
}

static void
removeWorkDir(void)
{
    int ret;
    char cmdBuf[MAXPATHLEN + 10];

    if (gStruct->workDir)
    {
	snprintf(cmdBuf, sizeof(cmdBuf), "rm -rf %s", gStruct->workDir);
	ret = system(cmdBuf);
	if(ret != 0) die(-1, "system for rm failed; exiting...\n");
	XtFree(gStruct->workDir);
	gStruct->workDir = (char *)NULL;
    }
}

static void
runShellCmd(char *cmd)
{
    if (gStruct->verbose)
	fprintf(stdout, "%s\n", cmd);

    if (system(cmd) != 0)
	dieRWD(-1, "%s: command failed: %s\n", EXEC_NAME, cmd);
}

static Boolean
doAdmin(int argc, char *argv[])
{
    if (strcmp(argv[1], "admin") == 0)
    {
	if (argc > 2)
	    printUsage(EXEC_NAME ": too many arguments\n", -1);

	runShellCmd("Librarian");

	return True;
    }

    return False;
}

static Boolean
doBuild(int argc, char *argv[])
{
    if (strcmp(argv[1], "build") == 0)
    {
	char *bookcase;
	char *bcCopy;
	char *dirName;
	char *cmdSrc;
	char *fileToTouch;
	int argsProcessed = parseArgs(argc - 2, &(argv[2])) + 2;

	if (!gStruct->library)
	    printUsage(EXEC_NAME ": -l required\n", -1);
	if (argsProcessed >= argc)
	    printUsage(EXEC_NAME ": no bookcases given\n", -1);

	checkDir(gStruct->library);

	if (!gStruct->libDesc)
	    gStruct->libDesc = "Information Library";

	checkGlobals();

	if (argsProcessed >= argc)
	    printUsage(EXEC_NAME ": no bookcases given\n", -1);

	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);

	for ( ; argsProcessed < argc; argsProcessed++)
	{
	    bookcase = argv[argsProcessed];
	    if (!checkStat(bookcase, FSTAT_IS_READABLE))
		dieRWD(-1, "%s: %s: %s\n",
		       EXEC_NAME, bookcase, strerror(errno));

	    bcCopy = XtsNewString(bookcase);
	    if ((dirName = dirname(bcCopy)) == (char *)NULL)
		dirName = ".";
	    dirName = XtsNewString(dirName);
	    XtFree(bcCopy);

	    if (gStruct->loadESIS)
	    {
		cmdSrc = buildPath("cat %s", bookcase);
	    }
	    else
	    {
		cmdSrc = parseDocument(False, bookcase, 0);
	    }

	    buildBookcase(cmdSrc, dirName);

	    XtFree(cmdSrc);
	    XtFree(dirName);
	}

	if (!gStruct->libName)
	    gStruct->libName = "infolib";
	fileToTouch = buildPath("%s/%s.oli",
				gStruct->library, gStruct->libName);
	touchFile(fileToTouch);
	XtFree(fileToTouch);

	return True;
    }

    return False;
}

static Boolean
doTocgen(int argc, char *argv[])
{
    if (strcmp(argv[1], "tocgen") == 0)
    {
	int argsProcessed = parseArgs(argc - 2, &(argv[2])) + 2;

	if (!gStruct->id)
	    gStruct->id = "TOC-NODE-ID";
	if (!gStruct->title)
	    gStruct->title = "Table of Contents";

	checkGlobals();

	if (argsProcessed >= argc)
	    printUsage(EXEC_NAME ": book document(s) required\n", -1);
	if (!gStruct->outFile)
	    printUsage(EXEC_NAME ": -f required for tocgen\n", -1);

	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);

	buildTOC(argc - argsProcessed, &argv[argsProcessed]);

	return True;
    }
    return False;
}

static Boolean
doUpdate(int argc, char *argv[])
{
    if (strcmp(argv[1], "update") == 0)
    {
	char bookCaseBuf[MAXPATHLEN + 1];
	char *styleSheet;
	char *cmdSrc;
	char *cmd;
	int argsProcessed = parseArgs(argc - 2, &(argv[2])) + 2;

	checkGlobals();

	if (!gStruct->library)
	    printUsage(EXEC_NAME ": -l required for update\n", -1);
	if (!checkStat(gStruct->library, FSTAT_IS_DIR))
	    die(-1, "%s: %s: %s\n", EXEC_NAME,
		gStruct->library, strerror(errno));

	if (!gStruct->bookCase)
	    printUsage(EXEC_NAME ": -b required\n", -1);
	snprintf(bookCaseBuf, sizeof(bookCaseBuf), "%s/%s",
				gStruct->library, gStruct->bookCase);
	if (!checkStat(bookCaseBuf, FSTAT_IS_DIR))
	    die(-1, "%s: No such bookcase: %s\n", EXEC_NAME,
		gStruct->bookCase);

	if (!checkStat(gStruct->styleProlog, FSTAT_IS_READABLE))
	    die(-1, "%s: faulty installation: %s\n", EXEC_NAME,
		gStruct->styleProlog);

	if (argsProcessed >= argc)
	    printUsage((char *)NULL, -1);
	styleSheet = argv[argsProcessed++];
	if (!checkStat(styleSheet, FSTAT_IS_READABLE))
	    die(-1, "%s: %s: %s\n", EXEC_NAME, styleSheet, strerror(errno));

	cmdSrc = parseDocument(False,
			       gStruct->styleProlog, styleSheet, 0);
	cmd = buildPath("%s | StyleUpdate %s %s",
			cmdSrc, gStruct->library, gStruct->bookCase);
	runShellCmd(cmd);

	XtFree(cmdSrc);
	XtFree(cmd);

	return True;
    }

    return False;
}

static Boolean
doValidate(int argc, char *argv[])
{
    if (strcmp(argv[1], "validate") == 0)
    {
	char *cmdSrc;
	char *cmd;
	char *bookcase;
	int argsProcessed = parseArgs(argc - 2, &(argv[2])) + 2;

	checkGlobals();

	if (argsProcessed >= argc)
	    printUsage(EXEC_NAME ": no documents given\n", -1);

	for ( ; argsProcessed < argc; argsProcessed++)
	{
	    bookcase = argv[argsProcessed];
	    if (!checkStat(bookcase, FSTAT_IS_READABLE))
		dieRWD(-1, "%s: %s: %s\n",
		       EXEC_NAME, bookcase, strerror(errno));

	    if (gStruct->saveESIS)
	    {
		cmdSrc = parseDocument(False, bookcase, 0);
		cmd = buildPath("%s > %s", cmdSrc, gStruct->saveESIS);
		runShellCmd(cmd);
		XtFree(cmdSrc);
		XtFree(cmd);
	    }
	    else
	    {
		parseDocument(True, bookcase, 0);
	    }
	}

	return True;
    }

    return False;
}

static Boolean
doHelp(int argc, char *argv[])
{
    if (strcmp(argv[1], "-h") == 0)
    {
	printUsage((char *)NULL, 0);

	/* NOTREACHED */
	return True;
    }

    return False;
}

static void
appendStr(char **str, int *curLen, int *maxLen, char *strToAppend)
{
    int len;
    char *newStr;

    if (!strToAppend)
	return;
    len = strlen(strToAppend);
    while (*curLen + len >= *maxLen)
    {
	newStr = XtRealloc(*str, (*maxLen + MAXPATHLEN) * sizeof(char));
	if (!newStr)
	    return;

	*str = newStr;
	*maxLen += MAXPATHLEN;
    }

    *((char *) memcpy (&((*str)[*curLen]), strToAppend, len) + len) = '\0';
    *curLen += len;
}

static char *
makeAbsPathEnv(char *var)
{
    char *oldPath, *newPath = NULL;
    char *newVar;
    int pathlen;

    if (!var)
	return (char *)NULL;

    oldPath = getenv(var);
    if (oldPath == (char *)NULL)
	return (char *)NULL;

    newPath = makeAbsPathStr(oldPath);

    if (gStruct->verbose)
	fprintf(stdout, "Expanding %s\n  from %s\n  to %s\n",
		var, oldPath, newPath);

    pathlen = strlen(var) + strlen(newPath) + 2;
    newVar = XtMalloc(pathlen);
    snprintf(newVar, pathlen, "%s=%s", var, newPath);
    putenv(newVar);

    if (newPath)
	XtFree(newPath);

    return newVar;
}

static char *
makeAbsPathStr(char *str)
{
    char *newPath = (char *)NULL;
    int newPathLen = 0;
    int maxPathLen = 0;
    char cwd[MAXPATHLEN + 1];
    char **cwdVector;
    int cwdVectorLen;
    char **pathVector;
    char **dirVector;
    int i, j, k;

    if (str == (char *)NULL)
	return (char *)NULL;
    str = XtsNewString(str);

    if (!getcwd(cwd, MAXPATHLEN))
	cwd[0] = '\0';

    /* Vectorize current dir, skipping over leading '/' */
    cwdVector = _DtVectorizeInPlace(cwd + 1, '/');
    for (cwdVectorLen = 0;
	 cwdVector[cwdVectorLen] != (char *)NULL;
	 cwdVectorLen++)
	/* EMPTY */
	;

    /* Vectorize incoming path */
    pathVector = _DtVectorizeInPlace(str, ':');

    for (i = 0; pathVector[i] != (char *)NULL; i++)
    {
	if (i > 0)
	    appendStr(&newPath, &newPathLen, &maxPathLen, ":");

	if ((pathVector[i][0] == '/') ||  /* Fully-specified pathname. */
	    ((pathVector[i][0] == '.') && /* Current dir. */
	     (pathVector[i][1] == '\0')))
	{
	    appendStr(&newPath, &newPathLen, &maxPathLen, pathVector[i]);
	    continue;
	}

	dirVector = _DtVectorizeInPlace(pathVector[i], '/');
	for (j = 0;
	     dirVector[j] != (char *)NULL && (strcmp(dirVector[j], "..") == 0);
	     j++)
	    /* EMPTY */
	    ;

	if ((j >= cwdVectorLen) && (dirVector[j] == (char *)NULL))
	{
	    appendStr(&newPath, &newPathLen, &maxPathLen, "/");
	}
	else
	{
	    for (k = 0; k < cwdVectorLen - j; k++)
	    {
		appendStr(&newPath, &newPathLen, &maxPathLen, "/");
		appendStr(&newPath, &newPathLen, &maxPathLen, cwdVector[k]);
	    }
	    for ( ; dirVector[j] != (char *)NULL; j++)
	    {
		appendStr(&newPath, &newPathLen, &maxPathLen, "/");
		appendStr(&newPath, &newPathLen, &maxPathLen, dirVector[j]);
	    }
	}

	XtFree((char *)dirVector);
    }

    XtFree((char *)pathVector);
    XtFree((char *)cwdVector);
    XtFree(str);

    return newPath;
}

static char *
addToEnv(char *var, char *addMe, Boolean prepend)
{
    char *envStr;
    int len, ptrlen;
    char *ptr;

    if (!var)
	return (char *)NULL;
    envStr = getenv(var);

    if (!addMe)
	return XtsNewString(envStr);

    len = strlen(var) + strlen(STR(envStr)) + strlen(addMe) + 3;
    ptrlen = len * sizeof(char);

    if ((ptr = XtMalloc(ptrlen)) != (char *)NULL)
    {
	if (envStr)
	{
	    if (prepend)
		snprintf(ptr, ptrlen, "%s=%s:%s", var, addMe, envStr);
	    else snprintf(ptr, ptrlen, "%s=%s:%s", var, envStr, addMe);
	}
	else snprintf(ptr, ptrlen, "%s=%s", var, addMe);

	putenv(ptr);
	return ptr;
    }

    return XtsNewString(envStr);
}

static char *
buildPath(char *format, ...)
{
    char pathBuf[(2 * MAXPATHLEN) + 1];
    va_list ap;

    va_start(ap, format);
    vsnprintf(pathBuf, sizeof(pathBuf), format, ap);
    va_end(ap);

    return XtsNewString(pathBuf);
}

/* Assumes gStruct->install and gStruct->clang */
/* are set (may be NULL) */
static char *
buildSGML(void)
{
    char *sgmlPath =
	buildPath("%s/infolib/%s/SGML",
		  STR(gStruct->install), STR(gStruct->clang));

    if (!checkStat(sgmlPath, FSTAT_IS_DIR))
    {
	XtFree(sgmlPath);

	sgmlPath = buildPath("%s/infolib/%s/SGML",
				STR(gStruct->install), LANG_COMMON);

	if (!checkStat(sgmlPath, FSTAT_IS_DIR)) {
	    XtFree(sgmlPath);
	    return (char *)NULL;
	}
    }
#ifdef SGML_DEBUG
    fprintf(stderr, "(DEBUG) buildSGML=\"%s\"\n", sgmlPath);
#endif

    return sgmlPath;
}

/* Assumes gStruct->sgml is set (may be NULL) */
static char *
buildDecl(void)
{
    return buildPath("%s/dtinfo.decl", STR(gStruct->sgml));
}

/* Assumes gStruct->sgml is set (may be NULL) */
static char *
buildStyleProlog(void)
{
    return buildPath("%s/styprolog.sgml", STR(gStruct->sgml));
}

/* Assumes gStruct->install is set (may be NULL) */
static char *
buildSpec(void)
{
    return buildPath("%s/infolib/etc/mmdb.infolib.spec",
		     STR(gStruct->install));
}

static void
defaultGlobals(void)
{
    memset((void *)gStruct, 0, sizeof(GlobalsStruct));

    gStruct->verbose = False;

    /* Clear out the ENV environment variable. */
    if (getenv("ENV") != (char *)NULL)
	putenv("ENV=");

    gStruct->pathEnv = makeAbsPathEnv("PATH");
    gStruct->ldLibraryPathEnv = makeAbsPathEnv("LD_LIBRARY_PATH");
    gStruct->libPathEnv = makeAbsPathEnv("LIBPATH");
    gStruct->shlibPathEnv = makeAbsPathEnv("SHLIB_PATH");
    gStruct->lcxPathEnv = makeAbsPathEnv("DTLCXSEARCHPATH");
    gStruct->dtInfoHomeEnv = makeAbsPathEnv("DTINFO_HOME");
    gStruct->dtInfoBinEnv = makeAbsPathEnv("DTINFO_BIN");
    gStruct->tmpDirEnv = makeAbsPathEnv("TMPDIR");
    gStruct->sgmlPathEnv = makeAbsPathEnv("SGML_PATH");
    gStruct->sgmlSearchPathEnv = makeAbsPathEnv("SGML_SEARCH_PATH");
    gStruct->sgmlCatFilesEnv = makeAbsPathEnv("SGML_CATALOG_FILES");
    gStruct->sgmlCatFiles = NULL;
    gStruct->sgmlCatFilesLen = 0;
    gStruct->sgmlCatFilesMaxLen = 0;

    gStruct->install = getenv("DTINFO_HOME");
    gStruct->arch = getenv("ARCH");

    { /* resolve lang from env variable */
      char* lang;
      if ((lang = getenv("LC_ALL")) == NULL)
	if ((lang = getenv("LC_CTYPE")) == NULL)
	  if ((lang = getenv("LANG")) == NULL)
	    lang = LANG_COMMON;

      gStruct->lang = strdup(lang); 
    }

    { /* resolve canonical lang using _DtLcx routines */
      _DtXlateDb db = NULL;
      char platform[_DtPLATFORM_MAX_LEN + 1];
      int execver, compver;

      gStruct->clang = NULL;

      if (_DtLcxOpenAllDbs(&db) == 0)
      {
	if (_DtXlateGetXlateEnv(db, platform, &execver, &compver) == 0)
	{
	  /* _DtLcxXlateOpToStd allocates space for std lang using strdup */
	  _DtLcxXlateOpToStd(db, platform, compver, DtLCX_OPER_SETLOCALE,
			gStruct->lang, &gStruct->clang, NULL, NULL, NULL);
	}

	_DtLcxCloseDb(&db);
	db = NULL;
      }

      if (gStruct->clang == NULL)
	gStruct->clang = strdup(CLANG_COMMON);
    }

    { /* resolve dtsearch language based on canonical lang */
      t_entry* iter;
      for (gStruct->dtsridx = 0, iter = langtbl; iter->name; iter++) {
	if (strcmp(iter->name, gStruct->clang) == 0) { /* found a match */
	  gStruct->dtsridx = iter - langtbl;
	  break;
	}
      }
    }

    if ((gStruct->sgml = buildSGML()) == NULL) {
	die(-1, "%s: Cannot find SGML files\n", EXEC_NAME);
    }

    gStruct->decl = buildDecl();
    gStruct->styleProlog = buildStyleProlog();

    if ((gStruct->tmpDir = getenv("TMPDIR")) == (char *)NULL)
	gStruct->tmpDir = "/usr/tmp";

    gStruct->spec = buildSpec();

    gStruct->dirMode = 0775;
    gStruct->searchEngine = "dtsearch";
    gStruct->parser = "nsgmls";
    if (!testExec(gStruct->parser, False))
	gStruct->parser = "sgmls";

    gStruct->keepWorkDir = False;
    gStruct->workDir = (char *)NULL;

    gStruct->tocElemIndex = 0;
}

static void
checkGlobals(void)
{
    if ((!gStruct->install) || (!checkStat(gStruct->install, FSTAT_IS_DIR)))
	die(-1, "%s: Cannot find DtInfo Toolkit installation directory.\n"
		"\n"
		"The DTINFO_HOME variable must be set to the directory "
		"where the DtInfo\n"
		"toolkit is installed.\n"
		"\n"
		"You probably invoked this script in an unsupported manner.\n",
	    EXEC_NAME);

    if (!gStruct->arch)
	die(-1, "%s: ARCH not set\n", EXEC_NAME);

    if (!checkStat(gStruct->sgml, FSTAT_IS_DIR))
	die(-1, "%s: Can't find DtInfo SGML directory (%s): %s\n",
	    EXEC_NAME, STR(gStruct->sgml), strerror(errno));

    if (!checkStat(gStruct->decl, FSTAT_IS_READABLE))
	die(-1, "%s: faulty installation: %s: %s\n",
	    EXEC_NAME, STR(gStruct->decl), strerror(errno));

    if (!checkStat(gStruct->spec, FSTAT_IS_READABLE))
	die(-1, "%s: faulty installation: %s: %s\n",
	    EXEC_NAME, STR(gStruct->spec), strerror(errno));

    if (!checkStat(gStruct->tmpDir, FSTAT_IS_DIR | FSTAT_IS_WRITABLE))
	die(-1, "%s: %s: %s\n",
	    EXEC_NAME, STR(gStruct->tmpDir), strerror(errno));

    checkExec("dbdrv");
    checkExec("NCFGen");
    checkExec("MixedGen");
    checkExec("NodeParser");
    checkExec("StyleUpdate");
    checkExec("valBase");
    checkExec(gStruct->parser);
    if (strcmp(gStruct->searchEngine, "dtsearch") == 0)
    {
	checkExec("dtsrcreate");
	checkExec("dtsrload");
	checkExec("dtsrindex");

	gStruct->dtsrlib = buildPath("%s/infolib/etc/%s/dtsr",
				     gStruct->install,
				     STR(langtbl[gStruct->dtsridx].name));

	if (!checkStat(gStruct->dtsrlib, FSTAT_IS_DIR)) {
	    XtFree(gStruct->dtsrlib);
	    gStruct->dtsrlib = buildPath("%s/infolib/etc/%s/dtsr",
					gStruct->install, LANG_COMMON);
	}
#ifdef DTSR_DEBUG
	fprintf(stderr, "(DEBUG) gStruct->dtsrlib=\"%s\"\n", gStruct->dtsrlib);
#endif
	gStruct->dbdfile = buildPath("%s/infolib/etc/dtsr/%s.dbd",
				     gStruct->install,
				     gStruct->searchEngine);
	gStruct->keytypes = "Default Head Graphics Example Index Table";
    }
    checkExec("validator");
}

static void
addCatFile(char *catalog, Bool needed)
{
    Boolean parserIsNSGMLS;
    char pathBuf[(2 * MAXPATHLEN) + 10];
    char *ptr1, *ptr2;
    int catlen;

    if (!checkStat(catalog, FSTAT_IS_READABLE)) {
	if (!needed)
	    return;
	dieRWD(-1, "%s: %s: %s\n",
	       EXEC_NAME, catalog, strerror(errno));
    }
    parserIsNSGMLS = (strcmp(gStruct->parser, "nsgmls") == 0);
    if (parserIsNSGMLS)
    {
	ptr1 = makeAbsPathStr(catalog);
	snprintf(pathBuf, sizeof(pathBuf), "-c%s ", ptr1);
	appendStr(&gStruct->sgmlCatFiles, &gStruct->sgmlCatFilesLen,
		  &gStruct->sgmlCatFilesMaxLen, pathBuf);
	XtFree(ptr1);
    }
    else
    {
	ptr1 = strrchr(catalog, '/');
	catlen = strlen(catalog);
	if (ptr1)
	    catlen -= strlen(ptr1);
	snprintf(pathBuf, sizeof(pathBuf), "%.*s/%%P:%.*s/%%S",
				catlen, catalog, catlen, catalog);
	ptr1 = makeAbsPathStr(pathBuf);
	ptr2 = addToEnv("SGML_PATH", ptr1, False);
	if (gStruct->sgmlPathEnv)
	    XtFree(gStruct->sgmlPathEnv);
	if (ptr1)
	    XtFree(ptr1);
	gStruct->sgmlPathEnv = ptr2;
    }
}

static int
parseArgs(int argc, char *argv[])
{
    int i;

    for (i = 0; (i < argc) && (argv[i][0] == '-'); i++)
    {
	if (strcmp(argv[i], "-l") == 0)
	{
	    if (++i < argc)
	    {
		gStruct->library = argv[i];
		if (checkStat(gStruct->library, FSTAT_EXISTS) &&
		    !checkStat(gStruct->library, FSTAT_IS_WRITABLE))
		    dieRWD(-1, "%s: %s: %s\n",
			   EXEC_NAME, gStruct->library, strerror(errno));
	    }
	}
	else if (strcmp(argv[i], "-b") == 0)
	{
	    if (++i < argc)
		gStruct->bookCase = argv[i];
	}
	else if (strcmp(argv[i], "-d") == 0)
	{
	    if (++i < argc)
	    {
		int j;

		gStruct->libDesc = XtsNewString(argv[i]);

		/* Change TABs to SPACEs */
		for (j = 0; gStruct->libDesc[j] != '\0'; j++)
		{
		    if (gStruct->libDesc[j] == '\t')
			gStruct->libDesc[j] = ' ';
		}
	    }
	}
	else if (strcmp(argv[i], "-n") == 0)
	{
	    if (++i < argc)
	    {
		gStruct->libName = argv[i];
		if (strlen(gStruct->libName) > 8)
		    dieRWD(-1, "%s: information library name must be \n"
			       "less than or equal to eight characters.\n", EXEC_NAME);
	    }
	}
	else if (strcmp(argv[i], "-f") == 0)
	{
	    if (++i < argc)
	    {
		gStruct->outFile = argv[i];
		if (checkStat(gStruct->outFile, FSTAT_EXISTS) &&
		    !checkStat(gStruct->outFile, FSTAT_IS_WRITABLE))
		    dieRWD(-1, "%s: %s already exists and not writable\n",
			   EXEC_NAME, gStruct->outFile);
	    }
	}
	else if (strcmp(argv[i], "-v") == 0)
	{
	    gStruct->verbose = True;
	}
	else if (strcmp(argv[i], "-h") == 0)
	{
	    printUsage((char *)NULL, 0);
	}
	else if (strcmp(argv[i], "-T") == 0)
	{
	    if (++i < argc)
	    {
		gStruct->tmpDir = argv[i];
		checkDir(gStruct->tmpDir);
	    }
	}
	else if (strcmp(argv[i], "-m") == 0)
	{
	    if (++i < argc)
		addCatFile(argv[i], True);
	}
	else if (strcmp(argv[i], "-s") == 0)
	{
	    if (++i < argc)
		gStruct->searchEngine = argv[i];
	}
	else if (strcmp(argv[i], "--save-esis") == 0)
	{
	    if (++i < argc)
		gStruct->saveESIS = argv[i];
	}
	else if (strcmp(argv[i], "--load-esis") == 0)
	{
	    if (++i < argc)
		gStruct->loadESIS = True;
	}
	else if (strcmp(argv[i], "--bin") == 0)
	{
	    if (++i < argc)
	    {
		char *ptr1, *ptr2;

		ptr1 = addToEnv("PATH", argv[i], True);
		ptr2 = makeAbsPathEnv("PATH");
		if (gStruct->pathEnv)
		    XtFree(gStruct->pathEnv);
		if (ptr1)
		    XtFree(ptr1);
		gStruct->pathEnv = ptr2;
	    }
	}
	else if (strcmp(argv[i], "--keep") == 0)
	{
	    gStruct->keepWorkDir = True;
	}
	else if (strcmp(argv[i], "-chapter") == 0)
	{
	    gStruct->tocElemIndex++;
	}
	else if (strcmp(argv[i], "-id") == 0)
	{
	    if (++i < argc)
		gStruct->id = argv[i];
	}
	else if (strcmp(argv[i], "-title") == 0)
	{
	    if (++i < argc)
		gStruct->title = argv[i];
	}
	else if (strcmp(argv[i], "-parser") == 0)
	{
	    if (++i < argc)
		gStruct->parser = argv[i];
	}
	else
	{
	    fprintf(stderr, "%s: unrecognized option %s ignored\n",
		    EXEC_NAME, argv[i]);
	}
    }

    return i;
}

static char *
parseDocument(Boolean runCmd, ...)
{
    va_list ap;
    char *ptr;
    char *cmd = (char *)NULL;
    int cmdLen = 0;
    int maxLen = 0;
    Boolean parserIsNSGMLS;

    parserIsNSGMLS = (strcmp(gStruct->parser, "nsgmls") == 0);

    if (!checkStat(gStruct->sgml, FSTAT_IS_DIR | FSTAT_IS_READABLE))
	dieRWD(-1, "%s: faulty installation: %s\n",
	       EXEC_NAME, strerror(errno));

    addCatFile(buildPath("%s/infolib/%s/SGML/catalog",
			 STR(gStruct->install), STR(gStruct->clang)), False);
    addCatFile(buildPath("%s/infolib/%s/SGML/catalog",
			 STR(gStruct->install), LANG_COMMON), True);

    if (parserIsNSGMLS)
    {
	if (!gStruct->sgmlSearchPathEnv)
	    gStruct->sgmlSearchPathEnv = addToEnv("SGML_SEARCH_PATH", ".", False);
    }
    else
    {
	ptr = addToEnv("SGML_PATH", "%S", False);
	if (gStruct->sgmlPathEnv)
	    XtFree(gStruct->sgmlPathEnv);
	gStruct->sgmlPathEnv = ptr;
    }

    appendStr(&cmd, &cmdLen, &maxLen, gStruct->parser);

    if (parserIsNSGMLS) {
	appendStr(&cmd, &cmdLen, &maxLen, " -bidentity ");
	appendStr(&cmd, &cmdLen, &maxLen, gStruct->sgmlCatFiles);
    }

    if (runCmd)
    {
	appendStr(&cmd, &cmdLen, &maxLen, " -sg ");

	if (! parserIsNSGMLS)
	    appendStr(&cmd, &cmdLen, &maxLen, gStruct->decl);

	va_start(ap, runCmd);
	while ((ptr = va_arg(ap, char *)) != 0)
	{
	    appendStr(&cmd, &cmdLen, &maxLen, " ");
	    appendStr(&cmd, &cmdLen, &maxLen, ptr);
	}
	va_end(ap);

	runShellCmd(cmd);
	XtFree(cmd);
	return (char *)NULL;
    }

    if (parserIsNSGMLS)
	appendStr(&cmd, &cmdLen, &maxLen, "-oline -wno-idref ");
    else
	appendStr(&cmd, &cmdLen, &maxLen, " -l ");

    appendStr(&cmd, &cmdLen, &maxLen, gStruct->decl);
    va_start(ap, runCmd);
    while ((ptr = va_arg(ap, char *)) != 0)
    {
	appendStr(&cmd, &cmdLen, &maxLen, " ");
	appendStr(&cmd, &cmdLen, &maxLen, ptr);
    }
    va_end(ap);

    return cmd;
}

static void
buildBookcase(char *cmdSrc, char *dirName)
{
    int ret1;
    char *ret2;
    char *dataBase;
    char *tmpFile;
    char *newMmdbPathEnv;
    char *bookCaseDir;
    char *bookCaseMap;
    char cmd[MAXPATHLEN * 3];
    char bookCaseName[MAXPATHLEN + 1]; /* Not paths, just big buffers. */
    char bookCaseDesc[MAXPATHLEN + 1];

    dataBase = makeWorkDir();

    tmpFile = storeBookCase(cmdSrc, "all", dataBase, dirName);
    newMmdbPathEnv = buildPath("MMDB_PATH=%s", STR(gStruct->library));
    if (gStruct->mmdbPathEnv)
	XtFree(gStruct->mmdbPathEnv);
    gStruct->mmdbPathEnv = newMmdbPathEnv;
    putenv(newMmdbPathEnv);

    if (gStruct->verbose)
	fprintf(stderr, "%s\n", newMmdbPathEnv);

    if (!findBookCaseNameAndDesc(tmpFile, bookCaseName, bookCaseDesc))
	dieRWD(-1, "%s: Missing Bookcase name\n", EXEC_NAME);
    XtFree(tmpFile);

    bookCaseDir = buildPath("%s/%s", STR(gStruct->library), bookCaseName);
    if (checkStat(bookCaseDir, FSTAT_IS_DIR))
    {
	fprintf(stderr, "%s: deleting existing %s ...\n",
		EXEC_NAME, bookCaseDir);
	snprintf(cmd, sizeof(cmd), "rm -rf %s", bookCaseDir);
	runShellCmd(cmd);

	if (checkStat(bookCaseDir, FSTAT_IS_DIR))
	    dieRWD(-1, "%s: failed to delete %s\n",
		   EXEC_NAME, bookCaseDir);
    }

    snprintf(cmd, sizeof(cmd), "dbdrv define %s %s \"%s\"",
	    gStruct->spec, bookCaseName, bookCaseDesc);
    runShellCmd(cmd);

    bookCaseMap = buildPath("%s/bookcase.map", gStruct->library);
    editMapFile(bookCaseName, bookCaseMap);

    /* 
     * changed not to use pipe for better error handling 
     */
    { 

      const char* style_file = makeTmpFile();

      snprintf(cmd, sizeof(cmd), "NCFGen -load-style %s %s > %s",
			bookCaseName, dataBase, style_file);

      runShellCmd(cmd);

      snprintf(cmd, sizeof(cmd), "cat %s | dbdrv stdin_load %s %s.stylesheet",
			style_file, bookCaseName, bookCaseName);
      runShellCmd(cmd);

      snprintf(cmd, sizeof(cmd), "rm -f %s", style_file);
      ret1 = system(cmd);
      if(ret1 != 0) die(-1, "system for rm failed; exiting...\n");
      XtFree((char*)style_file);
    }


    {

      const char* compress_file = makeTmpFile();

      snprintf(cmd, sizeof(cmd), "NCFGen -compressed %s %s > %s",
			bookCaseName, dataBase, compress_file);

      runShellCmd(cmd);

      snprintf(cmd, sizeof(cmd), "cat %s | dbdrv stdin_load %s %s.node",
			compress_file, bookCaseName, bookCaseName);
      runShellCmd(cmd);

      snprintf(cmd, sizeof(cmd), "rm -f %s", compress_file);
      ret1 = system(cmd);
      if(ret1 != 0) die(-1, "system for rm failed; exiting...\n");
      XtFree((char*)compress_file);
    }


    {
      const char* anonym_file = makeTmpFile();

      snprintf(cmd, sizeof(cmd), "MixedGen -compressed %s %s > %s",
			dataBase, bookCaseDir, anonym_file);

      runShellCmd(cmd);

      snprintf(cmd, sizeof(cmd), "cat %s | dbdrv mixed_load %s",
					anonym_file, bookCaseName);

      runShellCmd(cmd);

      snprintf(cmd, sizeof(cmd), "rm -f %s", anonym_file);
      ret1 = system(cmd);
      if(ret1 != 0) die(-1, "system for rm failed; exiting...\n");
      XtFree((char*)anonym_file);
    }

    validateBookCase(bookCaseMap, bookCaseName);
    XtFree(bookCaseMap);

    if (strcmp(gStruct->searchEngine, "dtsearch") == 0)
    {
	char curDir[MAXPATHLEN + 1];
	char newDir[MAXPATHLEN + 1];
	const char *dtsr_stp, *dtsr_sfx, *dtsr_knj;

	snprintf(cmd, sizeof(cmd), "mkdir -p %s/%s",
		bookCaseDir, gStruct->searchEngine);
	runShellCmd(cmd);

	snprintf(cmd, sizeof(cmd), "cp %s/%s/%s.fzk %s/%s",
		dataBase, gStruct->searchEngine, bookCaseName,
		bookCaseDir, gStruct->searchEngine);
	runShellCmd(cmd);

	snprintf(cmd, sizeof(cmd), "cp %s %s/%s",
		gStruct->dbdfile, bookCaseDir, gStruct->searchEngine);
	runShellCmd(cmd);

	if ((dtsr_stp = langtbl[gStruct->dtsridx].stp)) {
	    snprintf(cmd, sizeof(cmd), "cp %s/%s %s/%s/%s.stp",
		    gStruct->dtsrlib, dtsr_stp, bookCaseDir,
		    gStruct->searchEngine, bookCaseName);
	    runShellCmd(cmd);
	}

	if ((dtsr_sfx = langtbl[gStruct->dtsridx].sfx)) {
	    snprintf(cmd, sizeof(cmd), "cp %s/%s %s/%s/%s.sfx",
		    gStruct->dtsrlib, dtsr_sfx, bookCaseDir,
		    gStruct->searchEngine, bookCaseName);
	    runShellCmd(cmd);
	}

	if ((dtsr_knj = langtbl[gStruct->dtsridx].knj)) {
	    snprintf(cmd, sizeof(cmd), "cp %s/%s %s/%s/%s.knj",
		    gStruct->dtsrlib, dtsr_knj, bookCaseDir,
		    gStruct->searchEngine, bookCaseName);
	    runShellCmd(cmd);
	}

	curDir[0] = '\0';
	ret2 = getcwd(curDir, MAXPATHLEN);
	if(ret2 == (char *)NULL) die(-1, "getcwd failed; exiting...\n");
	snprintf(newDir, sizeof(newDir), "%s/%s",
				bookCaseDir, gStruct->searchEngine);
	if (chdir(newDir) != 0)
	    dieRWD(-1, "%s: Cannot find %s: %s\n",
		   EXEC_NAME, newDir, strerror(errno));

	snprintf(cmd, sizeof(cmd), "dtsrcreate %s-o -a%d -l%d %s",
		(gStruct->verbose) ? "" : "-q ", 210,
		langtbl[gStruct->dtsridx].dtsrlang, bookCaseName);
	runShellCmd(cmd);

	snprintf(cmd, sizeof(cmd), "dtsrload -d%s '-t\n' %s",
		bookCaseName, bookCaseName);
	runShellCmd(cmd);

	snprintf(cmd, sizeof(cmd), "dtsrindex -d%s '-t\n' %s",
		bookCaseName, bookCaseName);
	runShellCmd(cmd);

	snprintf(cmd, sizeof(cmd), "echo keytypes %s = %s > %s.ocf",
		bookCaseName, gStruct->keytypes, gStruct->searchEngine);
	runShellCmd(cmd);

	snprintf(cmd, sizeof(cmd), "%s.fzk", bookCaseName);
	unlink(cmd);
	snprintf(cmd, sizeof(cmd), "%s.dbd", gStruct->searchEngine);
	unlink(cmd);

	if (chdir(curDir) != 0)
	    dieRWD(-1, "%s: Cannot find %s: %s\n",
		   EXEC_NAME, curDir, strerror(errno));
    }
    XtFree(bookCaseDir);

    if (!gStruct->keepWorkDir)
	removeWorkDir();
}

static char *
storeBookCase(char *cmdSrc, char *tocOpt, char *dbName,
	      char *dirName)
{
    char *tmpFile;
    char *cmd;
    int ret;

    /*
     * changed not to use pipe for better error handling
     */
    {

      const char* nsgmls_file = makeTmpFile();

      cmd = buildPath("%s > %s;echo", cmdSrc, nsgmls_file);

      runShellCmd(cmd);

      tmpFile = makeTmpFile();

      cmd = buildPath("cat %s | NodeParser %s %s %s > %s",
			nsgmls_file, tocOpt, dbName, dirName, tmpFile);
      runShellCmd(cmd);

      cmd = buildPath("rm -f %s", nsgmls_file);
      ret = system(cmd);
      if(ret != 0) die(-1, "system for rm failed; exiting...\n");
      XtFree((char*)nsgmls_file);
    }

    XtFree(cmd);

    return tmpFile;
}

static Boolean
findBookCaseNameAndDesc(char *tmpFile, char *bookCaseName,
			char *bookCaseDesc)
{
    FILE *fp;
    char lineBuf[MAXPATHLEN + 1];
    char *p1, *p2;
    static char *patt1 = "BookCase name: `";
    static char *patt2 = "' desc: `";
    unsigned int len;

    if ((fp = fopen(tmpFile, "r")) == (FILE *)NULL)
	dieRWD(-1, "%s: opening %s: %s\n",
	       EXEC_NAME, tmpFile, strerror(errno));

    while (fgets(lineBuf, MAXPATHLEN, fp) != (char *)NULL)
    {
	if ((p1 = strstr(lineBuf, patt1)) != (char *)NULL)
	{
	    p1 += strlen(patt1);
	    if ((p2 = strstr(p1, patt2)) != (char *)NULL)
	    {
		len = p2 - p1;
		*((char *) memcpy(bookCaseName, p1, len) + len) = '\0';
		validateBookCaseName(bookCaseName);
		p1 = p2 + strlen(patt2);
		if ((p2 = strchr(p1, '\'')) != (char *)NULL)
		{
		    len = p2 - p1;
		    *((char *) memcpy(bookCaseDesc, p1, len) + len) = '\0';
		    fclose(fp);

		    return True;
		}
	    }
	}
    }
    fclose(fp);

    return False;
}

static void
validateBookCaseName(char *bookCaseName)
{
    Boolean isOk = False;

    if (strlen(bookCaseName) <= 8)
    {
	int i;

	for (i = 0; bookCaseName[i] != '\0'; i++)
	{
	    if (!isalnum((unsigned char) bookCaseName[i]))
		break;
	}

	isOk = (bookCaseName[i] == '\0');
    }

    if (!isOk)
	dieRWD(-1, "%s: Bookcase name `%s' is not valid;\n"
		   "only 8 alphanumeric characters are allowed\n",
	       EXEC_NAME, bookCaseName);
}

static void
validateBookCase(char *mapFile, char *bookCaseName)
{
    char *ret;
    FILE *fp;
    char lineBuf[MAXPATHLEN + 1];
    char cmdBuf[MAXPATHLEN + 1];
    char *bcName;

    if ((fp = fopen(mapFile, "r")) == (FILE *)NULL)
	dieRWD(-1, "%s: cannot open bookcase.map: %s\n",
	       EXEC_NAME, strerror(errno));

    ret = fgets(lineBuf, MAXPATHLEN, fp); /* Skip first line. */
    if(ret == (char *)NULL) die(-1, "fgets failed; exiting...\n");

    while (fgets(lineBuf, MAXPATHLEN, fp) != (char *)NULL)
    {
	if ((bcName = strtok(lineBuf, "\t\n")) != (char *)NULL)
	{
	    if (strcmp(bcName, bookCaseName) != 0)
	    {
		snprintf(cmdBuf, sizeof(cmdBuf), "valBase %s %s",
					bookCaseName, bcName);

		/* Should this return an error code instead of */
		/* exiting so that we can cleanup the bookcase? */
		runShellCmd(cmdBuf);
	    }
	}
    }

    fclose(fp);
}

static void
editMapFile(char *bookCaseName, char *bookCaseMap)
{
    size_t ret;
    struct stat statBuf;
    FILE *fp = NULL;
    char *file;
    char **fileVector;
    char *libDesc;
    char *oldDesc, *libID;
    char *firstLine;
    char **lineVector;
    char *lastLine;
    int i;
    int bcNameLen;
    Boolean replaced = False;

    if ((stat(bookCaseMap, &statBuf) != 0) ||
	((fp = fopen(bookCaseMap, "r")) == (FILE *)NULL))
	dieRWD(-1, "%s: %s: %s\n", EXEC_NAME, bookCaseMap,
	       strerror(errno));

    file = XtMalloc((statBuf.st_size + 1) * sizeof(char));
    ret = fread(file, statBuf.st_size, sizeof(char), fp);
    if(ret == 0) die(-1, "fread failed; exiting...\n");
    if (file[statBuf.st_size - 1] == '\n')
	file[statBuf.st_size - 1] = '\0';
    else file[statBuf.st_size] = '\0';
    fclose(fp);

    fileVector = _DtVectorizeInPlace(file, '\n');

    firstLine = XtsNewString(fileVector[0]);
    lineVector = _DtVectorizeInPlace(firstLine, '\t');
    if ((oldDesc = lineVector[0]) != (char *)NULL)
	libID = lineVector[1];
    else
	libID = (char *)NULL;
    XtFree((char *)lineVector);

    if ((libDesc = gStruct->libDesc) == (char *)NULL)
	libDesc = oldDesc;

    for (i = 0, lastLine = (char *)NULL; fileVector[i] != (char *)NULL; i++)
	lastLine = fileVector[i];

    if ((fp = fopen(bookCaseMap, "w")) == (FILE *)NULL)
	dieRWD(-1, "%s: %s: %s\n", EXEC_NAME, bookCaseMap,
	       strerror(errno));

    bcNameLen = strlen(bookCaseName);

    fprintf(fp, "%s\t%s\n", STR(libDesc), STR(libID));
    XtFree(firstLine);

    for (i = 1; fileVector[i] != (char *)NULL; i++)
    {
	if ((strncmp(fileVector[i], bookCaseName, bcNameLen) == 0) &&
	    (!isalnum((unsigned char) fileVector[i][bcNameLen])) &&
	    (fileVector[i][bcNameLen] != '_'))
	{
	    if (!replaced)
	    {
		fprintf(fp, "%s\n", lastLine);
		replaced = True;
	    }
	}
	else
	{
	    fprintf(fp, "%s\n", fileVector[i]);
	}
    }

    if (!replaced)
	fprintf(fp, "%s\n", lastLine);

    fclose(fp);

    XtFree((char *)fileVector);
    XtFree(file);
}

static void
buildTOC(int argc, char *argv[])
{
    FILE *fp;
    char lineBuf[MAXPATHLEN + 1];
    char idBuf[MAXPATHLEN + 1];
    char titleBuf[MAXPATHLEN + 1];
    char *p1, *p2;
    char *tocBC;
    char *tocDir;
    char *cmdSrc;
    unsigned int len;
    static char *patt1start = "<TOC id=\"";
    static char *patt1end = "\">";
    static char *patt2start = "<TITLE>";
    static char *patt2end = "</TITLE>";

    len = MIN(strlen(gStruct->id), MAXPATHLEN);
    *((char *) memcpy (idBuf, gStruct->id, len) + len) = '\0';
    len = MIN(strlen(gStruct->title), MAXPATHLEN);
    *((char *) memcpy (titleBuf, gStruct->title, len) + len) = '\0';

    if (checkStat(gStruct->outFile, FSTAT_IS_FILE))
    {
	if ((fp = fopen(gStruct->outFile, "r")) == (FILE *)NULL)
	    die(-1, "%s: %s: %s\n", EXEC_NAME, gStruct->outFile,
		strerror(errno));

	idBuf[0] = titleBuf[0] = '\0';
	while (fgets(lineBuf, MAXPATHLEN, fp) != (char *)NULL)
	{
	    if ((p1 = strstr(lineBuf, patt1start)) != (char *)NULL)
	    {
		p1 += strlen(patt1start);
		if ((p2 = strstr(p1, patt1end)) != (char *)NULL)
		{
		    len = p2 - p1;
		    *((char *) memcpy(idBuf, p1, len) + len) = '\0';
		}
	    }
	    if ((p1 = strstr(lineBuf, patt2start)) != (char *)NULL)
	    {
		p1 += strlen(patt2start);
		if ((p2 = strstr(p1, patt2end)) != (char *)NULL)
		{
		    len = p2 - p1;
		    *((char *) memcpy(titleBuf, p1, len) + len) = '\0';
		}
	    }
	}
	fclose(fp);

	if (idBuf[0] == '\0')
	    die(-1, "%s: %s has no ID\n", EXEC_NAME, gStruct->outFile);
	if (titleBuf[0] == '\0')
	    die(-1, "%s: %s has not TITLE\n", EXEC_NAME, gStruct->outFile);
    }

    tocBC = tocBookcase(argc, argv);
    cmdSrc = parseDocument(False, tocBC, 0);
    if ((tocDir = dirname(tocBC)) == (char *)NULL)
	tocDir = ".";
    tocDir = XtsNewString(tocDir);
    p1 = storeBookCase(cmdSrc, "toc", makeWorkDir(), tocDir);
    XtFree(p1);
    XtFree(tocDir);
    XtFree(cmdSrc);
    XtFree(tocBC);

    makeTOC(idBuf, titleBuf);

    if (!gStruct->keepWorkDir)
	removeWorkDir();
}

static char *
tocBookcase(int argc, char *argv[])
{
    char *tmpFile;
    FILE *fp;
    int i;

    tmpFile = makeTmpFile();
    if (gStruct->verbose)
	fprintf(stderr, "TOC Bookcase: %s\n", tmpFile);

    if ((fp = fopen(tmpFile, "w")) == (FILE *)NULL)
	dieRWD(-1, "%s: %s: %s\n", EXEC_NAME,
	       tmpFile, strerror(errno));

    fputs("<!DOCTYPE Bookcase PUBLIC \n"
	  "\"-//Common Desktop Environment//"
	  "DTD DtInfo Bookcase Description//EN\" \n"
	  "[\n"
	  "        <!-- Books -->\n",
	  fp);

    for (i = 0; i < argc; i++)
    {
	fprintf(fp, "\t<!ENTITY book%d   SYSTEM '%s' SUBDOC>\n",
		i + 1, argv[i]);
    }

    fputs("]>\n"
	  "<BOOKCASE StyleSheet=dummySty>\n"
	  "        <BOOKCASENAME>TOCDummy</>\n"
	  "        <BOOKCASEDESC>Dummy Bookcase for TOC Generation</>\n"
	  "        <StyleSheet name=\"dummySty\"><path>*</>"
	  "<online><linebreak after></></>\n"
	  "	<BOOK>\n"
	  "	<TITLE>Dummy Book for TOC Generation</>\n"
	  "	<TOCFILE></>\n",
	  fp);

    for (i = 0; i < argc; i++)
    {
	fprintf(fp, "\t<FILE>&book%d;</>\n", i + 1);
    }

    fputs("	</BOOK>\n"
	  "</BOOKCASE>\n",
	  fp);

    fclose(fp);

    return tmpFile;
}

static void
makeTOC(char *id, char *title)
{
    char *tocTitle;
    char *fileIn;
    char *trTitle;
    char *ptr1, *ptr2;
    FILE *fpIn;
    FILE *fpOut;
    TOCRecord *tocRecord;
    TOCEntry newTOCEntry;
    TOCEntry *tocEntries = (TOCEntry *)NULL;
    int nTOCEntries = 0;
    int maxTOCEntries = 0;
    int i, j;
    int level;
    int olvl;

    tocTitle = sgmlData(title);
    for (i = 0; id[i] != '\0'; i++)
    {
	if ((!isalnum((unsigned char) id[i])) &&
	    (id[i] != '.') && (id[i] != '-'))
		die(-1, "bad ID: %s\n", id);
    }

    fileIn = buildPath("%s/NodeMeta", makeWorkDir());
    if ((fpIn = fopen(fileIn, "r")) == (FILE *)NULL)
	dieRWD(-1, "%s: internal error: %s\n",
	       EXEC_NAME, strerror(errno));

    if ((fpOut = fopen(gStruct->outFile, "w")) == (FILE *)NULL)
	dieRWD(-1, "%s: %s: %s\n",
	       EXEC_NAME, gStruct->outFile, strerror(errno));

    while ((tocRecord = getTOCRecord(fpIn)))
    {
	char lineBuf[MAXPATHLEN + 1];

	if (tocRecord->code != NODE_META)
	    dieRWD(-1, "\n");

	trTitle = sgmlData(STR(tocRecord->title));

	ptr1 = replaceData(STR(tocRecord->loc), "&", "&#38;");
	ptr2 = replaceData(ptr1, "\"", "&#34;");
	XtFree(ptr1);

	snprintf(lineBuf, sizeof(lineBuf),
		"<TOCEntry LinkEnd=\"%s\">%s</> <!-- %s:%s -->\n",
		ptr2, trTitle, STR(tocRecord->file),
		STR(tocRecord->line));
	XtFree(ptr2);
	XtFree(trTitle);

	newTOCEntry.ord = STR(tocRecord->ord);
	newTOCEntry.entry = lineBuf;
	addTOCEntry(&tocEntries, &nTOCEntries, &maxTOCEntries,
		    &newTOCEntry);

	freeTOCRecord(tocRecord);
    }

    fclose(fpIn);

    fprintf(fpOut, "<!DOCTYPE TOC PUBLIC\n"
		   "    \"-//Common Desktop Environment//"
		   "DTD DtInfo Table of Contents//EN\">\n"
		   "<TOC id=\"%s\">\n"
		   "<TITLE>%s</TITLE>\n",
	    id, tocTitle);
    XtFree(tocTitle);

    level = -1;

    sortTOCEntries(tocEntries, nTOCEntries);
    for (i = 0; i < nTOCEntries; i++)
    {
	/*
	 * From the original PERL script (I don't understand the logic):
	 * ## The magic no. 6 here is based on the no. of sections that
	 * ## are allowed at one level. Currently , it is 99999, ie. only
	 * ## five digits are allowed
	 */
	olvl = strlen(tocEntries[i].ord) / 6;

	while (olvl <= level)
	{
	    for (j = 0; j < level * 4; j++)
		fputs(" ", fpOut);
	    fprintf(fpOut, "</%s>\n",
		    TOCElems[level + gStruct->tocElemIndex]);
	    level--;
	}

	while (olvl > level)
	{
	    level++;
	    fputs("\n", fpOut);
	    for (j = 0; j < level * 4; j++)
		fputs(" ", fpOut);
	    fprintf(fpOut, "<%s>\n",
		    TOCElems[level + gStruct->tocElemIndex]);
	}
	for (j = 0; j < level * 4; j++)
	    fputs(" ", fpOut);
	fputs(tocEntries[i].entry, fpOut);
    }
    freeTOCEntries(tocEntries, nTOCEntries);

    while (level >= 0)
    {
	for (j = 0; j < level * 4; j++)
	    fputs(" ", fpOut);
	fprintf(fpOut, "</%s>\n\n", TOCElems[level + gStruct->tocElemIndex]);
	level--;
    }

    fputs("</TOC>\n", fpOut);
    fclose(fpOut);
}

static char *
sgmlData(char *inData)
{
    char *outData;
    char *p1, *p2;

    p1 = replaceData(inData, "&", "&amp;");
    p2 = replaceData(p1, "<", "&lt;");
    outData = replaceData(p2, ">", "&gt;");

    XtFree(p1);
    XtFree(p2);

    return outData;
}

static char *
replaceData(char *inData, char *replaceMe, char *replacement)
{
    int i, slen, len;
    int newLen;
    int replaceMeLen = strlen(replaceMe);
    char *p, *endP;
    char *newData;

    p = inData;
    i = 0;
    while ((p = strstr(p, replaceMe)) != (char *)NULL)
    {
	i++;
	p += replaceMeLen;
    }

    if (i == 0)
	return XtsNewString(inData);

    newLen = strlen(inData) + (i * (strlen(replacement) - replaceMeLen));
    newData = XtMalloc((newLen + 1) * sizeof(char));
    newData[0] = '\0';
    p = inData;
    while ((endP = strstr(p, replaceMe)) != (char *)NULL)
    {
	slen = strlen(newData);
	len = endP - p;
	*((char *) memcpy(newData + slen, p, len) + len) = '\0';

	slen = strlen(newData);
	len = MIN(strlen(replacement), (newLen - slen) * sizeof(char));
	*((char *) memcpy(newData + slen, replacement, len) + len) = '\0';
	p = endP + replaceMeLen;
    }
    slen = strlen(newData);
    len = MIN(strlen(p), (newLen - slen) * sizeof(char));
    *((char *) memcpy(newData + slen, p, len) + len) = '\0';

    return newData;
}

static void
addTOCEntry(TOCEntry **tocEntries, int *nTOCEntries, int *maxEntries,
	    TOCEntry *newEntry)
{
    if (*nTOCEntries >= *maxEntries)
    {
	TOCEntry *newArray;

	newArray =
	    (TOCEntry *)XtRealloc((char *)*tocEntries,
				  (*maxEntries + 10) * sizeof(TOCEntry));
	if (!newArray)
	    return;

	*tocEntries = newArray;
	*maxEntries += 10;
    }

    (*tocEntries)[*nTOCEntries].ord = XtsNewString(newEntry->ord);
    (*tocEntries)[*nTOCEntries].entry = XtsNewString(newEntry->entry);
    (*nTOCEntries)++;
}

static int
compareTOCEntries(const void *entry1, const void *entry2)
{
    TOCEntry *tEntry1 = (TOCEntry *)entry1;
    TOCEntry *tEntry2 = (TOCEntry *)entry2;

    return strcmp(tEntry1->ord, tEntry2->ord);
}

static void
sortTOCEntries(TOCEntry *tocEntries, int nTOCEntries)
{
    qsort((void *)tocEntries, nTOCEntries, sizeof(TOCEntry),
	  compareTOCEntries);
}

static void
freeTOCEntries(TOCEntry *tocEntries, int nTOCEntries)
{
    int i;

    for (i = 0; i < nTOCEntries; i++)
    {
	XtFree(tocEntries[i].ord);
	XtFree(tocEntries[i].entry);
    }

    XtFree((char *)tocEntries);
}

static TOCRecord *
getTOCRecord(FILE *fp)
{
    char lineBuf[MAXPATHLEN + 1];
    int cols;
    TOCRecord *tocRecord;
    char *ptr;

    tocRecord = (TOCRecord *)XtMalloc(sizeof(TOCRecord));

    if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
    {
	XtFree((char *)tocRecord);
	return (TOCRecord *)NULL;
    }

    tocRecord->code = atoi(lineBuf);

    if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
    {
	XtFree((char *)tocRecord);
	return (TOCRecord *)NULL;
    }
    cols = atoi(lineBuf);

    if (cols < TOC_REC_COLS)
    {
	XtFree((char *)tocRecord);
	return (TOCRecord *)NULL;
    }

    /* DISCARD first item (book). */
    ptr = getTOCField(fp);
    XtFree(ptr);
    cols--;

    tocRecord->loc = getTOCField(fp);
    cols--;

    tocRecord->file = getTOCField(fp);
    cols--;

    tocRecord->line = getTOCField(fp);
    cols--;

    tocRecord->ord = getTOCField(fp);
    cols--;

    tocRecord->title = getTOCField(fp);
    cols--;

    while (cols > 0)
    {
	ptr = getTOCField(fp);
	XtFree(ptr);
	cols--;
    }

    return tocRecord;
}

static void
freeTOCRecord(TOCRecord *tocRecord)
{
    XtFree(tocRecord->loc);
    XtFree(tocRecord->file);
    XtFree(tocRecord->line);
    XtFree(tocRecord->ord);
    XtFree(tocRecord->title);

    XtFree((char *)tocRecord);
}

static char *
getTOCField(FILE *fp)
{
    char lineBuf[MAXPATHLEN + 1];
    char *ptr;
    int ptrLen;
    int lineType;
    char *longField = (char *)NULL;
    int lfLen = 0;
    int maxLen = 0;

    if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
	return (char *)NULL;
    if (strcmp(lineBuf, "#\n") == 0)
	return (char *)NULL;

    lineType = atoi(lineBuf);

    switch (lineType)
    {
    case COMPRESSED_STRING_CODE:
	if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
	    return (char *)NULL;

	/* FALL THROUGH! */

    case STRING_CODE:
	if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
	    return (char *)NULL;

	if ((ptr = strchr(lineBuf, '\t')) != (char *)NULL)
	    ptr++;

	break;

    case INTEGER_CODE:
    case OID_CODE:
	if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
	    return (char *)NULL;

	ptr = lineBuf;
	break;

    case SHORT_LIST_CODE:
	if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
	    return (char *)NULL;

	if (strcmp(lineBuf, "#\n") != 0)
	    dieRWD(-1, "\n");

	while ((ptr = getTOCField(fp)) != (char *)NULL)
	{
	    if (longField != (char *)NULL)
		appendStr(&longField, &lfLen, &maxLen, "#");
	    appendStr(&longField, &lfLen, &maxLen, ptr);

	    XtFree(ptr);
	}

	ptr = longField;
	break;

    case OID_LIST_CODE:
	if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
	    return (char *)NULL;

	if (strcmp(lineBuf, "#\n") != 0)
	    dieRWD(-1, "\n");

	for ( ; ; )
	{
	    if (fgets(lineBuf, MAXPATHLEN, fp) == (char *)NULL)
		return (char *)NULL;

	    /* Chop newline */
	    ptrLen = strlen(lineBuf);
	    if ((ptrLen > 0) && (lineBuf[ptrLen - 1] == '\n'))
		lineBuf[ptrLen - 1] = '\0';

	    if (strcmp(lineBuf, "#") == 0)
		break;

	    if (longField != (char *)NULL)
		appendStr(&longField, &lfLen, &maxLen, "#");
	    appendStr(&longField, &lfLen, &maxLen, lineBuf);
	}

	ptr = longField;
	break;

    default:
	fprintf(stderr, "Unknown code: %d\n", lineType);
	ptr = (char *)NULL;
	break;
    }

    if (ptr != (char *)NULL)
    {
	ptrLen = strlen(ptr);
	if ((ptrLen > 0) && (ptr[ptrLen - 1] == '\n'))
	    ptr[ptrLen - 1] = '\0';

	ptr = XtsNewString(ptr);

	if (longField != (char *)NULL)
	    XtFree(longField);

	return ptr;
    }

    return (char *)NULL;
}

int
main(int argc, char *argv[])
{
    GlobalsStruct globalsStruct;

    gStruct = &globalsStruct;

    if (argc < 2)
	printUsage((char *)NULL, -1);

    defaultGlobals();

    if (!doAdmin(argc, argv) &&
	!doBuild(argc, argv) &&
	!doTocgen(argc, argv) &&
	!doUpdate(argc, argv) &&
	!doValidate(argc, argv) &&
	!doHelp(argc, argv))
	printUsage(EXEC_NAME ": unrecognized subcommand `%s'\n", -1);

    return 0;
}
