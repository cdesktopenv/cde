# $XConsortium: Makefile /main/2 1996/05/13 11:32:08 drk $
# ###############################################
#  Makefile for building the Help4Help volume
#
#  (c) Copyright 1993, 1994 Hewlett-Packard Company	
#  (c) Copyright 1993, 1994 International Business Machines Corp.
#  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
#  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
#      Novell, Inc.
#
# ###############################################

HELPSRC = Help4Help.htg
HELPVOL = Help4Help.sdl
HELPCHARSET = iso8859.1

all: $(HELPVOL)

$(HELPVOL): $(HELPSRC)
	dthelptag $(HELPSRC)
#	dthelptag -verbose -charset $(HELPCHARSET) $(HELPSRC)

clean: 
	dthelptag -clean $(HELPSRC)


