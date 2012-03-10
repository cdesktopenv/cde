# $XConsortium: coprocess.sh /main/3 1995/11/01 16:55:15 rswiston $
# test the behavior of co-processes
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

Command=$0
integer Errors=0

function ping # id
{
	integer x=0
	while ((x < 5))
	do	read -r
		print -r "$1 $REPLY"
	done
}

ping three |&
exec 3>&p
ping four |&
exec 4>&p
ping pipe |&

integer count
for i in three four pipe four pipe four three pipe pipe three pipe
do	case $i in
	three)	to=-u3;;
	four)	to=-u4;;
	pipe)	to=-p;;
	esac
	count=count+1
	print  $to $i $count
done

while	((count > 0))
do	count=count-1
	read -p
#	print -r - "$REPLY"
	set -- $REPLY
	if	[[ $1 != $2 ]]
	then	err_exit "$1 does not match 2"
	fi
	case $1 in
	three);;
	four) ;;
	pipe) ;;
	*)	err_exit "unknown message +|$REPLY|+"
	esac
done

file=/tmp/regress$$
trap "rm -f $file" EXIT
cat > $file  <<\!
/bin/cat |&
!
chmod +x $file
$file 2> /dev/null  || err_exit "parent coprocess prevents script coprocess"
exec 5<&p 6>&p
exec 5<&- 6>&-
${SHELL-ksh} |&
print -p  $'print hello | cat\nprint Done'
read -t 5 -p
read -t 5 -p
if	[[ $REPLY != Done ]]
then	err_exit	"${SHELL-ksh} coprocess not working"
fi
exec 5<&p 6>&p
exec 5<&- 6>&-
count=0
{
echo line1 | grep 'line2'
echo line2 | grep 'line1'
} |&
SECONDS=0
while
   read -p -t 10 line
do
   ((count = count + 1))
   echo "Line $count: $line"
done
if	(( SECONDS > 8 ))
then	err_exit 'read -p hanging'
fi
exit $((Errors))
