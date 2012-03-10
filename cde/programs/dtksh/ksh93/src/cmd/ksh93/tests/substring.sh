# $XConsortium: substring.sh /main/3 1995/11/01 16:56:33 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

Command=$0
integer Errors=0 j=4
base=/home/dgk/foo//bar
string1=$base/abcabcabc
if	[[ ${string1:0} != "$string1" ]]
then	err_exit "string1:0"
fi
if	[[ ${string1:0:1000} != "$string1" ]]
then	err_exit "string1:0"
fi
if	[[ ${string1:1} != "${string1#?}" ]]
then	err_exit "string1:1"
fi
if	[[ ${string1:1:4} != home ]]
then	err_exit "string1:1:4"
fi
if	[[ ${string1:1:j} != home ]]
then	err_exit "string1:1:j"
fi
if	[[ ${string1:(j?1:0):j} != home ]]
then	err_exit "string1:(j?1:0):j"
fi
if	[[ ${string1%*zzz*} != "$string1" ]]
then	err_exit "string1%*zzz*"
fi
if	[[ ${string1%%*zzz*} != "$string1" ]]
then	err_exit "string1%%*zzz*"
fi
if	[[ ${string1#*zzz*} != "$string1" ]]
then	err_exit "string1#*zzz*"
fi
if	[[ ${string1##*zzz*} != "$string1" ]]
then	err_exit "string1##*zzz*"
fi
if	[[ ${string1%+(abc)} != "$base/abcabc" ]]
then	err_exit "string1%+(abc)"
fi
if	[[ ${string1%%+(abc)} != "$base/" ]]
then	err_exit "string1%%+(abc)"
fi
if	[[ ${string1%/*} != "$base" ]]
then	err_exit "string1%/*"
fi
if	[[ "${string1%/*}" != "$base" ]]
then	err_exit '"string1%/*"'
fi
if	[[ ${string1%"/*"} != "$string1" ]]
then	err_exit 'string1%"/*"'
fi
if	[[ ${string1%%/*} != "" ]]
then	err_exit "string1%%/*"
fi
if	[[ ${string1#*/bar} != /abcabcabc ]]
then	err_exit "string1#*bar"
fi
if	[[ ${string1##*/bar} != /abcabcabc ]]
then	err_exit "string1#*bar"
fi
if	[[ "${string1#@(*/bar|*/foo)}" != //bar/abcabcabc ]]
then	err_exit "string1#@(*/bar|*/foo)"
fi
if	[[ ${string1##@(*/bar|*/foo)} != /abcabcabc ]]
then	err_exit "string1##@(*/bar|*/foo)"
fi
if	[[ ${string1##*/@(bar|foo)} != /abcabcabc ]]
then	err_exit "string1##*/@(bar|foo)"
fi
foo=abc
if	[[ ${foo#a[b*} != abc ]]
then	err_exit "abc#a[b*} != abc"
fi
if	[[ ${foo//[0-9]/bar} != abc ]]
then	err_exit '${foo//[0-9]/bar} not expanding correctly'
fi
foo='(abc)'
if	[[ ${foo#'('} != 'abc)' ]]
then	err_exit "(abc)#( != abc)"
fi
if	[[ ${foo%')'} != '(abc' ]]
then	err_exit "(abc)%) != (abc"
fi
foo=a123b456c
if	[[ ${foo/[0-9]?/""} != a3b456c ]]
then	err_exit '${foo/[0-9]?/""} not expanding correctly'
fi
if	[[ ${foo//[0-9]/""} != abc ]]
then	err_exit '${foo//[0-9]/""} not expanding correctly'
fi
if	[[ ${foo/#a/b} != b123b456c ]]
then	err_exit '${foo/#a/b} not expanding correctly'
fi
if	[[ ${foo/#?/b} != b123b456c ]]
then	err_exit '${foo/#?/b} not expanding correctly'
fi
if	[[ ${foo/%c/b} != a123b456b ]]
then	err_exit '${foo/%c/b} not expanding correctly'
fi
if	[[ ${foo/%?/b} != a123b456b ]]
then	err_exit '${foo/%?/b} not expanding correctly'
fi
while read -r pattern string expected
do	if	(( expected ))
	then	if	[[ $string != $pattern ]]
		then	err_exit "$pattern does not match $string"
		fi
		if	[[ ${string##$pattern} != "" ]]
		then	err_exit "\${$string##$pattern} not null"
		fi
		if	[ "${string##$pattern}" != '' ]
		then	err_exit "\"\${$string##$pattern}\" not null"
		fi
		if	[[ ${string/$pattern} != "" ]]
		then	err_exit "\${$string/$pattern} not null"
		fi
	else	if	[[ $string == $pattern ]]
		then	err_exit "$pattern matches $string"
		fi
	fi
done <<- \EOF
	+(a)*+(a)	aabca	1
	!(*.o)		foo.o	0
	!(*.o)		foo.c	1
EOF
xx=a/b/c/d/e
yy=${xx#*/}
if	[[ $yy != b/c/d/e ]]
then	err_exit '${xx#*/} != a/b/c/d/e when xx=a/b/c/d/e'
fi
if	[[ ${xx//\//\\} != 'a\b\c\d\e' ]]
then	err_exit '${xx//\//\\} not working'
fi
exit $((Errors))
