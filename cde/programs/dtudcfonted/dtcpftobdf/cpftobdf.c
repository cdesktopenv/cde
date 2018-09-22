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
/* $XConsortium: cpftobdf.c /main/6 1996/11/08 02:02:03 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include 	<stdio.h>
#include 	<signal.h>
#include	<string.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	"bdfgpf.h"
#include	"udcutil.h"

static void	put_error(struct ptobhead *head, int er_no, char *prog_name);
static void	put_help(char *prog_name);
static void	Usage(char *prog_name);
static int	CnvGPFtoBDF(struct ptobhead *head) ;
static void	sigint_out(void) ;

static struct ptobhead Head;

static void
sigint_out(void)
{
	if ( Head.out_file ) {
		unlink( Head.out_file );
	}
	exit( 0 );
}

int main( int argc, char *argv[])
{
	int 	rtn, i;
	char	*GetTmpPath();
	char	*spacing ;
	char	*xlfdname ;

	for ( i = 1; i < argc; i++ ) {
		if ( !strcmp( argv[i], "-help" ) ) {
			put_help( argv[0] );
			exit( 0 );
		}
	}

	if ( !( argc % 2 ) ) {
		Usage(argv[0]);
	}

	spacing = xlfdname = NULL ;
	Head.in_file = Head.bdf_file = Head.snf_file = Head.out_file = NULL;
	Head.start_code = MIN_CODE ;
	Head.end_code   = MAX_CODE ;
	Head.code_category = ALL_CODE;

	if (!( bdftopcf = get_cmd_path( getenv( "PATH"), BDFTOPCF_CMD ))){
		bdftopcf = BDFTOPCF;
	}
	if (!( oakgtobdf = get_cmd_path( getenv( "PATH"), SNFTOBDF_CMD ))) {
		oakgtobdf = SNFTOBDF;
	}

	for ( i = 1; i < argc; i+=2 ) {
		if ( !strcmp( argv[i], "-bdf" ) ) {
			Head.bdf_file = argv[ i+1 ];
		} else if ( !strcmp( argv[i], "-p" ) ) {
			Head.in_file = argv[i+1];
		} else if ( !strcmp( argv[i], "-g" ) ) {
			Head.snf_file = argv[i+1];
		} else {
			Usage( argv[0] );
		}
	}

	if ( Head.in_file == NULL ) {
		Head.input = stdin;
	} else {
		if ( ( Head.input = fopen( Head.in_file, "r" ) ) == NULL ) {
			USAGE2("%s: The character pattern file cannot be opened. \"%s\"\n", argv[0], Head.in_file );
			exit( GPF_OPEN * (-1) );
		}
	}

	signal( SIGHUP, (void(*)())sigint_out );
	signal( SIGINT, (void(*)())sigint_out );
	signal( SIGQUIT, (void(*)())sigint_out );
	signal( SIGTERM, (void(*)())sigint_out );

	if ( Head.bdf_file == NULL ) {
		if ( Head.snf_file == NULL ) {
			USAGE1("%s: If the output is stdout, the user defined font file name cannot be omitted.\n", argv[0]);
			exit(PARAM_ERROR * (-1));
		}
		Head.output = stdout;
	} else {
		if ( ! ( Head.out_file = GetTmpPath( Head.bdf_file ) ) ) {
			exit( FATAL_ERROR * ( -1 ) );
		}

		if ( ( Head.output = fopen( Head.out_file, "w" ) ) == NULL ) {
			USAGE2("%s: The work file for the output cannot be opened. \"%s\"\n", argv[0], Head.out_file );
			exit( BDF_OPEN * (-1) );
		}
	}
	/* refuse proportional fonts */
	if ( GetUdcFontName( Head.snf_file, Head.bdf_file, &xlfdname ) ) {
	    USAGE1("%s : This font cannot get XLFD. Terminates abnormally.\n", argv[0]);
	    if ( Head.in_file != NULL ) {
		fclose( Head.input );
	    }
	    if ( Head.out_file != NULL ) {
		fclose( Head.output );
		unlink( Head.out_file );
	    }
	    exit( 1 );
	}
	GETSPACINGSTR( spacing, xlfdname ) ;
	if ( !strcmp( "p", spacing ) || !strcmp( "P", spacing ) ) {
	    USAGE2("%s cannot edit proportional fonts.(SPACING \"%s\")\n", argv[0], spacing );
	    if ( Head.in_file != NULL ) {
		fclose( Head.input );
	    }
	    if ( Head.out_file != NULL ) {
		fclose( Head.output );
		unlink( Head.out_file );
	    }
	    exit( 1 );
	}

	if ( ( rtn = CnvGPFtoBDF( &Head ) ) ) {
		if ( Head.in_file != NULL ) {
			fclose( Head.input );
		}
		if ( Head.out_file != NULL ) {
			fclose( Head.output );
			unlink( Head.out_file );
		}
		put_error( &Head, rtn, argv[0] );
		exit( rtn * (-1) );
	}

	if ( Head.in_file != NULL ) {
		fclose( Head.input );
	}
	if ( Head.out_file != NULL ) {
		fclose( Head.output );
		signal( SIGHUP, SIG_IGN );
		signal( SIGINT, SIG_IGN );
		signal( SIGQUIT, SIG_IGN );
		signal( SIGTERM, SIG_IGN );
		if ( Head.bdf_file != NULL ) {
			Link_NewFile( Head.out_file, Head.bdf_file );
			unlink( Head.out_file );
		}
	}
	exit( 0 );
}

static
CnvGPFtoBDF(struct ptobhead *head)
{
	char    textbuf[BUFSIZE] ;
	int	rtn ;

	if ( ( rtn = ReadGpfHeader( head, textbuf ) ) ) {
		return	rtn;
	}

	if ( ( rtn = WriteBdfHeader( head ) ) ) {
		return	rtn;
	}

	if ( ( head->p_width != head->bdf_width )
		|| ( head->p_height != head->bdf_height )
	) {
		head->zoomf = 1;
	} else {
		head->zoomf = 0;
	}

	if ( ( head->code = (int *)malloc( sizeof(int) * head->num_chars ) ) == NULL ) {
		return	FATAL_ERROR;
	}

	if ( ( head->ptn = (char **)malloc( sizeof( char * ) * head->num_chars ) ) == NULL ) {
		return	FATAL_ERROR;
	}

	if ( ( rtn = ReadGpfToMemory( head, textbuf ) ) ) {
		return	rtn;
	}

	if ( ( rtn = WritePtnToBdf( head ) ) ) {
		return	rtn;
	}

	return	0;
}

static void
put_error(struct ptobhead *head, int er_no, char *prog_name)
{
	switch( er_no ) {
	case	FATAL_ERROR:
		USAGE1("%s: Failed convert.\n", prog_name );
		break;

	case	BDF_OPEN:
		USAGE2("%s: The BDF file cannot be opened. \"%s\"\n", prog_name, head->bdf_file );
		break;

	case	BDF_READ:
		USAGE2("%s: The BDF file cannot be read. \"%s\"\n", prog_name, head->bdf_file );
		break;

	case BDF_WRITE:
		USAGE2("%s: It is not possible to write to the BDF file. \"%s\"\n", prog_name, head->out_file );
		break;

	case	BDF_INVAL:
		if ( head->snf_file ) {
			USAGE2("%s:  The information from the font file cannot be obtained.\"%s\"\n", prog_name, head->snf_file );
		} else {
			USAGE2("%s: The format of the BDF file is illegal. \"%s\"\n", prog_name, head->bdf_file );
		}
		break;

	case	GPF_OPEN:
		USAGE2("%s:  The character pattern file cannot be opened. \"%s\"\n", prog_name, head->in_file );
		break;

	case	GPF_READ:
		USAGE2("%s:  The character pattern file cannot be read. \"%s\"\n", prog_name, head->in_file );
		break;

	case	GPF_INVAL:
		USAGE2("%s:  The format of the character pattern file is illegal. \"%s\"\n", prog_name, head->in_file );
		break;

	default:
		break;
	}
}

static	void
put_help( char *prog_name)
{
	USAGE1("Usage: %s [-bdf BDF_file_name] [-p chracter_pattern_file_name]\n", prog_name);
	USAGE("\t\t[-g user_defined_character_file_name] [-help]\n");
}

static	void
Usage( char *prog_name)
{
	put_help( prog_name );
	exit( PARAM_ERROR * (-1) );
}
