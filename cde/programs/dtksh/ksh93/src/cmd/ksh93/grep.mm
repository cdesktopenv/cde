./" $XConsortium: grep.mm /main/2 1995/07/18 10:14:53 drk $
#	SHELL VERSION OF GREP
#
vflag= xflag= cflag= lflag= nflag=
set -f
integer errors=0
while	getopts "ce:f:[file]ilnxv [file]..." opt # look for grep options
do	case	$opt in
	v)	vflag=1;;
	x)	xflag=1;;
	c)	cflag=1;;
	l)	lflag=1;;
	n)	nflag=1;;
	e)	expr=$OPTARG;;
	f)	expr=$(< $OPTARG);;
	esac
done
(( $? == 2 )) && exit 2
shift $((OPTIND-1))				# next argument
if	[[ ! $expr ]]
then	expr=$1;shift
fi
[[ $xflag ]] || expr="*${expr}*"
noprint=$vflag$cflag$lflag		# don't print if these flags are set
integer n=0 c=0 tc=0 nargs=$#		# initialize counters
for i					# go thru the files
do	if	((nargs<=1))
	then	fname=''
	else	fname=$i:
	fi
	[[ $i ]]  &&  exec 0< $i	# open file if necessary
	while	read -r line		# read in a line
	do	(( n++ ))
		case	$line in
		$expr)			# line matches pattern
			[[ $noprint ]] || print -r -- "$fname${nflag:+$n:}$line"
			(( c++ ));;
		*)			# not a match
			if	[[ $vflag ]]
			then	print -r -- "$fname${nflag:+$n:}$line"
			fi;;
		esac
	done
	if	[[ $lflag ]] && ((c))
	then	print $i
	fi
	(( tc+=c,n=0,c=0 ))
done
[[ $cflag ]]  && print $tc		#  print count if cflag is set
exit $((tc==0))				#  set the exit value
