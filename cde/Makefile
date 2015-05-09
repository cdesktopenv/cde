# $TOG: Makefile /main/15 1999/10/12 09:33:30 mgreess $
     RELEASE = Release 2.2.3
     PRODUCT = CDE
       SHELL = /bin/sh
          RM = rm -f
          MV = mv
        MAKE = make
       NMAKE = nmake
   WORLDOPTS = 
WIN32WORLDOPTS = -i
         TOP = .
 CURRENT_DIR = .
   CONFIGSRC = $(TOP)/config
    IMAKESRC = $(CONFIGSRC)/imake
   DEPENDSRC = $(CONFIGSRC)/makedepend
   DEPENDTOP = ../..
    IMAKETOP = ../..
    IRULESRC = $(CONFIGSRC)/cf
       IMAKE = $(IMAKESRC)/imake
   IMAKE_CMD = $(IMAKE) -I$(IRULESRC) $(IMAKE_DEFINES)
   MAKE_OPTS = -f xmakefile
    MAKE_CMD = $(MAKE) $(MAKE_OPTS)
   NMAKE_CMD = $(NMAKE) $(MAKE_OPTS)
       FLAGS = $(MFLAGS) -f Makefile.ini BOOTSTRAPCFLAGS="$(BOOTSTRAPCFLAGS)"

all:
	@$(MAKE_CMD) xmakefile-exists || $(MAKE) all-initial
	@$(MAKE_CMD) $@

all-initial:
	@echo "Please begin by using make World or make Makefile.  You may"
	@echo "find it necessary to set the make variable BOOTSTRAPCFLAGS in"
	@echo "order to compile imake.  One common example is"
	@echo "\"BOOTSTRAPCFLAGS=-I<dir>\", where <dir> is the top of your"
	@echo "X11R6 source tree.  Remember to check the configuration"
	@echo "parameters in the config directory."
	@echo
	@echo "We recommend that you read the release notes carefully before"
	@echo "proceeding; they can be found under the doc directory.  If"
	@echo "the top-level Makefile gets corrupted, copy Makefile.ini to"
	@echo "Makefile and try again.  Do not name your log file make.log or"
	@echo "it will be deleted."

World:
	@echo ""
	@echo "Building $(RELEASE) of $(PRODUCT)"
	@echo ""
	@case "x$(BOOTSTRAPCFLAGS)" in x) \
	echo I hope you checked the configuration parameters in $(IRULESRC) ; \
	echo to see if you need to pass BOOTSTRAPCFLAGS. ; \
	echo "" ; \
	;; esac;
	@date
	@echo ""
	cd $(IMAKESRC) && $(MAKE) $(FLAGS) clean
	$(MAKE) $(MFLAGS) Makefile.boot
	$(MAKE_CMD) $(MFLAGS) VerifyOS
	$(MAKE_CMD) $(MFLAGS) Makefiles
	$(MAKE_CMD) $(MFLAGS) Makefiles.doc
	$(MAKE_CMD) $(MFLAGS) clean
	$(MAKE_CMD) $(MFLAGS) clean.doc
	$(MAKE_CMD) $(MFLAGS) includes
	$(MAKE_CMD) $(MFLAGS) includes.doc
	$(MAKE_CMD) $(MFLAGS) depend
	$(MAKE_CMD) $(MFLAGS) depend.doc
	$(MAKE_CMD) $(MFLAGS) $(WORLDOPTS)
	$(MAKE_CMD) $(MFLAGS) $(WORLDOPTS) all.doc
	@echo ""
	@date
	@echo ""
	@echo "Full build of $(RELEASE) of $(PRODUCT) complete."
	@echo ""


World.dev:
	@echo ""
	@echo "Building $(RELEASE) of $(PRODUCT) excluding DOC"
	@echo ""
	@case "x$(BOOTSTRAPCFLAGS)" in x) \
	echo I hope you checked the configuration parameters in $(IRULESRC) ; \
	echo to see if you need to pass BOOTSTRAPCFLAGS. ; \
	echo "" ; \
	;; esac;
	@date
	@echo ""
	cd $(IMAKESRC) && $(MAKE) $(FLAGS) clean
	$(MAKE) $(MFLAGS) Makefile.boot
	$(MAKE_CMD) $(MFLAGS) VerifyOS
	$(MAKE_CMD) $(MFLAGS) Makefiles
	$(MAKE_CMD) $(MFLAGS) clean
	$(MAKE_CMD) $(MFLAGS) includes
	$(MAKE_CMD) $(MFLAGS) depend
	$(MAKE_CMD) $(MFLAGS) $(WORLDOPTS)
	@echo ""
	@date
	@echo ""
	@echo "Full build of $(RELEASE) of $(PRODUCT) excluding DOC complete."
	@echo ""

World.doc:
	@echo ""
	@echo "Building $(RELEASE) of $(PRODUCT) DOC"
	@echo ""
	@case "x$(BOOTSTRAPCFLAGS)" in x) \
	echo I hope you checked the configuration parameters in $(IRULESRC) ; \
	echo to see if you need to pass BOOTSTRAPCFLAGS. ; \
	echo "" ; \
	;; esac;
	@date
	@echo ""
	cd $(IMAKESRC) && $(MAKE) $(FLAGS) clean
	$(MAKE) $(MFLAGS) Makefile.boot
	$(MAKE_CMD) $(MFLAGS) VerifyOS
	$(MAKE_CMD) $(MFLAGS) Makefiles.doc
	$(MAKE_CMD) $(MFLAGS) clean.doc
	$(MAKE_CMD) $(MFLAGS) includes.doc
	$(MAKE_CMD) $(MFLAGS) depend.doc
	$(MAKE_CMD) $(MFLAGS) $(WORLDOPTS) all.doc
	@echo ""
	@date
	@echo ""
	@echo "Full build of $(RELEASE) of $(PRODUCT) DOC complete."
	@echo ""

.PRECIOUS: Makefile

# This is just a sequence of bootstrapping steps we have to do.
# The steps are listed as separate targets so clearmake can wink in
# the Makefile.proto files.
Makefile.boot: imake.proto $(DEPENDSRC)/Makefile.proto depend.bootstrap $(IMAKESRC)/Makefile.proto imake.bootstrap $(DEPENDSRC)/proto.clean

imake.proto:
	cd $(IMAKESRC) && $(MAKE) $(FLAGS)
	$(RM) $(DEPENDSRC)/Makefile.proto

$(DEPENDSRC)/Makefile.proto:
	$(IMAKE_CMD) -s $(DEPENDSRC)/Makefile.proto -f $(DEPENDSRC)/Imakefile -DTOPDIR=$(DEPENDTOP) -DCURDIR=$(DEPENDSRC)

depend.bootstrap:
	cd $(DEPENDSRC) && $(RM) -r Makefile Makefile.dep makedepend *.o bootstrap
	cd $(DEPENDSRC) && $(MAKE) -f Makefile.proto bootstrap

$(IMAKESRC)/Makefile.proto:
	$(IMAKE_CMD) -s $(IMAKESRC)/Makefile.proto -f $(IMAKESRC)/Imakefile -DTOPDIR=$(IMAKETOP) -DCURDIR=$(IMAKESRC)

imake.bootstrap:
	cd $(IMAKESRC) && $(MAKE) -f Makefile.proto bootstrapdepend
	cd $(IMAKESRC) && $(MAKE) $(FLAGS) bootstrap
	cd $(IMAKESRC) && $(MAKE) -f Makefile.proto all 
	-@if [ -f xmakefile ]; then set -x; \
	  $(RM) xmakefile.bak; $(MV) xmakefile xmakefile.bak; \
	  else exit 0; fi
	$(MAKE) $(MFLAGS) xmakefile

$(DEPENDSRC)/proto.clean:
	cd $(DEPENDSRC) && $(RM) -r Makefile.proto

Makefile::
	$(MAKE) $(MFLAGS) xmakefile

xmakefile: Imakefile
	$(IMAKE_CMD) -s xmakefile -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT_DIR)

World.Win32:
	@echo :
	@echo Building $(RELEASE) of $(PRODUCT).
	@echo :
	@echo :
	cd $(IMAKESRC)
	$(NMAKE) -f Makefile.ini clean.Win32
	$(NMAKE) -f Makefile.ini imake.exe
	cd ..\..
	-if exist xmakefile.bak del xmakefile.bak
	-if exist xmakefile ren xmakefile xmakefile.bak
	$(IMAKE:/=\) -s xmakefile -I$(IRULESRC) $(IMAKE_DEFINES) -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT_DIR)
	$(NMAKE_CMD) $(MFLAGS) VerifyOS
	$(NMAKE_CMD) $(MFLAGS) Makefiles
	$(NMAKE_CMD) $(MFLAGS) clean
	$(NMAKE_CMD) $(MFLAGS) includes
	$(NMAKE_CMD) $(MFLAGS) depend
	$(NMAKE_CMD) $(MFLAGS) $(WIN32WORLDOPTS)
	@echo :
	@echo :
	@echo Full build of $(RELEASE) of $(PRODUCT) complete.
	@echo :


# don't allow any default rules in this Makefile
.SUFFIXES:

# a copy of every rule that might be invoked at top level

clean:
	    $(MAKE_CMD) $@
dangerous_strip_clean:
	    $(MAKE_CMD) $@
depend:
	    $(MAKE_CMD) $@
Everything:
	    $(MAKE_CMD) $@
external.ln:
	    $(MAKE_CMD) $@
includes:
	    $(MAKE_CMD) $@
install.ln:
	    $(MAKE_CMD) $@
install.man:
	    $(MAKE_CMD) $@
install:
	    $(MAKE_CMD) $@
Makefiles:
	    $(MAKE_CMD) $@
man_keywords:
	    $(MAKE_CMD) $@
tags:
	    $(MAKE_CMD) $@
VerifyOS:
	    $(MAKE_CMD) $@

# dev versions

Everything.dev:
	    $(MAKE_CMD) $@

# doc versions

clean.doc:
	    $(MAKE_CMD) $@
depend.doc:
	    $(MAKE_CMD) $@
Everything.doc:
	    $(MAKE_CMD) $@
includes.doc:
	    $(MAKE_CMD) $@
install.man.doc:
	    $(MAKE_CMD) $@
install.doc:
	    $(MAKE_CMD) $@
Makefiles.doc:
	    $(MAKE_CMD) $@
tags.doc:
	    $(MAKE_CMD) $@
