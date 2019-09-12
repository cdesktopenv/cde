# $XConsortium: shipfeature.sh /main/2 1995/07/19 17:09:46 drk $
:
# Glenn Fowler & Phong Vo
# AT&T Bell Laboratories
#
# C feature test support
# @(#)feature (AT&T Bell Laboratories) 03/01/94
#
# NOTE: .exe a.out suffix and [\\/] in path patterns for dos/nt

command=feature
version=03/01/94

binding="-Bdynamic -Bstatic -Wl,-ashared -Wl,-aarchive -call_shared -non_shared"
occ=cc
debug=
defhdr=
deflib=
executable='test -x'
gothdr=
gotlib=
dir=FEATURE
in=
out=
pragma=
case $RANDOM in
$RANDOM)shell=bsh
	$executable . 2>/dev/null || executable='test -r'
	;;
*)	shell=ksh
	;;
esac
static=.
statictest=
case $COTEMP in
"")	case $HOSTNAME in
	""|?|??|???|????|????)
		tmp=${HOSTNAME}
		;;
	*)	case $shell in
		ksh)	tmp=${HOSTNAME%${HOSTNAME#????}} ;;
		*)	eval `echo $HOSTNAME | sed 's/\(....\).*/tmp=\1/'` ;;
		esac
		;;
	esac
	tmp=${tmp}$$
	;;
*)	tmp=$COTEMP
	;;
esac
case $tmp in
??????????*)
	case $shell in
	ksh)	tmp=${tmp%${tmp#?????????}} ;;
	*)	eval `echo $tmp | sed 's/\(.........\).*/tmp=\1/'` ;;
	esac
	;;
?????????)
	;;
????????)
	tmp=F$tmp
	;;
esac
tmp=./$tmp

# standard error to /dev/null unless debugging
# standard output to the current output file
#
#	stdout	original standard output
#	stderr	original standard error
#	nullin	/dev/null input
#	nullout	/dev/null output

stdout=5 stderr=6 nullin=7 nullout=8
eval "exec $nullin>/dev/null $nullout>/dev/null $stdout>&1 $stderr>&2 2>&$nullout"

# options -- `-' for output to stdout otherwise usage

while	:
do	case $# in
	0)	in=-
		break
		;;
	esac
	case $1 in
	-)	out=-
		;;
	-*)	echo "Usage: $command [-] [op [arg ... [-l* *.a *.sh * ...]] [: op ...]" >&$stderr
		exit 2
		;;
	*)	break
		;;
	esac
	shift
done

# tmp files cleaned up on exit
# status: 0:success 1:failure 2:interrupt

status=2
trap "rm -f $tmp*.*; exit \$status" 0 1 2

# loop on op [ arg [ ... ] ] [ : op [ arg [ ... ] ] ]

cur=.
while	:
do	case $in in
	"")	case $# in
		0)	set set out + ;;
		esac
		;;
	*)	read lin || lin="set out +"
		set x $lin
		shift
		;;
	esac

	# check if "run xxx" is equivalent to "set in xxx"

	case $1 in
	run)	case $shell in
		ksh)	x=${2##*[\\/]}
			;;
		*)	case $2 in
			*/*)	x=`echo $2 | sed -e 's,.*[\\\\/],,'` ;;
			*)	x=$2 ;;
			esac
			;;
		esac
		case $x in
		*.*)	;;
		*)	set set in $2 ;;
		esac
		;;
	esac

	# set drops out early

	case $1 in
	""|\#*)	continue
		;;
	set)	shift
		case $1 in
		""|\#*)	op=
			;;
		*)	op=$1
			shift
			arg=
			while	:
			do	case $# in
				0)	break ;;
				esac
				case $1 in
				*" "*)	shift
					continue
					;;
				""|\#*)	break
					;;
				:)	shift
					break
					;;
				esac
				case $arg in
				"")	arg=$1 ;;
				*)	arg="$arg $1" ;;
				esac
				shift
			done
			;;
		esac
		case $op in
		cc)	occ=
			for x in $arg
			do	case $occ in
				"")	case $x in
					*=*)	case $shell in
						ksh)	export $x
							;;
						*)	eval $x
							export `echo $x | sed -e 's/=.*//'`
							;;
						esac
						;;
					*)	occ=$x
						;;
					esac
					;;
				*)	occ="$occ $x"
					;;
				esac
			done
			continue
			;;
		debug)	case $arg in
			0)	exec 2>&$nullout
				set -
				debug=
				;;
			""|1)	exec 2>&$stderr
				set -
				debug=
				;;
			2)	exec 2>&$stderr
				case $shell in
				ksh)	PS4="${PS4%+*([ 	])}+\$LINENO+ "
				esac
				set -x
				debug=-x
				;;
			*)	echo "$command: $arg: debug levels are 0, 1, 2" >&$stderr
				;;
			esac
			continue
			;;
		"in")	case $arg in
			"")	in=-
				;;
			*)	in=$arg
				if	test ! -f $in
				then	echo "$command: $in: not found" >&$stderr
					exit 1
				fi
				exec < $in
				case $out in
				"")	case $in in
					*[.\\/]*)
						case $shell in
						ksh)	out=${in##*[\\/]}
							out=${out%.*}
							;;
						*)	eval `echo $in | sed -e 's,.*[\\\\/],,' -e 's/\.[^.]*//' -e 's/^/out=/'`
							;;
						esac
						;;
					*)	out=$in
						;;
					esac
					;;
				esac
				;;
			esac
			continue
			;;
		nodebug)exec 2>&$nullout
			set -
			continue
			;;
		out)	out=$arg
			defhdr=
			deflib=
			gothdr=
			gotlib=
			;;
		prototyped|noprototyped)
			pragma="$pragma $op"
			;;
		pragma) pragma="$pragma $arg"
			;;
		shell)	shell=$arg
			continue
			;;
		static)	static=$arg
			continue
			;;
		*)	echo "$command: $op: unkown option" >&$stderr
			exit 1
			;;
		esac
		;;
	*)	case $1 in
		*\{)	op=-
			;;
		*)	op=$1
			shift
			;;
		esac
		arg=
		cc=$occ
		fail=
		hdr=
		lib=
		lst=hdr
		opt=
		pass=
		run=
		src=
		tst=
		case $# in
		0)	;;
		*)	case $1 in
			\#*)	set x
				shift
				;;
			*)	case $op in
				ref)	;;
				*)	case $1 in
					*\{)	arg=-
						;;
					*)	arg=$1
						shift
					esac
					;;
				esac
				;;
			esac
			while	:
			do	case $# in
				0)	break ;;
				esac
				case $1 in
				"")	;;
				\#*)	set x
					;;
				[a-z]*\{)
					v=$1
					x=
					shift
					while	:
					do	case $# in
						0)	case $in in
							"")	echo "$command: missing }end" >&$stderr
								exit 1
								;;
							esac
							while	:
							do	if	read o lin
								then	case $o in
									\}end)	set $o $lin
										break 2
										;;
									' '*|'	'*)
										set '' $o
										o=$2
										;;
									esac
								
									case $lin in
									"")	x="$x
$o" ;;
									*)	x="$x
$o $lin" ;;
									esac
								else	echo "$command: missing }end" >&$stderr
									exit 1
								fi
							done
							;;
						esac
						case $1 in
						\}end)	break
							;;
						*)	x="$x
$1"
							;;
						esac
						shift
					done
					case $v in
					fail\{)	fail=$x ;;
					pass\{)	pass=$x ;;
					test\{)	tst=$x ;;
					*)	src=$x run=$v ;;
					esac
					;;
				:)	shift
					break
					;;
				*[\(\)\{\}\ \	]*)
					tst="$tst $1"
					;;
				-)	case $lst in
					hdr)	hdr="$hdr $1" ;;
					lib)	lib="$lib $1" ;;
					esac
					;;
				-l*)	lib="$lib $1"
					lst=lib
					;;
				-*|+*)	case $op in
					ref)	occ="$occ $1" ;;
					run)	opt="$opt $1" ;;
					*)	cc="$cc $1" ;;
					esac
					;;
				*.a)	lib="$lib $1"
					lst=lib
					;;
				*[.\\/]*)
					hdr="$hdr $1"
					lst=hdr
					;;
				*)	tst="$tst $1"
					;;
				esac
				shift
			done
			;;
		esac
		;;
	esac

	# check the candidate headers

	case $hdr in
	?*)	z=$hdr
		hdr=
		hit=0
		for x in $z
		do	case $x in
			-)	case $hit in
				0)	hit=1 ;;
				1)	;;
				*)	break ;;
				esac
				continue
				;;
			*.h)	echo "#include \"$x\"" > $tmp.c
				$cc -E $tmp.c <&$nullin >&$nullout || continue
				;;
			*)	test -r $x || continue
				;;
			esac
			hdr="$hdr $x"
			case $hit in
			0)	;;
			1)	hit=2 ;;
			esac
		done
		;;
	esac

	# check the candidate libraries

	case $lib in
	?*)	z=$lib
		e=
		lib=
		echo "int main(){return(0);}" > $tmp.c
		for x in $z
		do	case $x in
			-)	case $lib in
				"")	continue ;;
				esac
				break
				;;
			esac
			while	:
			do	rm -f $tmp.exe
				if	$cc -o $tmp.exe $tmp.c $e $x <&$nullin >&$nullout
				then	lib="$lib $e $x"
					e=
					break
				else	case $e in
					"")	e=$x
						break
						;;
					*)	e=
						;;
					esac
				fi
			done
		done
		;;
	esac

	# precheck some more ops

	case $op in
	ref)	deflib="$deflib $lib"
		defhdr="$defhdr $hdr"
		continue
		;;
	esac
	ifs=$IFS
	IFS=,
	eval op=\"$op\"
	eval arg=\"$arg\"
	IFS=$ifs

	# check for op aliases

	x=
	for o in $op
	do	case $o in
		def|default)	x="$x cmd hdr key lib mth sys typ" ;;
		*)		x="$x $o" ;;
		esac
	done

	# loop on the ops o and args a

	for o in $x
	do	for a in $arg
		do	user=
			case $a in
			*[.\\/]*)
				case $o in
				hdr|sys)
					case $a in
					*[\\/]*) x=/ ;;
					*)	 x=. ;;
					esac
					case $shell in
					ksh)	p=${a%%${x}*}
						v=${a##*${x}}
						;;
					*)	eval `echo $a | sed -e "s/\\(.*\\)\\\\${x}\\(.*\\)/p=\\1 v=\\2/"`
						;;
					esac
					;;
				*)	case $shell in
					ksh)	p=${a%.*}
						p=${p##*[\\/]}
						v=${a##*.}
						v=${v##*[\\/]}
						;;
					*)	eval `echo $a | sed -e 's,.*[\\\\/],,' -e 's/\(.*\)\.\(.*\)/p=\1 v=\2/'`
						;;
					esac
					;;
				esac
				f=${p}/${v}
				case $o in
				run)	v=$p
					p=
					m=_${v}
					;;
				mem)	m=_${v}_${p}
					;;
				*)	m=_${p}_${v}
					;;
				esac
				;;
			*)	p=
				v=$a
				f=$a
				m=_${v}
				;;
			esac

			# check output redirection

			case $out in
			$cur)	;;
			*)	case $cur in
				$a|$v)	;;
				*)	case $cur in
					.)	;;
					-)	echo "#endif"
						;;
					*)	echo "#endif"
						exec >&-
						case $cur in
						*[\\/]*|*.h)	x=$cur ;;
						*)		x=$dir/$cur ;;
						esac
						proto -r $tmp.h
						sed 's,/\*[^/]*\*/, ,g' $tmp.h > $tmp.c
						sed 's,/\*[^/]*\*/, ,g' $x > $tmp.t
						if	cmp -s $tmp.c $tmp.t
						then	rm -f $tmp.h
						else	case $x in
							${dir}[\\/]$cur)	test -d $dir || mkdir $dir || exit 1 ;;
							esac
							mv $tmp.h $x
						fi
						;;
					esac
					case $out in
					+)	case $status in
						1)	;;
						*)	status=0 ;;
						esac
						exit $status
						;;
					-)	eval "exec >&$stdout"
						;;
					*)	exec >$tmp.h
						;;
					esac
					case $out in
					"")	case $a in
						*[\\/]*|???????????????*) cur=$v ;;
						*)			cur=$a ;;
						esac
						;;
					*)	cur=$out
						;;
					esac
					case $in in
					""|-|+)	case $o in
						run)	x=" from $a" ;;
						*)	x= ;;
						esac
						;;
					*)	x=" from $in"
						;;
					esac
					echo "/* : : generated$x by $command version $version : : */"
					for x in $pragma
					do	echo "#pragma $x"
					done
					case $out in
					""|-|+)	x=$m
						;;
					*.*)	case $shell in
						ksh)	i=$out
							x=_
							while	:
							do	case $i in
								*.*)	x=$x${i%%.*}_
									i=${i#*.}
									;;
								*)	x=$x$i
									break
									;;
								esac
							done
							;;
						*)	eval `echo $in | sed -e 's,\.,_,g' -e 's/^/x=/'`
							;;
						esac
						;;
					*)	x=_$out
						;;
					esac
					case $x in
					*-*)	echo "#if 1"
						;;
					*)	case $x in
						_cmd|_lib)
							case $shell in
							ksh)	c=${PWD##*[\\/]}
								c=${c#lib}
								c=${c%%.*}
								x=${x}_${c}
								;;
							*)	x=${x}_`echo $x | sed -e 's,.*[\\\\/],,' -e 's,^lib,,' -e 's,\..*,,'`
								;;
							esac
							;;
						esac
						x=`echo $x | sed -e 's/[^a-zA-Z0-9_]/_/g'`
						echo "#ifndef _def${x}"
						echo "#define _def${x}	1"
						;;
					esac
					;;
				esac
				;;
			esac

			# set up the candidate include list

			inc=
			for x in $defhdr $hdr
			do	case $x in
				*.h)	case $shell in
					ksh)	c=${x##*[\\/]}
						c=${c%%.*}
						case $x in
						*/*)	c=${x%%[\\/]*}_${c} ;;
						esac
						;;
					*)	eval `echo $x | sed -e 's,^\([^\\\\/]*\).*[\\\\/]\([^\\\\/]*\)\$,\1_\2,' -e 's/\..*//' -e 's/^/c=/'`
						;;
					esac
					case " $gothdr " in
					*" $x "*)
						;;
					*)	gothdr="$gothdr $x"
						echo "#define _inc_${c}	1	/* candidate header <$x> found */"
						;;
					esac
					inc="$inc
#include <$x>"
					;;
				esac
			done

			# set up the candidate lib list

			for x in $lib $deflib
			do	case " $gotlib " in
				*" $x "*)
					;;
				*)	gotlib="$gotlib $x"
					echo "/* candidate library $x found */"
					;;
				esac
			done

			# src overrides builtin test

			case $o in
			tst)	;;
			*)	m=_${o}${m}
				;;
			esac
			pre="#undef $v"
			case $src in
			?*)	echo "$inc
$src" > $tmp.c
				e=
				case $run in
				cat*|nocat*)
					echo "$src"
					;;
				run*|norun*)
					(eval "$src") <&$nullin || e=1
					;;
				mac*|nomac*)
					if	$cc -E $tmp.c <&$nullin >$tmp.i
					then	sed -e '/<<[ 	]*".*"[ 	]*>>/!d' -e 's/<<[ 	]*"//g' -e 's/"[ 	]*>>//g' $tmp.i
					else	e=1
					fi
					;;
				p*|nop*)$cc -DTEST=$p -DID=$v -E $tmp.c <&$nullin >&$nullout || e=1
					;;
				c*|noc*)$cc -DTEST=$p -DID=$v -c $tmp.c <&$nullin >&$nullout || e=1
					;;
				*)	rm -f $tmp.exe
					if	$cc -DTEST=$p -DID=$v -o $tmp.exe $tmp.c $lib $deflib <&$nullin >&$nullout && $executable $tmp.exe
					then	case $run in
						l*|nol*);;
						o*|noo*)$tmp.exe <&$nullin || e=1 ;;
						*)	$tmp.exe <&$nullin >&$nullout || e=1 ;;
						esac
					else	e=1
					fi
					;;
				esac
				case $run in
				no*)	case $e in
					"")	e=1 ;;
					*)	e= ;;
					esac
					;;
				esac
				case $e in
				"")	case $m in
					*-*)	;;
					*)	echo "#define $m	1	/* ${run}} passed */" ;;
					esac
					(eval "$pass") <&$nullin
					;;
				*)	(eval "$fail") <&$nullin
					;;
				esac
				continue
				;;
			esac

			# initialize common builtin state

			case $o in
			dat|lib|mth|run)
				case $statictest in
				"")	statictest=FoobaR
					echo "#if __cplusplus
extern \"C\" {
#endif
extern int $statictest();
#if __cplusplus
}
#endif
static int ((*i)())=$statictest;main(){return(i==0);}" > $tmp.c
					rm -f $tmp.exe
					if	$cc -o $tmp.exe $tmp.c <&$nullin >&$nullout && $executable $tmp.exe
					then	case $static in
						.)	static=
							echo '#include <stdio.h>
main(){printf("hello");return(0);}' > $tmp.c
							rm -f $tmp.exe
							if	$cc -c $tmp.c <&$nullin >&$nullout && $cc -o $tmp.exe $tmp.o <&$nullin >&$nullout 2>$tmp.e && $executable $tmp.exe
							then	e=`wc -l $tmp.e`
								set x x $binding
								while	:
								do	shift
									shift
									case $# in
									0)	break ;;
									esac
									rm -f $tmp.exe
									$cc -o $tmp.exe $1 $tmp.o <&$nullin >&$nullout 2>$tmp.e && $executable $tmp.exe || continue
									case `wc -l $tmp.e` in
									$e)	;;
									*)	continue ;;
									esac
									d=`ls -s $tmp.exe`
									rm -f $tmp.exe
									$cc -o $tmp.exe $2 $tmp.o <&$nullin >&$nullout 2>$tmp.e && $executable $tmp.exe || continue
									case `wc -l $tmp.e` in
									$e)	;;
									*)	continue ;;
									esac
									case `ls -s $tmp.exe` in
									$d)	;;
									*)	static=$2
										set x
										shift
										break
										;;
									esac
								done
							fi
							;;
						esac
					else	static=
					fi
					;;
				esac
				;;
			esac

			# builtin tests

			case $o in
			cmd)	case $p in
				?*)	continue ;;
				esac
				k=
				for j in "" usr
				do	case $j in
					"")	d= s= ;;
					*)	d=/$j s=_$j ;;
					esac
					for i in bin etc ucb
					do	if	test -f $d/$i/$a
						then	case $k in
							"")	k=1
								echo "#define $m	1	/* $a in ?(/usr)/(bin|etc|ucb) */"
								;;
							esac
							echo "#define ${s}_${i}_${v}	1	/* $d/$i/$a found */"
						fi
					done
				done
				;;
			dat)	case $p in
				?*)	continue ;;
				esac
				{
				echo "$pre"
				case $inc in
				?*)	echo "$inc"
					;;
				*)	echo "#if __cplusplus
extern \"C\" {
#endif
extern int $v;
#if __cplusplus
}
#endif"
					;;
				esac
				echo "int main(){char* i = (char*)&$v; return i!=0;}"
				} > $tmp.c
				$cc -c $tmp.c <&$nullin >&$nullout &&
				rm -f $tmp.exe
				if	$cc $static -o $tmp.exe $tmp.o $lib $deflib <&$nullin >&$nullout && $executable $tmp.exe
				then	echo "#define $m	1	/* $v in default lib(s) */"
				fi
				;;
			hdr|lcl|sys)
				case $p in
				lcl)	eval p='$'_lcl_$v
					case $p in
					?*)	continue ;;
					esac
					eval _lcl_$v=1
					p=
					f=$v
					;;
				*)	case $o in
					hdr)	echo "#include <$f.h>" > $tmp.c
						$cc -E $tmp.c <&$nullin >&$nullout &&
						echo "#define $m	1	/* #include <$f.h> ok */"
						;;
					sys)	echo "#include <sys/$f.h>" > $tmp.c
						$cc -E $tmp.c <&$nullin >&$nullout &&
						echo "#define $m	1	/* #include <sys/$f.h> ok */"
						;;
					esac
					continue
					;;
				esac
				case $p in
				?*)	x="$p\\$v" ;;
				*)	x=$f ;;
				esac
				case $f in
				*[\\/]*)g=$f ;;
				*)	g="$f sys/$f" ;;
				esac
				for f in $g
				do	echo "$pre
$inc
#include <$f.h>" > $tmp.c
					if	$cc -E $tmp.c <&$nullin >$tmp.i
					then	i=`sed -e '/^#[line 	]*1[ 	][ 	]*"[\\\\/].*[\\\\/]'$x'\.h"/!d' -e '/[\\\\/]sys[\\\\/]'$x'\.h"/d' -e s'/.*"\(.*\)".*/\1/' $tmp.i`
						for i in $i
						do	break
						done
					else	i=
					fi
					case $i in
					[\\/]*)	echo "#if defined(__STDPP__directive)"
						echo "__STDPP__directive pragma pp:hosted"
						echo "#endif"
						echo "#include \"$i\"	/* untrusted local <$f.h> */"
						echo "#undef	$m"
						echo "#define $m	1"
						break
						;;
					*)	echo "/* no local <$f.h> */"
						;;
					esac
				done
				;;
			key)	case $p in
				?*)	continue ;;
				esac
				echo "$pre
int f(){int $v = 1;return($v);}" > $tmp.c
				$cc -c $tmp.c <&$nullin >&$nullout ||
				echo "#define $m	1	/* $v is a reserved keyword */"
				;;
			lib|mth)case $p in
				?*)	continue ;;
				esac
				echo "$pre
$inc
#if __cplusplus
extern \"C\" {
#endif
extern int $v();
#if __cplusplus
}
#endif
static int ((*i)())=$v;main(){return(i==0);}" > $tmp.c
				$cc -c $tmp.c <&$nullin >&$nullout &&
				rm -f $tmp.exe
				if	$cc $static -o $tmp.exe $tmp.o $lib $deflib <&$nullin >&$nullout && $executable $tmp.exe
				then	case $o in
					lib)	echo "#define $m	1	/* $v() in default lib(s) */" ;;
					esac
				else	case $o in
					mth)	rm -f $tmp.exe
						$cc $static -o $tmp.exe $tmp.o -lm <&$nullin >&$nullout &&
						$executable $tmp.exe &&
						echo "#define $m	1	/* $v() in math lib */"
						;;
					esac
				fi
				;;
			mac)	case $p in
				?*)	continue ;;
				esac
				echo "$pre
$inc
#ifdef $v
'#define $m	1	/* $v is a macro */'
#endif" > $tmp.c
				$cc -E $tmp.c <&$nullin | sed -e "/^'#define/!d" -e "s/'//g"
				;;
			mem)	case $p in
				?*)	for i in "" "struct "
					do	echo "$pre
$inc
static $i$p i;
int n = sizeof(i.$v);" > $tmp.c
						$cc -c $tmp.c <&$nullin >&$nullout &&
						echo "#define $m	1	/* $v is member of $i$p */" &&
						break
					done
					;;
				*)	echo "$command: $o: <struct>.<member> expected" >&$stderr
					status=1
					;;
				esac
				;;
			nop)	;;
			one)	for i in $a $hdr
				do	x="#include <$i>"
					echo "$x" > $tmp.c
					if	$cc -E $tmp.c <&$nullin >&$nullout
					then	echo "$x"
						break
					fi
				done
				;;
			out)	;;
			run)	if	test ! -f $a
				then	echo "$command: $a: not found" >&$stderr
					exit 1
				fi
				case $a in
				*.c)	rm -f $tmp.exe
					cp $a $tmp.c
					$cc -o $tmp.exe $tmp.c $lib $deflib <&$nullin >&$nullout &&
					$executable $tmp.exe &&
					$tmp.exe $opt <&$nullin
					;;
				*.sh)	${SHELL-sh} $debug $a "cc='$cc' executable='$executable' id='$m' static='$static' tmp='$tmp'" $opt $hdr <&$nullin
					;;
				*)	false
					;;
				esac
				case $? in
				0)	;;
				*)	echo "$command: $a: cannot run" >&$stderr
					exit 1
					;;
				esac
				;;
			sym)	case $tst in
				"")	x=$v ;;
				*)	x=$tst ;;
				esac
				echo "$pre
$inc
'=' $x '='" > $tmp.c
				$cc -E $tmp.c <&$nullin \
				| sed \
					-e "/'='/!d" \
					-e "s/'='//g" \
					-e 's/[ 	]//g' \
					-e 's/((([^()]*)))->/->/g' \
					-e 's/(([^()]*))->/->/g' \
					-e 's/([^()]*)->/->/g' \
					-e 's/\([a-zA-Z_][a-zA-Z_0-9]*\)\[/\
ary \1[/g' \
					-e 's/\([a-zA-Z_][a-zA-Z_0-9]*\)(/\
fun \1[/g' \
					-e 's/\*->\([a-zA-Z_]\)/->\
ptr \1/g' \
					-e 's/->\([a-zA-Z_]\)/->\
reg \1/g' \
					-e "/^$v\$/d" \
					-e 's/^[a-zA-Z_][a-zA-Z_0-9]*$/\
nam &/g' \
				| sed \
					-e '/^... /!d' \
				| sort \
					-u \
				| sed \
					-e 's/\(...\) \([a-zA-Z_][a-zA-Z_0-9]*\).*/#ifndef _\1_'$v'\
#define _\1_'$v' \2\
#define _\1_'$v'_str "\2"\
#endif/'
				;;
			typ)	case $p in
				"")	x= ;;
				*)	x="$p " ;;
				esac
				case $typ in
				"")	typ="
#include <sys/types.h>"
					c=
					h=
					for i in time "" times
					do	case $i in
						"")	c=sys_
							h=sys/
							continue
							;;
						esac
						c=${c}$i
						h=${h}$i.h
						t="$typ
#include <$h>"
						echo "$t" > $tmp.c
						if	$cc -c $tmp.c <&$nullin >&$nullout
						then	typ="$t"
							echo "#define _inc_${c}	1	/* candidate header <$h> found */"
						fi
						c=
						h=
					done
					t=
					for i in stddef stdlib
					do	u="$t
#include <$i.h>"
						echo "$u$typ" > $tmp.c
						if	$cc -c $tmp.c <&$nullin >&$nullout
						then	t="$u"
							echo "#define _inc_$i	1	/* candidate header <$i.h> found */"
						else	break
						fi
					done
					typ="$t$typ"
					;;
				esac
				{
				case $p in
				long)	echo "$pre
$typ$inc
static $x$v i;
$x$v f() {
$x$v v; v = i;"
					echo "v <<= 4; i = v >> 2; i = v * i; i = i / v; v = v + i; i = i - v; i = 10; i = v % i; i |= v; v ^= i; i = 123; v &= i;
return v; }"
					;;
				*)	echo "$pre
$typ$inc
struct xxx { $x$v mem; };
static struct xxx v;
struct xxx* f() { return &v; }"
					;;
				esac
				echo "int main() { f(); return 0; }"
				} > $tmp.c
				rm -f $tmp.exe
				if	$cc -o $tmp.exe $tmp.c <&$nullin >&$nullout &&
				$executable $tmp.exe
				then	echo "#define $m	1	/* $x$v is a type */"
					user=$pass
				else	user=$fail
				fi
				;;
			*)	echo "$command: $o: unknown feature test" >&$stderr
				status=1
				;;
			esac
			case $user in
			?*)	(eval "$user") <&$nullin ;;
			esac
		done
	done
done
