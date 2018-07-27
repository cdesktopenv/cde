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
/* $XConsortium: cpgpf.c /main/6 1996/11/08 02:03:36 cde-fuj $ */
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
#include	<X11/fonts/fontstruct.h>

#include	"FaLib.h"
#include	"snfstruct.h"
#include	"udcutil.h"

#include	<errno.h>

#define	START_CODE	0
#define	END_CODE	1
#define	INPUT_FONT	0
#define	OUTPUT_FONT	1

#define RESIZE_BY_RATE( a, r )   ((int)( (float)(a) * (r)))
#define DEF_RESOLUTION	((78 / 72.27) * 100)

static	void	put_error_and_exit(struct btophead *ptob_in,
				   struct ptobhead *ptob_out,
				   int     er_no,
				   char    *prog_name);
static	void	put_help(char *prog_name);
static	void	sigint_out(void);
static	void	getDefaultCh(unsigned int cbuf);
static	void	MkFontProp(int font_height, char *font, char *output);

static	int	CpyGpfInit(struct btophead *i_gpf,
			   struct btophead *o_gpf,
			   struct ptobhead *w_gpf,
			   int             type);
static	int	writeGpf(struct btophead *i_gpf,
			 struct btophead *o_gpf,
			 struct ptobhead *w_gpf);
static	int	check_width(char *prog_name, char *len_str);
static	int	check_height(char *prog_name, char *len_str);
static	int	check_width_height(char *prog_name, char *len_str, int mode);
static	int	getGpfFileName(char    *com,
			       struct  btophead        *gpfinf,
			       int     code_area,
			       int     code_no,
			       char    *xlfdname,
			       char    *snf_file,
			       char    *style);
static	int	readGpf(struct btophead *i_gpf,
			char            *buf,
			int             startcode,
			int             endcode,
			int             num_gr,
			FalGlyphRegion  *gr);
static	int	readBdfHeaderAndPut(struct btophead *o_gpf,
				    struct ptobhead *w_gpf,
				    char *buf);
static	int	check_code(char    *prog_name,
			   char    *code_str,
			   int     mode,
			   char    *charset,
			   int     num_gr,
			   FalGlyphRegion  *gr);
static	int	readBdfToMemory(struct btophead *head,
				char   *buf,
				int     startcode,
				int     endcode,
				int     num_gr,
				FalGlyphRegion  *gr);
static	int	cpg_atoi(char *numstr);
static	int	is_in_udcregion(int start,
				int end,
				int num_gr,
				FalGlyphRegion *gr);

extern	int	fal_code_to_glyph(char            *locale,
				  unsigned long   codepoint,
				  FalGIInf        **gi,
				  int             *num_gi);

static struct ptobhead WriteGpf;

static	char	*targ_file = NULL;	/* UDC_file_name */
static	char	*com = NULL;		/* command_name */
static	char	create_font[BUFSIZE] ;
static	int	new_target = 0 ;
static	unsigned int	firstCol, lastCol, firstRow, lastRow ;
static	char	*locale ;
static	char	*spacing ;
static	char	*char_set ;
static	char	*xlfdname ;


static void
sigint_out(void)
{
    if (WriteGpf.out_file) {
	Unlink_Tmpfile( WriteGpf.out_file, com );
    }
    exit( 0 );
}


int main(int argc, char **argv)
{
	int 	code_area , help, no_infile, no_style, type ;
	int 	rtn, i, exit_code ;
#ifndef ROOT_ONLY
	int 	exit_stat;
#endif
	struct btophead	InputGpf;	/* input file  */
	struct btophead	OutputGpf;	/* output file */
	struct stat	statbuf;
	char		isnf_file[BUFSIZE] ;
	char		osnf_file[BUFSIZE] ;
	char		readbuf[BUFSIZE];
	char		*istyle, *ostyle ;
	char		*ixlfdname, *oxlfdname ;
	int 		chk_fd;
	FalGlyphRegion	*gr ;
	int		num_gr ;
	char		*start_code, *end_code ;
	int		startcode, endcode ;

	int 	code_no;
	char	*codeset = DEFAULT_CODESET;

	/*
	 * initiation of parameters
	 */
	com = argv[0];
	*create_font = '\0' ;
	if( (locale = (char *)getenv( "LANG" )) == NULL ){
	    locale = "C" ;
	}

	memset( &InputGpf, 0, sizeof(struct btophead) ) ;
	memset( &OutputGpf, 0, sizeof(struct btophead) ) ;
	memset( &WriteGpf, 0, sizeof(struct ptobhead) ) ;

	COMM_SETDEFAULTSTYLE( istyle ) ;
	COMM_SETDEFAULTSTYLE( ostyle ) ;

	InputGpf.in_file = OutputGpf.in_file = WriteGpf.out_file = NULL;
	ixlfdname = oxlfdname = '\0' ;
	spacing = char_set = xlfdname = NULL ;

	gr 		= NULL ;
	num_gr 		= 0 ;
	firstCol = firstRow = 0xff ;
	lastCol = lastRow = 0 ;

	start_code = end_code = '\0' ;
	startcode 	= MIN_CODE ;
	endcode		= MAX_CODE ;

	/* get full path of font compilers */
	if (!( bdftopcf = get_cmd_path( getenv( "PATH" ), BDFTOPCF_CMD ))) {
		bdftopcf = BDFTOPCF;
	}
	if (!( oakgtobdf = get_cmd_path( getenv( "PATH" ), SNFTOBDF_CMD ))) {
		oakgtobdf = SNFTOBDF;
	}
	if (!( bdftosnf = get_cmd_path( getenv( "PATH" ), BDFTOSNF_CMD ))) {
		bdftosnf = BDFTOSNF;
	}
	/*****************************************************************
	* check options
	*****************************************************************/
	code_area = NORMAL;
	help = no_infile = no_style = type = 0;

	for ( i = 1; i < argc; i++ ) {
		if ( *argv[i] != '-' ) {
		    if( !InputGpf.in_file ){
			InputGpf.in_file = argv[i] ;
		    }else if( !OutputGpf.in_file ){
			OutputGpf.in_file = argv[i] ;
		    } else {
			put_help(com);
			exit( 1 );
		    }
		}else if ( !strcmp( argv[i], "-ig" ) ) {
		    if ( ( i < argc-1) && (*argv[i+1] != '-' ) ) {
			InputGpf.in_file = argv[++i];
		    } else {
			no_infile = 1;
		    }
		} else if ( !strcmp( argv[i], "-og" ) ) {
		    if ( (i < argc-1) && (*argv[i+1] != '-') ){
			OutputGpf.in_file = argv[++i];
		    } else {
			no_infile = 2;
		    }
		} else if ( !strcmp( argv[i], "-system" ) ) {
			code_area |= SYSTM;
		} else if ( !strcmp( argv[i], "-help" ) ) {
			help = 1;
		} else if ( !strcmp(argv[i], "-width") ) {
		    if ((i < argc-1) && (*argv[i+1] != '-')) {
			OutputGpf.p_width =  check_width( com, argv[++i]);
			if (OutputGpf.p_width == 0) {
			    exit(1);
			}
		    } else {
			USAGE1("%s : The character width is not specified.\n", com);
			exit( 1 );
		    }
		} else if ( !strcmp(argv[i], "-height") ) {
		    if ((i < argc-1) && (*argv[i+1] != '-')) {
			OutputGpf.p_height = check_height( argv[0], argv[++i]);
			if (OutputGpf.p_height == 0) {
			    exit(1);
			}
		    } else {
			USAGE1("%s :  The character height is not specified.\n", com);
			exit(1);
		    }
		} else if ( !strcmp(argv[i], "-start") ) {
		    if ((i < argc-1) && (*argv[i+1] != '-')) {
			start_code = argv[++i] ;
		    } else {
			USAGE1("%s : The extract start code is not specified.\n", com);
			exit( 1 );
		    }
		} else if ( !strcmp(argv[i], "-end" ) ) {
		    if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ) {
			end_code = argv[++i] ;
		    } else {
			USAGE1("%s : The extract end code is not specified.\n", com);
			exit( 1 );
		    }
		} else if ( !strcmp( argv[i], "-istyle" ) ) {
		    if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ){
			istyle = argv[++i];
		    } else {
			no_style = 1;
		    }
		} else if ( !strcmp( argv[i], "-ostyle" ) ) {
		    if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ){
			ostyle = argv[++i];
		    } else {
			no_style = 2;
		    }
		} else if ( !strcmp( argv[i], "-codeset" ) ) {
		    if ( ( i < argc - 1 ) && ( *argv[i+1] != '-' )){
			codeset = argv[++i];
			COMM_SET_CODESET( codeset, code_area ) ;
		    }
		} else if ( !strcmp( argv[i], "-ixlfd" ) ) {
		    if ( i < argc - 1 ){
			ixlfdname = argv[++i];
		    }
		} else if ( !strcmp( argv[i], "-oxlfd" ) ) {
		    if ( i < argc - 1 ){
			oxlfdname = argv[++i];
		    }
		} else if ( COMM_SBOPT_STRCMP( argv, i ) ) {
		    COMM_SBOPT_SETSBOPT( code_area ) ;
		} else if ( COMM_TYPE_STRCMP( argv, i ) ) {
		    COMM_TYPE_SETTYPE( argc, argv, i, type ) ;
		} else {
		    put_help(com);
		    exit( 1 );
		}
	}

	/* help_message */
	if ( help == 1 ) {
		put_help( com );
		exit( 0 );
	}
	COMM_SBOPT_CHECKSBOPT( com, code_area ) ;

	if ( no_infile ) {
	    if ( no_infile == 1 )
                USAGE1("%s : The font file name for input cannot be omitted.\n", com );
	    else
                USAGE1("%s : The font file name for output cannot be omitted.\n", com );
	    exit( 1 );
	}

	if ( (code_no = cpg_atoi( codeset)) == -1 ) {
	    USAGE1("%s : The code set number is illegal.\n", com );
	    exit( 1 );
	}

	if ( no_style ) {
	    if ( no_style == 1 )
                USAGE1("%s : The style for input cannot be omitted.\n", com );
	    else
                USAGE1("%s : The style for output cannot be omitted.\n", com );
	    USAGE1("%s : The style is not specified.\n", com ) ;
	    exit( 1 );
	}

	/*****************************************************************
	* get file name
	*****************************************************************/
	/* copy origin */
	if ( getGpfFileName( com, &InputGpf, code_area, code_no, ixlfdname, isnf_file, istyle ) ) {
	    exit( 1 );
	}

	/* copy target */
	if ( getGpfFileName( com, &OutputGpf, code_area, code_no, oxlfdname, osnf_file, ostyle ) ) {
	    exit( 1 );
	}

	/*
	 * refuse proportional fonts
	 */
	if( ( !stat( OutputGpf.in_file, &statbuf ) )){
	    if ( GetUdcFontName( OutputGpf.in_file, NULL, &xlfdname ) ) {
		USAGE2("%s : This font cannot get XLFD(%s). Terminates abnormally.\n", com, OutputGpf.in_file );
		exit( 1 );
	    }
	    GETSPACINGSTR( spacing, xlfdname ) ;
	    if ( !strcmp( "p", spacing ) || !strcmp( "P", spacing ) ) {
		USAGE2("%s : cannot edit proportional fonts.(SPACING \"%s\")\n", com, spacing );
		exit( 1 );
	    }
	}else{
	    /* new file */
	    if( !(code_area & SYSTM) ){
		USAGE2("%s : \"%s\" does not exists.\n", com, OutputGpf.in_file );
		exit( 1 );
	    }
	}
	if( xlfdname )	free( xlfdname ) ;
	if ( GetUdcFontName( InputGpf.in_file, NULL, &xlfdname ) ) {
	    USAGE2("%s : This font cannot get XLFD(%s). Terminates abnormally.\n", com, InputGpf.in_file );
	    exit( 1 );
	}
	GETSPACINGSTR( spacing, xlfdname ) ;
	if ( !strcmp( "p", spacing ) || !strcmp( "P", spacing ) ) {
	    USAGE2("%s : cannot edit proportional fonts.(SPACING \"%s\")\n", com, spacing );
	    exit( 1 );
	}
	GETCHARSETSTR( char_set, xlfdname ) ;

	/*****************************************************************
	* get UDC code region
	*****************************************************************/
	if ( !(code_area & SYSTM) ) {
	    if ( GetUdcRegion( com, code_no, InputGpf.in_file, &num_gr, &gr ) ) {
		USAGE1("%s : This font cannot get UDC code region. Terminates abnormally.\n", com);
		exit( 1 );
	    }
	} else {
	    num_gr = 1 ;
	    if( (gr = (FalGlyphRegion *)malloc( sizeof(FalGlyphRegion) * num_gr )) == NULL ) {
		USAGE1("%s : malloc error. Terminates abnormally.\n", com);
		exit( 1 );
	    }
	    gr[0].start = MIN_CODE ;
	    gr[0].end   = MAX_CODE ;
	}

	/*
	 * check specified code region
	 */
	if( start_code && end_code ) {
	    if( (startcode = check_code(com, start_code, START_CODE, char_set, num_gr, gr )) < 0 ) {
		exit( 1 );
	    }
	    if( (endcode = check_code(com, end_code, END_CODE, char_set, num_gr, gr )) < 0 ) {
		exit( 1 );
	    }

	    if ( startcode > endcode ) {
		USAGE1("%s : The specification of extract start/end code is improper.\n", com);
		exit( 1 );
	    }

	    if( is_in_udcregion( startcode, endcode, num_gr, gr ) ){
		USAGE3("%s : The specified code \"%s-%s\" is outside of the UDC code region.\n", com, start_code, end_code );
		USAGE("The UDC code region of specified font is showed below.\n" );
		for( i=0; i<num_gr; i++ )
		    USAGE2("\t0x%x-0x%x\n", gr[i].start, gr[i].end );
		exit( 1 );
	    }
	}

	/*****************************************************************
	* check status of font file
	*****************************************************************/
	/* get InputGpf.in_file */
	if ((targ_file = GetRealFileName( OutputGpf.in_file )) == NULL){
	    if( errno == ENOENT && (code_area & SYSTM) ){
		/* create new file */
		if ( ( chk_fd = open( OutputGpf.in_file, O_RDWR | O_CREAT, 0644 ) ) < 0  ) {
		    USAGE2("%s : The font file of substance \"%s\" cannot be create.\n", com , OutputGpf.in_file );
		    exit( 1 );
		}
		targ_file = OutputGpf.in_file ;
		new_target = 1 ;
		close( chk_fd );
	    }else{
                USAGE2("%s : It is not possible to refer to the substance of the font file. \"%s\"\n", com, OutputGpf.in_file);
		exit(1);
	    }
	}
	WriteGpf.snf_file = targ_file;

	if ( ( chk_fd = open( targ_file, O_RDWR ) ) < 0  ) {
	    USAGE2("%s : The font file of substance \"%s\" cannot be opened.\n", com , targ_file );
	    exit( 1 );
	}
	if ( isLock( chk_fd ) == 1 ) {
	    USAGE2("%s : \"%s\" is opened by other application. \n", WriteGpf.snf_file, com );
	    close( chk_fd );
	    exit( 1 );
	}
	close( chk_fd );

	/*****************************************************************
	* set code category to inner structure
	*****************************************************************/

	WriteGpf.start_code	= MIN_CODE;
	WriteGpf.end_code	= MAX_CODE;
	WriteGpf.code_category	= ALL_CODE;

	COMM_SBFNT_SETUDCAREA( argv[0], OutputGpf, char_set, code_area, num_gr, gr ) ;

	/*
	 * CpyGpfInit()
	 *
	 * GPFTOBDF, BDFTOGPF
	 *
	 *  (InputGpf.in_file)  --> GPFTOBDF ==H
	 *                                     H
	 *  (OutputGpf.in_file) --> GPFTOBDF ==H
	 *                                     H
	 *				    dtcpgpf
	 *                                     H
	 * (WriteGpf.out_file) <--  BDFTOGPF ==H
	 *
	 */

	signal( SIGHUP , (void(*)())sigint_out );
	signal( SIGINT , (void(*)())sigint_out );
	signal( SIGQUIT, (void(*)())sigint_out );
	signal( SIGTERM, (void(*)())sigint_out );

	if ( rtn = CpyGpfInit( &InputGpf, &OutputGpf, &WriteGpf, type ) ) {
	    if ( WriteGpf.out_file ) {
		Unlink_Tmpfile( WriteGpf.out_file, com );
	    }
	    put_error_and_exit( &InputGpf, &WriteGpf, rtn, com );
	}

	/* read copy origin */
	if ( (rtn = readGpf( &InputGpf, readbuf, startcode, endcode, num_gr, gr )) ){
	    if ( WriteGpf.out_file ) {
		Unlink_Tmpfile( WriteGpf.out_file, com );
	    }
	    put_error_and_exit( &InputGpf, &WriteGpf, rtn, com );
	}
	fclose( InputGpf.input );

	/* read copy target */
	if( (rtn = readBdfHeaderAndPut(&OutputGpf, &WriteGpf, readbuf)) ) {
	    if ( WriteGpf.out_file ) {
		Unlink_Tmpfile( WriteGpf.out_file, com );
	    }
	    put_error_and_exit( &InputGpf, &WriteGpf, rtn, com );
	}
	fclose( OutputGpf.input );
	wait( 0 );

	/*
	 * write SNF output file
	 */
	if ( ( rtn = writeGpf( &InputGpf, &OutputGpf, &WriteGpf ) ) ) {
	    if ( WriteGpf.out_file ) {
		Unlink_Tmpfile( WriteGpf.out_file, com );
	    }
	    put_error_and_exit( &InputGpf, &WriteGpf, rtn, com );
	}

#ifdef ROOT_ONLY
	if ( pclose( WriteGpf.output ) ) {
		Unlink_Tmpfile( WriteGpf.out_file, com );
		put_error_and_exit( &InputGpf, &WriteGpf, PCLOSE_ERROR, com );
	}
#else
	fclose( WriteGpf.output );
	wait( &exit_stat );
#if !defined( SVR4 ) && !defined( SYSV )
	if ( !WIFEXITED(exit_stat) ) {
#else
	if ( ! ( WIFEXITED(exit_stat) && !WEXITSTATUS(exit_stat) ) ) {
#endif
                USAGE3("%s: The error occurred by %s (%08x).\n", com, oakgtobdf, exit_stat);
		Unlink_Tmpfile( WriteGpf.out_file, com );
		put_error_and_exit( &InputGpf, &WriteGpf, FATAL_ERROR, com );
	}
#endif
	signal( SIGHUP , SIG_IGN );
	signal( SIGINT , SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGTERM, SIG_IGN );

	sleep( 1 );
	if ( ( stat( WriteGpf.out_file, &statbuf ) ) || ( statbuf.st_size == 0 )
	    ) {
		Unlink_Tmpfile( WriteGpf.out_file, com );
		put_error_and_exit( &InputGpf, &WriteGpf, BDF_WRITE, com );
	}
	if ( stat( WriteGpf.snf_file, &statbuf ) ) {
		Unlink_Tmpfile( WriteGpf.out_file, com );
		exit( 1 );
	}
	exit_code = Make_NewFefFile( WriteGpf.snf_file, WriteGpf.out_file,
	    FONT_FILE_PARM, (uid_t)statbuf.st_uid, (gid_t)statbuf.st_gid, com ) ;

	if( !exit_code ){
	    if( *create_font != '\0' && new_target )
		fprintf( stdout, "%s : New font <%s> is created.\n", com, create_font );
	}
	exit( exit_code );
}


static	int
getGpfFileName(
char	*com,
struct	btophead	*gpfinf,
int	code_area,
int	code_no,
char	*xlfdname,
char	*snf_file,
char	*style)
{


	if ( gpfinf->in_file == NULL && xlfdname == NULL ) {
	    if ( code_area & SYSTM ) {
		USAGE1("%s : The GPF output file name cannot be omitted.\n", com );
	    } else {	/* string of charcter size */
		USAGE1("%s : The character size specification cannot be omitted.\n", com );
	    }
	    return -1 ;
	}

	/* open GPF file */
	if ( !(code_area & SYSTM) ) {
	    if( xlfdname ) {
		if ( GetUdcFileName( com, code_no, xlfdname, snf_file ) ) {
		    USAGE2("%s : Failed to get font file.( XLFD: %s ) Terminates abnormally.\n", com, xlfdname );
		    return -1 ;
		}
		gpfinf->in_file = snf_file ;
	    } else {

		switch ( GetFileName( com, gpfinf->in_file, style, code_no, snf_file ) ) {
		case 0:
			gpfinf->in_file = snf_file;
			break;
		case -1:
			return -1 ;
		default:
                        USAGE1("%s : The font file name cannot be obtained. Terminates abnormally.\n", com);
			return -1 ;
		}
	    }
	}

	return 0 ;
}


static
CpyGpfInit(
struct btophead	*i_gpf,
struct btophead	*o_gpf,
struct ptobhead	*w_gpf,
int		type)
{
	FontInfoRec	*finf;
	int 	ifd[2], ofd[2], snf_fd, permission;
	char	buf[BUFSIZE];
#ifdef ROOT_ONLY
	char	command[BUFSIZE], *ep ;
#else
	int 	pfd[2], out_fd ;
#endif

	/* SNF format */
	if( !new_target ){
	    if ( ChkPcfFontFile( w_gpf->snf_file ) ) {
		if( (snf_fd = open( w_gpf->snf_file, O_RDONLY ) ) >= 0 ) {
		    return BDF_OPEN_IN;
		}
	    }
	}else{
	    COMM_SNF_NEWTARGET( permission ) ;
	}

	/* open pipe for copy origin gpf */
	if (pipe(ifd) != 0) {
		return	PIPE_ERROR;
	}
	switch ( fork() ) {
	case	0:
		close( ifd[0] );
		close( 1 );
		if ( dup( ifd[1] ) < 0 ) {
		    return	DUP_ERROR;
		}
		close( ifd[1] );
		/* SNFTOBDF */
		execl( oakgtobdf, oakgtobdf, i_gpf->in_file, 0 );
		return	EXEC_ERROR;
	case	-1:
		return	FORK_ERROR;
	default:
		break;
	}
	close( ifd[1] );
	if ( ( i_gpf->input = fdopen( ifd[0], "r" ) ) == NULL ) {
		return	FDOPEN_ERROR;
	}

	/* open pipe for copy target gpf */
	if (pipe(ofd) != 0) {
		return	PIPE_ERROR;
	}
	switch ( fork() ) {
	case	0:
		close( ofd[0] );
		close( 1 );
		if ( dup( ofd[1] ) < 0 ) {
		    return	DUP_ERROR;
		}
		close( ofd[1] );
		/* SNFTOBDF */
		if( !new_target ){
		    execl( oakgtobdf, oakgtobdf, o_gpf->in_file, "-H", 0 );
		}else{
		    execl( oakgtobdf, oakgtobdf, i_gpf->in_file, "-H", 0 );
		}
		return	EXEC_ERROR;
	case	-1:
		return	FORK_ERROR;
	default:
		break;
	}
	close( ofd[1] );
	if ( ( o_gpf->input = fdopen( ofd[0], "r" ) ) == NULL ) {
		return	FDOPEN_ERROR;
	}

	/* open pipe for target gpf */
	if ( !( w_gpf->out_file = GetTmpPath( targ_file ) ) ) {
		return	MKTMP_ERROR;
	}

#ifdef ROOT_ONLY
	if( ChkPcfFontFile( w_gpf->snf_file ) == 0 ) {
		sprintf(command, "%s > %s", bdftopcf, w_gpf->out_file) ;
	} else {
		COMM_SNF_POPEN( permission, w_gpf->snf_file, w_gpf->out_file, ep, buf, command ) ;
	}
	if ((w_gpf->output = popen(command, "w")) == NULL) {
		return(POPEN_ERROR);
	}
#else	/* ROOT_ONLY */
	if ( pipe( pfd ) != 0 ) {
		return	PIPE_ERROR;
	}
	switch ( fork() ) {
	case	0:
	    if ( ( out_fd = open( w_gpf->out_file, O_WRONLY | O_CREAT, 0664 ) ) < 0 ) {
		return	BDF_OPEN_OUT;
	    }
	    close( 0 );
	    if ( dup(pfd[0]) < 0 ) {
		return	DUP_ERROR;
	    }
	    close( pfd[0] );
	    close( pfd[1] );
	    close( 1 );
	    if( dup( out_fd ) < 0 ) {
		close( out_fd );
		return	DUP_ERROR;
	    }
	    close( out_fd );
	    /* pcf format */
	    if ( ChkPcfFontFile( w_gpf->snf_file ) == 0 ) {
		execl( bdftopcf, bdftopcf, 0 );
		return	PCFFONTC_ERROR;
	    }
	    /* snf format */
	    if( type )	permission = type ;
	    COMM_SNF_EXECLBDFTOSNF( permission, buf, w_gpf->snf_file ) ;
	    return SNFFONTC_ERROR;

	case	-1:
	    return FORK_ERROR;
	default:
		break;
	}
	close( pfd[0] );
	if ( ( w_gpf->output = fdopen( pfd[1], "w" ) ) == NULL ) {
	    return	FDOPEN_ERROR;
	}
#endif	/* ROOT_ONLY */
	return	0;
}



static	int
readGpf(
struct btophead *i_gpf,
char   	      	*buf,
int		startcode,
int		endcode,
int		num_gr,
FalGlyphRegion	*gr)
{
	int	nchar, rtn;

	if ( ( rtn = ReadBdfHeader( i_gpf, buf ) ) ) {
	    return rtn;
	}

	nchar = i_gpf->num_chars ;

	if ( ( i_gpf->code = (int *)malloc( sizeof(int) * nchar ) ) == NULL ) {
	    return MALLOC_ERROR;
	}

	if ( ( i_gpf->ptn = (char **)malloc( sizeof(char *) * nchar ) ) == NULL ) {
	    return MALLOC_ERROR;
	}

	return	readBdfToMemory( i_gpf, buf, startcode, endcode, num_gr, gr ) ;
}



static	int
readBdfHeaderAndPut(struct btophead *o_gpf, struct ptobhead *w_gpf, char *buf)
{
	FILE	*fp ;
	char	*p;
	int 	getstat, zoom, property ;

	getstat = zoom = property = 0 ;

	if( o_gpf->p_width || o_gpf->p_height ){
	    zoom = 1 ;
	}
	if ( (fp = fopen( w_gpf->out_file, "w" )) == NULL ) {
	    return BDF_OPEN_HEAD ;
	}

	while ( 1 ) {
		if ( fgets( buf, BUFSIZE, o_gpf->input ) == NULL ) {
			fclose(fp) ;
			return	BDF_INVAL;
		}
		p = buf;
		SCAN_TO_NONSP( p )
		if ( !strncmp( p, SIZE, strlen( SIZE ) ) ) {
			if ( ( sscanf( p, "SIZE %f%d",
			    &(o_gpf->bdf_point), &(o_gpf->bdf_xdpi))) != 2 ) {
				fclose(fp) ;
				return	BDF_INVAL;
			}
			fprintf( fp, "%s", buf );
			getstat |= 0x01;

		} else if ( !strncmp( p, FONTBOUNDINGBOX, strlen( FONTBOUNDINGBOX ) ) ) {
			if ( ( sscanf( p, "FONTBOUNDINGBOX %d%d%d%d",
			    &(o_gpf->bdf_width), &(o_gpf->bdf_height),
			    &(o_gpf->bdf_x), &(o_gpf->bdf_y) )) != 4) {
				fclose(fp) ;
				return	BDF_INVAL;
			}
			/* -width / -height */
			if( zoom ){
			    /* -width / -height */
			    if( o_gpf->p_width )  o_gpf->bdf_width  = o_gpf->p_width ;
			    if( o_gpf->p_height ) o_gpf->bdf_height = o_gpf->p_height ;
			    fprintf( fp, "FONTBOUNDINGBOX %d %d %d %d\n",
				o_gpf->bdf_width, o_gpf->bdf_height,
				o_gpf->bdf_x, o_gpf->bdf_y );
			}else{
			    fprintf( fp, "%s", buf );
			}
			getstat |= 0x02;

		} else if ( !strncmp( p, "ENDPROPERTIES", strlen( "ENDPROPERTIES" ) ) ) {
			fprintf( fp, "%s", buf );
			fclose(fp) ;
			getstat |= 0x04;
			break;
		} else {

		    if( zoom ){
			if ( !strncmp( p, "STARTPROPERTIES", strlen( "STARTPROPERTIES" ) ) ) {
			    property = 1 ;
			    fprintf( fp, "%s", buf );
			} else if ( !strncmp( p, "FONT ", strlen( "FONT " ) ) ) {
			    char	font[BUFSIZE] ;
			    if( property ){
				if( (sscanf( buf, "FONT \"%s\"", font )) != 1 ){
				    fclose(fp) ;
				    return	BDF_INVAL;
				}
				MkFontProp( o_gpf->bdf_height, font, create_font ) ;
				fprintf( fp, "FONT \"%s\"\n", create_font ) ;
			    }else{
				if( (sscanf( buf, "FONT %s", font )) != 1 ){
				    fclose(fp) ;
				    return	BDF_INVAL;
				}
				MkFontProp( o_gpf->p_height, font, create_font ) ;
				fprintf( fp, "FONT %s\n", create_font ) ;
			    }
			} else if ( !strncmp( p, "PIXEL_SIZE ", strlen( "PIXEL_SIZE " ) ) ) {
			    fprintf( fp, "PIXEL_SIZE %d\n", o_gpf->bdf_height ) ;
			} else if ( !strncmp( p, "AVERAGE_WIDTH ", strlen( "AVERAGE_WIDTH " ) ) ) {
			    fprintf( fp, "AVERAGE_WIDTH %d\n", o_gpf->bdf_height*10 ) ;
			} else if ( !strncmp( p, "POINT_SIZE ", strlen( "POINT_SIZE " ) ) ) {
			    fprintf( fp, "POINT_SIZE %d\n", o_gpf->bdf_height*10 );
			} else if ( !strncmp( p, "RESOLUTION ", strlen( "RESOLUTION " ) ) ) {
			    int resol ;
			    if( (sscanf( buf, "RESOLUTION %d\n", &resol )) != 1 ){
				fclose(fp) ;
				return	BDF_INVAL;
			    }
			    fprintf( fp, "RESOLUTION %d\n", (int)((resol<0)?DEF_RESOLUTION:resol) );
			} else if ( !strncmp( p, "X_HEIGHT ", strlen( "X_HEIGHT " ) ) ) {
			    fprintf( fp, "X_HEIGHT %d\n", o_gpf->bdf_height );
			} else if ( !strncmp( p, "QUAD_WIDTH ", strlen( "QUAD_WIDTH " ) ) ) {
			    fprintf( fp, "QUAD_WIDTH %d\n", o_gpf->bdf_width );
			} else if ( !strncmp( p, "FONT_ASCENT ", strlen( "FONT_ASCENT " ) ) ) {
			    fprintf( fp, "FONT_ASCENT %d\n", (o_gpf->bdf_height + o_gpf->bdf_y) );
			} else if ( !strncmp( p, "FONT_DESCENT ", strlen( "FONT_DESCENT " ) ) ) {
			    fprintf( fp, "FONT_DESCENT %d\n", -(o_gpf->bdf_y) );
			} else if ( !strncmp( p, "DEFAULT_CHAR ", strlen( "DEFAULT_CHAR " ) ) ) {
			    fprintf( fp, "DEFAULT_CHAR %d\n", ((firstRow << 8) + firstCol) );
			} else {
			    fprintf( fp, "%s", buf );
			}
		    } else {	/* zoom */
			fprintf( fp, "%s", buf );
		    }
		}

	}	/* while loop */
	fclose(fp) ;
	if ( getstat != 0x07 ) {
		return	BDF_INVAL;
	}

	return	0;
}


static
writeGpf(struct btophead *i_gpf, struct btophead *o_gpf, struct ptobhead *w_gpf)
{
	FILE	*fp ;
	char	rdbuf[BUFSIZE] ;

	/* put BDF header from temporary file */
	if ( (fp = fopen( w_gpf->out_file, "r" )) == NULL ) {
	    return BDF_OPEN_HEAD ;
	}
	while( fgets( rdbuf, BUFSIZE, fp ) != (char *)NULL ){
	    fprintf( w_gpf->output, "%s", rdbuf ) ;
	}
	if( feof(fp) != 0 ){
	    fclose(fp) ;
	}else{
	    fclose(fp) ;
	    return BDF_READ_HEAD ;
	}

	/* set specified width or height */
	w_gpf->bdf_width 	= o_gpf->bdf_width ;
	w_gpf->bdf_height 	= o_gpf->bdf_height ;
	w_gpf->bdf_x		= o_gpf->bdf_x;
	w_gpf->bdf_y		= o_gpf->bdf_y;
	w_gpf->bdf_point 	= o_gpf->bdf_point;
	w_gpf->bdf_xdpi		= o_gpf->bdf_xdpi;

	w_gpf->p_width		= i_gpf->bdf_width;
	w_gpf->p_height		= i_gpf->bdf_height;

	if( w_gpf->bdf_width != w_gpf->p_width ||
	    w_gpf->bdf_height != w_gpf->p_height ) {
	    w_gpf->zoomf = 1;
	}else{
	    w_gpf->zoomf = 0;
	}

	w_gpf->num_chars	= i_gpf->num_chars;
	w_gpf->code		= i_gpf->code;
	w_gpf->ptn		= i_gpf->ptn;

	return	WritePtnToBdf( w_gpf );
}



static int
check_width(char *prog_name, char *len_str)
{
	return( check_width_height( prog_name, len_str, 0) );
}


static int
check_height(char *prog_name, char *len_str)
{
	return( check_width_height( prog_name, len_str, 1) );
}

static int
check_width_height(char *prog_name, char *len_str, int mode)
/* width ... 0 , height ... 1 */
{
	int		ret;
	char	*tmp;

	ret = (int)strtol( len_str, &tmp, 10 );	/* <-- ret = atoi( len_str ) */
	if ((tmp == len_str) || (ret < MIN_FONT_WIDTH) || (ret > MAX_FONT_WIDTH)) {
		switch( mode ) {
		case 0:
			USAGE2("%s : The specification of the character width is improper.\"%s\"．\n",
			    prog_name, len_str);
			break;
		case 1:
			USAGE2("%s : The specification of the character height is improper.\"%s\"．\n",
			    prog_name, len_str);
			break;
		default:
			break;
		}
		return(0);
	}
	return( ret );
}



static	int
check_code(
char	*prog_name,
char	*code_str,
int	mode,		/* start_code ... 0, end_code ... 1 */
char	*charset,
int	num_gr,
FalGlyphRegion	*gr)
{
	int		err_flg = 0;
	int		code_num;
	char	*tmp;
	FalGIInf	*gi ;
	int		num_gi ;
	int		i, code ;

	code = (int)strtol(code_str, &tmp, 16);
	if ( tmp == code_str ) {
	    err_flg = 1;
	}

	if( COMM_SBFNT_ISSBFNT( charset ) ) {
	    code_num = code ;
	    CONVGLYPHINDEX( code_num ) ;
	} else {
	    if( fal_code_to_glyph( locale, code, &gi, &num_gi) ) {
		USAGE2("%s : Failed to convert code point into glyph index.\"%s\" \n", prog_name, code_str ) ;
		err_flg = 1;
	    }
	    if( num_gi==1 ){
		code_num = gi[0].glyph_index ;
		err_flg = 0 ;
	    }else{
		for( i=0; i<num_gi; i++ ) {
		    if( !strcmp( gi[i].charset_str, char_set ) ) {
			code_num = gi[i].glyph_index ;
			err_flg = 0 ;
			break ;
		    } else		err_flg = 1 ;
		}
	    }
	}
	if( !err_flg ) {
	    for( i=0; i<num_gr; i++ ) {
		if( (code_num >= gr[i].start) && (code_num <= gr[i].end) )
		    return( code_num );
	    }
	    err_flg = 1 ;
	}
	if (err_flg) {
	    switch( mode ) {
	    case 0:
		USAGE2("%s : The specification of the extract start code is improper.\"%s\"．\n",
		    prog_name, code_str);
		break;
	    case 1:
		USAGE2("%s : The specification of the extract end code is improper.\"%s\"．\n",
		    prog_name, code_str);
		break;
	    default:
		USAGE2("%s : The specification of the extract code is improper.\"%s\"．\n",
		    prog_name, code_str);
	    }
	    return(-1);
	}
	return(-1);

}



static	int
readBdfToMemory(
struct btophead *head,
char   *buf,
int	startcode,
int	endcode,
int	num_gr,
FalGlyphRegion	*gr)
{
	int	    code, mwidth, num_char, bsize, rtn;
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
	    if ( !IsInRegion( code, num_gr, gr ) &&
		( startcode > code || endcode < code )
	    ){
		continue;
	    }
	    getDefaultCh( (unsigned int)code ) ;

	    head->code[num_char] = code;
	    if ( ( ptn = head->ptn[num_char++] = (char *)malloc( bsize ) ) == NULL ) {
		return(MALLOC_ERROR);
	    }
	    if ((rtn = GetBdfPtn(head, buf, ptn, mwidth, bsize)) != 0) {
		return(rtn);
	    }
	}
	return(0);
}


static	void
getDefaultCh(unsigned int cbuf)
{
	if( (cbuf & 0xff) < firstCol )	firstCol = cbuf & 0xff ;
	if( (cbuf >> 8)   < firstRow )	firstRow = cbuf >> 8 ;
	if( (cbuf & 0xff) > lastCol )	lastCol = cbuf & 0xff ;
	if( (cbuf >> 8)   > lastRow )	lastRow = cbuf >> 8 ;
}


static	int
cpg_atoi(char *numstr)
{
	int	val ;
	char	*ep, *cbuf ;

	ep = (char *)strchr( numstr, '\0' ) ;
	val = (int)strtol( numstr, &cbuf, 10 ) ;
	if ( cbuf == numstr || cbuf != ep ) {
	    return -1 ;
	}
	return val ;
}

static	int
is_in_udcregion( int start, int end, int num_gr, FalGlyphRegion *gr )
{
	int	i ;
	if( start < gr[0].start && end < gr[0].start ){
	    return -1 ;
	}
	for( i=0; i<num_gr-1; i++ ){
	    if( (start > gr[i].end && start < gr[i+1].start )
	     && (end > gr[i].end && end < gr[i+1].start )
	    )
	    return -1 ;
	}
	if( start > gr[num_gr-1].end && end > gr[num_gr-1].end ){
	    return -1 ;
	}
	return 0 ;
}


static	void
MkFontProp(int font_height, char *font, char *output)
{
	char	fbuf[BUFSIZE], split[BUFSIZE] ;
	char	*ep, *pix_sz, *point_sz, *rsl_x, *av_width, *char_set ;

	snprintf( fbuf, sizeof(fbuf), "%s", font ) ;
	snprintf( split, sizeof(split), "%s", font ) ;
	ep = fbuf ;
	ep += strlen( fbuf ) ;
	ep-- ;
	if( *ep == '"' )	*ep = '\0' ;
	POINTXLFDELM( pix_sz, fbuf, XLFD_ELM_PIXEL_SIZE ) ;
	POINTXLFDELM( point_sz, split, XLFD_ELM_POINT_SIZE ) ;
	POINTXLFDELM( rsl_x, split, XLFD_ELM_RESOLUTION_X ) ;
	POINTXLFDELM( av_width, split, XLFD_ELM_AVERAGE_WIDTH ) ;
	POINTXLFDELM( char_set, fbuf, XLFD_ELM_CHARSET_REGISTRY ) ;
	*pix_sz = '\0' ;
	*av_width = '\0' ;

	sprintf(output, "%s-%d-%s-%d%s", fbuf,
			font_height,	/* pixel size */
			rsl_x,
			font_height*10,	/* average width */
			char_set );
}



static	void
put_error_and_exit(
struct btophead *ptob_in,
struct ptobhead *ptob_out,
int 	er_no,
char	*prog_name)
{
	ErrMsgTable_AndExit( er_no, ptob_in->in_file, ptob_out->out_file,
	    NULL, NULL, NULL, prog_name );
	return;
}



static	void
put_help(char *prog_name)
{
        USAGE1("Usage: %s -ixlfd oxlfd  -oxlfd oxlfd\n", prog_name);
	USAGE("\t[in_character_size][out_character_size]\n");
	USAGE("\t[-start start_code][-end end_code]\n");
	USAGE("\t[-istyle ostyle][-ostyle ostyle]");
	COMM_HELP_MSG ;
	USAGE1("%s can copy glyphs of gpf file in the following code area.\n", prog_name);
	USAGE("codeset \t\tcode area\n");
	USAGE("----------------------------------------\n");
	DispUdcCpArea(stdout) ;
	USAGE("If the -start and -end option is omitted, the start/end code of each extractive area is specified.\n");
	USAGE("The xlfd name and character size may be obtained using dtlsgpf command.\n");
	return;
}
