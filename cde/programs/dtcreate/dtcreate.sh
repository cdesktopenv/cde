#!/bin/ksh

#######################################################################
#
#     dtcreate
#
#     The shell-script which implements dtcreate (a.k.a. CreateAction).
#
#     Hewlett-Packard Visual User Environment
#
#     Copyright (c) 1990 Hewlett-Packard Company
#
#     @(#) $XConsortium: dtcreate.sh /main/3 1995/11/01 16:13:20 rswiston $
#
######################################################################

#####################################################################
#
# determine_system - determines the system name and then sets
# the global variables _SIGUSR1 and _SIGUSR2 accordingly.
#
determine_system ()
{
        sys_name=`uname`

        case ${sys_name} in
                HP-UX)
                        _SIGUSR1=16
                        _SIGUSR2=17
                        ;;
                AIX)
                        _SIGUSR1=30
                        _SIGUSR2=31
                        ;;
                *)
                        _SIGUSR1=16
                        _SIGUSR2=17
                        ;;
        esac
}

#####################################################################
#
# getvalue accepts two arguments, the name of the resource whose
# value is desired and the name of the file where it is contained.
# The value of the resource is returned in $VALUE.
#
getvalue ()
{
        VALUE=`grep $1 $2 | sed -e "s/[^:]*: *//"`
}

enabledialog ()
{
        # Sending SIGUSR1 to the dtdialog process tells it to reenable
        # the dialog.
        kill -${_SIGUSR1} $1
}

toggleinvaliddialog ()
{
        # Sending SIGIUSR2 to the dtdialog process tells it to either
        # go to the invalid state (put up the invalid cursor) or go
        # back to its previous state (either active or busy).
        kill -${_SIGUSR2} $1
}

dismissdialog ()
{
        # Sending SIGINT to the dtdialog process tells it to remove the
        # dialog and die.
        kill -2 $1
}

##############################################################
#
# checkprocess accepts a single argument with is a process-id.
# If the specified process is running (as reported by ps -ef),
# checkprocess returns 1.  If it isn't running, checkprocess
# returns 0.
#
checkprocess ()
{
        RUNNING=`ps -ef | cut -c 9-14 | grep -c $1`

        if [ "$RUNNING" = "0" ] ; then
                return 0
        else
                return 1
        fi
}

# This is the main callback that is called whenever the user presses
# one of the buttons on the CreateAction dialog.
actioncallback ()
{
        # Check which button on the dialog was invoked.  The Help button
        # is automatically handled by dtdialog, so it is only the apply
        # and close buttons that we care about.
        getvalue selectedButton $DIALOG_OUTPUT

        if [ "$VALUE" = "apply" ] ; then
                checkactionvalues
        else
                cleanupandexit
        fi
}

checkactionvalues ()
{
        # Read all the values in from the dialog.
        getvalue windowId $DIALOG_OUTPUT
        CREATEACTION_WID=$VALUE

        getvalue name.text $DIALOG_OUTPUT
        ACTION=$VALUE

        getvalue largeIcon.text $DIALOG_OUTPUT
        LICON=$VALUE

        getvalue smallIcon.text $DIALOG_OUTPUT
        SICON=$VALUE

        getvalue description.text $DIALOG_OUTPUT
        DESCRIPTION=`echo $VALUE | tr -s "\012" " "`

        getvalue commandLine.text $DIALOG_OUTPUT
        EXECSTRING=$VALUE

        getvalue prompt.text $DIALOG_OUTPUT
        PROMPT=$VALUE

        getvalue commandType.active $DIALOG_OUTPUT
        if [ "$VALUE" = "xWin" -o "$VALUE" = "noOut" ] ; then
                WINDOWTYPE="NO_STDIO"
        elif [ "$VALUE" = "term" ] ; then
                WINDOWTYPE="PERM_TERMINAL"
        else
                WINDOWTYPE="TERMINAL"
        fi

        BAIL_OUT=""
        # Make sure that an action name and an exec-string were provided.
        if [ ! "$BAIL_OUT" -a ! "$ACTION" -a ! "$EXECSTRING" ] ; then
                dtdialog -descFile dtcreate.ddf -dialogName noNameOrExec \
                        -transientFor $CREATEACTION_WID &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                enabledialog $CREATEACTION_PID
                BAIL_OUT="yes"
        fi

        if [ ! "$BAIL_OUT" -a ! "$ACTION" ] ; then
                dtdialog -descFile dtcreate.ddf -dialogName noName \
                        -transientFor $CREATEACTION_WID &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                enabledialog $CREATEACTION_PID
                BAIL_OUT="yes"
        fi

        if [ ! "$BAIL_OUT" -a ! "$EXECSTRING" ] ; then
                dtdialog -descFile dtcreate.ddf -dialogName noExecString \
                        -transientFor $CREATEACTION_WID &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                enabledialog $CREATEACTION_PID
                BAIL_OUT="yes"
        fi

        # Make sure the action name does not contain any blanks.
        if [ ! "$BAIL_OUT" -a `echo $ACTION | grep -c ' '` -eq 1 ] ; then
                dtdialog -descFile dtcreate.ddf -dialogName foundBlank \
                        -transientFor $CREATEACTION_WID &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                enabledialog $CREATEACTION_PID
                BAIL_OUT="yes"
        fi

        if [ ! "$BAIL_OUT" ] ; then
                # Check the vf file we want to write to.  If it exists and
                # isn't writable, complain.
                VF_DIR=$HOME/.dt/types
                # Hack warning! Do temporary dt stuff.
                if [[ $doDT = 1 ]] then
                  VF_FILE=$VF_DIR/$ACTION.dt
                else
                  VF_FILE=$VF_DIR/$ACTION.vf
                fi
                if [ -f $VF_FILE -a ! -w $VF_FILE ] ; then
                        dtdialog -descFile dtcreate.ddf \
                                -dialogName notWritable \
                                -transientFor $CREATEACTION_WID \
                                $VF_FILE &
                        SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                        let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                        enabledialog $CREATEACTION_PID
                        BAIL_OUT="yes"
                fi
        fi

        # Check if the action name is longer than 11 characters.
        # If so, warn about the problems with short filename
        # systems and give the user a chance to change it.  This is a
        # modal dialog (not run in background) because the user has to
        # make a choice.
        if [ ! "$BAIL_OUT" -a `echo $ACTION | wc -c` -gt 11 ] ; then
                toggleinvaliddialog $CREATEACTION_PID
                dtdialog -descFile dtcreate.ddf \
                        -dialogName longName \
                        -transientFor $CREATEACTION_WID $ACTION

                if [ $? -eq 2 ] ; then
                        enabledialog $CREATEACTION_PID
                        BAIL_OUT="yes"
                else
                        toggleinvaliddialog $CREATEACTION_PID
                fi
        fi

        # Check that if the exec-string uses any shell features (e.g. pipes),
        # that a shell is explicitly specified.
        if [ ! "$BAIL_OUT" -a `echo $EXECSTRING | grep -c '[|&;<>]'` -eq 1 -a\
             ! `echo $EXECSTRING | grep -c 'sh '` -eq 1 ] ; \
           then
                dtdialog -descFile dtcreate.ddf -dialogName needShell \
                        -transientFor $CREATEACTION_WID &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                enabledialog $CREATEACTION_PID
                BAIL_OUT="yes"
        fi

        # Check that the prompt string does not contain quotes.  I
        # think there is a bug here that a single backquote (`) is not
        # detected.  I need to figure out the correct grep expression
        # to catch it.
        QUOTES=`echo \'$PROMPT\' | tr '!$%&(-\\\/"' '%%%%%%%%%' | fgrep -c %`
        if [ ! "$BAIL_OUT" -a $QUOTES -eq 1 ] ; then
                dtdialog -descFile dtcreate.ddf \
                        -transientFor $CREATEACTION_WID \
                        -dialogName promptQuote &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                enabledialog $CREATEACTION_PID
                BAIL_OUT="yes"
        fi

        # Check that if a prompt is supplied, that the action calls for
        # arguments.
        if [ ! "$BAIL_OUT" ] ; then
                if [ "$PROMPT" -a \
                    `echo $EXECSTRING | grep -c "\$[*0-9]"` -eq 0 ] ; then
                        dtdialog -descFile dtcreate.ddf \
                                -transientFor $CREATEACTION_WID \
                                -dialogName unusedPrompt &
                        SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                        let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
                        enabledialog $CREATEACTION_PID
                        BAIL_OUT="yes"
                fi
        fi

        # If no errors were caught, create the action and reenable the
        # dialog.
        if [ ! "$BAIL_OUT" ] ; then
                createaction
                enabledialog $CREATEACTION_PID
        fi
}

createaction ()
{
        # If the vf file already exists, we are going to copy it to a temp
        # file and comment out the definition for the current action.
        # Make sure that this temp file doesn't already exist so that an old
        # one doesn't get used by mistake.
        OLD_FILE=/tmp/`basename $0`2_$$
        rm -f ${OLD_FILE}
        if [ -w $VF_FILE ] ; then
                sed -e "/^ACTION ${ACTION}[     ]*$/,/^[END|}]/s/^/\# /" \
                        ${VF_FILE} > ${OLD_FILE}
        fi

        # Write out the new action definition.
        echo "\n#\n# Action created by dtcreate\n#" > $VF_FILE
        echo "ACTION $ACTION" > $VF_FILE
        echo "{" >> $VF_FILE

        if [ "${LICON}" ] ; then
                echo "\tLARGE_ICON\t\t$LICON" >> $VF_FILE
        fi

        if [ "${SICON}" ] ; then
                echo "\tSMALL_ICON\t\t$SICON" >> $VF_FILE
        fi

        if [ "${DESCRIPTION}" ] ; then
                echo "\tDESCRIPTION\t$DESCRIPTION" >> $VF_FILE
        fi

        echo "\tTYPE\t\tCOMMAND" >>$VF_FILE

        if [ "${EXECSTRING}" ] ; then
                # Process the exec-string and look for $*, $1, $2, etc.
                # These must be translated into DT's syntax of
                # %(File)Arg_n%.  Also insert the prompt string if one
                # was supplied.
                echo "\tEXEC_STRING\t\c" >>$VF_FILE
                if [ "$PROMPT" ] ; then
                        echo $EXECSTRING | sed \
                          -e "s/\$\*/%(File)Arg_1\"$PROMPT\"% %(File)Args%/" \
                          -e "s/\$\([1-9]\)/%(File)Arg_\1\"$PROMPT\"%/" \
                          -e "s/\$\([1-9]\)/%(File)Arg_\1%/g" \
                                >> $VF_FILE
                else
                        echo $EXECSTRING | sed \
                                -e "s/\$\*/%(File)Args%/g" \
                                -e "s/\$\([1-9]\)/%(File)Arg_\1%/g" >> $VF_FILE
                fi
        fi

        if [ "${WINDOWTYPE}" ] ; then
                echo "\tWINDOW_TYPE\t$WINDOWTYPE" >> $VF_FILE
        fi

        echo "}" >>$VF_FILE

        # If the temp file with the old contents of the vf file exists,
        # stick it on the end of the file we just created.
        if [ -r ${OLD_FILE} ] ; then
                echo "\n#\n# Commented out by dtcreate\n#" >> $VF_FILE
                cat ${OLD_FILE} >> ${VF_FILE}
                rm ${OLD_FILE}
        fi

        #
        # Invoke ReloadActions so that the new action
        # will be read.
        dtaction -timeout 1 ReloadActions

        #
        # Create the action copy file in the home directory.
        if [ -w ${TOOLBOX} -a ! -f ${TOOLBOX}/${ACTION} ] ; then
                touch ${TOOLBOX}/${ACTION}
                chmod 755 ${TOOLBOX}/${ACTION}
        fi

        # Check whether the action exists in the Home directory.  (Either we
        # just created it or it may have already existed.)
        if [ -x ${TOOLBOX}/${ACTION} ] ; then
                dtdialog -descFile dtcreate.ddf -dialogName actionExists \
                        -transientFor $CREATEACTION_WID \
                        $ACTION $VF_FILE &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
        else
                dtdialog -descFile dtcreate.ddf \
                        -dialogName actionDoesntExist \
                        -transientFor $CREATEACTION_WID \
                        $ACTION $VF_FILE &
                SECONDARY_DIALOG_PIDS[${NUM_SECONDARY_DIALOGS}]=$!
                let NUM_SECONDARY_DIALOGS=${NUM_SECONDARY_DIALOGS}+1
        fi
}

cleanupandexit()
{
        # Dismiss any secondary dialogs (e.g. Help, warnings, errors)
        # that have been created.
        if [ NUM_SECONDARY_DIALOGS -gt 0 ]; then
                for PID in ${SECONDARY_DIALOG_PIDS[*]}
                do
                        checkprocess $PID
                        if [ $? -eq 1 ] ; then
                                dismissdialog $PID
                        fi
                done
        fi

        dismissdialog $CREATEACTION_PID

        rm -f ${DIALOG_OUTPUT}
}


#####################################################################
# Main
#
#set -xv
DIALOG_OUTPUT=/tmp/`basename $0`$$
#TOOLBOX=$HOME/.dt/types/tools
TOOLBOX=$HOME
NUM_SECONDARY_DIALOGS=0

determine_system

# Hack warning:  This is to determine if should create .dt file or .vf file.
if [[ -f /opt/dt/types/useFT ]] then
  doDT=0
else
  doDT=1
fi

# Set up the callback function that is invoked when the user
# presses any of the dialog buttons.
trap "actioncallback" ${_SIGUSR1}

# Invoke dtdialog and pass in the process-ID of this script.
# When the user presses any of the dialog buttons, dtdialog
# will send us a signal which will cause the callback function
# to be called.
PARENT_PID=$$
dtdialog -descFile dtcreate.ddf -dialogName createAction -returnWinId \
        -signal SIGUSR1 -ppid $PARENT_PID -outFile $DIALOG_OUTPUT &

# Keep track of the process-ID of the dtdialog process that is
# managing the main CreateAction dialog.  We need this process-ID so
# that we can send it signals to do things such as reenable it.
CREATEACTION_PID=$!

# Sit in a loop as long as the main CreateAction dialog is up.  The
# signals cause the 'wait' call to exit and I don't know of any way to
# determine whether the wait existed due to a signal or because the
# process died.  Therefore we have to explicitly check if the process
# is alive.
EXIT=0
while [ $EXIT -eq 0 ] ; do
        wait $CREATEACTION_PID
        checkprocess $CREATEACTION_PID
        if [ $? -eq 0 ] ; then
                EXIT=1
        fi
done

exit 0


