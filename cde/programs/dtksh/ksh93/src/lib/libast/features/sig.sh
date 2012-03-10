: $XConsortium: sig.sh /main/2 1996/05/10 16:43:58 drk $
: generate sig features
eval $1
shift
echo "#include <signal.h>
int xxx;" > $tmp.c
$cc -c $tmp.c >/dev/null 2>$tmp.e
echo "#pragma prototyped
#if _DLL_INDIRECT_DATA && !_DLL
#define sig_info	(*_sig_info_)
#else
#define sig_info	_sig_info_
#endif

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
typedef TYPE (*Handler_t)(ARG);
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
	do	for j in int,... ... int
		do	$cc -c -DTYPE=$i -DARG=$j $tmp.c >/dev/null 2>$tmp.e || continue
			case `wc -l $tmp.e` in
			$e)	break 2 ;;
			esac
		done
	done
	echo "typedef $i (*Handler_t)($j);"
fi
echo '
typedef struct
{
	char**		name;
	char**		text;
	int		sigmax;
} Sig_info_t;

extern Sig_info_t	sig_info;

extern int		kill(pid_t, int);
extern int		killpg(pid_t, int);
extern int		sigunblock(int);'
