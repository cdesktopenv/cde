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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tttar.C /main/4 1995/10/20 16:59:46 rswiston $ 			 				
/*
 * tttar.cc - Link Service/ToolTalk object archive tool.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#if defined(__osf__) || defined(linux) || defined(CSRG_BASED)
#include <unistd.h>
#else
#if !defined(USL) && !defined(__uxp__)
#include <osfcn.h>
#endif
#endif
#include <netdb.h>
#include <locale.h>
#include <errno.h>
#include "util/tt_port.h"
#include "util/copyright.h"
#include "tt_options.h"
#include "archiver.h"

#if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP) 
#include <unistd.h>
#endif

TT_INSERT_COPYRIGHT

#ifdef OPT_PATCH
static char PatchID[] = "Patch Id: 100626_03.";
static int Patch_ID100626_03;
#endif

/*
 * Global variables
 */

char	our_process_name[80];
int	verbosity = 0;

/*
 * main()
 */
int
main(int argc, char **argv)
{
	archiver	       *tttar = new archiver( argv[0] );
	_Tt_string		tttarfile_name;
	_Tt_string		tttardir;
	bool_t			tttar_worked	= FALSE;
	bool_t			should_tttar	= FALSE;
	
	setlocale( LC_ALL, "" );

	tttar->parse_args( argc, argv );
	sprintf( our_process_name, "%s", (char *)tttar->process_name());
	verbosity = tttar->verbosity();

	if (tttar->should_tar()) {
		char *tempnam_result;
		/*
		 * Create a temporary directory to which we can
		 * chdir() in order to create or extract the
		 * tttarfile for or from the tarfile.
		 */
		tttardir = tempnam_result = tempnam(NULL, "tttardir");
		free(tempnam_result);
		if (mkdir( (char *)tttardir, S_IRWXU ) == 0) {
			tttarfile_name = tttardir.cat( "/tttarfile" );
			should_tttar = TRUE;
			if (    (tttar->mode() == EXTRACT)
			     || (tttar->mode() == LIST))
			{
				tttar->do_tar( tttardir, FALSE );
				/*
				 * Extract the tttarfile silently
				 */
				int old_verbosity = tttar->verbosity();
				Archive_mode old_mode = tttar->mode();
				tttar->verbosity_set( 0 );
				tttar->mode_set( EXTRACT );
				/*
				 * If we're not extracting from stdin, we
				 * can invoke tar(1) again.  Otherwise,
				 * do_tar() was smart enough to extract
				 * the tttarfile into the cwd the first
				 * time we called it.
				 */
				if (! tttar->only_1_look_at_tarfile()) {
					tttar->do_tar( tttardir, TRUE );
				} else if (tttar->mode() == EXTRACT) {
					/*
					 * The tttarfile was already
					 * extracted into cwd.
					 */
					tttarfile_name = "tttarfile";
				}
				tttar->verbosity_set( old_verbosity );
				tttar->mode_set( old_mode );
			}
		}
	} else {
		tttarfile_name = tttar->tarfile_arg();
		should_tttar = TRUE;
	}

	if (should_tttar) {
		tttar_worked = tttar->do_tttar( (char *)tttarfile_name,
					        tttar->should_tar() );
	}

	if (tttar->should_tar() && (tttar->mode() == CREATE)) {
		tttar->do_tar( tttardir, tttar_worked );
	}

	if (tttar->should_tar()) {
		/*
		 * Remove any temporary tttarfile.
		 */
		int status = unlink( (char *)tttarfile_name );
		/*
		 * We can't really expect an tttarfile in every tarfile,
		 * so don't complain if there isn't one to remove.
		if (status != 0) {
			fprintf( stderr, "%s: %s: %s\n",
				 our_process_name, (char *)tttarfile_name,
				 strerror(errno));
		}
		*/
		status = rmdir( (char *)tttardir );
		if (status != 0) {
			fprintf( stderr, "%s: %s: %s\n",
				 our_process_name, (char *)tttardir,
				 strerror(errno));
			exit(status);
		}
	}

	return(0);
}
