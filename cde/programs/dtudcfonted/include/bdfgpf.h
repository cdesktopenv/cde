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
/* $XConsortium: bdfgpf.h /main/7 1996/06/25 20:08:09 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "udccom.h"

struct btophead {
    char    *in_file;
    char    *out_file;
    char    *text_file;
    FILE    *input;
    FILE    *output;
    int	    num_chars;
    int     p_width;
    int     p_height;
    int	    bdf_width;
    int	    bdf_height;

	int 	bdf_x;
	int 	bdf_y;
    float   bdf_point;
    int     bdf_xdpi;
    int     *code;
    char    **ptn;
    int	    zoomf;

	int 	code_category;
    int	    start_code;
    int	    end_code;
};

struct ptobhead {
    char    *in_file;
    char    *out_file;
    char    *bdf_file;
    char    *snf_file;
    FILE    *input;
    FILE    *output;
    int     num_chars;
    int     p_width;	
    int     p_height;
    int     bdf_width;
    int     bdf_height;

	int 	bdf_x;
	int 	bdf_y;
    float   bdf_point;
    int     bdf_xdpi;

	int 	reserve ;
    int     *code;
    char    **ptn;
    int     zoomf;

	int 	code_category;
    int     start_code;
    int     end_code;
		
};

#define	MIN_CODE	0x0000
#define	MAX_CODE	0xFFFF

#define	ALL_CODE	0

extern char *oakgtobdf;	
extern char *bdftosnf;
extern char *bdftopcf;



#define TAB_CD		0x09
#define USE_ORG_SIZE    -1
#define MAX_FONT_WIDTH  999
#define MIN_FONT_WIDTH  2
#define MAX_FONT_HEIGHT 999
#define MIN_FONT_HEIGHT 2

#ifdef BUFSIZE
#undef BUFSIZE
#endif	/* BUFSIZE */
#define BUFSIZE	1024

/* KeyWord for BDF file */

#define STARTFONT		"STARTFONT "
#define STARTFONTsz		10	
#define FONTBOUNDINGBOX		"FONTBOUNDINGBOX "
#define FONTBOUNDINGBOXsz	16	
#define SIZE			"SIZE "
#define SIZEsz			5	
#define CHARS			"CHARS "
#define CHARSsz			6	
#define STARTCHAR		"STARTCHAR "
#define STARTCHARsz		10
#define ENCODING		"ENCODING "
#define ENCODINGsz		9
#define BITMAP			"BITMAP"
#define BITMAPsz		6
#define ENDCHAR			"ENDCHAR"
#define ENDCHARsz		7		
#define ENDFONT			"ENDFONT"
#define ENDFONTsz	        7	

#define CHARSET_REGISTRY	"CHARSET_REGISTRY "
#define CHARSET_REGISTRYsz	17


/* KeyWord for GPF file */

#define NUMFONTS		"numfonts:"
#define NUMFONTSsz        	9
#define WIDTH			"width:"
#define WIDTHsz			6
#define HEIGHT			"height:"
#define HEIGHTsz		7
#define CODE			"code:"
#define CODEsz			5
#define ENDDATA			"enddata"
#define ENDDATAsz		7

#define FILE_END	99

#define FATAL_ERROR	-1
#define	BDF_OPEN	-2
#define BDF_OPEN_IN	-2
#define BDF_READ	-3
#define BDF_WRITE	-4
#define BDF_INVAL	-5
#define	GPF_OPEN	-6
#define GPF_OPEN_IN	-6
#define GPF_READ	-7
#define GPF_WRITE	-8 
#define GPF_INVAL	-9
#define PARAM_ERROR	-10

#define BDF_OPEN_OUT	-11
#define GPF_OPEN_OUT	-12
#define BDF_OPEN_HEAD	-13
#define BDF_READ_HEAD	-14

/* system_ call error */
#define	MALLOC_ERROR	-15
#define	FORK_ERROR	-16
#define	EXEC_ERROR	-17
#define	PIPE_ERROR	-18
#define	DUP_ERROR	-19
#define	POPEN_ERROR	-20
#define	PCLOSE_ERROR	-21
#define	FDOPEN_ERROR	-22
#define	STAT_ERROR	-23
#define	MKTMP_ERROR	-24

#define	PCFFONTC_ERROR	-25
#define	SNFFONTC_ERROR	-26

#define SCAN_TO_NONSP(p) {\
    while (*(p) != 0) {\
	if ((*(p) == ' ') || (*(p) == TAB_CD) || (*(p) == 0xa1)) {\
	    (p)++;\
	} else {\
	    break;\
	}\
    }\
}

#define SCAN_TO_SP(p) {\
    while (*(p) != 0) {\
	if ((*(p) == ' ') || (*(p) == TAB_CD) || (*(p) == 0xa1)) {\
	    break;\
	} else {\
	    (p)++;\
	}\
    }\
}


#if defined( SVR4 ) || defined( SYSV )
#define WaitID( chld_pid, chld_stat ) 	waitpid( (chld_pid), &(chld_stat), WUNTRACED )
#else
#define WaitID( chld_pid, chld_stat ) {\
    for(;;){\
	int rtn ;\
	rtn = wait3( &(chld_stat), WUNTRACED, NULL );\
	if( rtn == (chld_pid) ) break ;\
	if( rtn == -1 ){\
	    if( errno == EINTR ) continue ;\
	    else                    break ;\
	}\
    }\
}
#endif

#ifndef	DEFAULT_CODESET
#define	DEFAULT_CODESET		"3"
#endif

/*
 * tempory file
 */
#define TempFileKey		"XXXXXX"
#define TEMPFILEKEY		TempFileKey
#ifndef	DEFAULT_TMPPATH
#define	DEFAULT_TMPPATH		"/var/tmp/"
#endif	/* DEFAULT_TMPPATH */

/*
 * Function prototypes
 */
extern	int	falcom_atoi(
#if NeedFunctionPrototypes
    char*	/* str	*/,
    char	/* ed_cod */,
    int*	/* val */
#endif
);

extern	int	GetDefaultFile(
#if NeedFunctionPrototypes
    int		/* size */,
    char*	/* style */,
    char*	/* fname */
#endif
);

extern	int	get_default_file(
#if NeedFunctionPrototypes
    FILE*	/* fp */,
    int		/* size */,
    char*	/* style */,
    char*	/* fname */
#endif
);

extern	char	*falcom_get_base_name(
#if NeedFunctionPrototypes
    char*	/* str */	
#endif
);

extern	int	GetFileName(
#if NeedFunctionPrototypes
    char*	/* com */,
    char*	/* size_str */,
    char*	/* style */,
    int 	/* codeset */,
    char*	/* ofile */
#endif
);

extern	void	ErrMsgTable_FalGetFontList(
#if NeedFunctionPrototypes
    char*	/* com */,
    int		/* utyerr */,
    int		/* utyderr */
#endif
);

extern	void	fal_cut_tailslash(
#if NeedFunctionPrototypes
    char*	/* name */
#endif
);

extern	char	*GetRealFileName( 
#if NeedFunctionPrototypes
    char*	/* name */
#endif
);

extern	int	IsInRegion(
#if NeedFunctionPrototypes
    int			/* code  */,
    int			/* num_gr */,
    FalGlyphRegion*	/* gr  */
#endif
);

extern	int	GetUdcFileName( 
#if NeedFunctionPrototypes
    char*	/* com  */,
    int		/* code_no  */,
    char*	/* xlfdname  */,
    char*	/* fname */
#endif
);

extern	int	GetUdcRegion( 
#if NeedFunctionPrototypes
    char*		/* com */,
    int			/* codeset */,
    char*		/* gpf_file */,
    int*		/* num_gr */,
    FalGlyphRegion**	/* gr */
#endif
);

extern	int	DispUdcCpArea( 
#if NeedFunctionPrototypes
    FILE*	/* fp */
#endif
);

extern	int	GetUdcFontName( 
#if NeedFunctionPrototypes
    char*	/* gpf_file */,
    char*	/* bdf_file */,
    char**	/* fontname */
#endif
);

extern	int 	FileLock( 
#if NeedFunctionPrototypes
    int 	/* fd */
#endif
);

extern	int 	FileUnLock( 
#if NeedFunctionPrototypes
    int 	/* fd */
#endif
);

extern	int 	isLock( 
#if NeedFunctionPrototypes
    int 	/* fd */
#endif
);

extern	int	ReadBdfHeader( 
#if NeedFunctionPrototypes
    struct btophead*	/* head */,
    char*		/* buf */
#endif
);

extern	int	ReadGpfHeader(
#if NeedFunctionPrototypes
    struct ptobhead*	/* head */,
    char*		/* buf */
#endif
);

extern	int	WriteBdfHeader(
#if NeedFunctionPrototypes
    struct ptobhead*	/* head */
#endif
);

extern	void	WriteGpfHeader(
#if NeedFunctionPrototypes
    struct btophead*	/* head */
#endif
);

extern	int	WritePtnToBdf(
#if NeedFunctionPrototypes
    struct ptobhead*	/* head */
#endif
);

extern	int	putDefaultChars(
#if NeedFunctionPrototypes
    struct ptobhead*	/* head */
#endif
);

extern	int	WritePtnToGpf(
#if NeedFunctionPrototypes
    struct btophead*	/* head */
#endif
);

extern	int	ReadBdfToMemory(
#if NeedFunctionPrototypes
    struct btophead*	/* head */,
    char*		/* buf */
#endif
);

extern	int	ReadBdfToMemory_with_init(
#if NeedFunctionPrototypes
    struct btophead*	/* head */,
    int			/* init_start */,
    int			/* init_end */,
    char*		/* buf */
#endif
);

extern	int	GetBdfCode(
#if NeedFunctionPrototypes
    struct btophead*	/* head */,
    char*		/* buf */,
    int*		/* code */
#endif
);

extern	int	GetBdfPtn(
#if NeedFunctionPrototypes
    struct btophead*	/* head */,
    char*		/* buf */,
    char*		/* ptn */,
    int			/* mwidth */,
    int			/* bsize */
#endif
);

extern	int	ReadGpfToMemory(
#if NeedFunctionPrototypes
    struct ptobhead*	/* head */,
    char*		/* buf */
#endif
);

extern	int	GetGpfCode(
#if NeedFunctionPrototypes
    struct ptobhead*	/* head */,
    char*		/* buf */,
    int*		/* code */
#endif
);

extern	int	GetGpfPtn(
#if NeedFunctionPrototypes
    struct ptobhead*	/* head */,
    char*		/* buf */,
    char*		/* ptn */,
    int			/* mwidth */,
    int			/* bsize */
#endif
);

extern	void	GetBdfLinePtn(
#if NeedFunctionPrototypes
    char*	/* mem */,
    char*	/* buf */,
    int	/* width */
#endif
);

extern	void	GetGpfLinePtn(
#if NeedFunctionPrototypes
    char*	/* mem */,
    char*	/* buf */,
    int	/* width */
#endif
);

extern	int	PutBdfPtn(
#if NeedFunctionPrototypes
    unsigned char*	/* ptn */,
    int			/* width */,
    int			/* height */,
    FILE*		/* fp */
#endif
);

extern	int	PutGpfPtn(
#if NeedFunctionPrototypes
    char*	/* ptn */,
    int		/* width */,
    int		/* height */,
    FILE*	/* fp */
#endif
);

extern	int	PtnZoom(
#if NeedFunctionPrototypes
    char*	/* dmem */,
    char*	/* smem */,
    int		/* sw */,
    int		/* sh */,
    int		/* dw */,
    int		/* dh */
#endif
);

extern	char	*GetTmpPath( 
#if NeedFunctionPrototypes
    char*	/* path */
#endif
);

extern	int	ChkNumString( 
#if NeedFunctionPrototypes
    char*	/* str */
#endif
);

extern	void	ErrMsgTable_AndExit(
#if NeedFunctionPrototypes
    int		/* er_no */,
    char*	/* snf_in */,
    char*	/* snf_out */,
    char*	/* gpf_in */,
    char*	/* gpf_out */,
    char*	/* bdf_in */,
    char*	/* prog_name  */
#endif
);

extern	void	fatal_err_msg( 
#if NeedFunctionPrototypes
    int		/* er_no */,
    char*	/* prog_name */
#endif
);

extern	void	ChatchSig() ;

extern	void	Put_File_Create_Err_Msg( 
#if NeedFunctionPrototypes
    int		/* msg_level */,
    char*	/* org_name */,
    char*	/* tmp_name */,
    char*	/* save_name */,
    char*	/* com */
#endif
);

extern	int	Make_NewFefFile( 
#if NeedFunctionPrototypes
    char*	/* org_name */,
    char*	/* tmp_name */,
    mode_t	/* mode */,
    uid_t	/* owner */,
    gid_t	/* group */,
    char*	/* com */
#endif
);

extern	int	Unlink_Tmpfile( 
#if NeedFunctionPrototypes
    char*	/* file */,
    char*	/* com */
#endif
);

extern	int	Chmod_File ( 
#if NeedFunctionPrototypes
    char*	/* fname */,
    mode_t	/* mode */,
    char*	/* com */
#endif
);

extern	int	Chown_File ( 
#if NeedFunctionPrototypes
    char*	/* fname */,
    uid_t	/* owner */,
    gid_t	/* group */,
    char*	/* com */
#endif
);

extern	int 	ChkPcfFontFile( 
#if NeedFunctionPrototypes
    char*	/* filename */
#endif
);

extern	int 	ChkSnfFontFile( 
#if NeedFunctionPrototypes
    char*	/* filename */
#endif
);

extern	char	*get_cmd_path( 
#if NeedFunctionPrototypes
    char*	/* path */,
    char*	/* cmd */
#endif
);

extern	int	SetCmdPath( 
#if NeedFunctionPrototypes
    char*	/* com  */,
    char**	/* path  */,
    char*	/* dflt_path  */,
    char*	/* cmd */
#endif
);

extern	int
Link_NewFile(
#if NeedFunctionPrototypes
    char *	/* rd_file */,
    char *	/* wr_file */
#endif
) ;

