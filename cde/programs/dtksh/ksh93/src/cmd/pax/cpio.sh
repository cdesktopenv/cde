# $XConsortium: cpio.sh /main/2 1995/07/19 17:13:12 drk $
:
# Glenn Fowler
# AT&T Bell Laboratories
# @(#)cpio.sh (gsf@research.att.com) 08/11/90
#
# cpio -> pax interface script
#

command=cpio
usage="
Usage: $command -o[acvBV] [-C size] [-M mesg] [-O file | >file ] <list
       $command -i[bcdfkmrtsuvBSV6] [-I file | <file] [pattern ...]
       $command -p[adlmuvV] directory"

case $1 in
*i*)	mode="-r" ;;
*o*)	mode="-w" ;;
*p*)	mode="-rw" ;;
*)	echo "$command: one of -i, -o, -p must be specified$usage" >&2; exit 1 ;;
esac
options=""
blocksize=1b
debug=
format=binary
list=""
logphys=-P
d_default="-d"
m_default="-m"
u_default="-u"
r_ok="1"
w_ok="1"
p_ok="1"
while	:
do	case $# in
	0)	break ;;
	esac
	case $1 in
	--)	shift; break ;;
	-*)	for opt in `echo '' $1 | sed -e 's/-//' -e 's/./& /g'`
		do	case $opt in
			'#')	case $debug in
				"")	debug=echo ;;
				*)	debug=args ;;
				esac
				;;
			[bsS6]) ;;
			[klvV])	options="$options -$opt" ;;
			a)	r_ok="" options="$options -p" ;;
			c)	format=cpio ;;
			d)	w_ok="" d_default="" ;;
			f)	w_ok="" p_ok="" options="$options -c" ;;
			i)	w_ok="" p_ok="" ;;
			m)	w_ok="" m_default="" ;;
			o)	r_ok="" p_ok="" u_default="" ;;
			p)	r_ok="" w_ok="" ;;
			r)	w_ok="" p_ok="" options="$options -i" ;;
			t)	w_ok="" p_ok="" list="1" ;;
			u)	w_ok="" u_default="" ;;
			B)	blocksize=5k ;;
			L)	logphys=-L ;;
			[CIMO])	a=`echo '' $1 | sed -e "s/[^$opt]*$opt//"`
				case $a in
				"")	case $# in
					1)	echo "$command: option -$opt requires an argument$usage" >&2; exit 1 ;;
					esac
					shift
					a=$1
					;;
				esac
				case $opt in
				C)	case $a in
					*[0-9])	a=${a}c ;;
					esac
					blocksize=$a
					;;
				I)	w_ok="" p_ok="" options="$options -f '$a'" ;;
				O)	r_ok="" p_ok="" options="$options -f '$a'" ;;
				M)	options="$options -$opt '$a'" ;;
				esac
				break
				;;
			*)	echo "$command: invalid option -$opt$usage" >&2; exit 1 ;;
			esac
		done
		;;
	*)	break ;;
	esac
	shift
done
case $mode in
-r)	case $r_ok in
	"")	echo "$command: options inconsistent with archive read" >&2; exit 1 ;;
	esac
	options="$options -b $blocksize"
	;;
-w)	case $w_ok in
	"")	echo "$command: options inconsistent with archive write" >&2; exit 1 ;;
	esac
	case $# in
	0)	;;
	*)	echo "$command: arguments not expected" >&2; exit 1 ;;
	esac
	options="$options -x $format -b $blocksize"
	;;
-rw)	case $p_ok in
	"")	echo "$command: options inconsistent with file pass" >&2; exit 1 ;;
	esac
	case $# in
	1)	;;
	*)	echo "$command: a single directory argument is expected$usage" >&2; exit 1 ;;
	esac
	;;
esac
case $list in
"1")	mode="" d_default="" m_default="" u_default="" ;;
esac
$debug pax $mode $logphys $options $d_default $m_default $u_default "$@"
