# $XConsortium: exit.sh /main/2 1996/05/10 16:42:35 drk $

function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}
function abspath
{
        base=$(basename $SHELL)
        cd ${SHELL%/$base}
        newdir=$(pwd)
        cd ~-
        print $newdir/$base
}
#test for proper exit of shell
Command=$0
integer Errors=0
ABSHELL=$(abspath)
mkdir /tmp/ksh$$ || err_exit "mkdir /tmp/ksh$$ failed"
cd /tmp/ksh$$ || err_exit "cd /tmp/ksh$$ failed"
print exit >.profile
${ABSHELL}  <<!
exec -c -a -ksh ${ABSHELL} -c "exit 1" 1>/dev/null 2>&1
!
if [[ $(echo $?) != 0 ]]
then err_exit 'exit in .profile is ignored'
fi
cd ~- || err_exit "cd back failed"
rm -r /tmp/ksh$$ || err_exit "rm -r /tmp/ksh$$ failed"
exit $((Errors))
