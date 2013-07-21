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
/* $TOG: DtNlUtils.c /main/10 1999/10/15 12:07:23 mgreess $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*****************************************************************************/
/*                                                                           */
/* This file contains the Dt versions of common string functions, which     */
/* have not yet been provided by the HP-UX platform.                         */
/* These functions know how to handle multi-byte strings.                    */
/*                                                                           */
/*****************************************************************************/

#include <ctype.h>
#include <string.h>
#include <Dt/DtNlUtils.h>
#include "DtSvcLock.h"

/*
 * Globals
 */
Boolean _DtNl_is_multibyte = False;


#ifdef NLS16
/*
 * Dt nls initialization function.
 *    will see if multibyte characters are
 *    supported for the locale.  If multibyte characters are not supported,
 *    then all of our string utilites simply call the standard libc function.
 */

void 
Dt_nlInit( void )
{
   char * bc;
   static Boolean first = True;

   _DtSvcProcessLock();
   if (!first) {
      _DtSvcProcessUnlock();
      return;
   }

   first = False;
   _DtSvcProcessUnlock();

   if (MB_CUR_MAX > 1)
      _DtNl_is_multibyte = True;
   else
      _DtNl_is_multibyte = False;
}

/*
 * Dt version of strtok(s1, s2).
 *    Returns a pointer to the span of characters in s1 terminated by
 *    one of the characters in s2.  Only s1 can be multibyte.
 */

char * 
Dt_strtok(
        char *s1,
        char *s2 )
{
   static char *ptr;
   char * return_ptr;
   int len;
   int offset;

   /* Use standard libc function, if no multibyte */
   if (!_DtNl_is_multibyte)
      return(strtok(s1, s2));

   /* 
    * If this is the first call, save the string pointer, and bypass
    * any leading separators.
    */
   if (s1)
      ptr = s1 + Dt_strspn(s1, s2);

   /* A Null string pointer has no tokens */
   if (ptr == NULL)
      return(NULL);

   /* Find out where the first terminator is */
   if ((len = Dt_strcspn(ptr, s2)) <= 0)
   {
      /* No tokens left */
      return(NULL);
   }

   /* Keep track of where the token started */
   return_ptr = ptr;

   /* Null out the terminator; we need to know how many bytes are
    * occupied by the terminator, so that we can skip over it to
    * the next character.
    */
   /*
    * We have to take care of the case when mblen() returns -1.
    */
   offset = mblen(ptr + len, MB_CUR_MAX);
   if( offset == -1 )
	offset = 1;
   *(ptr + len) = '\0';
   ptr += (len + offset);

  /* 
   * In preparation for the next pass, skip any other occurrances of
   * the terminator characters which were joined with the terminator
   * we first encountered.
   */
   len = Dt_strspn(ptr, s2);
   ptr += len;

   return(return_ptr);
}


char * 
Dt_strtok_r(
        char *s1,
        char *s2,
	char **ptr )
{
   char * return_ptr;
   int len;
   int offset;

   /* Use standard libc function, if no multibyte */
   if (!_DtNl_is_multibyte)
      return((char*) strtok_r(s1, s2, ptr));

   /* 
    * If this is the first call, save the string pointer, and bypass
    * any leading separators.
    */
   if (s1)
      *ptr = s1 + Dt_strspn(s1, s2);

   /* A Null string pointer has no tokens */
   if (*ptr == NULL)
      return(NULL);

   /* Find out where the first terminator is */
   if ((len = Dt_strcspn(*ptr, s2)) <= 0)
   {
      /* No tokens left */
      return(NULL);
   }

   /* Keep track of where the token started */
   return_ptr = *ptr;

   /* Null out the terminator; we need to know how many bytes are
    * occupied by the terminator, so that we can skip over it to
    * the next character.
    */
   /*
    * We have to take care of the case when mblen() returns -1.
    */
   offset = mblen(*ptr + len, MB_CUR_MAX);
   if( offset == -1 )
	offset = 1;
   *(*ptr + len) = '\0';
   *ptr += (len + offset);

  /* 
   * In preparation for the next pass, skip any other occurrances of
   * the terminator characters which were joined with the terminator
   * we first encountered.
   */
   len = Dt_strspn(*ptr, s2);
   *ptr += len;

   return(return_ptr);
}

/*
 * Dt version of strspn(s1, s2).
 *    Returns the span of characters in s1 contained in s2.  
 *    Only s1 can be multibyte.
 */

int 
Dt_strspn(
        char *s1,
        char *s2 )
{
   wchar_t s1char, s2char;
   int s1len, s2len;
   int i;
   int count;
   char * ptr;
   Boolean match;

   /* Use the standard libc function, if multibyte is not present */
   if (!_DtNl_is_multibyte)
      return(strspn(s1, s2));

   /* A Null string has no spans */
   if (s1 == NULL)
      return(0);

   count = 0;
   while (*s1)
   {
      /* Extract the next character from s1; may be multibyte */
      if ((s1len = mbtowc(&s1char, s1, MB_CUR_MAX)) < 0)
         return(0);
      s1 += s1len;

      /* 
       * Compare this character against all the chars in s2.  Keep
       * working through s1, until a character is found in s1 which
       * is not contained in s2.
       */
      ptr = s2;
      match = False;
      while (*ptr)
      {
         /* Extract the next character from s2; cannot be multibyte */
         s2char = *ptr++;

         /* If a match is found, keep processing s1 */
         if (s1char == s2char)
         {
            match = True;
            count += s1len;
            break;
         }
      }

      /* 
       * If we made it here because all of s2 was searched, and a match
       * was not found against s1, then we are done.
       */
      if (!match)
         return(count);
   }

   return(count);
}


/*
 * Dt version of strcspn(s1, s2).
 *    Returns the span of characters in s1 not contained in s2.
 *    Only s1 can be multibyte.
 */

int 
Dt_strcspn(
        char *s1,
        char *s2 )
{
   wchar_t s1char, s2char;
   int s1len, s2len;
   int i;
   int count;
   char * ptr;

   /* Use the standard libc function, if multibyte is not present */
   if (!_DtNl_is_multibyte)
      return(strcspn(s1, s2));

   /* An empty string has no spans */
   if (s1 == NULL)
      return(0);

   count = 0;
   while (*s1)
   {
      /* Extract the next character from s1; may be multibyte */
      if ((s1len = mbtowc(&s1char, s1, MB_CUR_MAX)) < 0)
         return(0);
      s1 += s1len;

      /* 
       * Compare this character against all the chars in s2.  Keep
       * working through s1, until a character is found in s1 which
       * is contained in s2.
       */
      ptr = s2;
      while (*ptr)
      {
         /* Extract the next character from s2; cannot be multibyte */
         s2char = *ptr++;
 
         /* If a match occurs, then we are done */
         if (s1char == s2char)
            return(count);
      }

      /*
       * If we've made it here, then we searched all of s2, and none of
       * its components matched s1; continue with the next character
       * in s1.
       */
      count += s1len;
   }

   return(count);
}


/*
 * Dt version of strchr(s, c).
 *    Returns a pointer to the first occurance of 'c' in 's'.
 */

char * 
Dt_strchr(
        char *s,
        char c )
{
   wchar_t schar;
   int i;
   int slen;
   wchar_t wc;
   char foo[2];

   if (s == NULL)
      return(NULL);

   /* Use standard libc function if multibyte is not enabled */
   if (!_DtNl_is_multibyte)
      return(strchr(s, c));

   foo[0] = c;
   foo[1] = '\0';
   mbtowc(&wc, foo, 2);

   do
   {
      /* Extract next char from 's'; may be multibyte */
      if ((slen = mbtowc(&schar, s, MB_CUR_MAX)) < 0)
         return(NULL);
      s += slen;

      /* If we match 'c', then return a pointer to this character */
      if (schar == wc)
         return (s - slen);
   } while (slen > 0);

   /* No match was found */
   return(NULL);
}


/*
 * Dt version of strrchr(s, c).
 *    Returns a pointer to the last occurance of 'c' in 's'.
 */

char * 
Dt_strrchr(
        char *s,
        char c )
{
   wchar_t schar;
   int i;
   int slen;
   char * last = NULL;
   wchar_t wc;
   char foo[2];

   if (s == NULL)
      return(NULL);

   /* Use standard libc function if multibyte is not enabled */
   if (!_DtNl_is_multibyte)
      return(strrchr(s, c));

   foo[0] = c;
   foo[1] = '\0';
   mbtowc(&wc, foo, 2);

   do
   {
      /* Extract next char from 's'; may be multibyte */
      if ((slen = mbtowc(&schar, s, MB_CUR_MAX)) < 0)
         return(NULL);
      s += slen;

      /* If we match 'c', keep track of it, and keep looking */
      if (schar == wc)
         last = s - slen;
   } while (slen > 0);

   return(last);
}


/*
 * Dt equivalent of s[strlen(s) - 1]
 *    Returns the last character in the string 's'.
 */

void 
Dt_lastChar(
        char *s,
        char **cptr,
        int *lenptr )
{
   int len = 0;

   if ((s == NULL) || (*s == '\0'))
   {
      *lenptr = 0;
      *cptr = NULL;
      return;
   }

   /* Use the easy method, if possible */
   if (!_DtNl_is_multibyte)
   {
      *cptr = s + strlen(s) - 1;
      *lenptr = 1;
      return;
   }

   /* Move through the string, keeping a ptr to the last character found */
   while (*s)
   {
      /*
       * We have to take care of the case when mbtowc() returns -1
       */
      len = mbtowc(NULL, s, MB_CUR_MAX);
      if ( len == -1 )
         len = 1;
      s += len;
   }

   /* Backup to the character before the NULL */
   *lenptr = mblen(s-len, MB_CUR_MAX);
   *cptr = s - len;
}


/*
 * Dt equivalent of strlen()
 *    Returns the number of characters (not bytes) in a string
 */

int 
Dt_charCount(
        char *s )
{
   int count = 0;
   int len;

   if (s == NULL)
      return(0);

   if (!_DtNl_is_multibyte)
      return(strlen(s));

   /* Move through the string, counting each character present */
   while (*s)
   {
      len = mblen(s, MB_CUR_MAX);
      /* if invalid character, still count it and continue */
      if (len == -1)
	len = 1;
      s += len;
      count++;
   }

   return(count);
}

/******************************************************************************
 *
 * _Dt_NextChar(s)
 *	return a pointer to the next multi-byte character after the character
 *	pointed to by "s".  If "s" does not point to a valid multi-byte
 *	character advance one byte.
 *
 ******************************************************************************/
char *
_Dt_NextChar(char *s)
{
	int len=1;
	
	if (_DtNl_is_multibyte || (MB_CUR_MAX > 1))
	    len = mblen ( s, MB_CUR_MAX);

	/*
	 * If "s" did not point to a vaild multi-byte character, 
         * move ahead one byte.
         */  
        if ( len == -1 )
	    len = 1;

        return s + len;
}

/******************************************************************************
 *
 * _Dt_PrevChar(start,s)
 *	return a pointer to the  multi-byte character preceding the
 *	character pointed to by "s".  If "s" does not point to a valid
 *	multi-byte character retreat one byte. "start" should point to 
 *      a character preceding "s" in the multi-byte string. 
 *
 ******************************************************************************/
char *
_Dt_PrevChar(const char *start, char *s)
{
	char *p;
        int len;

	if ( !_DtNl_is_multibyte || (MB_CUR_MAX == 1) )
	    return (s - 1);

        /*
         * Check if "*s" is a valid multi-byte character.
         * if not just return the previous byte.
         */
        if ( mblen(s,MB_CUR_MAX) < 0 )
	    return (s - 1);

        /*
         * "start" must be less than "s" ; if not return
         * (s-1)
         */
        if ( start >= s )
             return (s - 1);
 
        /*
         * Check that "start" points to a valid multi-byte character.
         * otherwise return "s-1"
         */
        if ( mblen(start,MB_CUR_MAX) < 0 )
	     return (s-1);

        /*
         * Starting from "start" traverse the string until we find
         * the character preceding "s". 
         */
	/*
	 * We have to take care of the case when mblen() returns -1.
	 */
	for (p = (char *)start;
		p + (len = (mblen(p,MB_CUR_MAX) == -1 ? 1 : mblen(p,MB_CUR_MAX))) < s;
		p += len)
		/* NULL STATEMENT */;

        /*
         * We should always find a multi-byte character preceding "s" if
         * "*s" is a valid multi-byte char and not the first character of
         * the text.
         */
        /* myassert(p < s); */
         
	return p;
}

/*
 * Dt mult-byte equivalent of isspace()
 */
int
_Dt_isspace(char *s)
{
	if ( !_DtNl_is_multibyte || MB_CUR_MAX == 1 )
		return isspace((u_char)*s);

	if ( mblen(s,MB_CUR_MAX) == 1 )
		return isspace((u_char)*s);
	else
		return 0;
}

/*
 * Dt mult-byte equivalent of isdigit()
 */
int
_Dt_isdigit(char *s)
{
	if ( !_DtNl_is_multibyte || MB_CUR_MAX == 1 )
		return isdigit(*s);

	if ( mblen(s,MB_CUR_MAX) == 1 )
		return isdigit(*s);
	else
		return 0;
}




/*
 * Dt equivalent of &(s[n])
 *    Returns a pointer to the indicated character
 */

char * 
_DtGetNthChar(
        char *s,
        int n )
{
   int count;
   int len;

   if ((s == NULL) || (n < 0) || (n > Dt_charCount(s)))
      return(NULL);

   count = 0;
   while ((count < n) && (*s))
   {
      if (_DtNl_is_multibyte)
         len = mblen(s, MB_CUR_MAX);
      else
         len = 1;
      /*
       * We have to take care of the case when mblen() returns -1.
       */
      if ( len == -1 )
         len = 1;

      s += len;
      count++;
   }

   return(s);
}


/*
 * multibyte version of strpbrk().
 * Only cs can be multibyte.
 */
char *
_dt_strpbrk(
        char *cs,
        char *ct)
{
    int len;
    size_t i;

    if(MB_CUR_MAX == 1)
        return(strpbrk(cs, ct));

    while(*cs) {
        len = mblen(cs, MB_CUR_MAX);
	if(len < 1)
	    len = 1;
        if(len == 1) {
            for(i = 0; i < strlen(ct); i++) {
                if(*cs == *(ct + i))
                    return(cs);
            }
        }
        cs += len;
    }
    return(NULL);
}


/*
 * returns 1 if a character before s2 in s1 is single-byte,
 * returns 0 if it is multi-byte.
 */
int
_is_previous_single(
        char *s1,
        char *s2)
{
    int n = 1;

    if(MB_CUR_MAX == 1)
        return(1);

    while(*s1) {
        if(s1 == s2) {
            if(n > 1)
                return(0);
            else
                return(1);
        }
        n = mblen(s1, MB_CUR_MAX) > 1 ? mblen(s1, MB_CUR_MAX) : 1;
        s1 += n;
    }
    return(1);
}

#else

char * 
_DtGetNthChar(
        char *s,
        int n )
{
   if ((s == NULL) || (n < 0) || (n > strlen(s)))
      return(NULL);

   return (s + n);
}

char *
_dt_strpbrk(
        char *cs,
        char *ct)
{
    return(strpbrk(cs, ct));
}

int
_is_previous_single(
        char *s1,
        char *s2)
{
    return(1);
}
#endif /* NLS16 */
