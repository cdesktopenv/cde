/* $XConsortium: Makefile /main/3 1996/05/10 17:18:25 drk $ */
/*
 * ksh makefile
 * This makefile should work with nmake 2.3 and higher 
 */

:PACKAGE:	ast

.SOURCE : edit sh bltins data tests
.SOURCE.h : include 

clobber = !(ksh)	/* move ksh to ksh.old on clobber */

SH = ksh		/* name of the shell program */
VERSION = 1.1

/* documentation files */
DOCFILES = sh.memo grep.mm builtins.mm README RELEASE COMPATIBILITY PROMO \
		OBSOLETE RELEASEa

KSHELL == 1		/* this is ksh */

/* set these variables to 1 to enable, null to disable */
DEBUG ==		/* debugging purposes only */
MTRACE ==   		/* on for malloc tracing */
SHOPT_ESH == 1		/* emacs/gmacs edit mode */
SHOPT_VSH == 1		/* vi edit mode */
SHOPT_SPAWN == 		/* use spawnve for fork/exec */
SHOPT_SUID_EXEC == 1	/* allow (safe) suid/sgid shell scripts */

/* use the following libraries only if they exist */
LIBS_opt := +lm +ljobs +li
if ! "$(-mam:N=static*)" && ! "$(HOSTTYPE:N=sgi.mips*)"
	LIBS_opt += +lsocket +lnsl
end
LIBS_opt += -ldl -ldld +lintl
$(LIBS_opt)	: .DONTCARE

LDLIBRARIES = $(LIBS_opt)

FILES_opt =		/* optional file list enabled by vars above */
FILES_opt += $(SHOPT_ESH:T=V?emacs.c)
FILES_opt += $(SHOPT_VSH:T=V?vi.c)
/*
LDFLAGS += -u  _XtAppInitialize -L/usr/add-on/X11/lib
*/

.ALL : $(SH) shcomp ksh.msg $(SHOPT_SUID_EXEC:T=V?suid_exec.c)

$(SH) :: sh.1 main.c -lshell +lcmd /* -lXaw -lXmu -lXt -lXtext -lX11 */ +last

DATAFILES = limits.c msg.c strdata.c testops.c keywords.c options.c \
                 signals.c aliases.c builtins.c variables.c lexstates.c

shell $(VERSION) :LIBRARY: nval.3 alarm.c cd_pwd.c cflow.c deparse.c \
	getopts.c hist.c misc.c print.c read.c sleep.c trap.c test.c \
	typeset.c ulimit.c umask.c whence.c \
	arith.c args.c array.c completion.c dlopen.c defs.c edit.c expand.c \
	fault.c fcin.c history.c init.c io.c jobs.c lex.c macro.c name.c \
	parse.c path.c string.c streval.c subshell.c tdump.c timers.c \
	trestore.c userinit.c waitevent.c xec.c $(DATAFILES) $(FILES_opt)

:READONLY: $(DATAFILES)

$(INCLUDEDIR) :INSTALLPROTO: nval.h shell.h

suid_exec ::	suid_exec.c

shcomp	::	shcomp.c -lshell -lcmd -last

ksh.msg :	$(DATAFILES)	/* build error message dictionary */
	grep '[ 	]e_[a-z].*".*[^\][a-z].*"' $(*) |  grep -v /bin/ |  grep -v /usr/ | grep -v /dev/ |  grep -v /etc/ | grep -v A__z | sed 's/.*=[ 	]*\(".*"\);.*/\1/' | sort | uniq > $(<)
	grep '[ 	]sh_opt.*".*[^\][a-z].*"' $(*) |  sed 's/.*=[ 	]*\(".*"\);.*/\1/'  >> $(<)
	grep '[ 	]is_[a-z].*".*[^\][a-z].*"' $(*) |  sed 's/.*=[ 	]*\(".*"\);.*/\1/'  >> $(<)
	grep '[         ]e_notset' $(*) | sed 's/.*"....\(.*"\).*/"\1/' >> $(<)
	grep '[         ]e_subst' $(*) | sed 's/.*"....\(.*"\).*/"\1/' >> $(<)
	grep '[         ]e_file' $(*) | sed 's/.*"....\(.*"\).*/"\1/' >> $(<)
	grep ',$' $(*:N=*signals.c)  | sed 's/.*\(".*"\),/\1/' | sort | uniq >> $(<)

:: syscall.s mamexec mamstate.c apollo.c deparse.c ksh.mk OPTIONS $(DOCFILES)

$(TESTS):: alias.sh arith.sh arrays.sh attributes.sh basic.sh bracket.sh \
	builtins.sh case.sh comvar.sh coprocess.sh exit.sh functions.sh \
	grep.sh heredoc.sh io.sh options.sh quoting.sh return.sh select.sh \
	substring.sh tilde.sh variables.sh nameref.sh shtests
