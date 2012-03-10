# $XConsortium: DtFuncs.compat.sh /main/1 1995/11/01 15:48:48 rswiston $ 
#
#   COMPONENT_NAME: desktop
#
#   FUNCTIONS: DtAddButtons
#		DtFloatBottom
#		DtFloatLeft
#		DtFloatRight
#		DtFloatTop
#		DtLeftOf
#		DtOver
#		DtRightOf
#		DtSetReturnKeyControls
#		DtUnder
#
#   ORIGINS: 27,118,119,120,121
#
#   This module contains IBM CONFIDENTIAL code. -- (IBM
#   Confidential Restricted when combined with the aggregated
#   modules for this product)
#   OBJECT CODE ONLY SOURCE MATERIALS
#
#   (C) COPYRIGHT International Business Machines Corp. 1995
#   All Rights Reserved
#   US Government Users Restricted Rights - Use, duplication or
#   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
###############################################################################
#  (c) Copyright 1993, 1994 Hewlett-Packard Company	
#  (c) Copyright 1993, 1994 International Business Machines Corp.
#  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
#  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
#      Novell, Inc.
###############################################################################
# This script is for compatibility with previous AIXwindows Desktop releases.
###############################################################################
. /usr/dt/lib/dtksh/DtFuncs.dtsh
alias DtAddButtons="DtkshAddButtons"
alias DtSetReturnKeyControls="DtkshSetReturnKeyControls"
alias DtUnder="DtkshUnder"
alias DtOver="DtkshOver"
alias DtRightOf="DtkshRightOf"
alias DtLeftOf="DtkshLeftOf"
alias DtFloatRight="DtkshFloatRight"
alias DtFloatLeft="DtkshFloatLeft"
alias DtFloatTop="DtkshFloatTop"
alias DtFloatBottom="DtkshFloatBottom"
alias DtAnchorRight="DtkshAnchorRight"
alias DtAnchorLeft="DtkshAnchorLeft"
alias DtAnchorTop="DtkshAnchorTop"
alias DtAnchorBottom="DtkshAnchorBottom"
alias DtSpanWidth="DtkshSpanWidth"
alias DtSpanHeight="DtkshSpanHeight"
alias DtDisplayErrorDialog="DtkshDisplayErrorDialog"
alias DtDisplayQuestionDialog="DtkshDisplayQuestionDialog"
alias DtDisplayWorkingDialog="DtkshDisplayWorkingDialog"
alias DtDisplayWarningDialog="DtkshDisplayWarningDialog"
alias DtDisplayInformationDialog="DtkshDisplayInformationDialog"
alias DtDisplayQuickHelpDialog="DtkshDisplayQuickHelpDialog"
alias DtDisplayHelpDialog="DtkshDisplayHelpDialog"
