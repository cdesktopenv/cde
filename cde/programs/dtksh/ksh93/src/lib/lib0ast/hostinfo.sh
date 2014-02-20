: $XConsortium: hostinfo.sh /main/2 1996/05/10 16:42:59 drk $
:
# hostinfo [ cpu debug name rating regress type ... ]
#
# print info for the current host on one line in argument order
#
# some of this is a stretch but we have to standardize on something
# if you don't like the results then change it here
#
# to generate a type regression record for the current host:
#
#	hostinfo regress type < /dev/null
#
# to test a file of type regression records:
#
#	hostinfo regress type < hosttype.tst
#
# lib/hostinfo/typemap is a file of <pattern> <value> pairs
# if the generated type matches a <pattern> then the type
# is changed to the corresponding <value>
#
# @(#)hostinfo (gsf@research.att.com) 05/09/95
#

command=hostinfo

args=$*
debug=
ifs=$IFS
magic=4400000
map=
regress=
shell=`(eval 'x=$((0+0))ksh; echo ${x#0}') 2>/dev/null`
something=
PATH=$PATH:/usr/kvm:/usr/ccs/bin:/usr/local/bin:/usr/add-on/gnu/bin:/usr/add-on/GNU/bin:/opt/gnu/bin:/opt/GNU/bin
export PATH

# validate the args

for info in $args
do	case $info in
	debug)	debug=1
		;;
	regress)regress=1
		;;
	cpu|name|rating|type)
		something=1
		;;
	*)	echo "$0: $info: unknown info name" >&2
		echo "Usage: $0 [ cpu debug name rating regress type ... ]" >&2
		exit 1
		;;
	esac
done
case $regress$something in
"")	set "$@" type ;;
esac
case $debug in
"")	exec 2>/dev/null
	;;
*)	PS4='+$LINENO+ '
	set -x
	;;
esac

# compute the info

output=
for info in $args
do	case $info in
	cpu)	cpu=1
		set	/usr/kvm/mpstat		'cpu[0-9]'	\
			/usr/etc/cpustatus	'enable'	\
			/usr/alliant/showsched	'CE'		\
			prtconf			'cpu-unit'
		while	:
		do	case $# in
			0)	break ;;
			esac
			i=`$1 2>/dev/null | tr ' 	' '

' | grep -c "^$2"`
			case $i in
			[1-9]*)	cpu=$i
				break
				;;
			esac
			shift 2
		done
		case $cpu in
		1)	set						\
									\
			hinv						\
			'/^[0-9][0-9]* .* Processors*$/'		\
			'/[ 	].*//'					\
									\
			/usr/bin/hostinfo				\
			'/^[0-9][0-9]* .* physically available\.*$/'	\
			'/[ 	].*//'					\

			while	:
			do	case $# in
				0)	break ;;
				esac
				i=`$1 2>/dev/null | sed -e "${2}!d" -e "s${3}"`
				case $i in
				[1-9]*)	cpu=$i
					break
					;;
				esac
				shift 3
			done
			;;
		esac
		output="$output $cpu"
		;;
	name)	name=`hostname || uname -n || cat /etc/whoami || echo local`
		output="$output $name"
		;;
	rating)	cd /tmp
		tmp=hi$$
		trap 'rm -f $tmp.*' 0 1 2
		cat > $tmp.c <<!
main()
{
	register unsigned long	i;
	register unsigned long	j;
	register unsigned long	k = 0;
	for (i = 0; i < 5000; i++)
		for (j = 0; j < 50000; j++)
			k += j;
	return k == 0;
}
!
		rating=1
		if	cc -o $tmp.exe -O $tmp.c ||
			gcc -o $tmp.exe -O $tmp.c
		then	set "" `{ time ./$tmp.exe; } 2>&1`
			while	:
			do	shift
				case $# in
				0)	break ;;
				esac
				case $1 in
				*[uU]*)	case $1 in
					[uU]*)	shift
						;;
					*)	IFS=${ifs}uU
						set $1
						IFS=$ifs
						;;
					esac
					case $shell in
					ksh)	IFS=${ifs}mMsS.
						set $1
						IFS=$ifs
						;;
					*)	m=`echo $1 | tr '[mMsS.]' ' '`
						set $m
						;;
					esac
					case $# in
					1)	m=0 s=$1 f=0 ;;
					2)	m=0 s=$1 f=$2 ;;
					*)	m=$1 s=$2 f=$3 ;;
					esac
					case $shell in
					ksh)	i="$(( $magic / ( ( $m * 60 + $s ) * 100 + $f ) ))"
						j="$(( ( $i % 10 ) / 10 ))"
						i="$(( i / 100 ))"
						if	(( $i >= 10 ))
						then	if	(( $j >= 5 ))
							then	i="$(( $i + 1 ))"
							fi
							j=
						else	j=.$j
						fi
						;;
					*)	i=`expr $magic / \( \( $m \* 60 + $s \) \* 100 + $f \)`
						j=`expr \( $i % 10 \) / 10`
						i=`expr $i / 100`
						if	expr $i \>= 10 >/dev/null
						then	if	expr $j \>= 5 >/dev/null
							then	i=`expr $i + 1`
							fi
							j=
						else	j=.$j
						fi
						;;
					esac
					rating=$i$j
					break
					;;
				esac
			done
		fi
		output="$output $rating"
		;;
	type)	IFS=:
		set /:$PATH
		IFS=$ifs
		shift
		f=../lib/hostinfo/typemap
		for i
		do	case $i in
			"")	i=. ;;
			esac
			if	test -f $i/$f
			then	map="`grep -v '^#' $i/$f` $map"
			fi
		done
		while	:
		do	case $regress in
			?*)	read expected host arch mach os sys rel ver ||
				case $regress in
				1)	regress=0 ;;
				*)	break ;;
				esac
				;;
			esac
			case $regress in
			""|0)	set "" \
				`hostname || uname -n || cat /etc/whoami || echo local` \
				`{ arch || uname -m || att uname -m || uname -s || att uname -s || echo unknown ;} | sed "s/[ 	]/-/g"` \
				`{ mach || machine || uname -p || att uname -p || echo unknown ;} | sed -e "s/[ 	]/-/g"` \
				`uname -a || att uname -a || echo unknown $host unknown unknown unknown unknown unknown`
				expected=$1 host=$2 arch=$3 mach=$4 os=$5 sys=$6 rel=$7 ver=$8
				;;
			esac
			type=unknown
			case $regress in
			?*)	regress=hostname
				case $host in
				*.*)	regress="$regress.domain" ;;
				esac
				regress="$regress $arch $mach $os hostname"
				case $sys in
				*.*)	regress="$regress.domain" ;;
				esac
				regress="$regress $rel $ver"
				;;
			esac
			case $host in
			*.*)	case $shell in
				ksh)	host=${host%%.*} ;;
				*)	host=`echo $host | sed -e 's/\..*//'` ;;
				esac
				;;
			esac
			case $mach in
			unknown)
				mach=
				;;
			r[3-9]000)
				case $shell in
				ksh)	mach="mips$((${mach#r}/1000-2))"
					;;
				*)	mach=`echo $mach | sed -e 's/^.//'`
					mach=mips`expr $mach / 1000 - 2`
					;;
				esac
				;;
			esac
			case $os in
			$host|unknown)
				set					\
									\
				/NextDeveloper		-d	next	\

				while	:
				do	case $# in
					0)	break ;;
					esac
					if	test $2 $1
					then	os=$3
						break
					fi
					shift 3
				done
				;;
			esac
			case $os in
			AIX*|aix*)
				type=ibm.risc
				;;
			HP-UX)	case $arch in
				9000/[78]*)
					type=hp.pa
					;;
				*)	type=hp.ux
					;;
				esac
				;;
			IRIX*|irix*)
				type=sgi.mips
				case $arch in
				[a-zA-Z][a-zA-Z][0-9][0-9]*|[a-zA-Z][a-zA-Z]1[3-9]*)
					type=${type}2
					;;
				[a-zA-Z][a-zA-Z][0-9]*)
					type=${type}1
					;;
				esac
				;;
			OSx*|SMP*|pyramid)
				type=pyr
				;;
			[Ss]ol*)
				type=sol.sun4
				;;
			[Ss]un*)
				case $shell in
				ksh)	x=${arch#sun?}
					type=${arch%$x}
					;;
				*)	type=`echo $arch | sed -e 's/\(sun.\).*/\1/'`
					;;
				esac
				case $rel in
				[0-4]*)	;;
				*)	case $type in
					'')	case $mach in
						sparc*)	type=sun4 ;;
						*)	type=$mach ;;
						esac
						;;
					esac
					type=sol.$type
					;;
				esac
				;;
			$host)	type=$arch
				case $type in
				*.*|*[0-9]*86|*68*)
					;;
				*)	case $mach in
					*[0-9]*86|*68*|mips)
						type=$type.$mach
						;;
					esac
					;;
				esac
				;;
			unknown)
				case $arch in
				?*)	case $arch in
					sun*)	mach= ;;
					esac
					type=$arch
					case $mach in
					?*)	type=$type.$mach ;;
					esac
					;;
				esac
				;;
			*)	case $ver in
				FTX*|ftx*)
					case $mach in
					*[0-9][a-zA-Z]*)
						case $shell in
						ksh)	mach="${mach%%+([a-zA-Z])}" ;;
						*)	mach=`echo $mach | sed -e 's/[a-zA-Z]*$//'` ;;
						esac
						;;
					esac
					type=stratus.$mach
					;;
				*)	case $shell in
					ksh)	type=${os%[0-9]*}
						type=${type%%[!A-Za-z_0-9.]*}
						;;
					*)	type=`echo $os | sed -e 's/[0-9].*//'`
						;;
					esac
					case $arch in
					'')	case $mach in
						?*)	type=$type.$mach ;;
						esac
						;;
					*)	type=$type.$arch ;;
					esac
					;;
				esac
			esac
			case $type in
			[0-9]*)	case $mach in
				?*)	type=$mach ;;
				esac
				case $type in
				*/MC)	type=ncr.$type ;;
				esac
				;;
			*.*)	;;
			*[0-9]*86|*68*)
				case $rel in
				[34].[0-9]*)
					type=att.$type
					;;
				esac
				;;
			[a-z]*[0-9])
				;;
			[a-z]*)	case $mach in
				$type)	case $ver in
					Fault*|fault*|FAULT*)
						type=ft.$type
						;;
					esac
					;;
				?*)	type=$type.$mach
					;;
				esac
				;;
			esac
			case $shell in
			ksh)	type=${type%%[-+/]*} ;;
			*)	type=`echo $type | sed -e 's/[-+/].*//'` ;;
			esac
			case $type in
			*.*)	case $shell in
				ksh)	lhs=${type%.*}
					rhs=${type#*.}
					;;
				*)	lhs=`echo $type | sed -e 's/\..*//'`
					rhs=`echo $type | sed -e 's/.*\.//'`
					;;
				esac
				case $rhs in
				[0-9]*86)
					rhs=i$rhs
					;;
				68*)	rhs=m$rhs
					;;
				esac
				case $rhs in
				i[2-9]86)	rhs=i386 ;;
				esac
				case $lhs in
				$rhs)	type=$lhs ;;
				*)	type=$lhs.$rhs ;;
				esac
				;;
			esac
			case $shell in
			*)	type=`echo $type | tr '[A-Z]' '[a-z]'` ;;
			esac

			# last chance mapping

			set "" "" $map
			while	:
			do	case $# in
				[012])	break ;;
				esac
				shift 2
				eval "	case \$type in
					$1)	type=\$2; break ;;
					esac"
			done
			case $regress in
			"")	break ;;
			esac
			case $expected in
			""|$type)	echo $type $regress ;;
			*)		echo FAIL $expected $type $regress ;;
			esac
		done
		output="$output $type"
		;;
	esac
done
case $regress in
"")	echo $output ;;
esac
