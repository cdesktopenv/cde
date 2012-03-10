# Copyright (c) 1993 Open Software Foundation, Inc., Cambridge, Massachusetts.
# All rights reserved
#
# Copyright (c) 1994  
# Open Software Foundation, Inc. 
#  
# Permission is hereby granted to use, copy, modify and freely distribute 
# the software in this file and its documentation for any purpose without 
# fee, provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation.  Further, provided that the name of Open 
# Software Foundation, Inc. ("OSF") not be used in advertising or 
# publicity pertaining to distribution of the software without prior 
# written permission from OSF.  OSF makes no representations about the 
# suitability of this software for any purpose.  It is provided "as is" 
# without express or implied warranty. 
#
#
# This will extract the name(s) and purpose(s) from and osf-refpage
# instance.  The first line is category attr of osf-refpage.
# The second line is "REF-NAME: ", then the main name/purpose
# (ref-name/ref-purpose).  (name/purpose are separated by a single tab.)
# Subsequent lines are, "SUB-NAME: ", then sub-name/sub-purpose.
#
# Usage: instant -t refname.ts your-document.esis
#
GI:		OSF-REFPAGE
StartText:	CATEGORY: ${CATEGORY x}
EndText:	^
-
GI:		REF-NAME SUB-NAME
StartText:	^${_gi}:\s
-
GI:		REF-PURPOSE SUB-PURPOSE
StartText:	\t
EndText:	^
-
GI:		RSECTION
Ignore:		all
-
Relation:	ancestor REF-NAME
-
GI:		*
Ignore:		data
-
