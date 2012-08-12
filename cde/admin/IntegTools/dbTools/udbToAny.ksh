#!/bin/ksh 

# ------------------------------------------------------------- 
#  udbToAny.ksh 
# 
#     This script was leveraged from "databaseConversionTool.ksh" 
# it should provide a superset of the functionality of that script; 
# however the primary motivation was to get away from the use of
# multiple divergent parsers for the ".udb" style database.  The
# parser has been moved into an awk "library" file: udbParseLib.awk.
# this parser can and should be used by all scripts wishing to parse
# the ".udb" data bases. 
#
# ----------------------------
# new features:
#
#  -toDB		to convert to old mksubmit-style ".db" format
#  -toLst  		to convert to old domain-port-style ".lst" files
#  -toCustom <prt>	to specify a print routine for a custom format
#  -custom <awklib>	to specify a library containing custom print routines.
#  -udbParseLib <awkLib> to specify a library containing an alternate parser. 
#  -mailTo  <user>	to specify an administrator who will receive mail
#			concerning parsing errors. 
#
#  -Database		is now obsolete (although it will still work)
#  <udbfile> ... ...	The script can now take multiple udb files (Only the
#			defaults specified in the first udb file will apply).
#  
# ----------------------------
#
#     This script converts a universal database to an 
# HP OSF style System Definition Database (SDD) file,
# or a set of args suitable to supply to the Chelmsford deliver
# tool, or a set of commands to build a delivery tree.
# For more details, please refer to the "X11 and VUE for HP_OSF/1.0"
# document.
#
#  This script was leveraged (read hacked extensively) from
#  the "udbToDb" script by Jim Andreas.  Ron Voll authored the
#  original "udbToDb" script.
#
# -------------------------------------------------------------

# usage:  databaseToSDD.ksh Option udbFile
#
# where Option is one of the following:
#
# -toDB		     convert a .udb to ".db" format
# -toLst	     convert a .udb to ".lst" format
# -toSDD             convert a .udb to SDD format
# -toDeliverArgs     convert a .udb to args that the deliver tool likes
# -toReleaseTree     convert a .udb to a script to build a delivery tree
# -toCheckBuild      convert a .udb to a script to check the items in a build
# -toFileList        convert a .udb to a list of files for carbon units
# -Machine           specifies 3,7,8 for hp-ux releases
# -ReleaseStream     {hp-ux, osf, whatever} 
# -NoDefaults        do not convert any records labeled default
# -toTargetList      convert a .udb to a list of target files in product tree
#                      the leading path is stripped and just the target
#                      file is left - for easy diffing with some other
#                      version of a release
# -custom <awkFile>		Supply a custom "awk" print library
# -toCustom <prt routine>
# -udbParseLib <awkFile>	Supply an alternate  "awk" parser library


# -------------------------------------------------------------
#  ConvertRoutine
# 
#   This ksh function invokes awk to do all of the dirty
# work.  The DoAction variable is consulted only in the
# final stages of printing the desired info after the
# .udb "phrases" are parsed.
#
# -------------------------------------------------------------
ConvertRoutine()
{


#
# set what we are going to do
#
    typeset DoAction=$1

#
# set the "release stream" or token in a block in the .udb
# file for which we will scan.
#
    typeset BlockToken=$2

# 
# and for HP-UX releases, the particular machine 68k/Snake/S800 3/7/8
#
    typeset machine=$3
# 
# set flag if default blocks are to be processed
#
    typeset UseDefaultBlocks=$4

    shift
    shift
    shift
    shift

    AWK=/usr/bin/awk
    if [ -x /usr/bin/nawk ]; then
	AWK=/usr/bin/nawk
    fi

#
# Custom print routines may use the following parser variables:
#	defOrder[]	--- An array containing the names of the fields in
#			    a data base record (in the correct order).
#	NumEntries	--- One MORE than the number of entries in the
#			    "defOrder" array.  This is the number of fields
#			    in a data base record.
#	rec[]		--- An associative array indexed by data base record
#			    field name containing the value of the field.
#
# Assign custom print routines to be used for output.  The default is to
# use the "printDb" function associated with the library.
#

    typeset	PRT=printDb
    case "$DoAction" in
    toDB)
	PRT=printDb
        ;;
    toLst)
	PRT=printLst
        ;;
    toFileList|toTargetList|toCheckBuild|toReleaseTree|toDeliverArgs|toSDD)
	PRT=printGather;
	;;
    toCustom) 
	CUSTOM_PRINT="-f $CUSTOM_PRINT_LIB"
	PRT=$CUS_PRT
	;;
    *)	# Unknown Action
	echo "$0: Unknown Action>> \"$doAction\""
	exit 1;
	;;
    esac

   cat > $TMPFILE <<EOF
#
# The function name "PRTREC" is used by the parsing routines
# to do the output. By providing a custom output function you
# can print the database any way you want.  The default is to
# use the "printRecord" function built defined in the awk file
# containing the awk parser.
#
function PRTREC(rec) {
	$PRT(rec)
}
BEGIN {
	parseUdb()
}
{
	print "Getting New Line AWK style -- Problem?"		
	exit 1
}
EOF

   #
   # Create a single awk file for use with the "-f" parameter.
   # IBM's awk only allows one "-f"
   #
    cat "$UDB_PARSE_LIB" >> $TMPFILE
    [ -z "$CUSTOM_PRINT" ]  || cat "$CUSTOM_PRINT_LIB" >> $TMPFILE

    $AWK -v mailTo="$Administrator" \
	 -v action="$DoAction" \
	 -v BlockToken="$BlockToken" \
	 -v Machine="$machine" \
	 -v UseDefaultBlocks="$UseDefaultBlocks" \
	 -v DeBugFile="$DEBUGFILE" \
	 -v DeBug="$DEBUGLEVEL" \
	 -f $TMPFILE $*


#
# Removed from parameter list because IBM's awk only allows one "-f"
#	 $CUSTOM_PRINT \
#	 -f "$UDB_PARSE_LIB" \
#

     rm $TMPFILE
}

#
#  print a handy usage message to stderr (file descriptor 2 )
#
#
usage()
{
    exec >&2

    echo "$ScriptName: usage:" 
    echo ""
    echo "  $ScriptName [Options] <UdbFile> ..."
    echo ""
    echo "     -toDB              convert a .udb to \".db\" format"
    echo "     -toLst              convert a .udb to \".lst\" format"
    echo "     -toSDD             convert a .udb to SDD format"
    echo "     -toDeliverArgs     convert a .udb to args that the deliver tool likes"
    echo "     -toReleaseTree     convert a .udb to a script to build a delivery tree"
    echo "     -toCheckBuild      convert a .udb to a script to check a build"
    echo "     -toFileList        convert a .udb to a list of files"
    echo "     -toTargetList      convert a .udb to a list of product files"
    echo "     -ReleaseStream     {hp-ux, osf, whatever}"
    echo "     -Machine           specifies machine 3,7,8 for hp-ux"
    echo "     -NoDefaults        do not convert any records labeled \"default\""
    echo "     -Database path     (obsolete) specifies full path to the .udb file to convert"
    echo "   -mailTo  <user>	  Specify a user to receive mail on errors."
    echo "   -toCustom	<prt>	  Specify the name of a custom print routine."  
    echo "   -custom <awkFile>    Supply a custom "awk" print library."
    echo " -udbParseLib <awkFile> Supply an alternate 'awk' parser library"
    exit 1
}


# OK, here is where we really start execution.
#   Check that the first argument defines what this script is
#  supposed to do:

#    Obscurity footprint-in-the-sand:  "${1##*/}" is equivalent
#      to basename(1)
#
ScriptName=${0##*/}

# -toSDD             convert a .udb to SDD format
# -toDeliverArgs     convert a .udb to args that the deliver tool likes
# -toReleaseTree     convert a .udb to a script to build a delivery tree
# -toCheckBuild      convert a .udb to a script to check the items in a build

if [ $# -le 3 ]; then
    usage $0
fi

typeset TakeDefaultBlocks="Y"
typeset Administrator=""
#typeset DBTOOLSRC=/x/toolsrc/dbTools
typeset DBTOOLSRC=`dirname $0`
typeset UDB_PARSE_LIB="$DBTOOLSRC/udbParseLib.awk"
typeset CUSTOM_PRINT_LIB=""
typeset DEBUGFILE="/dev/tty"
typeset DEBUGLEVEL=0
typeset TMPFILE=`mktemp /tmp/awkXXXXXXXXXXXXXXXXXXXXX`

if [ $# -gt 2 ]; then 
    while [ $# -gt 0 ]
    do
	case $1 in 
	-NoDefaults)
	    TakeDefaultBlocks=N
	    shift
	    continue
	    ;;
	-toDB)
	    Action=toDB
	    shift;
	    continue;
	    ;;
	-toLst)
	    Action=toLst
	    shift;
	    continue;
	    ;;
	-toSDD)
	    Action=toSDD
	    shift
	    continue
	    ;;
	-toDeliverArgs)
	    Action=toDeliverArgs
	    shift
	    continue
	    ;;
	-toReleaseTree)
	    Action=toReleaseTree
	    shift
	    continue
	    ;;
	-toCheckBuild)
	    Action=toCheckBuild
	    shift
	    continue
	    ;;
	-toTargetList)
	    Action=toTargetList
	    shift
	    continue
	    ;;
	-toFileList)
	    Action=toFileList
	    shift
	    continue
	    ;;
	-Machine)
	    if [ "x$2" = "x" ]; then
		usage
	    fi
	    Machine=$2
	    shift
	    shift
	    continue
	    ;;
	-ReleaseStream)
            if [ "x$2" = "x" ]; then
                usage
            fi
            ReleaseStream=$2
            shift
            shift
            continue
	    ;;
	-Database)
            if [ "x$2" = "x" ]; then
                usage
            fi
	    if [ ! -r "$2" ]; then 
		usage
	    fi
            Database="$Database $2"
            shift
            shift
            continue
	    ;;
	-udbParseLib) # specify alternate "awk" parser location
            if [ "x$2" = "x" ]; then
                usage
            fi
	    if [ ! -r "$2" ]; then 
		usage
	    fi
            UDB_PARSE_LIB=$2
            shift
            shift
            continue
	    ;;
	-toCustom) # specify custom "awk" print routines
            if [ "x$2" = "x" ]; then
                usage
            fi
	    Action=toCustom
            CUS_PRT=$2
            shift
            shift
            continue
	    ;;
	-custom) # specify custom "awk" print routines
            if [ "x$2" = "x" ]; then
                usage
            fi
	    if [ ! -r "$2" ]; then 
		usage
	    fi
            CUSTOM_PRINT_LIB=$2
            shift
            shift
            continue
	    ;;
	-mailTo) # specify an administrator who receives mail about errors.
            if [ "x$2" = "x" ]; then
                usage
            fi
            Administrator=$2
            shift
            shift
            continue
	    ;;
	-DeBugFile) # specify a debug file and debug level for parser debug info
            if [ "x$2" = "x" ]; then
                usage
            fi
            if [ "x$3" = "x" ]; then
                usage
            fi
            DEBUGFILE=$2
	    shift 2
            continue
	    ;;
	-DeBugLevel) # specify a debug level for parser debug info
            if [ "x$2" = "x" ]; then
                usage
            fi
            if [ "x$3" = "x" ]; then
                usage
            fi
            DEBUGLEVEL=$2
	    shift 2
            continue
	    ;;
	-*)
	    echo "unknown option: $1"
	    echo ""
	    usage
	    exit 1;
	    ;;
	*) if [ ! -r $1 ]; then
		usage
	   fi
	   Database="$Database $1"
	   shift;
	   ;;
	esac
    done
fi

if [ "$Action" = "toCustom" ]; then
	if [ -z "$CUSTOM_PRINT_LIB" ]; then
		echo "You Must specify an awk file containing the custom print routine \"$CUS_PRT\""
		exit 1;
	fi
fi


if [ "$Machine" = "" ]; then
    Machine=NA
elif [ "$Machine" = "300" ]; then
    Machine="3"
elif [ "$Machine" = "700" ]; then
    Machine="7"
elif [ "$Machine" = "800" ]; then
    Machine="8"
fi
if [ "$ReleaseStream" = "" ]; then
    echo "$ScriptName: need to specify a -ReleaseStream" >&2
    exit 1
fi
if [ "$Database" = "" ]; then
    echo "$ScriptName: need to specify a -Database" >&2
    exit 1
fi


ConvertRoutine $Action $ReleaseStream $Machine $TakeDefaultBlocks $Database 
