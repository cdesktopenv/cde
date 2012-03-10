#include <stdlib.h>
#include <stdio.h>
#include "LocaleXlate.h"
#include "XlationSvc.h"

int
main()
{
_DtXlateDb  myDb = NULL;
char        myPlatform[_DtPLATFORM_MAX_LEN+1];
char       *locale;
char       *newLocale;
int         execVer;
int         compVer;

locale = getenv("LANG");
if (!locale || (strcmp(locale, "C") == 0) || (strcmp(locale, "POSIX") == 0)) {
     puts("C.ISO-8859-1");
     exit(0);
}

if ((_DtLcxOpenAllDbs(&myDb) != 0) ||
    (_DtXlateGetXlateEnv(myDb,myPlatform,&execVer,&compVer) != 0)) {
    fprintf(stderr,
            "Warning: could not open locale translation database.\n");
    exit(1);
}

if (_DtLcxXlateOpToStd(myDb,
			myPlatform,
			execVer,
			DtLCX_OPER_SETLOCALE,
			locale,
			&newLocale,
			NULL,
			NULL,
			NULL)) {
	fprintf(stderr,
		"Warning: could not translate local to CDE locale\n");
}

puts(newLocale);

_DtLcxCloseDb(&myDb);
}
