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
/* $XConsortium: getcgm.c /main/7 1996/07/01 15:35:00 cde-hal $ */
/* short module to interface the external entry point to the guts of the
   CGM interpreter */

#include "spec.h"
#include "cgm.h"


enum _DtGrLoadStatus 
processCGM(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context)
{
  unsigned char *dat_ptr;
  /* declare two CGM state variables, one active, one for defaults */
  static cgm_s_type cgm_act, cgm_def, *cgm_s_ptr;
  static struct cmd_info_s cmd_info;

  /* now go do some work */

  if (stream->type == _DtGrNONE)
      return(_DtGrCONVERT_FAILURE);
  
  if (stream->type == _DtGrFILE) {  /* open the cgm file */
      if (!open_cgm_file(stream)) {
	  burp(stderr, "couldn't open file\n");
	  return _DtGrCONVERT_FAILURE;
      }
  }
  /* stream->type == _DtGrBUFFER */
  if (stream->type == _DtGrBUFFER) {  /* prep the cgm buffer */
    if(!open_cgm_buffer(stream)) {
      burp(stderr, "couldn't open file\n");
      return _DtGrCONVERT_FAILURE;
    }
  }
  
  /* prepare the CGM state records */
  if (!cgm_defs(&cgm_def)) {
    burp(stderr, "couldn't set defaults\n");
    return _DtGrCONVERT_FAILURE;
  }
  if (!cgm_assign(&cgm_def, &cgm_act)) {
    burp(stderr, "couldn't reset to defaults\n");
    return _DtGrCONVERT_FAILURE;
  }
  /* initialise the command info */
  cmd_info.no = 1;
  cmd_info.sub_no = 0;
  /* setup the output device */
  xl_setup(screen, DisplayOfScreen(screen), RootWindowOfScreen(screen),
	   depth, colormap, visual, gc, in_out_width, in_out_height,
	   ret_colors, ret_num_colors, ret_pixmap, media_resolution);
  /* initialize the drawing pixel (for safety) */
  cgm_def.tPixel = BlackPixelOfScreen(screen);

  /* now go thru the input file */
  do {
    /* get next command */
    dat_ptr = get_b_cmd(&cmd_info, stream);
    if (!dat_ptr) {		/* not successful */
      burp(stderr, "EOF at class %d, element %d (%d bytes) ?\n",
	   cmd_info.Class, cmd_info.element, cmd_info.p_len);
      return _DtGrCONVERT_FAILURE;
    }

    /* are we at the metafile descriptor level ? */
    cgm_s_ptr = (cmd_info.Class == B_MDESCLASS) ? &cgm_def : &cgm_act;

    /* do we need to reset to defaults ? */
    if ((cmd_info.Class == B_DELIMCLASS) && 
	(cmd_info.element == B_BPIC) &&
	(!cgm_assign(&cgm_def, &cgm_act))) {
      burp(stderr, "couldn't reset to defaults\n");
      return _DtGrCONVERT_FAILURE;
    }
    if (!do_b_cmd(&cmd_info, dat_ptr, cgm_s_ptr)) {
      burp(stderr, "trouble with cmd, class = %d, element = %d\n",
	   cmd_info.Class, cmd_info.element);
      return _DtGrCONVERT_FAILURE;
    }
    /* update cmd_info */
    ++cmd_info.no;
    /* terminate after performing first end picture or end metafile */
  } while (!((cmd_info.Class == 0) && ((cmd_info.element == B_EPIC) ||
				       (cmd_info.element == B_EMF))));
  return _DtGrSUCCESS;
}
