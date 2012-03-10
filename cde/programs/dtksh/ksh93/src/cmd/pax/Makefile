/* $XConsortium: Makefile /main/3 1996/05/10 17:18:44 drk $ */
:PACKAGE: ast

DEBUG == 1

pax :: RELEASE HISTORY pax.1 pax.1.posix \
	pax.h options.h \
	bio.c convert.c copy.c pax.c \
	delta.c file.c format.c misc.c \
	-lodelta -lvdelta

nocom :: nocom.c

$(LIBDIR)/pax :INSTALLDIR: cia.fmt ppar.fmt

:INSTALLDIR: cpio tar

bax :: bax.1 bax.sh

cpio :: cpio.sh

tar :: tar.sh
