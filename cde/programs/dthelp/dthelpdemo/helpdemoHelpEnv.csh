##########################################################################
#  (c) Copyright 1993, 1994 Hewlett-Packard Company	
#  (c) Copyright 1993, 1994 International Business Machines Corp.
#  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
#  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
#      Novell, Inc.
#
#  Csh script to set DTHELPUSERSEARCHPATH to find
#  the help volume used by helpdemo.
#
#  To activate, execute "source ./helpdemoHelpEnv.csh"
#
##########################################################################
set PWD = `pwd`
setenv DTHELPUSERSEARCHPATH $PWD/help/%H:$PWD/help/%H.hv:$PWD/help/%H.sdl
