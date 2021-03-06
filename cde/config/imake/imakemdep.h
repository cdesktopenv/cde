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
/* $TOG: imakemdep.h /main/102 1998/02/06 11:02:26 kaleb $ */
/*

Copyright (c) 1993, 1994, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/


/* 
 * This file contains machine-dependent constants for the imake utility.
 * When porting imake, read each of the steps below and add in any necessary
 * definitions.  In general you should *not* edit ccimake.c or imake.c!
 */

#ifdef CCIMAKE
/*
 * Step 1:  imake_ccflags
 *     Define any special flags that will be needed to get imake.c to compile.
 *     These will be passed to the compile along with the contents of the
 *     make variable BOOTSTRAPCFLAGS.
 */
#ifdef hpux
#ifdef hp9000s800
#define imake_ccflags "-DSYSV"
#else
#define imake_ccflags "-Wc,-Nd4000,-Ns3000 -DSYSV"
#endif
#endif

#ifdef stellar
#define imake_ccflags "-DSYSV"
#endif

#if defined(Oki) || defined(NCR)
#define imake_ccflags "-Xa -DSVR4"
#endif

#ifdef sony
#if defined(SYSTYPE_SYSV) || defined(_SYSTYPE_SYSV)
#define imake_ccflags "-DSVR4"
#else
#include <sys/param.h>
#if NEWSOS < 41
#define imake_ccflags "-Dbsd43 -DNOSTDHDRS"
#else
#if NEWSOS < 42
#define imake_ccflags "-Dbsd43"
#endif
#endif
#endif
#endif

#ifdef _CRAY
#define imake_ccflags "-DSYSV -DUSG"
#endif

#if defined(_IBMR2) || defined(aix)
#define imake_ccflags "-Daix -DSYSV"
#endif

#ifdef Mips
#  if defined(SYSTYPE_BSD) || defined(BSD) || defined(BSD43)
#    define imake_ccflags "-DBSD43"
#  else
#    define imake_ccflags "-DSYSV"
#  endif
#endif

#ifdef is68k
#define imake_ccflags "-Dluna -Duniosb"
#endif

#ifdef SYSV386
# ifdef SVR4
#  define imake_ccflags "-Xa -DSVR4"
# else
#  define imake_ccflags "-DSYSV"
# endif
#endif

#ifdef SVR4
# ifdef i386
#  define imake_ccflags "-Xa -DSVR4"
# endif
#endif

#ifdef SYSV
# ifdef i386
#  define imake_ccflags "-DSYSV"
# endif
#endif

#ifdef __convex__
#define imake_ccflags "-fn -tm c1"
#endif

#ifdef __sxg__
#define imake_ccflags "-DSYSV -DUSG -DNOSTDHDRS"
#endif

#ifdef sequent
#define imake_ccflags "-DX_NOT_STDC_ENV -DX_NOT_POSIX"
#endif

#ifdef _SEQUENT_
#define imake_ccflags "-DSYSV -DUSG"
#endif

#if defined(SX) || defined(PC_UX)
#define imake_ccflags "-DSYSV"
#endif

#ifdef nec_ews_svr2
#define imake_ccflags "-DUSG"
#endif

#if defined(nec_ews_svr4) || defined(_nec_ews_svr4) || defined(_nec_up) || defined(_nec_ft)
#define imake_ccflags "-DSVR4"
#endif

#ifdef  MACH
#define imake_ccflags "-DNOSTDHDRS"
#endif

#ifdef  __FreeBSD__
#include <sys/param.h>
#if defined(__FreeBSD_version)
#if __FreeBSD_version < 500000
#define imake_ccflags "-DCPP_IN_LIBEXEC"
#endif
#else
#define imake_ccflags "-DCPP_IN_LIBEXEC"
#endif
#endif

/* this is for OS/2 under EMX. This won't work with DOS */
#if defined(__EMX__)
#define imake_ccflags "-DBSD43"
#endif

#else /* not CCIMAKE */
#ifndef MAKEDEPEND
/*
 * Step 2:  dup2
 *     If your OS doesn't have a dup2() system call to duplicate one file
 *     descriptor onto another, define such a mechanism here (if you don't
 *     already fall under the existing category(ies).
 */
#if defined(SYSV) && !defined(_CRAY) && !defined(Mips) && !defined(_SEQUENT_)
#define	dup2(fd1,fd2)	((fd1 == fd2) ? fd1 : (close(fd2), \
					       fcntl(fd1, F_DUPFD, fd2)))
#endif


/*
 * Step 3:  FIXUP_CPP_WHITESPACE
 *     If your cpp collapses tabs macro expansions into a single space and
 *     replaces escaped newlines with a space, define this symbol.  This will
 *     cause imake to attempt to patch up the generated Makefile by looking
 *     for lines that have colons in them (this is why the rules file escapes
 *     all colons).  One way to tell if you need this is to see whether or not
 *     your Makefiles have no tabs in them and lots of @@ strings.
 */
#if defined(sun) || defined(SYSV) || defined(SVR4) || defined(hcx) || defined(__llvm__)
#define FIXUP_CPP_WHITESPACE
#endif

/*
 * Step 4:  USE_CC_E, DEFAULT_CC, DEFAULT_CPP
 *     If you want to use cc -E instead of cpp, define USE_CC_E.
 *     If use cc -E but want a different compiler, define DEFAULT_CC.
 *     If the cpp you need is not in /lib/cpp, define DEFAULT_CPP.
 */
#ifdef hpux
#define USE_CC_E
#endif
#if defined(_IBMR2) && !defined(DEFAULT_CPP)
#define DEFAULT_CPP "/usr/ccs/lib/cpp"
#endif
#if defined(sun) && (defined(SVR4) || defined(__svr4__) || defined(__SVR4) || defined(__sol__))
#define DEFAULT_CPP "/usr/ccs/lib/cpp"
#endif
#if defined(__NetBSD__) || (defined(__FreeBSD__) && !defined(CPP_IN_LIBEXEC))
#define DEFAULT_CPP "/usr/bin/cpp"
#endif
#ifdef __sxg__
#define DEFAULT_CPP "/usr/lib/cpp"
#endif
#ifdef _CRAY
#define DEFAULT_CPP "/lib/pcpp"
#endif
#if defined(__386BSD__) || defined(__OpenBSD__) \
	|| (defined(__FreeBSD__) && defined(CPP_IN_LIBEXEC))
#define DEFAULT_CPP "/usr/libexec/cpp"
#endif
#if defined(__FreeBSD__) && (__FreeBSD__ >= 10) && !defined(__llvm__)
#undef DEFAULT_CPP
#endif
#if defined(__sgi) && defined(__ANSI_CPP__)
#define USE_CC_E
#endif
#ifdef  MACH
#define USE_CC_E
#endif
#ifdef __minix_vmd
#define DEFAULT_CPP "/usr/lib/cpp"
#endif
#if defined(__EMX__)
/* expects cpp in PATH */
#define DEFAULT_CPP "cpp"
#endif

/*
 * Step 5:  cpp_argv
 *     The following table contains the flags that should be passed
 *     whenever a Makefile is being generated.  If your preprocessor 
 *     doesn't predefine any unique symbols, choose one and add it to the
 *     end of this table.  Then, do the following:
 * 
 *         a.  Use this symbol in Imake.tmpl when setting MacroFile.
 *         b.  Put this symbol in the definition of BootstrapCFlags in your
 *             <platform>.cf file.
 *         c.  When doing a make World, always add "BOOTSTRAPCFLAGS=-Dsymbol" 
 *             to the end of the command line.
 * 
 *     Note that you may define more than one symbol (useful for platforms 
 *     that support multiple operating systems).
 */

#define	ARGUMENTS 50	/* number of arguments in various arrays */
char *cpp_argv[ARGUMENTS] = {
	"cc",		/* replaced by the actual program to exec */
	"-I.",		/* add current directory to include path */
#ifdef unix
	"-Uunix",	/* remove unix symbol so that filename unix.c okay */
#endif
#if defined(__386BSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(MACH) || defined(ISC) || defined(__linux__) || defined(__hpux__) || defined(__vxworks)
# ifdef __i386__
	"-D__i386__",
# endif
# ifdef __i486__
        "-D__i486__",
# endif
# ifdef __i586__
        "-D__i586__",
# endif
# ifdef __i686__
        "-D__i686__",
# endif
# ifdef __k6__
        "-D__k6__",
# endif
# ifdef __ia64__
        "-D__ia64__",
# endif
# ifdef __amd64__
        "-D__amd64__",
# endif
# ifdef __x86_64__
        "-D__amd64__",
# endif
# ifdef __ppc__
        {"-D__powerpc__", "1"},
# endif
# ifdef __ppc64__
        {"-D__powerpc64__", "1"},
# endif
# ifdef __powerpc__
        {"-D__powerpc__", "1"},
# endif
# ifdef __powerpc64__
        {"-D__powerpc64__", "1"},
# endif

# if defined(__GNUC__) && !defined(__llvm__)
	"-traditional",
# endif
# ifdef __llvm__
	"-fms-extensions",
	"-Wno-invalid-token-paste",
	"-Wno-invalid-pp-token",
# endif
#endif

#ifdef M4330
	"-DM4330",	/* Tektronix */
#endif
#ifdef M4310
	"-DM4310",	/* Tektronix */
#endif
#ifdef sony
	"-Dsony",	/* Sony */
#if !defined(SYSTYPE_SYSV) && !defined(_SYSTYPE_SYSV) && NEWSOS < 42
	"-Dbsd43",
#endif
#endif
#ifdef _IBMR2
	"-D_IBMR2",	/* IBM RS-6000 (we ensured that aix is defined above */
#ifndef aix
#define aix		/* allow BOOTSTRAPCFLAGS="-D_IBMR2" */
#endif
#endif /* _IBMR2 */
#ifdef aix
	"-Daix",	/* AIX instead of AOS */
#ifndef ibm
#define ibm		/* allow BOOTSTRAPCFLAGS="-Daix" */
#endif
#endif /* aix */
#ifdef ibm
	"-Dibm",	/* IBM PS/2 and RT under both AOS and AIX */
#endif
#ifdef luna
	"-Dluna",	/* OMRON luna 68K and 88K */
#ifdef luna1
	"-Dluna1",
#endif
#ifdef luna88k		/* need not on UniOS-Mach Vers. 1.13 */
	"-traditional", /* for some older version            */
#endif			/* instead of "-DXCOMM=\\#"          */
#ifdef uniosb
	"-Duniosb",
#endif
#ifdef uniosu
	"-Duniosu",
#endif
#endif /* luna */
#ifdef _CRAY		/* Cray */
	"-Ucray",
#endif
#ifdef Mips
	"-DMips",	/* Define and use Mips for Mips Co. OS/mach. */
# if defined(SYSTYPE_BSD) || defined(BSD) || defined(BSD43)
	"-DBSD43",	/* Mips RISCOS supports two environments */
# else
	"-DSYSV",	/* System V environment is the default */
# endif
#endif /* Mips */
#ifdef MOTOROLA
	"-DMOTOROLA",    /* Motorola Delta Systems */
# ifdef SYSV
	"-DSYSV", 
# endif
# ifdef SVR4
	"-DSVR4",
# endif
#endif /* MOTOROLA */
#ifdef i386
	"-Di386",
# ifdef SVR4
	"-DSVR4",
# endif
# ifdef SYSV
	"-DSYSV",
#  ifdef ISC
	"-DISC",
#   ifdef ISC40
	"-DISC40",       /* ISC 4.0 */
#   else
#    ifdef ISC202
	"-DISC202",      /* ISC 2.0.2 */
#    else
#     ifdef ISC30
	"-DISC30",       /* ISC 3.0 */
#     else
	"-DISC22",       /* ISC 2.2.1 */
#     endif
#    endif
#   endif
#  endif
#  ifdef ESIX
	"-DESIX",
#  endif
#  ifdef ATT
	"-DATT",
#  endif
#  ifdef DELL
	"-DDELL",
#  endif
# endif
#endif
#ifdef SYSV386		/* System V/386 folks, obsolete */
	"-Di386",
# ifdef SVR4
	"-DSVR4",
# endif
# ifdef SYSV
	"-DSYSV",
#  ifdef ISC
	"-DISC",
#   ifdef ISC40
	"-DISC40",       /* ISC 4.0 */
#   else
#    ifdef ISC202
	"-DISC202",      /* ISC 2.0.2 */
#    else
#     ifdef ISC30
	"-DISC30",       /* ISC 3.0 */
#     else
	"-DISC22",       /* ISC 2.2.1 */
#     endif
#    endif
#   endif
#  endif
#  ifdef ESIX
	"-DESIX",
#  endif
#  ifdef ATT
	"-DATT",
#  endif
#  ifdef DELL
	"-DDELL",
#  endif
# endif
#endif
#ifdef Oki
	"-DOki",
#endif
#ifdef sun
#if defined(SVR4) || defined(__svr4__) || defined(__SVR4) || defined(__sol__)
	"-DSVR4",
#endif
#endif
#ifdef NCR
	"-DNCR",	/* NCR */
#endif
#ifdef linux
        "-traditional",
        "-D__linux__",
#endif
#ifdef __minix_vmd
	"-Dminix",
#endif

#if defined(__EMX__)
	"-traditional",
	"-Demxos2",
#endif

};


/*
 * Step 6: DEFAULT_OS_MAJOR_REV, DEFAULT_OS_MINOR_REV, DEFAULT_OS_TEENY_REV,
 *	and DEFAULT_OS_NAME.
 *	If your systems provides a way to generate the default major,
 *	minor, teeny, or system names at runtime add commands below.
 *	The syntax of the _REV strings is 'f fmt' where 'f' is an argument
 *	you would give to uname, and "fmt" is a scanf() format string.
 *	Supported uname arguments are "snrvm", and if you specify multiple
 *	arguments they will be separated by spaces.  No more than 5 arguments
 *	may be given.  Unlike uname() order of arguments matters.
 */
#if defined(aix)
/* uname -v returns "x" (e.g. "4"), and uname -r returns "y" (e.g. "1") */
# define DEFAULT_OS_MAJOR_REV	"v %[0-9]"
# define DEFAULT_OS_MINOR_REV	"r %[0-9]"
/* No information available to generate default OSTeenyVersion value. */
# define DEFAULT_OS_NAME	"srvm %[^\n]"
#elif defined(sun) || defined(__linux__)
/* uname -r returns "x.y[.z]", e.g. "5.4" or "4.1.3" */
# define DEFAULT_OS_MAJOR_REV	"r %[0-9]"
# define DEFAULT_OS_MINOR_REV	"r %*d.%[0-9]"
# define DEFAULT_OS_TEENY_REV	"r %*d.%*d.%[0-9]"
# define DEFAULT_OS_NAME	"srvm %[^\n]"
#elif defined(hpux)
/* uname -r returns "W.x.yz", e.g. "B.10.01" */
# define DEFAULT_OS_MAJOR_REV	"r %*[^.].%[0-9]"
# define DEFAULT_OS_MINOR_REV	"r %*[^.].%*d.%1s"
# define DEFAULT_OS_TEENY_REV	"r %*[^.].%*d.%*c%[0-9]"
# define DEFAULT_OS_NAME	"srvm %[^\n]"
#elif defined(__FreeBSD__)
/* uname -r returns "x.y[.z]-mumble", e.g. "9.0-RELEASE" or "11.0-CURRENT" */
# define DEFAULT_OS_MAJOR_REV   "r %[0-9]"
# define DEFAULT_OS_MINOR_REV   "r %*d.%[0-9]"
# define DEFAULT_OS_TEENY_REV   "v %*s %*s %*s r%[0-9]"
# define DEFAULT_OS_NAME        "srm %[^\n]"
#elif defined(__NetBSD__)
/* 386BSD, and BSD/OS too? */
/* uname -r returns "x.y[.z]-mumble", e.g. "2.1.5-RELEASE" or "2.2-0801SNAP" */
# define DEFAULT_OS_MAJOR_REV   "r %[0-9]"
# define DEFAULT_OS_MINOR_REV   "r %*d.%[0-9]"
# define DEFAULT_OS_TEENY_REV   "r %*d.%*d.%[0-9]"
# define DEFAULT_OS_NAME        "srm %[^\n]"
#elif defined(__OpenBSD__)
# define DEFAULT_OS_MAJOR_REV   "r %[0-9]"
# define DEFAULT_OS_MINOR_REV   "r %*d.%[0-9]"
# define DEFAULT_OS_NAME        "srm %[^\n]"
#endif

#else /* else MAKEDEPEND */
/*
 * Step 7:  predefs
 *     If your compiler and/or preprocessor define any specific symbols, add
 *     them to the the following table.  The definition of struct symtab is
 *     in util/makedepend/def.h.
 */
#undef DEF_EVALUATE
#undef DEF_STRINGIFY
#define DEF_EVALUATE(__x) #__x
#define DEF_STRINGIFY(_x) DEF_EVALUATE(_x)

struct symtab	predefs[] = {
#ifdef ibm032
	{"ibm032", "1"},
#endif
#ifdef ibm
	{"ibm", "1"},
#endif
#ifdef aix
	{"aix", "1"},
#endif
#ifdef sun
	{"sun", "1"},
#endif
#ifdef sun2
	{"sun2", "1"},
#endif
#ifdef sun3
	{"sun3", "1"},
#endif
#ifdef sun4
	{"sun4", "1"},
#endif
#ifdef sparc
	{"sparc", "1"},
#endif
#ifdef __sparc__
	{"__sparc__", "1"},
#endif
#ifdef hpux
	{"hpux", "1"},
#endif
#ifdef __hpux
	{"__hpux", "1"},
#endif
#ifdef __hp9000s800
	{"__hp9000s800", "1"},
#endif
#ifdef __hp9000s700
	{"__hp9000s700", "1"},
#endif
#ifdef vax
	{"vax", "1"},
#endif
#ifdef VMS
	{"VMS", "1"},
#endif
#ifdef cray
	{"cray", "1"},
#endif
#ifdef CRAY
	{"CRAY", "1"},
#endif
#ifdef _CRAY
	{"_CRAY", "1"},
#endif
#ifdef att
	{"att", "1"},
#endif
#ifdef mips
	{"mips", "1"},
#endif
#ifdef __mips__
	{"__mips__", "1"},
#endif
#ifdef stellar
	{"stellar", "1"},
#endif
#ifdef mc68000
	{"mc68000", "1"},
#endif
#ifdef mc68020
	{"mc68020", "1"},
#endif
#if defined(__GNUC__) && !defined(__linux__)
	{"__GNUC__", DEF_STRINGIFY(__GNUC__)},
#endif
#ifdef __GNUC_MINOR__
	{"__GNUC_MINOR__", DEF_STRINGIFY(__GNUC_MINOR__)},
#endif
#if __STDC__
	{"__STDC__", "1"},
#endif
#ifdef __HIGHC__
	{"__HIGHC__", "1"},
#endif
#ifdef CMU
	{"CMU", "1"},
#endif
#ifdef luna
	{"luna", "1"},
#ifdef luna1
	{"luna1", "1"},
#endif
#ifdef luna2
	{"luna2", "1"},
#endif
#ifdef luna88k
	{"luna88k", "1"},
#endif
#ifdef uniosb
	{"uniosb", "1"},
#endif
#ifdef uniosu
	{"uniosu", "1"},
#endif
#endif
#ifdef ieeep754
	{"ieeep754", "1"},
#endif
#ifdef is68k
	{"is68k", "1"},
#endif
#ifdef m68k
        {"m68k", "1"},
#endif
#ifdef m88k
        {"m88k", "1"},
#endif
#ifdef __m88k__
	{"__m88k__", "1"},
#endif
#ifdef bsd43
	{"bsd43", "1"},
#endif
#ifdef hcx
	{"hcx", "1"},
#endif
#ifdef sony
	{"sony", "1"},
#ifdef SYSTYPE_SYSV
	{"SYSTYPE_SYSV", "1"},
#endif
#ifdef _SYSTYPE_SYSV
	{"_SYSTYPE_SYSV", "1"},
#endif
#endif
#ifdef __alpha
	{"__alpha", "1"},
#endif
#ifdef __unix__
	{"__unix__", "1"},
#endif
#ifdef __sxg__
	{"__sxg__", "1"},
#endif
#ifdef _SEQUENT_
	{"_SEQUENT_", "1"},
	{"__STDC__", "1"},
#endif
#ifdef nec_ews_svr2
	{"nec_ews_svr2", "1"},
#endif
#ifdef nec_ews_svr4
	{"nec_ews_svr4", "1"},
#endif
#ifdef _nec_ews_svr4
	{"_nec_ews_svr4", "1"},
#endif
#ifdef _nec_up
	{"_nec_up", "1"},
#endif
#ifdef SX
	{"SX", "1"},
#endif
#ifdef nec
	{"nec", "1"},
#endif
#ifdef _nec_ft
	{"_nec_ft", "1"},
#endif
#ifdef PC_UX
	{"PC_UX", "1"},
#endif
#ifdef sgi
	{"sgi", "1"},
#endif
#ifdef __sgi
	{"__sgi", "1"},
#endif
#ifdef __FreeBSD__
	{"__FreeBSD__", "1"},
#endif
#ifdef __OpenBSD__
	{"__OpenBSD__", "1"},
#endif
#ifdef __NetBSD__
	{"__NetBSD__", "1"},
#endif
#ifdef __EMX__
	{"__EMX__", "1"},
#endif
# ifdef ia64
        {"ia64", "1"},
# endif
# ifdef __ia64__
        {"__ia64__", "1"},
# endif
# if defined (amd64) || defined (x86_64)
        {"amd64", "1"},
        {"x86_64", "1"},
# endif
# if defined (__amd64__) || defined (__x86_64__)
        {"__amd64__", "1"},
        {"__x86_64__", "1"},
# endif
# ifdef __i386
        {"__i386", "1"},
# endif
# ifdef __i386__
        {"__i386__", "1"},
# endif
# ifdef __i486__
        {"__i486__", "1"},
# endif
# ifdef __i586__
        {"__i586__", "1"},
# endif
# ifdef __i686__
        {"__i686__", "1"},
# endif
# ifdef __k6__
        {"__k6__", "1"},
# endif
# ifdef i386
        {"i386", "1"},
# endif
# ifdef i486
        {"i486", "1"},
# endif
# ifdef i586
        {"i586", "1"},
# endif
# ifdef i686
        { "i686", "1"},
# endif
# ifdef k6
        {"k6", "1"},
# endif
# ifdef sparc
        {"sparc", "1"},
# endif
# ifdef __sparc__
        {"__sparc__", "1"},
# endif
#if defined(__ppc__)
        {"__ppc__", "1"},
#endif
#if defined(__ppc64__)
        {"__ppc64__", "1"},
#endif
# ifdef __powerpc__
        {"__powerpc__", "1"},
# endif
# ifdef __powerpc64__
        {"__powerpc64__", "1"},
# endif
#if defined(__BIG_ENDIAN__)
      {"__BIG_ENDIAN__", "1"},
#endif
#if defined(__LITTLE_ENDIAN__)
      {"__LITTLE_ENDIAN__", "1"},
#endif
#if defined(__vxworks)
      {"vxworks", "1"},
#endif
	/* add any additional symbols before this line */
	{NULL, NULL}
};

#endif /* MAKEDEPEND */
#endif /* CCIMAKE */
