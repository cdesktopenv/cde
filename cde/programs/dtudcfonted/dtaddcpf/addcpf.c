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
/* $XConsortium: addcpf.c /main/9 1996/11/08 02:00:29 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include 	<stdio.h>
#include 	<locale.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#ifndef SVR4
#if !defined( SYSV )
#include	<sys/resource.h>
#endif
#include	<sys/wait.h>
#else
#include	<wait.h>
#endif

#include	<unistd.h>
#include	<string.h>
#include	<stdlib.h>

#include	"bdfgpf.h"

#include	<X11/Xmd.h>
#include	<X11/Xproto.h>
#include	"fontstruct.h"

#include	"FaLib.h"
#include	"snfstruct.h"
#include	"udcutil.h"

#include	<errno.h>


static int	rw_init() ;
static int	readSnf() ;
static int	readSnf_with_init() ;
static int	readSnfHeader() ;
static int	readBdfHeaderAndPut() ;
static int	mergePtn() ;
static int	ModifyPtn() ;
static int	InsertPtn() ;
static int	writeSnf() ;
static void	put_error_and_exit();
static void	put_help();
static int	readBdfToMemory_with_init() ;

extern	int	fal_glyph_to_code() ;

static struct ptobhead WriteSnf;

static	char	*targ_file = NULL;	/* UDC_file_name */
static	char	*com = NULL;		/* command_name */
static	char	*spacing ;
static	char	*char_set ;
static	char	*util_locale ;

static	pid_t	gtob_pid = 0;
static	pid_t	btop_pid = 0;
#if defined( SVR4 ) || defined( SYSV ) || defined(__osf__) || defined(__FreeBSD__)
static	int	chld_stat ;
#else
static	union	wait	chld_stat ;
#endif

static void
sigint_out()
{
	if (WriteSnf.out_file) {
		Unlink_Tmpfile( WriteSnf.out_file, com );
	}
	exit( 0 );
}

main( argc, argv )
int 	argc;
char	*argv[];
{
	int 	code_area , init, modify, help, no_infile, no_style;
	int	fupd = 0 ;
	int 	init_all, rtn, i;
#ifndef ROOT_ONLY
	int 	exit_stat;
#endif
	struct btophead	ReadSnf;	/* output file (GPF) */
	struct ptobhead	ReadGpf;	/* input file (CPF) */
	struct stat	statbuf;
	char		snf_file[BUFSIZE] ;
	char		buf[BUFSIZE];
	char		*style ;
	char		*xlfdname, *cbuf ;
	int 		chk_fd;
	FalGlyphRegion	*gr ;
	int		num_gr ;
	char		*ep ;

	int 	code_no;
	char	*codeset = DEFAULT_CODESET;

	if( (util_locale = (char *)getenv( "LANG" )) == NULL ){
	    util_locale = "C" ;
	}
	ReadSnf.in_file = ReadGpf.in_file = WriteSnf.out_file = NULL;
	com = argv[0];
	COMM_SETDEFAULTSTYLE( style ) ;	
	cbuf = xlfdname = ep = '\0' ;

	gr = NULL ;
	num_gr = 0 ;
	spacing = char_set = NULL ;

	if (!( bdftopcf = get_cmd_path( getenv( "PATH" ), BDFTOPCF_CMD ))) {
		bdftopcf = BDFTOPCF;
	}
	if (!( oakgtobdf = get_cmd_path( getenv( "PATH" ), SNFTOBDF_CMD ))) {
		oakgtobdf = SNFTOBDF;
	}
	if (!( bdftosnf = get_cmd_path( getenv( "PATH" ), BDFTOSNF_CMD ))) {
		bdftosnf = BDFTOSNF;
	}

	code_area = NORMAL;
	init = modify = help = no_infile = no_style = init_all = 0;

	for ( i = 1; i < argc; i++ ) {
		if ( !strcmp( argv[i], "-g" ) ) {
			if ( ( i < argc-1) && (*argv[i+1] != '-' ) ) {
				ReadSnf.in_file = argv[++i];
			}
		} else if ( !strcmp( argv[i], "-p" ) ) {
			if ( (i < argc-1) && (*argv[i+1] != '-') ){
				ReadGpf.in_file = argv[++i];
			} else {
				no_infile = 1;
			}
		} else if ( !strcmp( argv[i], "-init" ) ) {
			init = 1;
		} else if ( !strcmp( argv[i], "-modify" ) ) {
			modify = 1;
		} else if ( !strcmp( argv[i], "-system" ) ) {
			code_area |= SYSTM;
		} else if ( !strcmp( argv[i], "-help" ) ) {
			help = 1;
		} else if ( !strcmp( argv[i], "-style" ) ) {
			if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ){
				style = argv[++i];
			} else {
				no_style = 1;
			}
		} else if ( !strcmp( argv[i], "-codeset" ) ) {
			if ( ( i < argc - 1 ) && ( *argv[i+1] != '-' )){
				codeset = argv[++i];
				COMM_SET_CODESET( codeset, code_area ) ;
			}
		} else if ( !strcmp( argv[i], "-xlfd" ) ) {
			if ( i < argc - 1 ){
				xlfdname = argv[++i];
			}
		} else if ( COMM_SBOPT_STRCMP( argv, i ) ) {
		    COMM_SBOPT_SETSBOPT( code_area ) ;
		} else if ( !strcmp( argv[i], "-f" ) ) {
			fupd = 1;
		} else {
		    put_help(argv[0]);
		    exit( 1 );
		}
	}

	/* help_message */
	if ( help == 1 ) {
		put_help( argv[0] );
		exit( 0 );
	}
	COMM_SBOPT_CHECKSBOPT( argv[0], code_area ) ;

	if ( no_infile ) {
                USAGE1("%s : The input file name following -p option cannot be omitted.\n", argv[0] );
		exit( 1 );
	}

	ep = (char *)strchr( codeset, '\0' ) ;
	code_no = (int)strtol( codeset, &cbuf, 10 ) ;
	if ( cbuf == codeset || cbuf != ep ) {
		USAGE2("%s : The codeset number is not right.¡Ê %s ¡Ë\n",
			argv[0], codeset );
		exit( 1 );
	}

	if ( no_style ) {
                USAGE1("%s : The style is not specified.\n", argv[0] ) ;
		exit( 1 );
	}

	if ( ReadSnf.in_file == '\0' && xlfdname == '\0' ) {
		if ( code_area & SYSTM ) {
                        USAGE1("%s : The GPF output file name cannot be omitted.\n", argv[0] );
		} else {	/* string of charcter size */
                        USAGE1("%s : The character size specification cannot be omitted.\n", argv[0] );
		}
		exit( 1 );
	}

	/* open GPF file */
	if ( !(code_area & SYSTM) ) {
	    if( xlfdname ) {
		if ( GetUdcFileName( argv[0], code_no, xlfdname, snf_file ) ) {
                       	USAGE1("%s : Failed to get font file. Terminates abnormally.\n", argv[0]);
			exit( 1 );
		}
		ReadSnf.in_file = snf_file ;
	    } else {
		

		switch ( GetFileName( argv[0], ReadSnf.in_file, style, code_no, snf_file ) ) {
		case 0:
			ReadSnf.in_file = snf_file;
			break;
		case -1:
			exit(1);
		default:
                        USAGE1("%s : The font file name cannot be obtained. Terminates abnormally.\n", argv[0]);
			exit(1);
		}
	    }
	    if ( GetUdcRegion( argv[0], code_no, ReadSnf.in_file, &num_gr, &gr ) ) {
		USAGE1("%s : This font cannot get UDC code region. Terminates abnormally.\n", argv[0]);
		exit( 1 );
	    }
	} else {
	    num_gr = 1 ;
	    if( (gr = (FalGlyphRegion *)malloc( sizeof(FalGlyphRegion) * num_gr )) == NULL ) {
		USAGE1("%s : malloc error. Terminates abnormally.\n", argv[0]);
		exit( 1 );
	    }
	    gr[0].start = MIN_CODE ;
	    gr[0].end   = MAX_CODE ;
	}
	/*
	 * refuse proportional fonts
	 */
	if ( GetUdcFontName( ReadSnf.in_file, NULL, &xlfdname ) ) {
	    USAGE1("%s : This font cannot get XLFD. Terminates abnormally.\n", argv[0]);
	    exit( 1 );
	}
	GETSPACINGSTR( spacing, xlfdname ) ;
	if ( !strcmp( "p", spacing ) || !strcmp( "P", spacing ) ) {
	    USAGE2("%s cannot edit proportional fonts.(SPACING \"%s\")\n", argv[0], spacing );
	    exit( 1 );
	}
	GETCHARSETSTR( char_set, xlfdname ) ;

	/* get ReadSnf.in_file */
	if ((targ_file = GetRealFileName( ReadSnf.in_file )) == NULL){
                USAGE2("%s : It was not possible to refer to the substance of the font file. \"%s\"\n", argv[0], ReadSnf.in_file);
		exit(1);
	}
	WriteSnf.snf_file = targ_file;

	if ( ( chk_fd = open( targ_file, O_RDWR ) ) < 0  ) {
	    USAGE2("%s : The font file of substance \"%s\" cannot be opened.\n", argv[0] , targ_file );
	    exit( 1 );
	}
	if( !fupd ){
	    if ( isLock( chk_fd ) == 1 ) {
		USAGE1("%s : Editing by other application. \n", argv[0] );
		close( chk_fd );
		exit( 1 );
	    }
	}
	close( chk_fd );

	/* We read whole characters from gpf file. */
	ReadSnf.start_code = MIN_CODE;
	ReadSnf.end_code   = MAX_CODE;
	ReadSnf.code_category = ALL_CODE;

	switch ( code_area ) {
	case	SYSTM:
		ReadGpf.start_code = MIN_CODE;
		ReadGpf.end_code   = MAX_CODE;
		ReadGpf.code_category = ALL_CODE;
		break;
	case	SYSTM | CDSET1 :
		ReadGpf.start_code = MIN_CODE;
		ReadGpf.end_code   = MAX_CODE;
		ReadGpf.code_category = ALL_CODE;
		break;
	default:
		COMM_SBFNT_SETUDCAREA( argv[0], ReadGpf, char_set, code_area, num_gr, gr ) ;
	}

	COMM_SBFNT_CONVCODETOGI( char_set, ReadSnf ) ;
	COMM_SBFNT_CONVCODETOGI( char_set, ReadGpf ) ;

	signal( SIGHUP , (void(*)())sigint_out );
	signal( SIGINT , (void(*)())sigint_out );
	signal( SIGQUIT, (void(*)())sigint_out );
	signal( SIGTERM, (void(*)())sigint_out );


	/*
	 * rw_init() 
	 *
	 * GPFTOBDF, BDFTOGPF
	 *
	 *  (ReadSnf.in_file)  -->  GPFTOBDF ==H 
	 *                                     H
	 *					dtaddcpf  <-- (ReadGpf.in_file)
	 *                                     H
	 * (WriteSnf.out_file) <--  BDFTOGPF ==H
	 *
	 */
	if ( rtn = rw_init( &ReadGpf, &ReadSnf, &WriteSnf, init_all ) ) {
		if ( WriteSnf.out_file ) {
			Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		}
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf, rtn, argv[0] );
	}

	if ( rtn = ReadGpfHeader( &ReadGpf, buf ) ) {
		if ( WriteSnf.out_file ) {
			Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		}
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf, rtn, argv[0] );
	}

	if ( init_all ) {
		/* We read only header of gpf file. */
		rtn = readSnfHeader( &ReadGpf, &ReadSnf, &WriteSnf, buf );
	} else if ( init ) {
		/* We read characters in UDC area from gpf file. */
		rtn = readSnf_with_init( &ReadGpf, &ReadSnf, &WriteSnf,
		    init, buf, num_gr, gr );
	} else {
		/* We read whole characters from gpf file. */
		rtn = readSnf( &ReadGpf, &ReadSnf, &WriteSnf, buf );
	}
	if ( rtn ) {
		if ( WriteSnf.out_file ) {
			Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		}
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf, rtn, argv[0] );
	}

	fclose( ReadSnf.input );
	WaitID( gtob_pid, chld_stat ) ;

	if ( ( rtn = mergePtn( argv[0], &ReadGpf, &ReadSnf, buf, code_area, modify, argv[0], num_gr, gr, code_no ) ) ) {
		if ( WriteSnf.out_file ) {
			Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		}
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf, rtn, argv[0] );
	}

	/* write SNF output file */
	if ( ( rtn = writeSnf( &ReadSnf, &WriteSnf ) ) ) {
		if ( WriteSnf.out_file ) {
			Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		}
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf, rtn, argv[0] );
	}

#ifdef ROOT_ONLY
	if ( pclose( WriteSnf.output ) ) {
		Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf, PCLOSE_ERROR, argv[0] );
	}
#else
	fclose( WriteSnf.output );
	WaitID( btop_pid, chld_stat ) ;
#if !defined( SVR4 ) && !defined( SYSV ) &&!defined(__osf__) && !defined(__FreeBSD__)
	if ( !WIFEXITED(chld_stat) ) {
#else
	if ( ! ( WIFEXITED(chld_stat) && !WEXITSTATUS(chld_stat) ) ) {
#endif
	    USAGE4("%s: The error occurred by %s (%08x). Cannot write %s\n",
		    argv[0], bdftopcf, chld_stat, WriteSnf.out_file);
		Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf,
					    FATAL_ERROR, argv[0] );
	}
#endif
	signal( SIGHUP , SIG_IGN );
	signal( SIGINT , SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGTERM, SIG_IGN );

	if ( ( stat( WriteSnf.out_file, &statbuf ) ) ||
	     ( statbuf.st_size == 0 )
	    ) {
		Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		put_error_and_exit( &ReadGpf, &ReadSnf, &WriteSnf,
						BDF_WRITE, argv[0] );
	}
	if ( stat( WriteSnf.snf_file, &statbuf ) ) {
		Unlink_Tmpfile( WriteSnf.out_file, argv[0] );
		exit( 1 );
	}
	exit( Make_NewFefFile(
		WriteSnf.snf_file, WriteSnf.out_file,
		FONT_FILE_PARM,
		(uid_t)statbuf.st_uid, (gid_t)statbuf.st_gid,
		argv[0] ));
}

static
rw_init(r_gpf, r_snf, w_snf, init_all )
struct ptobhead	*r_gpf;
struct btophead	*r_snf;
struct ptobhead	*w_snf;
int		init_all;
{
	FontInfoRec	*finf;
	int 	fd[2], snf_fd, permission;
	char	buf[BUFSIZE];
#ifdef ROOT_ONLY
	char	command[BUFSIZE], *ep ;
#else
	int 	pfd[2], ofd;
#endif

	if ( r_gpf->in_file == NULL ) {
		r_gpf->input = stdin;
	} else {
		if ( ( r_gpf->input = fopen(r_gpf->in_file, "r")) == NULL ) {
			return	GPF_OPEN_IN;
		}
	}

	/* SNF format */
	if ( ChkPcfFontFile( w_snf->snf_file ) ) {
		permission = 0 ;
		if( (snf_fd = open( w_snf->snf_file, O_RDONLY ) ) >= 0 ) {
		    COMM_SNF_FILEVERSION( snf_fd, finf, buf, permission ) ;
		    if ( permission < 0 ) {
			    return	BDF_INVAL;
		    }
		} else {
		    return BDF_OPEN_IN;
		}
	}
	if (pipe(fd) != 0) {
		return	PIPE_ERROR;
	}
	switch (gtob_pid = fork()) {
	case	0:
		close( fd[0] );
		close( 1 );
		if ( dup( fd[1] ) < 0 ) {
			return	DUP_ERROR;
		}

		close( fd[1] );

		/* SNFTOBDF */
		execl( oakgtobdf, oakgtobdf, r_snf->in_file, 0 );
		return	EXEC_ERROR;

	case	-1:
		return	FORK_ERROR;

	default:
		break;
	}
	close( fd[1] );
	if ( ( r_snf->input = fdopen( fd[0], "r" ) ) == NULL ) {
		return	FDOPEN_ERROR;
	}

	if ( !( w_snf->out_file = GetTmpPath( targ_file ) ) ) {
		return	MKTMP_ERROR;
	}

#ifdef ROOT_ONLY
	if( ChkPcfFontFile( w_snf->snf_file ) == 0 ) {
		sprintf(command, "%s > %s", bdftopcf, w_snf->out_file) ;
	} else {
		COMM_SNF_POPEN( permission, w_snf->snf_file, w_snf->out_file, ep, buf, command ) ;
	}
	if ((w_snf->output = popen(command, "w")) == NULL) {
		return(POPEN_ERROR);
	}
#else	/* ROOT_ONLY */
	if ( pipe( pfd ) != 0 ) {
		return	PIPE_ERROR;
	}
	switch (btop_pid = fork()) {
	case	0:
		if ( ( ofd = open( w_snf->out_file, O_WRONLY | O_CREAT, 0664 ) ) < 0 ) {
			return	BDF_OPEN_OUT;
		}
		close( 0 );
		if ( dup(pfd[0]) < 0 ) {
			return	DUP_ERROR;
		}
		close( pfd[0] );
		close( pfd[1] );
		close( 1 );
		if( dup( ofd ) < 0 ) {
			return	DUP_ERROR;
		}
		close( ofd );
		if ( ChkPcfFontFile( w_snf->snf_file ) == 0 ) {
			execl( bdftopcf, bdftopcf, 0 );
			return	PCFFONTC_ERROR;
		}

		COMM_SNF_EXECLBDFTOSNF( permission, buf, w_snf->snf_file ) ;
		return SNFFONTC_ERROR;

	case	-1:
					return	FORK_ERROR;
				
		    default:
		break;
	}
	close( pfd[0] );
	if ( ( w_snf->output = fdopen( pfd[1], "w" ) ) == NULL ) {
		return	FDOPEN_ERROR;
	}
#endif	/* ROOT_ONLY */
	return	0;
}


static
readSnf(r_gpf, r_snf, w_snf, buf)
struct ptobhead *r_gpf;
struct btophead *r_snf;
struct ptobhead *w_snf;
char   	      	*buf;
{
	int	nchar, rtn;

	if ( ( rtn = readBdfHeaderAndPut( r_snf, w_snf, buf ) ) ) {
		return	rtn;
	}

	nchar = r_snf->num_chars + r_gpf->num_chars;

	if ( ( r_snf->code = (int *)malloc( sizeof(int) * nchar ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	if ( ( r_snf->ptn = (char **)malloc( sizeof(char *) * nchar ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	return	ReadBdfToMemory( r_snf, buf ) ;
}

static
readSnf_with_init(r_gpf, r_snf, w_snf, init, buf, num_gr, gr )
struct ptobhead *r_gpf;
struct btophead *r_snf;
struct ptobhead *w_snf;
int 		init;
char		*buf;
int		num_gr ;
FalGlyphRegion	*gr ;
{
	int 	nchar, rtn;


	if ( ( rtn = readBdfHeaderAndPut( r_snf, w_snf, buf ) ) ) {
		return	rtn;
	}

	nchar = r_snf->num_chars + r_gpf->num_chars;

	if ( ( r_snf->code = (int *)malloc( sizeof(int) * nchar ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	if ( ( r_snf->ptn = (char **)malloc( sizeof(char *) * nchar ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	return	readBdfToMemory_with_init( r_snf, init, buf, num_gr, gr ) ;
}


static
readSnfHeader(r_gpf, r_snf, w_snf, buf)
struct ptobhead *r_gpf;
struct btophead *r_snf;
struct ptobhead *w_snf;
char	*buf;
{
	int 	rtn;

	if ( ( rtn = readBdfHeaderAndPut( r_snf, w_snf, buf ) ) ) {
		return	rtn;
	}

	r_snf->num_chars = 0;

	if ( ( r_snf->code = (int *)malloc( sizeof(int) * r_gpf->num_chars ) ) == NULL ) {
		return	MALLOC_ERROR;
	}
	if ( ( r_snf->ptn = (char **)malloc( sizeof(char *) * r_gpf->num_chars ) ) == NULL ) {
		return	MALLOC_ERROR;
	}
	return	0;
}

static
readBdfHeaderAndPut(r_snf, w_snf, buf)
struct btophead *r_snf;
struct ptobhead *w_snf;
char	*buf;
{
	char	*p;
	int 	getstat = 0;

	while ( 1 ) {
		if ( fgets( buf, BUFSIZE, r_snf->input ) == NULL ) {
			return	BDF_INVAL;
		}
		p = buf;
		SCAN_TO_NONSP( p )
		    if ( !strncmp( p, SIZE, strlen( SIZE ) ) ) {
			if ( ( sscanf( p, "SIZE %f%d",
			    &(r_snf->bdf_point), &(r_snf->bdf_xdpi))) != 2 ) {
				return	BDF_INVAL;
			}
			fprintf( w_snf->output, "%s", buf );
			getstat |= 0x01;

		} else if ( !strncmp( p, FONTBOUNDINGBOX, strlen( FONTBOUNDINGBOX ) ) ) {
			if ( ( sscanf( p, "FONTBOUNDINGBOX %d%d%d%d",
			    &(r_snf->bdf_width), &(r_snf->bdf_height),
			    &(r_snf->bdf_x), &(r_snf->bdf_y) )) != 4) {
				return	BDF_INVAL;
			}
			fprintf( w_snf->output, "%s", buf );
			getstat |= 0x02;

		} else if ( !strncmp( p, CHARS, strlen( CHARS ) ) ) {
			if ( ( sscanf( p, "CHARS %d", &( r_snf->num_chars ) ) ) != 1 ) {
				return	BDF_INVAL;
			}
			getstat |= 0x04;
			break;
		} else {
			fprintf( w_snf->output, "%s", buf );
		}
	}

	if ( getstat != 0x07 ) {
		return	BDF_INVAL;
	}
	return	0;
}

#define	DispCodePoint( command, char_set, gidx, dspcode, code_no, locale ){\
	if( COMM_SBFNT_ISSBFNT( (char_set) ) ){\
		dspcode = DISPCODEPOINT( (char_set), (gidx) ) ;\
	}else{\
	    if( fal_glyph_to_code( (locale), (char_set), FALGETFALCODESET((code_no)), (gidx), &(dspcode) ) ){\
		USAGE3("%s : Failed to convert glyph index into code point.(0x%x charset: %s)\n", (command), (gidx), (char_set) ) ;\
		return -1 ;\
	    }\
	}\
}

static
mergePtn(com, r_gpf, r_snf, buf, code_area, modify, prog_name, num_gr, gr, code_no)
char		*com ;
struct ptobhead *r_gpf;
struct btophead *r_snf;
char		*buf;
int 		code_area;
int 		modify;
char		*prog_name;
int		num_gr ;
FalGlyphRegion	*gr ;
int		code_no ;
{
	int 	code, rtn, msize, i, j, dspcode;
	char	*ptn;

	if ( ( r_gpf->p_width != r_snf->bdf_width )
	    || ( r_gpf->p_height != r_snf->bdf_height )
	    ) {
		r_gpf->zoomf = 1;

		msize = ( r_gpf->p_width + 7 ) / 8 * r_gpf->p_height;

		if ( ( ptn = (char *)malloc( msize ) ) == NULL ) {
			return	MALLOC_ERROR;
		}
		r_gpf->ptn = &ptn;

	} else {
		r_gpf->zoomf = 0;
	}

	for ( i = 0; i < r_gpf->num_chars; i++ ) {
		if ( ( rtn = GetGpfCode( r_gpf, buf, &code ) ) < 0 ) {
			return	rtn;
		} else if ( rtn == FILE_END ) {
			break;
		}
		if( COMM_SBFNT_ISSBFNT( char_set ) ) CONVGLYPHINDEX( code ) ;
		if (
		( code < r_gpf->start_code) || ( code > r_gpf->end_code ) 
		||( !IN_CODE( r_gpf->code_category, SHIFT_ON( code ) ) )
		||(
		    (r_gpf->code_category == ALL_CODE) &&
		    !( code_area & SYSTM ) &&
		    IsInRegion( code, num_gr, gr )
		    )
		) {
		    DispCodePoint( com, char_set, code, dspcode, code_no, util_locale ) ;
		    USAGE2("%s : The font of a specified code cannot be added/changed \"0x%x\".\n", prog_name, dspcode );
		    fgets(buf, BUFSIZE, r_gpf->input);
		    continue;
		}

		for ( j = 0; j < r_snf->num_chars; j++ ) {
		    if ( r_snf->code[j] == code ) {
			if ( !modify ) {
			    DispCodePoint( com, char_set, code, dspcode, code_no, util_locale ) ;
			    USAGE2("%s : The font has already been registered in a specified code. \"0x%x\"\n", prog_name, dspcode );
			    fgets( buf, BUFSIZE, r_gpf->input );
			    break;
			}
			if ( ( rtn = ModifyPtn( r_gpf, r_snf, buf, j ) ) ) {
			    return	rtn;
			}
			break;
		    } else if ( r_snf->code[j] > code ) {
			if ( ( rtn = InsertPtn( r_gpf, r_snf, buf, code, j ) ) ) {
			    return	rtn;
			}
			break;
		    }
		}
		if ( j == r_snf->num_chars ) {
			if ( ( rtn = InsertPtn( r_gpf, r_snf, buf, code, j ) ) ) {
				return	rtn;
			}
		}
	}
	return	0;
}


static
ModifyPtn( r_gpf, r_snf, buf, ix )
struct ptobhead *r_gpf;
struct btophead *r_snf;
char	*buf;
int 	ix;
{
	int 	mwidth, msize, rtn;

	mwidth = ( r_gpf->p_width + 7 ) / 8;
	msize = mwidth * r_gpf->p_height;

	if ( r_gpf->zoomf ) {
		if ( ( rtn = GetGpfPtn( r_gpf, buf, r_gpf->ptn[0], mwidth, msize ) ) ) {
			return	rtn;
		}
		if ( ( rtn = PtnZoom( r_snf->ptn[ix], r_gpf->ptn[0],
		    r_gpf->p_width, r_gpf->p_height,
		    r_snf->bdf_width, r_snf->bdf_height ) ) ) {
			return	rtn;
		}
	} else {
		if ( ( rtn = GetGpfPtn( r_gpf, buf, r_snf->ptn[ix], mwidth, msize ) ) ) {
			return	rtn;
		}
	}
	return	0;
}

static
InsertPtn( r_gpf, r_snf, buf, code, ix )
struct ptobhead *r_gpf;
struct btophead *r_snf;
char	*buf;
int 	code;
int 	ix;
{
	int 	mwidth, msize, rtn, i;

	for ( i = r_snf->num_chars; i > ix; i-- ) {
		r_snf->code[i] = r_snf->code[i-1];
		r_snf->ptn[i] = r_snf->ptn[i-1];
	}
	r_snf->code[ix] = code;

	r_snf->num_chars++;

	mwidth = (r_snf->bdf_width + 7) / 8;
	msize = mwidth * r_snf->bdf_height;

	if ( ( r_snf->ptn[ix] = (char *)malloc( msize ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	if ( r_gpf->zoomf ) {
		mwidth = (r_gpf->p_width + 7) / 8;
		msize = mwidth * r_gpf->p_height;
		if ( ( rtn = GetGpfPtn( r_gpf, buf, r_gpf->ptn[0], mwidth, msize ) ) ) {
			return	rtn;
		}
		if ( ( rtn = PtnZoom( r_snf->ptn[ix], r_gpf->ptn[0],
		    r_gpf->p_width, r_gpf->p_height,
		    r_snf->bdf_width, r_snf->bdf_height ) ) ) {
			return	rtn;
		}
	} else {
		if ( ( rtn = GetGpfPtn( r_gpf, buf, r_snf->ptn[ix], mwidth, msize ) ) ) {
			return	rtn;
		}
	}
	return(0);
}

static
writeSnf( r_snf, w_snf )
struct btophead *r_snf;
struct ptobhead *w_snf;
{
	w_snf->zoomf = 0;
	w_snf->num_chars = r_snf->num_chars;
	w_snf->code = r_snf->code;
	w_snf->ptn = r_snf->ptn;
	w_snf->bdf_width = r_snf->bdf_width;
	w_snf->bdf_height = r_snf->bdf_height;

	w_snf->bdf_x = r_snf->bdf_x;
	w_snf->bdf_y = r_snf->bdf_y;

	w_snf->bdf_point = r_snf->bdf_point;
	w_snf->bdf_xdpi = r_snf->bdf_xdpi;

	return	WritePtnToBdf( w_snf );
}



static void
put_error_and_exit(ptob_in, btop, ptob_out, er_no, prog_name)
struct ptobhead *ptob_in;
struct btophead *btop;
struct ptobhead *ptob_out;
int 	er_no;
char	*prog_name;
{
	ErrMsgTable_AndExit( er_no, ptob_in->in_file, ptob_out->out_file,
	    btop->in_file,    NULL,
	    ptob_in->bdf_file,
	    prog_name
	    );
	return;
}



static void
put_help( prog_name )
char	*prog_name;
{
        USAGE1("Usage: %s -xlfd xlfd_name \n", prog_name);
        USAGE("\t\t[-g character_size][-p character_pattern_file_name]\n");
	USAGE("\t\t[-style style]");
	USAGE("\t\t[-init]\t(clear whole glyphs of gpf file that used for user defined characters)\n" ) ;
	USAGE("\t\t[-modify]\t(permits the change of font)\n");
	COMM_HELP_MSG ;
	USAGE1("%s can insert or modify glyphs in the following code area.\n", prog_name);
	USAGE("codeset \t\tcode area\n");
	USAGE("----------------------------------------\n");
	DispUdcCpArea() ;
	USAGE("The xlfd name and character size may be obtained using dtlsgpf command.\n");
	return;
}



static int
readBdfToMemory_with_init(head, init, buf, num_gr, gr )
struct btophead *head;
int	init;
char   *buf;
int	num_gr ;
FalGlyphRegion	*gr ;
{
    int	    code, mwidth, num_char, bsize, rtn ;
    char    *ptn;

    num_char = 0;
    mwidth = (head->bdf_width + 7) / 8;
    bsize = mwidth * head->bdf_height;
    while(1) {
	if ((rtn = GetBdfCode(head, buf, &code)) < 0) {
	    return(rtn);	/* contain BDF_INVAL */
	} else if (rtn == FILE_END) {
	    head->num_chars = num_char;
	    break;
	}
	rtn = IsInRegion( code, num_gr, gr ) ;
	if (
	   ( code < head->start_code) || ( code > head->end_code ) 
	    ||( !IN_CODE( head->code_category, SHIFT_ON( code ) ) )
	    ||(
		    (head->code_category == ALL_CODE)
		&& ( (init)? ((rtn)?0:1) : ((rtn)?1:0) )
	    ||( COMM_ISDEFAULTCHAR( code ) )
	    )
	) {
	    continue;
	}
	head->code[num_char] = code;
	if ((ptn = head->ptn[num_char++] =
	    (char *)malloc(bsize)) == NULL) {
	    return(MALLOC_ERROR);
	}
	if ((rtn = GetBdfPtn(head, buf, ptn, mwidth, bsize)) != 0) {
	    return(rtn);
	}
    }
    return(0);
}



