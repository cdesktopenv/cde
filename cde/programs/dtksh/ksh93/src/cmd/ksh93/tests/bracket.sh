# $XConsortium: bracket.sh /main/3 1995/11/01 16:54:43 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

Command=$0
integer Errors=0
null=''
if	[[ ! -z $null ]]
then	err_exit "-z: null string should be of zero length"
fi
file=/tmp/regress$$
if	[[ -z $file ]]
then	err_exit "-z: $file string should not be of zero length"
fi
trap "rm -f $file" EXIT
rm -f $file
if	[[ -a $file ]]
then	err_exit "-a: $file shouldn't exist"
fi
> $file
if	[[ ! -a $file ]]
then	err_exit "-a: $file should exist"
fi
chmod 777 $file
if	[[ ! -r $file ]]
then	err_exit "-r: $file should be readable"
fi
if	[[ ! -w $file ]]
then	err_exit "-w: $file should be writable"
fi
if	[[ ! -w $file ]]
then	err_exit "-x: $file should be executable"
fi
if	[[ ! -w $file || ! -r $file ]]
then	err_exit "-rw: $file should be readable/writable"
fi
if	[[ -s $file ]]
then	err_exit "-s: $file should be of zero size"
fi
if	[[ ! -f $file ]]
then	err_exit "-f: $file should be an ordinary file"
fi
if	[[  -d $file ]]
then	err_exit "-f: $file should not be a directory file"
fi
if	[[  ! -d . ]]
then	err_exit "-d: . should not be a directory file"
fi
if	[[  -f /dev/null ]]
then	err_exit "-f: /dev/null  should not be an ordinary file"
fi
chmod 000 $file
if	[[ -r $file ]]
then	err_exit "-r: $file should not be readable"
fi
if	[[ ! -O $file ]]
then	err_exit "-r: $file should be owned by me"
fi
if	[[ -w $file ]]
then	err_exit "-w: $file should not be writable"
fi
if	[[ -w $file ]]
then	err_exit "-x: $file should not be executable"
fi
if	[[ -w $file || -r $file ]]
then	err_exit "-rw: $file should not be readable/writable"
fi
if	[[   -z x &&  -z x || ! -z x ]]
then	:
else	err_exit " wrong precedence"
fi
if	[[   -z x &&  (-z x || ! -z x) ]]
then	err_exit " () grouping not working"
fi
if	[[ foo < bar ]]
then	err_exit "foo comes before bar"
fi
[[ . -ef $(pwd) ]] || err_exit ". is not $PWD"
set -o allexport
[[ -o allexport ]] || err_exit '-o: did not set allexport option'
if	[[ -n  $null ]]
then	err_exit "'$null' has non-zero length"
fi
if	[[ ! -r /dev/fd/0 ]]
then	err_exit "/dev/fd/0 not open for reading"
fi
if	[[ ! -w /dev/fd/2 ]]
then	err_exit "/dev/fd/2 not open for writing"
fi
if	[[ ! . -ot $file ]]
then	err_exit ". should be older than $file"
fi
if	[[ /bin -nt $file ]]
then	err_exit "$file should be newer than /tmp"
fi
if	[[ $file != /tmp/* ]]
then	err_exit "$file should match /tmp/*"
fi
if	[[ $file = '/tmp/*' ]]
then	err_exit "$file should not equal /tmp/*"
fi
[[ ! ( ! -z $null && ! -z x) ]]	|| err_exit "negation and grouping"
[[ -z '' || -z '' || -z '' ]]	|| err_exit "three ors not working"
[[ -z '' &&  -z '' && -z '' ]]	|| err_exit "three ors not working"
(exit 8)
if	[[ $? -ne 8 || $? -ne 8 ]]
then	err_exit 'value $? within [[...]]'
fi
x='(x'
if	[[ '(x' != '('* ]]
then	err_exit " '(x' does not match '('* within [[...]]"
fi
if	[[ '(x' != "("* ]]
then	err_exit ' "(x" does not match "("* within [[...]]'
fi
if	[[ '(x' != \(* ]]
then	err_exit ' "(x" does not match \(* within [[...]]'
fi
if	[[ 'x(' != *'(' ]]
then	err_exit " 'x(' does not match '('* within [[...]]"
fi
if	[[ 'x&' != *'&' ]]
then	err_exit " 'x&' does not match '&'* within [[...]]"
fi
if	[[ 'xy' = *'*' ]]
then	err_exit " 'xy' matches *'*' within [[...]]"
fi
if	[[ 3 > 4 ]]
then	err_exit '3 < 4'
fi
if	[[ 4 < 3 ]]
then	err_exit '3 > 4'
fi
if	[[ 3x > 4x ]]
then	err_exit '3x < 4x'
fi
x='bin|dev|?'
cd /
if	[[ $(print $x) != "$x" ]]
then	err_exit 'extended pattern matching on command arguments'
fi
if	[[ dev != $x ]]
then	err_exit 'extended pattern matching not working on variables'
fi
if	[[ -u $SHELL ]]
then	err_exit "setuid on $SHELL"
fi
if	[[ -g $SHELL ]]
then	err_exit "setuid on $SHELL"
fi
test -d .  -a '(' ! -f . ')' || err_exit 'test not working'
if	[[ '!' != ! ]]
then	err_exit 'quoting unary operator not working'
fi
exit $((Errors))
