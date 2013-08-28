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
/*
 * $XConsortium: Buffer.C /main/5 1996/10/04 11:25:25 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#define C_Buffer
#define L_Basic

#include "Prelude.h"

#include <string.h>

// If necessary in the future, we can rework Buffer class to provide
// global buffer by default and private buffers on request.  DJB 

char *Buffer::f_start;
char *Buffer::f_end;
int Buffer::f_reference_count;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

inline void
Buffer::init (u_int initial_size)
{
  f_start = new char [initial_size];
  f_end = f_start + initial_size;
}  

Buffer::Buffer (u_int initial_size)
{
  //  printf ("Constructing a %d byte buffer\n", initial_size);
  if (f_start == NULL)
    init (initial_size);
  f_point = f_start;
  f_end_of_data = f_point;
  f_reference_count++;
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

Buffer::~Buffer()
{
  f_reference_count--;

  if (f_reference_count == 0)
    {
      delete[] f_start;
      f_start = NULL;
      f_point = NULL;
      f_end = NULL;
      f_end_of_data = NULL;
    }
}


// /////////////////////////////////////////////////////////////////
// check_space - make sure the is space for additional data
// /////////////////////////////////////////////////////////////////

void
Buffer::check_space (u_int size)
{
  assert (f_start != NULL);
  assert (f_point != NULL);
  assert (f_end != NULL);
  assert (f_end_of_data != NULL);
  if (f_point + size > f_end)
    {
      // Allocate new, larger space. 
      char *new_block = new char [f_point + size - f_start];
      // Copy bytes in use up to (but not including) point.
      if (f_point - f_start > 0)
	memcpy (new_block, f_start, f_point - f_start);
      // Compute the point in the new buffer. 
      f_point = new_block + (f_point - f_start);
      delete f_start;
      // Update start and end relative to new buffer. 
      f_start = new_block;
      f_end = f_start + size;
    }
}


// /////////////////////////////////////////////////////////////////
// read/write for integers
// /////////////////////////////////////////////////////////////////

#ifdef SVR4
static u_int g_ordering = 0x01020304;
#else
static u_int g_ordering = 0x01020304;
#endif
static const unsigned char *const g_byte_pos =
  (const unsigned char *const) &g_ordering;
static int g_int;
static char *const g_intbuf = (char *) &g_int;

void
Buffer::write (const int integer)
{
  char *inbuf = (char *) &integer;

  // Convert bytes from native to MSB first ordering.
  g_int = 0;
  for (unsigned int i = 0; i < sizeof (int); i++)
    if (g_byte_pos[i] != 0)
      g_intbuf[g_byte_pos[i]-1] = inbuf[i];

  write (g_intbuf, 4, 1);
}

void
Buffer::read (int *integer)
{
  *integer = 0;

  // Convert from MSB first ordering to native.
  for (unsigned int i = 0; i < sizeof (int); i++)
    if (g_byte_pos[i] != 0)
      ((char *) integer)[i] = f_point[g_byte_pos[i]-1];

  f_point += 4;
  assert (f_point <= f_end_of_data);
}


// /////////////////////////////////////////////////////////////////
// read/write for strings
// /////////////////////////////////////////////////////////////////

void
Buffer::write (const char *string)
{
  int length = strlen (string);
  write (length);
  write (string, sizeof (char), length + 1);
}

// NOTE: The caller is responsible for deleting the string that is
// returned by this function!

u_int
Buffer::read (char **string)
{
  int length;
  read (&length);
  ON_DEBUG(printf ("Reading a %d byte string\n", length));
  *string = new char [length + 1];
  read (*string, sizeof (char), length + 1);
  ON_DEBUG(printf ("String read is actually %ld bytes\n", (long)strlen(*string)));
  assert (length == strlen (*string));
  ON_DEBUG(printf ("Read string <%s>\n", *string));
  return (length);
}

u_int
Buffer::read (char *const string)
{
  int length;
  read (&length);
  read (string, sizeof (char), length + 1);
  assert (length == strlen (string));
  return (length);
}


// /////////////////////////////////////////////////////////////////
// read/write for bytes
// /////////////////////////////////////////////////////////////////

void
Buffer::write (const char *bytes, u_int size, u_int length)
{
  ON_DEBUG(printf ("Writing %d bytes to buffer @ %p\n", size * length, f_point));
  int num_bytes = size * length;
  check_space (num_bytes);
  for (unsigned int i = 0; i < size * length; i++)
    ON_DEBUG(printf ("%02X ", (unsigned char) bytes[i]));
  ON_DEBUG(printf ("\n"));
  memcpy (f_point, bytes, num_bytes);
  f_point += num_bytes;
  f_end_of_data = f_point;
}

void
Buffer::read (char **bytes, u_int size, u_int length)
{
  int num_bytes = size * length;
  ON_DEBUG(printf ("Reading %d bytes from buffer @ %p\n", num_bytes, f_point));
  assert (f_point + num_bytes <= f_end_of_data);
  *bytes = f_point;
  for (unsigned int i = 0; i < size * length; i++)
    ON_DEBUG(printf ("%02X ", (unsigned int) (*bytes)[i]));
  ON_DEBUG(printf ("\n"));
  f_point += num_bytes;
}

void
Buffer::read (char *const bytes, u_int size, u_int length)
{
  char *b;
  read (&b, size, length);
  memcpy (bytes, b, size * length);
}
