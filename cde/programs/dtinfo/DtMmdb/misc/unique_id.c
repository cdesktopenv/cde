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
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: unique_id.c /main/3 1996/06/11 17:40:45 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#ifdef SVR4
#include <sys/systeminfo.h>
#endif

/* **************************************************************
 * unique_id - generate a 15 character NULL terminated id
 * ************************************************************** */

/* This code assumes:

   sizeof (time_t) == 4,
   sizeof (int) == 4,
   sizeof (short) == 2,
   sizeof ("hostid") == 4,
   sizeof ("pid") == 2,
*/

/* #define DEBUG */

static char mapping[] =
  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '.', '_' };

static unsigned int mask[] =
  { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

#define COPY_BITS(DEST,DPOS,SRC,SPOS,LEN) \
  DEST |= ((SRC & (mask[LEN] << SPOS)) >> SPOS) << DPOS;

#define PRINT_BITS(BITS) \
  { int i; unsigned long bits = BITS; printf (#BITS " = 0x%04x", BITS); \
    for (i = 0; i < sizeof(BITS) * 8; i++, bits <<= 1) { \
      if (!(i%4)) putchar (' '); \
      (bits & (1L << (sizeof(BITS) * 8)-1)) ? putchar('1') : putchar('0'); } \
    putchar ('\n'); } 

#if defined(hpux)
#include <sys/utsname.h>
static unsigned int
gethostid()
{
  struct utsname u;
  int i;

  i=uname(&u);
  if (i==-1)
    abort();
  if (u.idnumber[0])
    return atoi(u.idnumber);
  abort();
}
#elif defined(SVR4)
static unsigned int
gethostid()
{
  char buffer[256];
  sysinfo (SI_HW_SERIAL, buffer, sizeof (buffer));
  return (atoi (buffer));
}
#endif

const char *
unique_id (void)
{
  static char buf[16];
  static unsigned int hostid;
  static struct timeval cur_time, old_time;
  static unsigned short pid;
  static int i;

  /* -------- First get the information -------- */

  /* Loop until first char is alpha-numeric. */
  do
    {

      /* Loop over time until unique. */
      do
	{
	  /* Failure of this call is catastrophic: */
	  if (gettimeofday (&cur_time, NULL) == -1)
	    {
	      perror ("unique_id:gettimeofday");
	      abort();
	    }
	  /* Truncate microseconds to milliseconds. */
	  cur_time.tv_usec /= 1000;
	}
      while (cur_time.tv_usec == old_time.tv_usec &&
	     cur_time.tv_sec == old_time.tv_sec);

      old_time.tv_usec = cur_time.tv_usec;
      old_time.tv_sec = cur_time.tv_sec;

      if (pid == 0)
	pid = getpid();
      if (hostid == 0)
	hostid = gethostid();

#ifdef DEBUG
      PRINT_BITS ((unsigned int)cur_time.tv_usec);
      PRINT_BITS ((unsigned int)cur_time.tv_sec);
      PRINT_BITS (pid);
      PRINT_BITS (hostid);
#endif

      for (i = 0; i < 15; i++)
	buf[i] = 0;

      COPY_BITS (buf[0], 0, cur_time.tv_usec, 0, 6);
      COPY_BITS (buf[1], 0, cur_time.tv_usec, 6, 4);
      COPY_BITS (buf[1], 4, cur_time.tv_sec, 0, 2);
      COPY_BITS (buf[2], 0, cur_time.tv_sec, 2, 6);
      COPY_BITS (buf[3], 0, cur_time.tv_sec, 8, 6);
      COPY_BITS (buf[4], 0, cur_time.tv_sec, 14, 6);
      COPY_BITS (buf[5], 0, cur_time.tv_sec, 20, 6);
      COPY_BITS (buf[6], 0, cur_time.tv_sec, 26, 6);
      COPY_BITS (buf[7], 0, pid, 0, 6);
      COPY_BITS (buf[8], 0, pid, 6, 6);
      COPY_BITS (buf[9], 0, pid, 12, 4);
      COPY_BITS (buf[9], 4, hostid, 0, 2);
      COPY_BITS (buf[10], 0, hostid, 2, 6);
      COPY_BITS (buf[11], 0, hostid, 8, 6);
      COPY_BITS (buf[12], 0, hostid, 14, 6);
      COPY_BITS (buf[13], 0, hostid, 20, 6);
      COPY_BITS (buf[14], 0, hostid, 26, 6);

      for (i = 0; i < 15; i++)
	{
#ifdef DEBUG
	  unsigned char ch = buf[i];
	  printf ("%2d  0x%02x  ", i, ch);
	  PRINT_BITS (ch);
#endif
	  buf[i] = mapping[(int)buf[i]];
	}

    } while (!isalnum (buf[0]));

  return (buf);
}


#ifdef TEST

int
main (int argc, char **argv)
{
  int count = 0;
  int i;

  if (argc == 1)
    count = 1;
  else if (argc == 2)
    count = atoi (argv[1]);

  if (count == 0)
    {
      printf (stderr, "usage: uid [count]");
      exit (1);
    }

  printf ("Generating %d unique ids\n", count);
  for (i = 0; i < count; i++)
    puts (unique_id());
}

#endif /* TEST */
