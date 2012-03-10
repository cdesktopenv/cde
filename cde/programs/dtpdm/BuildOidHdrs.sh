#!/bin/sh
# $XConsortium: BuildOidHdrs.sh /main/3 1996/08/12 18:41:52 cde-hp $
# 
#  (c) Copyright 1996 Digital Equipment Corporation.
#  (c) Copyright 1996 Hewlett-Packard Company.
#  (c) Copyright 1996 International Business Machines Corp.
#  (c) Copyright 1996 Sun Microsystems, Inc.
#  (c) Copyright 1996 Novell, Inc. 
#  (c) Copyright 1996 FUJITSU LIMITED.
#  (c) Copyright 1996 Hitachi.
# 

in_file=PdmOidDefs.in
oid_file=PdmOidDefs.h
str_file=PdmOidStrs.h

tmpbase=`basename $0`
oid_tmp=$tmpbase.oid_tmp
str_tmp=$tmpbase.str_tmp

rm -f $oid_tmp > /dev/null 2>&1
rm -f $str_tmp > /dev/null 2>&1

first=1

echo "parsing $in_file \c"

cat $in_file |
(
    while read oid str mset mnum defmsg
    do
	if [ -n "$oid" -a "$oid" != "#" ]
	then
	    if [ $first -eq 1 ]
	    then
		first=0
	    else
		echo "," >> $oid_tmp
		echo "," >> $str_tmp
	    fi
	    echo "    $oid\c" >> $oid_tmp
	    str_length=`echo "$str\c" | wc -c`
	    if [ -z "$mset" ] ; then
		mset="-1"
	    fi
	    if [ -z "$mnum" ] ; then
		mnum="-1"
	    fi
	    if [ -z "$defmsg" ] ; then
		defmsg="(const char*)NULL"
	    else
		defmsg="\"$defmsg\""
	    fi
	    echo "    { \"$str\", $str_length, $mset, $mnum, $defmsg }\c" \
		>> $str_tmp
	    echo ".\c"
	fi
    done
    echo >> $oid_tmp
    echo >> $str_tmp
)

echo "done"

echo "creating $oid_file ...\c"

(
    echo '/* $'XConsortium'$ */'
    echo '/* This is an automatically-generated file. Do not edit. */'
    echo '/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */'
    echo
    echo 'typedef enum {'
    cat $oid_tmp
    echo '} PdmOid;'
) > $oid_file

echo "done"

echo "creating $str_file ...\c"

(
    echo '/* $'XConsortium'$ */'
    echo '/* This is an automatically-generated file. Do not edit. */'
    echo '/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */'
    echo
    echo "static int PdmOidStringMapCount = `cat $str_tmp | wc -l`;"
    echo
    echo 'static const PdmOidStringMapEntry PdmOidStringMap[] = {'
    cat $str_tmp
    echo '};'
) > $str_file

echo "done"

rm -f $oid_tmp > /dev/null 2>&1
rm -f $str_tmp > /dev/null 2>&1
