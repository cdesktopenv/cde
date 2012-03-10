#!/bin/ksh
# $XConsortium: env.sh /main/3 1996/06/19 17:12:59 drk $
# This file is to be "sourced" into your current shell (i.e., invoked
# as ". env.sh").  It's purpose is to set up environment variables to
# point to the install locations of the pieces of the dtdocbook
# program.

# If all the things you need are based on a common path, simply
# setting PATH_TOP to that path will make the necessary
# customization.  Otherwise, the path to each directory will have to
# be edited individually.

PATH_TOP=/projects/help/sgml/OSF/tpt/DtDocBook

# the dtdocbook program and its constituent executable parts
export PATH=${PATH}:${PATH_TOP}/BinStuff

# the dtdocbook style sheet and character maps
export TPT_LIB=${PATH_TOP}/LibStuff:${PATH_TOP}/SgmlStuff:${PATH_TOP}/BinStuff

# the DocBook SGML declaration
export SGML_DIR=${PATH_TOP}/SgmlStuff

# the icons for note/caution/warning (tip/important)
export ICON_DIR=${PATH_TOP}/Icons

# the catalog directory for finding PUBLIC entities
# you will want to edit and run the catalog.sh or catalog.csh program
# in the Catalog directory to build the desired symbolic links to your
# actual entities
export SGML_PATH=${PATH_TOP}/Catalog/%P
