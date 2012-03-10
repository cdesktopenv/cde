# $XConsortium: basic.sh /main/3 1995/11/01 16:54:32 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

# test basic file operations like redirection, pipes, file expansion
Command=$0
integer Errors=0
umask u=rwx,go=rx || err_exit "umask u=rws,go=rx failed"
mkdir  /tmp/ksh$$ || err_exit "mkdir /tmp/ksh$$ failed" 
trap "rm -rf /tmp/ksh$$" EXIT
pwd=$PWD
cd /tmp/ksh$$ || err_exit "cd /tmp/ksh$$ failed"
mkdir dir
if	[[ $(print */) != dir/ ]]
then	err_exit 'file expansion with trailing / not working'
fi
date > dat1 || err_exit "date > dat1 failed"
test -r dat1 || err_exit "dat1 is not readable"
x=dat1
cat <$x > dat2 || err_exit "cat < $x > dat2 failed"
cat dat1 dat2 | cat  | cat | cat > dat3 || err_exit "cat pipe failed"
cat > dat4 <<!
$(date)
!
cat dat1 dat2 | cat  | cat | cat > dat5 &
wait $!
set -- dat*
if	(( $# != 5 ))
then	err_exit "dat* matches only $# files"
fi
> foo\\abc
set -- foo*
if	[[ $1 != 'foo\abc' ]]
then	err_exit 'foo* does not match foo\abc'
fi
cd ~- || err_exit "cd back failed"
rm -r /tmp/ksh$$ || err_exit "rm -r /tmp/ksh$$ failed"
bar=foo
eval foo=\$bar
if	[[ $foo != foo ]]
then	err_exit 'eval foo=\$bar not working'
fi
bar='foo=foo\ bar'
eval $bar
if	[[ $foo != 'foo bar' ]]
then	err_exit 'eval foo=\$bar, with bar="foo\ bar" not working'
fi
cd /tmp
cd ../../dev || err_exit "cd ../../dev failed"
#if	[[ $PWD != /dev ]]
#then	err_exit 'cd ../../dev is not /dev'
#fi
( sleep 2; cat <<!
foobar
!
) | cat > /tmp/foobar$$ &
wait $!
foobar=$( < /tmp/foobar$$) 
if	[[ $foobar != foobar ]]
then	err_exit "$foobar is not foobar"
fi
{
	print foo
	/bin/echo bar
	print bam
} > /tmp/foobar$$
if	[[ $( < /tmp/foobar$$) != $'foo\nbar\nbam' ]]
then	err_exit "Output file pointer not shared correctly."
fi
cat > /tmp/foobar$$ <<\!
	print foo
	/bin/echo bar
	print bam
!
chmod +x /tmp/foobar$$
if	[[ $(/tmp/foobar$$) != $'foo\nbar\nbam' ]]
then	err_exit "Script not working."
fi
if	[[ $(/tmp/foobar$$ | /bin/cat) != $'foo\nbar\nbam' ]]
then	err_exit "Script | cat not working."
fi
if	[[ $( /tmp/foobar$$) != $'foo\nbar\nbam' ]]
then	err_exit "Output file pointer not shared correctly."
fi
rm -f /tmp/foobar$$
x=$( (print foo) ; (print bar) )
if	[[ $x != $'foo\nbar' ]]
then	err_exit " ( (print foo);(print bar ) failed"
fi
x=$( (/bin/echo foo) ; (print bar) )
if	[[ $x != $'foo\nbar' ]]
then	err_exit " ( (/bin/echo);(print bar ) failed"
fi
x=$( (/bin/echo foo) ; (/bin/echo bar) )
if	[[ $x != $'foo\nbar' ]]
then	err_exit " ( (/bin/echo);(/bin/echo bar ) failed"
fi
cat > /tmp/ksh$$ <<\!
builtin cat
cat - > /dev/null
test -p /dev/fd/0 && print yes
!
chmod +x /tmp/ksh$$
if	[[ $( (print) | /tmp/ksh$$;:) != yes ]]
then	err_exit "standard input no longer a pipe"
fi
print 'print $0' > /tmp/ksh$$
print ". /tmp/ksh$$" > /tmp/ksh$$x
chmod +x /tmp/ksh$$x
if	[[ $(/tmp/ksh$$x) != /tmp/ksh$$x ]]
then	err_exit '$0 not correct for . script'
fi
rm -r /tmp/ksh$$ /tmp/ksh$$x
mkdir  /tmp/ksh$$ || err_exit "mkdir /tmp/ksh$$ failed" 
cd /tmp/ksh$$ || err_exit "cd /tmp/ksh$$ failed"
print ./b > ./a; print ./c > b; print ./d > c; print ./e > d; print "echo \"hello there\"" > e 
chmod 755 a b c d e
x=$(./a)
if	[[ $x != "hello there" ]]
then	err_exit "nested scripts failed" 
fi
x=$( (./a) | cat)
if	[[ $x != "hello there" ]]
then	err_exit "scripts in subshells fail" 
fi
cd ~- || err_exit "cd back failed"
rm -r /tmp/ksh$$ || err_exit "rm -r /tmp/ksh$$ failed"
x=$( (/bin/echo foo) 2> /dev/null )
if	[[ $x != foo ]]
then	err_exit "subshell in command substitution fails"
fi
exec 1>&-
x=$(print hello)
if	[[ $x != hello ]]
then	err_exit "command subsitution with stdout closed failed"
fi
cd $pwd
x=$(cat <<\! | $SHELL
/bin/echo | /bin/cat
/bin/echo hello
!
)
if	[[ $x != $'\n'hello ]]
then	err_exit "$SHELL not working when standard input is a pipe"
fi
x=$( (/bin/echo hello) 2> /dev/null )
if	[[ $x != hello ]]
then	err_exit "subshell in command substitution with 1 closed fails"
fi
cat > /tmp/ksh$$ <<- \!
read line 2> /dev/null
print done
!
if	[[ $($SHELL /tmp/ksh$$ <&-) != done ]]
then	err_exit "executing script with 0 closed fails"
fi
trap '' INT
cat > /tmp/ksh$$ <<- \!
trap 'print bad' INT
kill -s INT $$
print good
!
chmod +x /tmp/ksh$$
if	[[ $($SHELL  /tmp/ksh$$) != good ]]
then	err_exit "traps ignored by parent not ignored"
fi
trap - INT
cat > /tmp/ksh$$ <<- \!
read line
/bin/cat
!
if	[[ $($SHELL /tmp/ksh$$ <<!
one
two
!
)	!= two ]]
then	err_exit "standard input not positioned correctly"
fi
exit $((Errors))
