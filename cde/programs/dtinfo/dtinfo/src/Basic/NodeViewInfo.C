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
/*	Copyright (c) 1996 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: NodeViewInfo.C /main/17 1996/11/18 15:50:15 mustafa $
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

#include <limits.h>
#include <wchar.h>

#define C_NodeViewInfo
#define L_Basic

#define C_xList
#define L_Support

#define C_PixmapGraphic
#define L_Graphics

#define C_GraphicsMgr
#define C_MessageMgr
#define C_NodeMgr
#define C_NodeParser
#define L_Managers

#include "Prelude.h"

#include <DtI/LinkMgrP.h>

#undef self
#include <DtI/DisplayAreaP.h>
#include <DtI/FontI.h>
#include <DtI/FontAttrI.h>
#include <DtI/RegionI.h>

#include "OnlineRender/SegClientData.hh"

#include "UAS/DtSR/Util_Classes/DictIter.hh"

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

NodeViewInfo::NodeViewInfo (UAS_Pointer<UAS_Common> &node_ptr, _DtCvTopicInfo *topic)
: f_node_ptr (node_ptr),
  f_topic(topic),
  f_current_hit(NULL),
  f_def_key(""),
  f_def_val((unsigned long)-1),
  f_color_dict(f_def_key, f_def_val)
{
#ifdef DEBUG
  printf( "make NodeViewInfo %p\n\n", this );
#endif
}


/*
   Recursively clean up our segment structures
*/
static void
cleanup(_DtCvSegment *segment)
{
  if (segment)
    {
      switch (segment->type & _DtCvPRIMARY_MASK)
	{
	case _DtCvCONTAINER:
	  if (segment->handle.container.id)
	    free (segment->handle.container.id);
	  cleanup (segment->handle.container.seg_list);
	  break;
	case _DtCvSTRING:
            if (segment->handle.string.string)
                delete (char*)(segment->handle.string.string);
	  if (segment->client_use) {
	    delete (SegClientData*)segment->client_use;
	    segment->client_use = NULL;
	  }
	  break;
	case _DtCvTABLE:
	  {
	    _DtCvTable *table = &segment->handle.table ;
	    if (table->col_w)
	      {
		for (int i = 0 ; i < table->num_cols; i++)
		  delete table->col_w[i] ;
		delete table->col_w ;

	      }
	    delete table->col_justify ;
	    delete table->justify_chars ;
	    char **ids = table->cell_ids ;
	    while (*ids)
	      delete *ids++ ;
	    delete table->cell_ids ;
	    _DtCvSegment **cells = table->cells ;
	    while (*cells)
	      cleanup(*cells++);
	    delete table->cells ;
	  }
	  break ;
	case _DtCvMARKER:
	  delete segment->handle.marker ;
	  break;
	case _DtCvREGION:
	  {
	    _DtHelpDARegion	*pReg = (_DtHelpDARegion*) segment->handle.region.info ;
	    DtHelpGraphicStruct *pGS  = (DtHelpGraphicStruct *) pReg->handle;
	    delete pGS ;
	    delete pReg ;
	    if (segment->client_use) {
              // Note: ((SegClientData*)segment->client_use)->GraphicHandle()
              //       is deleted separately through f_pixmap_list
	      delete (SegClientData*)segment->client_use;
	      segment->client_use = NULL;
	    }
	  }
	  break ;
	}
      cleanup(segment->next_seg);

      delete segment;
    }
}

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

NodeViewInfo::~NodeViewInfo()
{
  _DtLinkDbDestroy(f_topic->link_data);	// kill the link database

  cleanup(f_topic->seg_list);	// kill the segments and their items

  f_topic->seg_list = 0 ;
  delete f_topic ;		// sayonara

  if (f_pixmap_list != NULL)
  {		// explicitly cleanup graphics in f_pixmap_list
    List_Iterator<UAS_Pointer<Graphic> > gli (f_pixmap_list) ;
    while (gli)
    {
#ifdef DEBUG
      printf( "delete UAS_Pointer<Graphic> gli.item() = %p\n",
		(void*)gli.item() );
#endif
      f_pixmap_list.remove(gli) ;
    }
  }
#ifdef DEBUG
  printf( "kill NodeViewInfo %p\n\n", this );
#endif
}

void
NodeViewInfo::add_graphic(UAS_Pointer<Graphic> &pg)
{
  f_pixmap_list.append(pg);

#ifdef DEBUG
  printf( "NodeViewInfo: add UAS_Pointer<Graphic> %p\n", (void*)pg );
  if (f_pixmap_list != NULL)
  {
      List_Iterator<UAS_Pointer<Graphic> > gli (f_pixmap_list) ;
      printf( "graphics list is now: " );
      while (gli)
      {
        printf( " %p ", (void*)gli.item() );
        gli++;
      }
      printf( "\n" );
  }
#endif
}

static unsigned int
DtCvStrLen(_DtCvSegment* seg)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING)
	return 0;

    unsigned int len = 0;

    if (seg->type & _DtCvWIDE_CHAR) {
	wchar_t* seg_str = (wchar_t*)seg->handle.string.string;
	for (; *seg_str; seg_str++, len++);
    }
    else {
	char* seg_str = (char*)seg->handle.string.string;
	for (; *seg_str; seg_str++, len++);
    }

    return len;
}

static unsigned int
DtCvStrVccToIndex(_DtCvSegment* seg, unsigned int vcc)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING
		    || seg->client_use == NULL)
	return (unsigned int)-1;

    unsigned int index = 0;

    unsigned int rel_vcc;
    if ((rel_vcc = vcc - ((SegClientData*)seg->client_use)->vcc()) < 0)
	return (unsigned int)-1;

    if (seg->type & _DtCvWIDE_CHAR) {
	wchar_t* seg_str = (wchar_t*)seg->handle.string.string;
	for (; *seg_str && rel_vcc > 0; seg_str++, index++) {
	    if (*seg_str != ' ' && *seg_str != '\t' && *seg_str != '\n')
		rel_vcc--;
	}
	if (*seg_str == 0 && rel_vcc > 0)
	    index = (unsigned int)-1;
	else {
	    for (; *seg_str; seg_str++, index++) {
		if (*seg_str != ' ' && *seg_str != '\t' && *seg_str != '\n')
		    break;
	    }
	}
    }
    else {
	unsigned char* seg_str = (unsigned char*)seg->handle.string.string;
	for (; *seg_str && rel_vcc > 0; seg_str++, index++) {
	    if (*seg_str != ' ' && *seg_str != '\t' &&
				   *seg_str != '\n' && *seg_str != 0xA0)
		rel_vcc--;
	}
	if (*seg_str == 0 && rel_vcc > 0)
	    index = (unsigned int)-1;
	else {
	    for (; *seg_str; seg_str++, index++) {
		if (*seg_str != ' ' && *seg_str != '\t' &&
				       *seg_str != '\n' && *seg_str != 0xA0)
		    break;
	    }
	}
    }

    return index;
}

static unsigned int
DtCvStrVcLenSync(_DtCvSegment* seg)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING
		    || seg->client_use == NULL)
	return (unsigned int)-1;

    unsigned int vclen = 0;

    if (seg->type & _DtCvWIDE_CHAR) {
	wchar_t* seg_str = (wchar_t*)seg->handle.string.string;
	for (; *seg_str; seg_str++) {
	    if (*seg_str != ' ' && *seg_str != '\t' && *seg_str != '\n')
		vclen++;
	}
    }
    else {
	unsigned char* seg_str = (unsigned char*)seg->handle.string.string;
	for (; *seg_str; seg_str++) {
	    if (*seg_str != ' ' && *seg_str != '\t' &&
				   *seg_str != '\n' && *seg_str != 0xA0)
		vclen++;
	}
    }

    // synchronization
    ((SegClientData*)seg->client_use)->vclen() = vclen;

    return vclen;
}

static unsigned int
DtCvStrVcLen(_DtCvSegment* seg)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING)
	return 0;

    return ((SegClientData*)seg->client_use)->vclen();
}

static unsigned int
DtCvStrVcc(_DtCvSegment* seg)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING
		    || seg->client_use == NULL)
	return (unsigned int)-1;

    return ((SegClientData*)seg->client_use)->vcc();
}

static void
DtCvStrVcc(_DtCvSegment* seg, unsigned int vcc)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING
		    || seg->client_use == NULL)
	return;

    ((SegClientData*)seg->client_use)->vcc() = vcc;
}

// Chop a segment into two separate segments.
// nc is the number of characters of the first segment.

_DtCvSegment*
chop_segment(_DtCvSegment* seg, unsigned int nc)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING)
	return NULL;

    if (nc == 0)
	return NULL;

    unsigned int seg_nc = DtCvStrLen(seg);

    if (nc == seg_nc) {
	assert( nc > 0 );
	return seg;
    }

    unsigned long widec = seg->type & _DtCvWIDE_CHAR;

    void* nseg_str;
    if (widec) {
	wchar_t* src  = (wchar_t*)seg->handle.string.string + nc;
	wchar_t* dest = new wchar_t[seg_nc - nc + 1];
	for (int i = 0; i < seg_nc - nc; i++)
	    dest[i] = src[i];
	dest[seg_nc - nc] = '\0';
	nseg_str = (void*)dest;

	wchar_t* seg_str = (wchar_t*)seg->handle.string.string;
	seg_str[nc] = '\0'; // chop seg string
    }
    else {
	char* src  = (char*)seg->handle.string.string + nc;
	char* dest = new char[seg_nc - nc + 1];
	for (int i = 0; i < seg_nc - nc; i++)
	    dest[i] = src[i];
	dest[seg_nc - nc] = '\0';
	nseg_str = (void*)dest;

	unsigned char* seg_str = (unsigned char*)seg->handle.string.string;
	seg_str[nc] = '\0'; // chop seg string
    }
    DtCvStrVcLenSync(seg);

    unsigned int seg_vcc   = DtCvStrVcc(seg);
    unsigned int seg_vclen = DtCvStrVcLen(seg);

#ifdef SYMBOL_HIGHLIGHT
    _DtCvSegment* symseg = new _DtCvSegment(*seg);
    symseg->next_seg  = seg->next_seg;
    symseg->next_disp = seg->next_disp;
    seg ->next_seg  = symseg;
    seg ->next_disp = symseg;

    symseg->handle.string.string = new char[2];
    *(unsigned char*) symseg->handle.string.string	= 0xA4;
    *((unsigned char*)symseg->handle.string.string + 1) = 0x00;
    symseg->client_use   = NULL;
    symseg->internal_use = NULL;
    symseg->type |=  _DtCvAPP_FLAG1;
    symseg->type &= ~_DtCvWIDE_CHAR;

    if (symseg->type & _DtCvNEW_LINE) // take _DtCvNEW_LINE over
	seg->type &= ~_DtCvNEW_LINE;

    symseg->type &=
		~(_DtCvSEARCH_FLAG | _DtCvSEARCH_BEGIN | _DtCvSEARCH_END);

    seg = symseg;
#endif

    _DtCvSegment* nseg = new _DtCvSegment(*seg);
    nseg->next_seg  = seg->next_seg;
    nseg->next_disp = seg->next_disp;
    seg ->next_seg  = nseg;
    seg ->next_disp = nseg;

    if (nseg->type & _DtCvNEW_LINE) // take _DtCvNEW_LINE over
	seg->type &= ~_DtCvNEW_LINE;

    if (widec)
	nseg->type |= _DtCvWIDE_CHAR;

    nseg->type &= ~_DtCvAPP_FLAG1;

    nseg->type &= ~(_DtCvSEARCH_FLAG | _DtCvSEARCH_BEGIN | _DtCvSEARCH_END);

    nseg->internal_use = NULL;

    nseg->handle.string.string = nseg_str;

    // set client_use properly
    nseg->client_use = (_DtCvPointer)new SegClientData(_DtCvSTRING);
    DtCvStrVcc(nseg, seg_vcc + seg_vclen);
    DtCvStrVcLenSync(nseg);

    return nseg;
}

// FUNCTION highlight_search_hit
// INPUT
//   vcc : vcc of the segment to be highlighted
//   len : character length of the segment to be highlighted
//
// RETURN
//   the segment to be highlighted

_DtCvSegment*
highlight_search_hit(_DtCvSegment* seg, unsigned int vcc, unsigned int len)
{
    if (seg == NULL || (seg->type & _DtCvPRIMARY_MASK) != _DtCvSTRING)
	return NULL;

    if (len <= 0)
	return NULL;

    unsigned int seg_vcc = DtCvStrVcc(seg);
    unsigned int seg_nc  = DtCvStrLen(seg);

    if (vcc < seg_vcc) // vcc falls short
	return NULL;

    unsigned long widec = seg->type & _DtCvWIDE_CHAR;

    // translate rel_vcc (vccs) into nseg_nc (characters)
    unsigned int nseg_nc = DtCvStrVccToIndex(seg, vcc);
    if (nseg_nc == (unsigned int)-1) // vcc is beyond segment
	return NULL;

#ifdef DEBUG
    fprintf(stderr, "(DEBUG) vcc=%d, index=%d, clen=%d\n", vcc, nseg_nc, len);
#endif

    if (len == 0)
	return NULL;
    else if (seg_vcc == vcc && seg_nc == len)
	return seg;

    if (len > seg_nc - nseg_nc) { // len goes beyond this segment
	if (seg->next_seg == NULL) {
	    len = seg_nc - nseg_nc;
	}
	else {
	    _DtCvSegment* sibling = seg->next_seg;
	    unsigned long type = sibling->type & _DtCvPRIMARY_MASK;
	    if (type != _DtCvSTRING || sibling->client_use == NULL ||
		DtCvStrVcc(sibling) == (unsigned int)-1 ||
		(seg->type & _DtCvWIDE_CHAR) != (sibling->type & _DtCvWIDE_CHAR)) {
		len = seg_nc - nseg_nc;
	    }
	    else { // let's merge segments
		seg->next_seg  = sibling->next_seg;
		seg->next_disp = sibling->next_disp;
		sibling->next_seg = NULL;
		sibling->next_disp = NULL;
		
		if (widec) {
		    wchar_t* src = (wchar_t*)sibling->handle.string.string;
		    wchar_t* dst = (wchar_t*)seg->handle.string.string;
		    int len = wcslen(dst) + wcslen(src);
		    seg->handle.string.string = (void*)
			realloc(seg->handle.string.string,
					sizeof(wchar_t) * (len + 1));
		    dst = (wchar_t*)seg->handle.string.string;
		    wcscat(dst, src);
		}
		else {
		    char* src = (char*)sibling->handle.string.string;
		    char* dst = (char*)seg->handle.string.string;
		    int len = strlen(dst) + strlen(dst);
		    seg->handle.string.string = (void*)
			realloc(seg->handle.string.string, len + 1);
		    dst = (char*)seg->handle.string.string;
		    strcat(dst, src);
		}
		DtCvStrVcLenSync(seg);

		if (sibling->handle.string.string)
		    free (sibling->handle.string.string);
		if (sibling->client_use) {
		    delete (SegClientData*)sibling->client_use;
		    sibling->client_use = NULL;
		}

		// NOTE: sibling is kept (i.e. not deleted)
		// this fact may cause problems in next highlight_search_hit

		return highlight_search_hit(seg, vcc, len);
	    }
	}
    }

    if (nseg_nc) {
#ifdef DEBUG	
	if (widec) {
	    char buf[MB_LEN_MAX + 1];
	    wchar_t* debug_p = (wchar_t*)seg->handle.string.string + nseg_nc;
	    int n = wctomb(buf, *debug_p);
	    assert( n >= 0 );
	    *(buf + n) = '\0';
	    fprintf(stderr, "(DEBUG) initial=%s\n", buf);
	}
	else {
	    char* debug_p = (char*)seg->handle.string.string + nseg_nc;
	    fprintf(stderr, "(DEBUG) initial=%c\n", *debug_p);
	}
#endif
	if ((seg = chop_segment(seg, nseg_nc)) == NULL)
	    return NULL;
    }

    if (len != seg_nc - nseg_nc)
	chop_segment(seg, len);

    return seg;
}

static _DtCvSegment *
traverse_for_vcc(_DtCvSegment* seg, unsigned int vcc)
{
    if (seg == NULL)
	return NULL;

    unsigned long seg_ptype = seg->type & _DtCvPRIMARY_MASK;

    if (seg_ptype == _DtCvSTRING && seg->client_use) {

	assert( DtCvStrVcc(seg) <= vcc );

	if (DtCvStrVcc(seg) != (unsigned int)-1) { // check for validity
	    if (vcc < DtCvStrVcc(seg) + DtCvStrVcLen(seg)) // found
		return seg;
	}
    }

    _DtCvSegment* therein = NULL;

    if (seg_ptype == _DtCvCONTAINER) { // traverse subordinates
	_DtCvSegment* subordinates;
	if ((subordinates = seg->handle.container.seg_list) != NULL)
	    therein = traverse_for_vcc(subordinates, vcc);
    }
    else if (seg_ptype == _DtCvTABLE) {
	_DtCvSegment** cell = seg->handle.table.cells;
	for (; *cell; cell++) {
	    if (therein = traverse_for_vcc(*cell, vcc))
		break;
	}
    }
    if (therein)
	return therein;

    // traverse siblings
    _DtCvSegment* siblings;
    if ((siblings = seg->next_seg) != NULL)
	therein = traverse_for_vcc(siblings, vcc);

    return therein;
}

_DtCvSegment*
NodeViewInfo::top_container()
{
    if (f_topic == NULL)
	return NULL;

    // retrieve the top container
    _DtCvSegment* root_seg;
    if ((root_seg = f_topic->seg_list) == NULL)
	return NULL;
    assert( (root_seg->type & _DtCvPRIMARY_MASK) == _DtCvCONTAINER );

    return root_seg;
}

void
clear_search_hits_traverse(_DtCvSegment* seg)
{
    if (seg == NULL)
	return;

    unsigned long seg_ptype = seg->type & _DtCvPRIMARY_MASK;

    if (seg_ptype == _DtCvSTRING && seg->client_use) {
	if (seg->type & _DtCvSEARCH_FLAG)
	    seg->type &=
		~(_DtCvSEARCH_FLAG | _DtCvSEARCH_BEGIN | _DtCvSEARCH_END |
		  _DtCvAPP_FLAG2);
    }

    if (seg_ptype == _DtCvCONTAINER) { // traverse subordinates
	_DtCvSegment* subordinates;
	if ((subordinates = seg->handle.container.seg_list) != NULL)
	    clear_search_hits_traverse(subordinates);
    }
    else if (seg_ptype == _DtCvTABLE) {
	_DtCvSegment** cell = seg->handle.table.cells;
	for (; *cell; cell++)
	    clear_search_hits_traverse(*cell);
    }

    // traverse siblings
    _DtCvSegment* siblings;
    if ((siblings = seg->next_seg) != NULL)
	clear_search_hits_traverse(siblings);

    return;
}

void
NodeViewInfo::clear_search_hits()
{
    // retrieve the top container
    _DtCvSegment* root_seg;
    if ((root_seg = top_container()) == NULL)
	return;

    clear_search_hits_traverse(root_seg);

    f_hits.clearAndDestroy();
    f_current_hit = NULL;
}

void
NodeViewInfo::set_search_hits(UAS_Pointer<UAS_List<UAS_TextRun> >& hits)
{
    if (f_current_hit) { // clear search hits previously displayed
	clear_search_hits();
    }

    // retrieve the top container
    _DtCvSegment* root_seg;
    if ((root_seg = top_container()) == NULL)
	return;

    unsigned int idx;

    for (idx = 0; idx < hits->length(); idx++) { // for each TextRun

	UAS_Pointer<UAS_TextRun>& hit = hits->item(idx);

	_DtCvSegment* seg;

	if ((seg = traverse_for_vcc(root_seg, hit->offset())) == NULL) {
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) traverse_for_vcc failed, offset=%d\n",
								hit->offset());
#endif
	    continue;
	}
#ifdef DEBUG
	else
	    fprintf(stderr, "(DEBUG) traverse_for_vcc succeeded, offset=%d\n",
								hit->offset());
#endif

	assert( (seg->type & _DtCvPRIMARY_MASK) == _DtCvSTRING );

	if (seg = highlight_search_hit(seg, hit->offset(), hit->length())) {
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) highlight_search_hit succeeded, "
			    			"offset=%d\n", hit->offset());
#endif
	    seg->type |=
			_DtCvSEARCH_FLAG | _DtCvSEARCH_BEGIN | _DtCvSEARCH_END;

            sr_DtCvSegment* sr_seg = new sr_DtCvSegment(seg);

	    if (idx == 0) {
		seg->type |= _DtCvAPP_FLAG2;
		f_current_hit = sr_seg;
	    }
	    f_hits.append(sr_seg);
	}
#ifdef DEBUG
	else
	    fprintf(stderr, "(DEBUG) highlight_search_hit failed, offset=%d\n",
								hit->offset());
#endif
    }
}

// returns 0 if upon success. returns -1 if it traversal was not changed.

NodeViewInfo::trav_status_t
NodeViewInfo::adjust_current_search_hit(trav_dir_t dir)
{
    trav_status_t status = SUCCESS;

    CC_TPtrDlistIterator<sr_DtCvSegment> iter(f_hits);

    for (++iter; *iter.key() != *f_current_hit;)
	++iter;

    if (dir == PREV) {
	if (*iter.key() != *f_hits.first())
	    --iter;
    }
    else {
	assert( dir == NEXT );
	if (*iter.key() != *f_hits.last())
	    ++iter;
    }

    if (*f_current_hit != *iter.key()) {
	f_current_hit->hitseg->type &= ~_DtCvAPP_FLAG2;

	f_current_hit = iter.key();
	f_current_hit->hitseg->type |=  _DtCvAPP_FLAG2;

	if (dir == PREV) {
	    if (*f_current_hit == *f_hits.first())
		status = REACH_LIMIT;
	}
	else
	    if (*f_current_hit == *f_hits.last())
		status = REACH_LIMIT;
    }
    else
	status = NOT_MOVED;

    return status;
}

int
NodeViewInfo::hit_entries() const
{
    return f_hits.entries();
}

int
NodeViewInfo::search_hit_idx()
{
    int i;

    CC_TPtrDlistIterator<sr_DtCvSegment> iter(f_hits);

    for (i = 0, ++iter; *iter.key() != *f_current_hit; i++)
	++iter;

    return i;
}

void
NodeViewInfo::comp_pixel_values_traverse(_DtCvSegment* seg, Display* dpy,
					 Colormap &cmap)
{
    if (seg == NULL)
	return;

    unsigned long seg_ptype = seg->type & _DtCvPRIMARY_MASK;

    if (seg_ptype == _DtCvSTRING && seg->client_use) {
	XColor screen, exact;
	SegClientData* pSCD = (SegClientData*)seg->client_use;
	assert( pSCD->type() == _DtCvSTRING );

	if (pSCD->bg_color() && pSCD->bg_pixel() == (unsigned long)-1) {
	    UAS_String bg_color = pSCD->bg_color();
	    unsigned long& bg_pixel = f_color_dict[bg_color];
	    if (bg_pixel == (unsigned long)-1) {
		if (XAllocNamedColor(dpy, cmap, pSCD->bg_color(),
							&screen, &exact))
		    pSCD->bg_pixel(bg_pixel = screen.pixel);
	    }
	    else
		pSCD->bg_pixel(bg_pixel);
	}

	if (pSCD->fg_color() && pSCD->fg_pixel() == (unsigned long)-1) {
	    UAS_String fg_color = pSCD->fg_color();
	    unsigned long& fg_pixel = f_color_dict[fg_color];
	    if (fg_pixel == (unsigned long)-1) {
		if (XAllocNamedColor(dpy, cmap, pSCD->fg_color(),
							&screen, &exact))
		    pSCD->fg_pixel(fg_pixel = screen.pixel);
	    }
	    else
		pSCD->fg_pixel(fg_pixel);
	}
    }

    if (seg_ptype == _DtCvCONTAINER) { // traverse subordinates
	_DtCvSegment* subordinates;
	if ((subordinates = seg->handle.container.seg_list) != NULL)
	    comp_pixel_values_traverse(subordinates, dpy, cmap);
    }

    // traverse siblings
    _DtCvSegment* siblings;
    if ((siblings = seg->next_seg) != NULL)
	comp_pixel_values_traverse(siblings, dpy, cmap);

    return;
}

void
NodeViewInfo::comp_pixel_values(Display* dpy, Colormap &cmap)
{
    // retrieve the top container
    _DtCvSegment* root_seg;
    if ((root_seg = top_container()) == NULL)
	return;

    comp_pixel_values_traverse(root_seg, dpy, cmap);

#ifdef CM_DEBUG
    DictIter<UAS_String, unsigned long> dictiter;
    for (dictiter = f_color_dict.first(); dictiter() ; dictiter++) {
	fprintf(stderr, "(DEBUG) color=\"%s\", pixel=0x%lx\n",
				(char*)dictiter.key(), dictiter.value());
    }
#endif

    return;
}

_DtCvSegment*
NodeViewInfo::get_segment(unsigned int vcc)
{
  return traverse_for_vcc(top_container(), vcc);
}

unsigned int
NodeViewInfo::segment_to_vcc(_DtCvSegment* seg)
{
  return DtCvStrVcc(seg);
}

