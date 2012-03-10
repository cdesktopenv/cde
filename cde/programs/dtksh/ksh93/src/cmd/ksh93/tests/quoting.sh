# $XConsortium: quoting.sh /main/3 1995/11/01 16:56:20 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

Command=$0
integer Errors=0
if	[[ 'hi there' != "hi there" ]]
then	err_exit "single quotes not the same as double quotes"
fi
x='hi there'
if	[[ $x != 'hi there' ]]
then	err_exit "$x not the same as 'hi there'"
fi
if	[[ $x != "hi there" ]]
then	err_exit "$x not the same as \"hi there \""
fi
if	[[ \a\b\c\*\|\"\ \\ != 'abc*|" \' ]]
then	err_exit " \\ differs from '' "
fi
if	[[ "ab\'\"\$(" != 'ab\'\''"$(' ]]
then	err_exit " \"\" differs from '' "
fi
if	[[ $(print -r - 'abc*|" \') !=  'abc*|" \' ]]
then	err_exit "\$(print -r - '') differs from ''"
fi
if	[[ $(print -r - "abc*|\" \\") !=  'abc*|" \' ]]
then	err_exit "\$(print -r - '') differs from ''"
fi
if	[[ "$(print -r - 'abc*|" \')" !=  'abc*|" \' ]]
then	err_exit "\"\$(print -r - '')\" differs from ''"
fi
if	[[ "$(print -r - "abc*|\" \\")" !=  'abc*|" \' ]]
then	err_exit "\"\$(print -r - "")\" differs from ''"
fi
if	[[ $(print -r - $(print -r - 'abc*|" \')) !=  'abc*|" \' ]]
then	err_exit "nested \$(print -r - '') differs from ''"
fi
if	[[ "$(print -r - $(print -r - 'abc*|" \'))" !=  'abc*|" \' ]]
then	err_exit "\"nested \$(print -r - '')\" differs from ''"
fi
if	[[ $(print -r - "$(print -r - 'abc*|" \')") !=  'abc*|" \' ]]
then	err_exit "nested \"\$(print -r - '')\" differs from ''"
fi
unset x
if	[[ ${x-$(print -r - "abc*|\" \\")} !=  'abc*|" \' ]]
then	err_exit "\${x-\$(print -r - '')} differs from ''"
fi
if	[[ ${x-$(print -r - "a}c*|\" \\")} !=  'a}c*|" \' ]]
then	err_exit "\${x-\$(print -r - '}')} differs from ''"
fi
x=$((echo foo)|(cat))
if	[[ $x != foo  ]]
then	err_exit "((cmd)|(cmd)) failed"
fi
x=$(print -r -- "\"$HOME\"")
if	[[ $x != '"'$HOME'"' ]]
then	err_exit "nested double quotes failed"
fi
: ${z="a{b}c"}
if	[[ $z != 'a{b}c' ]]
then	err_exit '${z="a{b}c"} not correct'
fi
unset z
: "${z="a{b}c"}"
if	[[ $z != 'a{b}c' ]]
then	err_exit '"${z="a{b}c"}" not correct'
fi
if	[[ $(print -r -- "a\*b") !=  'a\*b' ]]
then	err_exit '$(print -r -- "a\*b") differs from  a\*b'
fi
unset x
if	[[ $(print -r -- "a\*b$x") !=  'a\*b' ]]
then	err_exit '$(print -r -- "a\*b$x") differs from  a\*b'
fi
x=hello
set -- ${x+foo bar bam}
if	(( $# !=3 ))
then	err_exit '${x+foo bar bam} does not yield three arguments'
fi
set -- ${x+foo "bar bam"}
if	(( $# !=2 ))
then	err_exit '${x+foo "bar bam"} does not yield two arguments'
fi
set -- ${x+foo 'bar bam'}
if	(( $# !=2 ))
then	err_exit '${x+foo '\''bar bam'\''} does not yield two arguments'
fi
set -- ${x+foo $x bam}
if	(( $# !=3 ))
then	err_exit '${x+foo $x bam} does not yield three arguments'
fi
set -- ${x+foo "$x" bam}
if	(( $# !=3 ))
then	err_exit '${x+foo "$x" bam} does not yield three arguments'
fi
set -- ${x+"foo $x bam"}
if	(( $# !=1 ))
then	err_exit '${x+"foo $x bam"} does not yield one argument'
fi
set -- "${x+foo $x bam}"
if	(( $# !=1 ))
then	err_exit '"${x+foo $x bam}" does not yield one argument'
fi
set -- ${x+foo "$x "bam}
if	(( $# !=2 ))
then	err_exit '${x+foo "$x "bam} does not yield two arguments'
fi
x="ab$'cd"
if	[[ $x != 'ab$'"'cd" ]]
then	err_exit '$'"' inside double quotes not working"
fi
x=`print 'ab$'`
if	[[ $x != 'ab$' ]]
then	err_exit '$'"' inside `` quotes not working"
fi
unset a
x=$(print -r -- "'\
\
")
if	[[ $x != "'" ]]
then	err_exit 'line continuation in double strings not working'
fi
x=$(print -r -- "'\
$a\
")
if	[[ $x != "'" ]]
then	err_exit 'line continuation in expanded double strings not working'
fi
x='\*'
if	[[ $(print -r -- $x) != '\*' ]]
then	err_exit 'x="\\*";$x != \*'
fi
x='   hello    world    '
set -- $x
if	(( $# != 2 ))
then	err_exit 'field splitting error'
fi
x=$(print -r -- '1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890
1234567890123456789012345678901234567890123456789012345678901234567890')
if	(( ${#x} != (15*71-1) ))
then	err_exit "length of x, ${#x}, is incorrect should be $((15*71-1))"
fi
x='$hi'
if	[[ $x\$ != '$hi$' ]]
then	err_exit ' $x\$, with x=$hi, does not expand to $hi$'
fi
if	[[ $x$ != '$hi$' ]]
then	err_exit ' $x$, with x=$hi, does not expand to $hi$'
fi
set -- $(/bin/echo foo;sleep 1;/bin/echo bar)
if	[[ $# != 2 ]]
then	err_exit 'word splitting after command substitution not working'
fi
unset q
if	[[ "${q:+'}q${q:+'}" != q ]]
then	err_exit 'expansion of "{q:+'\''}" not correct when q unset'
fi
q=1
if	[[ "${q:+'}q${q:+'}" != "'q'" ]]
then	err_exit 'expansion of "{q:+'\''}" not correct when q set'
fi
x=$'x\' #y'
if	[[ $x != "x' #y" ]]
then	err_exit "$'x\' #y'" not working
fi
x=$q$'x\' #y'
if	[[ $x != "1x' #y" ]]
then	err_exit "$q$'x\' #y'" not working
fi
IFS=,
x='a,b\,c,d'
set -- $x
if	[[ $2 != 'b\' ]]
then	err_exit "field splitting of $x with IFS=$IFS not working"
fi
foo=bar
bar=$(print -r -- ${foo+\\n\ })
if	[[ $bar != '\n ' ]]
then	err_exit '${foo+\\n\ } expansion error' 
fi
unset bar
bar=$(print -r -- ${foo+\\n\ $bar})
if	[[ $bar != '\n ' ]]
then	err_exit '${foo+\\n\ $bar} expansion error with bar unset' 
fi
x='\\(..\\)|&\|\|\\&\\|'
if	[[ $(print -r -- $x) != "$x" ]]
then	err_exit '$x, where x=\\(..\\)|&\|\|\\&\\| not working'
fi
x='\\('
if	[[ $(print -r -- a${x}b) != a"${x}"b ]]
then	err_exit 'a${x}b, where x=\\( not working'
fi
x=
if	[[ $(print -r -- $x'\\1') != '\\1' ]]
then	err_exit 'backreference inside single quotes broken'
fi
set -- ''
set -- "$@"
if	(( $# != 1 ))
then	err_exit '"$@" not preserving nulls'
fi
x=
if	[[ $(print -r s"!\2${x}\1\a!") != 's!\2\1\a!' ]]
then	err_exit  'print -r s"!\2${x}\1\a!" not equal s!\2\1\a!'
fi
if	[[ $(print -r $'foo\n\n\n') != foo ]]
then	err_exit 'trailing newlines on comsubstitution not removed'
fi
unset x
if	[[ ${x:='//'} != '//' ]]
then	err_exit '${x:='//'} != "//"'
fi
if	[[ $(print -r "\"hi$\"")	!= '"hi$"' ]]
then	err_exit '$\ not correct inside ""'
fi
exit $((Errors))
