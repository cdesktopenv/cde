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
/* $TOG: main.C /main/7 1998/10/26 17:19:37 mgreess $ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include "dmx.hh"
#include <locale.h>
#include <sys/param.h>
// #include <Dt/DtNlUtils.h>
#ifdef __cplusplus
extern "C" {
#endif
    void Dt_nlInit( void );
#ifdef __cplusplus
}
#endif
#include <Dt/EnvControlP.h>
#include <Dt/DtPStrings.h>

/*
 * globals
 */

gid_t       _originalEgid;    // startup effective gid
gid_t       _originalRgid;    // startup real gid

void   
enableGroupPrivileges(void *)
{
    if(-1 == setgid(_originalEgid)) {
        fprintf(stderr, "Failed to enable group priviledges\n");
    }
}

void   
disableGroupPrivileges(void *)
{
    if(-1 == setgid(_originalRgid)) {
        fprintf(stderr, "Failed to disable group priviledges\n");
    }
}

int
main (int argc, char **argv)
{
	// parse command-line options
	int	c;
	extern char *optarg;
	extern int optind;
	char	*ffile = NULL;
	int	aflag = 0, pgflag = 0;
	int	errflag = 0;

        // we have to be set-gid to group "mail" when opening and storing
        // folders.  But we don't want to do everything as group mail.
        // here we record our original gid, and set the effective gid
        // back the the real gid.  We'll set it back when we're dealing
        // with folders...
        //
        _originalEgid = getegid();      // remember effective group ID
        _originalRgid = getgid();       // remember real group ID
        disableGroupPrivileges((void *)0); // disable group privileges from
					   // here on

	/*
	 * To make DtDts*() function correctly, we have to call
	 * DtInitialize() or DtAppInitialize(). But they require Widget....
	 * Instead, just call Dt_nlInit() that is an internal function of
	 * libDtSvc.a. This is a temporary hack.....
	 */
	_DtEnvControl(DT_ENV_SET);
	setlocale( LC_ALL, "" );
	Dt_nlInit();

	while ((c = getopt(argc, argv, "f:aph?")) != EOF)
		switch (c)
		{
			case 'p':
				pgflag++;
				//printf ("Print each message on its own page\n");
				break;
			case 'a':
				aflag++;
				//printf ("Strip attachments\n");
				break;
			case 'f':
				ffile = optarg;
				//printf ("Input file is: %s\n", ffile);
				break;
			case '?':
			case 'h':
				errflag++;
				break;
			default:	
				errflag++;
		}
	if (errflag)
	{
		fprintf(stderr, "usage: %s [-p] [-a] [-f <filename>] \n", argv[0]);
		exit (1);
	}

	//for ( ; optind < argc; optind++)
		//(void)printf("%s\n", argv[optind]);
	

	// create DtMail session

	DtMailEnv		dmxenv;
	DtMail::Session		*session;

	DmxMailbox		mbox;

	session = new DtMail::Session (dmxenv, "dtmailpr");


	if (handleError (dmxenv, "new session") == B_TRUE)
		exit (1);

	if (session == NULL)
	{
		fprintf (stderr, "Error opening session...exiting.\n");
		exit (1);
	}

        // Register all callbacks the backend may have to deal with
        session->registerDisableGroupPrivilegesCallback(disableGroupPrivileges,
							(void *)0);
        session->registerEnableGroupPrivilegesCallback(enableGroupPrivileges,
							(void *)0);

	// initialize typing system (will go away eventually)
	DtDtsLoadDataTypes ();

	// temporary hack, until I'm sure that buffer objects are working
	char buf [BUFSIZ];
	int n = 0;
	char 	*name;
	FILE	*msgFile=NULL;

	if (ffile == NULL)
	{
                static char *tmpdir = new char[MAXPATHLEN+1];

                sprintf(
			tmpdir,
			"%s/%s",
			getenv("HOME"),
			DtPERSONAL_TMP_DIRECTORY);

	        name = tempnam(tmpdir, "dtmpr");
		if ((msgFile = fopen (name, "w+")) == NULL)
		{
			perror ("tmpfile");
			exit (1);
		}

		while ( (n = read (fileno (stdin), buf, BUFSIZ)) > 0)
		{
			if (write (fileno (msgFile), buf, n) != n)
			{
				perror ("write");
				exit (1);
			}
		}

		if (n < 0)
		{
			perror ("read");
		}
	
	        fclose (msgFile);
		delete [] tmpdir;

	} else {
		name = ffile;
	}
	DtMail::MailBox		*mailbox = NULL;

	// try to construct mbox
	mailbox = session->mailBoxConstruct (
				dmxenv,
				DtMailFileObject,
				name,
				NULL,
				NULL,
				NULL);

	if (handleError (dmxenv, "new DtMail::MailBox") == B_TRUE)
		exit (1);

	mbox.name = new char [strlen (name) +1];
	strcpy (mbox.name, name);

	mbox.mbox = mailbox;
	mbox.loadMessages ();

	int m = 0;

	for (m = 1; m <= mbox.messageCount; m++)
	{
		mbox.msg[m].getFlags ();
		mbox.msg[m].display ();
		if (m < mbox.messageCount) {
			if (pgflag) {
				printf ("");
			} else {
				printf ("\n\n");
			}
		}
	}

	return 0;
}
