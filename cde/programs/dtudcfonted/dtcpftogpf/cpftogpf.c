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
/* $XConsortium: cpftogpf.c /main/7 1996/11/08 02:02:48 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include	<stdlib.h>
#include 	<stdio.h>
#include 	<fcntl.h>
#include 	<signal.h>
#include	<sys/types.h>
#ifndef SVR4
#if !defined( SYSV )
#include 	<sys/resource.h>
#endif
#ifdef __osf__
#define _BSD
#endif
#include	<sys/wait.h>
#ifdef __osf__
#undef _BSD
#endif
#else
#include	<wait.h>
#endif
#include	<sys/stat.h>
#include	<string.h>

#include	<unistd.h>

#include	<X11/Xmd.h>
#include	<X11/Xproto.h>

#include	"bdfgpf.h"

#include	"snfstruct.h"
#include	"fontstruct.h"

#include	"FaLib.h"
#include	"udcutil.h"

#include	<errno.h>


static	void	sigint_out() ;
static	void	put_error_and_exit();
static	void	put_help();
static	int	CnvGPFtoBDF() ;

static struct ptobhead Head;

static	char	*targ_file = NULL;	/* UDC character filename */
static	char	*com = NULL;		/* command name */
static	char	*spacing ;
static	char	*char_set ;


static void
sigint_out()
{
	if (Head.out_file) {
		Unlink_Tmpfile(Head.out_file, com);
	}
	exit(0);
}

main( argc, argv )
int	argc;
char	*argv[];
{
	FontInfoRec   *finf;
	int	help, no_infile, style_err, rtn, i;
	struct	stat	statbuf;
	char	snf_file[BUFSIZE], buf[BUFSIZE];
	int	pfd[2], fd, snf_fd, permission;
	int	exit_stat;
	char	*style ;	/* style */
	int 	chk_fd;
	pid_t	chld_pid = 0;
#if defined( SVR4 ) || defined( SYSV ) || defined(CSRG_BASED)
	int	chld_stat ;
#else
	union	wait	chld_stat ;
#endif
	int	code_no, code_area ;
	FalFontData	key ;
	char	*xlfdname, *cbuf, *ep ;
	char	*codeset = DEFAULT_CODESET ;

	/* format */
	Head.in_file = Head.bdf_file = Head.snf_file = Head.out_file = NULL;
	help = no_infile = style_err = 0;
	com = argv[0];
	COMM_SETDEFAULTSTYLE( style ) ;
	memset( &key, '\0', sizeof(FalFontData) ) ;
	xlfdname = cbuf = ep = '\0' ;
	spacing = char_set = NULL ;
	code_no = code_area = 0 ;

	if (!( bdftopcf = get_cmd_path( getenv( "PATH" ), BDFTOPCF_CMD ))) {
		bdftopcf = BDFTOPCF;
	}
	if (!( oakgtobdf = get_cmd_path( getenv( "PATH" ), SNFTOBDF_CMD ))) {
		oakgtobdf = SNFTOBDF;
	}
	if (!( bdftosnf = get_cmd_path( getenv( "PATH" ), BDFTOSNF_CMD ))) {
		bdftosnf = BDFTOSNF;
	}

	Head.code_category = ALL_CODE;
	Head.start_code = MIN_CODE;
	Head.end_code   = MAX_CODE;

	for (i=1; i<argc; i++) {
		if ( !strcmp( argv[i], "-system" ) ) {
			code_area |= SYSTM ;
		} else if ( !strcmp( argv[i], "-help" ) ) {
			help = 1;
		} else if ( !strcmp( argv[i], "-g" ) ) {
			if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ) {
				Head.snf_file = argv[++i];
			}
		} else if (!strcmp(argv[i], "-p")) {
			if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ) {
				Head.in_file = argv[++i];
			} else {
				no_infile = 1;
			}
		} else if ( !strcmp(argv[i], "-style" ) ) {
			if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ){
				style = argv[++i];
			} else {
				style_err = 1;
			}

		} else if ( !strcmp( argv[i], "-codeset" ) ) {
			if (
				( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ) &&
				( strlen( argv[i + 1] ) == 1 )
			) {
				if ( *argv[i+1] == '1' )	code_area |= CDSET1 ;
				codeset = argv[++i] ;
			} else {
				put_help( argv[0] );
				exit( 1 );
			}
		} else if ( !strcmp( argv[i], "-xlfd" ) ) {
			if ( i < argc-1 ) {
				xlfdname = argv[++i] ;
			} else {
				put_help( argv[0] );
				exit( 1 );
			}
		} else {
			put_help( argv[0] );
			exit( 1 );
		}
	}

	ep = (char *)strchr( codeset, '\0' ) ;
	code_no = (int)strtol( codeset, &cbuf, 10 ) ;
	if( cbuf == codeset || cbuf != ep ) {
	    USAGE1("%s : The code set number is not right.\n", argv[0] ) ;
	    exit(1) ;
	}

	if ( help ) {
		put_help( argv[0] );
		exit( 0 );
	}

	if ( no_infile ) {
		USAGE1("%s : The input file name following -p option cannot be omitted.\n", argv[0] );
		exit( 1 );
	}

	if ( style_err ) {
		USAGE1("%s : The style is not specified.\n", argv[0] );
		exit( 1 );
	}

	if ( Head.snf_file == NULL && xlfdname == '\0' ) {
		if ( code_area & SYSTM ) {
			USAGE1("%s : The SNF output file name cannot be omitted.\n", argv[0] );
		} else {
			USAGE1("%s : The character size specification cannot be omitted.\n", argv[0] );
		}
		exit( 1 );
	}

	if ( !( code_area & SYSTM ) ) {


	    if( xlfdname ) { 
		if( GetUdcFileName( com, code_no, xlfdname, snf_file ) ) {
		    USAGE1("%s : The font file name cannot be obtained. Terminates abnormally.\n", com );
		    exit( 1 );
		}
		Head.snf_file = snf_file;
	    } else { 
		switch ( GetFileName( argv[0], Head.snf_file, style, code_no, snf_file ) ) {
		case	0:
		    Head.snf_file = snf_file;
		    break;
		case	-1:
		    /* output GetFileName() */
		    exit( 1 );
		default:
		    USAGE1("%s : The font file name cannot be obtained. Terminates abnormally.\n", argv[0] );
		    exit( 1 );
		}
	    } 
	}

	/*
	 * refuse proportional fonts
	 */
	if ( GetUdcFontName( Head.snf_file, NULL, &xlfdname ) ) {
	    USAGE1("%s : This font cannot get XLFD. Terminates abnormally.\n", argv[0]);
	    exit( 1 );
	}
	GETSPACINGSTR( spacing, xlfdname ) ;
	if ( !strcmp( "p", spacing ) || !strcmp( "P", spacing ) ) {
	    USAGE2("%s cannot edit proportional fonts.(SPACING \"%s\")\n", argv[0], spacing );
	    exit( 1 );
	}
	GETCHARSETSTR( char_set, xlfdname ) ;

	COMM_SET_CODECATEGORY( char_set, code_area, Head) ;

	if ( ( targ_file = GetRealFileName( Head.snf_file ) ) == NULL ){
		USAGE2("%s : It was not possible to refer to the substance of the font file. \"%s\"\n", argv[0], Head.snf_file );
		exit( 1 );
	}


	if ( ( chk_fd = open( targ_file, O_RDWR ) ) < 0  ) {
		USAGE2("%s : The font file of substance \"%s\" cannot be opened.\n", argv[0] , targ_file );
	exit( 1 );
	}
	if ( isLock( chk_fd ) == 1 ) {
		USAGE1("%s : Editing by other application.\n", argv[0] );
		close( chk_fd );
		exit( 1 );
	}
	close( chk_fd );

	/* 
	 * open input file(CPF) 
	 */
	if ( Head.in_file == NULL ) {
		Head.input = stdin;
	} else {
		if ( ( Head.input = fopen( Head.in_file, "r" ) ) == NULL ) {
			put_error_and_exit( &Head, GPF_OPEN_IN, argv[0] );
		}
	}

	/* 
	 * get GPF filename
	 */
	if ( !( Head.out_file = GetTmpPath( targ_file ) ) ) {
		put_error_and_exit( &Head, MKTMP_ERROR, argv[0] );
	}

	permission = 0;
	if ( ( snf_fd = open( Head.snf_file, O_RDONLY ) ) >= 0 ) {
		if ( ChkPcfFontFile( Head.snf_file) ) {
			/* snf file */
			COMM_SNF_FILEVERSION( snf_fd, finf, buf, permission ) ;
			if( permission < 0 ) {
				put_error_and_exit( &Head, BDF_INVAL, argv[0] );
			}
		} else {
			/* pcf */
			close( snf_fd );
		}

	} else {
		put_error_and_exit( &Head, BDF_OPEN_IN, argv[0] );
	}


	if ( pipe( pfd ) != 0 ) {
		put_error_and_exit( &Head, PIPE_ERROR, argv[0] );
	}

	signal( SIGHUP , (void(*)())sigint_out );
	signal( SIGINT , (void(*)())sigint_out );
	signal( SIGQUIT, (void(*)())sigint_out );
	signal( SIGTERM, (void(*)())sigint_out );

	switch ( chld_pid = fork() ) {
	case	0:
		if ( ( fd = open( Head.out_file, O_WRONLY | O_CREAT, 0664 ) ) < 0 ) {
			put_error_and_exit( &Head, BDF_OPEN_OUT, argv[0] );
		}
		close(0);
		if ( dup( pfd[0] ) < 0 ) {
			put_error_and_exit( &Head, DUP_ERROR, argv[0] );
		}
		close( pfd[0] );
		close( pfd[1] );
		close( 1 );
		if ( dup( fd ) < 0 ) {
			put_error_and_exit( &Head, DUP_ERROR, argv[0] );
		}


		if ( ChkPcfFontFile( Head.snf_file ) == 0 ) {
			execl( bdftopcf, bdftopcf, 0 );
			/* exec error */
			put_error_and_exit( &Head, PCFFONTC_ERROR, argv[0] );
		}

		COMM_SNF_EXECLBDFTOSNF( permission, buf, Head.snf_file ) ;
		/* exec error */
		put_error_and_exit( &Head, SNFFONTC_ERROR, argv[0] );

	case	-1:
		/* fork() error */
		put_error_and_exit( &Head, FORK_ERROR, argv[0] );

	default:
		break;
	}

	close( pfd[0] );
	if ( ( Head.output = fdopen( pfd[1], "w" ) ) == NULL ) {
		close( pfd[1] );
		kill( chld_pid, SIGKILL );
		WaitID( chld_pid, chld_stat ) ;
		put_error_and_exit( &Head, FDOPEN_ERROR, argv[0] );
	}

	if ( ( rtn = CnvGPFtoBDF( &Head ) ) ) {
		fclose( Head.input );
		fclose( Head.output );
		close( pfd[1] );
		kill( chld_pid, SIGKILL );
		WaitID( chld_pid, chld_stat ) ;
		Unlink_Tmpfile( Head.out_file, argv[0] );
		put_error_and_exit( &Head, rtn, argv[0] );
	}
	fclose( Head.output );
	close( pfd[1] );
	wait( &exit_stat );
#if !defined( SVR4 ) && !defined( SYSV ) && !defined(__FreeBSD__)
	if ( !WIFEXITED(exit_stat) ) {
#else
	if (! ( WIFEXITED(exit_stat) && !WEXITSTATUS(exit_stat) ) ) {
#endif
		USAGE3("%s: The error occurred by %s (%08x).\n", argv[0], bdftopcf, exit_stat );
		Unlink_Tmpfile( Head.out_file, argv[0] );
		exit( 1 );
	}
	fclose( Head.input );
	signal( SIGHUP , SIG_IGN );
	signal( SIGINT , SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGTERM, SIG_IGN );

	sleep(1) ;
	if ( (stat( Head.out_file,&statbuf ) ) ||
	    ( statbuf.st_size == 0 ) ) {
		Unlink_Tmpfile( Head.out_file, argv[0] );
		put_error_and_exit( &Head, BDF_WRITE, argv[0] );
	}
	if ( stat( targ_file, &statbuf ) ) {
		Unlink_Tmpfile( Head.out_file, argv[0] );
		put_error_and_exit( &Head, BDF_WRITE, argv[0] );
	}

	exit ( Make_NewFefFile( targ_file, Head.out_file,
	    FONT_FILE_PARM, (uid_t)statbuf.st_uid, (gid_t)statbuf.st_gid, argv[0]) );
}


static
CnvGPFtoBDF( head )
struct ptobhead *head;
{
	int	    rtn;
	char    textbuf[BUFSIZE];

	if ( ( rtn = ReadGpfHeader( head, textbuf ) ) ) {
		return	rtn;
	}

	if ( ( rtn = WriteBdfHeader( head ) ) ) {
		return	rtn;
	}

	if ( ( head->p_width != head->bdf_width ) ||
		( head->p_height != head->bdf_height ) 
	) {
		head->zoomf = 1;
	} else {
		head->zoomf = 0;
	}

	if ( ( head->code = (int *)malloc( sizeof(int) * head->num_chars ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	if ( ( head->ptn = (char **)malloc( sizeof(char *)*head->num_chars ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	if ( ( rtn = ReadGpfToMemory(head, textbuf ) ) ) {
		return	rtn;
	}

	if ( ( rtn = WritePtnToBdf( head ) ) ) {
		return	rtn;
	}

	return	0;
}


static	void
put_error_and_exit( ptob, er_no, prog_name )
struct ptobhead *ptob;
int    er_no;
char   *prog_name;
{
	ErrMsgTable_AndExit( er_no, ptob->snf_file, ptob->out_file,
	    ptob->in_file,  NULL,
	    ptob->bdf_file,
	    prog_name
	    );
	return;
}


static	void
put_help( prog_name )
char	*prog_name;
{
	USAGE1("Usage: %s -xlfd xlfd_name\n", prog_name);
	USAGE("\t\t[-g character_size][-p character_pattern_file_name]\n" );
	USAGE("\t\t [-style style] \n");
	COMM_HELP_MSG ;
	USAGE("The character size may be obtained using the dtlsgpf command.\n");
	USAGE("\n");
}
