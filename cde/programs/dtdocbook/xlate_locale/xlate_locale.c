/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
#include <stdlib.h>
#include <stdio.h>
#include "LocaleXlate.h"
#include "XlationSvc.h"

int
main(void)
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
