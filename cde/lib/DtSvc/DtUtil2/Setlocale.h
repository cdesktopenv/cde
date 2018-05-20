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
/* $XConsortium: Setlocale.h /main/3 1995/10/26 15:27:34 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/* Hp DT's version of an 8.0 include file; needed for Fnmatch */

#ifndef DtSETLOCALE_INCLUDED /* allow multiple inclusions */
#define DtSETLOCALE_INCLUDED

#ifdef _NAMESPACE_CLEAN
#define _1kanji __1kanji
#define _2kanji __2kanji
#define _downshift __downshift
#define _upshift __upshift
#endif /* _NAMESPACE_CLEAN */

#include	<locale.h>
#include	<limits.h>
#include	<nl_types.h>
#include	<langinfo.h>
#include	<Dt/Collate.h>

#ifndef _NL_ORDER
#define _NL_ORDER
typedef int nl_order;
#endif
#ifndef _NL_MODE
#define _NL_MODE
typedef int nl_mode;
#endif
#ifndef _NL_OUTDGT
#define _NL_OUTDGT
typedef int nl_outdgt;
#endif
#ifndef MAXLNAMELEN
#define MAXLNAMELEN 14
#endif
#endif

struct _era_data {			/* defines an Emperor/Era time period */
	short start_year;		/* starting date of era */
	unsigned short start_month;
	unsigned short start_day;
	short end_year;			/* ending date of era */
	unsigned short end_month;
	unsigned short end_day;
	short origin_year;		/* time axis origin for era (one of start_year or end_year) */
	short offset;			/* offset from 0 for 1st year of era */
	short signflag;			/* adjusts sign of (year - origin_year) value */
	unsigned short reserved;
	unsigned char *name;		/* name of era */
	unsigned char *format;		/* instead of nl_langinfo(ERA_FMT) */
};

extern	int		__nl_langid[];	/* langid of currently loaded language	*/
extern	unsigned char	*__ctype;	/* pointer to ctype table	*/
extern	unsigned char	*_1kanji;	/* pointer to 1st of 2 kanji table */
extern	unsigned char	*_2kanji;	/* pointer to 2nd of 2 kanji table */
extern	unsigned char	*_upshift;	/* pointer to up shift table */
extern	unsigned char	*_downshift;	/* pointer to down shift table */
#ifdef EUC
extern	unsigned char	*__e_cset;	/* pointer to expanded char set table */
extern	unsigned char	*__ein_csize;	/* pointer to expanded in_csize table */
extern	unsigned char	*__eout_csize;	/* pointer to expanded out_csize table*/
#endif /* EUC */
extern struct _era_data *_nl_era[];	/* array of era info str pointer */
extern	int		_nl_radix;	/* radix character */
extern	int		_sh_low;	/* lowest char in shift table domain */
extern	int		_sh_high;	/* highest char in shift table domain */
extern	int		__nl_char_size;	/* size of characters */
#ifdef EUC
extern	int		__nl_code_scheme;/* flag for char code scheme */
extern	int		__cs_SBYTE;	/* flag for 1 byte char code scheme */
extern	int		__cs_HP15;	/* flag for HP15 char code scheme */
extern	int		__cs_EUC;	/* flag for EUC char code scheme */
extern	unsigned char	__in_csize[];	/* input char size */
extern	unsigned char	__out_csize[];	/* output char size */
extern	unsigned int	__euc_template[]; /* euc process code template */
#endif /* EUC */
extern	nl_direct	_nl_direct;	/* direction flag */
extern	int		_nl_context;	/* directionality context flag */
extern  nl_order	_nl_order;	/* order flag */
extern  nl_mode		_nl_mode;	/* mode flag; Latin or non-Latin */
extern  nl_outdgt	_nl_outdigit;	/* digit output : ascii or alt digit */

extern	int		_nl_space_alt;	/* value of alternative space */
extern	unsigned char   *_nl_dgt_alt;	/* buffer for alt digit string */
extern	unsigned char	*_nl_punct_alt;	/* buffer for alt punctuation string */
extern  unsigned char	*_nl_pascii;	/* buffer for ascii punctuation string */
extern  unsigned char	*_nl_dascii;	/* buffer for ascii digits string */
extern	int		_nl_map21;	/* non-zero if 2-to-1 mappings */
extern	int		_nl_onlyseq;	/* true if only 1-to-1 char w no pri */
extern  int		_nl_collate_on;	/* true if collation table loaded */
extern  int		_nl_mb_collate;	/* true if collation is multibyte */

extern	unsigned char	 *_seqtab;	/* dictionary sequence number table */
extern	unsigned char	 *_pritab;	/* 1to2/2to1 flag + priority table */
extern	struct col_21tab *_tab21;	/* 2-to-1 mapping table	*/
extern	struct col_12tab *_tab12;	/* 1-to-2 mapping table */

extern unsigned char	*__errptr;	/* pointer to an area _errlocale() can use as a buffer */

extern struct lconv	*_lconv;
extern unsigned char	*__category_name[];

extern unsigned char	**__nl_info;	/* pointers to locale langinfo strings */
extern unsigned char	*__C_langinfo[];/* default langinfo strings for the C locale */
#define _NL_MAX_MSG	ERA_FMT		/* last nl_langinfo item */

/***************************************************************************

    The remainder of this file includes structures for the language files.
    The files are built by buildlang(1M).

    The structure of the files is as follows :

        ----------------------------------
	|  Table Header (A)	         |
	----------------------------------
	| Category/Modifier Structures(B)|
	==================================
	|  LC_ALL Table Header  (C)      |
	-   -   -   -   -   -   -   -    -
	|  LC_ALL Data			 |
	----------------------------------
	|  LC_COLLATE Table Header (D)   |
	-   -   -   -   -   -   -   -    -
	|  LC_COLLATE Data		 |
	----------------------------------
	|  LC_CTYPE  Table Header (E)	 |
	-   -   -   -   -   -   -   -    -
	|  LC_CTYPE  Data		 |
	----------------------------------
	|  LC_MONETARY Table Header (F)
	-   -   -   -   -   -   -   -    -
	|  LC_MONETARY  Data		 |
	----------------------------------
	|  LC_NUMERIC  Table Header (G)	 |
	-   -   -   -   -   -   -   -    -
	|  LC_NUMERIC  Data		 |
	----------------------------------
	|  LC_TIME  Table Header (H)	 |
	-   -   -   -   -   -   -   -    -
	|  LC_TIME  Data		 |
	----------------------------------

*****************************************************************************/


/* Category Id's */


/* Table Header (A) */

struct table_header {
	unsigned int size;		/* size of table header and category
					   structure. (A) + (B)  */
	unsigned short nl_langid;	/* _nl_langid */
	unsigned char lang[3*MAXLNAMELEN+2+1];	/* language name */
	unsigned short cat_no;		/* number of categories defined */
	unsigned short mod_no;		/* number of modifiers defined */
	unsigned short rev_flag;	/* true if HP defined */
	unsigned char rev_str[36];	/* Revision String */
	unsigned short codeset;		/* 0 if 1 byte, 1 if 2 byte */
	unsigned int	reserved1;
	unsigned int	reserved2;
	unsigned int	reserved3;
};

/* Category/Modifier Structure (B)

   Catinfotype structure describes a category/modifier table
   There is one structure for each category and modifier defined.
   These entries follow the table header */


struct catinfotype
{
	int size;				/* size of category table */
	int address;				/* address of category table -
						   offset from the beginning of
						   the category tables () */
	short catid;				/* category id */
	unsigned char mod_name[MAXLNAMELEN+1];	/* name of modifier */
	short mod_addr;				/* address of category table
						   for modifier - offset from
						   beginning of file */
};


/* Below are the category headers for each of the defined categories
   All addresses are offset from the beginning of the category information */

/* LC_ALL Table  (C) */

struct all_header {
	unsigned short yes_addr;		/* msg_index[YESSTR] */
	unsigned short no_addr;		/* msg_index[NOSTR] */
	unsigned short direct_addr;	/* msg_index[DIRECTION] */
					/* _nl_direct */
	unsigned short context_addr;	/* _nl_context */
};

/* LC_COLLATE Tables (D) */

struct col_header {
	unsigned int seqtab_addr;	/* _seqtab */
	unsigned int pritab_addr;	/* _pritab */
	unsigned short nl_map21;	/* not an address */
	unsigned short nl_onlyseq;	/* not an address */
	unsigned int tab21_addr;
	unsigned int tab12_addr;
};


/* LC_CTYPE Tables (E) */

struct ctype_header {
	unsigned int _sh_high;		/* _sh_high */
	int _sh_low;			/* _sh_low */
	unsigned int _ctype_addr;	/* __ctype */
	unsigned int kanji1_addr;	/* _1kanji */
	unsigned int kanji2_addr;	/* _2kanji */
	unsigned int upshift_addr;	/* _upshift */
	unsigned int downshift_addr;	/* _downshift */
	unsigned short byte_char_addr;	/* msg_index[BYTES_CHAR] */
	unsigned short alt_punct_addr;	/* msg_index[ALT_PUNCT] */
					/* _nl_punct_alt[] */
					/* _nl_space_alt   */
#ifdef EUC
	unsigned int io_csize_addr;	/* __io_csize[] */
	unsigned int e_cset_addr;	/* __e_cset */
	unsigned int ein_csize_addr;	/* __ein_csize */
	unsigned int eout_csize_addr;	/* __eout_csize */
#endif /* EUC */
};



/* LC_MONETARY Tables (F) */


struct monetary_header {
	unsigned short int_frac_digits;		/* _lconv->short_frac_digits */
	unsigned short frac_digits;		/* _lconv->frac_digits */
	unsigned short p_cs_precedes;		/* _lconv->p_cs_precedes */
	unsigned short p_sep_by_space;		/* _lconv->p_sep_by_space */
	unsigned short n_cs_precedes;		/* _lconv->n_cs_precedes */
	unsigned short n_sep_by_space;		/* _lconv->n_sep_by_space */
	unsigned short p_sign_posn;		/* _lconv->p_sign_posn */
	unsigned short n_sign_posn;		/* _lconv->n_sign_posn */
	unsigned short curr_symbol_lconv;		/* _lconv->currency_symbol */
	unsigned short curr_symbol_li;		/* msg_index[CRNCYSTR] */
	unsigned short mon_decimal_point;		/* _lconv->mon_decimal_point */
	unsigned short int_curr_symbol;		/* _lconv->short_curr_symbol */
	unsigned short mon_thousands_sep;		/* _lconv->mon_thousands_sep */
	unsigned short mon_grouping;		/* _lconv->mon_grouping */
	unsigned short positive_sign;		/* _lconv->positive_sign */
	unsigned short negative_sign;		/* _lconv->negative_sign */
};



/* LC_NUMERIC Tables (G) */


struct numeric_header {
	unsigned short grouping;			/* _lconv->grouping */
	unsigned short decimal_point;		/* _lconv->decimal_point */
						/* msg_index[RADIXCHAR] */
						/* _nl_radix */
	unsigned short thousands_sep;		/* _lconv->thousands_sep */
						/* msg_index[THOUSEP] */
	unsigned short alt_digit_addr;		/* msg_index[ALT_DIGIT] */
};



/* LC_TIME Tables (H) */

struct time_header {
	unsigned short d_t_fmt;			/* msg_index[D_T_FMT] */
	unsigned short d_fmt;			/* msg_index[D_FMT] */
	unsigned short t_fmt;			/* msg_index[T_FMT] */
	unsigned short day_1;			/* msg_index[DAY_1] */
	unsigned short day_2;			/* msg_index[DAY_2] */
	unsigned short day_3;			/* msg_index[DAY_3] */
	unsigned short day_4;			/* msg_index[DAY_4] */
	unsigned short day_5;			/* msg_index[DAY_5] */
	unsigned short day_6;			/* msg_index[DAY_6] */
	unsigned short day_7;			/* msg_index[DAY_7] */
	unsigned short abday_1;			/* msg_index[ABDAY_1] */
	unsigned short abday_2;			/* msg_index[ABDAY_2] */
	unsigned short abday_3;			/* msg_index[ABDAY_3] */
	unsigned short abday_4;			/* msg_index[ABDAY_4] */
	unsigned short abday_5;			/* msg_index[ABDAY_5] */
	unsigned short abday_6;			/* msg_index[ABDAY_6] */
	unsigned short abday_7;			/* msg_index[ABDAY_7] */
	unsigned short mon_1;			/* msg_index[MON_1] */
	unsigned short mon_2;			/* msg_index[MON_2] */
	unsigned short mon_3;			/* msg_index[MON_3] */
	unsigned short mon_4;			/* msg_index[MON_4] */
	unsigned short mon_5;			/* msg_index[MON_5] */
	unsigned short mon_6;			/* msg_index[MON_6] */
	unsigned short mon_7;			/* msg_index[MON_7] */
	unsigned short mon_8;			/* msg_index[MON_8] */
	unsigned short mon_9;			/* msg_index[MON_9] */
	unsigned short mon_10;			/* msg_index[MON_10] */
	unsigned short mon_11;			/* msg_index[MON_11] */
	unsigned short mon_12;			/* msg_index[MON_12] */
	unsigned short abmon_1;			/* msg_index[ABMON_1] */
	unsigned short abmon_2;			/* msg_index[ABMON_2] */
	unsigned short abmon_3;			/* msg_index[ABMON_3] */
	unsigned short abmon_4;			/* msg_index[ABMON_4] */
	unsigned short abmon_5;			/* msg_index[ABMON_5] */
	unsigned short abmon_6;			/* msg_index[ABMON_6] */
	unsigned short abmon_7;			/* msg_index[ABMON_7] */
	unsigned short abmon_8;			/* msg_index[ABMON_8] */
	unsigned short abmon_9;			/* msg_index[ABMON_9] */
	unsigned short abmon_10;		/* msg_index[ABMON_10] */
	unsigned short abmon_11;		/* msg_index[ABMON_11] */
	unsigned short abmon_12;		/* msg_index[ABMON_12] */
	unsigned short am_str;			/* msg_index[AM_STR] */
	unsigned short pm_str;			/* msg_index[PM_STR] */
	unsigned short year_unit;		/* msg_index[YEAR_UNIT] */
	unsigned short mon_unit;		/* msg_index[MON_UNIT] */
	unsigned short day_unit;		/* msg_index[DAY_UNIT] */
	unsigned short hour_unit;		/* msg_index[HOUR_UNIT] */
	unsigned short min_unit;		/* msg_index[MIN_UNIT] */
	unsigned short sec_unit;		/* msg_index[SEC_UNIT] */
	unsigned short era_fmt;			/* msg_index[ERA_FMT] */
	unsigned short era_count;		/* number of era entries */
	unsigned short era_names;		/* address of era name and format strings */
	unsigned short era_addr;		/* address of era data structure entries */
	unsigned short reserved;		/* address of era data structure entries */
};

#endif /* DtSETLOCALE_INCLUDED */
