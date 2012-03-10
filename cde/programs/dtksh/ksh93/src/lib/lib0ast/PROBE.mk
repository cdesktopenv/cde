/* $XConsortium: PROBE.mk /main/2 1996/10/30 14:44:50 drk $ */
/*
 * @(#)probe (AT&T Bell Laboratories) 11/11/91
 *
 * <lang> <tool> :PROBE: *.probe *.sh *
 *
 * common probe script installation
 * generates probe.sh and probe in .
 */

":PROBE:" : .MAKE .OPERATOR
	probe.sh : $(LIBDIR)/probe/$(<:O=1)/probe $(>:N=*.(probe|sh))
		cat $(*) > $(<)
	$(LIBDIR)/probe/$(<:O=1)/$(<:O=2) :INSTALLDIR: probe $(>:N!=*.(probe|sh))
