/* $XConsortium: Makefile /main/3 1996/05/10 17:20:23 drk $ */
/*
 * command library -- commands small enough to be builtins
 */

:PACKAGE: ast

VERSION = 1.2
CCFLAGS = -O $(CC.PIC)

HOSTTYPE == "`hostinfo`"

cmd $(VERSION) :LIBRARY: RELEASE cmdinit.c cmdrecurse.c \
	cmd.h cmdlib.h wc.h \
	basename.c cat.c chgrp.c chmod.c chown.c cmp.c comm.c \
	cut.c dirname.c fold.c getconf.c head.c id.c join.c \
	logname.c mkdir.c mkfifo.c paste.c pathchk.c rev.c \
	rmdir.c tail.c tee.c tty.c uname.c uniq.c wc.c \
	revlib.c wclib.c \
	-lfsg

$(INCLUDEDIR) :INSTALLDIR: cmd.h
