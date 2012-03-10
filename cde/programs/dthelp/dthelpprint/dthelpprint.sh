#!/bin/sh
#######################################################
### File:		dthelpprint.sh
###
### Default Location:	/usr/dt/bin/dthelpprint.sh
###
### (c) Copyright 1993, 1994 Hewlett-Packard Company
### (c) Copyright 1993, 1994 International Business Machines Corp.
### (c) Copyright 1993, 1994 Sun Microsystems, Inc.
### (c) Copyright 1993, 1994 Novell, Inc.
###
### Purpose:
###
### The dthelpprint executable generates a fully formatted
### temporary file and then calls the dthelpprint.sh script 
### with options to actually print the file.
###
### Through the use of a shell script, dthelpprint
### does not need to link against libDtSvc and also
### supports better customization of help printing.
###
### Description:
###
### This script determines whether the Cde1 environment
### is installed and whether there is a Print action.
### If there is an action, use it to print the file.
### If there isn't and Cde1 is installed, try to use dtlp directly.
### If no Cde1 components are available, lp is used.  
###
### Product:		@(#)Cde1
###
### Invoked by:		dthelpprint (only!)
###
### Revision:		$XConsortium: dthelpprint.sh /main/3 1995/11/07 13:18:44 rswiston $
###
#########################################################

# set -x       # trace on

#########################################################
### Function: usage()
### Prints the usage message and exits
#########################################################
usage()
{
   echo "usage:  dthelpprint.sh options";
   echo "options:";
   echo "\t -d <lpdest>         : printer to use";
   echo "\t -f <print file>     : file to print";
   echo "\t -m <print command>  : print command to use";
   echo "\t -n <num copies>     : number copies to print";
   echo "\t -u <user file name> : filename to show user";
   echo "\t -w                  : print raw";
   echo "\t -s                  : print silent";
   echo "\t -e                  : remove file";
   exit 0;
}

#########################################################
### Main()
### set the env vars based on the arg list options
### These env vars conform to the dtlp interface
#########################################################
# define which executables to use
ProgDtKsh=/usr/dt/bin/dtksh
ProgDtAction=/usr/dt/bin/dtaction
ProgDtLp=/usr/dt/bin/dtlp          # only executed directly if Print action not avail
ProgLp=/usr/bin/lp
ActionPrint=Print

# init vars and consts
FlagActionOk=0;
True="True"
False="False"

# get the options
if [ $# -lt 2 ]; then usage; fi;
for argument in $*
do
   case $argument in
	-d)  LPDEST=$2; shift 2; export LPDEST;  ## Cde1 Print API
	    ;;
	-e) DTPRINTFILEREMOVE=$True; shift; export DTPRINTFILEREMOVE;  ## Cde1 Print API
	    ;;
	-s) DTPRINTSILENT=$True; shift; export DTPRINTSILENT;  ## Cde1 Print API
	    ;;
	-u) DTPRINTUSERFILENAME=$2; shift 2; export DTPRINTUSERFILENAME; ## Cde1 Print API
	    ;;
	-f) OptFile=$2; shift 2; export OptFile;  ## local variables
	    ;;
	-m) OptLpCommand=$2; shift 2; export OptLpCommand;  ## local variables
	    ;;
	-n) OptCopyCnt=$2; shift 2; export OptCopyCnt;  ## local variables
	    ;;
	-w) OptRaw=$True; shift; export OptRaw;  ## local variables
	    ;;
	--) shift; break;
	    ;;
	-\?) usage;
	    ;;
   esac
done

# can't print if no file is spec'd
if [ -z "$OptFile" -o ! -r "$OptFile" ];
then exit 1;
fi;

# is Cde1 installed and OptLpCommand not specified ?
if [ -x $ProgDtAction -a -x $ProgDtKsh -a -z "$OptLpCommand" ];
then 
   # exec a dtksh script to determine whether the print action exists
   $ProgDtKsh -c "XtInitialize TOPLEVEL chkPntr Dtksh;\
	      DtDbLoad;\
	      if DtActionExists $ActionPrint;\
	      then exit 1;\
	      else exit 0;\
	      fi;";
   # if action exists, use it; action removes the print file
   FlagActionOk=$?;
   if [ $FlagActionOk = 1 ];
   then
       # don't iterate wildly
       if [ -z "$OptCopyCnt" -o "$OptCopyCnt" -lt 0 -o "$OptCopyCnt" -gt "100" ]
       then  OptCopyCnt=1;
       fi;

       # honor copycount by looping
       # only set the DTPRINTFILEREMOVE for the last iteration
       VarOldFileRemove=$DTPRINTFILEREMOVE      # save orig value
       DTPRINTFILEREMOVE=$False                  # deactivate the remove request
       while [ "$OptCopyCnt" -gt "1" ]
       do
           $ProgDtAction $ActionPrint $OptFile;   # take other options from env vars
           OptCopyCnt=`expr "$OptCopyCnt" - 1`;
       done;
       DTPRINTFILEREMOVE=$VarOldFileRemove;      # restore orig value
       $ProgDtAction $ActionPrint $OptFile;  # take other options from env vars

       exit 0;
   # else if dtlp is installed, use it directly; it removes the print file
   elif [ -x $ProgDtLp ]
   then
        $ProgDtLp ${LPDEST:+-d} ${LPDEST:+$LPDEST} \
		${DTPRINTFILEREMOVE:+-e} \
		${DTPRINTSILENT:+-s} \
		-u "${DTPRINTUSERFILENAME:-Help Information}" \
		-b "Help" \
		${OptLpCommand:+-m} ${OptLpCommand:+$OptLpCommand} \
		-n ${OptCopyCnt:-1} \
		${OptRaw:+-w} \
                "$OptFile";

        exit 0;
   fi;           # use Print actio or ProgDtLp
fi;          # if Cde1 installed

# if Cde1 not installed or print action & dtlp not avail or OptLpCommand set
if [ -n "$FlagActionOk" -a "$FlagActionOk" = 0 -a -r "$OptFile" ];
then 
     ${OptLpCommand:-$ProgLp} -s -t "Help" \
				${LPDEST:+-d} ${LPDEST:+$LPDEST} \
				${OptRaw:+"-oraw"} \
				-n ${OptCopyCnt:-1} \
				$OptFile;
     if [ "$DTPRINTFILEREMOVE" = $True ]; then rm -f $OptFile; fi;
fi;

exit 0;
