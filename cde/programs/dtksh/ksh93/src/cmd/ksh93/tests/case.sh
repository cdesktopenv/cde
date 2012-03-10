# $XConsortium: case.sh /main/2 1995/07/19 17:10:58 drk $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

Command=$0
integer Errors=0

bar=foo2
bam=foo[3]
for i in foo1 foo2 foo3 foo4 foo5 foo6
do	foo=0
	case $i in
	foo1)	foo=1;;
	$bar)	foo=2;;
	$bam)	foo=3;;
	foo[4])	foo=4;;
	${bar%?}5)
		foo=5;;
	"${bar%?}6")
		foo=6;;
	esac
	if	[[ $i != foo$foo ]]
	then	err_exit "$i not matching correct pattern"
	fi
done
f="[ksh92]"
case $f in
\[*\])  ;;
*)      err_exit "$f does not match \[*\]";;
esac
exit $((Errors))
