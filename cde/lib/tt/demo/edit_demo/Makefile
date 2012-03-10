# $XConsortium: Makefile /main/6 1996/05/13 12:04:04 drk $
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

Sun_EditDemo: cntl edit edit.types

edit:
	LD_RUN_PATH=$(MOTIFHOME)/lib:$(OPENWINHOME)/lib $(LINK.c) -o $@ edit.c $(LDLIBS)
cntl:
	LD_RUN_PATH=$(MOTIFHOME)/lib:$(OPENWINHOME)/lib $(LINK.c) -o $@ cntl.c $(LDLIBS)

edit.types: edit.types.model
	tt_type_comp '-DTOOLTALK_DEMO_DIR=\"`pwd`\"' edit.types.model
	@echo ToolTalk types installed for Sun_EditDemo.
	@echo Remember to \"kill -USR2\" the ttsession process so the new
	@echo types will be active.

clean:
	rm -f cntl edit core edit.types

FRC:
