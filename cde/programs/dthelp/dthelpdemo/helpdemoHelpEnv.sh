# $XConsortium: helpdemoHelpEnv.sh /main/2 1995/07/19 17:09:31 drk $
##########################################################################
#  (c) Copyright 1993, 1994 Hewlett-Packard Company	
#  (c) Copyright 1993, 1994 International Business Machines Corp.
#  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
#  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
#      Novell, Inc.
#
# Sh and Ksh script to set DTHELPUSERSEARCHPATH to find
# the help volume used by helpdemo.
#
# To activate, execute ". ./helpdemoHelpEnv.sh"
#
##########################################################################
export DTHELPUSERSEARCHPATH="$PWD/help/%H:$PWD/help/%H.hv:$PWD/help/%H.sdl"
