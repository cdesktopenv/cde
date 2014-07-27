#!/bin/sh
# $XConsortium: :CheckEdit.sh /main/2 1995/07/19 18:04:23 drk $

#
#  check edit of a .udb file.
#  
#  first parm gives the name of the database (with .udb extension)
#
#     Assumes you are in the directory where you made the edit 
#     (formerly tried to run against the checked out copy in 
#     the source tree, this was a mistake :-)
#

/x/toolsrc/dbTools/databaseConversionTool.ksh \
  -toFileList           \
  -ReleaseStream hpux  \
  -Machine 7            \
  -Database $1
