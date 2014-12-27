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
/* (c) Copyright 1997 The Open Group */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 * xdm - display manager daemon
 *
 * $TOG: policy.c /main/6 1997/03/14 13:45:03 barstow $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/*
 * policy.c.  Implement site-dependent policy for XDMCP connections
 */

# include "dm.h"

static ARRAY8 noAuthentication = { (CARD16) 0, (CARD8Ptr) 0 };
static ARRAY8 loopbackAddress  = { (CARD16) 0, (CARD8Ptr) 0 };

typedef struct _XdmAuth {
    ARRAY8  authentication;
    ARRAY8  authorization;
} XdmAuthRec, *XdmAuthPtr;

XdmAuthRec auth[] = {
#ifdef HASXDMAUTH
{ {(CARD16) 20, (CARD8 *) "XDM-AUTHENTICATION-1"},
  {(CARD16) 19, (CARD8 *) "XDM-AUTHORIZATION-1"},
},
#endif
{ {(CARD16) 0, (CARD8 *) 0},
  {(CARD16) 0, (CARD8 *) 0},
}
};

#define NumAuth	(sizeof auth / sizeof auth[0])



/***************************************************************************
 *
 *  Local procedure declarations
 *
 ***************************************************************************/

static char * WillingMsg( void ) ;


/********    End Local Function Declarations    ********/





ARRAY8Ptr 
ChooseAuthentication( ARRAYofARRAY8Ptr authenticationNames )
{
    int	i, j;

    for (i = 0; i < authenticationNames->length; i++)
	for (j = 0; j < NumAuth; j++)
	    if (XdmcpARRAY8Equal (&authenticationNames->data[i],
				  &auth[j].authentication))
		return &authenticationNames->data[i];
    return &noAuthentication;
}

int 
CheckAuthentication( struct protoDisplay *pdpy, ARRAY8Ptr displayID,
        	     ARRAY8Ptr name, ARRAY8Ptr data )
{
#ifdef HASXDMAUTH
    if (name->length && !strncmp (name->data, "XDM-AUTHENTICATION-1", 20))
	return XdmCheckAuthentication (pdpy, displayID, name, data);
#endif
    return TRUE;
}

int 
SelectAuthorizationTypeIndex( ARRAY8Ptr authenticationName,
			      ARRAYofARRAY8Ptr authorizationNames )
{
    int	i, j;

    for (j = 0; j < NumAuth; j++)
	if (XdmcpARRAY8Equal (authenticationName,
			      &auth[j].authentication))
	    break;
    if (j < NumAuth)
    {
    	for (i = 0; i < authorizationNames->length; i++)
	    if (XdmcpARRAY8Equal (&authorizationNames->data[i],
				  &auth[j].authorization))
	    	return i;
    }
    for (i = 0; i < authorizationNames->length; i++)
	if (ValidAuthorization (authorizationNames->data[i].length,
				(char *) authorizationNames->data[i].data))
	    return i;
    return -1;
}

#if 0
int 
Willing( struct sockaddr *addr, int addrlen, ARRAY8Ptr authenticationName,
	 ARRAY8Ptr status )
#endif

int 
Willing(
        ARRAY8Ptr addr,
#if NeedWidePrototypes
        int connectionType,
#else
        CARD16 connectionType,
#endif /* NeedWidePrototypes */
        ARRAY8Ptr authenticationName,
        ARRAY8Ptr status,
        xdmOpCode type )
{
    static char	statusBuf[256];
    int		ret;
#if 0
    extern char	*localHostname ();
#endif
    ret = AcceptableDisplayAddress (addr, connectionType, type);
    if (!ret)
	snprintf (statusBuf, sizeof(statusBuf), "Display not authorized to connect");
    else
	snprintf (statusBuf, sizeof(statusBuf), "%s", WillingMsg());
#if 0
    	snprintf (statusBuf, sizeof(statusBuf), "host %s", localHostname());
#endif
    /* enforce termination */
    statusBuf[255] = '\0';

    status->length = strlen(statusBuf);
    status->data = (CARD8Ptr) malloc (status->length);
    if (!status->data)
	status->length = 0;
    else
	bcopy (statusBuf, (char *)status->data, status->length);
    return ret;
}

ARRAY8Ptr 
Accept( struct sockaddr *from, int fromlen,
#if NeedWidePrototypes
        int displayNumber )
#else
        CARD16 displayNumber )
#endif /* NeedWidePrototypes */
{
    return 0;
}

int 
SelectConnectionTypeIndex( ARRAY16Ptr connectionTypes,
			   ARRAYofARRAY8Ptr connectionAddresses )
{

    int i;
    
    /*
     *  the current selection policy is to use the first connection address
     *  that is not the loopback address...
     */
     
    /*
     *  initialize loopback address array if not already done so...
     *
     */
    if (loopbackAddress.length == 0 &&
	XdmcpAllocARRAY8 (&loopbackAddress, 4) ) {

	loopbackAddress.data[0] = 127;
	loopbackAddress.data[1] = 0;
	loopbackAddress.data[2] = 0;
	loopbackAddress.data[3] = 1;
    }
    
    for (i = 0; i < connectionAddresses->length; i++) {
	if (!XdmcpARRAY8Equal (&connectionAddresses->data[i],
				&loopbackAddress))
	    break;
    }

    return i;
}




/***************************************************************************
 *
 *  WillingMsg
 *
 *  Generate a message for the "Willing" status field.
 *  
 ***************************************************************************/

# define LINEBUFSIZE 32

static char *
WillingMsg( void )
{
    static char retbuf[LINEBUFSIZE];
    char	tmpbuf[LINEBUFSIZE * 8];
    char	*cp;
    char	tmpfilename[L_tmpnam + 1];
    FILE	*f;


    /* Return selected part from an 'uptime' to Server 	*/
    /* for use in hosts status field when XDMCP broadcast is used */
    /* (useful for selecting host to be managed by) 	*/

    strcpy(retbuf, "Willing to Manage");
    
    strcpy(tmpbuf,"uptime | ");
    strcat(tmpbuf,"awk '{printf(\"%s %-.5s  load: %.3s, %.3s, %.3s\",$(NF-6),$(NF-5),$(NF-2),$(NF-1),$NF)}'");
    strcat(tmpbuf," > ");

    if ( tmpnam(tmpfilename) != (char *)NULL ) {

	strcat(tmpbuf,tmpfilename);

	if(-1 == system(tmpbuf)) {
            perror(strerror(errno));
        }

	if ((f = fopen(tmpfilename,"r")) != (FILE *) NULL) {
	    fgets(tmpbuf,LINEBUFSIZE,f);
	    if ( (cp = strchr(tmpbuf,'\n')) != NULL) 
		*cp = '\0';

	    if (strlen(tmpbuf) > 10) 	/* seems reasonable? */
    		strcpy(retbuf, tmpbuf);

	    fclose(f);
	}
	
	unlink(tmpfilename);
    }

    return (retbuf);
}
