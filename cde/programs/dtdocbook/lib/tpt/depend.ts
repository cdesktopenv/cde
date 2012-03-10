# ______________________________________________________________________
# Copyright 1993 Open Software Foundation, Inc., Cambridge, Massachusetts.
# All rights reserved.
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
# $XConsortium: depend.ts /main/2 1996/07/18 14:22:57 drk $
# ______________________________________________________________________
#
# Generate part of the dependency list for Makefiles.
# For referenced graphic entities, output a line like:
#	input-file.sgml: graphic-file.eps
# For elements whose name matches a blessed list, output a line like this:
#	ID.latex: input-file.sgml
#
# These are sane places where you can start formatting
GI:	CHAPTER SECTION DIVISION DISPLAY TABLE APPENDIX LIST
	OSF-REFPAGE PART PREFACE PDIVSION OSF-BOOK
StartText: ${_attval ID 10}
Ignore: data
-
GI: GRAPHIC
Replace: ${_infile root}.dvi:\t${_filename}\n
-
GI: XREF
Replace: ${_infile root}.dvi:\t${_followlink LINKEND 20}\n
-
GI: *
Ignore: data
-
GI:	_id
SpecID:	10
Replace: ${ID}.latex:\t${_infile}\n
-
GI:	_xref
SpecID:	20
Replace: ${_infile}
#Message: XREF ${_infile}\n
-
