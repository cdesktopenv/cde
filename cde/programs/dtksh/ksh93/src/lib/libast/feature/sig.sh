# $XConsortium: sig.sh /main/2 1995/07/19 17:13:42 drk $
: generate sig features
: include FEATURE/signal
eval $1
shift
echo "int xxx;" > $tmp.c
$cc -c $tmp.c >/dev/null 2>$tmp.e
echo "#pragma prototyped
#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:hide kill killpg
#else
#define kill	______kill
#define killpg	______killpg
#endif
#include <signal.h>
#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:nohide kill killpg
#else
#undef	kill
#undef	killpg
#endif
#ifndef sigmask
#define sigmask(s)	(1<<((s)-1))
#endif"
echo "#include <signal.h>
#ifdef TYPE
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
typedef TYPE (*Handler_t)(int);
#else
typedef TYPE (*Handler_t)();
#endif
#endif
Handler_t f()
{
	Handler_t	handler;
	handler = signal(1, SIG_IGN);
	return(handler);
}" > $tmp.c
if	$cc -c $tmp.c >/dev/null
then	:
else	e=`wc -l $tmp.e`
	for i in void int
	do	$cc -c -DTYPE=$i $tmp.c >/dev/null 2>$tmp.e || continue
		case `wc -l $tmp.e` in
		$e)	break ;;
		esac
	done
	echo "typedef $i (*Handler_t)(int);"
fi
case $# in
0)	;;
*)	cat $* | grep '^#define [^_]' ;;
esac
echo '
extern char*	sig_name[];
extern char*	sig_text[];

extern int	kill(pid_t, int);
extern int	killpg(pid_t, int);
extern int	sigunblock(int);'
