: $XConsortium: options.sh /main/2 1996/05/10 16:42:10 drk $

: check for shell magic #!
: include OPTIONS
eval $1
shift
OPTIONS=$1
cat > /tmp/file$$ <<!
#! /bin/echo
exit 1
!
chmod 755 /tmp/file$$
if	/tmp/file$$ > /dev/null
then	echo "#define SHELLMAGIC	1"
fi
rm -f /tmp/file$$
: see whether or not in the ucb universe
if	test -f /bin/universe && univ=`/bin/universe` > /dev/null 2>&1
then	if	test ucb = "$univ"
	then	echo "#define SHOPT_UCB	1"
	fi
fi
if	test -d /dev/fd
then	echo "#define SHOPT_DEVFD	1"
fi
: Check for VPIX
if	test -f /usr/bin/vpix
then	echo "#define SHOPT_VPIX	1"
fi

: get the option settings from the options file
if [ -z "$OPTIONS" ] ; then
  OPTIONS="OPTIONS"
fi
. $OPTIONS
for i in ACCT ACCTFILE BRACEPAT CRNL DYNAMIC ECHOPRINT ESH FS_3D \
	JOBS KIA MULTIBYTE OLDTERMIO OO P_SUID RAWONLY \
	SEVENBIT SPAWN SUID_EXEC TIMEOUT VSH
do	: This could be done with eval, but eval broken in some shells
	j=0
	case $i in
	ACCT)		j=$ACCT;;
	ACCTFILE)	j=$ACCTFILE;;
	BRACEPAT)	j=$BRACEPAT;;
	CRNL)		j=$CRNL;;
	DYNAMIC)	j=$DYNAMIC;;
	ECHOPRINT)	j=$ECHOPRINT;;
	ESH)		j=$ESH;;
	FS_3D)		j=$FS_3D;;
	JOBS)		j=$JOBS;;
	KIA)		j=$KIA;;
	MULTIBYTE)	j=$MULTIBYTE;;
	OLDTERMIO)	echo "#include <sys/termios.h>" > /tmp/dummy$$.c
			echo "#include <sys/termio.h>" >>/tmp/dummy$$.c
			if	$cc -E /tmp/dummy$$.c > /dev/null 2>&1
			then	j=$OLDTERMIO
			fi
			rm -f dummy$$.c;;
	OO)		j=$OO;;
	P_SUID)		j=$P_SUID;;
	RAWONLY)	j=$RAWONLY;;
	SEVENBIT)	j=$SEVENBIT;;
	SPAWN)		j=$SPAWN;;
	SUID_EXEC)	j=$SUID_EXEC;;
	TIMEOUT)	j=$TIMEOUT;;
	VSH)		j=$VSH;;
	esac
	case $j in
	0|"")	;;
	*)	echo "#define SHOPT_$i	$j" ;;
	esac
done
cat <<\!
#if (MB_LEN_MAX-1)<=0 || !defined(_lib_mbtowc)
#   undef SHOPT_MULTIBYTE
#endif
!
