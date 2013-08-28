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
 * $XConsortium: AnchorCanvas.C /main/7 1996/10/21 13:41:12 rcs $
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

#define C_OString
#define C_HashTbl
#define C_NodeViewInfo
#define C_NamedData
#define L_Basic

#define C_AnchorCanvas
#ifdef UseTmlRenderer
#define C_ViewportAgent
#endif
#define L_Agents

#include <Prelude.h>

#define CURRENT_FORMAT "TML-1"

AnchorCanvas::~AnchorCanvas()
{
  // need this because base class was not getting destructed 
}

// Constuctor converts the range to string value for the base Anchor class.


AnchorCanvas::AnchorCanvas (const char *locator, MarkInfo& mi)
{
  // format:
  // locator;vcc[tab]offset[tab]length,...

  UAS_String location(locator);

  location = location + ";";


  /* -------- Now form the anchor string -------- */
  for (unsigned int i = 0; i < mi.length(); i++) {
      UAS_Pointer<MarkUnitInfo> mui_t = mi[i];
      MarkUnitInfo& mui = *mui_t;
      char buf[64];
      if (i > 0)
	  location = location + ",";
      snprintf(buf, sizeof(buf),
			"%u\t%u\t%u", mui.vcc(), mui.offset(), mui.length());
      location = location + buf;
  }

  // Save these things in the base Anchor class. 
  set ((char*)location, CURRENT_FORMAT);
#ifdef BOOKMARK_DEBUG
  fprintf(stderr, "AnchorCanvas() = <%s>\n", (char*)location);
#endif
}


// /////////////////////////////////////////////////////////////////
// yposition - compute yposition of the mark in a given viewport
// /////////////////////////////////////////////////////////////////

#ifdef JBM
Position
AnchorCanvas::yposition (ViewportAgent *vpa)
{
  // First make sure that the Anchor is in the expected format. 
  if (strcmp (format(), CURRENT_FORMAT) != 0)
    throw (Exception());

  // Snag the information: TML-1 format is locator,offset,length.
  char *locator = new char[strlen(location())];
  int offset, length;
  ON_DEBUG (printf ("Stored mark anchor = <%s>\n", location()));
  int count;
  count = sscanf (location(), "%[^,],%d,%d", locator, &offset, &length);
  if (count != 3)
    {
      delete [] locator;
      ON_DEBUG (printf ("Only got %d args\n", count));
      throw (Exception());
    }

  // Find the TML node associated with the locator in the anchor.
  // NOTE: This table should be replaced with template classes.
  bnode *bn;
  if (*locator != '\0')
    {
      ON_DEBUG (printf ("AnchorCanvas: looking up locator <%s>\n", locator));
      NamedData *tml_node_data = (NamedData *)
	vpa->node_view_info()->locator_table().
	find (OString (locator, OString::NO_COPY));
      delete [] locator;

      if (tml_node_data == NULL)
	throw (Exception());

      bn = (bnode *) tml_node_data->data();
    }
  else
    {
      delete [] locator;
      bn = (bnode *) vpa->node_view_info()->get_model()->get_root();
    }

  // Get the y position
  view_pos *vpos =
    vpa->tml_viewport().mpos_2_vpos (bn->get_span().get_start() + offset);

  if (vpos == NULL)
    throw (Exception());

  return (vpos->get_y());
}
#endif
