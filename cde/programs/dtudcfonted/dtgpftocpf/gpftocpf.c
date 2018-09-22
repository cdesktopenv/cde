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
/* $XConsortium: gpftocpf.c /main/6 1996/11/08 02:05:11 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include 	<stdio.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<sys/types.h>
#include        <sys/stat.h>
#ifndef SVR4
#if !defined ( SYSV )
#include        <sys/resource.h>
#endif
#include        <sys/wait.h>
#else
#include        <wait.h>
#endif
#include	<errno.h>
#include 	<locale.h>
#include	<string.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	"bdfgpf.h"

#include	<X11/Xmd.h>
#include	<X11/Xproto.h>
#include	"snfstruct.h"
#include	"FaLib.h"
#include	"udcutil.h"

#define	START_CODE	0
#define	END_CODE	1

static	int	CnvBDFtoGPF(struct btophead *head,
			    int num_gr,
			    FalGlyphRegion *gr,
			    int code_area);
static	int	check_width(char *prog_name, char *len_str);
static	int	check_height(char *prog_name, char *len_str);
static	int	check_width_height(char *prog_name, char *len_str, int mode);
static	int	check_code(char    *prog_name,
			   char    *code_str,
			   int     mode,
			   char    *charset,
			   int     num_gr,
			   FalGlyphRegion  *gr);
static	int	readBdfToMemory(struct btophead *head,
				char   *buf,
				int     num_gr,
				FalGlyphRegion  *gr,
				int     code_area);
static	void	sigint_out(void);
static	void	put_error_and_exit(struct btophead *btop,
				   int er_no,
				   char *prog_name);
static	void	put_help(char *prog_name);
static	void	Usage(char *prog_name);

extern	int	fal_code_to_glyph(char            *locale,
				  unsigned long   codepoint,
				  FalGIInf        **gi,
				  int             *num_gi);

static struct btophead Head;
static	char	*locale;
static	char	*spacing;
static	char	*char_set;


static	void
sigint_out(void)
{
	if ( Head.out_file ) {
		unlink( Head.out_file );
	}
	exit( 0 );
}

int main(int argc, char *argv[])
{
	int 	help, code_area, fd[2], rtn, i;
	struct stat	st;
	char   snf_file[BUFSIZE];
	char	*style ;
	int	code_no ;
	int	num_gr ;
	FalGlyphRegion	*gr ;
	char	*xlfdname, *cbuf, *ep ;
	char	*codeset = DEFAULT_CODESET ;
	char	*start_code, *end_code ;

	/* format */
	if( (locale = (char *)getenv( "LANG" )) == NULL ){
	    locale = "C" ;
	}

	Head.in_file = Head.out_file = Head.text_file = NULL;
	Head.p_width = Head.p_height = USE_ORG_SIZE;

	Head.code_category = ALL_CODE ;
	Head.start_code = MIN_CODE;
	Head.end_code   = MAX_CODE;
	help = code_area = 0;
	COMM_SETDEFAULTSTYLE( style ) ;
	xlfdname = cbuf = start_code = end_code = ep = '\0' ;
	spacing = char_set = NULL ;
	gr = '\0' ;
	num_gr = 0 ;

	if ( ! ( oakgtobdf = get_cmd_path( getenv( "PATH" ), SNFTOBDF_CMD ) ) ) {
		oakgtobdf = SNFTOBDF;
	}
	/*
	 *	check options
	 */
	for ( i = 1; i < argc; i++ ) {
		if ( !strcmp(argv[i], "-help" ) ) {
		    help = 1;
		} else if ( !strcmp(argv[i], "-system" ) ) {
		    code_area |= SYSTM;
		} else if ( !strcmp( argv[i], "-codeset" ) ) {
		    if (
			( (i+1) < argc ) &&
			( strlen( argv[i+1] ) == 1 )
		    ) {
			if ( *argv[i+1] == '1' ) code_area |= CDSET1;
			codeset = argv[++i] ;
		    } else {
			put_help( argv[0] );
			exit( 1 );
		    }
		} else if ( COMM_SBOPT_STRCMP( argv, i ) ) {
		    COMM_SBOPT_SETSBOPT( code_area ) ;
		} else {
			continue ;
		}
	}

	if ( help ) {
	    put_help( argv[0] );
	    exit( 0 );
	}

	ep = (char *)strchr( codeset, '\0' ) ;
	code_no = (int)strtol( codeset, &cbuf, 10 ) ;
	if( cbuf == codeset || cbuf != ep ) {
		USAGE1("%s : The code set number is not right.\n", argv[0] ) ;
		exit(1) ;
	}

	COMM_SBOPT_CHECKSBOPT( argv[0], code_area ) ;

	/*
	 *	get parameters
	 */
	for (i=1; i<argc; i++) {
		if ( !strcmp(argv[i], "-g") ) {
			if((i < argc-1) && (*argv[i+1] != '-')){
				Head.in_file = argv[++i];
			}

		} else if ( !strcmp(argv[i], "-p") ) {
			if ((i < argc-1) && (*argv[i+1] != '-')){
				Head.out_file = argv[++i];
			} else {
				USAGE1("%s : The output file name following -p option cannot be omitted.\n",
				    argv[0]);
				exit(1);
			}

		} else if ( !strcmp(argv[i], "-width") ) {
			if ((i < argc-1) && (*argv[i+1] != '-')) {
				Head.p_width =  check_width( argv[0], argv[++i]);
				if (Head.p_width == 0) {
					exit(1);
				}
			}
			else {
				USAGE1("%s : The character width is not specified.\n", argv[0]);
				exit( 1 );
			}

		} else if ( !strcmp(argv[i], "-height") ) {
			if ((i < argc-1) && (*argv[i+1] != '-')) {
				Head.p_height = check_height( argv[0], argv[++i]);
				if (Head.p_height == 0) {
					exit(1);
				}
			}
			else {
				USAGE1("%s :  The character height is not specified.\n", argv[0]);
				exit(1);
			}

		} else if ( !strcmp(argv[i], "-start") ) {
			if ((i < argc-1) && (*argv[i+1] != '-')) {
				start_code = argv[++i] ;
			} else {
				USAGE1("%s : The extract start code is not specified.\n",
				    argv[0]);
				exit( 1 );
			}

		} else if ( !strcmp(argv[i], "-end" ) ) {
			if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ) {
				end_code = argv[++i] ;
			} else {
				USAGE1("%s : The extract end code is not specified.\n",
				    argv[0]);
				exit( 1 );
			}

		} else if ( !strcmp(argv[i], "-style" ) ) {
			if ( ( i < argc-1 ) && ( *argv[i+1] != '-' ) ){
				style = argv[++i];
			} else {
				USAGE1("%s : The style is not specified.\n", argv[0]);
				exit( 1 );
			}
		} else if ( !strcmp(argv[i], "-xlfd" ) ) {
			if ( i < argc-1 ){
				xlfdname = argv[++i];
			} else {
				USAGE1("%s : The XLFD name is not specified.\n", argv[0]);
				exit( 1 );
			}
		} else if ( !strcmp(argv[i], "-help" )
			|| !strcmp(argv[i], "-system")
			|| COMM_SBOPT_STRCMP( argv, i )
		) {
			continue;

		} else if ( !strcmp( argv[i], "-codeset" ) ) {
			i++;
			continue;

		} else {
			Usage(argv[0]);
		}
	}

	if ( Head.in_file == NULL && xlfdname == NULL ) {
		if ( code_area & SYSTM ) {
			USAGE1("%s : The GPF output file name cannot be omitted.\n", argv[0] );
		} else {
			USAGE1("%s : The specification of the character size cannot be omitted.\n", argv[0] );
		}
		exit( 1 );
	}

	signal( SIGHUP , (void(*)())sigint_out );
	signal( SIGINT , (void(*)())sigint_out );
	signal( SIGQUIT, (void(*)())sigint_out );
	signal( SIGTERM, (void(*)())sigint_out );

	if ( !(code_area & SYSTM ) ) {

	    if( xlfdname ) {
		if( GetUdcFileName( argv[0], code_no, xlfdname, snf_file ) ) {
			USAGE1("%s : The font file name cannot be obtained. Terminates abnormally.\n", argv[0]);
			exit(1) ;
		}
		Head.in_file = snf_file;
	    } else {
		switch (GetFileName(argv[0], Head.in_file, style, code_no, snf_file)) {
		case	0:
			Head.in_file = snf_file;
			break;
		case	-1:
			exit(1);
		default:
			USAGE1("%s : The font file name cannot be obtained. Terminates abnormally.\n", argv[0]);
			exit(1);
		}
	    }
	    if( GetUdcRegion( argv[0], code_no, Head.in_file, &num_gr, &gr ) ) {
		    USAGE1("%s : The font file cannot get UDC code area.\n", argv[0]);
		    exit(1) ;
	    }
	} else {
	    num_gr = 1 ;
	    if( (gr = (FalGlyphRegion *)malloc( sizeof(FalGlyphRegion) * num_gr )) == NULL ) {
		USAGE1("%s : malloc error occurred. Terminates abnormally.\n", argv[0]);
		exit(1) ;
	    }
	    gr[0].start = MIN_CODE ;
	    gr[0].end   = MAX_CODE ;
	}
	/*
	 * refuse proportional fonts
	 */
	if ( GetUdcFontName( Head.in_file, NULL, &xlfdname ) ) {
	    USAGE1("%s : This font cannot get XLFD. Terminates abnormally.\n", argv[0]);
	    exit( 1 );
	}
	GETSPACINGSTR( spacing, xlfdname ) ;
	if ( !strcmp( "p", spacing ) || !strcmp( "P", spacing ) ) {
	    USAGE2("%s cannot edit proportional fonts.(SPACING \"%s\")\n", argv[0], spacing );
	    exit( 1 );
	}
	GETCHARSETSTR( char_set, xlfdname ) ;

	switch( code_area ) {
	    case SYSTM :
		Head.code_category	= ALL_CODE ;
		Head.start_code		= MIN_CODE ;
		Head.end_code		= MAX_CODE ;
		break ;
	    case SYSTM | CDSET1 :
		Head.code_category	= ALL_CODE ;
		Head.start_code		= MIN_CODE ;
		Head.end_code		= MAX_CODE ;
		break ;
	    default :
		COMM_SBFNT_SETUDCAREA( argv[0], Head, char_set, code_area, num_gr, gr ) ;
		break ;
	}

	if( start_code && end_code ) {
	    Head.start_code = check_code(argv[0], start_code, START_CODE, char_set, num_gr, gr );
	    if(Head.start_code < 0) {
		    exit( 1 );
	    }
	    Head.end_code = check_code(argv[0], end_code, END_CODE, char_set, num_gr, gr );
	    if(Head.end_code < 0) {
		    exit( 1 );
	    }
	}

	if ( Head.start_code > Head.end_code ) {
		USAGE1("%s : The specification of extract start/end code is improper.\n", argv[0]);
		exit( 1 );
	}

	COMM_SBFNT_CONVCODETOGI( char_set, Head ) ;

	/* open to output file */
	if ( Head.out_file == NULL ) {
		Head.output = stdout;
	} else {
		if ( stat(Head.out_file, &st) != 0 ) {
			if ((Head.output = fopen(Head.out_file, "w")) == NULL) {
				put_error_and_exit( &Head, GPF_OPEN_OUT, argv[0] );
			}
		} else {
			Head.text_file = Head.out_file;
			if ( ( Head.out_file = GetTmpPath( Head.out_file ) ) == NULL ) {
				put_error_and_exit( &Head, MKTMP_ERROR, argv[0] );
			}
			if ( ( Head.output = fopen( Head.out_file, "w" ) ) == NULL ) {
				put_error_and_exit( &Head, GPF_OPEN_OUT, argv[0] );
			}
		}
	}

	if ( pipe( fd ) != 0 ) {
		put_error_and_exit( &Head, PIPE_ERROR, argv[0] );
	}

	switch( fork() ) {
	case	0:
		close( fd[0] );
		close( 1 );
		if ( dup( fd[1] ) < 0 ) {
			put_error_and_exit( &Head, DUP_ERROR, argv[0] );
		}
		close( fd[1] );
		execl( oakgtobdf, oakgtobdf, Head.in_file, 0 );
		put_error_and_exit( &Head, EXEC_ERROR, argv[0] );

	case	-1:
		/* fork fail */
		put_error_and_exit( &Head, FORK_ERROR, argv[0] );

	default:
		close( fd[1] );
		Head.input = fdopen( fd[0], "r" );
	}

	if ( rtn = CnvBDFtoGPF( &Head, num_gr, gr, code_area ) ) {
		wait( 0 );
		fclose( Head.input );
		if ( Head.out_file ) {
			unlink( Head.out_file );
		}
		put_error_and_exit( &Head, rtn, argv[0] );
	}
	wait(0);
	fclose( Head.input );
	signal( SIGHUP,  SIG_IGN );
	signal( SIGINT,  SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGTERM, SIG_IGN );
	if ( Head.text_file ) {
		fclose( Head.output );
		Link_NewFile( Head.out_file, Head.text_file );
		unlink( Head.out_file );
	}
	exit( 0 );
}


static	int
CnvBDFtoGPF(
struct btophead *head,
int num_gr,
FalGlyphRegion *gr,
int code_area)
{
	int	    rtn;
	char    bdfbuf[BUFSIZE];

	/* readto BDF head information */
	if ((rtn = ReadBdfHeader(head, bdfbuf))) {
		return(rtn);
	}

	if (head->p_width == USE_ORG_SIZE) {
		head->p_width = head->bdf_width;
	}
	if (head->p_height == USE_ORG_SIZE) {
		head->p_height = head->bdf_height;
	}

	if ((head->p_width != head->bdf_width) ||
	    (head->p_height != head->bdf_height)) {
		head->zoomf = 1;
	} else {
		head->zoomf = 0;
	}

	if ((head->code = (int *)malloc(sizeof(int)*head->num_chars)) == NULL) {
		return(MALLOC_ERROR);
	}

	if ((head->ptn = (char **)malloc(sizeof(char *)*head->num_chars)) == NULL) {
		return(MALLOC_ERROR);
	}

	if ((rtn = readBdfToMemory(head, bdfbuf, num_gr, gr, code_area))) {
		return(rtn);
	}

	WriteGpfHeader(head);

	if ((rtn = WritePtnToGpf(head))) {
		return(rtn);
	}

	return(0);
}


static	void
put_error_and_exit(struct btophead *btop, int er_no, char *prog_name)
{
	ErrMsgTable_AndExit( er_no, btop->in_file, NULL,
	    NULL,          btop->out_file,
	    NULL,
	    prog_name
	    );
	return;
}





static	void
put_help(char *prog_name)
{
	USAGE1("Usage : %s -xlfd xlfd_name\n", prog_name);
	USAGE("\t\t[-g character_size][-p character_pattern_file_name]\n");
	USAGE("\t\t[-style style]\n");
	USAGE("\t\t[-start start_code] [-end end_code]\n");
	USAGE("\t\t[-width character_width] [-height character_height]\n");
	COMM_HELP_MSG ;
	USAGE1("%s can extract glyphs in cpf file format in the following code area.\n", prog_name);
	USAGE("codeset \t\tcode area\n");
	USAGE("----------------------------------------\n");
	DispUdcCpArea(stdout) ;
	USAGE("If the -start and -end option is omitted, the start/end code of each extractive area is specified.\n");
	USAGE("The xlfd name and character size may be obtained using dtlsgpf command.\n");
	return;
}

static	void
Usage(char *prog_name)
{
	put_help( prog_name );
	exit(1);
}


static	int
check_width(char *prog_name, char *len_str)
{
	return( check_width_height( prog_name, len_str, 0) );
}


static	int
check_height(char *prog_name, char *len_str)
{
	return( check_width_height( prog_name, len_str, 1) );
}

static	int
check_width_height(char	*prog_name, char *len_str, int mode)
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
	FalGIInf	*gi = NULL;
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

	XFree(gi);

	if( !err_flg ) {
	    for( i=0; i<num_gr; i++ ) {
		if( (code_num >= gr[i].start) && (code_num <= gr[i].end) )
		    return( code_num );
	    }
	}

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


static	int
readBdfToMemory(
struct btophead *head,
char   *buf,
int	num_gr,
FalGlyphRegion	*gr,
int	code_area)
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
	    if (
		( code < head->start_code) || ( code > head->end_code )
		||( !IN_CODE( head->code_category, SHIFT_ON( code ) ) )
		||(
		    (head->code_category == ALL_CODE) &&
		    !( code_area & SYSTM ) &&
		    IsInRegion( code, num_gr, gr )
		)
	    ) {
		continue;
	    }

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
