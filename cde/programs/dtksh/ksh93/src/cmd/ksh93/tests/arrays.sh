# $XConsortium: arrays.sh /main/3 1995/11/01 16:54:14 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

function fun
{
	integer i
	unset xxx
	for i in 0 1
	do	xxx[$i]=$i
	done
}

Command=$0
integer Errors=0
set -A x zero one two three four 'five six'
if	[[ $x != zero ]]
then	err_exit '$x is not element 0'
fi
if	[[ ${x[0]} != zero ]]
then	err_exit '${x[0] is not element 0'
fi
if	(( ${#x[0]} != 4 ))
then	err_exit "length of ${x[0]} is not 4"
fi
if	(( ${#x[@]} != 6  ))
then	err_exit 'number of elements of x is not 6'
fi
if	[[ ${x[2]} != two  ]]
then	err_exit ' element two is not 2'
fi
if	[[ ${x[@]:2:1} != two  ]]
then	err_exit ' ${x[@]:2:1} is not two'
fi
set -A y -- ${x[*]}
if	[[ $y != zero ]]
then	err_exit '$x is not element 0'
fi
if	[[ ${y[0]} != zero ]]
then	err_exit '${y[0] is not element 0'
fi
if	(( ${#y[@]} != 7  ))
then	err_exit 'number of elements of y is not 7'
fi
if	[[ ${y[2]} != two  ]]
then	err_exit ' element two is not 2'
fi
set +A y nine ten
if	[[ ${y[2]} != two  ]]
then	err_exit ' element two is not 2'
fi
if	[[ ${y[0]} != nine ]]
then	err_exit '${y[0] is not nine'
fi
unset y[4]
if	(( ${#y[@]} != 6  ))
then	err_exit 'number of elements of y is not 6'
fi
if	(( ${#y[4]} != 0  ))
then	err_exit 'string length of unset element is not 0'
fi
unset foo
if	(( ${#foo[@]} != 0  ))
then	err_exit 'number of elements of unset variable foo is not 0'
fi
foo=''
if	(( ${#foo[0]} != 0  ))
then	err_exit 'string length of null element is not 0'
fi
if	(( ${#foo[@]} != 1  ))
then	err_exit 'number of elements of null variable foo is not 1'
fi
unset foo
foo[0]=foo
foo[3]=bar
unset foo[0]
unset foo[3]
if	(( ${#foo[@]} != 0  ))
then	err_exit 'number of elements of left in variable foo is not 0'
fi
unset foo
foo[3]=bar
foo[0]=foo
unset foo[3]
unset foo[0]
if	(( ${#foo[@]} != 0  ))
then	err_exit 'number of elements of left in variable foo again is not 0'
fi
fun
if	(( ${#xxx[@]} != 2  ))
then	err_exit 'number of elements of left in variable xxx is not 2'
fi
fun
if	(( ${#xxx[@]} != 2  ))
then	err_exit 'number of elements of left in variable xxx again is not 2'
fi
set -A foo -- "${x[@]}"
if	(( ${#foo[@]} != 6  ))
then	err_exit 'number of elements of foo is not 6'
fi
if	(( ${#PWD[@]} != 1  ))
then	err_exit 'number of elements of PWD is not 1'
fi
unset x
x[2]=foo x[4]=bar
if	(( ${#x[@]} != 2  ))
then	err_exit 'number of elements of x is not 2'
fi
s[1]=1 c[1]=foo
if	[[ ${c[s[1]]} != foo ]]
then	err_exit 'c[1]=foo s[1]=1; ${c[s[1]]} != foo'
fi
unset s
typeset -Ai s
y=* z=[
s[$y]=1
s[$z]=2
if	(( ${#s[@]} != 2  ))
then	err_exit 'number of elements of  is not 2'
fi
(( s[$z] = s[$z] + ${s[$y]} ))
if	[[ ${s[$z]} != 3  ]]
then	err_exit '[[ ${s[$z]} != 3  ]]'
fi
if	(( s[$z] != 3 ))
then	err_exit '(( s[$z] != 3 ))'
fi
(( s[$y] = s[$y] + ${s[$z]} ))
if	[[ ${s[$y]} != 4  ]]
then	err_exit '[[ ${s[$y]} != 4  ]]'
fi
if	(( s[$y] != 4 ))
then	err_exit '(( s[$y] != 4 ))'
fi
unset y
set -A y 2 4 6
typeset -i y
z=${y[@]}
typeset -R12 y
typeset -i y
if      [[ ${y[@]} != "$z" ]]
then    err_exit 'error in array conversion from int to R12'
fi
if      (( ${#y[@]} != 3  ))
then    err_exit 'error in count of array conversion from int to R12'
fi
unset abcdefg
:  ${abcdefg[1]}
set | grep '^abcdefg$' >/dev/null && err_exit 'empty array variable in set list'
unset x y
x=1
typeset -i y[$x]=4
if	[[ ${y[1]} != 4 ]]
then    err_exit 'arithmetic expressions in typeset not working'
fi
unset foo
typeset foo=bar
typeset -A foo
if	[[ ${foo[0]} != bar ]]
then	err_exit 'initial value not preserved when typecast to associative'
fi
unset foo
foo=(one two)
typeset -A foo
foo[two]=3
if	[[ ${#foo[*]} != 3 ]]
then	err_exit 'conversion of indexed to associative array failed'
fi
set a b c d e f g h i j k l m
if	[[ ${#} != 13 ]]
then	err_exit '${#} not 13'
fi
unset xxx
xxx=foo
if	[[ ${!xxx[@]} != 0 ]]
then	err_exit '${!xxx[@]} for scalar not 0'
fi
if	[[ ${11} != k ]]
then	err_exit '${11} not working'
fi
if	[[ ${@:4:1} != d ]]
then	err_exit '${@:4:1} not working'
fi
foovar1=abc
foovar2=def
if	[[ ${!foovar@} != +(foovar[[:alnum:]]?([ ])) ]]
then	err_exit '${!foovar@} does not expand correctly'
fi
if	[[ ${!foovar1} != foovar1 ]]
then	err_exit '${!foovar1} != foovar1'
fi
unset xxx
: ${xxx[3]}
if	[[ ${!xxx[@]} ]]
then	err_exit '${!xxx[@]} should be null'
fi
exit $((Errors))
