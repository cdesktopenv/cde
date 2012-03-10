# $XConsortium: comvar.sh /main/3 1996/05/10 16:42:24 drk $

function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}
# test for compound variables
Command=$0
integer Errors=0
Point=(
	float x=1. y=0.
)
eval p="$Point"
if	(( (p.x*p.x + p.y*p.y) > 1.01 ))
then	err_exit 'compound variable not working'
fi
nameref foo=p
if	[[ ${foo.x} != ${Point.x} ]]
then	err_exit 'reference to compound object not working'
fi
unset foo
rec=(
	name='Joe Blow'
	born=(
		month=jan
		integer day=16
		year=1980
	)
)
eval newrec="$rec"
if	[[ ${newrec.name} != "${rec.name}" ]]
then	err_exit 'copying a compound object not working'
fi
if	(( newrec.born.day != 16 ))
then	err_exit 'copying integer field of  compound object not working'
fi
exit $((Errors))
