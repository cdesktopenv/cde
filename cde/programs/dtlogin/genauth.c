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
/* $TOG: genauth.c /main/6 1997/03/25 12:33:13 barstow $ */
/* (c) Copyright 1997, The Open Group */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*
 * @DEC_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log$
 * Revision 1.1.2.2  1995/04/21  13:05:23  Peter_Derr
 * 	dtlogin auth key fixes from deltacde
 * 	[1995/04/14  18:03:41  Peter_Derr]
 *
 * 	R6 xdm code used in dtlogin.
 * 	[1995/04/10  16:52:31  Peter_Derr]
 *
 * Revision 1.1.3.3  1995/02/20  21:03:19  Peter_Derr
 * 	merge XC fix-11
 * 	[1995/02/20  20:13:02  Peter_Derr]
 * 
 * Revision 1.1.3.2  1994/07/13  19:26:25  Peter_Derr
 * 	Include Wrap.h to get definitions for XDM-AUTHENTICATION-1
 * 	authorization mechanism.
 * 	[1994/07/13  12:15:59  Peter_Derr]
 * 
 * $EndLog$
 */
#ifndef lint
static char *rcsid = "@(#)$RCSfile: genauth.c $ $Revision: /main/6 $ (DEC) $Date: 1997/03/25 12:33:13 $";
#endif
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 */

# include   <X11/Xauth.h>
# include   <X11/Xos.h>
# include   "dm.h"

#include <errno.h>

#ifdef X_NOT_STDC_ENV
#define Time_t long
extern Time_t time ();
extern int errno;
#else
#include <time.h>
#define Time_t time_t
#endif

#ifndef DONT_USE_DES
# ifndef USE_CRYPT
#  ifdef AIXV3
#   define USE_CRYPT
#  endif
#  ifdef ultrix
#   define USE_CRYPT
#  endif
#  ifdef hpux
#   define USE_CRYPT
#  endif
#  ifdef macII
#   define USE_CRYPT
#  endif
#  ifdef __FreeBSD__
#   define USE_CRYPT
#  endif
#  ifdef __OpenBSD__
#   define USE_CRYPT
#  endif
#  ifdef sun
#   define USE_CRYPT
#   if (OSMAJORVERSION >= 4)
     /* avoid strange sun crypt hackery */
#    define crypt _crypt
#   endif
#  endif
# endif
#endif

#if !defined (DONT_USE_DES) && !defined (USE_CRYPT)
# define USE_ENCRYPT
#endif

#ifdef HASXDMAUTH
static unsigned char	key[8];
#else
static long		key[2];
#endif

static sumFile (char *name, long sum[2]);

#ifdef HASXDMAUTH

typedef unsigned char auth_cblock[8];	/* block size */

typedef struct auth_ks_struct { auth_cblock _; } auth_wrapper_schedule[16];

extern void _XdmcpWrapperToOddParity();

static
longtochars (l, c)
    long	    l;
    unsigned char    *c;
{
    c[0] = (l >> 24) & 0xff;
    c[1] = (l >> 16) & 0xff;
    c[2] = (l >> 8) & 0xff;
    c[3] = l & 0xff;
}

static void
InitXdmcpWrapper (void)
{
    long	    sum[2];
    unsigned char   tmpkey[8];
/*
 * randomFile is and xdm resource not defined in dtlogin.
 *
 *   if (!sumFile (randomFile, sum)) {
 */
    if (!sumFile ("/dev/mem", sum)) {
	sum[0] = time ((Time_t *) 0);
	sum[1] = time ((Time_t *) 0);
    }
    longtochars (sum[0], tmpkey+0);
    longtochars (sum[1], tmpkey+4);
    tmpkey[0] = 0;
    _XdmcpWrapperToOddParity (tmpkey, key);
}

#endif /* HASXDMAUTH */

#ifndef HASXDMAUTH
/* A random number generator that is more unpredictable
   than that shipped with some systems.
   This code is taken from the C standard. */

static unsigned long int next = 1;

static int
xdm_rand(void)
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

static void
xdm_srand(unsigned int seed)
{
    next = seed;
}
#endif /* no HASXDMAUTH */

#ifdef USE_ENCRYPT
static void
bitsToBytes (unsigned long bits[2], char bytes[64])
{
    int	bit, byte;
    int	i;

    i = 0;
    for (byte = 0; byte < 2; byte++)
	for (bit = 0; bit < 32; bit++)
	    bytes[i++] = ((bits[byte] & (1 << bit)) != 0);
}
#endif /* USE_ENCRYPT */

# define FILE_LIMIT	1024	/* no more than this many buffers */

/* for linux/csrg we use a simpler method to get 2 random longs from
 *  the OS's random number device.
 */

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#define READ_LIMIT (sizeof (long) * 2)

static int
sumFile (char *name, long sum[2])
{
  long    buf[2];
  int	  fd;
  int     ret_status = 0;

  if ( (fd = open (name, 0)) < 0 )
    {
      LogError((unsigned char *) "Cannot open randomFile \"%s\", errno = %d\n",
               name, errno);
      return 0;
    }

  sum[0] = 0;
  sum[1] = 0;

  if (read(fd, (char *)buf, READ_LIMIT) != READ_LIMIT)
    {
      LogError((unsigned char *) "Could not read %d bytes from '%s'\n",
               READ_LIMIT, name);
      /* cheap fallback */
      sum[0] = (long)time((Time_t *) 0);
      sum[1] = sum[0];
    }
  else
    {
      sum[0] = buf[0];
      sum[1] = buf[1];
      ret_status = 1;
    }

  close(fd);
  return ret_status;
}

#undef READ_LIMIT

#else /* linux || CSRG_BASED */

static int
sumFile (char *name, long sum[2])
{
    long    buf[1024*2];
    int	    cnt;
    int	    fd;
    int	    loops;
    int	    reads;
    int	    i;
    int     ret_status = 0;

    fd = open (name, 0);
    if (fd < 0) {
	LogError((unsigned char *) "Cannot open randomFile \"%s\", errno = %d\n", name, errno);
	return 0;
    }
#ifdef FRAGILE_DEV_MEM
    if (strcmp(name, "/dev/mem") == 0) lseek (fd, (off_t) 0x100000, SEEK_SET);
#endif
    reads = FILE_LIMIT;
    sum[0] = 0;
    sum[1] = 0;
    while ((cnt = read (fd, (char *) buf, sizeof (buf))) > 0 && --reads > 0) {
	loops = cnt / (2 * sizeof (long));
	for (i = 0; i < loops; i+= 2) {
	    sum[0] += buf[i];
	    sum[1] += buf[i+1];
	    ret_status = 1;
	}
    }
    if (cnt < 0)
	LogError((unsigned char *) "Cannot read randomFile \"%s\", errno = %d\n", name, errno);
    close (fd);
    return ret_status;
}
#endif /* linux || CSRG_BASED */

void
GenerateAuthData (char *auth, int len)
{
    long	    ldata[2];

#ifdef ITIMER_REAL
    {
	struct timeval  now;

	X_GETTIMEOFDAY (&now);
	ldata[0] = now.tv_sec;
	ldata[1] = now.tv_usec;
    }
#else
    {
	long    time ();

	ldata[0] = time ((long *) 0);
	ldata[1] = getpid ();
    }
#endif /* ITIMER_REAL */

#ifdef HASXDMAUTH
    {
    	int		    bit;
    	int		    i;
	auth_wrapper_schedule    schedule;
	unsigned char	    tdata[8];
	static int	    xdmcpAuthInited;
    
	longtochars (ldata[0], tdata+0);
	longtochars (ldata[1], tdata+4);
	if (!xdmcpAuthInited)
	{
	    InitXdmcpWrapper ();
	    xdmcpAuthInited = 1;
	}
	_XdmcpAuthSetup (key, schedule);
    	for (i = 0; i < len; i++) {
	    auth[i] = 0;
	    for (bit = 1; bit < 256; bit <<= 1) {
	    	_XdmcpAuthDoIt (tdata, tdata, schedule, 1);
	    	if (tdata[0] + tdata[1] & 0x4)
		    auth[i] |= bit;
	    }
    	}
    }
#else

    InitCryptoKey ();

#if defined(USE_CRYPT)
    {
    	int	    i, j, k;
    	char    *result, *crypt ();
	char	cdata[9];
	long	sdata;
    
	for (j = 0; j < 2; j++)
	{
	    sdata = ldata[j];
	    for (i = 0; i < 4; i++)
	    {
		k = j * 4 + i;
		cdata[k] = sdata & 0xff;
		if (cdata[k] == 0)
		    cdata[k] = 1;
		sdata >>= 8;
	    }
	}
	cdata[8] = '\0';
	for (i = 0; i < len; i += 4)
	{
	    result = crypt (cdata, (const char *) key);
	    k = 4;
	    if (i + k > len)
		k = len - i;
	    for (j = 0; j < k; j++)
		auth[i + j] = result[2 + j];
	    for (j = 0; j < 8; j++)
		cdata[j] = result[2 + j];
	}
    }
#elif defined(USE_ENCRYPT)
    {
    	char    key_bits[64];
    	char    data_bits[64];
    	int	    bit;
    	int	    i;
    
    	bitsToBytes (key, key_bits);
    	bitsToBytes (ldata, data_bits);
    	setkey (key_bits);
    	for (i = 0; i < len; i++) {
	    auth[i] = 0;
	    for (bit = 1; bit < 256; bit <<= 1) {
	    	encrypt (data_bits, 0);
	    	if (data_bits[bit])
		    auth[i] |= bit;
	    }
    	}
    }
#else
    {
    	int	    seed;
    	int	    value;
    	int	    i;
    
    	seed = (ldata[0] + key[0]) +
	      ((ldata[1] + key[1]) << 16);
    	xdm_srand (seed);
    	for (i = 0; i < len; i++)
    	{
	    value = xdm_rand ();
	    auth[i] = (value & 0xff00)  >> 8;
    	}
	value = len;
	if (value > sizeof (key))
	    value = sizeof (key);
    	memmove( (char *) key, auth, value);
    }
#endif
#endif
}

#ifndef HASXDMAUTH

static int  cryptoInited = 0;

int 
InitCryptoKey( void )
{
#if defined(linux) 
    /* non-blocking */
    char    *key_file = "/dev/urandom";
#elif defined(CSRG_BASED) || defined(sun)
    /* non-blocking */
    char    *key_file = "/dev/random";
#else
# warning "Using /dev/mem for random bits."
    /* JET - this seems like a really bad idea. */
    char    *key_file = "/dev/mem";
#endif    
    if (cryptoInited)
	return 0;

    /*
     *  If the sumFile fails to produce a result
     *  use the time of day.
     */
    if (!sumFile (key_file, key)) {

#ifdef ITIMER_REAL
      {
	struct timeval  now;
	struct timezone zone;
	gettimeofday (&now, &zone);
	key[0] = now.tv_sec;
	key[1] = now.tv_usec;
      }
#else
      {
	long    time ();
	
	key[0] = time ((long *) 0);
	key[1] = getpid ();
      }
#endif

    }
    cryptoInited = 1;

    return 1;
}

#endif /* HASXDMAUTH */
