# $XConsortium: Makefile /main/6 1996/05/13 12:04:22 drk $
# %%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
# %%  (c) Copyright 1993, 1994 International Business Machines Corp.	
# %%  (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.		
# %%  (c) Copyright 1993, 1994 Novell, Inc. 				

OPENWINHOME=/usr/openwin
MOTIFHOME=/usr/dt

CFLAGS += -g
CPPFLAGS += -I$(MOTIFHOME)/include -I$(OPENWINHOME)/include
LDFLAGS += -L$(MOTIFHOME)/lib -L$(OPENWINHOME)/lib
LDLIBS += -lXm -lXt -ltt -lX11 -lgen

.KEEP_STATE:

ttsample1:
	LD_RUN_PATH=$(MOTIFHOME)/lib:$(OPENWINHOME)/lib $(LINK.c) -o $@ ttsample1.c $(LDLIBS)

clean:
	rm -f ttsample1

FRC:
