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
/* $XConsortium: exksh.h /main/3 1995/11/01 15:54:01 rswiston $ */
/* "%W%" */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF    */
/*	UNIX System Laboratories, Inc.			*/
/*	The copyright notice above does not evidence any       */
/*	actual or intended publication of such source code.    */

#ifndef _Dtksh_exksh_h
#define _Dtksh_exksh_h

#include <sys/types.h>

#ifndef SYMS_ONLY

#define SH_FAIL 1
#define SH_SUCC 0

#define PRSYMBOLIC			1
#define PRMIXED				2
#define PRDECIMAL			4
#define PRHEX				8
#define PRMIXED_SYMBOLIC	16
#define PRNAMES				32

#define UPP(CH) (islower(CH) ? toupper(CH) : (CH))
#define C_PAIR(STR, CH1, CH2) (((STR)[0] == (CH1)) && ((STR)[1] == (CH2)))
#define XK_USAGE(X) return(xk_usage(X), SH_FAIL);

/* In the future, this will require following pointers, unless we
** can always trace back types to typedefs.  For example, unsigned long is
** a typedef, but it is simple because it is really just a long.
*/
#define IS_SIMPLE(TBL) ((TBL)->flags & F_SIMPLE)

#ifndef N_DEFAULT /* From name.h */
/* Stolen out of include/name.h, the problems of including things
** out of the ksh code is major.  Hence, the copy rather than the
** include.
*/

struct Bfunction {
	long	(*f_vp)();		/* value function */
	long	(*f_ap)();		/* assignment function */
};

#endif /* N_DEFAULT: From name.h */

#define ALLDATA		INT_MAX

#define BIGBUFSIZ (10 * BUFSIZ)

#define IN_BAND		1
#define OUT_BAND	2
#define NEW_PRIM	4

struct fd {
	int vfd;
	int flags;
	char mode;
	struct strbuf *lastrcv;
	int rcvcount;
	int sndcount;
	int uflags;
};

struct vfd {
	int fd;
};

extern struct fd *Fds;
extern struct vfd *Vfds;

struct libdesc {
	char *name;
	void *handle;
};
struct libstruct {
	char *prefix;
	int nlibs;
	struct libdesc *libs;
};


#ifndef OSI_LIB_CODE
#define PARPEEK(b, s) (((b)[0][0] == s[0]) ? 1 :  0 )
#define PAREXPECT(b, s) (((b)[0][0] == s[0]) ? 0 : -1 )
#define OFFSET(T, M) ((int)(&((T)NULL)->M))

typedef char *string_t;

/*
 * Structures for driving generic print/parse/copy/free routines
 */

typedef struct memtbl {
	char *name;	/* name of the member */
	char *tname;	/* name of the typedef */
	char  kind;	/* kind of member, see #defines below */
	char  flags;	/* flags for member, see #defines below */
	short  tbl;	/* -1 or index into ASL_allmems[] array */
	short  ptr;	/* number of "*" in front of member */
	short  subscr;	/* 0 if no subscript, else max number of elems */
	short  delim;	/* 0 if no length delim, +1 if next field, -1 if prev */
	short  id;	/* Id of the ASL in which this def is made */
	short  offset;	/* offset into the C structure */
	short  size;	/* size of this member, for easy malloc'ing */
	long  choice;	/* def of tag indicating field chosen for unions */
} memtbl_t;

struct envsymbols {
	char *name;
	int  id;
	int  (*parsefunc)();
	int  (*printfunc)();
	char *tname;
	int  intlike;
	int  string;
	int  topptr;
	int  valbits;
	struct {
		char *name;
		unsigned long val;
		int  cover;
	} vals[64];
}; 


/*
 * Definitions for the kind field of the above structure 
 */

#define K_CHAR		(0)	/* char or unchar */
#define K_SHORT		(1)	/* short or ushort */
#define K_INT		(2)	/* int or uint */
#define K_LONG		(3)	/* long, unsigned long, PRIM, etc. */
#define K_STRING	(4)	/* char * or char [] */
#define K_OBJID		(5)	/* objid_t *, note the star is included */
#define K_ANY		(6)	/* any_t */
#define K_STRUCT	(7)	/* struct { } */
#define K_UNION		(8)	/* union { } */
#define K_TYPEDEF	(9)	/* typedef */
#define K_DSHORT	(10)	/* short delimiter */
#define K_DINT	(11)	/* int delimiter */
#define K_DLONG	(12)	/* long delimiter */

/*
 * Definitions for the flags field of the above structure, bitmask
 */

#define F_SIMPLE		(1)	/* simple, flat type */
#define F_FIELD			(2) /* memtbl is a field of a structure, not the
								name of a type */
#define F_TBL_IS_PTR	(4) /* tbl field is pointer, not number; */
#define F_TYPE_IS_PTR	(8) /* type is built-in, but is already a pointer, like K_STRING */

#define SUCCESS	0
#define FAIL	(-1)

#define TRUE	1
#define FALSE	0

/* The following macro, RIF, stands for Return If Fail.  Practically
 * every line of encode/decode functions need to do this, so it aids
 * in readability.
 */
#define RIF(X) do { if ((X) == FAIL) return(FAIL); } while(0)

#endif /* not OSI_LIB_CODE */

#if !defined(OSI_LIB_CODE) || defined(NEED_SYMLIST)
struct symlist {
	struct memtbl tbl;
	int isflag;
	int nsyms;
	struct symarray *syms;
};
#endif

#define DYNMEM_ID		(1)
#define BASE_ID			(2)

#define ALTPUTS(STR) puts(STR)

#ifndef NULL
#define NULL	(0)
#endif

#ifdef SPRINTF_RET_LEN
#define lsprintf sprintf
#endif

#define MAX_CALL_ARGS 15

#define TREAT_SIMPLE(TBL) ((TBL)->ptr || IS_SIMPLE(TBL))

#ifdef EXKSH_INCLUDED
#define XK_PRINT(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) (_Delim = 0, xk_print(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7))
#define XK_PARSE(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) (_Delim = 0, xk_parse(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7))
#define XK_FREE(ARG1, ARG2, ARG3, ARG4, ARG5) (_Delim = 0, xk_free(ARG1, ARG2, ARG3, ARG4, ARG5))
#endif

#define NOHASH		1
#define TYPEONLY	2
#define STRUCTONLY	4

#endif /* not SYMS_ONLY */

struct symarray {
	const char *str;
	unsigned long addr;
};

#endif /* _Dtksh_exksh_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
