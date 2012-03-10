/* $XConsortium: Makefile /main/2 1996/05/10 17:19:23 drk $ */
/*
 * ast package initialization
 */

:PACKAGE: ast

hostinfo :: hostinfo.sh

/*
 * probe initialization
 */

for T C.probe
	$(LIBDIR)/probe/$(T:B)/probe :INSTALL: $(T)
end

$(LIBDIR)/make :INSTALLDIR: PROBE.mk

:: RELEASE hosttype.tst

/*
 * proto initialization
 */

$(INCLUDEDIR) :INSTALLDIR: prototyped.h

$(BINDIR)/proto : .DONTCARE

prototyped.h : $(BINDIR)/proto
	$(PROTO) -f /dev/null > h.$(tmp).h
	if	$(CMP) -s h.$(tmp).h $(<)
	then	$(RM) -f h.$(tmp).h
	else	$(MV) h.$(tmp).h $(<)
	fi
