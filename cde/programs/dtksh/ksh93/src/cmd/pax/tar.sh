# $XConsortium: tar.sh /main/2 1995/07/19 17:13:19 drk $
:
# Glenn Fowler
# AT&T Bell Laboratories
# @(#)tar.sh (gsf@research.att.com) 04/06/89
#
# tar -> pax interface script
#

command=tar
usage="
Usage: $command c[vwfbB[[0-9][hlm]]] [pathname ...]
       $command r[vwfbB[[0-9][hlm]]] [files ...]
       $command t[vwfbB[[0-9][hlm]]
       $command u[vwfbB[[0-9][hlm]]] [pathname ...]
       $command x[vwfblmB[[0-9][hlm]]] [pathname ...]"

case $1 in
*[tx]*)		mode="-r" ;;
*[cru]*)	mode="-w" ;;
*)		echo "$command: one of crtux must be specified$usage" >&2; exit 1 ;;
esac
options="-P"
file="-t 0"
list=""
r_ok="1"
w_ok="1"
arg=$1
lastopt=""
shift
for opt in `echo '' $arg | sed -e 's/^ -//' -e 's/./& /g'`
do	case $opt in
	[0-9])	file="-t $opt" ;;
	[hlm])	case $lastopt in
		[0-9])	file="${file}$opt" ;;
		*)	case $opt in
			h)	options="$options -L" ;;
			l)	;;
			m)	r_ok="" options="$options -$opt" ;;
			esac
			;;
		esac
		;;
	[v])	options="$options -$opt" ;;
	b)	case $# in
		0)	echo "$command: blocking factor argument expected$usage" >&2; exit 1 ;;
		esac
		options="$options -b ${1}b"
		shift
		;;
	c)	r_ok="" ;;
	f)	case $# in
		0)	echo "$command: file name argument expected$usage" >&2; exit 1 ;;
		esac
		case $1 in
		-)	file="" ;;
		*)	file="-f $1" ;;
		esac
		shift
		;;
	r)	r_ok="" options="$options -a" ;;
	t)	w_ok="" list="1" ;;
	u)	r_ok="" options="$options -u" ;;
	w)	options="$options -y" ;;
	x)	w_ok="" ;;
	B)	options="$options -b 10k" ;;
	*)	echo "$command: invalid option -$opt$usage" >&2; exit 1 ;;
	esac
	lastopt=$opt
done
case $mode in
-r)	case $r_ok in
	"")	echo "$command: options inconsistent with archive read" >&2; exit 1 ;;
	esac
	;;
-w)	case $w_ok in
	"")	echo "$command: options inconsistent with archive write" >&2; exit 1 ;;
	esac
	case $# in
	0)	set - "." ;;
	esac
	options="$options -x tar"
	;;
esac
case $list in
"1")	mode="" ;;
esac
pax $mode $options $file "$@"
