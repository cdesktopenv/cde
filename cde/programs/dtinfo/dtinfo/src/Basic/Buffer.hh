/*
 * $XConsortium: Buffer.hh /main/3 1996/06/11 16:18:01 cde-hal $
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


class Buffer
{
public:
  Buffer (u_int initial_size = 1024);
  ~Buffer();

  void init (u_int initial_size = 1024);

  char *point() const
    { return (f_point); }
  // Might want to add assert to make sure point is valid.  15:07 01/12/93 DJB 
  void point (char *new_point)
    { f_point = new_point; }
  const char *data() const
    { return (f_start); }
  u_int length() const
    { return (f_end_of_data - f_start); }
  // Methods for saving buffer independent position inforation, since
  // the buffer can be dynamically reallocated.  You must save offsets
  // if you want to get an existing string out of the buffer after a
  // write. 
  int offset (const char *point) const
    { return (point - f_start); }
  char *position (int offset)
    { return (f_start + offset); }
  // Return the number of bytes of real data remaining after the point. 
  u_int remaining() const
    { return (f_end_of_data - f_point); }
  void reset()
    { f_point = f_start; }

  void write (const int integer);

#ifndef __osf__
  void write (const size_t integer)
    { write ((size_t) ((void *) integer)); }
#else
  void write (const unsigned int integer)
    { write ((int) integer); }
#endif

  void write (const char *string);
  void write (const char *bytes, u_int size, u_int length);

  void read (int *integer);
  void read (unsigned int *integer)
    { read ((int *) integer); }
  // Versions to get copy (space must exist!): 
  u_int read (char *const string);
  void read (char *const bytes, u_int size, u_int length);
  // Versions to point into buffer memory:
  u_int read (char **string);
  void read (char **bytes, u_int size, u_int length);

private:
  void check_space (u_int);

private:
  static char *f_start;
  char        *f_point;
  static char *f_end;
  char        *f_end_of_data;
  static int   f_reference_count;
};
