#!/bin/sh
# $XConsortium: crayar.sh /main/2 1995/07/19 18:05:29 drk $
lib=$1
shift
if cray2; then
        bld cr $lib `lorder $* | tsort`
else
        ar clq $lib $*
fi

