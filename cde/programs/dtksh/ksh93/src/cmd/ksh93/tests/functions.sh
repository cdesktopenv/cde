# $XConsortium: functions.sh /main/3 1995/11/01 16:55:36 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

integer Errors=0
Command=$0
integer foo=33
bar=bye
# check for global variables and $0
function foobar
{
	case $1 in
	1) 	print -r - "$foo" "$bar";;
	2)	print -r - "$0";;
	3)	typeset foo=foo
		integer bar=10
	 	print -r - "$foo" "$bar";;
	4)	trap 'foo=36' EXIT
		typeset foo=20;;
	esac
}
function print
{
	command print hi
}
if	[[ $(print) != hi ]]
then	err_exit "command print not working inside print function"
fi
unset -f print

if	[[ $(foobar 1) != '33 bye' ]]
then	err_exit 'global variables not correct'
fi

if	[[ $(foobar 2) != 'foobar' ]]
then	err_exit '$0  not correct'
fi

if	[[ $(bar=foo foobar 1) != '33 foo' ]]
then	err_exit 'environment override not correct'
fi
if	[[ $bar = foo ]]
then	err_exit 'scoping error'
fi

if	[[ $(foobar 3) != 'foo 10' ]]
then	err_exit non-local variables
fi

foobar 4
if	[[ $foo != 36 ]]
then	err_exit EXIT trap in wrong scope
fi
unset -f foobar || err_exit "cannot unset function foobar"
typeset -f foobar>/dev/null  && err_exit "typeset -f has incorrect exit status"

function foobar
{
	(return 0)
}
> /tmp/shtests$$.1
{
foobar
if	[ -r /tmp/shtests$$.1 ]
then	rm -r /tmp/shtests$$.1
else	err_exit 'return within subshell inside function error'
fi
}
abc() print hi
if	[[ $(abc) != hi ]]
then	err_exit 'abc() print hi not working'
fi
(
	function f
	{
		exit 1
	}
	f
	err_exit 'exit from function not working'
)
unset -f foo
function foo
{
	x=2
	(
		x=3
		cd /tmp
		print bar
	)
	if	[[ $x != 2 ]]
	then	err_exit 'value of x not restored after subshell inside function'
	fi
}
x=1
dir=$PWD
if	[[ $(foo) != bar ]]
then	err_exit 'cd inside nested subshell not working'
fi
if	[[ $PWD != "$dir" ]]
then	err_exit 'cd inside nested subshell changes $PWD'
fi
fun() /bin/echo hello
if	[[ $(fun) != hello ]]
then	err_exit one line functions not working
fi
trap 'rm -f /tmp/script$$' EXIT
cat > /tmp/script$$ <<-\!
	print -r -- "$1"
!
chmod +x /tmp/script$$
function passargs
{
	/tmp/script$$ "$@"
}
if	[[ $(passargs one) != one ]]
then	err_exit 'passing args from functions to scripts not working'
fi
cat > /tmp/script$$ <<-\!
	trap 'exit 0' EXIT
	function foo
	{
		/tmp > /dev/null  2>&1
	}
	foo
!
if	! /tmp/script$$
then	err_exit 'exit trap incorrectly triggered' 
fi
if	! $SHELL -c /tmp/script$$
then	err_exit 'exit trap incorrectly triggered when invoked with -c' 
fi
exit $((Errors))
