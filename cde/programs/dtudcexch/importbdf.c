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
/* $XConsortium: importbdf.c /main/5 1996/06/27 10:40:33 ageorge $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 *
 *  Authors: Seiya Miyazaki     FUJITSU LIMITED
 *           Hiroyuki Chiba     FUJITSU LIMITED
 *
 */



#include 	<stdio.h>
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

#include	<X11/Xmd.h>
#include	<X11/Xproto.h>
#include	"bdfgpf.h"
#include	"FaLib.h"

#include	<errno.h>

#define UNLINK_TMPFILE( file ){ \
        if ( unlink( file ) != 0 ) { \
                exit(-1) ; \
        } \
}

static impFileConvInit();
static impGetGpfInf();
static impReadBdfHeaderAndPut();
static impMergePtn();
static impModifyPtn();
static impInsertPtn();
static impWriteSnf();
static int impWritePtnToBdf();
static int impPutDefaultChars();
static int impReadBdfToMemory();

extern	int  expCheckCode();


static struct ptobhead WriteGpf;

static char	*targ_file = NULL;	/* UDC_file_name */


static void
sigint_out(void)
{
	if (WriteGpf.out_file) {
		UNLINK_TMPFILE( WriteGpf.out_file );
	}
	exit( 0 );
}




int
ImpBDFCodeList(char *bdf_name, int *code_num, int **code_list)
{
	int	*glyph_list ;
	FILE	*bdfp ;
	char	*p ;
	char	buf[2048] ;
	int	cnt ;

	/* open BDF file */
	if( (bdfp = fopen( bdf_name, "r" )) == NULL ){
	    return(-1);
	}

	/* get number of characters in BDF file */
	while ( 1 ) {
	    if( fgets( buf, BUFSIZE, bdfp ) == NULL) {
		return (BDF_INVAL);
	    }
	    p = buf;
	    SCAN_TO_NONSP( p )
	    if ( !strncmp( p, CHARS, (unsigned int)strlen( CHARS ) ) ) {
		if ( ( sscanf( p, "CHARS %d", code_num ) ) != 1 ) {
		    return  BDF_INVAL;
		}
		break ;
	    }
	}

	/* alloc memory for codes */
	if( (glyph_list = (int *)calloc( *code_num, (unsigned int)sizeof(int) )) == NULL ){
	    return(-1);
	}

	/* get codes of BDF file */
	cnt = 0 ;
	while( cnt < *code_num ) {
	    if (fgets(buf, BUFSIZE, bdfp ) == NULL) {
		return (BDF_INVAL);
	    }
	    p = buf;
	    SCAN_TO_NONSP( p )
	    if (!strncmp(p, ENDFONT, (unsigned int)ENDFONTsz)) {
		break;
	    }else if (!strncmp(p, ENCODING, (unsigned int)ENCODINGsz)) {
		if ((sscanf(p, "ENCODING %d", &glyph_list[cnt] )) != 1) {
		    return(BDF_INVAL);
		}
		cnt ++ ;
	    }
	}
	if( cnt != *code_num ){
		return(-1) ;
	}

	/* close BDF file */
	if( fclose( bdfp ) ){
	    return(-1);
	}

	*code_list = glyph_list ;

	return(0) ;
}



int
ImpBDFCodeListFree(int **code_list)
{
	free( *code_list ) ;
	return(0) ;
}



int
ImpBDFCheck(
char	*bdf_name,	/* BDF file name */
char	*gpf_name	/* GPF file name */
)
{
	/* parameter check */
	if( bdf_name == NULL || gpf_name == NULL ){
		return(-1) ;
	}
	return(0) ;
}




int
ImpBDFtoGpf(
char	*bdf_name,
char	*gpf_name,
int	bdf_codenum,
int	*bdf_codelist
)
{
	int 	rtn ;
	int 	exit_stat;
	struct btophead	ReadGpf;
	struct btophead	ReadUdc;
	struct stat		statbuf;
	char		buf[BUFSIZE];

	int 	chk_fd;

	ReadGpf.in_file = ReadUdc.in_file = WriteGpf.out_file = NULL;

	if (!( bdftopcf = get_cmd_path( getenv( "PATH" ), BDFTOPCF_CMD ))) {
		bdftopcf = BDFTOPCF;
	}
	if (!( oakgtobdf = get_cmd_path( getenv( "PATH" ), SNFTOBDF_CMD ))) {
		oakgtobdf = SNFTOBDF;
	}
	if (!( bdftosnf = get_cmd_path( getenv( "PATH" ), BDFTOSNF_CMD ))) {
		bdftosnf = BDFTOSNF;
	}

	/*
	* set input/output file name
	*/

	ReadUdc.in_file = bdf_name ;
	ReadUdc.code_category = ALL_CODE ;
	ReadUdc.start_code    = MIN_CODE ;
	ReadUdc.end_code      = MAX_CODE ;

	ReadGpf.in_file = gpf_name ;
	ReadGpf.code_category = ALL_CODE ;
	ReadGpf.start_code    = MIN_CODE ;
	ReadGpf.end_code      = MAX_CODE ;

	/*
	* get real file name of GPF file
	*/
	if ((targ_file = GetRealFileName( ReadGpf.in_file )) == NULL){
		return(-1);
	}

	/*
	* WriteGpf.snf_file = ReadGpf.in_file
	*/
	WriteGpf.snf_file = targ_file;

	signal( SIGHUP , (void(*)())sigint_out );
	signal( SIGINT , (void(*)())sigint_out );
	signal( SIGQUIT, (void(*)())sigint_out );
	signal( SIGTERM, (void(*)())sigint_out );

	/*
	 * impFileConvInit()
	 *
	 * SNFTOBDF, BDFTOSNF
	 *
	 *  (ReadGpf.in_file)  -->  SNFTOBDF ==H
	 *                                     H
	 *					oakaddp  <-- (ReadUdc.in_file)
	 *                                     H
	 * (WriteGpf.out_file) <--  BDFTOSNF ==H
	 *
	 */

	/*
	* make a temporary file by BDF format corresponded to target GPF file
	*/
	if ( rtn = impFileConvInit( &ReadUdc, &ReadGpf, &WriteGpf ) ) {
		if ( WriteGpf.out_file ) {
			UNLINK_TMPFILE( WriteGpf.out_file );
		}
		return(-1) ;
	}

	/*
	* get informations from import file in BDF format
	*/
	if ( rtn = ReadBdfHeader( &ReadUdc, buf ) ) {
		if ( WriteGpf.out_file ) {
			UNLINK_TMPFILE( WriteGpf.out_file );
		}
		return(-1) ;
	}

	/*
	* get informations from target file in BDF format
	*/

	if ( rtn = impGetGpfInf( &ReadGpf, &WriteGpf, buf, bdf_codenum ) ) {
		if ( WriteGpf.out_file ) {
			UNLINK_TMPFILE( WriteGpf.out_file );
		}
		return(-1) ;
	}

	/* wait for dtgpftobdf */
	fclose( ReadGpf.input );
	wait( 0 );

	/*
	* merge the UDC glyphs into GPF file (in BDF format)
	*/
	if ( ( rtn = impMergePtn( &ReadUdc, &ReadGpf, buf,
				  bdf_codenum, bdf_codelist  ) ) ) {
		if ( WriteGpf.out_file ) {
			UNLINK_TMPFILE( WriteGpf.out_file );
		}
		return(-1) ;
	}

	/*
	* write the UDC data into GPF output file (in BDF format)
	*/
	if ( ( rtn = impWriteSnf( &ReadGpf, &WriteGpf ) ) ) {
		if ( WriteGpf.out_file ) {
			UNLINK_TMPFILE( WriteGpf.out_file );
		}
		return(-1) ;
	}

	/*
	* convert temporary file into GPF format
	*/
	fclose( ReadUdc.input );
	fclose( WriteGpf.output );
	wait( &exit_stat );
#if !defined( SVR4 ) && !defined( SYSV )
	if ( !WIFEXITED(exit_stat) ) {
#else
	if ( ! ( WIFEXITED(exit_stat) && !WEXITSTATUS(exit_stat) ) ) {
#endif
		UNLINK_TMPFILE( WriteGpf.out_file );
		return(-1) ;
	}
	signal( SIGHUP , SIG_IGN );
	signal( SIGINT , SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGTERM, SIG_IGN );

	sleep( 1 );
	if ( ( stat( WriteGpf.out_file, &statbuf ) ) || ( statbuf.st_size == 0 )
	    ) {
		UNLINK_TMPFILE( WriteGpf.out_file );
		return(-1) ;
	}
	if ( stat( WriteGpf.snf_file, &statbuf ) ) {
		UNLINK_TMPFILE( WriteGpf.out_file );
		return( -1 );
	}
	/*
	* convert the temporary file to target file
	*/
	return( Make_NewFefFile( WriteGpf.snf_file, WriteGpf.out_file,
	    FONT_FILE_PARM, (uid_t)statbuf.st_uid, (gid_t)statbuf.st_gid, "ImpBDFtoGpf()" ));
}


/*
* sub functions
*/

static
impFileConvInit(
struct btophead	*r_udc,
struct btophead	*r_gpf,
struct ptobhead	*w_gpf
)
{
	int 	fd[2], snf_fd, permission;
	char	buf[BUFSIZE];
	int 	pfd[2], ofd;

	FontInfoRec	*finf;
	char	*optlist[5] = {0,0,0,0,0};
	char	*ep ;
	int	i ;
	struct	stat	statbuf ;



	if ( ( r_udc->input = fopen(r_udc->in_file, "r")) == NULL ) {
		return	GPF_OPEN_IN;
	}

	/*
	* case of SNF file
	*/
	if ( ChkPcfFontFile( w_gpf->snf_file ) ) {
		/* snf */
		if ( ( snf_fd = open( w_gpf->snf_file, O_RDONLY ) ) >= 0 ) {
			return	BDF_OPEN_IN;
		}
	}

	if (pipe(fd) != 0) {
		return	PIPE_ERROR;
	}
	switch ( fork() ) {
	case	0:
		close( fd[0] );
		close( 1 );
		if ( dup( fd[1] ) < 0 ) {
			return	DUP_ERROR;
		}

		close( fd[1] );

		/* gpf -> bdf */
		execl( oakgtobdf, oakgtobdf, r_gpf->in_file, 0 );
		return	EXEC_ERROR;

	case	-1:
		return	FORK_ERROR;

	default:
		break;
	}
	close( fd[1] );
	if ( ( r_gpf->input = (FILE *)fdopen( fd[0], "r" ) ) == NULL ) {
		return	FDOPEN_ERROR;
	}

	if ( !( w_gpf->out_file = GetTmpPath( targ_file ) ) ) {
		return	MKTMP_ERROR;
	}

	if ( pipe( pfd ) != 0 ) {
		return	PIPE_ERROR;
	}
	switch ( fork() ) {
	case	0:
		if ( ( ofd = open( w_gpf->out_file, O_WRONLY | O_CREAT, 0664 ) ) < 0 ) {
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
		/*
		* case of PCF file format
		*/
		if ( ChkPcfFontFile( w_gpf->snf_file ) == 0 ) {
			execl( bdftopcf, bdftopcf, 0 );
			return	EXEC_ERROR;
		}
		/*
		* case of SNF file format
		*/
		COMM_SNF_EXECLBDFTOSNF( permission, buf, w_gpf->snf_file ) ;
		return	EXEC_ERROR;

	case	-1:
		return	FORK_ERROR;

	default:
		break;
	}
	close( pfd[0] );
	if ( ( w_gpf->output = (FILE *)fdopen( pfd[1], "w" ) ) == NULL ) {
		return	FDOPEN_ERROR;
	}
	return	0;
}





static
impGetGpfInf(
struct btophead *r_gpf,
struct ptobhead *w_gpf,
char   	      	*buf,
int		bdf_codenum
)
{
	int	    nchar, rtn;

	if ( ( rtn = impReadBdfHeaderAndPut( r_gpf, w_gpf, buf ) ) ) {
		return	rtn;
	}

	nchar = r_gpf->num_chars + bdf_codenum ;

	if ( ( r_gpf->code = (int *)malloc( sizeof(int) * nchar ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	if ( ( r_gpf->ptn = (char **)malloc( sizeof(char *) * nchar ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	return	ReadBdfToMemory( r_gpf, buf ) ;
}



static
impReadBdfHeaderAndPut(
struct btophead *r_gpf,
struct ptobhead *w_gpf,
char	*buf
)
{
	char	*p;
	unsigned int 	getstat = 0;

	while ( 1 ) {
		if ( fgets( buf, BUFSIZE, r_gpf->input ) == NULL ) {
			return	BDF_INVAL;
		}
		p = buf;
		SCAN_TO_NONSP( p )
		    if ( !strncmp( p, SIZE, (unsigned int)strlen( SIZE ) ) ) {
			if ( ( sscanf( p, "SIZE %f%d",
			    &(r_gpf->bdf_point), &(r_gpf->bdf_xdpi))) != 2 ) {
				return	BDF_INVAL;
			}
			fprintf( w_gpf->output, "%s", buf );
			getstat |= 0x01;

		} else if ( !strncmp( p, FONTBOUNDINGBOX, (unsigned int)strlen( FONTBOUNDINGBOX ) ) ) {
			if ( ( sscanf( p, "FONTBOUNDINGBOX %d %d %d %d",
			    &(r_gpf->bdf_width), &(r_gpf->bdf_height),
			    &(r_gpf->bdf_x), &(r_gpf->bdf_y) ) ) != 4 ) {
				return	BDF_INVAL;
			}
			fprintf( w_gpf->output, "%s", buf );
			getstat |= 0x02;

		} else if ( !strncmp( p, CHARS, (unsigned int)strlen( CHARS ) ) ) {
			if ( ( sscanf( p, "CHARS %d", &( r_gpf->num_chars ) ) ) != 1 ) {
				return	BDF_INVAL;
			}
			getstat |= 0x04;
			break;
		} else {
			fprintf( w_gpf->output, "%s", buf );
		}
	}

	if ( getstat != 0x07 ) {
		return	BDF_INVAL;
	}
	return	0;
}


static
impMergePtn(
struct	btophead *r_udc,
struct	btophead *r_gpf,
char	*buf,
int	bdf_codenum,
int	*bdf_codelist
)
{
	int 	code, rtn, msize, i, j;
	char	*ptn;

	if ( ( r_udc->bdf_width != r_gpf->bdf_width )
	    || ( r_udc->bdf_height != r_gpf->bdf_height )
	    ) {
		r_udc->zoomf = 1;

		msize = ( r_udc->bdf_width + 7 ) / 8 * r_udc->bdf_height;

		if ( ( ptn = (char *)malloc( msize ) ) == NULL ) {
			return	MALLOC_ERROR;
		}
		r_udc->ptn = &ptn;

	} else {
		r_udc->zoomf = 0;
	}

	for ( i = 0; i < r_udc->num_chars; i++ ) {
		if ( ( rtn = GetBdfCode( r_udc, buf, &code ) ) < 0 ) {
			return	rtn;
		} else if ( rtn == FILE_END ) {
			break;
		}

		if( expCheckCode( code, bdf_codenum, bdf_codelist ) ) {
			continue ;
		}

		for ( j = 0; j < r_gpf->num_chars; j++ ) {
			if ( r_gpf->code[j] == code ) {
				if ( ( rtn = impModifyPtn( r_udc, r_gpf, buf, j ) ) ) {
					return	rtn;
				}
				break;
			} else if ( r_gpf->code[j] > code ) {
				if ( ( rtn = impInsertPtn( r_udc, r_gpf, buf, code, j ) ) ) {
					return	rtn;
				}
				break;
			}
		}
		if ( j == r_gpf->num_chars ) {
			if ( ( rtn = impInsertPtn( r_udc, r_gpf, buf, code, j ) ) ) {
				return	rtn;
			}
		}
	}
	return	0;
}

static
impModifyPtn(struct btophead *r_udc, struct btophead *r_gpf, char *buf, int ix)
{
	int 	mwidth, msize, rtn;

	mwidth = ( r_udc->bdf_width + 7 ) / 8;
	msize = mwidth * r_udc->bdf_height;

	if ( r_udc->zoomf ) {
		if ( ( rtn = GetBdfPtn( r_udc, buf, r_udc->ptn[0], mwidth, msize ) ) ) {
			return	rtn;
		}
		if ( ( rtn = PtnZoom( r_gpf->ptn[ix], r_udc->ptn[0],
		    r_udc->bdf_width, r_udc->bdf_height,
		    r_gpf->bdf_width, r_gpf->bdf_height ) ) ) {
			return	rtn;
		}
	} else {
		if ( ( rtn = GetBdfPtn( r_udc, buf, r_gpf->ptn[ix], mwidth, msize ) ) ) {
			return	rtn;
		}
	}
	return	0;
}

static
impInsertPtn(
struct btophead *r_udc,
struct btophead *r_gpf,
char	*buf,
int 	code,
int 	ix
)
{
	int 	mwidth, msize, rtn, i;

	for ( i = r_gpf->num_chars; i > ix; i-- ) {
		r_gpf->code[i] = r_gpf->code[i-1];
		r_gpf->ptn[i] = r_gpf->ptn[i-1];
	}
	r_gpf->code[ix] = code;

	r_gpf->num_chars++;

	mwidth = (r_gpf->bdf_width + 7) / 8;
	msize = mwidth * r_gpf->bdf_height;

	if ( ( r_gpf->ptn[ix] = (char *)malloc( msize ) ) == NULL ) {
		return	MALLOC_ERROR;
	}

	if ( r_udc->zoomf ) {
		mwidth = (r_udc->bdf_width + 7) / 8;
		msize = mwidth * r_udc->bdf_height;
		if ( ( rtn = GetBdfPtn( r_udc, buf, r_udc->ptn[0], mwidth, msize ) ) ) {
			return	rtn;
		}
		if ( ( rtn = PtnZoom( r_gpf->ptn[ix], r_udc->ptn[0],
		    r_udc->bdf_width, r_udc->bdf_height,
		    r_gpf->bdf_width, r_gpf->bdf_height ) ) ) {
			return	rtn;
		}
	} else {
		if ( ( rtn = GetBdfPtn( r_udc, buf, r_gpf->ptn[ix], mwidth, msize ) ) ) {
			return	rtn;
		}
	}
	return(0);
}

static
impWriteSnf(struct btophead *r_gpf, struct ptobhead *w_gpf)
{
	w_gpf->zoomf = 0;
	w_gpf->num_chars  = r_gpf->num_chars;
	w_gpf->code       = r_gpf->code;
	w_gpf->ptn        = r_gpf->ptn;
	w_gpf->bdf_width  = r_gpf->bdf_width;
	w_gpf->bdf_height = r_gpf->bdf_height;
	w_gpf->bdf_x      = r_gpf->bdf_x;
	w_gpf->bdf_y      = r_gpf->bdf_y;
	w_gpf->bdf_point  = r_gpf->bdf_point;
	w_gpf->bdf_xdpi   = r_gpf->bdf_xdpi;

	return	WritePtnToBdf( w_gpf );
}
