# $XConsortium: nameref.sh /main/2 1995/07/19 17:11:56 drk $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

Command=$0
integer Errors=0
function checkref
{
	nameref foo=$1 bar=$2
	if	[[ $foo !=  $bar ]]
	then	err_exit "foo=$foo != bar=$bar"
	fi
	foo=hello
	if	[[ $foo !=  $bar ]]
	then	err_exit "foo=$foo != bar=$bar"
	fi
	foo.child=child
	if	[[ ${foo.child} !=  ${bar.child} ]]
	then	err_exit "foo.child=${foo.child} != bar=${bar.child}"
	fi
}

name=first
checkref name name
name.child=second
checkref name name
.foo=top
.foo.bar=next
checkref .foo.bar .foo.bar
if	[[ ${.foo.bar} !=  hello ]]
then	err_exit ".foo.bar=${.foo.bar} != hello"
fi
if	[[ ${.foo.bar.child} !=  child ]]
then	err_exit ".foo.bar.child=${.foo.bar.child} != child"
fi
function func1
{
        nameref color=$1
        func2 color
}

function func2
{
        nameref color=$1
        set -s -- ${!color[@]}
	print -r -- "$@"
}

typeset -A color
color[apple]=red
color[grape]=purple
color[banana]=yellow
if	[[ $(func1 color) != 'apple banana grape' ]]
then	err_exit "nameref or nameref not working"
fi
nameref x=.foo.bar
if	[[ ${!x} != .foo.bar ]]
then	err_exit "${!x} not working"
fi
typeset +n x
unset x
nameref x=.foo.bar
function x.set
{
	[[ ${.sh.value} ]] && print hello
}
if	[[ $(.foo.bar.set) != $(x.set) ]]
then	err_exit "function references  not working"
fi
if	[[ $(typeset +n) != x ]]
then	err_exit "typeset +n doesn't list names of reference variables"
fi
if	[[ $(typeset -n) != x=.foo.bar ]]
then	err_exit "typeset +n doesn't list values of reference variables"
fi
file=/tmp/shtest$$
typeset +n foo bar 2> /dev/null
unset foo bar
export bar=foo
nameref foo=bar
if	[[ $foo != foo ]]
then	err_exit "value of nameref foo !=  $foo"
fi
trap "rm -f $file" EXIT INT
cat > $file <<\!
print -r -- $foo
!
chmod +x "$file"
y=$( $file)
if	[[ $y != '' ]]
then	err_exit "reference variable not cleared"
fi
{ 
	command nameref xx=yy
	command nameref yy=xx
} 2> /dev/null && err_exit "self reference not detected"
exit $((Errors))
