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
/******************************************************************************
 ******************************************************************************
 **
 ** File:         util.c
 ** RCS:          $XConsortium: util.c /main/3 1996/10/30 11:16:26 drk $
 **
 ** Description:  Utility code for the dtpdmd
 **
 ** (c) Copyright 1995, 1996, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

#define UTIL_DOT_C

#include "dtpdmdP.h"
#include <setjmp.h>
#include <unistd.h>

jmp_buf  xio_quickie_jmp_buf;

/******************************************************************************
 *
 * Misc utility routines.
 */

int xio_quickie_handler( Display *dpy )
{
    longjmp( xio_quickie_jmp_buf, 1 );
}

/***************************************
 *
 * str_dup using Xmalloc
 */
char *xpstrdup(char * str)
{
    int len;
    char *newstr;

    len = strlen(str) + 1;
    newstr = (char *) Xmalloc( len );
    memcpy( newstr, str, len );
    return( newstr );
}

/***************************************
 *
 * Multi-byte capable version of strspn(s1, s2).
 *    Returns the span of characters in s1 contained in s2.
 *    Only s1 can be multibyte.
 */
int
xpstrspn(
        char *s1,
        char *s2 )
{
#ifdef NLS16
   wchar_t s1char, s2char;
   int s1len, s2len;
   int i;
   int count;
   char * ptr;
   Boolean match;


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
#else
   return(strspn(s1, s2));
#endif /* NLS16 */
}

/***************************************
 *
 * Multi-byte capable version of strcspn(s1, s2).
 *    Returns the span of characters in s1 not contained in s2.
 *    Only s1 can be multibyte.
 */
int
xpstrcspn(
        char *s1,
        char *s2 )
{
#ifdef NLS16
   wchar_t s1char, s2char;
   int s1len, s2len;
   int i;
   int count;
   char * ptr;


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
#else
   return(strcspn(s1, s2));
#endif /* NLS16 */
}

/***************************************
 * 
 * Multi-byte capable version of strtok(s1, s2).
 *    Returns a pointer to the span of characters in s1 terminated by
 *    one of the characters in s2.  Only s1 can be multibyte.
 */
char *
xpstrtok(
        char *s1,
        char *s2 )
{
#ifdef NLS16
   static char * ptr = NULL;
   char * return_ptr;
   int len;
   int offset;


   /*
    * If this is the first call, save the string pointer, and bypass
    * any leading separators.
    */
   if (s1)
      ptr = s1 + xpstrspn(s1, s2);

   /* A Null string pointer has no tokens */
   if (ptr == NULL)
      return(NULL);

   /* Find out where the first terminator is */
   if ((len = xpstrcspn(ptr, s2)) <= 0)
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
   offset = mblen(ptr + len, MB_CUR_MAX);
   *(ptr + len) = '\0';
   ptr += (len + offset);

  /*
   * In preparation for the next pass, skip any other occurrences of
   * the terminator characters which were joined with the terminator
   * we first encountered.
   */
   len = xpstrspn(ptr, s2);
   ptr += len;

   return(return_ptr);
#else
   return(strtok(s1, s2));
#endif /* NLS16 */
}

void xp_add_argv( char ***argv, char *str )
{
    int i;

    if ( *argv ) {
	for ( i = 0; (*argv)[i]; i++ );
	*argv = (char **) Xrealloc( (char *) *argv, sizeof(char *) * (i + 2) );
    }
    else {
	i = 0;
	*argv = (char **) Xmalloc( sizeof(char *) * 2 );
    }

    (*argv)[i] = str;
    (*argv)[i+1] = (char *) NULL;
}

