/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $TOG: isgarbage.c /main/5 1998/04/10 08:04:25 mgreess $ 			 				 */
#include "isam_impl.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int
isgarbage(char * isfname)
{
	char			isfname2[MAXPATHLEN];
	int			isfd = -1, isfd2 = -1;
	char			buffer[ISMAXRECLEN];
	char			*recbuf;
	struct dictinfo 	info;
	struct keydesc		keybuf;
	struct stat		statbuf;
	int			count = 0,i;

	(void)strcpy(isfname2, isfname);
	(void)strcat(isfname2, "~");

	if ((isfd = isopen(isfname, ISEXCLLOCK + ISINPUT)) == ISERROR) {
		goto ERROR;
	}

	if (isindexinfo(isfd, (struct keydesc *)&info, 0) == ISERROR) {
		goto ERROR;
	}

	if (strlen(isfname) + 5 >= ISMAXRECLEN)
	  recbuf = (char*) malloc(strlen(isfname) + 5);
	else
	  recbuf = buffer;

	sprintf(recbuf, "%s.rec", isfname);
	if (stat(recbuf, &statbuf) < 0) {
		goto ERROR;
	}

	iserase(isfname2);	/* Delete any old backup (~) files. */

	if ((isfd2 = isbuild(isfname2, info.di_recsize, nokey,
		     (DICTVARLENBIT&info.di_nkeys?ISVARLEN: ISFIXLEN)
			     + ISEXCLLOCK + ISINOUT)) == ISERROR) {
		goto ERROR;
	}

	/* Copy all records */

	while (isread(isfd, recbuf, ISNEXT) == ISOK) {
		iswrite(isfd2, recbuf);
		count++;
	}

	if (count != info.di_nrecords) {
		goto ERROR;
	}
	
	/* Build all indexes from index info */

	for (i = 1; i <= (info.di_nkeys & DICTNKEYSMASK); i++) {
		if (isindexinfo(isfd, &keybuf, i) == ISERROR) {
			goto ERROR;
		}
		if (i == 1 && keybuf.k_nparts != 0) {
			/* Add primary index */
			if (isaddprimary(isfd2, &keybuf) == ISERROR) {
				goto ERROR;
			}
		} 
		else if (i > 1) {
			/* Add secondary index */
			if (isaddindex(isfd2, &keybuf) == ISERROR) {
				goto ERROR;
			}
		}
	}

	(void)isclose(isfd);
	(void)isclose(isfd2);

	iserase(isfname);		/* cannot abort at this point ! */
	if (isrename(isfname2, isfname) == ISERROR) {
		return (ISERROR);
	}

	return (ISOK);

      ERROR:

	if (isfd != -1)
	  (void)isclose(isfd);

	if (isfd2 != -1) {
		(void)isclose(isfd2);
		(void)iserase(isfname2);
	}
	if (recbuf != buffer) free(recbuf);
	return (ISERROR);
}

