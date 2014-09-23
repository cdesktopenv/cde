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
 * $XConsortium: make_file.c /main/6 1996/06/28 08:31:16 mustafa $
 * 
 * @(#)make_file.c	3.53 19 Jan 1995	cde_app_builder/src/abmf
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * Create a make file that can be used to test the generated application.
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ab/util_types.h>
#include <ab_private/abio.h>
#include "abmfP.h"
#include "obj_namesP.h"
#include "utilsP.h"
#include "write_cP.h"
#include "make_fileP.h"

#define MAX_LIBS	32

#define CODEGEN_MACRO	"DTCODEGEN"

typedef unsigned AbmfLibDesc;
#define LibNone		((AbmfLibDesc)0)
#define LibDtHelp 	((AbmfLibDesc)1)
#define LibDtSvc 	((AbmfLibDesc)2)
#define LibDtTerm 	((AbmfLibDesc)3)
#define LibDtWidget 	((AbmfLibDesc)4)
#define LibX11 		((AbmfLibDesc)5)
#define LibXext 	((AbmfLibDesc)6)
#define LibXm 		((AbmfLibDesc)7)
#define LibXt 		((AbmfLibDesc)8)
#define LibWlarchive	((AbmfLibDesc)9)
#define LibWldefault	((AbmfLibDesc)10)
#define LibTt		((AbmfLibDesc)11)
#define LibICE          ((AbmfLibDesc)12)
#define LibXp           ((AbmfLibDesc)13)
#define LibSM           ((AbmfLibDesc)14)

typedef struct
{
    int		numLibs;
    AbmfLibDesc	libs[MAX_LIBS];
} AbmfLibsRec, *AbmfLibs;

#define ABMF_LIB_PREPEND	(-1)
#define ABMF_LIB_APPEND		(-2)
#define ABMF_LIB_REPLACE_DUP	(-3)
#define ABMF_LIB_REJECT_DUP	(-4)
#define ABMF_LIB_COMPLAIN_DUP	(-5)

static int	lib_find(AbmfLibs libs, AbmfLibDesc lib);
static int	lib_add(AbmfLibs libs, AbmfLibDesc lib, int where, int how);
static int	determine_libs(AbmfLibs libs, ABObj project, AB_OS_TYPE osType);

static STRING	cvt_to_obj(STRING fileName);
static int write_os_params(
			File		makeFile,
			AB_OS_TYPE	osType,
			AbmfLibs	libs
			);
static int write_aix_stuff(File makeFile, AbmfLibs libs);
static int write_hpux_stuff(File makeFile, AbmfLibs libs);
static int write_sunos_params(File makeFile, AbmfLibs libs);
static int write_unixware_params(File makeFile, AbmfLibs libs);
static int write_uxp_params(File makeFile, AbmfLibs libs);
static int write_osf1_stuff(File makeFile, AbmfLibs libs);
static int write_lnx_params(File makeFile, AbmfLibs libs);
static int write_fbsd_params(File makeFile, AbmfLibs libs);
static int write_nbsd_params(File makeFile, AbmfLibs libs);
static int write_obsd_params(File makeFile, AbmfLibs libs);

static int	determine_aix_libs(AbmfLibs libs, ABObj project);
static int	determine_hpux_libs(AbmfLibs libs, ABObj project);
static int	determine_sunos_libs(AbmfLibs libs, ABObj project);
static int	determine_unixware_libs(AbmfLibs libs, ABObj project);
static int	determine_uxp_libs(AbmfLibs libs, ABObj project);
static int      determine_osf1_libs(AbmfLibs libs, ABObj project);
static int      determine_lnx_libs(AbmfLibs libs, ABObj project);
static int      determine_fbsd_libs(AbmfLibs libs, ABObj project);
static int      determine_nbsd_libs(AbmfLibs libs, ABObj project);
static int      determine_obsd_libs(AbmfLibs libs, ABObj project);

static int	write_file_header(
			GenCodeInfo	genCodeInfo, 
			ABObj		project, 
			AB_OS_TYPE	osType
		);
static int	write_ab_params(
			GenCodeInfo	genCodeInfo, 
			ABObj		project
		);
static int	write_targets(
			GenCodeInfo	genCodeInfo,
			ABObj		project,
			AB_OS_TYPE	osType
		);

static int	write_local_libraries(
			File		makeFile, 
			AbmfLibs	libs,
			AB_OS_TYPE	osType
		);

static int write_codegen_macro(
			File		makeFile
		);

/*
 * Write makefile.
 */
int
abmfP_write_make_file(
		      GenCodeInfo 	genCodeInfo,
		      ABObj 		project,
		      AB_OS_TYPE	osType,
		      BOOL 		useSourceBrowser
)
{
    File	makeFile = genCodeInfo->code_file;
    AbmfLibsRec	libs;
    libs.numLibs = 0;

    determine_libs(&libs, project, osType);

    write_file_header(genCodeInfo, project, osType);
    write_os_params(makeFile, osType, &libs);
    write_codegen_macro(makeFile);
    write_ab_params(genCodeInfo, project);

    /*
     * Sun needs a NO_PARALLEL directive
     */
    if (osType == AB_OS_SUNOS)
    {
        abio_printf(genCodeInfo->code_file,
"\n"
".NO_PARALLEL: $(TARGETS.h) $(TARGETS.h.merged) $(TARGETS.c) $(TARGETS.c.merged)\n"
	);
    }

    write_targets(genCodeInfo, project, osType);

    return 0;
}


static int
determine_libs(AbmfLibs libs, ABObj project, AB_OS_TYPE osType)
{
    int			return_value = 0;
    CGenProjData 	projData = mfobj_get_proj_data(project);

    if (projData->has_ui_obj)
    {
	lib_add(libs, LibDtWidget, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibDtHelp, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibDtSvc, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibXm, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibXt, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibXext, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibX11, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibICE, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibSM, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibXp, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);

	if (osType == AB_OS_HPUX)
	{
	    lib_add(libs, LibTt, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);
	}
    }

    if (projData->has_terminal)
    {
	lib_add(libs, LibDtHelp, ABMF_LIB_PREPEND, ABMF_LIB_REJECT_DUP);
	lib_add(libs, LibDtTerm, ABMF_LIB_PREPEND, ABMF_LIB_REJECT_DUP);
    }

#if defined(USL) || defined(__uxp__)
    if (1) /* Workaround because tooltalk not being used when needed */
#else
    if (obj_get_tooltalk_level(project) != AB_TOOLTALK_NONE)
#endif
	lib_add(libs, LibTt, ABMF_LIB_APPEND, ABMF_LIB_REJECT_DUP);

    switch(osType)
    {
	case AB_OS_AIX:
		return_value = determine_aix_libs(libs, project);
		break;

	case AB_OS_HPUX:
		return_value = determine_hpux_libs(libs, project);
		break;

	case AB_OS_SUNOS:
		return_value = determine_sunos_libs(libs, project);
		break;
        case AB_OS_OSF1:
                return_value = determine_osf1_libs(libs, project);
                break;
	case AB_OS_UNIXWARE:
		return_value = determine_unixware_libs(libs, project);
		break;
	case AB_OS_UXP:
		return_value = determine_uxp_libs(libs, project);
		break;
	case AB_OS_LNX:
		return_value = determine_lnx_libs(libs, project);
		break;
	case AB_OS_FBSD:
		return_value = determine_fbsd_libs(libs, project);
		break;
	case AB_OS_NBSD:
		return_value = determine_nbsd_libs(libs, project);
		break;
	case AB_OS_OBSD:
		return_value = determine_obsd_libs(libs, project);
		break;
    }

    return return_value;
}

static int
determine_aix_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}


static int
determine_hpux_libs(AbmfLibs libs, ABObj project)
{
    /*
    lib_add(libs, LibWlarchive, ABMF_LIB_PREPEND, ABMF_LIB_REPLACE_DUP);
    lib_add(libs, LibWldefault, ABMF_LIB_APPEND, ABMF_LIB_REPLACE_DUP);
    */
    return 0;
}


static int
determine_sunos_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static int
determine_uxp_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static int
determine_osf1_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static int
determine_unixware_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static int
determine_lnx_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static int
determine_fbsd_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static int
determine_nbsd_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static int
determine_obsd_libs(AbmfLibs libs, ABObj project)
{
    return 0;
}

static STRING
get_string_for_lib(AbmfLibDesc lib, AB_OS_TYPE osType)
{
    STRING	libString = NULL;

    switch (lib)
    {
	case LibNone:
	    libString = abmfP_str_empty;
	break;

	case LibDtHelp:
	    libString = "-lDtHelp";
	break;

	case LibDtSvc:
	    libString = "-lDtSvc";
	break;

	case LibDtTerm:
	    libString = "-lDtTerm";
	break;

	case LibDtWidget:
	    libString = "-lDtWidget";
	break;

	case LibTt:
	    libString = "-ltt";
	break;

	case LibX11:
	    libString = "-lX11";
	break;

	case LibXext:
	    libString = "-lXext";
	break;

	case LibXm:
	    libString = "-lXm";
	break;

	case LibXt:
	    libString = "-lXt";
	break;

	case LibXp:
	    libString = "-lXp";
	break;

	case LibICE:
	    libString = "-lICE";
	break;

	case LibSM:
	    libString = "-lSM";
	break;

	case LibWlarchive:
	    libString = "-Wl,-a,archive";
	break;

	case LibWldefault:
	    libString = "-Wl,-a,default";
	break;
    }

    return libString;
}


static int
lib_find(AbmfLibs libs, AbmfLibDesc lib)
{
    int i;
    int		numLibs = libs->numLibs;

    for (i = 0; i < numLibs; ++i)
    {
	if (libs->libs[i] == lib)
	{
	    return i;
	}
    }
    return ERR_NOT_FOUND;
}


static int
lib_add(AbmfLibs libs, AbmfLibDesc lib, int where, int how)
{
    int		pos = 0;
    int		old_pos = -1;
    
    if ((old_pos = lib_find(libs, lib)) >= 0)
    {
	if (how == ABMF_LIB_REJECT_DUP)
	{
	    return ERR_DUPLICATE_KEY;
	}
	if (how == ABMF_LIB_REPLACE_DUP)
	{
	    return ERR_NOT_IMPLEMENTED;
	}
    }
    if (libs->numLibs >= MAX_LIBS)
    {
	return ERR_DATA_SPACE_FULL;
    }

    /*
     * Figure out where to put it
     */
    pos = where;
    if (pos == ABMF_LIB_PREPEND)
    {
	pos = 0;
    }
    if (pos == ABMF_LIB_APPEND)
    {
	pos = libs->numLibs;
    }

    /*
     * Insert it
     */
    if (pos < libs->numLibs)
    {
	/* insertion */
	memmove(&(libs->libs[pos +1]), &(libs->libs[pos]),
		    ((libs->numLibs+1) - pos) * sizeof(libs->libs[0]));
	libs->libs[pos] = lib;
	++(libs->numLibs);
    }
    else
    {
	libs->libs[(libs->numLibs)++] = lib;
    }

    return 0;
}


static int
write_file_header(
			GenCodeInfo	genCodeInfo,
			ABObj		project,
			AB_OS_TYPE	osType
)
{
    STRING	osName = util_os_type_to_string(osType);
    File	makeFile = genCodeInfo->code_file;

    abio_printf(makeFile, 
    "\n"
"###########################################################################\n"
"#\n"
"#    CDE Application Builder Makefile\n"
"#\n"
"# This file was generated by `%s' from project file `%s'.\n"
"# The target platform for this Makefile is %s.\n"
"#\n"
"###########################################################################\n",
	util_get_program_name(),
	obj_get_file(project),
	osName);

    return 0;
}


static int
write_os_params(
			File		makeFile,
			AB_OS_TYPE	osType,
			AbmfLibs	libs
)
{
    int		return_value = 0;

    switch(osType)
    {
	case AB_OS_AIX:
		return_value = write_aix_stuff(makeFile, libs);
		break;

	case AB_OS_HPUX:
		return_value = write_hpux_stuff(makeFile, libs);
		break;

	case AB_OS_SUNOS:
		return_value = write_sunos_params(makeFile, libs);
		break;

	case AB_OS_UNIXWARE:
                return_value = write_unixware_params(makeFile, libs);
                break;

	case AB_OS_UXP:
                return_value = write_uxp_params(makeFile, libs);
                break;

        case AB_OS_OSF1:
                return_value = write_osf1_stuff(makeFile, libs);
                break;

        case AB_OS_LNX:
                return_value = write_lnx_params(makeFile, libs);
                break;

        case AB_OS_FBSD:
                return_value = write_fbsd_params(makeFile, libs);
                break;

        case AB_OS_NBSD:
                return_value = write_nbsd_params(makeFile, libs);
                break;

        case AB_OS_OBSD:
                return_value = write_obsd_params(makeFile, libs);
                break;
    }

    return return_value;
}


static int
write_aix_stuff(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_AIX);

    abio_printf(makeFile, 
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile, "\n");
     abio_puts(makeFile,
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = -DSYSV -DAIXV3 -D_ALL_SOURCE"
        " -D_AIX -D__AIX -D__aix -D_aix"
        " -Daix -DMULTIBYTE -DNLS16 -DMESSAGE_CAT -DAIX_ILS\n"
"        ANSI_DEFINES =\n"
"\n"
"        CDEBUGFLAGS = -g -D__STR31__\n"
"        COPTFLAGS = -O -D__STR31__\n"
"        SYS_LIBRARIES = -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n"
);

    write_local_libraries(makeFile, libs, AB_OS_AIX);

    abio_puts(makeFile,
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}


static int
write_hpux_stuff(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_HPUX);

    abio_printf(makeFile, 
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile, "\n");
     abio_puts(makeFile,
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = -DSYSV -DNLS16 -DMALLOC_0_RETURNS_NULL -DMERGE -DNDEBUG"
        " -D__hpux -Dhpux -DOSMAJORVERSION=9 -DOSMINORVERSION=0 -DSHMLINK"
        " -D__hp9000s800 -Dhp9000s800 -Dhp9000s700 -DHPPEX\n"
"        ANSI_DEFINES = -Aa -D_HPUX_SOURCE -DANSICPP\n"
"\n"
"        CDEBUGFLAGS = -g -z\n"
"        COPTFLAGS = -O -z\n"
"        SYS_LIBRARIES = -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n"
);

    write_local_libraries(makeFile, libs, AB_OS_HPUX);

    abio_puts(makeFile,
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}


static int
write_sunos_params(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_SUNOS);

    abio_printf(makeFile, 
    "\n"
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile,
"\n"
".KEEP_STATE:\n"
"\n"
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = \n"
"        ANSI_DEFINES = \n"
"\n"
"        CDEBUGFLAGS = -g\n"
"        COPTFLAGS = -O2\n"
"        SYS_LIBRARIES = -lgen -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH) -R$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH) -R$(ALLX_LIBPATH)\n"
);
    write_local_libraries(makeFile, libs, AB_OS_SUNOS);

    abio_puts(makeFile, 
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}

static int
write_uxp_params(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_UXP);

    abio_printf(makeFile, 
    "\n"
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile, "\n");
     abio_puts(makeFile,
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = \n"
"        ANSI_DEFINES = \n"
"\n"
"        CDEBUGFLAGS = -O\n"
"        SYS_LIBRARIES = -lnsl -lsocket -lm -ldl -lgen -lresolv -lw\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n");

    write_local_libraries(makeFile, libs, AB_OS_UXP);

     abio_puts(makeFile,
"        CFLAGS = $(CDEBUGFLAGS) -D_POSIX_SOURCE=1 $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}

static int
write_osf1_stuff(File makeFile, AbmfLibs libs)
{
    STRING      osName = util_os_type_to_string(AB_OS_OSF1);

    abio_printf(makeFile,
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
        osName);

     abio_puts(makeFile, "\n");
     abio_puts(makeFile,
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = -DDEC\n"
"        ANSI_DEFINES =\n"
"\n"
"        CDEBUGFLAGS = -g\n"
"        COPTFLAGS = -O2\n"
"        SYS_LIBRARIES = -ldnet_stub -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n"
"        EXTRA_LOAD_FLAGS = -Wl,-rpath,$(CDE_LIBPATH):$(ALLX_LIBPATH)\n"
);

    write_local_libraries(makeFile, libs, AB_OS_OSF1);

    abio_puts(makeFile,
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS) $(EXTRA_LOAD_FLAGS)\n"
"\n"
);

    return 0;
}

static int
write_unixware_params(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_UNIXWARE);

    abio_printf(makeFile, 
    "\n"
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile, "\n");
     abio_puts(makeFile,
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = \n"
"        ANSI_DEFINES = \n"
"\n"
"        CDEBUGFLAGS = -O\n"
"        SYS_LIBRARIES = -lnsl -lsocket -lm -ldl -lgen -lresolv -lw\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n");

    write_local_libraries(makeFile, libs, AB_OS_UNIXWARE);

     abio_puts(makeFile,
"        CFLAGS = $(CDEBUGFLAGS) -D_POSIX_SOURCE=1 $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}

static int
write_lnx_params(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_LNX);

    abio_printf(makeFile,
    "\n"
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile,
"\n"
".KEEP_STATE:\n"
"\n"
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = \n"
"        ANSI_DEFINES = \n"
"\n"
"        CDEBUGFLAGS = -g\n"
"        COPTFLAGS = -O2\n"
"        SYS_LIBRARIES = -lgen -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n"
);
    write_local_libraries(makeFile, libs, AB_OS_LNX);

    abio_puts(makeFile,
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}

static int
write_fbsd_params(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_FBSD);

    abio_printf(makeFile,
    "\n"
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile,
"\n"
".KEEP_STATE:\n"
"\n"
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = \n"
"        ANSI_DEFINES = \n"
"\n"
"        CDEBUGFLAGS = -g\n"
"        COPTFLAGS = -O2\n"
"        SYS_LIBRARIES = -lgen -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n"
);
    write_local_libraries(makeFile, libs, AB_OS_FBSD);

    abio_puts(makeFile,
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}

static int
write_nbsd_params(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_NBSD);

    abio_printf(makeFile,
    "\n"
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile,
"\n"
".KEEP_STATE:\n"
"\n"
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = \n"
"        ANSI_DEFINES = \n"
"\n"
"        CDEBUGFLAGS = -g\n"
"        COPTFLAGS = -O2\n"
"        SYS_LIBRARIES = -lgen -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n"
);
    write_local_libraries(makeFile, libs, AB_OS_NBSD);

    abio_puts(makeFile,
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}

static int
write_obsd_params(File makeFile, AbmfLibs libs)
{
    STRING	osName = util_os_type_to_string(AB_OS_OBSD);

    abio_printf(makeFile,
    "\n"
"###########################################################################\n"
"# These are the %s-dependent configuration parameters that must be\n"
"# set in order for any application to build.\n"
"###########################################################################\n",
	osName);

     abio_puts(makeFile,
"\n"
".KEEP_STATE:\n"
"\n"
"        RM = rm -f\n"
"        INCLUDES = -I/usr/dt/include -I/X11/include\n"
"\n"
"        STD_DEFINES = \n"
"        ANSI_DEFINES = \n"
"\n"
"        CDEBUGFLAGS = -g\n"
"        COPTFLAGS = -O2\n"
"        SYS_LIBRARIES = -lgen -lm\n"
"        CDE_LIBPATH = /usr/dt/lib\n"
"        CDE_LDFLAGS = -L$(CDE_LIBPATH)\n"
"        ALLX_LIBPATH = /X11/lib\n"
"        ALLX_LDFLAGS = -L$(ALLX_LIBPATH)\n"
);
    write_local_libraries(makeFile, libs, AB_OS_OBSD);

    abio_puts(makeFile,
"\n"
"        CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES)"
            " $(ANSI_DEFINES)\n"
"        LDLIBS = $(SYS_LIBRARIES)\n"
"        LDOPTIONS = $(CDE_LDFLAGS) $(ALLX_LDFLAGS)\n"
"\n"
);

    return 0;
}

static int	
write_local_libraries(
			File		makeFile, 
			AbmfLibs	libs,
			AB_OS_TYPE	osType
)
{
    int		numLibs = libs->numLibs;
    int		i = 0;
    AbmfLibDesc	lib = LibNone;
    STRING	libString = NULL;

    abio_puts(makeFile, "        LOCAL_LIBRARIES =");
    for (i = 0; i < numLibs; ++i)
    {
	lib = libs->libs[i];
	libString = get_string_for_lib(lib, osType);
	if (libString != NULL)
	{
	    abio_printf(makeFile, " %s", libString);
	}
    }
    abio_puts(makeFile, nlstr);

    return 0;
}


static int
write_ab_params(
		GenCodeInfo	genCodeInfo,
		ABObj		project
)
{
    File	makeFile = genCodeInfo->code_file;
    int		numModules = trav_count_cond(project, AB_TRAV_MODULES, obj_is_defined_module);
    int		thisModuleNum = 0;
    AB_TRAVERSAL	trav;
    ABObj		module = NULL;

    abio_printf(makeFile, 
    "\n"
"###########################################################################\n"
"#  These parameters are generated by %s according to the structure\n"
"#  of the project as defined in the .bip and .bil files\n"
"###########################################################################\n",
	util_get_program_name());

    abio_puts(makeFile, "\n");
    abio_printf(makeFile, "PROGRAM = %s\n", abmfP_get_exe_file_name(project));

    abio_puts(makeFile, "\n");
    abio_puts(makeFile, "TARGETS.h = \\\n");
    abio_printf(makeFile, "\t%s \\\n", 
	abmfP_get_utils_header_file_name(project));
    for (thisModuleNum = 0, trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL;
	++thisModuleNum)
    {
	if (!obj_is_defined(module))
	{
	    continue;
	}

        abio_printf(makeFile, "\t%s", abmfP_get_ui_header_file_name(module));
	if (thisModuleNum < (numModules-1))
	{
	    abio_puts(makeFile, " \\");
	}
	abio_puts(makeFile, "\n");

    }
    trav_close(&trav);

    /*
     * TARGETS.c
     */
    abio_puts(makeFile, "\n");
    abio_puts(makeFile, "TARGETS.c = \\\n");
    abio_printf(makeFile, "\t%s \\\n", abmfP_get_utils_c_file_name(project));
    for (thisModuleNum = 0, trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL;
	++thisModuleNum)
    {
        if (!obj_is_defined(module))
        { 
            continue; 
        } 

        abio_printf(makeFile, "\t%s", abmfP_get_ui_c_file_name(module));
	if (thisModuleNum < (numModules-1))
	{
	    abio_puts(makeFile, " \\");
	}
	abio_puts(makeFile, "\n");

    }
    trav_close(&trav);

    /*
     * TARGETS.h.merged
     *
     * These are hand-edited, as well, so we cannot consider them
     * target .h files, which can be removed and regenerated on a whim.
     */
    abio_puts(makeFile, "\n");
    abio_puts(makeFile, "TARGETS.h.merged = \\\n");
    abio_printf(makeFile, "\t%s\n", 
	abmfP_get_project_header_file_name(project));


    /*
     * TARGETS.c.merged
     *
     * These are hand-edited, as well, so we cannot consider them
     * target .c files, which can be removed and regenerated on a whim.
     */
    abio_puts(makeFile, "\n");
    abio_puts(makeFile, "TARGETS.c.merged = \\\n");
    abio_printf(makeFile, "\t%s \\\n", abmfP_get_project_c_file_name(project));
    for (thisModuleNum = 0, trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL;
	++thisModuleNum)
    {
        if (!obj_is_defined(module))
        { 
            continue; 
        } 

        abio_printf(makeFile, "\t%s", abmfP_get_stubs_c_file_name(module));
	if (thisModuleNum < (numModules-1))
	{
	    abio_puts(makeFile, " \\");
	}
	abio_puts(makeFile, "\n");

    }
    trav_close(&trav);


    abio_puts(makeFile, "\n");
    abio_puts(makeFile, "SOURCES.h = $(TARGETS.h.merged) $(TARGETS.h)\n");

    abio_puts(makeFile, "\n");
    abio_puts(makeFile, "SOURCES.c = $(TARGETS.c.merged) $(TARGETS.c)\n");

    abio_puts(makeFile, "\n");
    abio_puts(makeFile, "OBJS = \\\n");
    abio_printf(makeFile, "\t%s \\\n",
	cvt_to_obj(abmfP_get_project_c_file_name(project)));
    abio_printf(makeFile, "\t%s \\\n",
	cvt_to_obj(abmfP_get_utils_c_file_name(project)));
    for (thisModuleNum = 0, trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL;
	++thisModuleNum)
    {
        if (!obj_is_defined(module))
        { 
            continue; 
        } 

        abio_printf(makeFile, "\t%s \\\n",
		cvt_to_obj(abmfP_get_ui_c_file_name(module)));
	abio_printf(makeFile, "\t%s", 
		cvt_to_obj(abmfP_get_stubs_c_file_name(module)));

	if (thisModuleNum < (numModules-1))
	{
	    abio_puts(makeFile, " \\");
	}
	abio_puts(makeFile, "\n");
    }
    trav_close(&trav);

    return 0;
}


static int
write_codegen_macro(File makeFile)
{
    abio_printf(makeFile, "        %s = %s\n", 
		CODEGEN_MACRO, util_get_program_name());

    return 0;
}


static BOOL
obj_is_module_func(ABObj obj)
{
    return obj_is_module(obj);
}

static int
write_targets(
	GenCodeInfo	genCodeInfo,
	ABObj		project,
	AB_OS_TYPE	osType
)
{
    File	makeFile = genCodeInfo->code_file;
    STRING	prog = util_get_program_name();
    STRING	projFile = obj_get_file(project);
    STRING	modFile = NULL;
    char	modName[MAXPATHLEN+1];
    ABObj	firstModule = NULL;
    STRING	firstModuleFileName = NULL;
    char	projFlag[1024];
    AB_TRAVERSAL	moduleTrav;
    ABObj		module = NULL;
    *projFlag = 0;
    *modName = 0;

    firstModule = obj_get_child_cond(project, 0, obj_is_module_func);
    if (firstModule != NULL)
    {
        firstModuleFileName = obj_get_file(firstModule);
    }
    if (obj_is_default(project))
    {
	strcpy(projFlag, "-nomerge -noproject");
    }
    else
    {
	sprintf(projFlag, "-changed -merge -p %s", projFile);
    }

    abio_puts(makeFile, 
    "\n"
    "######################################################################\n"
    "# Standard targets.                                                  #\n"
    "######################################################################\n"
    );

    abio_puts(makeFile,
"all:: $(PROGRAM)\n"
"\n"
"$(PROGRAM) : $(SOURCES.c) $(SOURCES.h) $(OBJS)\n"
"	$(RM) $@\n"
"	$(CC) -o $@ $(OBJS) $(LDOPTIONS) $(LOCAL_LIBRARIES) $(LDLIBS)\n"
"\n"
);

    if (! (obj_is_default(project) && (firstModuleFileName == NULL)) )
    {
	if (osType == AB_OS_SUNOS)
            abio_printf(makeFile, "%s + %s + %s + %s: %s\n",
	        abmfP_get_project_c_file_name(project), 
	        abmfP_get_project_header_file_name(project), 
	        abmfP_get_utils_c_file_name(project),
	        abmfP_get_utils_header_file_name(project),
	        obj_get_file(project));
	else
            abio_printf(makeFile, "%s %s %s %s: %s\n",
	        abmfP_get_project_c_file_name(project), 
	        abmfP_get_project_header_file_name(project), 
	        abmfP_get_utils_c_file_name(project),
	        abmfP_get_utils_header_file_name(project),
	        obj_get_file(project));
 
        abio_printf(makeFile, 
	    "\t$(%s) %s -main", CODEGEN_MACRO, projFlag);
        if (obj_is_default(project))
        {
	    abio_printf(makeFile, " %s", firstModuleFileName);
	}
        abio_printf(makeFile, "\n");

	abio_printf(makeFile, "%s: %s %s $(TARGETS.h) $(TARGETS.h.merged)\n",
	    cvt_to_obj(abmfP_get_project_c_file_name(project)),
	    abmfP_get_project_c_file_name(project),
	    obj_get_file(project));
    }

    /*
     * Make the project.c file dependent upon the module header files.
     * That way, if a module changes, main() and the x-module connections
     * will be recompiled.
     */
    abio_printf(makeFile, "%s: $(TARGETS.h)\n\n",
        abmfP_get_project_c_file_name(project));

    for (trav_open(&moduleTrav, project, AB_TRAV_MODULES);
	(module = trav_next(&moduleTrav)) != NULL; )
    {
	if (!obj_is_defined(module))
	{
	    continue;
	}

	modFile = obj_get_file(module);
	util_strncpy(modName, obj_get_name(module), MAXPATHLEN+1);

	if (osType == AB_OS_SUNOS)
	    abio_printf(makeFile, "\n%s + %s + %s: %s %s\n",
		abmfP_get_ui_c_file_name(module), 
		abmfP_get_ui_header_file_name(module),
		abmfP_get_stubs_c_file_name(module),
		projFile, 
		modFile);
	else
	    abio_printf(makeFile, "\n%s %s %s: %s %s\n",
		abmfP_get_ui_c_file_name(module), 
		abmfP_get_ui_header_file_name(module),
		abmfP_get_stubs_c_file_name(module),
		projFile, 
		modFile);

	abio_printf(makeFile, "\t$(%s) %s %s\n", 
			CODEGEN_MACRO, projFlag, modName);

	/*
	 * .o dependency on .c and .bil file 
	 */
	abio_printf(makeFile, "%s: %s %s %s\n",
	    cvt_to_obj(abmfP_get_ui_c_file_name(module)),
	    abmfP_get_ui_c_file_name(module),
	    abmfP_get_ui_header_file_name(module),
	    modFile);
	abio_printf(makeFile, "%s: %s %s %s\n",
	    cvt_to_obj(abmfP_get_stubs_c_file_name(module)),
	    abmfP_get_stubs_c_file_name(module),
	    abmfP_get_ui_header_file_name(module),
	    modFile);
    }
    trav_close(&moduleTrav);

    abio_puts(makeFile,
"\n"
"CLEAN_FILES =	core .dtcodegen.log *.BAK *.delta \\\n"
"		$(PROGRAM) $(OBJS) $(TARGETS.c) $(TARGETS.h)\n" 
"clean:\n"
"	$(RM)  $(CLEAN_FILES)\n"
"\n"
"scour:\n"
"	$(RM) $(CLEAN_FILES) $(TARGETS.h.merged) $(TARGETS.c.merged) \\\n"
"	      Makefile Makefile.aix Makefile.hpux Makefile.sunos \\\n"
"	      Makefile.osf1 Makefile.uxp Makefile.unixware Makefile.linux \\\n"
"	      Makefile.freebsd Makefile.netbsd Makefile.openbsd\n"
);

    return 0;
}


/*
 * Converts a .c file name to a .o file name
 */
static STRING
cvt_to_obj(STRING fileName)
{
    static char	buf[MAXPATHLEN] = "";
    strcpy(buf, fileName);
    buf[strlen(buf)-1] = 'o';
    return buf;
}


