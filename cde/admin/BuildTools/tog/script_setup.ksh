#!/bin/ksh
#
# script_setup.ksh
#
########################################################################
#set -x

##########################################################################
##########################################################################
#
# Common global code
#
##########################################################################
##########################################################################
umask 002
TEMPORARY_FILES=""
trap '/bin/rm -f $TEMPORARY_FILES; exit 1' INT QUIT TERM

do_exit ()
{
    do_delete_temporary_files
    exit $1
}

do_delete_temporary_files ()
{
    if [ -n "$TEMPORARY_FILES" ]; then
	/bin/rm -f $TEMPORARY_FILES
	TEMPORARY_FILES=""
    fi
}

do_register_temporary_file ()
{
    if [ -n "$1" ]; then
	if [ -z "$TEMPORARY_FILES" ]; then
	    TEMPORARY_FILES="$1"
	else
	    TEMPORARY_FILES="$TEMPORARY_FILES $1"
	fi
    fi
}

do_check_file ()
{
	# $1 = the file to check
	# $2 = file flag (e.g. "x" for -x, "f" for -f, etc.
	# #3 = error message

	case $2 in
		-x)	if [ ! -x $1 ]; then
				print -u2 "Exiting ...  Executable '$1 $3"
				do_exit 1
			fi ;;
		-d)	if [ ! -d $1 ]; then
				print -u2 "Exiting ...  Directory '$1 $3"
				do_exit 1
			fi ;;
		-f)	if [ ! -f $1 ]; then
				print -u2 "Exiting ...  File '$1 $3"
				do_exit 1
			fi ;;
		-L)	if [ ! -L $1 ]; then
				print -u2 "Exiting ...  Sym link '$1 $3"
				do_exit 1
			fi ;;
		*)	print -u2 "Exiting ... '$1' NOT found!"
			do_exit 1 ;;
	esac
}

##########################################################################
##########################################################################
#
# Directory and executable paths.
#
CLEAR_CASE_TOOL=/usr/atria/bin/cleartool
if [ "" = "$PROJECT_NAME" ]; then
	LOG_DIR_BASE=/project/dt/logs/build
else
	LOG_DIR_BASE=/project/${PROJECT_NAME}/logs/build
fi


##########################################################################
#
# Initialize the scripts and data files
#
BUILD_PROJECT=$SCRIPTS_DIR/build_project
BUILD_SUMMARY=$SCRIPTS_DIR/build_summary
BUILD_WORLD=$SCRIPTS_DIR/build_world
COMPONENT_FILE="-c $SCRIPTS_DIR/cde.components"
COMPRESS_MSG=$SCRIPTS_DIR/compress_msg
EXTRACT_LOG=$SCRIPTS_DIR/extract_log
EXTRACT_MSG=$SCRIPTS_DIR/extract_msg
INITIALIZE_VIEW=$SCRIPTS_DIR/initialize_view

CDE_COMPONENTS=$SCRIPTS_DIR/cde.components
CDEDOC_COMPONENTS=$SCRIPTS_DIR/cdedoc.components
CDETEST_COMPONENTS=$SCRIPTS_DIR/cdetest.components
MOTIF_COMPONENTS=$SCRIPTS_DIR/motif.components
X_COMPONENTS=$SCRIPTS_DIR/x11.components
BUILD_MSGS=$SCRIPTS_DIR/dt_make.msg
ERROR_MSGS=$SCRIPTS_DIR/dt_errors.msg
IGNORE_MSGS=$SCRIPTS_DIR/dt_ignore.msg
WARNING_MSGS=$SCRIPTS_DIR/dt_warnings.msg


##########################################################################
#
# Strings used in the Subject line of mailed reports
#
SUBJECT_BUILD_SUMMARY="CDE: Build Summary"
SUBJECT_SUBMISSIONS="CDE: Submissions"
SUBJECT_CHECKOUTS="CDE: Check-Outs"
SUBJECT_BUILD_COMPLETE="CDE: Build Complete"
SUBJECT_DATE='+%m/%d/%y'


##########################################################################
#
# Build Strings marking information extracted by the build_summary script.
#
BTAG_CMPL="BUILD COMPLETE"
BTAG_CFGS="BUILD CFG SPEC"
BTAG_DATE="BUILD DATE    "
BTAG_DFMT="+%a %h %d, %H:%M"
BTAG_LOGD="BUILD LOG DIR "
BTAG_PRJT="BUILD PROJECT "
BTAG_PTFM="BUILD PLATFORM"
BTAG_TYPE="BUILD TYPE    "
BTAG_VIEW="BUILD VIEW    "

BTAG_ENDD="BUILD ENDED   "
BTAG_LOGF="LOG FILE      "
BTAG_STRT="BUILD STARTED "
BTAG_SYST="SYSTEM        "

##########################################################################
#
# TOP of project development hierarchies
#
X_TOP=/proj/x11/xc
MOTIF_TOP=/proj/motif
CDE_TOP=/proj/cde
CDEDOC_TOP=/proj/cde
CDETEST_TOP=/proj/cde-test

##########################################################################
#
# Mail aliases for the project groups
#
TRW_MAIL_ALIAS="devtrw"
CDE_MAIL_ALIAS="devobj"
X_MAIL_ALIAS="devobj"
