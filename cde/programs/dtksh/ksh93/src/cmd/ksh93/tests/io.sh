# $XConsortium: io.sh /main/3 1995/11/01 16:55:57 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}
Command=$0
integer Errors=0
# cut here
mkdir /tmp/ksh$$ || err_exit "mkdir /tmp/ksh$$ failed"
cd /tmp/ksh$$ || err_exit "cd /tmp/ksh$$ failed"
print foo > file1
print bar >> file1
if	[[ $(<file1) != $'foo\nbar' ]]
then	err_exit 'append (>>) not working'
fi
set -o noclobber
exec 3<> file1
read -u3 line
if	[[ $line != foo ]]
then	err_exit '<> not working right with read'
fi
if	( 4> file1 ) 2> /dev/null
then	err_exit 'noclobber not causing exclusive open'
fi
set +o noclobber
if	command exec 4< /dev/fd/3
then	read -u4 line
	if	[[ $line != bar ]]
	then	'4< /dev/fd/3 not working correctly'
	fi
fi
cat > close0 <<\!
exec 0<&-
echo $(./close1)
!
print "echo abc" > close1
chmod +x close0 close1
x=$(./close0)
if	[[ $x != "abc" ]]
then	err_exit "picked up file descriptor zero for opening script file"
fi
cd ~- || err_exit "cd back failed"
rm -r /tmp/ksh$$ || err_exit "rm -r /tmp/ksh$$ failed"
