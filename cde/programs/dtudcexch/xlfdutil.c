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
/* $XConsortium: xlfdutil.c /main/2 1996/10/14 14:45:50 barstow $ */
/*
 *  All Rights Reserved, Copyright (c) FUJITSU LIMITED 1995
 *
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 *
 *  Authors: Seiya Miyazaki     FUJITSU LIMITED
 *           Hiroyuki Chiba     Fujitsu Basic Software Corporation
 *
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <nl_types.h>

#include <Xm/Xm.h>

#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/MessageB.h>

#include "FaLib.h"
#include "falxlfd.h"

#include "selectxlfd.h"

#ifdef DEBUG
#undef Dprintf
#define Dprintf fprintf
#else
#define Dprintf
#endif /* DEBUG */

/*
 * parameters
 */
static char* skipSpace() ;

extern	FalFontDataList		*font_L ;
extern	FalxFontDataBuff	KeyBuff ;
extern	FalxCodeList		*CodeList ;
extern	FalxStyleList		*StyleList ;
extern	FalxSizeList		*SizeList ;
extern	int			SelectedOffset ;
extern	int			getmask ;

extern char	*fullPathName ;


extern Widget	toplevel ;
extern Widget
	xlfdWform, cpyWform,
	slctBLabel[BUTTONITEMS],slctBText[BUTTONITEMS],	slctButton[BUTTONITEMS],
	listPop[BUTTONITEMS],	listW[BUTTONITEMS],
	errorMD, msgBox ;


/****************************************************************
 * functions for get font infomation
****************************************************************/

int
GetXLFDInfomations()
{
	Dprintf( stdout, "GetXLFDInfomations()\n" ) ;

	/* initialize */
	getmask		= 0 ;
	SelectedOffset	= 0 ;
	fullPathName	= NULL ;
	FalxInitDataBuff( &KeyBuff ) ;

	/* get UDC informations */
	if( FalxGetFontList( &font_L, XtDisplayOfObject( toplevel ) ) ){
		Dprintf( stderr, "GetXLFDInfomations() : FalxGetFontList() error.\n" ) ;
		return(-1);
	}

	/* create code area list */
	if( FalxCreateCodeList( &CodeList, font_L ) ){
		Dprintf( stderr, "\nGetXLFDInfomations() : FalxCreateCodeList() error.\n" ) ;
		return(-1);
	}

	/* create style list */
	if( FalxCreateStyleList( &StyleList, font_L ) ){
		Dprintf( stderr, "\nGetXLFDInfomations() : FalxCreateStyleList() error.\n" ) ;
		return(-1);
	}

	/* create size list */
	if( FalxCreateSizeList( &SizeList, font_L ) ){
		Dprintf( stderr, "\nGetXLFDInfomations() : FalxCreateSizeList() error.\n" ) ;
		return(-1);
	}

	return(0) ;
}





int
GetItemsToDisplay( int num, int *itemcnt, XmString **xms_list )
{
	int		i, j, cnt ;
	char		*sp ,string[256], buf[256] ;
	Arg		args[1];
	int		length, maxlength ;
	FalxXlfdList	*xlist ;

	int		tmp_cnt ;
	char		**tmp_dlist, *str ;
	XmString	*xms ;
	int		rlen ;
	int		rtn ;


	switch( num ){
	    case 0 :	/* XLFD		*/
		/* set key data         */
		for( i=1; i<BUTTONITEMS; i++ ){
		    if( (str = XmTextGetString( slctBText[i] )) == NULL ){
			exit(-1) ;
		    }
		    if( *str == ' ' || *str == NULL )	continue ;
		    if( SetKeyBuff( i, str ) ){
			exit(-1) ;
		    }
		}
		rtn = FalxGetXlfdList( &xlist, font_L, &KeyBuff, getmask ) ;
		if( rtn )	return( rtn ) ;
		tmp_cnt  = xlist->num ;
		tmp_dlist = xlist->list ;
		break ;
	    case 1 :	/* code area	*/
		tmp_cnt = CodeList->num ;
		tmp_dlist = (char **)malloc( sizeof(char *) * tmp_cnt ) ;
		if( tmp_dlist == NULL ) 	return(-1) ;
		for( i=0; i<tmp_cnt; i++ ){
		    sp = string ;
		    rlen = sprintf( sp, "CS:%d ", (CodeList->dlist[i]->fontset) ) ;
		    sp += rlen ;
		    for( j=0; j<CodeList->dlist[i]->code_area_num; j++ ){
			rlen = sprintf( sp, "%x - %x ",
				 CodeList->dlist[i]->alist[j]->udc_start,
				 CodeList->dlist[i]->alist[j]->udc_end   ) ;
			sp += rlen ;
		    }
		    if( (tmp_dlist[i] = strdup( string )) == NULL ){
			return(-1) ;
		    }
		}
		break ;
	    case 2 :	/* style	*/
		tmp_cnt  = StyleList->num ;
		tmp_dlist = (char **)malloc( sizeof(char *) * tmp_cnt ) ;
		for( i=0; i<tmp_cnt; i++ ){
		    if( (tmp_dlist[i] = strdup( StyleList->list[i] )) == NULL ){
			return(-1) ;
		    }
		}
		break ;
	    case 3 :	/* size		*/
		tmp_cnt = SizeList->num ;
		tmp_dlist = (char **)malloc( sizeof(char *) * tmp_cnt ) ;
		for( i=0; i<tmp_cnt; i++ ){
		    sprintf( string, "%d ", SizeList->list[i] ) ;
		    if( (tmp_dlist[i] = strdup( string )) == NULL ){
			return(-1) ;
		    }
		}
		break ;
	    default :	/* error	*/
		return(-1) ;
	}
	if( (xms = (XmString *)calloc( tmp_cnt, sizeof(XmString) )) == NULL ){
	    return( -1 ) ;
	}

	/*
	*	set list for display
	*/
	maxlength = 0 ;
	for ( i=0; i<tmp_cnt; i++ ){
	    xms[i] = XmStringCreateLocalized( tmp_dlist[i] );
	}

	/*
	*	free allocated memory
	*/

	switch( num ){
	    case 0 :	/* XLFD		*/
		FalxFreeXlfdList( xlist ) ;
		break ;
	    case 1 :	/* code area	*/
	    case 2 :	/* style	*/
	    case 3 :	/* size		*/
		for( i=0; i<tmp_cnt; i++ ){
		    free( tmp_dlist[i] ) ;
		}
		free( tmp_dlist ) ;
		break ;
	    default :	/* error	*/
		return(-1) ;
	}


	*itemcnt  = tmp_cnt ;
	*xms_list = xms ;

	return(0) ;

}




static char*
skipSpace( char *str, int skipcnt )
{
	int	i, spacecnt ;
	char	*sp ;
	int	len ;

	if( *str == NULL )	return( NULL ) ;
	len = strlen( str ) ;
	sp = str ;
	for( i=0,spacecnt=0; i<len-1; i++, sp++ ) {
	    if( *sp == NULL )	return( NULL ) ;
	    if( *sp == ' ' )	spacecnt++ ;
	    if( spacecnt == skipcnt )	break ;
	}
	sp++ ;
	return( sp ) ;
}




int
SetKeyBuff( int num, char *str )
{
	int	i, j ;
	int	cdset, start, end ;
	FalxUdcArea	*tmp_list ;
	char	hyp, *sp ;

	switch( num ){
	    case 0 :		/* XLFD		*/
		KeyBuff.FontData.xlfdname = str ;
		break ;
	    case 1 :		/* code area	*/
		getmask |= FAL_FONT_MASK_CODE_SET ;
		if( KeyBuff.FileData.list != NULL ){
		    free( KeyBuff.FileData.list ) ;
		}
		KeyBuff.FileData.list	= NULL ;
		tmp_list		= NULL ;

		sp = str ;
		if( sscanf( sp, "CS:%d ", &cdset ) != 1 ){
			return(-1) ;
		}
		if( (sp = skipSpace( sp, 1 )) == NULL )	return( -1 ) ;
		num = 0 ;
		KeyBuff.FontData.cd_set = cdset ;
		while( sscanf( sp, "%x %c %x ", &start, &hyp, &end ) == 3 ){
		    if( hyp != '-' )	break ;
		    if( tmp_list == NULL ){
			tmp_list = (FalxUdcArea *)malloc( sizeof(FalxUdcArea) ) ;
		    }else{
			tmp_list = (FalxUdcArea *)realloc( KeyBuff.FileData.list,
					 sizeof(FalxUdcArea) * (num+1) ) ;
		    }
		    if( tmp_list == NULL )	return(-1) ;
		    tmp_list[num].udc_start = (int)start ;
		    tmp_list[num].udc_end   = (int)end   ;
		    num ++ ;
		    if( (sp = skipSpace( sp, 3 )) == NULL )	break  ;
		}
		if( num == 0 )	return(-1) ;
		KeyBuff.FileData.code_area_num	= num ;
		KeyBuff.FileData.list		= tmp_list ;
		break ;
	    case 2 :		/* style	*/
		getmask |= FAL_FONT_MASK_STYLE_NAME ;
		KeyBuff.FontData.style.name = str ;
		break ;
	    case 3 :		/* size		*/
		getmask |= FAL_FONT_MASK_LETTER_W ;
		getmask	|= FAL_FONT_MASK_LETTER_H ;
		KeyBuff.FontData.letter.w = atoi( str ) ;
		KeyBuff.FontData.letter.h = atoi( str ) ;
		break ;
	    default :
		return(-1) ;
	}
	return(0);
}
