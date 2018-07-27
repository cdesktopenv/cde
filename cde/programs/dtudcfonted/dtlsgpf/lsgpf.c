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
/* $XConsortium: lsgpf.c /main/7 1996/11/08 02:05:57 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include	<stdio.h>
#include	<errno.h>
#include	<locale.h>
#include	<string.h>

#include	"FaLib.h"
#include	"falfont.h"
#include	"bdfgpf.h"
#include	"udcutil.h"

static	char	*utillocale;
static	char	*fontdir;
static	char	*command_name;

typedef struct {
	int		num;
	struct {
		int	start;
		int	end;
	}entry[BUFSIZE];
}MISS_ENTRY;

/* request for sort */
#define	REQ_STYLE	(1<<1)
#define	REQ_CHARSIZE	(1<<2)
#define	REQ_LETTER	(1<<3)
#define	REQ_DELEQUAL	(1<<16)

/* error flag */
#define	NO_FONT		(1<<1)
#define	SAME_FONT	(1<<2)
#define	REDEFINED_FONT	(1<<3)

/* request flag */
#define	A_OPT	(1<<1)
#define	L_OPT	(1<<2)
#define	X_OPT	(1<<3)
#define	C_OPT	(1<<4)

static	void	dsp_info_list(char            *com,
			      int             dspopt,
			      FalFontDataList *lst,
			      MISS_ENTRY      *unknown_file_lst,
			      int             *errflg);
static	void	dsp_title(FILE *fp, int dspopt);
static	void	dspinfo_1line(FalFontData     data,
			      FILE            *fp,
			      MISS_ENTRY      *unknown_file_lst,
			      int             offset,
			      int             *errflg,
			      int             dspopt);
static	void	disp_no_fontslist(int dspopt);
static	void	sort_offset_list(FalFontDataList *flist,
				 int             mask,
				 int             **sort_list,
				 int             *data_cnt);
static	int	sort_fontlist(FalFontDataList *lst);
static	void	dsp_error_title(char *com, int *errflg, int dspopt);
static	int	dsp_error_files(FalFontDataList *lst, MISS_ENTRY *unknown);

static	void	put_help(char *prog_name);
static	int	search_fontfile_name(FalFontData     data,
				     char            *fname,
				     MISS_ENTRY      *unknown_file_lst,
				     int             offset,
				     int             *errflg);
static int	get_new_target(FalFontData *target, FalFontData *choose);

int
main( int argc, char *argv[] )
{
	FalFontDataList *lst_m;	/* font information list struct */
	FalFontData	key;	/* font reference information creation struct */
	MISS_ENTRY	unknown_m ;	/* error */
	int		i, j;
	int		code_sw;	/* code set flag */

	int		mask;
	int		code_set ;
	int		errflg ;
	int		dsp_opt ;

	/* initialize */
	command_name = argv[0] ;
	errflg = 0 ;
	unknown_m.num = 0;
	code_sw = 0;
	dsp_opt = 0 ;

	/* get locale strings */
	if( (utillocale = (char *)getenv( "LANG" )) == NULL ){
	    utillocale = "C" ;
	}

	if( (fontdir = (char *)getenv( "DTUDCFONTS" )) == (char *)NULL ) {
		fontdir = DTUDCFONTSLISTDIR ;
	}

	/*
	 * dtlsgpf check on which options are required.
	 */
	for (i=1 ; i < argc ; i++) {

	    if (!strcmp(argv[i], "-help")) {
		put_help(argv[0]);
		exit(0);
	    }
	    if (*argv[i] != '-') {
		put_help(argv[0]);
		exit(1);
	    }

	    if ( !strcmp( argv[i], "-codeset" ) ) {
		char	*cbuf ;
		if ( (i+1) >= argc ){
		    put_help( argv[0] );
		    exit( 1 );
		}
		i++;
		code_set = (int)strtol( argv[i], &cbuf, 10 ) ;
		if( argv[i] == cbuf ) {
		    put_help( argv[0] );
		    exit( 1 );
		}
		code_sw = 1 ;
	    }

	    for (j=1;  argv[i][j] != '\0' ; j++) {
		char	req ;
		req = argv[i][j] ;
		if ( req == 'l')	dsp_opt |= L_OPT ;
		else if ( req == 'a')	dsp_opt |= A_OPT ;
		else if ( req == 'x')	dsp_opt |= X_OPT ;
		else if ( req == 'C')	dsp_opt |= C_OPT ;
		else {
			put_help(argv[0]);
			exit( 1 );
		}
	    }
	}

	/* "-ax" -> "-a" */
	if( dsp_opt == ( A_OPT | X_OPT ) )	dsp_opt = A_OPT ;

	/*
	 * display information list of UDC code region
	 */
	if( dsp_opt & C_OPT ){
	    fprintf(stdout ,"codeset\t\tcode area\n");
	    fprintf(stdout ,"--------------------------------------------\n");
	    if ( DispUdcCpArea( stdout ) == -1 ){
		ErrMsgTable_FalGetFontList( argv[0], fal_utyerror, fal_utyderror );
		exit( 1 );
	    }
	    exit( 0 );
	}

	/*
	 * get an information list of UDC fonts
	 */
	mask = 0;
	mask = 	FAL_FONT_MASK_UNDEFINED | FAL_FONT_MASK_DEFINED ;
	if( code_sw ) {
		mask |= FAL_FONT_MASK_CODE_SET ;
		key.cd_set = FALGETFALCODESET( code_set ) ;
	}

	/* get font information list */
	if ( FalGetFontList( &key, mask, &lst_m ) == FAL_ERROR ){
		ErrMsgTable_FalGetFontList( argv[0], fal_utyerror, fal_utyderror );
		exit( 1 );
	}

	/* sort element of font infomation list */
	if( sort_fontlist( lst_m ) == FAL_ERROR ) {
		FalFreeFontList( lst_m );
		return FAL_ERROR ;
	}

	/*
	 * dtlsgpf shows UDC informations as user's request.
	 */
	dsp_title( stdout, dsp_opt );
	if ( lst_m->num == 0) {
	    disp_no_fontslist( dsp_opt ) ;
	} else {
	    dsp_info_list( argv[0], dsp_opt, lst_m, &unknown_m, &errflg );
	    if (unknown_m.num != 0) {
		if( dsp_opt & L_OPT ) {
		    dsp_error_title(argv[0], &errflg, dsp_opt );
		    if( dsp_error_files( lst_m, &unknown_m ) ) {
			exit( 1 );
		    }
		}
	    }
	}

	FalFreeFontList( lst_m );
	exit( 0 );
}




static void
dsp_info_list(
char		*com,
int 		dspopt,
FalFontDataList	*lst,
MISS_ENTRY	*unknown_file_lst,
int		*errflg)
{
	FalFontDataList	srt_lst;
	int	i, j ;
	int	entry, entry_end;
	int	ret;
	char	*filename;	/* file base's name */
	char	fname[BUFSIZE];
	char	tmpfname[BUFSIZE];
	int	srt_cnt, *sort_list, cnt, mask ;

	if( lst->num == 0 ){
	     return ;
	}

	srt_lst.num = lst->num ;
	srt_lst.list = (FalFontData *)malloc( sizeof(FalFontData) * lst->num ) ;
	if( srt_lst.list == NULL ) {
		USAGE("dtlsgpf : malloc error.\n" ) ;
		exit(1) ;
	}

	for (i=0, srt_cnt=0 ; i < lst->num ; i++) {
		int	letter_cmp;

		entry = entry_end = i;
		for (j=i ;
		    (j < lst->num) &&
		    !strcmp(lst->list[j].style.name, lst->list[entry].style.name) &&
		    (lst->list[j].size.h == lst->list[entry].size.h  ) ;
		    j++)
		{
		    if( (lst->list[j].letter.h == 0) ||
		        (lst->list[j].letter.w == 0) )	break ;
			entry_end = j;
		}

		if ( entry == entry_end ) {
		    if ( dspopt & L_OPT || dspopt & X_OPT )
			dspinfo_1line( lst->list[entry], stdout,
					unknown_file_lst, entry,
					errflg, dspopt );
		    else	memmove( &(srt_lst.list[srt_cnt++]),
					    &(lst->list[entry]),
					    sizeof(FalFontData) );
		    continue;
		}

		letter_cmp = lst->list[entry].letter.h;
		for (j=entry ; j <= entry_end ; j++) {
			if (lst->list[j].letter.h != letter_cmp) {
				break;
			}
		}

		if (j > entry_end) {
			ret = GetDefaultFile( lst->list[entry].size.h,
			    lst->list[entry].style.name, fname);
			if ( ret < 0 ) {
			    fname[0]='\0';
			    for ( j = entry; j <= entry_end; j++ ) {
				if ( dspopt & L_OPT || dspopt & X_OPT )
				    dspinfo_1line( lst->list[j], stdout,
						unknown_file_lst, j,
						errflg, dspopt );
				else	memmove(
					    &(srt_lst.list[srt_cnt++]),
					    &(lst->list[j]),
					    sizeof(FalFontData) );
			    }
			    i = entry_end;
			    continue ;
			}
			for( j = entry; j <= entry_end; j++ ) {
			    ret = search_fontfile_name(
				    lst->list[j], tmpfname,
				    unknown_file_lst, j, errflg
				     ) ;
			    if( ret == -1 )		continue ;
			    else if( ret == -2 )	exit(1) ;
			    filename = falcom_get_base_name( tmpfname ) ;
			    if ( !strcmp( filename, fname ) ){
				if ( dspopt & L_OPT || dspopt & X_OPT )
				    dspinfo_1line( lst->list[j], stdout,
						    unknown_file_lst, j,
						    errflg, dspopt );
				else	memmove(
					    &(srt_lst.list[srt_cnt++]),
					    &(lst->list[j]),
					    sizeof(FalFontData) );
				break;
			    }
			}
			i = entry_end;
			continue;
		}


		ret = GetDefaultFile(
				lst->list[entry].size.h,
				lst->list[entry].style.name,
				fname
			);
		if( ret ) {
		    switch ( ret ) {
			case	-1:
			    for ( j = entry; j <= entry_end; j++ ) {
				if ( dspopt & L_OPT || dspopt & X_OPT )
				    dspinfo_1line(
						lst->list[j], stdout,
						unknown_file_lst, j,
						errflg, dspopt );
				else	memmove(
					    &(srt_lst.list[srt_cnt++]),
					    &(lst->list[j]),
					    sizeof(FalFontData) );
			    }
			    break ;
			case	-2:
			    unknown_file_lst->entry[ unknown_file_lst->num ].start = entry;
			    unknown_file_lst->entry[ unknown_file_lst->num ].end   = entry_end;
			    unknown_file_lst->num++;
			    *errflg |= REDEFINED_FONT ;
			    break ;
			case	-3:
			    USAGE4("%s : The user defined character information file cannot be read.\"%s/%s/%s \"\n\n",
			    com, fontdir, utillocale, UDFLIST_DEF );
			    exit(1) ;
			case	-4:
			    USAGE4("%s : The mistake is found in the description of the user defined character information file.\"%s/%s/%s \"\n\n",
			    com, utillocale, fontdir, UDFLIST_DEF );
			    for ( j = entry; j <= entry_end; j++ ) {
				if ( dspopt & L_OPT || dspopt & X_OPT )
				    dspinfo_1line( lst->list[j], stdout,
						unknown_file_lst, j,
						errflg, dspopt );
				else	memmove(
					    &(srt_lst.list[srt_cnt++]),
					    &(lst->list[j]),
					    sizeof(FalFontData) );
			    }
			    break ;
		    }
		    i = entry_end;
		    continue;
		}

		for ( j = entry; j <= entry_end; j++ ) {
			ret = search_fontfile_name(
				lst->list[j], tmpfname,
				unknown_file_lst, j, errflg
				) ;
			if( ret == -1 )		continue ;
			else if( ret == -2 )	exit(1) ;
			filename = falcom_get_base_name( tmpfname ) ;
			if ( !strcmp( filename, fname ) ){
			    if ( dspopt & L_OPT || dspopt & X_OPT )
				dspinfo_1line( lst->list[j], stdout,
					    unknown_file_lst, j,
					    errflg, dspopt );
			    else	memmove(
					    &(srt_lst.list[srt_cnt++]),
					    &(lst->list[j]),
					    sizeof(FalFontData) );
			    break;
			}
		}
		if ( j > entry_end ) {
			unknown_file_lst->entry[ unknown_file_lst->num ].start = entry;
			unknown_file_lst->entry[ unknown_file_lst->num ].end  = entry_end;
			unknown_file_lst->num++;
			*errflg |= REDEFINED_FONT ;
		}
		i = entry_end;
	}

	/*
	 * dtlsgpf displays the infomations of user defined characters.
	 */
	if ( !(dspopt & L_OPT) ) { /* display character size and style */
		srt_lst.num = srt_cnt ;

	    if ( dspopt & A_OPT )
		mask = REQ_STYLE | REQ_CHARSIZE | REQ_LETTER | REQ_DELEQUAL ;
	    else
		mask = REQ_CHARSIZE | REQ_DELEQUAL ;

		sort_offset_list( &srt_lst, mask, &sort_list, &cnt ) ;

		for (i=0 ; i<cnt; i++)
		    dspinfo_1line( srt_lst.list[sort_list[i]], stdout,
				    unknown_file_lst, sort_list[i],
				    errflg, dspopt );
		free( sort_list ) ;
		free( srt_lst.list ) ;
	}
}




static void
dsp_error_title(char *com, int *errflg, int dspopt)
{
    USAGE("\n\n" );
    if( dspopt & A_OPT ) {
	if( *errflg & SAME_FONT )
	    USAGE1("%s :  There are fonts that are same character size and have same style, \ncode set and XLFD. It is not possible to select user defined character fonts uniquely.\n\n", com);
	USAGE("Please inquire of the system manager.\n\n");
    } else {
	if( *errflg & NO_FONT )
	    USAGE1("\n%s : Mistake is found in the name of user defined character fonts \nfollowing character size and style.\n\n", com);
	if( *errflg & REDEFINED_FONT )
	    USAGE3(" Please describe the following file name in the user defined character \ninformation file(%s/%s/%s).\n\n",
	    fontdir, utillocale, UDFLIST_DEF );
    }
}





static int
dsp_error_files( FalFontDataList *lst, MISS_ENTRY *unknown )
{
	int 	i,j;
	int 	entry_start, entry_end;
	char	*filename;
	char	tmp_fname[BUFSIZE];
	int	ret ;

	if ( unknown->num == 0 ){
		return	0;
	}
	for (i=0 ; i<unknown->num ; i++) {
		entry_start = unknown->entry[i].start;
		entry_end = unknown->entry[i].end;

		USAGE1("\tcode set \t%d \n",
			FALCODESETTONUM( lst->list[entry_start].cd_set ) ) ;
		USAGE1("\tcharacter_size \t%d\n",
			lst->list[entry_start].size.h ) ;
		USAGE1("\tstyle \t\t%s \n", lst->list[entry_start].style.name ) ;
		USAGE("\n" );
		for ( j = entry_start; j <= entry_end; j++ ) {
			ret = search_fontfile_name( lst->list[j], tmp_fname, NULL, 0, 0 ) ;
			if( ret == -1 ){
				USAGE("\t\tThere is no font file that is required." );
				USAGE1("\t%s\n", lst->list[j].xlfdname );
				continue ;
			} else if( ret == -2 ){
			    return -1 ;
			}
			filename = tmp_fname ;
			USAGE1("\t\t%s", filename );
			USAGE1("\t%s\n", lst->list[j].xlfdname );
		}
		USAGE("\n" );
	}
	USAGE("\n" );
	return	0;
}



#define SORTOFFSETNUM {\
	offset = size_offset_list[j] ;\
	size_offset_list[j] = size_offset_list[i] ;\
	size_offset_list[i] = offset ;\
}


static void
sort_offset_list(
FalFontDataList	*flist,
int		mask,
int		**sort_list,
int		*data_cnt)
{
	int	*size_offset_list ;
	int	cnt, i, j ;
	int	offset ;
	int	exist ;

	if( flist->num == 0 ){
	    return ;
	}

	if( (size_offset_list = (int *)malloc( flist->num * sizeof(int) )) == NULL )
	{
		USAGE("dtlsgpf: malloc error.\n" );
		exit(1) ;
	}
	for( i=0 ; i < flist->num; i++ )	size_offset_list[i] = -1 ;

	for( i=0, cnt=0; i < flist->num; i++ ) {
	    if( mask & REQ_DELEQUAL ) {
		exist = REQ_DELEQUAL ;
		for( j=0; j<cnt; j++ ) {
			if( size_offset_list[j] == -1 )	break ;
			if( (mask & REQ_CHARSIZE) &&
			(flist->list[size_offset_list[j]].size.h == flist->list[i].size.h)
			) {
				exist |= REQ_CHARSIZE ;
			}
			if( (mask & REQ_LETTER) &&
			(flist->list[size_offset_list[j]].letter.h == flist->list[i].letter.h)
			) {
				exist |= REQ_LETTER ;
			}
			if( (mask & REQ_STYLE) &&
			!strcmp(flist->list[size_offset_list[j]].style.name, flist->list[i].style.name)
			) {
				exist |= REQ_STYLE ;
			}
			if( exist == mask )	break ;
			else			exist = REQ_DELEQUAL ;
		}	/* size_offset_list */
		if( exist == mask )	continue ;
	    }	/* REQ_DELEQUAL */
	    size_offset_list[cnt++] = i ;
	}	/* flist */
	/* sort data */
	for( i=0; i<cnt-1; i++ ) {
	    for( j=i+1; j<cnt; j++ ) {
		if( mask & REQ_LETTER ) {
		    if( flist->list[size_offset_list[i]].letter.h >
			flist->list[size_offset_list[j]].letter.h)
		    {
			SORTOFFSETNUM ;
		    }
		}
		if( mask & REQ_CHARSIZE ) {
		    if( flist->list[size_offset_list[i]].size.h >
			flist->list[size_offset_list[j]].size.h)
		    {
			SORTOFFSETNUM ;
		    }
		}
		if( mask & REQ_STYLE ) {
		    if( strcoll(flist->list[size_offset_list[i]].style.name,
				flist->list[size_offset_list[j]].style.name) > 0 )
		    {
			SORTOFFSETNUM ;
		    }
		}
	    }
	}
	*sort_list = size_offset_list ;
	*data_cnt  = cnt ;
}



static void
dsp_title( FILE *fp, int dspopt )
{
    if( dspopt & L_OPT || dspopt & X_OPT ) {
	fprintf(fp, "\n" ) ;

	if( dspopt & X_OPT )	fprintf(fp, " code set" );
	if( dspopt & L_OPT )	fprintf(fp, "\tfilename\t" );
        if( (dspopt != X_OPT) && (dspopt != (L_OPT | X_OPT)) )
	    fprintf(fp, "character size\t" );
	COMM_DSP_LETTER_T ;
	if( dspopt & A_OPT )	fprintf(fp, "style \t" );
	COMM_DSP_PRM ;
	if( dspopt != X_OPT )	fprintf(fp, "\n" ) ;
	if( dspopt & X_OPT )	fprintf(fp, "\tXLFD\n" );
	fprintf(fp ,"-----------------------------------------------------------------------------\n");
    }
}





static void
dspinfo_1line(
FalFontData 	data,
FILE		*fp,
MISS_ENTRY	*unknown_file_lst,
int		offset,
int		*errflg,
int		dspopt)
{
	int	dsp_on ;
	char	*namep;
	char	tmp_fname[BUFSIZE];
	int	ret ;

	dsp_on = 0;

	/* get font informations */
	ret = search_fontfile_name( data, tmp_fname, unknown_file_lst, offset, errflg ) ;
	if( ret == -1 )		return ;
	else if( ret == -2 )	exit(1) ;
	namep = tmp_fname ;

	/* display font informations */
	if( dspopt & X_OPT )	fprintf( fp, "%d \t", FALCODESETTONUM( data.cd_set ) );

	if( dspopt & L_OPT )	fprintf( fp, "%s\t", namep );

	if( (dspopt != X_OPT) &&
	    (dspopt != (L_OPT | X_OPT))
	) {
	    fprintf( fp, "%d", data.size.h );
	}

	if( dspopt & A_OPT ) {
	    if( data.letter.h ) {
		if( !(dspopt & L_OPT) )
		    fprintf( fp, ".%d", data.letter.h );
		else
		    fprintf( fp, "\t%4d", data.letter.h );
	    }
	    fprintf( fp, "\t" );
	    fprintf( fp, "%s", data.style.name );
	}

	if( dspopt & L_OPT ) {
	    fprintf( fp, "\t" );
	    if( data.prm & FAL_FONT_DISPLAY ) {
		fprintf( fp, "display");
		dsp_on = 1;
	    }
	    if( data.prm & FAL_FONT_PRINTER ) {
		if( dsp_on ) {
		    fprintf( fp, " & ");
		}
		fprintf( fp, "printer");
	    }
	}
	if( dspopt != X_OPT ){
	    fprintf( fp, "\n");
	    if( dspopt & X_OPT )	fprintf( fp, "\t" );
	}
	if( dspopt & X_OPT )	fprintf( fp, "%s\n", data.xlfdname );
	if( dspopt != X_OPT ){
	    if( dspopt & X_OPT )	fprintf( fp, "\n" );
	}
}





static void
disp_no_fontslist( int dspopt )
{
    fprintf( stdout, "\nThere are no fonts that are used for user defined character.\n" ) ;
}


static void
put_help( char *prog_name )
{
	USAGE1("Usage : %s [-lax][-la][-lx][-C][-codeset number][-help]\n", prog_name);
	USAGE("\t-l       :  display of file name and character size\n");
	USAGE("\t-a       :  display of style(FAMILY_NAME) information\n");
	USAGE("\t-x       :  display of XLFD information\n");
	USAGE("\t-C       :  display of code area informations\n");
	USAGE("\t-codeset :  specification of the codeset\n");
	USAGE("\t-help    :  display of of the command parameter informations\n");

	USAGE("        (If the option is not specified, only the character size are displayed.)\n");
}



static	int
search_fontfile_name(
FalFontData	data,
char		*fname,
MISS_ENTRY	*unknown_file_lst,
int		offset,
int		*errflg)
{
	FalFontID	fid ;
	FalFontDataList	*flist ;
	int		mask ;
	char		*tmp_fname ;

	mask =
		FAL_FONT_MASK_UNDEFINED | FAL_FONT_MASK_DEFINED |
		FAL_FONT_MASK_XLFDNAME |
		FAL_FONT_MASK_SIZE_H |
		FAL_FONT_MASK_STYLE_NAME | FAL_FONT_MASK_CODE_SET
		;

	if( data.size.w >= 0 )	mask |= FAL_FONT_MASK_SIZE_W ;
	if( data.letter.w )	mask |= FAL_FONT_MASK_LETTER_W ;
	if( data.letter.h )	mask |= FAL_FONT_MASK_LETTER_H ;
	if( data.letter.x )	mask |= FAL_FONT_MASK_LETTER_X ;
	if( data.letter.y )	mask |= FAL_FONT_MASK_LETTER_Y ;

	if( data.style.def )	mask |= FAL_FONT_MASK_STYLE_DEF ;
	if( data.shape.name )	mask |= FAL_FONT_MASK_SHAPE_NAME ;
	if( data.shape.def )	mask |= FAL_FONT_MASK_SHAPE_DEF ;
	COMM_SBCHR_SETFONTDATA( data, mask ) ;
	if( data.prm )		mask |= FAL_FONT_MASK_PERMISSION ;

	fid = FalOpenSysFont( &data, mask, &flist ) ;
	if( fid == 0 ) {
		memset( fname, '\0', sizeof(fname) ) ;
		if( flist ){
		    FalFreeFontList( flist ) ;
		    if( unknown_file_lst ) {
			unknown_file_lst->entry[ unknown_file_lst->num ].start = offset ;
			unknown_file_lst->entry[ unknown_file_lst->num ].end  = offset ;
			unknown_file_lst->num++;
			*errflg |= SAME_FONT ;
		    }
		    return(-1);
		} else {
		  switch( fal_utyerror ) {
		   case _FAL_OPEN_ER :
		   case _FAL_READ_ER :
		   case _FAL_STAT_ER :
			switch( fal_utyderror ) {
			case EACCES :
			case ENOENT :
			    if( unknown_file_lst ) {
				unknown_file_lst->entry[ unknown_file_lst->num ].start = offset ;
				unknown_file_lst->entry[ unknown_file_lst->num ].end  = offset ;
				unknown_file_lst->num++;
				*errflg |= NO_FONT ;
			    }
			    return(-1) ;
			default :
			    USAGE2("System call error occurred. fal_utyerror = %d fal_utyderror = %d\n", fal_utyerror, fal_utyderror ) ;
			    return(-2) ;
			}
		   case _FAL_FONT_ER :
			if( unknown_file_lst ) {
			    unknown_file_lst->entry[ unknown_file_lst->num ].start = offset ;
			    unknown_file_lst->entry[ unknown_file_lst->num ].end  = offset ;
			    unknown_file_lst->num++;
			    *errflg |= NO_FONT ;
			}
			return(-1) ;
		   case _FAL_MALOC_ER :
			USAGE("malloc error occurred.\n" ) ;
			return(-2) ;
		   default :
			USAGE1("%s : cannot get font information list.\n", command_name ) ;
			return(-2) ;
		  }
		}
	}
	if( FalFontIDToFileName( fid, &tmp_fname ) == FAL_ERROR ) {
		USAGE1("%s : cannot get file name.\n", command_name ) ;
		FalCloseFont( fid ) ;
		return(-2) ;
	}
	strcpy( fname, tmp_fname ) ;
	FalFree( tmp_fname ) ;
	FalCloseFont( fid ) ;
	return(0) ;
}

static int
sort_fontlist( FalFontDataList *lst )
{
	int             i,j;
	FalFontDataList srt;
	FalFontData             target;
	int                     target_entry;

	if( lst->num == 0 ){
	     return(0) ;
	}

	srt.num  = lst->num;

	if (( srt.list = (FalFontData *)malloc(sizeof(FalFontData) * srt.num)) == NULL) {
		return	FAL_ERROR;
	}
	memmove (srt.list, lst->list, sizeof(FalFontData) * srt.num);
	/* start to sort */
	for ( i=0 ; i < srt.num -1 ; i++ ) {

		memmove( &target, &(srt.list[i]), sizeof(FalFontData));
		target_entry = i;

		for ( j=i+1 ; j < srt.num ; j++ ) {
			if ( get_new_target( &target, &(srt.list[j]) ) ) {
				memmove( &target, &(srt.list[j]), sizeof(FalFontData));
				target_entry = j;
			}
		}
		if (target_entry != i) {
			memmove( &target, &(srt.list[target_entry]),
			    sizeof(FalFontData)
			    );
			memmove( &(srt.list[i+1]), &(srt.list[i]),
			    sizeof(FalFontData)*(target_entry -i)
			    );
			memmove( &(srt.list[i]), &target,
			    sizeof(FalFontData)
			    );
		}
	}
	free(lst->list);
	lst->list = srt.list;
	return(0);
}


/* sort flag                      */
/* return data ¡§   sort    ... 1 */
/*                  no sort ... 0 */

static int
get_new_target( FalFontData *target, FalFontData *choose )
{
	FalFontData	diff;
	DEF_STR_CHK ;
	DEF_STR_SET ;

	diff.style.def = choose->style.def - target->style.def;
	str_chk.style =  strcmp( choose->style.name, target->style.name ) ;

	NEW_TARGET_SET( diff, choose, target, str_chk ) ;

	diff.shape.def = choose->shape.def - target->shape.def;
	if( (choose->shape.name != NULL) && (target->shape.name != NULL) ) {
	    str_chk.shape =  strcmp( choose->shape.name, target->shape.name ) ;
	}

	diff.cd_set   = choose->cd_set   - target->cd_set;
	diff.size.h   = choose->size.h   - target->size.h;
	diff.size.w   = choose->size.w   - target->size.w;
	diff.letter.h = choose->letter.h - target->letter.h;
	diff.letter.w = choose->letter.w - target->letter.w;

	/* codeset */
	if ( diff.cd_set < 0 ) 		return	1;
	if ( diff.cd_set > 0 ) 		return	0;

	/* character size height */
	if ( diff.size.h < 0 ) 		return	1;
	if ( diff.size.h > 0 ) 		return	0;

	/* letter size height */
	if ( diff.letter.h < 0 ) 	return	1;
	if ( diff.letter.h > 0 ) 	return	0;

	/* character size wide */
	if ( diff.size.w < 0 ) 		return	1;
	if ( diff.size.w > 0 ) 		return	0;

	/* letter size wide */
	if ( diff.letter.w < 0 ) 	return	1;
	if ( diff.letter.w > 0 ) 	return	0;

	/* style */
	if ( target->style.def == FAL_FONT_UNKNOWN ) {
		if ( choose->style.def > 0 ) 	return	1;
		/* style is FAL_FONT_UNKNOWN both "target" and "choose" */
		if ( str_chk.style < 0 ) 	return	1;
		if ( str_chk.style > 0) 	return	0;
	} else if ( choose->style.def == FAL_FONT_UNKNOWN ) {
		return	0;
	}
	/* target->style.def and choose->style.def is not FAL_FONT_UNKNOWN */
	if ( diff.style.def < 0 ) 	return	1;
	if ( diff.style.def > 0 ) 	return	0;

	NEW_TARGET_CHK( diff, choose, target, str_chk ) ;

	/* character style */
	if ( target->shape.def == FAL_FONT_UNKNOWN ) {
		if ( choose->shape.def > 0 ) 	return	1;
		if ( str_chk.shape < 0 ) 	return	1;
		if ( str_chk.shape > 0 ) 	return	0;
	} else if (choose->shape.def == FAL_FONT_UNKNOWN ) {
		return	0;
	}
	if ( diff.shape.def < 0 ) 	return	1;
	if ( diff.shape.def > 0 ) 	return	0;

	return	0;
}
