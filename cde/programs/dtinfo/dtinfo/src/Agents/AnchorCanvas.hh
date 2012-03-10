/*
 * $XConsortium: AnchorCanvas.hh /main/5 1996/10/18 19:46:29 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
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

#include <X11/Intrinsic.h>

#include "Marks/MarkInfo.hh"

// This class provides a simple interface to create an Anchor from
// a selection range in TML. 

class AnchorCanvas : public Anchor
{
public:
  // Creation of unset Anchor. 
  AnchorCanvas()
    { }

  ~AnchorCanvas();

  // Creation of anchor based on position. 
  AnchorCanvas (const char *locator, MarkInfo& mi);

#ifdef JBM
  Position yposition (ViewportAgent *);
#endif
};

