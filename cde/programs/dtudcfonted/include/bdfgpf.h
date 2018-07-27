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
/* $XConsortium: bdfgpf.h /main/7 1996/06/25 20:08:09 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "udccom.h"
#include "FaLib.h"

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

    int     bdf_x;
    int     bdf_y;
    float   bdf_point;
    int     bdf_xdpi;

    int     reserve ;
    int     *code;
    char    **ptn;
    int     zoomf;

    int     code_category;
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
    char*	/* str	*/,
    char	/* ed_cod */,
    int*	/* val */
);

extern	int	GetDefaultFile(
    int		/* size */,
    char*	/* style */,
    char*	/* fname */
);

extern	int	get_default_file(
    FILE*	/* fp */,
    int		/* size */,
    char*	/* style */,
    char*	/* fname */
);

extern	char	*falcom_get_base_name(
    char*	/* str */
);

extern	int	GetFileName(
    char*	/* com */,
    char*	/* size_str */,
    char*	/* style */,
    int 	/* codeset */,
    char*	/* ofile */
);

extern	void	ErrMsgTable_FalGetFontList(
    char*	/* com */,
    int		/* utyerr */,
    int		/* utyderr */
);

extern	void	fal_cut_tailslash(
    char*	/* name */
);

extern	char	*GetRealFileName(
    char*	/* name */
);

extern	int	IsInRegion(
    int			/* code  */,
    int			/* num_gr */,
    FalGlyphRegion*	/* gr  */
);

extern	int	GetUdcFileName(
    char*	/* com  */,
    int		/* code_no  */,
    char*	/* xlfdname  */,
    char*	/* fname */
);

extern	int	GetUdcRegion(
    char*		/* com */,
    int			/* codeset */,
    char*		/* gpf_file */,
    int*		/* num_gr */,
    FalGlyphRegion**	/* gr */
);

extern	int	DispUdcCpArea(
    FILE*	/* fp */
);

extern	int	GetUdcFontName(
    char*	/* gpf_file */,
    char*	/* bdf_file */,
    char**	/* fontname */
);

extern	int 	FileLock(
    int 	/* fd */
);

extern	int 	FileUnLock(
    int 	/* fd */
);

extern	int 	isLock(
    int 	/* fd */
);

extern	int	ReadBdfHeader(
    struct btophead*	/* head */,
    char*		/* buf */
);

extern	int	ReadGpfHeader(
    struct ptobhead*	/* head */,
    char*		/* buf */
);

extern	int	WriteBdfHeader(
    struct ptobhead*	/* head */
);

extern	void	WriteGpfHeader(
    struct btophead*	/* head */
);

extern	int	WritePtnToBdf(
    struct ptobhead*	/* head */
);

extern	int	putDefaultChars(
    struct ptobhead*	/* head */
);

extern	int	WritePtnToGpf(
    struct btophead*	/* head */
);

extern	int	ReadBdfToMemory(
    struct btophead*	/* head */,
    char*		/* buf */
);

extern	int	ReadBdfToMemory_with_init(
    struct btophead*	/* head */,
    int			/* init_start */,
    int			/* init_end */,
    char*		/* buf */
);

extern	int	GetBdfCode(
    struct btophead*	/* head */,
    char*		/* buf */,
    int*		/* code */
);

extern	int	GetBdfPtn(
    struct btophead*	/* head */,
    char*		/* buf */,
    char*		/* ptn */,
    int			/* mwidth */,
    int			/* bsize */
);

extern	int	ReadGpfToMemory(
    struct ptobhead*	/* head */,
    char*		/* buf */
);

extern	int	GetGpfCode(
    struct ptobhead*	/* head */,
    char*		/* buf */,
    int*		/* code */
);

extern	int	GetGpfPtn(
    struct ptobhead*	/* head */,
    char*		/* buf */,
    char*		/* ptn */,
    int			/* mwidth */,
    int			/* bsize */
);

extern	void	GetBdfLinePtn(
    char*	/* mem */,
    char*	/* buf */,
    int	/* width */
);

extern	void	GetGpfLinePtn(
    char*	/* mem */,
    char*	/* buf */,
    int	/* width */
);

extern	int	PutBdfPtn(
    unsigned char*	/* ptn */,
    int			/* width */,
    int			/* height */,
    FILE*		/* fp */
);

extern	int	PutGpfPtn(
    char*	/* ptn */,
    int		/* width */,
    int		/* height */,
    FILE*	/* fp */
);

extern	int	PtnZoom(
    char*	/* dmem */,
    char*	/* smem */,
    int		/* sw */,
    int		/* sh */,
    int		/* dw */,
    int		/* dh */
);

extern	char	*GetTmpPath(
    char*	/* path */
);

extern	int	ChkNumString(
    char*	/* str */
);

extern	void	ErrMsgTable_AndExit(
    int		/* er_no */,
    char*	/* snf_in */,
    char*	/* snf_out */,
    char*	/* gpf_in */,
    char*	/* gpf_out */,
    char*	/* bdf_in */,
    char*	/* prog_name  */
);

extern	void	fatal_err_msg(
    int		/* er_no */,
    char*	/* prog_name */
);

extern	void	ChatchSig() ;

extern	void	Put_File_Create_Err_Msg(
    int		/* msg_level */,
    char*	/* org_name */,
    char*	/* tmp_name */,
    char*	/* save_name */,
    char*	/* com */
);

extern	int	Make_NewFefFile(
    char*	/* org_name */,
    char*	/* tmp_name */,
    mode_t	/* mode */,
    uid_t	/* owner */,
    gid_t	/* group */,
    char*	/* com */
);

extern	int	Unlink_Tmpfile(
    char*	/* file */,
    char*	/* com */
);

extern	int	Chmod_File (
    char*	/* fname */,
    mode_t	/* mode */,
    char*	/* com */
);

extern	int	Chown_File (
    char*	/* fname */,
    uid_t	/* owner */,
    gid_t	/* group */,
    char*	/* com */
);

extern	int 	ChkPcfFontFile(
    char*	/* filename */
);

extern	int 	ChkSnfFontFile(
    char*	/* filename */
);

extern	char	*get_cmd_path(
    char*	/* path */,
    char*	/* cmd */
);

extern	int	SetCmdPath(
    char*	/* com  */,
    char**	/* path  */,
    char*	/* dflt_path  */,
    char*	/* cmd */
);

extern	int
Link_NewFile(
    char *	/* rd_file */,
    char *	/* wr_file */
);
