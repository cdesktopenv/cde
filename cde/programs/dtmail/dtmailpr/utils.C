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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: utils.C /main/4 1996/04/21 19:44:10 drk $ */

/*
 *+SNOTICE
 *
 *	$:$
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include "dmx.hh"

// fn proto for mailx fn
static char * dispname(const char *hdr);

// error-handling: should do something with minor codes

boolean_t
handleError (DtMailEnv &dterror, char *msg)
{
	if (dterror.isSet () == DTM_TRUE)
	{
		fprintf (stderr, "dtmailpr: (%s) %s\n", 
			msg, (const char *)dterror);
		dterror.logError (DTM_FALSE, "dtmailpr: (%s) %s\n",
			msg, (const char *)dterror);

		dterror.clear ();
		return B_TRUE;
	}

	dterror.clear ();
	return B_FALSE;
}
	

char *
errorString (enum DmxHeaders hdr)
{
	switch (hdr)
	{
		case DMXFROM:
			return "(unknown)";
		case DMXSUBJ:
			return "(no subject)";
		case DMXCLENGTH:
			return "0";
		case DMXSTATUS:
			return "  ";
		case DMXDATE:
			return "(unknown date)";
		case DMXTO:
			return " ";
		case DMXNUMHDRS:
		default:
			return " ";
	}
}

char *
formatHeader (DtMailHeaderLine &info, enum DmxHeaderType htype)
{
	int	i = 0, length = 0;
	int	buflength = 0;
	char	*fbuf;

	const char	*header [DMXNUMHDRS];

	for (i = 0; i < DMXNUMHDRS; i++)
	{
		length = info.header_values[i].length ();
		if (length == 0)
		{
			header [i] = errorString ((enum DmxHeaders)i);
		} else {
			header [i] = *((info.header_values[i])[0]);
		}
	}

	for (i = 0; i < DMXNUMHDRS; i++)
	{
		buflength += strlen (header [i]);
	}

	fbuf = new char [buflength + 64];

	switch (htype)
	{
		case MSGLIST:
			sprintf (fbuf,
			"%-18.18s %-16.16s %4d/%-5s %-.25s",
				dispname (header [DMXFROM]),
				header [DMXDATE],
				atoi (header [DMXCLENGTH]) / 40,
				header [DMXCLENGTH],
				header [DMXSUBJ]
				);
			break;
		case MSGHEADER:
			sprintf (fbuf,
			"From: %s\nDate: %s\nTo: %s\nSubject: %s\n",
				dispname (header [DMXFROM]),
				header [DMXDATE],
				header [DMXTO],
				header [DMXSUBJ]);
			break;
		case NUMHDRTYPES:
		default:
			return "";
	}
    	return (fbuf); //need to free this after using it
}

// stuff grabbed from mailx...it's ugly, but it looks pretty

#define NOSTR ((char *) 0)    /* Nill string pointer */
#define LINESIZE 5120            /* max readable line width */
static char *phrase(char *, int , int );

/*
 * Return a pointer to a dynamic copy of the argument.
 */
// changed salloc to malloc
char *
savestr(char *str)
{
	register char *cp, *cp2, *top;

	for (cp = str; *cp; cp++)
		;
	top = (char *)malloc((unsigned)(cp-str + 1));
	if (top == NOSTR)
		return(NOSTR);
	for (cp = str, cp2 = top; *cp; cp++)
		*cp2++ = *cp;
	*cp2 = 0;
	return(top);
}


char *
skin(char *name)
{
	return phrase(name, 0, 0);
}
/*
 * Return the full name from an RFC-822 header line
 * or the last two (or one) component of the address.
 */

static char *
dispname(const char *hdr)
// made it a const char * instead of a char *
{
	char *cp, *cp2;

	if (hdr == 0)
		return 0;
	if (((cp = const_cast <char *> (strchr(hdr, '<'))) != 0) && (cp > hdr)) {
		*cp = 0;
		if ((*hdr == '"') && ((cp = const_cast <char *> (strrchr(++hdr, '"'))) != 0))
			*cp = 0;
		return (char *)hdr;
	} else if ((cp = const_cast <char *> (strchr(hdr, '('))) != 0) {
		hdr = ++cp;
		if ((cp = const_cast <char *> (strchr(hdr, '+'))) != 0)
			*cp = 0;
		if ((cp = const_cast <char *> (strrchr(hdr, ')'))) != 0)
			*cp = 0;
		return (char *)hdr;
	}
	cp = skin((char *)hdr);
	if ((cp2 = strrchr(cp, '!')) != 0) {
		while (cp2 >= cp && *--cp2 != '!');
		cp = ++cp2;
	}
	return cp;
}


#define equal(a, b)     (strcmp(a,b)==0)/* A nice function to string compare */

/*
 * Skin an arpa net address according to the RFC 822 interpretation
 * of "host-phrase."
 */
// changed salloc to malloc
static char *
phrase(char *name, int token, int comma)
{
	register char c;
	register char *cp, *cp2;
	char *bufend, *nbufp;
	int gotlt, lastsp, didq;
	char nbuf[LINESIZE];
	int nesting;

	if (name == NOSTR)
		return(NOSTR);
	if (strlen(name) >= (unsigned)LINESIZE)
		nbufp = (char *)malloc(strlen(name));
	else
		nbufp = nbuf;
	gotlt = 0;
	lastsp = 0;
	bufend = nbufp;
	for (cp = name, cp2 = bufend; (c = *cp++) != 0;) {
		switch (c) {
		case '(':
			/*
				Start of a comment, ignore it.
			*/
			nesting = 1;
			while ((c = *cp) != 0) {
				cp++;
				switch(c) {
				case '\\':
					if (*cp == 0) goto outcm;
					cp++;
					break;
				case '(':
					nesting++;
					break;
				case ')':
					--nesting;
					break;
				}
				if (nesting <= 0) break;
			}
		outcm:
			lastsp = 0;
			break;
		case '"':
			/*
				Start a quoted string.
				Copy it in its entirety.
			*/
			didq = 0;
			while ((c = *cp) != 0) {
				cp++;
				switch (c) {
				case '\\':
					if ((c = *cp) == 0) goto outqs;
					cp++;
					break;
				case '"':
					goto outqs;
				}
				if (gotlt == 0 || gotlt == '<') {
					if (lastsp) {
						lastsp = 0;
						*cp2++ = ' ';
					}
					if (!didq) {
						*cp2++ = '"';
						didq++;
					}
					*cp2++ = c;
				}
			}
		outqs:
			if (didq)
				*cp2++ = '"';
			lastsp = 0;
			break;

		case ' ':
		case '\t':
		case '\n':
			if (token && (!comma || c == '\n')) {
			done:
				cp[-1] = 0;
				return cp;
			}
			lastsp = 1;
			break;

		case ',':
			*cp2++ = c;
			if (gotlt != '<') {
				if (token)
					goto done;
				bufend = cp2 + 1;
				gotlt = 0;
			}
			break;

		case '<':
			cp2 = bufend;
			gotlt = c;
			lastsp = 0;
			break;

		case '>':
			if (gotlt == '<') {
				gotlt = c;
				break;
			}

			/* FALLTHROUGH . . . */

		default:
			if (gotlt == 0 || gotlt == '<') {
				if (lastsp) {
					lastsp = 0;
					*cp2++ = ' ';
				}
				*cp2++ = c;
			}
			break;
		}
	}
	*cp2 = 0;
	return (token ? --cp : equal(name, nbufp) ? name :
	    nbufp == nbuf ? savestr(nbuf) : nbufp);
}



