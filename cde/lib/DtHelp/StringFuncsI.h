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
/* $XConsortium: StringFuncsI.h /main/5 1996/08/30 13:42:52 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        StringFuncsI.h
 **
 **   Project:     Cde DtHelp
 **
 **   Description: Header file for StringFuncs.c
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpStringFuncsI_h
#define _DtHelpStringFuncsI_h

/* opaque handle of the iconv context */
typedef struct _DtHelpCeIconvContextRec * _DtHelpCeIconvContext;

/****************************************************************************
 *			Semi Public Macros
 ****************************************************************************/
#define _DtHelpCeToLower(c) \
		(('A' <= (c) && (c) <= 'Z') ? (c) + 'a' - 'A' : (c))
/****************************************************************************
 *			Semi Public Re-defines
 ****************************************************************************/
#if !defined(_AIX)
#define	_DtHelpCeStrCaseCmp(s1,s2)	strcasecmp(s1,s2)
#endif /* _AIX */

/****************************************************************************
 *			Semi Public Routines
 ****************************************************************************/
/******************************************************************************
 * Function: _DtHelpCeCountChars (char *s1, max_len, ret_len)
 *
 *      Returns in 'ret_len' the number of characters (not bytes)
 *      in string s1.
 *
 * Returns:
 *      -1  If found an invalid character. 'ret_len' contains the
 *          number of 'good' characters found.
 *       0  If successful.
 *****************************************************************************/

/******************************************************************************
 * Function: _DtHelpCeStrcspn (const char *s1, const char *s2, max_len, &ret_len)
 *
 *      Returns in 'ret_len' the length of the initial segment of string
 *      s1 which consists entirely of characters not found in string s2.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found a character in string s2
 *       1  If found the null byte character.
 *****************************************************************************/

/******************************************************************************
 * Function: _DtHelpCeStrspn (const char *s1, const char *s2, max_len, &ret_len)
 *
 *      Returns in 'ret_len' the length of the initial segment of string
 *      s1 which consists entirely of characters found in string s2.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found a character not in string s2
 *       1  If found the null byte character.
 *****************************************************************************/

/******************************************************************************
 * Function: _DtHelpCeStrchr(const char *s1,const char *value, max_len, ret_ptr)
 *
 *      Returns in 'ret_ptr' the address of the first occurence of 'value'
 *      in string s1.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found value in string s2
 *       1  If found the null byte character without finding 'value'.
 *          'ret_ptr' will also be null in this case.
 *****************************************************************************/

/******************************************************************************
 * Function: _DtHelpCeStrrchr(const char *s1,const char *value,max_len,ret_ptr)
 *
 *      Returns in 'ret_ptr' the address of the last occurence of 'value'
 *      in string s1.
 *
 * Returns:
 *      -1  If found an invalid character.
 *       0  If found value in string s2
 *       1  If found the null byte character without finding 'value'.
 *          'ret_ptr' will also be null in this case.
 *****************************************************************************/

/*****************************************************************************
 * Function: _DtHelpCeUpperCase
 *
 * Parameters:  string          Specifies the string to change into
 *                              upper case.
 *
 * Return Value: Nothing
 *
 * Purpose:     To change all lower case characters into upper case.
 *
 *****************************************************************************/

/************************************************************************
 * Function: _DtHelpCEStrHashToKey
 *
 * Parameters:  C-format string to hash
 *
 * Purpose:
 *   Does a very simple hash operation on the string and returns the value
 *
 * Returns:  hash value
 *
 ************************************************************************/

/******************************************************************************
 * Function:    _DtHelpCEGetStrcollProc
 *
 * Parameters:  none
 *
 * Returns:     Ptr to the proper collation function to use
 *              If the LANG="C", then it is strcasecmp().
 *              If it's not, then it is strcoll().
 *
 * Purpose:     When the LANG="C", strcoll() performs collation
 *              identical to strcmp(), which is strictly bitwise.
 *              To get case-insensitive collation, you need to use
 *              strcasecmp() instead.  If LANG != "C", then
 *              strcoll() collates according to the language
 *              setting.
 *
 *****************************************************************************/

/*****************************************************************************
 * Function:        String _DtHelpCeStripSpaces (string)
 *
 *
 * Parameters:    String to process
 *
 * Return Value:  Processed string
 *
 * Purpose:       Strip all leading and trailing spaces.
 *                Processing is in place
 *
 *****************************************************************************/

/*****************************************************************************
 * Function:        void _DtHelpCeCompressSpace (string)
 *
 * Parameters:      string to process
 *
 * Return Value:    processed string
 *
 * Purpose:           This function strips all leading and trailing spaces
 *                    from the string; it also compresses any intervening
 *                    spaces into a single space.  This is useful when
 *                    comparing name strings.  For instance, the string:
 *                    "    First    Middle    Last   "
 *
 *                    would compress to:
 *
 *                    "First Middle Last"
 *
 *                     Processing is in place.
 *
 *****************************************************************************/

/*****************************************************************************
 * Function: _DtHelpCeLowerCase - not_used, commented out
 *
 * Parameters:  string          Specifies the string to change into
 *                              lower case.
 *
 * Return Value: Nothing
 *
 * Purpose:     To change all upper case characters into lower case.
 *
 *****************************************************************************/

typedef int (*_CEStrcollProc)(const char *,const char *);

extern  void  **_DtHelpCeAddPtrToArray (
                        void		**array,
                        void		 *ptr );
extern	int	_DtHelpCeCountChars (
			char		*s1,
			int		 max_len,
			int		*ret_len );
extern  int	_DtHelpCeFreeStringArray ( char **array);
#if	defined(_AIX)
extern	int	_DtHelpCeStrCaseCmp(
			const char	*s1,
			const char	*s2);
#endif /* _AIX */
extern	int	_DtHelpCeStrCaseCmpLatin1(
			const char	*s1,
			const char	*s2);
extern	int	_DtHelpCeStrNCaseCmpLatin1(
			const char	*s1,
			const char	*s2,
			size_t		 n);
extern	int	_DtHelpCeStrchr (
			const char	*s1,
			const char	*value,
			int		 max_len,
			char		**ret_ptr);
extern	int	_DtHelpCeStrrchr (
			const char	*s1,
			const char	*value,
			int		 max_len,
			char		**ret_ptr);
extern	int	_DtHelpCeStrcspn (
			const char	*s1,
			const char	*s2,
			int		 max_len,
			int		*ret_len );
extern	int	_DtHelpCeStrspn (
			char		*s1,
			char		*s2,
			int		 max_len,
			int		*ret_len );
extern	void	_DtHelpCeUpperCase (char *string);
extern  int     _DtHelpCeStrHashToKey(
                        const char *    str);
extern  _CEStrcollProc _DtHelpCeGetStrcollProc(void);
extern  char *  _DtHelpCeStripSpaces (
                        char * string);
extern  void    _DtHelpCeCompressSpace (
                        char * string);
extern  int _DtHelpCeIconvStr1Step(
			const char * fromCode,
			const char * fromStr,
			const char * toCode,
			char * *     ret_toStr,
			int          dflt1,
			int          dflt2);
extern int _DtHelpCeIconvOpen(
                        _DtHelpCeIconvContext * ret_iconvContext,  /* iconv */
                        const char * fromCode,   /* codeset name */
                        const char * toCode,     /* codeset name */
                        int          dflt1,      /* 1-byte default char */
                        int          dflt2);      /* 2-byte default char */
extern  int _DtHelpCeIconvStr(
                        _DtHelpCeIconvContext iconvContext, /* iconv */
                        const char * fromStr,      /* string to convert */
                        char * *     ret_toStr,    /* converted str */
                        size_t *     ret_toStrLen, /* converted str */
                        char *       toStrBuf,     /* can pass in a buf */
                        size_t       toStrBufLen); /* length of buf */

extern void _DtHelpCeIconvClose(
                        _DtHelpCeIconvContext * io_iconvContext);
extern int _DtHelpCeIconvContextSuitable(
                        _DtHelpCeIconvContext iconvContext,
                        const char *          fromCode,
                        const char *          toCode);



#endif /* _DtHelpStringFuncsI_h */
