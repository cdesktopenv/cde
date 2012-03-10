#!/bin/sh
# %%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
# %%  (c) Copyright 1993, 1994 International Business Machines Corp.	
# %%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
# %%  (c) Copyright 1993, 1994 Novell, Inc. 				
# %%  $XConsortium: ttinstall.sh /main/3 1995/10/20 16:23:17 rswiston $ 			 				
# @(#)ttinstall.sh	1.10 30 Jul 1993
# Shell script for doing installs.
# The old bsd install did pretty much what we want, but the SYSV install
# doesn't.  Hence this shell script which does what we need.
# 
# Usage:  ttinstall <version> <dest-dir> file...
# 
# ttinstall will create dest-dir if needed and copy each named file
# to that directory.  If a file's name ends in .so.[0-9] or .so.[0-9][0-9],
# a symlink is created in the destination directory without the version.
# (This breaks if any shared library has a version over 99, but I'm not
# worried.)
# If the file is ELF (binary or shared library), then mcs is used to
# strip the comment and replace it with a short one containing
# the version.

if [ "$RECURSING_ON" != "$0" ]; then
	RECURSING_ON="$0"
	export RECURSING_ON
	exec ksh "$0" "$@"
fi

# HP-UX mkdir -p fails to work if any of the components
# in the name are automounter symlinks... so fake mkdir -p with
# a recursive shell function.

function mkdir_p
{
# if directory names have trailing /, we get called with a null string	
	if	[[ "$1" = "" ]]
	then	return
	fi
# bail out if directory already exists -- this means recursion stops
# as soon as we back up into an existing directory	
	if	[[ -d $1 ]]
	then	return
	fi
# only recurse if path has a slash in it. 	
	case $1 in
	  */*)	mkdir_p ${1%/*}
	  	mkdir $1
		;;
	  *)	mkdir $1
	  	;;
	esac
	return
}			

version=$1; shift
destdir=$1; shift
datestamp=`date '+%d %h %y'`

if [[ ! -d $destdir ]]
then	print -n -u2 Creating directory $destdir ...
	rm -f $destdir
	mkdir_p $destdir
	print done.
fi

while test "$1"
do	file="`basename $1`"
	print -n -u2 Installing $1 in $destdir ...
	rm -f $destdir/${1##*/}
	# Try to install by linking, otherwise copy
	ln $1 $destdir || cp $1 $destdir
	case $1 in
	    *.so.[0-9])	print -n -u2 adding symlink ...
			rm -f $destdir/${file%.[0-9]}
			ln -s $file $destdir/${file%.[0-9]}
			;;
	    *.so.[0-9][0-9]) print -n -u2 adding symlink ...
			rm -f $destdir/${file%.[0-9][0-9]}
			ln -s $file $destdir/${file%.[0-9][0-9]}
			;;
	esac
	print -u2 done.
	filetype=`file $1`
	case $filetype in
	    *ELF*) print -n -u2 stripping comments ...
		mcs -d -a "@(#)ToolTalk $version $datestamp" $destdir/${1##*/}
		print -u2 done.
		;;
	esac
	shift
done
