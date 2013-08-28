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
/* $XConsortium: cgmio.c /main/5 1996/07/01 15:34:30 cde-hal $ */
/* this is the I/O module for the CGM interpreter */
/* we will do all of our I/O with non-specific calls, e.g., fread */
/* this should not be too slow since we intend to no random I/O */
#include "cgm.h"		/* our standard defs */
#include "GraphicsP.h"

/* necessary globals for the input file */

/* we'll get our records BUFSIZ bytes at a time */
#define CGMB_SIZE BUFSIZ		/* might change later */
static unsigned char cgm_buffer[CGMB_SIZE];
/* pointers into the input buffer */
static unsigned char *begin_ptr, *endin_ptr, *in_ptr;
static int io_debug = 0, recs_got, bytes_read;

/* function to get an input record, return no of bytes read */
static int get_cgm_rec(unsigned char *start_ptr,
		       int buffer_size,
		       _DtGrStream *cgmptr)
{
  int bytes_got;
  
  bytes_got = _DtGrRead(start_ptr, 1, buffer_size, cgmptr);
  ++recs_got;
  bytes_read += bytes_got;
  /* printf("bytes_got %d\n",bytes_got);  [PLA] */
  return(bytes_got);
}
/* function to open the CGM file */
/* stream is a file stream */
int open_cgm_file(_DtGrStream *stream)
{
  int bytes_got;
  fprintf(stderr, "Attempting to open file, %s\n", stream->source.file.filename);
  /* no uncompress supported */
  if (!(stream->source.file.fileptr = 
	fopen(stream->source.file.filename, "r"))) {
    burp(stderr, "couldn't open %s for input\n", stream->source.file.filename);
    return(0);
  } else if (io_debug) burp(stderr, "opened %s for input\n",
			    stream->source.file.filename);
  
  /* get the first record */
  begin_ptr = cgm_buffer;
  if ((bytes_got = get_cgm_rec(begin_ptr, CGMB_SIZE, stream)) < 1) {
    burp(stderr, "empty file %s ?\n", stream->source.file.filename);
    return(0);
  }
  endin_ptr = begin_ptr + bytes_got;
  in_ptr = begin_ptr;
  return(1);
}

/* function to prepare the CGM buffer */
/* stream is a buffer stream */
int open_cgm_buffer(_DtGrStream *stream)
{
  int bytes_got;

  /* get the first record */
  begin_ptr = cgm_buffer;
  if ((bytes_got = get_cgm_rec(begin_ptr, CGMB_SIZE, stream)) < 1) {
    burp(stderr, "empty file %s ?\n", stream->source.file.filename);
    return(0);
  }
  endin_ptr = begin_ptr + bytes_got;
  in_ptr = begin_ptr;
  return(1);
}

/* function to get a number of binary format bytes */
static int get_b_bytes(unsigned char *out_ptr,
		       int bytes_wanted, _DtGrStream *stream)
{
  int bytes_got, i;
  if (!begin_ptr) {
    burp(stderr, "haven't opened file yet !\n");
    return 0;
  }
  /* check for legitimacy */
  if (bytes_wanted < 0) {
    burp(stderr, "negative no of bytes requested (%d) ?\n", 
	 bytes_wanted);
    return(0);
  }
  if (bytes_wanted % 2) burp(stderr, "odd %d bytes ?\n", bytes_wanted);
  
  /* move over bytes from input buffer */
  for (i=0; (bytes_wanted > 0) && (in_ptr < endin_ptr); ++i) {
    *out_ptr++ = *in_ptr++;
    --bytes_wanted;
  }
  if (!bytes_wanted) return(1); 	/* finished */
  
  /* get some more */
  while (bytes_wanted >= CGMB_SIZE) {	/* read 'em straight in */
    if ((bytes_got = get_cgm_rec(out_ptr, CGMB_SIZE, stream)) < 1) {
      burp(stderr, "couldn't get bytes in get_b_bytes\n");
      return(0);
    }
    out_ptr += bytes_got;
    bytes_wanted -= bytes_got;
  }
  /* buffer the rest */
  while (bytes_wanted > 0) {
    if ((bytes_got = get_cgm_rec(begin_ptr, CGMB_SIZE, stream)) < 1) {
      burp(stderr, "couldn't get buffered bytes in get_b_bytes\n");
      return(0);
    }
    /* realign the pointers */
    endin_ptr = begin_ptr + bytes_got;
    in_ptr = begin_ptr;
    /* move over bytes from input buffer */
    for (i=0; (bytes_wanted > 0) && (in_ptr < endin_ptr); ++i) {
      *out_ptr++ = *in_ptr++;
      --bytes_wanted;
    }
  }
  
  
  return(1);
}

/* function to input a complete CGM Binary command */
unsigned char *get_b_cmd(struct cmd_info_s *cmd_ptr, _DtGrStream *stream)
{
#define INIT_MEM (50 * 1024)	/* initial allocation */
  static unsigned char *mem_ptr = NULL;
  static int mem_allocated;
  int data_left, new_len;
  
  /* if first time in, get the initial memory allocation */
  if ((!mem_ptr) && (!(mem_ptr = (unsigned char *) malloc(INIT_MEM)))) {
    burp(stderr, "couldn't allocate initial memory for get_b_cmd\n");
    return(NULL);
  } else mem_allocated = INIT_MEM;
  
  /* get the next two bytes */
  if (!get_b_bytes(mem_ptr, 2, stream)) {
    burp(stderr, "trouble getting first 2 bytes of command\n");
    return(NULL);
  }
  /* mark the position */
  cmd_ptr->byte_no = bytes_read - (endin_ptr - in_ptr) - 2;
  
  /* now decipher the command header (lots of magic numbers !)*/
  cmd_ptr->Class = (int) ((mem_ptr[0] >> 4) & 15);
  cmd_ptr->element = (int) (((mem_ptr[0] << 3) & 127) |
			    ((mem_ptr[1] >> 5) & 7));
  cmd_ptr->p_len = (int) (mem_ptr[1] & 31);

  /* is this a short form command ? */
  if (cmd_ptr->p_len < 31) {
    if (!get_b_bytes(mem_ptr, (cmd_ptr->p_len % 2) ? 
		     cmd_ptr->p_len + 1 : cmd_ptr->p_len, stream)) {
      burp(stderr, "trouble with get_b_bytes\n");
      return(NULL);
    } else return(mem_ptr);	/* all done */
  } else {				/* long form */
    data_left = 1;
    cmd_ptr->p_len = 0;			/* start fresh */
    while (data_left) {		/* some data still to get */
      /* need the next two bytes */
      if (!get_b_bytes(mem_ptr + cmd_ptr->p_len, 2, stream)) {
	burp(stderr, "trouble with get_b_bytes\n");
	return(NULL);
      }
      /* how much to come ? */
      new_len = (int) (((mem_ptr[cmd_ptr->p_len] & 127) << 8) | 
		       mem_ptr[cmd_ptr->p_len + 1]);
      if (new_len % 2) ++new_len;	/* round up */
      /* any more to come ? */
      data_left = (mem_ptr[cmd_ptr->p_len] >> 7) & 1;
      /* do we have enough room ? */
      if ((cmd_ptr->p_len + new_len) > mem_allocated) {
	if (!(mem_ptr = (unsigned char *)
	      realloc(mem_ptr, 	mem_allocated = cmd_ptr->p_len + new_len))) {
	  burp(stderr, "couldn't get %d bytes in get_b_cmd\n",
	       mem_allocated);
	  return(NULL);
	} else if (io_debug)
	  burp(stderr, "increasing to %d bytes allocated\n", mem_allocated);
      }
      /* get the bytes needed */
      if (!get_b_bytes(mem_ptr + cmd_ptr->p_len, new_len, stream)) {
	burp(stderr, "couldn't get next %d bytes in get_b_cmd\n",
	     new_len);
	return(NULL);
      }
      cmd_ptr->p_len += new_len;
    }
  }
  return(mem_ptr);		/* all done */
#undef INIT_MEM
}
