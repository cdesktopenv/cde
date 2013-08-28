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
/* $XConsortium: cgm.h /main/6 1996/07/18 16:36:10 drk $ */
/* This is the module containing common definitions for all modules of */
/* the CGM Pixmap generator */
#include <stdio.h>		/* standard I/O package */
#include <stdlib.h>
#include <X11/Xlib.h>     /* XLib stuff */
#include <X11/Intrinsic.h>     /* Xt toolkit */
#include "GraphicsP.h"

extern int open_cgm_file(_DtGrStream *stream); 		/* in cgmio.c */
extern int open_cgm_buffer(_DtGrStream *stream); 		/* in cgmio.c */
/* in cgmcmds.c */
extern int xl_setup(Screen*, Display*, Window, int, Colormap, Visual*, GC,
		Dimension*, Dimension*, Pixel**, int*, Pixmap*, unsigned short);
/* the external entry point */

/* define the standard CGM elements and classes in terms of their CGM */
/* binary representations */
/* how many classes ? */
#define B_NOCLASSES 8
/* the Delimiter elements (Class 0) */
#define B_DELIMCLASS 0		/* Class Number */
#define B_DELIMSIZE 6		/* Class Size */
/* now the elements */
#define B_NOOP 0		/* No-op */
#define B_BMF 1			/* Begin Metafile */
#define B_EMF 2			/* End Metafile */
#define B_BPIC 3		/* Begin Picture */
#define B_BPICBODY 4		/* Begin Picture Body */
#define B_EPIC 5		/* End Picture */

/* the Metafile descriptor elements (Class 1) */
#define B_MDESCLASS 1		/* Class Number */
#define B_MDESSIZE 16		/* Class Size */
/* now the elements */
#define B_MFVERSION 1		/* Metafile version */
#define B_MFDESCRIP 2		/* Metafile descriptor */
#define B_VDCTYPE 3		/* VDC type */
#define B_INTPREC 4		/* Integer Precision */
#define B_REALPREC 5		/* Real precision */
#define B_INDEXPREC 6		/* Index precision */
#define B_COLPREC 7		/* Colour precision */
#define B_CINDPREC 8		/* Colour index precision */
#define B_MAXCIND 9		/* Maximum colour index */
#define B_CVEXTENT 10		/* Colour value extent */
#define B_MFELLIST 11		/* Metafile element list */
#define B_MFDEFREP 12		/* Metafile defaults replacement */
#define B_FONTLIST 13		/* Font list */
#define B_CHARLIST 14		/* Character set list */
#define B_CHARANNOUNCE 15	/* Character coding announcer */

/* the Picture descriptor elements (Class 2) */
#define B_PDESCLASS 2		/* Class Number */
#define B_PDESSIZE 8		/* Class Size */
/* now the elements */
#define B_SCALMODE 1		/* Scaling Mode */
#define B_COLSELMODE 2		/* Colour Selection Mode */
#define B_LWIDSPECMODE 3	/* Line Width Specification Mode */
#define B_MARKSIZSPECMODE 4	/* Marker Size Specification Mode */
#define B_EDWIDSPECMODE 5	/* Edge Width Specification Mode */
#define B_VDCEXTENT 6		/* VDC Extent */
#define B_BACKCOLR 7		/* Background Colour */

/* the Control elements (Class 3) */
#define B_CTRLCLASS 3		/* Class Number */
#define B_CTRLSIZE 7		/* Class Size */
/* now the elements */
#define B_VDCINTPREC 1		/* VDC Integer Precision */
#define B_VDCRPREC 2		/* VDC Real Precision */
#define B_AUXCOLR 3		/* Auxiliary Colour */
#define B_TRANSP 4		/* Transparency */
#define B_CLIPRECT 5		/* Clip Rectangle */
#define B_CLIPINDIC 6		/* Clip Indicator */

/* the Graphical Primitives (Class 4) */
#define B_GPRIMCLASS 4		/* Class Number */
#define B_GPRIMSIZE 20		/* Class Size */
/* now the elements */
#define B_POLYLINE 1		/* Polyline */
#define B_DISPOLY 2		/* Disjoint Polyline */
#define B_POLYMARKER 3		/* Polymarker */
#define B_TEXT 4		/* regular text */
#define B_RESTEXT 5		/* Restricted Text */
#define B_APPTEXT 6		/* Appended Text */
#define B_POLYGON 7		/* Polygon */
#define B_POLYSET 8		/* Polygon Set */
#define B_CELLARRAY 9		/* Cell Array */
#define B_GENDPRIM 10		/* Generalised Drawing Primitive */
#define B_RECTANGLE 11		/* Rectangle */
#define B_CIRCLE 12		/* Circle */
#define B_CIRC3 13		/* 3 pt circular arc */
#define B_CIRC3CLOSE 14		/* 3 pt circular arc, closed */
#define B_CIRCCENTRE 15		/* Circular arc specified by centre */
#define B_CIRCCCLOSE 16		/* Circular arc, centre, closed */
#define B_ELLIPSE 17		/* Ellipse */
#define B_ELARC 18		/* Elliptical arc */
#define B_ELARCCLOSE 19		/* Elliptical arc, closed */

/* the Attribute elements (Class 5) */
#define B_ATTRCLASS 5		/* Class Number */
#define B_ATTRSIZE 36		/* Class Size */
/* now the elements */
#define B_LBINDEX 1		/* Line Bundle index */
#define B_LTYPE 2		/* Line Type */
#define B_LWIDTH 3		/* Line Width */
#define B_LCOLR 4		/* Line Colour */
#define B_MBINDEX 5		/* Marker Bundle Index */
#define B_MTYPE 6		/* Marker Type */
#define B_MSIZE 7		/* Marker Size */
#define B_MCOLR 8		/* Marker Colour */
#define B_TBINDEX 9		/* Text Bundle Index */
#define B_TFINDEX 10		/* Text Font Index */
#define B_TPREC 11		/* Text Precision */
#define B_CEXPFAC 12		/* Character Expansion Factor */
#define B_CSPACE 13		/* Character Spacing */
#define B_TCOLR 14		/* Text Colour */
#define B_CHEIGHT 15		/* Character Height */
#define B_CORIENT 16		/* Character Orientation */
#define B_TPATH 17		/* Text Path */
#define B_TALIGN 18		/* Text Alignment */
#define B_CSETINDEX 19		/* Character Set Index */
#define B_ALTCSETINDEX 20	/* Alternate Character Set Index */
#define B_FILLBINDEX 21		/* Fill Bundle Index */
#define B_INTSTYLE 22		/* Interior Style */
#define B_FILLCOLR 23		/* Fill Colour */
#define B_HATCHINDEX 24		/* Hatch Index */
#define B_PATINDEX 25		/* Patern Index */
#define B_EDBINDEX 26		/* Edge Bundle Index */
#define B_ETYPE 27		/* Edge Type */
#define B_EWIDTH 28		/* Edge Width */
#define B_ECOLR 29		/* Edge Colour */
#define B_EVIS 30		/* Edge Visibility */
#define B_FILLREF 31		/* Fill Reference Point */
#define B_PATTAB 32		/* Pattern Table */
#define B_PATSIZE 33		/* Pattern Size */
#define B_COLTAB 34		/* Colour Table */
#define B_ASPSFLAGS 35		/* Aspect Source Flags */

/* the Escape element (Class 6) */
#define B_ESCCLASS 6		/* Class Number */
#define B_ESCSIZE 2		/* Class Size */
/* now the elements */
#define B_ESCAPE 1		/* Escape Flag */

/* the External elements (Class 7) */
#define B_EXTCLASS 7		/* Class Number */
#define B_EXTSIZE 3		/* Class Size */
/* now the elements */
#define B_MESSAGE 1		/* Message data */
#define B_APDATA 2		/* application data */

/* macro for error messages */
#define burp (void) fprintf

/* now the structure that maintains the state of the CGM file */
/* first some required typedefs and macros */
#define MAX_FONTS 10		/* maximum no. of fonts */
#define VDC_INT 0		/* integer vdc type */
#define VDC_REAL 1		/* real VDC type */
#define ABSTRACT_S 0		/* abstract scaling */
#define METRIC_S 1		/* metric scaling */
#define I_C_M 0			/* indexed colour mode */
#define D_C_M 1			/* direct colour mode */
#define ABSOLUTE 0		/* absolute mode */
#define SCALED 1		/* scaled mode */
#define OFF 0			/* boolean */
#define ON 1			/* boolean */
#define STRING_P 0		/* string precision */
#define CHARACTER_P 1		/* character precision */
#define STROKE_P 2		/* stroke precision */
#define RIGHT_P 0		/* right path */
#define LEFT_P 1		/* left path */
#define UP_P 2			/* up path */
#define DOWN_P 3		/* down path */
#define NORMAL_H 0		/* normal horizontal alignment */
#define LEFT_H 1		/* left horizontal alignment */
#define CENTRE_H 2		/* centre horizontal alignment */
#define RIGHT_H 3		/* right horizontal alignment */
#define CONT_H 4		/* continuous horizontal alignment */
#define NORMAL_V 0		/* normal vertical alignment */
#define TOP_V 1			/* top vertical alignment */
#define CAP_V 2			/* cap vertical alignment */
#define HALF_V 3		/* half vertical alignment */
#define BASE_V 4		/* base vertical alignment */
#define BOTTOM_V 5		/* bottom vertical alignment */
#define CONT_V 6		/* continuous vertical alignment */
/* fill styles */
#define HOLLOW_F 0
#define SOLID_F 1
#define PATTERN_F 2
#define HATCH_F 3
#define EMPTY_F 4
/* Hatch styles */
#define HOR_H 1			/* horizontal hatch */
#define VER_H 2			/* vertical hatch */
#define POS_H 3			/* positive slope hatch */
#define NEG_H 4			/* negative slope hatch */
#define VH_H 5			/* vertical and horizontal hatch */
#define LR_H 5			/* left and right horizontal hatch */
#define SOLID_L	1		/* solid line */
#define DASH_L	2		/* dashed line */
#define DOT_L	3		/* dotted line */
#define DASH_D_L 4		/* dash-dot line */
#define DASH_D_D_L 5		/* dash-dot-dot line */
#define NO_ASPSF 18		/* no of aspect source flags */
#define F_INDIV 0		/* individual flag */
#define F_BUNDLED 1		/* bundled flag */
/* now the individual aspect source flags */
#define F_LTYPE 0		/* Line Type */
#define F_LWIDTH 1		/* Line Width */
#define F_LCOLR 2		/* Line Colour */
#define F_MTYPE 3		/* Marker Type */
#define F_MSIZE 4		/* Marker Size */
#define F_MCOLR 5		/* Marker Colour */
#define F_TFINDEX 6		/* Text Font Index */
#define F_TPREC 7		/* Text Precision */
#define F_CEXPFAC 8		/* Character Expansion Factor */
#define F_CSPACE 9		/* Character Spacing */
#define F_TCOLR 10		/* Text Colour */
#define F_INTSTYLE 11		/* Interior Style */
#define F_FILLCOLR 12		/* Fill Colour */
#define F_HATCHINDEX 13		/* Hatch Index */
#define F_PATINDEX 14		/* Patern Index */
#define F_ETYPE 15		/* Edge Type */
#define F_EWIDTH 16		/* Edge Width */
#define F_ECOLR 17		/* Edge Colour */

/* the real precision type */
typedef struct rp_struct {int fixed; int exp; int fract;} rp_type;
/* type for values that can be real or integer */
typedef struct ri_struct {float r; int i;} ri_type;
/* type for values that can be real, integer, or scaled */
typedef struct ris_struct {float r; int i; float s;} ris_type;
/* the pattern table type */
typedef struct pat_struct { 
  int 			index;		/* pattern table index */
  int 			nx;		/* x dimension */
  int 			ny;		/* y dimension */
  int			l_c_p;		/* local colour precision */
  int			*array;		/* colour array */
  struct pat_struct	*next;		/* pointer to next one */
} pat_type;

/* the big type */
typedef struct cgm_state_struct {
  /* Metafile descriptors */
  int 		mfversion;		/* version number 	*/
  char		*mfdescrip;		/* metafile descriptor	*/
  int 		vdctype;		/* vdc type 		*/
  int 		intprec;		/* integer precision 	*/
  rp_type 	realprec;		/* real precision 	*/
  int 		indexprec;		/* index precision 	*/
  int		colprec;		/* colour precision 	*/
  int		cindprec;		/* colour index prec.	*/
  int		maxcind;		/* max. colour index 	*/
  int 		cvextent[6];		/* colour value extent	*/
  int		mfellist_s;		/* mf element list size */
  int		*mfellist;		/* mf element list 	*/
  char		*fontlist[MAX_FONTS];	/* font list		*/
  int		charlist_s;		/* character set size 	*/
  int		chartype[MAX_FONTS];	/* character set type	*/
  char		*charlist[MAX_FONTS];	/* character set list 	*/
  int		charannounce;		/* character announcer	*/
  /* picture descriptors */
  int		scalmode;		/* scaling mode 	*/
  float		scalmode_f;		/* metric scaling factor*/
  int		colselmode;		/* colour selection mode*/
  int		lwidspecmode;		/* line width spec mode */
  int		marksizspecmode;	/* marker size spec mode*/
  int		edwidspecmode;		/* edge width spec mode	*/
  ri_type	vdcextent[4];		/* vdc extent		*/
  int		backcolr[3];		/* background colour	*/
  /* control elements */
  int		vdcintprec;		/* vdc integer precision*/
  rp_type	vdcrprec;		/* vdc real precision	*/
  int		auxcolr[4];		/* auxiliary colour	*/
  int		transp;			/* transparency		*/
  ri_type	cliprect[4];		/* clipping rectangle	*/
  int		clipindic;		/* clipping indicator	*/
  /* attribute elements */
  int		lbindex;		/* line bundle index	*/
  int		ltype;			/* line type		*/
  ris_type	lwidth;			/* line width		*/
  int		lcolr[4];		/* line colour		*/
  int		mbindex;		/* marker bundle index	*/
  int		mtype;			/* marker type		*/
  ris_type	msize;			/* marker size		*/
  int		mcolr[4];		/* marker colour	*/
  int		tbindex;		/* text bundle index	*/
  int		tfindex;		/* text font index	*/
  int		tprec;			/* text precision	*/
  float		cexpfac;		/* character exp factor	*/
  float		cspace;			/* character space	*/
  int		tcolr[4];		/* text colour		*/
  Pixel		tPixel;			/* text colour		*/
  ri_type	cheight;		/* character height	*/
  ri_type	corient[4];		/* character orientation*/
  int		tpath;			/* text path		*/
  ri_type	talign[2];		/* text alignment	*/
  int		csetindex;		/* character set index	*/
  int		altcsetindex;		/* alt. char set index	*/
  int		fillbindex;		/* fill bundle index	*/
  int		intstyle;		/* interior style	*/
  int		fillcolr[4];		/* fill colour		*/
  int		hatchindex;		/* hatch index		*/
  int		patindex;		/* pattern index	*/
  int		edbindex;		/* edge bundle index	*/
  int		etype;			/* edge type		*/
  ris_type	ewidth;			/* edge width		*/
  int		ecolr[4];		/* edge colour		*/
  int		evis;			/* edge visibility	*/
  ri_type	fillref[2];		/* fill reference point	*/
  pat_type	*pattab;		/* pattern table	*/
  ri_type	patsize[4];		/* pattern size		*/
  int		*coltab;		/* colour table		*/
  int		aspsflags[NO_ASPSF];	/* aspect source flags	*/
} cgm_s_type;
/* return codes */
#define CGM_WARN 2	/* warning code */
/* structure to keep info on the command being processed */
struct cmd_info_s {
  int 	Class;		/* command Class */
  int 	element;	/* command element number */
  int 	no;		/* its command position in file */
  int 	sub_no;		/* its subcommand position in file */
  int	byte_no;	/* its byte position in file */
  int 	p_len;		/* its parameter length */
};
#if 0
/* now put in the strings we will use for diagnostics and listings */
/* use B_ATTRSIZE since it is the largest Class (of 8) */
static char *cgm_name[8][B_ATTRSIZE] = {
  {  /* Delimiters first */
    "No-Op",
    "Begin Metafile",
    "End Metafile",
    "Begin Picture",
    "Begin Picture Body",
    "End Picture"
    }
  ,
  {
    /* Metafile Descriptors */
    "",
    "Metafile Version",
    "Metafile Description",
    "VDC Type",
    "Integer Precision",
    "Real Precision",
    "Index Precision",
    "Colour Precision",
    "Colour Index Precision",
    "Maximum Colour Index",
    "Colour Value Extent",
    "Metafile Element List",
    "Metafile Defaults Replacement",
    "Font List",
    "Character Set List",
    "Character Coding Announcer"
    }
  ,
  {	/* Picture Descriptors */
    "",
    "Scaling Mode",
    "Colour Selection Mode",
    "Line Width Specification Mode",
    "Marker Size Specification Mode",
    "Edge Width Specification Mode",
    "VDC Extent",
    "Background Colour",
  }
  ,
  {	/* Control Elements */
    "",
    "VDC Integer Precision",
    "VDC Real Precision",
    "Auxiliary Colour",
    "Transparency",
    "Clip Rectangle",
    "Clip Indicator"
    }
  ,
  {	/* Graphical Primitives */
    "",
    "Polyline",
    "Disjoint Polyline",
    "Polymarker",
    "Text",
    "Restricted Text",
    "Append Text",
    "Polygon",
    "Polygon Set",
    "Cell Array",
    "Generalised Drawing Primitive",
    "Rectangle",
    "Circle",
    "Circular Arc 3 Point",
    "Circular Arc",
    "Circular Arc Centre",
    "Circular Arc Centre Close",
    "Ellipse",
    "Elliptical Arc",
    "Elliptical Arc Close",
  }
  ,
  {	/* Attribute Elements */
    "",
    "Line Bundle Index",
    "Line Type",
    "Line Width",
    "Line Colour",
    "Marker Bundle Index",
    "Marker Type",
    "Marker Size",
    "Marker Colour",
    "Text Bundle Index",
    "Text Font Index",
    "Text Precision ",
    "Character Expansion Factor",
    "Character Spacing",
    "Text Colour",
    "Character Height",
    "Character Orientation",
    "Text Path",
    "Text Alignment",
    "Character Set Index",
    "Alternate Character Set Index",
    "Fill Bundle Index",
    "Interior Style",
    "Fill Colour",
    "Hatch Index",
    "Pattern Index",
    "Edge Bundle Index",
    "Edge Type",
    "Edge Width",
    "Edge Colour",
    "Edge Visibility",
    "Fill Reference Point",
    "Pattern Table",
    "Pattern Size",
    "Colour Table",
    "Aspect Source Flags"
    }
  ,
  {	/* Escape Element */
    "",
    "Escape"
    }
  ,
  {	/* External Element */
    "",
    "Message",
    "Application Data"
    }
};
#endif

extern unsigned char *get_b_cmd(struct cmd_info_s*, 
				_DtGrStream *stream);	/* in cgmio.c */
extern int do_b_cmd(struct cmd_info_s*, unsigned char*,cgm_s_type*);
/* in cgmcmds.c */
extern int cgm_defs(cgm_s_type*);			/* in cgmcommon.c */
extern int cgm_assign(cgm_s_type*, cgm_s_type*);	/* in cgmcommon.c */
/* structure for the description of partial text pieces */
struct partialTextStruct {
  char *text;
  float cexpfac;
  float cspace;
  Pixel tcolr;
  float cheight;
  struct partialTextStruct *next;
};
typedef struct partialTextStruct partialText;




