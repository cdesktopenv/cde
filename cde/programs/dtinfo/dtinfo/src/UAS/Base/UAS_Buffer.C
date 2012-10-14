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
// $XConsortium: UAS_Buffer.cc /main/3 1996/06/11 16:36:44 cde-hal $
#include <string.h>
#include <stdio.h>
#include "UAS_Buffer.hh"

// If necessary in the future, we can rework UAS_Buffer class to provide
// global buffer by default and private buffers on request.  SWM

char *UAS_Buffer::f_start;
char *UAS_Buffer::f_end;
int UAS_Buffer::f_reference_count;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

inline void
UAS_Buffer::init (unsigned int initial_size)
{
  f_start = new char [initial_size];
  f_end = f_start + initial_size;
}  

UAS_Buffer::UAS_Buffer (unsigned int initial_size)
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

UAS_Buffer::~UAS_Buffer()
{
  f_reference_count--;

  if (f_reference_count == 0)
    {
      delete f_start;
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
UAS_Buffer::check_space (unsigned int size)
{
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
static unsigned int g_ordering = 0x01020304;
#else
static unsigned int g_ordering = 0x01020304;
#endif
static const unsigned char *const g_byte_pos =
  (const unsigned char *const) &g_ordering;
static int g_int;
static char *const g_intbuf = (char *) &g_int;

void
UAS_Buffer::write (const ssize_t integer)
{
  char *inbuf = (char *) &integer;

  // Convert bytes from native to MSB first ordering.
  g_int = 0;
  for (int i = 0; i < sizeof (int); i++)
    if (g_byte_pos[i] != 0)
      g_intbuf[g_byte_pos[i]-1] = inbuf[i];

  write (g_intbuf, 4, 1);
}

void
UAS_Buffer::read (int *integer)
{
  *integer = 0;

  // Convert from MSB first ordering to native.
  for (int i = 0; i < sizeof (int); i++)
    if (g_byte_pos[i] != 0)
      ((char *) integer)[i] = f_point[g_byte_pos[i]-1];

  f_point += 4;
}


// /////////////////////////////////////////////////////////////////
// read/write for strings
// /////////////////////////////////////////////////////////////////

void
UAS_Buffer::write (const char *string)
{
  ssize_t length = strlen (string);
  write (length);
  write (string, sizeof (char), length + 1);
}

// NOTE: The caller is responsible for deleting the string that is
// returned by this function!

unsigned int
UAS_Buffer::read (char **string)
{
  int length;
  read (&length);
  *string = new char [length + 1];
  read (*string, sizeof (char), length + 1);
  return (length);
}

unsigned int
UAS_Buffer::read (char *const string)
{
  int length;
  read (&length);
  read (string, sizeof (char), length + 1);
  return (length);
}


// /////////////////////////////////////////////////////////////////
// read/write for bytes
// /////////////////////////////////////////////////////////////////

void
UAS_Buffer::write (const char *bytes, unsigned int size, unsigned int length)
{
  int num_bytes = size * length;
  check_space (num_bytes);
  for (int i = 0; i < size * length; i++)
  memcpy (f_point, bytes, num_bytes);
  f_point += num_bytes;
  f_end_of_data = f_point;
}

void
UAS_Buffer::read (char **bytes, unsigned int size, unsigned int length)
{
  int num_bytes = size * length;
  *bytes = f_point;
  for (int i = 0; i < size * length; i++)
  f_point += num_bytes;
}

void
UAS_Buffer::read (char *const bytes, unsigned int size, unsigned int length)
{
  char *b;
  read (&b, size, length);
  memcpy (bytes, b, size * length);
}
