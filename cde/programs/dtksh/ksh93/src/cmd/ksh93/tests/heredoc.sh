# $XConsortium: heredoc.sh /main/3 1995/11/01 16:55:47 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

Command=$0
integer Errors=0
f=/tmp/here1$$
g=/tmp/here2$$
trap "rm -f $f $g" EXIT
cat > $f <<!
hello world
!
if	[[ $(<$f) != 'hello world' ]]
then	err_exit "'hello world' here doc not working"
fi
cat > $g <<\!
hello world
!
cmp $f $g 2> /dev/null || err_exit "'hello world' quoted here doc not working"
cat > $g <<- !
	hello world
!
cmp $f $g 2> /dev/null || err_exit "'hello world' tabbed here doc not working"
cat > $g <<- \!
	hello world
!
cmp $f $g 2> /dev/null || err_exit "'hello world' quoted tabbed here doc not working"
x=hello
cat > $g <<!
$x world
!
cmp $f $g 2> /dev/null || err_exit "'$x world' here doc not working"
cat > $g <<!
$(print hello) world
!
cmp $f $g 2> /dev/null || err_exit "'$(print hello) world' here doc not working"
cat > $f <<\!!
!@#$%%^^&*()_+~"::~;'`<>?/.,{}[]
!!
if	[[ $(<$f) != '!@#$%%^^&*()_+~"::~;'\''`<>?/.,{}[]' ]]
then	err_exit "'hello world' here doc not working"
fi
cat > $g <<!!
!@#\$%%^^&*()_+~"::~;'\`<>?/.,{}[]
!!
cmp $f $g 2> /dev/null || err_exit "unquoted here doc not working"
exec 3<<!
	foo
!
if	[[ $(<&3) != '	foo' ]]
then	err_exit "leading tabs stripped with <<!"
fi
$SHELL -c "
eval `echo 'cat <<x'` "|| err_exit "eval `echo 'cat <<x'` core dumps"
cat > /dev/null <<EOF # comments should not cause core dumps
abc
EOF
cat >$g << :
:
:
cmp /dev/null $g 2> /dev/null || err_exit "empty here doc not working"
x=$(print $( cat <<HUP
hello
HUP
)
)
if	[[ $x != hello ]]
then	err_exit "here doc inside command sub not working"
fi
y=$(cat <<!
${x:+${x}}
!
)
if	[[ $y != "${x:+${x}}" ]]
then	err_exit '${x:+${x}} not working in here document'
fi
exit $((Errors))
