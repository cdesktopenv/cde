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
/***************************************************************************
*
* File:         findnewrcs.c
* RCS:          $XConsortium: findnewrcs.c /main/3 1995/10/30 13:42:46 rswiston $
* Description:  The 'findnewrcs' command is used to descend a tree, checking
*               out the newest version of any RCS file which has been updated.
*               It can also be used to create a clone of a source tree using
*               symbolic links.
* Author:       Dave Serisky, Corvallis Workstation Operation
* Created:      Tue Jun  7 09:39:47 1988
* Modified:     Brian Cripe, (bcripe@hpcvlx) bcripe@hpcvxbc
* Modified:     Marc Ayotte, (marca@hpcvlx)
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
*
********************************************************************************
*/

static char version[] = "@(#) $XConsortium: findnewrcs.c /main/3 1995/10/30 13:42:46 rswiston $";

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef apollo
#ifndef S_ISLNK
#define S_ISLNK(m) (((m) & 0170000) == 0120000)
#endif
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & 0170000) == 0040000)
#endif
#ifndef S_ISREG
#define S_ISREG(m) (((m) & 0170000) == 0100000)
#endif
#define UID_NO_CHANGE -1
#include <sys/dir.h>
#else
# if defined(USL) && !defined(S_ISLNK)
# define S_ISLNK(m) ((m&0xF000) == S_IFLNK)
# endif
#include <dirent.h>
#endif

#ifndef __hpux
#define UID_NO_CHANGE -1
#endif

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#ifdef apollo
#include <sys/time.h>
#else
#include <utime.h>
#endif

extern char *getcwd();
extern char *malloc();
extern DIR *opendir();
#ifdef apollo
extern struct direct *readdir();
#else
extern struct dirent *readdir();
#endif
extern long telldir();
extern char *mktemp();

char Buffer[BUFSIZ];
char *WorkingDirCmp;
int WorkingLen;
char *SourceDirCmp;
int SourceLen;

void GetNew();
char *ResolveLink();

static int Debug = 0;
static int Quiet = 0;
static int Verbose = 0;
static int MakeDirs = 0;
static int DoDiffs = 0;
static int SourcePriority = 0;
static int NoRcsLinks = 0;
static int NoRealFiles = 0;
static int LinkFiles = 0;
static int CloneDotA = 0;
static int CloneDotO = 0;
static int CloneMakefile = 0;
static int CloneExecutables = 0;
static int Check = 0;
static int DoExecute = 0;
static int FollowDirLinks = 0;
static int ResolveFileLinks = 1;
static int ProcessSCCSdirs = 0;
static char *DiffCommand = "diff";

static char *OutFile = "/tmp/,outXXXXXX";
static char *Makefile = "Makefile";

typedef struct _flist {
    char *fullname;
    char *keyname;
    int keylen;
    int used;
    struct stat st;
    int st_valid;
    struct stat lst;
    int lst_valid;
    struct _flist *next;
} flist;

void
Usage(ProgName)
char *ProgName;
{
    (void) fprintf(stderr, "\
usage: %s [options] [-W <working dir>] [-S <RCS dir>] [subdir ...] \n\
\n\
options:\n\
    -i[Aaoxm]\n\
        clone:  A - all the following\n\
            a - \"*.a\" files\n\
            o - \"*.o\" files\n\
            x - executables ('x' mode bits set)\n\
            m - \"Makefile*\" files\n\
    -v  verbose (the more, the verboser)\n\
        source files\n\
    -L  take checked out source over RCS regardless of time\n\
    -O  don't check out new files, Only create symbolic links to exiting files\n\
          (this option is for cloning build trees from controlled source trees)\n\
    -R  don't create any RCS links\n\
    -l  symbolicly link files instead of copying them\n\
    -m  make any missing directories\n\
    -q  quiet\n\
    -d  diff new versions of existing files\n\
    -c<cmd> use <cmd> to do diffs (default is diffc)\n\
    -o<out> write diffs to <out> instead of /tmp/,out??????\n\
    -W<dir> use working directory <dir> (default is \".\")\n\
    -S<dir> use RCS/source directory <dir>\n\
        (default is /RCS directories of working tree)\n\
    -f  follow source-tree links to non-RCS dirs (old behavior)\n\
    -X  execute instead of generating shell script\n\
    -C  process SCCS directories if present\n\
    -s  do not resolve symbolic links for files. use file name\n\
", ProgName);
    (void) exit(1);
}

static void do_rm_rf(), do_touch(), do_ln_s(), do_diff(), do_rm_f();
static void do_cp(), do_chmod(), do_co_q(), do_mkdir(), do_chgrp();

main(argc, argv)
int argc;
char **argv;
{
    char *WorkingDir = (char *) 0;
    char *SourceDir = (char *) 0;
    register int i;
    register char *c;
    int same = 0;
    struct stat st;

    extern int getopt();
    extern char *optarg;
    extern int optind;
    extern int opterr;

    (void) setvbuf(stdout, (char *) 0, _IOLBF, BUFSIZ);
    OutFile = mktemp(OutFile);

    while ((i = getopt(argc, argv, "-S:-W:i:OLRXflsCmqvd~c:o:")) != EOF) {
    switch (i) {
    case 'W' :
        /* Working directory.  Default is current directory */
        WorkingDir = optarg;
        break;

    case 'S' :
        /* Source directory.  Default is to use working directories
        ** /RCS directories.
        */
        SourceDir = optarg;
        break;

    case 'i' :
        /* Parse off no ignore options */
        for (c = optarg; *c; c++) {
        switch (*c) {
        case 'A' :
            /* don't ignore any... */
            (void) CloneDotA++;
            (void) CloneDotO++;
            (void) CloneExecutables++;
            (void) CloneMakefile++;
            break;

        case 'a' :
            /* don't ignore *.a files */
            (void) CloneDotA++;
            break;

        case 'o' :
            /* don't ignore *.o files */
            (void) CloneDotO++;
            break;

        case 'x' :
            /* don't ignore executables */
            (void) CloneExecutables++;
            break;

        case 'm' :
            /* don't ignore Makefiles */
            (void) CloneMakefile++;
            break;

        default :
            (void) Usage(argv[0]);
            (void) exit(1);
            break;
        }
        }
        break;

    case 'R' :
        /* dont create any links for "/RCS" */
        (void) NoRcsLinks++;
        break;

    case 'L' :
        /* Take existing checked out files even if they are older than
        ** the RCS copy.
        */
        (void) SourcePriority++;
        break;
    
    case 'O' :
        /* Don't check out new files, just create sym links to existing files.
        ** This is to protect controlled source/build trees from having
	** files bypass the crt process.
        */
        (void) NoRealFiles++;
        break;
    
    case 'f' :
        FollowDirLinks = 1;
        break;

    case 'C' :
        ProcessSCCSdirs = 1;
        break;

    case 'X' :
        DoExecute = 1;
        break;

    case 'l' :
        /* Link files (when possible) instead of creating them. */
        (void) LinkFiles++;
        break;

    case 'm' :
        /* Make any directories necessary. */
        (void) MakeDirs++;
        break;

    case 'q' :
        /* Be quiet. */
        (void) Quiet++;
        break;

    case 'v' :
        /* Increase verbosity. */
        (void) Verbose++;
        break;

    case 'd' :
        /* Diff out new files. */
        (void) DoDiffs++;
        break;

    case 'c' :
        /* Alternative diff command. */
        DiffCommand = optarg;
        break;

    case 'o' :
        /* Output file name for diffs */
        OutFile = optarg;
        break;

    case 's' :
        /* don't resolve symbolic links for files */
        ResolveFileLinks = 0;
        break;

    case '~' :
        /* Increment debug switch */
        (void) Debug++;
        break;
    
    default:
        (void) Usage(argv[0]);
        (void) exit(1);
        break;
    }
    }

    /* no WorkingDir specified or not root relative? */
    if (!WorkingDir || !*WorkingDir || ('/' != *WorkingDir)) {
    /* get the current directory */
    if (NULL == (c = getcwd(Buffer, sizeof(Buffer)))) {
        (void) fprintf(stderr, "%s: can't get current working directory\n",
            argv[0]);
        (void) exit(1);
    }

    /* did they specify a partial path? */
    if (WorkingDir && *WorkingDir) {
        /* prepend the path to the working dir */
        char *c2;

        c2 = malloc((unsigned) (strlen(c) + strlen(WorkingDir) + 2));
        (void) sprintf(c2, "%s/%s", c, WorkingDir);
        WorkingDir = c2;
    } else {
        /* working dir is current path */
        WorkingDir = malloc((unsigned) (strlen(c) + 1));
        (void) strcpy(WorkingDir, c);
    }
    }

    /* SourceDir specified? */
    if (!SourceDir || !*SourceDir) {
    /* Let's see if there is an /RCS dir in WorkingDir */
    SourceDir = malloc((unsigned) (strlen(WorkingDir) + 5));
    (void) strcpy(SourceDir, WorkingDir);
    (void) strcat(SourceDir, "/RCS");
    if (!lstat(SourceDir, &st)) {
        if (S_ISLNK(st.st_mode)) {
        SourceDir = ResolveLink(SourceDir, 1);
        }
    }
    /* strip off the "/RCS" portion of it */
    c = strrchr(SourceDir, '/');
    if (*c && !strcmp(c, "/RCS"))
        *c = '\0';
    (void) fprintf(stderr, "using sourcedir of \"%s\"\n", SourceDir);
    } else {
    /* is it root relative? */
    if ('/' != *SourceDir) {
        /* get the current directory */
        char *c2;

        if (NULL == (c = getcwd(Buffer, sizeof(Buffer)))) {
        (void) fprintf(stderr,
            "%s: can't get current working directory\n",
            argv[0]);
        (void) exit(1);
        }

        c2 = malloc((unsigned) (strlen(c) + strlen(SourceDir) + 2));
        (void) sprintf(c2, "%s/%s", c, SourceDir);
        SourceDir = c2;
    }
    }

    /* are the 2 directories the same name?
    */
    if (!strcmp(SourceDir, WorkingDir)) {
    (void) same++;
    }

    WorkingDirCmp = malloc((unsigned) (strlen(WorkingDir) + 2));
    (void) strcpy(WorkingDirCmp, WorkingDir);
    (void) strcat(WorkingDirCmp, "/");

    SourceDirCmp = malloc((unsigned) (strlen(SourceDir) + 2));
    (void) strcpy(SourceDirCmp, SourceDir);
    (void) strcat(SourceDirCmp, "/");

    WorkingLen = strlen(WorkingDirCmp);
    SourceLen = strlen(SourceDirCmp);

    if (DoExecute)
    {
    if (DoDiffs)
    {
        do_rm_rf(OutFile);
        do_touch(OutFile);
    }
    }
    else
    {
    printf("#!/bin/sh\n");
    printf("#\n# the following makes things more readeable\n\n");
    printf("w='%s'\t# Working directory\n", WorkingDir);
    printf("s='%s'\t# Source directory\n", SourceDir);
    if (DoDiffs)
        printf("rm -rf %s; touch %s\n", OutFile, OutFile);
    printf("\n");
    }

    if (optind < argc) {
    char *SubSourceDir;
    char *SubWorkingDir;

    for (; optind < argc; optind++) {
        SubSourceDir = malloc((unsigned) (strlen(SourceDir) +
            strlen(argv[optind]) + 2));
        (void) sprintf(SubSourceDir, "%s/%s", SourceDir, argv[optind]);
        SubWorkingDir = malloc((unsigned) (strlen(WorkingDir) +
            strlen(argv[optind]) + 2));
        (void) sprintf(SubWorkingDir, "%s/%s", WorkingDir, argv[optind]);
        (void) GetNew(SubWorkingDir, SubSourceDir, same);
        (void) free(SubWorkingDir);
        (void) free(SubSourceDir);
    }
    } else {
    (void) GetNew(WorkingDir, SourceDir, same);
    }
    (void) exit(0);
/*NOTREACHED*/
}

void
GetNew(WorkingDir, SourceDir, same)
char *WorkingDir;
char *SourceDir;
int same;
{
    FILE *f;
    char *NextSourceDir;
    char *NextWorkingDir;
    char *RcsDir;
    char *SourceFile;
    char *WorkingFile;
    int FreeSourceFile;
    int FreeWorkingFile;
    DIR *D;
#ifdef apollo
    struct direct *dir;
#else
    struct dirent *dir;
#endif
    struct stat st;
    int Tries;
    char *c,*d;
    char buf[BUFSIZ];
    flist *WorkingHead = (flist *) 0;
    flist *SourceHead = (flist *) 0;
    flist *RcsHead = (flist *) 0;
    flist *WorkingPtr;
    flist *SourcePtr;
    flist *RcsPtr;
    int shouldnt_have_source;

    if (Verbose)
    (void) fprintf(stderr, "Scanning directory \"%s\"\n", SourceDir);
    if (Debug > 1)
    (void) fprintf(stderr, ">> GetNew(\"%s\", \"%s\")\n", WorkingDir,
        SourceDir);

    /* set up the working directory's RCS directory name */
    RcsDir = malloc((unsigned char *) strlen(WorkingDir) + strlen("/RCS") + 1);
    (void) sprintf(RcsDir, "%s/RCS", WorkingDir);
    if (Debug > 0) 
    (void) fprintf(stderr, ">> RcsDir=\"%s\"\n", RcsDir);

    /* check to see if the working dir's RCS directory exists */
    /* if you can't stat it and it is a symlink to nowhere, do not
     * recreate the link. The cde source server works on RCS symlinks
     * to nowhere.
     */
    if (stat(RcsDir, &st)) { 
    /* if not a symlink, create the RCS directory */
        if (readlink(RcsDir, buf, sizeof(buf)) < 0) {
            /* no -- create one off of the SourceDir */
            c = malloc((unsigned char *) strlen(SourceDir) +
                strlen("/RCS") + 1);
            (void) sprintf(c, "%s/RCS", SourceDir);
            d = malloc((unsigned char *) strlen(SourceDir) +
                strlen("/RCS") + 1);
            (void) sprintf(d, "%s/RCS", SourceDir);
            /* resolve out symbolic links */
            c = ResolveLink(c, 1);
            if (Debug > 0)
                (void) fprintf(stderr, ">> RCS RcsDir=\"%s\"\n", c);

            /* do we want RCS links? */
            /* If Rcsdir points to a symlink, recreate the link
            * even if no destination directory exists.
            */
            if ((!stat(c, &st) || (readlink(d, buf, sizeof(buf)) > 0)) 
                && !NoRcsLinks) {
                /* yes -- since the WorkingDir didn't have an /RCS directory,
                ** let's make one.
                */
                if (!Quiet)
                    (void) fprintf(stderr, "%s: creating symlink\n",
                        RcsDir + WorkingLen);
                /* check and see if the RCS dir is in the SourceDir */
                if (DoExecute)
                {
                    do_rm_f(RcsDir);
                    do_ln_s(c, RcsDir);
                }
                else
                {
                    /* remove the link before linking */
                    (void) printf("rm -f ${w}/%s\n",
                            RcsDir + WorkingLen);
                    if (strncmp(c, SourceDir, SourceLen) ||
                        (c[SourceLen] != '/')) {
                        /* need full RCS path */
                        (void) printf("ln -s %s ${w}/%s\n",
                            c, RcsDir + WorkingLen);
                    } 
                    else {
                        /* RCS path is relative to SourceDir */
                        (void) printf("ln -s ${s}/%s ${w}/%s\n",
                            c + SourceLen, RcsDir + WorkingLen);
                    }
                }
            }
            (void) free(RcsDir);
            RcsDir = c;
        }
    }

    /* resolve "fake" symbolic links to get a real directory */
    for (Tries = 10; Tries > 0; Tries--) {
    if (!lstat(RcsDir, &st)) {
        if (S_ISLNK(st.st_mode)) {
        RcsDir = ResolveLink(RcsDir, 1);
        } else if (S_ISREG(st.st_mode)) {
        /* open the file... */
        if (f = fopen(RcsDir, "r")) {
            /* read it... */
            (void) fgets(Buffer, sizeof(Buffer), f);
            Buffer[strlen(Buffer) - 1] = '\0';
            (void) free(RcsDir);

            /* RcsDir is the contents of the file... */
            RcsDir = malloc((unsigned) strlen(Buffer) + 1);
            (void) strcpy(RcsDir, Buffer);
            if (Debug)
            (void) fprintf(stderr, ">> soft link RcsDir=\"%s\"\n",
                RcsDir);

            /* we are done, close it... */
            (void) fclose(f);
        }
        } else {
        /* not a symlink or "soft" link */
        break;
        }
    } else {
        /* directory does not exist */
        break;
    }
    }
    if (Tries <= 0) {
    if (!Quiet)
        (void) fprintf(stderr, "%s: >10 \"soft\" links\n", RcsDir);
    }

    /* cd to the RcsDir, and scan it */
    RcsHead = (flist *) 0;
    if (chdir(RcsDir)) {
    if (errno != ENOENT)
        (void) perror(RcsDir);
    } else {
    if (D = opendir(".")) {
        while (dir = readdir(D)) {
        /* all RCS files have a length > 2 */
#ifndef apollo
        if (strlen(dir->d_name) < 3)
#else
        if (dir->d_namlen < 3)
#endif
            continue;
        
        /* all RCS files have a name ending in ",v" */
#ifndef apollo
        if (strcmp(dir->d_name + strlen(dir->d_name) - 2, ",v"))
#else
        if (strcmp(dir->d_name + dir->d_namlen - 2, ",v"))
#endif
            continue;

        /* build the full name of the file */
        (void) strcpy(Buffer, RcsDir);
        (void) strcat(Buffer, "/");
        (void) strcat(Buffer, dir->d_name);

        /* stat the file */
        if (stat(dir->d_name, &st)) {
            (void) perror(Buffer);
            continue;
        }

        /* Valid entry.  It's time to stuff it away */
        if (RcsHead) {
            RcsPtr->next = (flist *) malloc(sizeof(flist));
            RcsPtr = RcsPtr->next;
        } else {
            RcsHead = (flist *) malloc(sizeof(flist));
            RcsPtr = RcsHead;
        }
        /* null out link */
        RcsPtr->next = (flist *) 0;

        /* this entry is new */
        RcsPtr->used =0;

        /* stuff away full path */
        RcsPtr->fullname = malloc((unsigned) (strlen(Buffer) + 1));
        (void) strcpy(RcsPtr->fullname, Buffer);

        /* drop ",v" of dir->d_name */
        /* stuff away key path */
#ifndef apollo
                dir->d_name[strlen(dir->d_name) - 2] = '\0';
                RcsPtr->keyname = malloc((unsigned) (strlen(dir->d_name) + 1));
                (void) strcpy(RcsPtr->keyname, dir->d_name);
                RcsPtr->keylen = strlen(dir->d_name);
#else
        dir->d_name[dir->d_namlen - 2] = '\0';
        dir->d_namlen -= 2;
        RcsPtr->keyname = malloc((unsigned) (dir->d_namlen + 1));
        (void) strcpy(RcsPtr->keyname, dir->d_name);
        RcsPtr->keylen = dir->d_namlen;
#endif
        /* stuff away stat structure */
        RcsPtr->st = st;
        RcsPtr->st_valid = 1;
        RcsPtr->lst_valid = 0;
        }
        (void) closedir(D);
    }
    }

    /* cd to the WorkingDir, and scan it */
    WorkingHead = (flist *) 0;
    if (chdir(WorkingDir)) {
    if (errno != ENOENT)
        (void) perror(WorkingDir);
    } else {
    if (D = opendir(".")) {
        while (dir = readdir(D)) {
        /* ignore "." and ".." */
#ifndef apollo
                if (('.' == *(dir->d_name)) || (!strcmp(dir->d_name, "..")))
                    continue;
#else
        if (((1 == dir->d_namlen) && ('.' == *(dir->d_name))) ||
            ((2 == dir->d_namlen) && !strcmp(dir->d_name, "..")))
            continue;
#endif
        
        /* build the full name of the file */
        (void) strcpy(Buffer, WorkingDir);
        (void) strcat(Buffer, "/");
        (void) strcat(Buffer, dir->d_name);

        /* stat the file */
        if (stat(dir->d_name, &st)) {
            /* don't send error if RCS directory is a
             * symlink to nowhere.
             */
            if (strcmp("RCS",dir->d_name)) {
                (void) perror(Buffer); 
            }
            continue;
        }

        /* Valid entry.  It's time to stuff it away */
        if (WorkingHead) {
            WorkingPtr->next = (flist *) malloc(sizeof(flist));
            WorkingPtr = WorkingPtr->next;
        } else {
            WorkingHead = (flist *) malloc(sizeof(flist));
            WorkingPtr = WorkingHead;
        }
        /* null out link */
        WorkingPtr->next = (flist *) 0;

        /* this entry is new */
        WorkingPtr->used =0;

        /* stuff away full path */
        WorkingPtr->fullname = malloc((unsigned) (strlen(Buffer) + 1));
        (void) strcpy(WorkingPtr->fullname, Buffer);
        /* stuff away key path */
#ifndef apollo
                WorkingPtr->keyname = malloc((unsigned) (strlen(dir->d_name) + 1));
                (void) strcpy(WorkingPtr->keyname, dir->d_name);
                WorkingPtr->keylen = strlen(dir->d_name);
#else
        WorkingPtr->keyname = malloc((unsigned) (dir->d_namlen + 1));
        (void) strcpy(WorkingPtr->keyname, dir->d_name);
        WorkingPtr->keylen = dir->d_namlen;
#endif
        /* stuff away stat structure */
        WorkingPtr->st = st;
        WorkingPtr->st_valid = 1;
        if (S_ISDIR(st.st_mode) && !lstat(dir->d_name, &st))
        {
            WorkingPtr->lst = st;
            WorkingPtr->lst_valid = 1;
        }
        else
            WorkingPtr->lst_valid = 0;
        }
        (void) closedir(D);
    }
    }

    /* cd to the SourceDir, and scan it.  If the dirs are the same, we
    ** can just forget this part
    */
    SourceHead = (flist *) 0;
    if (!same) {
    if (chdir(SourceDir)) {
        if (errno != ENOENT)
        (void) perror(SourceDir);
    } else {
        if (D = opendir(".")) {
        while (dir = readdir(D)) {
            /* ignore "." and ".." */
#ifndef apollo
                    if ((('.' == *(dir->d_name))) ||
                            (!strcmp(dir->d_name, "..")))
                        continue;
#else
            if (((1 == dir->d_namlen) && ('.' == *(dir->d_name))) ||
                ((2 == dir->d_namlen) &&
                !strcmp(dir->d_name, "..")))
            continue;
#endif
            
            /* build the full name of the file */
            (void) strcpy(Buffer, SourceDir);
            (void) strcat(Buffer, "/");
            (void) strcat(Buffer, dir->d_name);

            /* lstat the file */
            if (lstat(dir->d_name, &st)) {
            (void) perror(Buffer);
            continue;
            }


            /* Valid entry.  It's time to stuff it away */
            if (SourceHead) {
            SourcePtr->next = (flist *) malloc(sizeof(flist));
            SourcePtr = SourcePtr->next;
            } else {
            SourceHead = (flist *) malloc(sizeof(flist));
            SourcePtr = SourceHead;
            }
            /* null out link */
            SourcePtr->next = (flist *) 0;

            /* this entry is new */
            SourcePtr->used =0;

            /* stuff away full path */
            SourcePtr->fullname =
                malloc((unsigned) (strlen(Buffer) + 1));
            (void) strcpy(SourcePtr->fullname, Buffer);
            /* stuff away key path */
#ifndef apollo
                    SourcePtr->keyname = malloc((unsigned) (strlen(dir->d_name) + 1));
                    (void) strcpy(SourcePtr->keyname, dir->d_name);
                    SourcePtr->keylen = strlen(dir->d_name);
#else
            SourcePtr->keyname = malloc((unsigned) (dir->d_namlen + 1));
            (void) strcpy(SourcePtr->keyname, dir->d_name);
            SourcePtr->keylen = dir->d_namlen;
#endif
            /* stuff away lstat structure */
            SourcePtr->lst = st;
            SourcePtr->lst_valid = 1;
            /* was it a symbolic link? */
            if (S_ISLNK(st.st_mode)) {
            /* do a plain stat on it */
            (void) stat(dir->d_name, &st);
            }
            /* stuff it away */
            SourcePtr->st = st;
            SourcePtr->st_valid = 1;
        }
        (void) closedir(D);
        }
    }
    }

    /*
    ** PASS 1:
    **
    **      Check all the files in the working dir.
    */
    if (Debug)
    (void) fprintf(stderr, ">> Pass 1: scanning WorkingDir \"%s\"\n",
        WorkingDir);
    for (WorkingPtr = WorkingHead; WorkingPtr; WorkingPtr = WorkingPtr->next) {
    /* initialize */
    if (Debug > 1)
        (void) fprintf(stderr, ">>  keyname=\"%s\", fullname=\"%s\"\n",
            WorkingPtr->keyname, WorkingPtr->fullname);
    shouldnt_have_source = 0;

    /* ignore .o and .a files */
    c = WorkingPtr->keyname + WorkingPtr->keylen - 2;
    if (WorkingPtr->keylen >= 2) {
        if (!strcmp(c, ".o")) {
        (void) shouldnt_have_source++;
        if (!CloneDotO) {
            if (Debug > 1)
            (void) fprintf(stderr, ">>   .o -- ignored\n");
            continue;
        }
        }
        if (!strcmp(c, ".a")) {
        (void) shouldnt_have_source++;
        if (!CloneDotA) {
            if (Debug > 1)
            (void) fprintf(stderr, ">>   .a -- ignored\n");
            continue;
        }
        }
    }
    if (!strncmp(WorkingPtr->keyname, Makefile, strlen(Makefile))) {
        (void) shouldnt_have_source++;
        if (!CloneMakefile) {
        if (Debug > 1)
            (void) fprintf(stderr, ">>   Makefile* -- ignored\n");
        continue;
        }
    }

    /* ignore RCS as well */
    if (!strcmp(WorkingPtr->keyname, "RCS")) {
        if (Debug > 1)
        (void) fprintf(stderr, ">>   RCS -- ignored\n");
        continue;
    }

    /* ignore SCCS as well */
    if (!(ProcessSCCSdirs)) {
        if (!strcmp(WorkingPtr->keyname, "SCCS")) {
            if (Debug > 1)
                (void) fprintf(stderr, ">>   SCCS -- ignored\n");
            continue;
        }
    }

    /* ignore regular files with x mode */
    if (!CloneExecutables && WorkingPtr->st_valid &&
        (S_ISREG(WorkingPtr->st.st_mode)) &&
        (0111 == (WorkingPtr->st.st_mode & 0111))) {
        if (Debug > 1)
        (void) fprintf(stderr, ">>   executable -- ignored\n");
        continue;
    }

    /* find corresponding sourcefile */
    for (SourcePtr = SourceHead; SourcePtr; SourcePtr = SourcePtr->next) {
        if ((WorkingPtr->keylen == SourcePtr->keylen) &&
            !strcmp(WorkingPtr->keyname, SourcePtr->keyname)) {
        /* mark it as used */
        (void) SourcePtr->used++;
        break;
        }
    }
    
    if (Debug > 1)
        if (SourcePtr)
        (void) fprintf(stderr,
            "   source: keyname=\"%s\"  fullname=\"%s\"\n",
            SourcePtr->keyname, SourcePtr->fullname);
        else
        (void) fprintf(stderr, "   no source file\n");

    /* is the working file actually a directory? */
    if (WorkingPtr->st_valid &&
        (S_ISDIR(WorkingPtr->st.st_mode))) {
        /* yes, go recursive... */
        if (FollowDirLinks ||
        !WorkingPtr->lst_valid ||
        !S_ISLNK(WorkingPtr->lst.st_mode))
        {
            if (Debug > 1)
            (void) fprintf(stderr, ">>   directory\n");
            (void) GetNew(WorkingPtr->fullname,
                SourcePtr ? SourcePtr->fullname : WorkingPtr->fullname,
                same);
        }
    } else {
        /* find corresponding RCS file */
        for (RcsPtr = RcsHead; RcsPtr; RcsPtr = RcsPtr->next) {
        if ((WorkingPtr->keylen == RcsPtr->keylen) &&
            !strcmp(WorkingPtr->keyname, RcsPtr->keyname)) {
            /* mark it as used */
            (void) RcsPtr->used++;
            break;
        }
        }

        if (Debug > 1)
        if (RcsPtr)
            (void) fprintf(stderr,
                "   rcs: keyname=\"%s\"  fullname=\"%s\"\n",
                RcsPtr->keyname, RcsPtr->fullname);
        else
            (void) fprintf(stderr, "   no rcs file\n");

        /* print a message if there is no corresponding source */
        if (!RcsPtr && !SourcePtr) {
        /* don't print a message for .snf files */
        if ((WorkingPtr->keylen > 4) && !strcmp(WorkingPtr->keyname +
            WorkingPtr->keylen - 4, ".snf"))
            (void) shouldnt_have_source++;

        if (Verbose && !shouldnt_have_source)
            (void) fprintf(stderr, "%s: no source or RCS file\n",
                WorkingPtr->fullname + WorkingLen);
        }

        if (!RcsPtr || !RcsPtr->st_valid ||
            (SourcePtr &&
            (SourcePtr->st.st_mtime >= RcsPtr->st.st_mtime)) ||
            (SourcePriority && SourcePtr)) {
        /* no RCS file, or source newer than RCS, or source
        ** exists and has priority
        */
        if (Debug > 1)
            (void) fprintf(stderr, ">>   source newer than rcs\n");
        if (SourcePtr &&
            (WorkingPtr->st.st_mtime < SourcePtr->st.st_mtime)) {
            if (Debug > 1)
            (void) fprintf(stderr,
                ">>   source newer than working\n");
            /* does the WorkingFile have any write mode bits
            ** set?
            */
            if (WorkingPtr->st.st_mode & 0222) {
            if (!Quiet)
                (void) fprintf(stderr,
                    "%s: writable -- not updated\n",
                    WorkingPtr->fullname + WorkingLen);
            continue;
            }

            if (!Quiet)
            (void) fprintf(stderr, "%s: newer file\n",
                WorkingPtr->fullname + WorkingLen);


            /* check to see if file symlinks should resolve */
            if (ResolveFileLinks)
                if (SourcePtr->lst_valid &&
                 S_ISLNK(SourcePtr->lst.st_mode)) {
                    SourceFile = ResolveLink(SourcePtr->fullname, 0);
                    FreeSourceFile = 1;
                } 
                else {
                    SourceFile = SourcePtr->fullname;
                }
            else {
                SourceFile = SourcePtr->fullname;
            }

            /* do we need to do a diff? */
            if (DoDiffs) {
            if (DoExecute)
            {
                do_diff(NULL,
                    DiffCommand,
                    WorkingPtr->fullname,
                    SourceFile,
                    OutFile);
            }
            else
            {
                if (strncmp(SourceFile, SourceDirCmp, SourceLen))
                    /* not in SourceDir... */
                    (void) printf("%s ${w}/%s %s >>%s 2>&1\n",
                        DiffCommand,
                        WorkingPtr->fullname + WorkingLen,
                        SourceFile, OutFile);
                else
                    /* in SourceDir... */
                    (void) printf("%s ${w}/%s ${s}/%s >>%s 2>&1\n",
                        DiffCommand,
                        WorkingPtr->fullname + WorkingLen,
                        SourceFile + SourceLen, OutFile);
            }
            }
            if (DoExecute)
            {
            do_rm_f(WorkingPtr->fullname);
            if (LinkFiles)
                do_ln_s(SourceFile, WorkingPtr->fullname);
            else
                do_cp(SourceFile, WorkingPtr->fullname);
            }
            else
            {
                if (strncmp(SourceFile, SourceDirCmp, SourceLen)) {
                /* not in SourceDir... */
                (void) printf("rm -f ${w}/%s\n",
                    WorkingPtr->fullname + WorkingLen);
                (void) printf("\t%s %s ${w}/%s\n",
                    LinkFiles ? "ln -s" : "cp",
                    SourceFile,
                    WorkingPtr->fullname + WorkingLen);
                } else {
                /* in SourceDir... */
                (void) printf("rm -f ${w}/%s\n",
                    WorkingPtr->fullname + WorkingLen);
                (void) printf("\t%s ${s}/%s ${w}/%s\n",
                    LinkFiles ? "ln -s" : "cp",
                    SourceFile + SourceLen,
                    WorkingPtr->fullname + WorkingLen);
                }
            }
            if (!LinkFiles)
            {
            /* don't chmod the file if we did a link... */

            if (DoExecute)
            {
                do_chmod(WorkingPtr->fullname,
                (SourcePtr->st.st_mode & 07777 & ~0222) | 0444);
            }
            else
                (void) printf("\tchmod 0%03o ${w}/%s\n",
                 (SourcePtr->st.st_mode & 07777 & ~0222) | 0444,
                 WorkingPtr->fullname + WorkingLen);
            }
            if (FreeSourceFile)
            (void) free(SourceFile);
        } else {
            if (Debug > 1)
            (void) fprintf(stderr,
                ">>   source older than working\n");
        }
        } else {
        /* RCS file is the newest */
        if (Debug > 1)
            (void) fprintf(stderr, ">>   rcs newer than source\n");
        if (RcsPtr && (WorkingPtr->st.st_mtime < RcsPtr->st.st_mtime)) {
            if (Debug > 1)
            (void) fprintf(stderr,
                ">>   rcs newer than working\n");
            /* does the WorkingFile have any write mode bits
            ** set?
            */
            if (WorkingPtr->st.st_mode & 0222) {
            if (!Quiet)
                (void) fprintf(stderr,
                    "%s: writable -- not updated\n",
                    WorkingPtr->fullname + WorkingLen);
            continue;
            }


            if (!Quiet)
            (void) fprintf(stderr, "%s: newer RCS file\n",
                WorkingPtr->fullname + WorkingLen);
          if (DoExecute)
          {
            do_diff(RcsDir, DiffCommand,
                RcsPtr->keyname,
                WorkingPtr->fullname,
                OutFile);
	    if (! NoRealFiles)
	    {
               do_co_q(RcsDir, WorkingPtr->fullname);
	    }
          }
          else
          {
            if (strncmp(RcsDir, SourceDir, SourceLen) ||
                (RcsDir[SourceLen] != '/')) {
            /* RcsDir is not in SourceDir */

            if (DoDiffs) {
                /* diff the file */
                (void) printf("\t (cd %s; %s %s ${w}/%s  >>%s 2>&1)\n",
                    RcsDir,
                    DiffCommand,
                    RcsPtr->keyname,
                    WorkingPtr->fullname + WorkingLen,
                    OutFile);
            }
	      if (! NoRealFiles)
              {
               /* check out the file */
               (void) printf("(cd %s; co -q ${w}/%s) \n",
                   RcsDir, WorkingPtr->fullname + WorkingLen);
              }

            } else {
            /* RcsDir is in SourceDir */

            if (DoDiffs) {
                /* diff the file */
                (void) printf("\t (cd ${s}/%s; %s %s ${w}/%s  >>%s 2>&1)\n",
                    RcsDir + SourceLen,
                    DiffCommand,
                    RcsPtr->keyname,
                    WorkingPtr->fullname + WorkingLen,
                    OutFile);
            }

              if (! NoRealFiles)
              {
               /* check out the file */
               (void) printf("(cd ${s}/%s; co -q ${w}/%s) \n",
                    RcsDir + SourceLen, WorkingPtr->fullname + WorkingLen);
              }
            }
          }

        } else {
            if (Debug > 1)
            (void) fprintf(stderr,
                ">>   source older than working\n");
        }
        }
    }
    }

    /*
    ** PASS 2:
    **      Check all the files in the source (checked out) dir.
    */
    if (Debug)
    (void) fprintf(stderr, ">> Pass 2: scanning SourceDir \"%s\"\n",
        SourceDir);
    for (SourcePtr = SourceHead; SourcePtr; SourcePtr = SourcePtr->next) {
    /* did we get this one already? */
    if (SourcePtr->used)
        continue;

    /* ignore "RCS" */
    if (!strcmp(SourcePtr->keyname, "RCS"))
        continue;

    /* ignore SCCS as well */
    if (!(ProcessSCCSdirs)) {
        if (!strcmp(SourcePtr->keyname, "SCCS")) {
            if (Debug > 1)
                (void) fprintf(stderr, ">>   SCCS -- ignored\n");
            continue;
        }
    }

    /* find corresponding RCS file */
    for (RcsPtr = RcsHead; RcsPtr; RcsPtr = RcsPtr->next) {
        if ((SourcePtr->keylen == RcsPtr->keylen) &&
            !strcmp(SourcePtr->keyname, RcsPtr->keyname)) {
        /* mark it as used */
        break;
        }
    }

    /* which one do we use, RCS or source? */
    if (RcsPtr) {
        if (!SourcePriority &&
            (SourcePtr->st.st_mtime < RcsPtr->st.st_mtime)) {
        /* we'll catch this on pass 3 */
        continue;
        } else {
        /* mark the RCS file as used */
        (void) RcsPtr->used++;
        }
    }

    /* ignore .o and .a files */
    c = SourcePtr->keyname + SourcePtr->keylen - 2;
    if (SourcePtr->keylen >= 2) {
        if (!strcmp(c, ".o")) {
        (void) shouldnt_have_source++;
        if (!CloneDotO)
            continue;
        }
        if (!strcmp(c, ".a")) {
        (void) shouldnt_have_source++;
        if (!CloneDotA)
            continue;
        }
    }

    /* the files might be Makefile's */
    if (!CloneMakefile && !strncmp(SourcePtr->keyname, Makefile,
        strlen(Makefile)))
        continue;

    WorkingFile = malloc((unsigned) strlen(WorkingDir) +
        SourcePtr->keylen + 2);
    (void) strcpy(WorkingFile, WorkingDir);
    (void) strcat(WorkingFile, "/");
    (void) strcat(WorkingFile, SourcePtr->keyname);

    if (S_ISDIR(SourcePtr->st.st_mode)) {
        /* check for directories present in the source tree,
        ** but not in this tree.  Don't bother if the SourceDir
        ** is the same as WorkingDir.  If the directory exists
        ** in the working tree, we already set the used flag
        ** and never got here in the first place.
        */
        if (!same) {
        if (MakeDirs) {
            if (!Quiet)
            (void) fprintf(stderr, "%s: building directory\n",
                WorkingFile + WorkingLen);
            if (!FollowDirLinks &&
            SourcePtr->lst_valid &&
            S_ISLNK(SourcePtr->lst.st_mode))
            {

            /* check to see if file symlinks should resolve */
            if (ResolveFileLinks)
                SourceFile = ResolveLink(SourcePtr->fullname, 0);
            else
                SourceFile = SourcePtr->fullname;

            if (DoExecute)
                do_ln_s(SourceFile, WorkingFile);
            else
            {
                if (strncmp(SourceFile, SourceDir, SourceLen))
                (void) printf("ln -s %s ${w}/%s\n",
                    SourceFile,
                    WorkingFile + WorkingLen);
                else
                (void) printf("ln -s ${s}/%s ${w}/%s\n",
                    SourceFile + SourceLen,
                    WorkingFile + WorkingLen);
            }
            free(SourceFile);
            }
            else
            {
            if (DoExecute)
            {
                do_rm_rf(WorkingFile);
                do_mkdir(WorkingFile);
                do_chmod(WorkingFile, 0755);
                /* bad for other architectures */
                /* do_chgrp(WorkingFile, "users"); */
            }
            else
            {
                (void) printf("rm -rf ${w}/%s\n",
                    WorkingFile + WorkingLen);
                (void) printf("\tmkdir ${w}/%s\n",
                    WorkingFile + WorkingLen);
                (void) printf("\tchmod 0755 ${w}/%s\n",
                    WorkingFile + WorkingLen);
                /* bad for other architectures */
                /*(void) printf("\tchgrp users ${w}/%s\n",
                    WorkingFile + WorkingLen); */
            }
                (void) GetNew(WorkingFile, SourcePtr->fullname, same);
            }
                
        } else {
            if (!Quiet)
            (void) fprintf(stderr, "%s: missing directory\n",
                WorkingFile + WorkingLen);
        }
        }
    } else {
        /* regular file (probably)... */
        /* check to see if file symlinks should resolve */
            if (ResolveFileLinks)
            if (SourcePtr->lst_valid && S_ISLNK(SourcePtr->lst.st_mode)) {
            SourceFile = ResolveLink(SourcePtr->fullname, 0);
            FreeSourceFile = 1;
            } 
        else {
            SourceFile = SourcePtr->fullname;
            FreeSourceFile = 0;
            }
            else {
        SourceFile = SourcePtr->fullname;
        FreeSourceFile = 0;
            }

        if (!Quiet)
        (void) fprintf(stderr, "%s: new file\n",
            WorkingFile + WorkingLen);
        if (DoExecute)
        {
        if (LinkFiles)
            do_ln_s(SourceFile, WorkingFile);
        else
            do_cp(SourceFile, WorkingFile);
        }
        else
        {
            if (strncmp(SourceFile, SourceDir, SourceLen)) {
            (void) printf("%s %s ${w}/%s\n",
                LinkFiles ? "ln -s" : "cp",
                SourceFile,
                WorkingFile + WorkingLen);
            } else {
            (void) printf("%s ${s}/%s ${w}/%s\n",
                LinkFiles ? "ln -s" : "cp",
                SourceFile + SourceLen,
                WorkingFile + WorkingLen);
            }
        }
        if (!LinkFiles)
        {
        if (DoExecute)
            do_chmod(WorkingFile,
                (SourcePtr->st.st_mode & 07777 & ~0222) | 0444);
        else
            (void) printf("\tchmod 0%03o ${w}/%s\n",
                (SourcePtr->st.st_mode & 07777 & ~0222) | 0444,
                WorkingFile + WorkingLen);
        }
        if (FreeSourceFile)
        (void) free(SourceFile);
    }
    (void) free(WorkingFile);
    }

    /*
    ** PASS 3:
    **
    **      Check all the files in the RCS dir.
    */
    if (Debug)
    (void) fprintf(stderr, ">> scanning RcsDir \"%s\"\n", RcsDir);
    for (RcsPtr = RcsHead; RcsPtr; RcsPtr = RcsPtr->next) {
    /* did we get this one already? */
    if (RcsPtr->used)
        continue;

    /* build the WorkingFile */
    WorkingFile = malloc((unsigned) strlen(WorkingDir) +
        RcsPtr->keylen + 2);
    (void) strcpy(WorkingFile, WorkingDir);
    (void) strcat(WorkingFile, "/");
    (void) strcat(WorkingFile, RcsPtr->keyname);

    /* New RCS file */
    if (!Quiet)
      (void) fprintf(stderr, "%s: new RCS file\n",
                WorkingFile + WorkingLen);
    if (DoExecute)
    {
	if (! NoRealFiles)
        {
            do_co_q(RcsDir, WorkingFile);
        }
    }
    else
    {
        if (strncmp(RcsDir, SourceDir, SourceLen) ||
            (RcsDir[SourceLen] != '/')) {
            /* RcsDir is not in SourceDir */
	    if (! NoRealFiles)
            {
	        /* check it out... */
                (void) printf("(cd %s; co -q ${w}/%s)\n",
                     RcsDir,
                     WorkingFile + WorkingLen);
	    }
        } else {
            /* RcsDir is in SourceDir */
	    if (! NoRealFiles)
	    {
                 /* check it out... */
                 (void) printf("(cd %s; co -q ${w}/%s)\n",
                      RcsDir + SourceLen,
                      WorkingFile + WorkingLen);
	     }
        }
    }
    (void) free(WorkingFile);
    }

    /* free storage */
    while (WorkingHead) {
    WorkingPtr = WorkingHead;
    WorkingHead = WorkingPtr->next;
    (void) free(WorkingPtr->fullname);
    (void) free(WorkingPtr->keyname);
    (void) free((char *) WorkingPtr);
    }
    while (SourceHead) {
    SourcePtr = SourceHead;
    SourceHead = SourcePtr->next;
    (void) free(SourcePtr->fullname);
    (void) free(SourcePtr->keyname);
    (void) free((char *) SourcePtr);
    }
    while (RcsHead) {
    RcsPtr = RcsHead;
    RcsHead = RcsPtr->next;
    (void) free(RcsPtr->fullname);
    (void) free(RcsPtr->keyname);
    (void) free((char *) RcsPtr);
    }

    (void) free(RcsDir);
    return;
}

char
*ResolveLink(filename, freeit)
char *filename;
int freeit;
{
    int len;
    int try;
    char *c;
    char *tname;

    tname = filename;

    if (Debug) {
    (void) fprintf(stderr, "ResolveLink %s->", tname);
    (void) fflush(stderr);
    }
    for (try = 0; try < 10; try++) {
    if ((len = readlink(tname, Buffer, sizeof(Buffer))) < 0) {
        break;
    } else {
        Buffer[len] = '\0';
        if ('/' != *Buffer) {
        if (c = strrchr(tname, '/')) {
            *c = '\0';
            c = malloc((unsigned) (strlen(tname) +
                strlen(Buffer) + 2));
            (void) sprintf(c, "%s/%s", tname, Buffer);
        } else {
            c = malloc((unsigned) (strlen(Buffer) + 1));
            (void) strcpy(c, Buffer);
        }
        } else {
        c = malloc((unsigned) (strlen(Buffer) + 1));
        (void) strcpy(c, Buffer);
        }
        if (freeit++)
        (void) free(tname);
        tname = c;
    }
    }
    if (!freeit) {
    c = malloc((unsigned) (strlen(tname) + 1));
    (void) strcpy(c, tname);
    tname = c;
    }
    if (Debug) {
    (void) fprintf(stderr, "%s\n", tname);
    (void) fflush(stderr);
    }
    return(tname);
}

/* New code for -X option */

static void do_rm_rf(target)
char *target;
{
    char *malloc(), *temp;
    struct stat buf;
    if (lstat(target, &buf) != -1)
    {
        if (S_ISDIR(buf.st_mode))
    {
        DIR *thisdir;
#ifdef apollo
        struct direct *thisent;
#else
        struct dirent *thisent;
#endif
        thisdir = opendir(target);
        if (thisdir != NULL)
        {
        while (thisent = readdir(thisdir))
        {
            if (strcmp(thisent->d_name, ".") &&
                strcmp(thisent->d_name, ".."))
            {
            temp = malloc(strlen(target) +
                      strlen(thisent->d_name) + 2);
            if (temp != NULL)
            {
                (void)strcpy(temp, target);
                (void)strcat(temp, "/");
                (void)strcat(temp, thisent->d_name);
                    do_rm_rf(temp);
                free(temp);
            }
            }
        }
        closedir(thisdir);
        (void)rmdir(target);
        }
    }
    else
        (void)unlink(target);
    }
}

static void do_touch(filename)
char *filename;
{
#ifdef apollo
    struct timeval tvp[2];
    struct timezone tzp;
    gettimeofday(tvp, &tzp);
    tvp[1] = tvp[0];
    if (utimes(filename, tvp) == -1 &&
    errno == ENOENT)
#else
    if (utime(filename, (struct utimbuf *)NULL) == -1 &&
    errno == ENOENT)
#endif
    (void)creat(filename, 0777);
}

static void do_ln_s(src, dest)
char *src, *dest;
{
    if (symlink(src, dest) == -1)
    perror(dest);
}

static void do_diff(dirname, diffcmd, file1, file2, outfile)
char *dirname, *diffcmd, *file1, *file2, *outfile;
{
    pid_t pid, res, fork();

    pid = fork();

    if (pid == -1)
    perror("fork() failed");
    else
    {
    if (pid == 0)
    {
        if (dirname != NULL && chdir(dirname) == -1)
        perror("dirname");
        else
        {
        int des;
        des = open(outfile, O_WRONLY | O_APPEND | O_CREAT, 0777);
        if (des == -1)
        {
            perror(outfile);
            exit(1);
        }
        (void)dup2(des, fileno(stdout));
        (void)dup2(des, fileno(stderr));
        execlp(diffcmd, diffcmd, file1, file2, NULL);
        perror(diffcmd);
        }
        exit(1);
    }
    while ((res = wait(NULL)) != pid && res != -1);
    }
}

static void do_rm_f(target)
char *target;
{
    struct stat buf;
    if (lstat(target, &buf) != -1 &&
        !(S_ISDIR(buf.st_mode)))
    (void)unlink(target);
}

static void do_cp(src, dest)
char *src, *dest;
{
    int res1, res2, fd1, fd2;
    char buf[BUFSIZ];

    fd1 = open(src, O_RDONLY);
    if (fd1 == -1)
    {
    perror(src);
    return;
    }
    fd2 = open(dest, O_WRONLY | O_CREAT, 0777);
    if (fd2 == -1)
    {
    perror(dest);
    close(fd1);
    return;
    }

    while ((res1 = read(fd1, buf, BUFSIZ)) > 0)
    {
    while ((res2 = write(fd2, buf, res1)) > 0 && (res1 -= res2));
    if (res2 == -1) break;
    }

    if (res1 == -1)
    perror(src);
    else if (res2 == -1)
    perror(dest);

    close(fd1);
    close(fd2);
}

static void do_chmod(filename, newmode)
char *filename;
int newmode;
{
    if (chmod(filename, newmode) == -1)
    perror(filename);
}

static void do_co_q(dirname, targetname)
char *dirname, *targetname;
{
    pid_t pid, res, fork();

    pid = fork();

    if (pid == -1)
    perror("fork() failed");
    else
    {
    if (pid == 0)
    {
        if (chdir(dirname) == -1)
        perror("dirname");
        else
        {
        execlp("co", "co", "-q", targetname, NULL);
        perror("failed to exec co(1)");
        }
        exit(1);
    }
    while ((res = wait(NULL)) != pid && res != -1);
    }
}

static void do_mkdir(dirname)
char *dirname;
{
    if (mkdir(dirname, 0777) == -1)
    perror(dirname);
}

static void do_chgrp(filename, newgroup)
char *filename, *newgroup;
{
    struct group *group, *getgrnam();
    group = getgrnam(newgroup);
    if (group == NULL)
    fprintf(stderr, "%s: no such group\n", newgroup);
    else
    chown(filename, UID_NO_CHANGE, group->gr_gid);
}
