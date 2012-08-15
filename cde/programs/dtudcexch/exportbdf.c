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
/* $XConsortium: exportbdf.c /main/4 1996/04/10 13:49:20 ageorge $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 *
 *  Authors: Seiya Miyazaki     FUJITSU LIMITED
 *           Hiroyuki Chiba     FUJITSU LIMITED
 */
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#ifndef SVR4
#if !defined( SYSV )
#include <sys/resource.h>
#endif
#ifdef __osf__
#define _BSD
#endif
#include <sys/wait.h>
#ifdef __osf__
#undef _BSD
#endif
#else
#include <wait.h>
#endif
#include <memory.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "bdfgpf.h"

static	int 	writeBdfHeader();
static	int 	readBdfToMemory();
static	void	sigint_out();
static char	buf[BUFSIZE];
static	struct	ptobhead	w_bdf ;

static	void
sigint_out()
{
    fclose(w_bdf.output) ;
    fclose(w_bdf.input) ;	/* stream */
    exit( 0 );
}

int
expCheckCode( code, code_num, code_list )
unsigned int	code ;
int	code_num ;
int	*code_list ;
{
	int	i ;

	if( code < MIN_CODE || code > MAX_CODE )  return -1 ;
	for( i=0; i<code_num; i++ ){
	    if( code == code_list[i] )	return 0 ;
	}
	return -1 ;
}

int
ExpGpftoBDF( gpf_name, bdf_name, code_num, code_list, comment_num, comment_list, make_all )
char    *gpf_name ;     /* pointer to GPF file name area 	*/
char    *bdf_name ;     /* pointer to BDF file name area 	*/
int     code_num ;      /* number of GPF code 			*/
int     *code_list ;    /* pointer to GPF code lists 		*/
int     comment_num ;   /* number comments 			*/
char    **comment_list ;/* pointer to the list of comments 	*/
int	make_all ;	/* convert whole GPF fomat file to BDF  */
{
	struct	stat	statbuf ;
	struct	btophead	r_gpf ;
	int			rtn, i, num_chars ;

	/*
	 * parameter check
	 */
	if( gpf_name == NULL || bdf_name == NULL ){
	    fprintf(stderr, "GPF or BDF file name is not specified.\n" ) ;
	    return -1 ;
	}
	/*
	 * initiation
	 */
	memset( &w_bdf, 0, sizeof(struct ptobhead) ) ;
	memset( &r_gpf, 0, sizeof(struct btophead) ) ;

	if ( stat( SNFTOBDF, &statbuf ) ) {
	    if (!( oakgtobdf = get_cmd_path( getenv( "PATH" ), SNFTOBDF_CMD ))) {
		fprintf( stderr, "There is not %s command.\n", SNFTOBDF_CMD ) ;
		return -1 ;
	    }
	}else{
	    oakgtobdf = SNFTOBDF;
	}

	/*
	 * export glyphs in BDF format
	 */
	w_bdf.snf_file = gpf_name ;
	if( (w_bdf.output = fopen( bdf_name, "w" )) == NULL ){
	    fprintf(stderr, "\"%s\" cannot open.\n", bdf_name ) ;
	    return -1 ;
	}
	
	signal( SIGHUP , (void(*)())sigint_out );
	signal( SIGINT , (void(*)())sigint_out );
	signal( SIGQUIT, (void(*)())sigint_out );
	signal( SIGTERM, (void(*)())sigint_out );

	if( (rtn = writeBdfHeader(&w_bdf, comment_num, comment_list)) ){
	    fprintf(stderr, "\"%s\" cannot write header.\n", bdf_name ) ;
	    fclose(w_bdf.output) ;
	    fclose(w_bdf.input) ;	/* stream */
	    return rtn ;
	}
	
	r_gpf.bdf_width  	= w_bdf.bdf_width ;
	r_gpf.bdf_height 	= w_bdf.bdf_height ;
	r_gpf.input 		= w_bdf.input ;

	num_chars = ((make_all) ? w_bdf.num_chars : code_num) ;

	if( (r_gpf.code = (int *)malloc( sizeof(int) * num_chars)) == NULL ) {
	    fclose(w_bdf.output) ;
	    fclose(w_bdf.input) ;
	    return(MALLOC_ERROR);
	}

	if( (r_gpf.ptn = (char **)calloc( num_chars, sizeof(char *) )) == NULL ) {
	    fclose(w_bdf.output) ;
	    fclose(w_bdf.input) ;
	    return(MALLOC_ERROR);
	}

	if( (rtn = readBdfToMemory(&r_gpf, buf, code_num, code_list, make_all)) ){
	    fprintf(stderr, "\"%s\" cannot read glyph.\n", bdf_name ) ;
	    fclose(w_bdf.output) ;
	    fclose(w_bdf.input) ;
	    return rtn ;
	}
	fclose(w_bdf.input) ;
	wait(0) ;

	w_bdf.zoomf 	= 0 ;
	w_bdf.num_chars = r_gpf.num_chars ;
	w_bdf.code 	= r_gpf.code ;
	w_bdf.ptn  	= r_gpf.ptn  ;

	if( (rtn = WritePtnToBdf( &w_bdf, buf )) ){
	    fprintf(stderr, "\"%s\" cannot write glyph.\n", bdf_name ) ;
	    fclose(w_bdf.output) ;
	    return rtn ;
	}
	fclose(w_bdf.output) ;
	
	signal( SIGHUP , SIG_IGN );
	signal( SIGINT , SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGTERM, SIG_IGN );

	/*
	 * free memories
	 */
	free( r_gpf.code ) ;
	for(i=0; i<r_gpf.num_chars; i++){
	    if(r_gpf.ptn[i])	free(r_gpf.ptn[i]) ;
	}
	free( r_gpf.ptn ) ;

	return 0 ;
}

static	int
writeBdfHeader(head, comment_num, comment_list)
struct ptobhead *head;
int     comment_num ;   /* number comments 			*/
char    **comment_list ;/* pointer to the list of comments 	*/
{
	FILE		*fp;
	int		fd[2];
	unsigned int	getstat;
	char		buf[BUFSIZE], *p;

	int 		cnt ;
	int		comflg ;
	pid_t		chld_pid = 0;
#if defined( SVR4 ) || defined( SYSV ) || defined(__FreeBSD__)
	int		chld_stat ;
#else
	union	wait	chld_stat ;
#endif

	if (head->snf_file != NULL) {
		if (pipe(fd) != 0) {
			return	PIPE_ERROR;
		}
		switch (chld_pid = fork()) {
		case	0:
			close(1);
			if(dup(fd[1]) < 0) {
				return(DUP_ERROR);
			}
			close(fd[0]);
			close(fd[1]);
			execl( oakgtobdf, oakgtobdf, head->snf_file, 0);
			return	EXEC_ERROR;
		case	-1:
			return(FORK_ERROR);
		default:
			break;
		}
		close(fd[1]);
		if((fp = (FILE *)fdopen(fd[0], "r")) == NULL) {
			close( fd[0] );
			kill( chld_pid, SIGKILL );
			WaitID( chld_pid, chld_stat ) ;
			return	FDOPEN_ERROR;
		}
	} else {
		return(BDF_OPEN_HEAD);
	}
	head->input = fp ;
	getstat = 0;
	comflg = 0 ;

	while ( 1 ) {
		if (fgets(buf, BUFSIZE, fp) == NULL) {
		    fclose( fp );
		    if (head->snf_file != NULL) {
			close(fd[0]);
			kill( chld_pid, SIGKILL );
			WaitID( chld_pid, chld_stat ) ;
		    }
		    return(BDF_INVAL);
		}
		p = buf;
		SCAN_TO_NONSP(p);

		if (!strncmp(p, CHARS, CHARSsz)) {
		    if ((sscanf(p, "CHARS %d", &(head->num_chars))) != 1 ){
			return(BDF_INVAL);
		    }
		    getstat |= 0x04;
		    break;
		}
		/*
		 * write user comments
		 */
		if ( !strncmp(p, "FONT", strlen("FONT"))
		    && comment_list && !comflg
		) {
		    int	i ;
		    for( i=0; i<comment_num; i++ ){
			char	*ep ;
			if( (ep = (char *)strchr( comment_list[i], '\n' )) != NULL )
			    *ep = '\0' ;
			if( comment_list[i] == '\0' )	continue ;
			fprintf(head->output, "COMMENT %s\n", comment_list[i]);
		    }
		    fprintf(head->output, "COMMENT\n");
		    comflg++ ;
		}

		fprintf(head->output, "%s", buf);

		if (!strncmp(p, SIZE, SIZEsz)) {
		    if ((sscanf(p, "SIZE %f%d",
			&(head->bdf_point), &(head->bdf_xdpi))) != 2) {
			fclose(fp);
			if (head->snf_file != NULL) {
			    close(fd[0]);
			    kill( chld_pid, SIGKILL );
			    WaitID( chld_pid, chld_stat ) ;
			}
			return(BDF_INVAL);
		    }
		    getstat |= 0x01;
		    continue;
		}
		if (!strncmp(p, FONTBOUNDINGBOX, FONTBOUNDINGBOXsz)) {
		    if (( cnt = sscanf( p, "FONTBOUNDINGBOX %d%d%d%d", 
				&(head->bdf_width), &(head->bdf_height), 
				&(head->bdf_x), &(head->bdf_y))) != 4
		    ) {
			fclose(fp);
			if (head->snf_file != NULL) {
			    close(fd[0]);
			    kill( chld_pid, SIGKILL );
			    WaitID( chld_pid, chld_stat ) ;
			}
			return	BDF_INVAL;
		    }
		    getstat |= 0x02;
		    continue;
		}
		get_charset_registry(head, p) ;
	}

	if (getstat != 0x07) {
		return	BDF_INVAL;
	}

	return	0;
}


static	int
readBdfToMemory(head, buf, code_num, code_list, make_all)
struct btophead *head;
char	*buf;
int     code_num ;      /* number of GPF code 			*/
int     *code_list ;    /* pointer to GPF code lists 		*/
int	make_all ;	/* convert whole GPF fomat file to BDF  */
{
	int	code, mwidth, num_char, bsize, rtn;
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
	    if ( !make_all ) {
		if ( expCheckCode(code, code_num, code_list) ) {
		    continue;
		}
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

