: $XConsortium: preroot.sh /main/2 1996/05/10 16:43:41 drk $
: generate preroot features
eval $1
shift
if	/etc/preroot / /bin/echo >/dev/null
then	cat <<!
#pragma prototyped

#define FS_PREROOT	1			/* preroot enabled	*/
#define PR_BASE		"CCS"			/* preroot base env var	*/
#define PR_COMMAND	"/etc/preroot"		/* the preroot command	*/
#define PR_REAL		"/dev/.."		/* real root pathname	*/
#define PR_SILENT	"CCSQUIET"		/* no command trace	*/

extern char*		getpreroot(char*, const char*);
extern int		ispreroot(const char*);
extern int		realopen(const char*, int, int);
extern void		setpreroot(char**, const char*);

!
else	echo "/* preroot not enabled */"
fi
