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
/* $XConsortium: udcutil.h /main/3 1996/11/08 01:58:55 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#ifndef PRINT_USAGE
#define USAGE(a)
#define USAGE1(a, b)
#define USAGE2(a, b, c)
#define USAGE3(a, b, c, d)
#define USAGE4(a, b, c, d, e)
#else
#define USAGE(a)		fprintf(stderr, a)
#define USAGE1(a, b)		fprintf(stderr, a, b)
#define USAGE2(a, b, c)		fprintf(stderr, a, b, c)
#define USAGE3(a, b, c, d)	fprintf(stderr, a, b, c, d)
#define USAGE4(a, b, c, d, e)	fprintf(stderr, a, b, c, d, e)
#endif

#define	UDC_LINE_MAX	2048

/* macro for get character strings */

#define AllocString( str, orgstr, rtn ){ \
	(str) = (char *)malloc( sizeof(char) * (strlen((orgstr))+1) ) ;\
	if( !(str) ){\
		return( (rtn) ) ;\
	}\
	strcpy( (str), (orgstr) ) ;\
}

#define AddString( str, addstr, rtn ){ \
	(str) = (char *)realloc( (str), sizeof(char) * ( strlen((str)) + strlen((addstr)) + 1 ) ) ;\
	if( !(str) ){\
		return( (rtn) ) ;\
	}\
	strcat( (str), (addstr) ) ;\
}

#define ResizeString( str, rtn ){ \
	(str) = (char *)realloc( (str), sizeof(char) * ( strlen((str)) + 1 ) ) ;\
	if( !(str) ){\
		return( (rtn) ) ;\
	}\
}

#define FreeString( str ){ \
	if( (str) ) {\
		free( (str) ) ;\
	}\
	(str) = NULL ;\
}

/* access to "fonts.dir" */

#define   XLFD_ELM_CNT			14

#define XLFD_ELM_FOUNDRY		1
#define XLFD_ELM_FAMILY_NAME		2
#define XLFD_ELM_WEIGHT_NAME 		3
#define XLFD_ELM_SLANT			4
#define XLFD_ELM_SETWIDTH_NAME		5
#define XLFD_ELM_ADD_STYLE_NAME		6
#define XLFD_ELM_PIXEL_SIZE		7
#define XLFD_ELM_POINT_SIZE		8
#define XLFD_ELM_RESOLUTION_X		9
#define XLFD_ELM_RESOLUTION_Y		10
#define XLFD_ELM_SPACING		11
#define XLFD_ELM_AVERAGE_WIDTH		12
#define XLFD_ELM_CHARSET_REGISTRY	13
#define XLFD_ELM_CHARSET_ENCODING	14


static	int	udc_font_elm_cnt ;
static	char	udc_font_str_buffer[UDC_LINE_MAX] ;
static	char	*udc_font_sp ;

#define POINTXLFDELM( sp, xlfdname, elm_num ) { \
	udc_font_sp = (xlfdname) ; \
	udc_font_elm_cnt = 0 ;\
	while(*udc_font_sp){\
	    if(*udc_font_sp == '-')	udc_font_elm_cnt++ ;\
	    if( udc_font_elm_cnt == (elm_num) ){\
		break ;\
	    }\
	    udc_font_sp++ ;\
	}\
	(sp) = udc_font_sp ;\
}

#define GETXLFDELMSTR( sp, xlfdname, elm_num ) { \
	snprintf( udc_font_str_buffer, sizeof(udc_font_str_buffer), "%s", (xlfdname) ) ; \
	udc_font_sp = udc_font_str_buffer ; \
	udc_font_elm_cnt = 0 ;\
	while(*udc_font_sp){\
	    if(*udc_font_sp == '-')	udc_font_elm_cnt++ ;\
	    if( udc_font_elm_cnt == (elm_num) ){\
		udc_font_sp++ ;\
		break ;\
	    }\
	    udc_font_sp++ ;\
	}\
	(sp) = udc_font_sp ;\
	while(*udc_font_sp){\
	    if(*udc_font_sp == '-')	udc_font_elm_cnt++ ;\
	    if( udc_font_elm_cnt == (elm_num)+1 ){\
		*udc_font_sp = '\0' ;\
		break ;\
	    }\
	    udc_font_sp++ ;\
	}\
}

#define GETCHARSETSTR( char_set, xlfdname ) { \
	snprintf( udc_font_str_buffer, sizeof(udc_font_str_buffer), "%s", (xlfdname) ) ; \
	POINTXLFDELM( (char_set), udc_font_str_buffer, XLFD_ELM_CHARSET_REGISTRY ) ; \
	(char_set)++ ; \
}

#define GETSPACINGSTR( spacing, xlfdname ) { \
	GETXLFDELMSTR( (spacing), (xlfdname), XLFD_ELM_SPACING ) ; \
}
