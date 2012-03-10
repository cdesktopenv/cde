# Shell script for linking C++ ToolTalk routines.  We have
# %%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
# %%  (c) Copyright 1993, 1994 International Business Machines Corp.	
# %%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
# %%  (c) Copyright 1993, 1994 Novell, Inc. 				
# %%  $XConsortium: tt_ld.sh /main/4 1995/10/20 16:23:10 rswiston $ 			 				
# to do this "by hand" to get all the compiler libraries included
# just once, with libC static and libc and libm dynamic.

# Calling sequence
# tt_ld $(CCLIBDIR) $@ $(LDFLAGS) -- $(OBJECT_FILES) $(TT_LIBS) $(TT_AUX_LIBS)
#

CCLIBDIR=$1; shift
BINARY=$1; shift
LDFLAGS=""
while	test "$1" != "--"
do	LDFLAGS="$LDFLAGS $1"
	shift
done
shift

if	test "$TARGET_SYS" = "SVR4";
then
	echo $0 is not used for SVR4 based systems.  Check your Makefiles and
	echo environment variables.
	exit 1
else
	if test "$PROF" = "gprof";
	then
		if ld -dc -dp -e start -X $LDFLAGS -Bstatic -o $BINARY \
				$CCLIBDIR/gcrt0.o $CCLIBDIR/cg87/_crt1.o \
				-L$CCLIBDIR/cg87 -L$CCLIBDIR \
				$* \
				-Bstatic -lX11 -lC  -lc_p -lm >/tmp/ld.$$.4.err 2>&1
		then	$CCLIBDIR/c++filt </tmp/ld.$$.4.err
			rm -f /tmp/ld.$$.4.err
			$CCLIBDIR/patch $BINARY
			exit 0
		else	$CCLIBDIR/c++filt </tmp/ld.$$.4.err
			rm -f /tmp/ld.$$.4.err
			exit 1
		fi
	else
		if ld -dc -dp -e start -X $LDFLAGS -o $BINARY \
				$CCLIBDIR/crt0.o $CCLIBDIR/cg87/_crt1.o \
				-L$CCLIBDIR/cg87 -L$CCLIBDIR \
				$* \
				-Bstatic -lC -Bdynamic -lc -lm >/tmp/ld.$$.4.err 2>&1
		then	$CCLIBDIR/c++filt </tmp/ld.$$.4.err
			rm -f /tmp/ld.$$.4.err
			$CCLIBDIR/patch $BINARY
			exit 0
		else	$CCLIBDIR/c++filt </tmp/ld.$$.4.err
			rm -f /tmp/ld.$$.4.err
			exit 1
		fi
	fi
fi

