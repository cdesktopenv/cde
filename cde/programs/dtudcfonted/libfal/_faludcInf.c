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
/* $XConsortium: _faludcInf.c /main/4 1996/10/15 10:28:13 drk $ */
/* udcInf.c 1.4 - Fujitsu source for CDEnext    96/03/19 13:41:00 	*/ 
/*
 * Copyright 1995 by FUJITSU LIMITED
 * This is source code modified by FUJITSU LIMITED under the Joint
 * Development Agreement for the CDEnext PST.
 * This is unpublished proprietry source code of FUJITSU LIMITED
 *
 * Modifier: Takanori Tateno   FUJITSU LIMITED
 *
 */
#include <stdio.h>
#include <locale.h>
#include "syncx.h"
#include "_fallibint.h"
#include "_fallcint.h"
#include "_fallcPubI.h"
#include "_fallcGeneric.h"
#include "_falomGeneric.h"

/*
   external symbols 
*/
extern FontData falread_EncodingInfo();
extern int fal_get_codeset();

extern int fal_utyderror ;
extern int fal_utyerror ;

extern unsigned long fal_utyerrno ;

#define FAL_ERROR -1


/*
    UDC structure define
*/
typedef struct _FalGlyphRegion {
    unsigned long  start;
    unsigned long  end;
} FalGlyphRegion ;

/*
 *  "code" no jyoui "i" byte me wo "unsigned char" toshite kaesu.
 */
static unsigned char getbyte(code,i)
unsigned long code;
int	      i;
{
    unsigned long byteL;
    unsigned char byte;
    byteL = code  >> (8*(3-i));
    byte = (unsigned char)(0x000000ff & byteL);
    return(byte);
}
/*
    get codeset which described by charset_str and locale.
    for examples ...
    locale          :  ja_JP
    charset_str     :  JISX0208.1983-0
*/

fal_get_codeset(locale,charset_str,codeset,num_codeset)
char *locale;
char *charset_str;
int  **codeset;
int  *num_codeset;
{
    XLCdRec lcdrec;
    XLCd     lcd;
    XLCdPublicRec xlcdp;
    XPointer rdb;
    int num = 0,count,num_ret=0,i,*ret;
    char **value,buf[128],*ptr;


    fal_utyderror = 0;
    fal_utyerror = 0;

    if((locale == NULL) || (charset_str == NULL)){
        fal_utyerror = 5;
        fal_utyderror = 1;
	fal_utyerrno = 0x04 ;
        return(FAL_ERROR);
    }
    if(codeset == NULL){
        fal_utyerror = 5;
        fal_utyderror = 2;
	fal_utyerrno = 0x04 ;
        return(FAL_ERROR);
    }

    /*    create XLCd     */
    xlcdp.pub.siname = locale;
    lcdrec.core = (XLCdCore)&xlcdp;
    lcd = &lcdrec;
        /* create X RDB (X NLS DB) */
    rdb = _fallcCreateLocaleDataBase(lcd);
    if(rdb == NULL){
        fal_utyerror = 1;
	fal_utyerrno = 0x15 ;
        return(FAL_ERROR);
    }

    for(num=0;;num++){
        /* XLC_FONTSET */
        sprintf(buf, "fs%d.font.primary", num);
        _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
        if(count < 1){
            break ;
        }
        for(i=0;i<count;i++){
            strcpy(buf,value[i]);
            ptr = (char *)strchr(buf,(int)':');
            *ptr = 0;
            if(!_fallcCompareISOLatin1(charset_str,buf)){
                num_ret += 1;
                if(num_ret == 1){
                    ret = (int *)Xmalloc(sizeof(int));
                } else {
		    ret = 
                        (int *)Xrealloc(ret,num_ret*sizeof(int)); 
                }
                if(ret == NULL){
		    fal_utyerrno = 0x03 ;
                    return(FAL_ERROR);
                }
		ret[num_ret-1]=num;
                break ;
            }
        }
    }
    if(num_ret == 0){
        *num_codeset = 0;
        *codeset = NULL;
        return (0xff);
    }

    *num_codeset = num_ret;
    *codeset = ret;
    return 0;
}

static Bool gi_to_vgi(gi,vgi,scope)
unsigned long 	gi,*vgi;
FontScope	scope;
{
    if(scope->shift_direction == (unsigned long)'+'){
        gi  -= scope->shift;
    } else {
        gi  += scope->shift;
    }
    if(gi >= scope->start && gi <= scope->end){
	*vgi = gi;
        return(True);
    }
    return(False);
}

static void shift_area(udc,scope)
FalGlyphRegion *udc;
FontScope scope;
{
    if(scope->shift_direction == (unsigned long)'+'){
        udc->start += scope->shift;
        udc->end   += scope->shift;
    } else {
        udc->start -= scope->shift;
        udc->end   -= scope->shift;
    }
}

/*
    get UDC area with glyph index.
    for examples ...
    locale          :  ja_JP
    charset_str     :  JISX0208.1983-0
*/
FalGetUDCGIArea(locale,codeset,charset_str,gr,num_gr)
char *locale;
int codeset;
char *charset_str;
FalGlyphRegion  **gr;
int  *num_gr;
{
    XLCdRec lcdrec;
    XLCd     lcd;
    XLCdPublicRec xlcdp;
    XPointer rdb;
    FalGlyphRegion *udc;
    int num = 0,count,num_ret=0;
    int i,j,k;
    char **value,buf[128],ptr;
    FontData font_data;


    fal_utyderror = 0;
    fal_utyerror = 0;
    fal_utyerrno = 0x00 ;

    if((locale == NULL) || (charset_str == NULL)){
        fal_utyerror = 5;
        fal_utyderror = 1;
	fal_utyerrno = 0x04;
	fal_utyerrno |= (0x0b<<8) ;
        return(FAL_ERROR);
    }
    if(gr == NULL){
        fal_utyerror = 5;
        fal_utyderror = 1;
	fal_utyerrno = 0x04;
	fal_utyerrno |= (0x0b<<8) ;
        return(FAL_ERROR);
    }
    if(num_gr == NULL){
        fal_utyerror = 5;
        fal_utyderror = 2;
	fal_utyerrno = 0x04;
	fal_utyerrno |= (0x0b<<8) ;
        return(FAL_ERROR);
    }

        /* create XLCd */
    xlcdp.pub.siname = locale;
    lcdrec.core = (XLCdCore)&xlcdp;
    lcd = &lcdrec;
        /* create X RDB (X NLS DB) */
    rdb = _fallcCreateLocaleDataBase(lcd);
    if(rdb == NULL){
        fal_utyerror = 1;
	fal_utyerrno = 0x15 ;
	fal_utyerrno |= (0x0b<<8) ;
        return(FAL_ERROR);
    }
    udc = NULL;

        /* XLC_FONTSET */
        sprintf(buf, "fs%d.charset.udc_area", codeset-1);
        _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
        if(count > 0){
            udc = (FalGlyphRegion *)Xmalloc(count * sizeof(FalGlyphRegion));
            if(udc == NULL){
		fal_utyerrno = 0x03 ;
		fal_utyerrno |= (0x0b<<8) ;
		return(FAL_ERROR);
            }
            for(i=0;i<count;i++){
                sscanf(value[i],"\\x%lx,\\x%lx", &(udc[i].start), &(udc[i].end));
            }
        }

    *num_gr = count;

        sprintf(buf, "fs%d.font.primary", codeset-1);
        _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
        if(count > 0){
	    font_data = falread_EncodingInfo(count,value);
            for(i=0;i<count;i++){
                if( !_fallcCompareISOLatin1(font_data[i].name,charset_str)){
                    for(j=0;j<(*num_gr);j++){
                    	for(k=0;k<font_data[i].scopes_num;k++){
			    if(udc[j].start == font_data[i].scopes[k].start
                                  && font_data[i].scopes[k].shift){
                                shift_area(&udc[j],&(font_data[i].scopes[k]));
                            }
			}
		    }
		}
	    }
        }

    *gr = udc;
    return 0;
}

/* 
 *      Code convert wo tomonau UDC area no kakutoku
 *      GetUDCCPArea() / glyph_to_code()           
 *
 */

int fal_gi_to_vgi(lcd,locale,charset_str,codeset,gi,vgi,charsetname)
XLCd 	lcd;
char 	*locale;
char 	*charset_str;
int 	codeset;
unsigned long 	gi;
unsigned long 	*vgi;
char    *charsetname;
{
    FalGlyphRegion *udc;
    int num = 0,count,num_ret=0;
    int i,j,k;
    char **value,buf[128],ptr;
    FontData font_data;


    sprintf(buf, "fs%d.charset.name", codeset-1);
    _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
    if(count > 0){
        strcpy(charsetname,value[0]);
    }
    sprintf(buf, "fs%d.font.primary", codeset-1);
    _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
    if(count > 0){
	font_data = falread_EncodingInfo(count,value);
        for(i=0;i<count;i++){
            if( !_fallcCompareISOLatin1(font_data[i].name,charset_str)){
                for(k=0;k<font_data[i].scopes_num;k++){
	            if( gi_to_vgi(gi,vgi,&(font_data[i].scopes[k])) == True){
			return(True);
                    }
	        }
	    }
        }
    }
/*
    free_fontdata(font_data);
*/
    *vgi = gi;
    return(True);
}

Bool falnon_standard(lcd,charset)
XLCd 	lcd;
XlcCharSet 	charset;
{
    char buf[256];
    int count,i;
    char **value;
    if(charset->ct_sequence == NULL){
            return(False);
    }
    for(i=0;;i++){
	sprintf(buf, "csd%d.charset_name", i);
	_fallcGetLocaleDataBase(lcd, "XLC_CHARSET_DEFINE", buf, &value, &count);
	if(count > 0){
	    if(!_fallcNCompareISOLatin1(value[0], 
			charset->name,strlen(value[0])) ){
		return(True);
	    }
	} else {
            return(False);
	}
    }
}

void
falmake_none_standard(from,charset,src)
char *from;
XlcCharSet 	charset;
char *src;
{
    int name_len,seq_len,i;
    name_len = 2 + strlen(charset->encoding_name) + 1;
    seq_len = strlen(charset->ct_sequence);
    strcpy(from,charset->ct_sequence);
    from[seq_len]    = name_len / 128 + 128;
    from[seq_len+1]  = name_len % 128 + 128;
    strcpy(&from[seq_len + 2],charset->encoding_name);
    from[seq_len+name_len-1]  = 0x02;  /* STX */
    strcpy(&from[seq_len + name_len],src);
}
int 
fal_glyph_to_code(locale,charset_str,codeset,glyph_index,codepoint)
char 	*locale;
char 	*charset_str;
int 	codeset;
unsigned long 	glyph_index;
unsigned long 	*codepoint;
{
    XLCd 	lcd;
    unsigned char *from; int	from_left;
    unsigned char *to  ; int 	to_left = 10;
    unsigned char *dst;
    unsigned char byte;
    unsigned long from32[25];
    unsigned long to32[25];
    int	     i,j;
    char tmp[256],charsetname[256],src[10];
    XlcConv 	conv;
    XlcCharSet 	charset;
    XPointer args[2];

    from = (unsigned char *)from32;
    to   = (unsigned char *)to32;
    dst  = (unsigned char *)to32;

    memset(dst,0,25);
	
    lcd = (XLCd)_fallcGenericLoader(locale);

    fal_gi_to_vgi(lcd,locale,charset_str,codeset,
	glyph_index,&glyph_index,charsetname);

    for(i=0,j=0;i<4;i++){
	byte = getbyte(glyph_index,i);
	if(byte){
	    src[j] = byte;
            j ++;
	}
    }
    src[j] = 0;


    /* get charset */
/*
    sprintf(tmp,"%s%s",charset_str,":GL");
    charset_name = strdup(tmp);
*/
    charset = _fallcGetCharSet(charsetname);
    if(charset == NULL){
	fal_utyerrno = 0x16 ;
	return(FAL_ERROR);
    }
    /* make ct */
    if( falnon_standard(lcd,charset)) {
        falmake_none_standard(from,charset,src);
    } else if(charset->ct_sequence){
        sprintf((char *)from,"%s%s",charset->ct_sequence,src);
    } else {
        sprintf((char *)from,"%s\0",src);
    }
    /* compound text -> multi byte */
    conv = _fallcOpenConverter(lcd, XlcNCompoundText, lcd, XlcNMultiByte);
    from_left = strlen((char *)from);
    _fallcConvert(conv,(XPointer *)&from,&from_left,
                     (XPointer *)&to,  &to_left,args,0);
    _fallcCloseConverter(conv);
    _fallcDestroyLC(lcd);

    *codepoint = 0;
    for(i=0;dst[i];i++){
        *codepoint = ((*codepoint << 8) | dst[i]) ;
    }
    return(0);
}

typedef struct _FalCodeRegion {
	unsigned long start,end;
} FalCodeRegion ;

int
FalGetUDCCPArea(locale,codeset,charset_str,cr,num_cr)
char 		*locale;
int 		codeset;
char 		*charset_str;
FalCodeRegion **cr;
int  *num_cr;
{
    int i,num_gr,ret;
    FalGlyphRegion *gr;
    FalCodeRegion  *crr;

    fal_utyerror  = 0;
    fal_utyderror = 0;

    if(cr == NULL){
        fal_utyerror  = 5;
        fal_utyderror = 1;
	fal_utyerrno = 0x04 ;
	fal_utyerrno |= (0x0a<<8) ;
        return(FAL_ERROR);
    }
    if(num_cr == NULL){
        fal_utyerror  = 5;
        fal_utyderror = 2;
	fal_utyerrno = 0x04 ;
	fal_utyerrno |= (0x0a<<8) ;
        return(FAL_ERROR);
    }

    ret = FalGetUDCGIArea(locale,codeset,charset_str,&gr,&num_gr);
    if(ret == FAL_ERROR){
	fal_utyerrno &= 0xff ;
	fal_utyerrno |= (0x0a<<8) ;
	return(ret);
    }

    crr = (FalCodeRegion *)Xmalloc(num_gr*sizeof(FalCodeRegion));
    if(crr == NULL){
	Xfree(gr);
	fal_utyerrno = 0x03 ;
	fal_utyerrno |= (0x0a<<8) ;
	return(FAL_ERROR);
    }

    for(i=0;i<num_gr;i++){
	ret = fal_glyph_to_code(locale,charset_str,codeset,
		gr[i].start, &(crr[i].start));
	if(ret == FAL_ERROR){
	    fal_utyerrno |= (0x0a<<8) ;
	    Xfree(gr);
	    Xfree(crr);
	    return(ret);
	}
	ret = fal_glyph_to_code(locale,charset_str,codeset,
		gr[i].end, &(crr[i].end));
	if(ret == FAL_ERROR){
	    fal_utyerrno |= (0x0a<<8) ;
	    Xfree(gr);
	    Xfree(crr);
	    return(ret);
	}
    }
    Xfree(gr);
    *cr = crr;
    *num_cr = num_gr;
    return(0);
}

/*
 *    code_to_glyph()
 *    
 */
typedef struct _FalGIInf {
    char 		*charset_str;
    unsigned long 	glyph_index;
} FalGIInf ;

/*
 *
 *
 */
static Bool vgi_to_gi(gi,vgi,scope)
unsigned long 	*gi,vgi;
FontScope	scope;
{
    if(vgi >= scope->start && vgi <= scope->end){
        if(scope->shift_direction == (unsigned long)'+'){
            *gi = vgi + scope->shift;
        } else {
            *gi = vgi - scope->shift;
        }
        return(True);
    }
    return(False);
}
/*
 *
 *
 */
static Bool  
fal_vgi_to_gi(lcd,locale,vglyph,glyph,charset,charsetname)
XLCd    lcd;
char    *locale;
unsigned long   vglyph;
unsigned long   *glyph;
XlcCharSet	charset;
char    *charsetname;
{
    int num = 0,count,num_ret=0;
    int i,j,k;
    char **value,buf[128],ptr;
    FontData font_data;
    CodeSet cs;

    
    for(i=0;;i++){
        sprintf(buf, "fs%d.charset.name",i);
        _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
        if(count > 0){
            if(!_fallcNCompareISOLatin1(charset->name,value[0],
			strlen(charset->name))){
		break;
	    }
        } else {
	    fal_utyerrno = 0x17 ;
	    return(False);
	}
    }
/*
    sprintf(buf, "fs%d.charset.name", codeset-1);
    _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
    if(count > 0){
        strcpy(charsetname,value[0]);
    }
*/
    sprintf(buf, "fs%d.font.primary", i);
    _fallcGetLocaleDataBase(lcd, "XLC_FONTSET", buf, &value, &count);
    if(count > 0){
	font_data = falread_EncodingInfo(count,value);
        for(i=0;i<count;i++){
            for(k=0;k<font_data[i].scopes_num;k++){
	        if( vgi_to_gi(glyph,vglyph,&(font_data[i].scopes[k])) == True){
		    strcpy(charsetname,font_data[i].name);
		    return(True);
                }
	    }
        }
    }
    *glyph = vglyph;
    return(True);
}
int
fal_code_to_glyph(locale,codepoint,gi,num_gi)
char 		*locale;
unsigned long 	codepoint;
FalGIInf 	**gi;
int 		*num_gi;
{
    XLCd        lcd;
    unsigned char *from; int    from_left;
    unsigned char *to  ; int    to_left = 10;
    unsigned char *dst;
    unsigned char byte;
    unsigned int from32[25];
    unsigned int to32[25];
    int      i,j;
    char tmp[256],charsetname[256],src[10];
    XlcConv     conv;
    XlcCharSet  charset;
    XPointer args[2];
    unsigned long glyph,vglyph;

    from = (unsigned char *)from32;
    to   = (unsigned char *)to32;
    dst  = (unsigned char *)to32;
    memset(dst,0,25);

    lcd = (XLCd)_fallcGenericLoader(locale);

    for(i=0,j=0;i<4;i++){
	byte = getbyte(codepoint,i);
	if(byte){
	    src[j] = byte;
            j ++;
	}
    }
    src[j] = 0;
    sprintf((char *)from,"%s\0",src);
    /* multi byte -> vgi */
    conv = _fallcOpenConverter(lcd, XlcNMultiByte, lcd, XlcNCharSet);
    from_left = strlen((char *)from);
    args[0] = (XPointer) &charset;
    _fallcConvert(conv,(XPointer *)&from,&from_left,
                     (XPointer *)&to,  &to_left,args,1);

    /* vgi -> gi */
    vglyph = 0;
    for(i=0;dst[i];i++){
        vglyph = ((vglyph << 8) | dst[i]) ;
    }
    if(fal_vgi_to_gi(lcd,locale,vglyph,&glyph,charset,charsetname)==False){
        _fallcCloseConverter(conv);
	_fallcDestroyLC(lcd);
        *num_gi = 0;
	return(0);
    }

    _fallcCloseConverter(conv);
    _fallcDestroyLC(lcd);

    *gi = (FalGIInf *)Xmalloc(sizeof(FalGIInf));
    (*gi)->charset_str = (char *)Xmalloc(strlen(charsetname)+1);
    strcpy((*gi)->charset_str,charsetname);
    (*gi)->glyph_index = glyph;
    if(*gi == NULL){
	fal_utyerrno = 0x03 ;
        return(FAL_ERROR);
    }
    *num_gi = 1;
    return(0);
}

