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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: sdl.c /main/5 1998/04/06 13:19:43 mgreess $ */
/*   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */
/* Miscellaneous Procedures for HP Tag/TeX translator */

#include <sys/stat.h>
#include <sys/wait.h>
#if defined(__linux__)
#  include <sys/fcntl.h>
#endif

#include <errno.h>

#if defined(__linux__)
#include <getopt.h>
#endif

#include "userinc.h"
#include "globdec.h"

#include "LocaleXlate.h"
#include "XlationSvc.h"

static char *operantLocale;

static char openLoids[]  = "<LOIDS COUNT=\"%d\">\n";
static char closeLoids[] = "</LOIDS>\n";
static int  nIds;

static char wideCharacterString[] = "wide character string";


char *mb_malloc(long size)
{
char   *cptr;
size_t  ssize;

ssize = (size_t) size;
cptr = (char *) malloc(ssize);

#if DEBUG_MB_MALLOC
fprintf(stdout, "malloc 0x%p\n", cptr);
fflush(stdout);
#endif

*cptr = '\0';
return cptr;
}


void mb_free(char **pptr)
{

#if DEBUG_MB_MALLOC
fprintf(stdout, "free 0x%p\n", *pptr);
fflush(stdout);
#endif

free((void *) *pptr);
*pptr = NULL;
}


char *mb_realloc(char *ptr, long size)
{
void   *vptr;
size_t  ssize;

ssize = (size_t) size;
vptr = realloc((void *) ptr, ssize);

#if DEBUG_MB_MALLOC
if (ptr != vptr)
    {
    fprintf(stdout, "realloc 0x%p 0x%p\n", ptr, vptr);
    fflush(stdout);
    }
#endif

return (char *) vptr;
}


void OpenTag(LOGICAL hasAttributes, LOGICAL newlineOK)
{
PutString("<");
PutWString(m_parent(0));
if (!hasAttributes)
    {
    PutString(">");
    if (newlineOK)
	PutString("\n");
    }
}


static void OpenNamedTag(char *name, LOGICAL hasAttributes, LOGICAL newlineOK)
{
PutString("<");
PutString(name);
if (!hasAttributes)
    {
    PutString(">");
    if (newlineOK)
	PutString("\n");
    }
}


void CloseTag(LOGICAL newlineOK)
{
char *etagc;

etagc = ">";
if (newlineOK)
    etagc = ">\n";

PutString("</");
PutWString(m_parent(0));
PutString(etagc);
}


static void CloseNamedTag(char *name, LOGICAL newlineOK)
{
char *etagc;

etagc = ">";
if (newlineOK)
    etagc = ">\n";

PutString("</");
PutString(name);
PutString(etagc);
}


void ImpliedAttribute(char *name, M_WCHAR *value)
{
if (value)
    {
    PutString(" ");
    PutString(name);
    PutString("=\"");
    PutWString(value);
    PutString("\"");
    }
}


void RequiredAttribute(char *name, M_WCHAR *value)
{
M_WCHAR  null;
M_WCHAR *wc_name;

if (!value)
    {
    null = 0;
    value = &null;
    wc_name = MakeWideCharString(name);
    m_err2("Missing %s in %s", wc_name, m_parent(0));
    m_free(wc_name, "wide character name");
    }
PutString(" ");
PutString(name);
PutString("=\"");
PutWString(value);
PutString("\"");
}


M_WCHAR *CopyWString(M_WCHAR *string)
{
M_WCHAR *retVal;

if (!string) return NULL;

retVal = m_malloc(w_strlen(string) + 1, wideCharacterString);
w_strcpy(retVal, string);
return retVal;
}


void MakeNewElement(ElementTypes type)
{
pCurrentElement = (ElementPtr) mb_malloc(sizeof(Element));
memset((void *) pCurrentElement, 0, sizeof(Element));
pCurrentElement->type = type;
if (type == e_cdata)
    {
    pCurrentElement->u.u_cdata.buffer   = m_malloc(256, "u_cdata.buffer");
    pCurrentElement->u.u_cdata.buffPos  = -1;
    pCurrentElement->u.u_cdata.buffSize = 256;
    }
}


void DeleteElement(ElementPtr pElement)
{
switch (pElement->type)
    {
    case e_sdldoc:
	if (pElement->u.u_sdldoc.pub_id)
	    m_free(pElement->u.u_sdldoc.pub_id, wideCharacterString);
	if (pElement->u.u_sdldoc.doc_id)
	    m_free(pElement->u.u_sdldoc.doc_id, wideCharacterString);
	if (pElement->u.u_sdldoc.first_page)
	    m_free(pElement->u.u_sdldoc.first_page, wideCharacterString);
	if (pElement->u.u_sdldoc.timestmp)
	    m_free(pElement->u.u_sdldoc.timestmp, wideCharacterString);
	if (pElement->u.u_sdldoc.product)
	    m_free(pElement->u.u_sdldoc.product, wideCharacterString);
	if (pElement->u.u_sdldoc.prodpn)
	    m_free(pElement->u.u_sdldoc.prodpn, wideCharacterString);
	if (pElement->u.u_sdldoc.prodver)
	    m_free(pElement->u.u_sdldoc.prodver, wideCharacterString);
	if (pElement->u.u_sdldoc.license)
	    m_free(pElement->u.u_sdldoc.license, wideCharacterString);
	if (pElement->u.u_sdldoc.language)
	    m_free(pElement->u.u_sdldoc.language, wideCharacterString);
	if (pElement->u.u_sdldoc.charset)
	    m_free(pElement->u.u_sdldoc.charset, wideCharacterString);
	if (pElement->u.u_sdldoc.author)
	    m_free(pElement->u.u_sdldoc.author, wideCharacterString);
	if (pElement->u.u_sdldoc.version)
	    m_free(pElement->u.u_sdldoc.version, wideCharacterString);
	if (pElement->u.u_sdldoc.sdldtd)
	    m_free(pElement->u.u_sdldoc.sdldtd, wideCharacterString);
	if (pElement->u.u_sdldoc.srcdtd)
	    m_free(pElement->u.u_sdldoc.srcdtd, wideCharacterString);
	break;
    case e_virpage:
	if (pElement->u.u_virpage.id)
	    m_free(pElement->u.u_virpage.id, wideCharacterString);
	if (pElement->u.u_virpage.level)
	    m_free(pElement->u.u_virpage.level, wideCharacterString);
	if (pElement->u.u_virpage.version)
	    m_free(pElement->u.u_virpage.version, wideCharacterString);
	if (pElement->u.u_virpage.language)
	    m_free(pElement->u.u_virpage.language, wideCharacterString);
	if (pElement->u.u_virpage.charset)
	    m_free(pElement->u.u_virpage.charset, wideCharacterString);
	if (pElement->u.u_virpage.doc_id)
	    m_free(pElement->u.u_virpage.doc_id, wideCharacterString);
	if (pElement->u.u_virpage.ssi)
	    m_free(pElement->u.u_virpage.ssi, wideCharacterString);
	break;
    case e_snb:
	if (pElement->u.u_snb.version)
	    m_free(pElement->u.u_snb.version, wideCharacterString);
	break;
    case e_block:
	if (pElement->u.u_block.id)
	    m_free(pElement->u.u_block.id, wideCharacterString);
	if (pElement->u.u_block.level)
	    m_free(pElement->u.u_block.level, wideCharacterString);
	if (pElement->u.u_block.version)
	    m_free(pElement->u.u_block.version, wideCharacterString);
	if (pElement->u.u_block.class)
	    m_free(pElement->u.u_block.class, wideCharacterString);
	if (pElement->u.u_block.language)
	    m_free(pElement->u.u_block.language, wideCharacterString);
	if (pElement->u.u_block.charset)
	    m_free(pElement->u.u_block.charset, wideCharacterString);
	if (pElement->u.u_block.length)
	    m_free(pElement->u.u_block.length, wideCharacterString);
	if (pElement->u.u_block.app)
	    m_free(pElement->u.u_block.app, wideCharacterString);
	if (pElement->u.u_block.timing)
	    m_free(pElement->u.u_block.timing, wideCharacterString);
	if (pElement->u.u_block.ssi)
	    m_free(pElement->u.u_block.ssi, wideCharacterString);
	break;
    case e_form:
	if (pElement->u.u_form.id)
	    m_free(pElement->u.u_form.id, wideCharacterString);
	if (pElement->u.u_form.level)
	    m_free(pElement->u.u_form.level, wideCharacterString);
	if (pElement->u.u_form.version)
	    m_free(pElement->u.u_form.version, wideCharacterString);
	if (pElement->u.u_form.class)
	    m_free(pElement->u.u_form.class, wideCharacterString);
	if (pElement->u.u_form.language)
	    m_free(pElement->u.u_form.language, wideCharacterString);
	if (pElement->u.u_form.charset)
	    m_free(pElement->u.u_form.charset, wideCharacterString);
	if (pElement->u.u_form.length)
	    m_free(pElement->u.u_form.length, wideCharacterString);
	if (pElement->u.u_form.app)
	    m_free(pElement->u.u_form.app, wideCharacterString);
	if (pElement->u.u_form.ssi)
	    m_free(pElement->u.u_form.ssi, wideCharacterString);
	break;
    case e_fstyle:
	if (pElement->u.u_fstyle.ncols)
	    m_free(pElement->u.u_fstyle.ncols, wideCharacterString);
	break;
    case e_frowvec:
	if (pElement->u.u_frowvec.hdr)
	    m_free(pElement->u.u_frowvec.hdr, wideCharacterString);
	if (pElement->u.u_frowvec.cells)
	    m_free(pElement->u.u_frowvec.cells, wideCharacterString);
	break;
    case e_fdata:
	break;
    case e_p:
	if (pElement->u.u_p.id)
	    m_free(pElement->u.u_p.id, wideCharacterString);
	if (pElement->u.u_p.version)
	    m_free(pElement->u.u_p.version, wideCharacterString);
	if (pElement->u.u_p.type)
	    m_free(pElement->u.u_p.type, wideCharacterString);
	if (pElement->u.u_p.ssi)
	    m_free(pElement->u.u_p.ssi, wideCharacterString);
	break;
    case e_cp:
	if (pElement->u.u_cp.id)
	    m_free(pElement->u.u_cp.id, wideCharacterString);
	if (pElement->u.u_cp.version)
	    m_free(pElement->u.u_cp.version, wideCharacterString);
	if (pElement->u.u_cp.type)
	    m_free(pElement->u.u_cp.type, wideCharacterString);
	if (pElement->u.u_cp.ssi)
	    m_free(pElement->u.u_cp.ssi, wideCharacterString);
	break;
    case e_head:
	if (pElement->u.u_head.id)
	    m_free(pElement->u.u_head.id, wideCharacterString);
	if (pElement->u.u_head.version)
	    m_free(pElement->u.u_head.version, wideCharacterString);
	if (pElement->u.u_head.class)
	    m_free(pElement->u.u_head.class, wideCharacterString);
	if (pElement->u.u_head.language)
	    m_free(pElement->u.u_head.language, wideCharacterString);
	if (pElement->u.u_head.charset)
	    m_free(pElement->u.u_head.charset, wideCharacterString);
	if (pElement->u.u_head.type)
	    m_free(pElement->u.u_head.type, wideCharacterString);
	if (pElement->u.u_head.ssi)
	    m_free(pElement->u.u_head.ssi, wideCharacterString);
	if (pElement->u.u_head.abbrev)
	    m_free(pElement->u.u_head.abbrev, wideCharacterString);
	break;
    case e_subhead:
	if (pElement->u.u_subhead.id)
	    m_free(pElement->u.u_subhead.id, wideCharacterString);
	if (pElement->u.u_subhead.version)
	    m_free(pElement->u.u_subhead.version, wideCharacterString);
	if (pElement->u.u_subhead.class)
	    m_free(pElement->u.u_subhead.class, wideCharacterString);
	if (pElement->u.u_subhead.language)
	    m_free(pElement->u.u_subhead.language, wideCharacterString);
	if (pElement->u.u_subhead.charset)
	    m_free(pElement->u.u_subhead.charset, wideCharacterString);
	if (pElement->u.u_subhead.type)
	    m_free(pElement->u.u_subhead.type, wideCharacterString);
	if (pElement->u.u_subhead.ssi)
	    m_free(pElement->u.u_subhead.ssi, wideCharacterString);
	if (pElement->u.u_subhead.abbrev)
	    m_free(pElement->u.u_subhead.abbrev, wideCharacterString);
	break;
    case e_anchor:
	if (pElement->u.u_anchor.id)
	    m_free(pElement->u.u_anchor.id, wideCharacterString);
	break;
    case e_link:
	if (pElement->u.u_link.rid)
	    m_free(pElement->u.u_link.rid, wideCharacterString);
	if (pElement->u.u_link.button)
	    m_free(pElement->u.u_link.button, wideCharacterString);
	if (pElement->u.u_link.linkinfo)
	    m_free(pElement->u.u_link.linkinfo, wideCharacterString);
	if (pElement->u.u_link.descript)
	    m_free(pElement->u.u_link.descript, wideCharacterString);
	if (pElement->u.u_link.window)
	    m_free(pElement->u.u_link.window, wideCharacterString);
	if (pElement->u.u_link.traversal)
	    m_free(pElement->u.u_link.traversal, wideCharacterString);
	break;
    case e_snref:
	if (pElement->u.u_snref.id)
	    m_free(pElement->u.u_snref.id, wideCharacterString);
	break;
    case e_refitem:
	if (pElement->u.u_refitem.rid)
	    m_free(pElement->u.u_refitem.rid, wideCharacterString);
	if (pElement->u.u_refitem.class)
	    m_free(pElement->u.u_refitem.class, wideCharacterString);
	if (pElement->u.u_refitem.button)
	    m_free(pElement->u.u_refitem.button, wideCharacterString);
	if (pElement->u.u_refitem.ssi)
	    m_free(pElement->u.u_refitem.ssi, wideCharacterString);
	break;
    case e_alttext:
	break;
    case e_rev:
	break;
    case e_key:
	if (pElement->u.u_key.charset)
	    m_free(pElement->u.u_key.charset, wideCharacterString);
	if (pElement->u.u_key.class)
	    m_free(pElement->u.u_key.class, wideCharacterString);
	if (pElement->u.u_key.ssi)
	    m_free(pElement->u.u_key.ssi, wideCharacterString);
	break;
    case e_sphrase:
	if (pElement->u.u_sphrase.class)
	    m_free(pElement->u.u_sphrase.class, wideCharacterString);
	if (pElement->u.u_sphrase.ssi)
	    m_free(pElement->u.u_sphrase.ssi, wideCharacterString);
	break;
    case e_if:
	break;
    case e_cond:
	if (pElement->u.u_cond.interp)
	    m_free(pElement->u.u_cond.interp, wideCharacterString);
	break;
    case e_then:
	break;
    case e_else:
	break;
    case e_spc:
	if (pElement->u.u_spc.name)
	    m_free(pElement->u.u_spc.name, wideCharacterString);
	break;
    case e_graphic:
	if (pElement->u.u_graphic.id)
	    m_free(pElement->u.u_graphic.id, wideCharacterString);
	if (pElement->u.u_graphic.format)
	    m_free(pElement->u.u_graphic.format, wideCharacterString);
	if (pElement->u.u_graphic.method)
	    m_free(pElement->u.u_graphic.method, wideCharacterString);
	if (pElement->u.u_graphic.xid)
	    m_free(pElement->u.u_graphic.xid, wideCharacterString);
	break;
    case e_text:
	if (pElement->u.u_text.id)
	    m_free(pElement->u.u_text.id, wideCharacterString);
	if (pElement->u.u_text.language)
	    m_free(pElement->u.u_text.language, wideCharacterString);
	if (pElement->u.u_text.charset)
	    m_free(pElement->u.u_text.charset, wideCharacterString);
	break;
    case e_audio:
	if (pElement->u.u_audio.id)
	    m_free(pElement->u.u_audio.id, wideCharacterString);
	if (pElement->u.u_audio.format)
	    m_free(pElement->u.u_audio.format, wideCharacterString);
	if (pElement->u.u_audio.method)
	    m_free(pElement->u.u_audio.method, wideCharacterString);
	if (pElement->u.u_audio.xid)
	    m_free(pElement->u.u_audio.xid, wideCharacterString);
	break;
    case e_video:
	if (pElement->u.u_video.id)
	    m_free(pElement->u.u_video.id, wideCharacterString);
	if (pElement->u.u_video.format)
	    m_free(pElement->u.u_video.format, wideCharacterString);
	if (pElement->u.u_video.method)
	    m_free(pElement->u.u_video.method, wideCharacterString);
	if (pElement->u.u_video.xid)
	    m_free(pElement->u.u_video.xid, wideCharacterString);
	break;
    case e_animate:
	if (pElement->u.u_animate.id)
	    m_free(pElement->u.u_animate.id, wideCharacterString);
	if (pElement->u.u_animate.format)
	    m_free(pElement->u.u_animate.format, wideCharacterString);
	if (pElement->u.u_animate.method)
	    m_free(pElement->u.u_animate.method, wideCharacterString);
	if (pElement->u.u_animate.xid)
	    m_free(pElement->u.u_animate.xid, wideCharacterString);
	break;
    case e_script:
	if (pElement->u.u_script.id)
	    m_free(pElement->u.u_script.id, wideCharacterString);
	if (pElement->u.u_script.interp)
	    m_free(pElement->u.u_script.interp, wideCharacterString);
	break;
    case e_crossdoc:
	if (pElement->u.u_crossdoc.id)
	    m_free(pElement->u.u_crossdoc.id, wideCharacterString);
	if (pElement->u.u_crossdoc.xid)
	    m_free(pElement->u.u_crossdoc.xid, wideCharacterString);
	break;
    case e_man_page:
	if (pElement->u.u_man_page.id)
	    m_free(pElement->u.u_man_page.id, wideCharacterString);
	if (pElement->u.u_man_page.xid)
	    m_free(pElement->u.u_man_page.xid, wideCharacterString);
	break;
    case e_textfile:
	if (pElement->u.u_textfile.id)
	    m_free(pElement->u.u_textfile.id, wideCharacterString);
	if (pElement->u.u_textfile.xid)
	    m_free(pElement->u.u_textfile.xid, wideCharacterString);
	if (pElement->u.u_textfile.offset)
	    m_free(pElement->u.u_textfile.offset, wideCharacterString);
	if (pElement->u.u_textfile.format)
	    m_free(pElement->u.u_textfile.format, wideCharacterString);
	break;
    case e_sys_cmd:
	if (pElement->u.u_sys_cmd.id)
	    m_free(pElement->u.u_sys_cmd.id, wideCharacterString);
	if (pElement->u.u_sys_cmd.command)
	    m_free(pElement->u.u_sys_cmd.command, wideCharacterString);
	break;
    case e_callback:
	if (pElement->u.u_callback.id)
	    m_free(pElement->u.u_callback.id, wideCharacterString);
	if (pElement->u.u_callback.data)
	    m_free(pElement->u.u_callback.data, wideCharacterString);
	break;
    case e_switch:
	if (pElement->u.u_switch.id)
	    m_free(pElement->u.u_switch.id, wideCharacterString);
	if (pElement->u.u_switch.interp)
	    m_free(pElement->u.u_switch.interp, wideCharacterString);
	if (pElement->u.u_switch.branches)
	    m_free(pElement->u.u_switch.branches, wideCharacterString);
	break;
    case e_cdata:
	m_free(pElement->u.u_cdata.buffer, "u_cdata.buffer");
	break;
    case e_rel_docs:
	break;
    case e_rel_file:
	break;
    case e_notes:
	break;
    case e_loids:
	break;
    case e_id:
	if (pElement->u.u_id.type)
	    m_free(pElement->u.u_id.type, wideCharacterString);
	if (pElement->u.u_id.rid)
	    m_free(pElement->u.u_id.rid, wideCharacterString);
	if (pElement->u.u_id.rssi)
	    m_free(pElement->u.u_id.rssi, wideCharacterString);
	if (pElement->u.u_id.rlevel)
	    m_free(pElement->u.u_id.rlevel, wideCharacterString);
	if (pElement->u.u_id.offset)
	    m_free(pElement->u.u_id.offset, wideCharacterString);
	break;
    case e_index:
	break;
    case e_entry:
	if (pElement->u.u_entry.id)
	    m_free(pElement->u.u_entry.id, wideCharacterString);
	if (pElement->u.u_entry.main)
	    m_free(pElement->u.u_entry.main, wideCharacterString);
	if (pElement->u.u_entry.locs)
	    m_free(pElement->u.u_entry.locs, wideCharacterString);
	if (pElement->u.u_entry.syns)
	    m_free(pElement->u.u_entry.syns, wideCharacterString);
	if (pElement->u.u_entry.sort)
	    m_free(pElement->u.u_entry.sort, wideCharacterString);
	break;
    case e_lophrases:
	break;
    case e_phrase:
	break;
    case e_toss:
	break;
    case e_keystyle:
	if (pElement->u.u_keystyle.class)
	    m_free(pElement->u.u_keystyle.class, wideCharacterString);
	if (pElement->u.u_keystyle.ssi)
	    m_free(pElement->u.u_keystyle.ssi, wideCharacterString);
	if (pElement->u.u_keystyle.rlevel)
	    m_free(pElement->u.u_keystyle.rlevel, wideCharacterString);
	if (pElement->u.u_keystyle.phrase)
	    m_free(pElement->u.u_keystyle.phrase, wideCharacterString);
	if (pElement->u.u_keystyle.srch_wt)
	    m_free(pElement->u.u_keystyle.srch_wt, wideCharacterString);
	if (pElement->u.u_keystyle.enter)
	    m_free(pElement->u.u_keystyle.enter, wideCharacterString);
	if (pElement->u.u_keystyle.exit)
	    m_free(pElement->u.u_keystyle.exit, wideCharacterString);
	if (pElement->u.u_keystyle.pointsz)
	    m_free(pElement->u.u_keystyle.pointsz, wideCharacterString);
	if (pElement->u.u_keystyle.set_width)
	    m_free(pElement->u.u_keystyle.set_width, wideCharacterString);
	if (pElement->u.u_keystyle.color)
	    m_free(pElement->u.u_keystyle.color, wideCharacterString);
	if (pElement->u.u_keystyle.xlfd)
	    m_free(pElement->u.u_keystyle.xlfd, wideCharacterString);
	if (pElement->u.u_keystyle.xlfdi)
	    m_free(pElement->u.u_keystyle.xlfdi, wideCharacterString);
	if (pElement->u.u_keystyle.xlfdb)
	    m_free(pElement->u.u_keystyle.xlfdb, wideCharacterString);
	if (pElement->u.u_keystyle.xlfdib)
	    m_free(pElement->u.u_keystyle.xlfdib, wideCharacterString);
	if (pElement->u.u_keystyle.typenam)
	    m_free(pElement->u.u_keystyle.typenam, wideCharacterString);
	if (pElement->u.u_keystyle.typenami)
	    m_free(pElement->u.u_keystyle.typenami, wideCharacterString);
	if (pElement->u.u_keystyle.typenamb)
	    m_free(pElement->u.u_keystyle.typenamb, wideCharacterString);
	if (pElement->u.u_keystyle.typenamib)
	    m_free(pElement->u.u_keystyle.typenamib, wideCharacterString);
	if (pElement->u.u_keystyle.style)
	    m_free(pElement->u.u_keystyle.style, wideCharacterString);
	if (pElement->u.u_keystyle.spacing)
	    m_free(pElement->u.u_keystyle.spacing, wideCharacterString);
	if (pElement->u.u_keystyle.weight)
	    m_free(pElement->u.u_keystyle.weight, wideCharacterString);
	if (pElement->u.u_keystyle.slant)
	    m_free(pElement->u.u_keystyle.slant, wideCharacterString);
	if (pElement->u.u_keystyle.special)
	    m_free(pElement->u.u_keystyle.special, wideCharacterString);
	break;
    case e_headstyle:
	if (pElement->u.u_headstyle.class)
	    m_free(pElement->u.u_headstyle.class, wideCharacterString);
	if (pElement->u.u_headstyle.ssi)
	    m_free(pElement->u.u_headstyle.ssi, wideCharacterString);
	if (pElement->u.u_headstyle.rlevel)
	    m_free(pElement->u.u_headstyle.rlevel, wideCharacterString);
	if (pElement->u.u_headstyle.phrase)
	    m_free(pElement->u.u_headstyle.phrase, wideCharacterString);
	if (pElement->u.u_headstyle.srch_wt)
	    m_free(pElement->u.u_headstyle.srch_wt, wideCharacterString);
	if (pElement->u.u_headstyle.orient)
	    m_free(pElement->u.u_headstyle.orient, wideCharacterString);
	if (pElement->u.u_headstyle.vorient)
	    m_free(pElement->u.u_headstyle.vorient, wideCharacterString);
	if (pElement->u.u_headstyle.placement)
	    m_free(pElement->u.u_headstyle.placement, wideCharacterString);
	if (pElement->u.u_headstyle.headw)
	    m_free(pElement->u.u_headstyle.headw, wideCharacterString);
	if (pElement->u.u_headstyle.stack)
	    m_free(pElement->u.u_headstyle.stack, wideCharacterString);
	if (pElement->u.u_headstyle.flow)
	    m_free(pElement->u.u_headstyle.flow, wideCharacterString);
	if (pElement->u.u_headstyle.pointsz)
	    m_free(pElement->u.u_headstyle.pointsz, wideCharacterString);
	if (pElement->u.u_headstyle.set_width)
	    m_free(pElement->u.u_headstyle.set_width, wideCharacterString);
	if (pElement->u.u_headstyle.color)
	    m_free(pElement->u.u_headstyle.color, wideCharacterString);
	if (pElement->u.u_headstyle.xlfd)
	    m_free(pElement->u.u_headstyle.xlfd, wideCharacterString);
	if (pElement->u.u_headstyle.xlfdi)
	    m_free(pElement->u.u_headstyle.xlfdi, wideCharacterString);
	if (pElement->u.u_headstyle.xlfdb)
	    m_free(pElement->u.u_headstyle.xlfdb, wideCharacterString);
	if (pElement->u.u_headstyle.xlfdib)
	    m_free(pElement->u.u_headstyle.xlfdib, wideCharacterString);
	if (pElement->u.u_headstyle.typenam)
	    m_free(pElement->u.u_headstyle.typenam, wideCharacterString);
	if (pElement->u.u_headstyle.typenami)
	    m_free(pElement->u.u_headstyle.typenami, wideCharacterString);
	if (pElement->u.u_headstyle.typenamb)
	    m_free(pElement->u.u_headstyle.typenamb, wideCharacterString);
	if (pElement->u.u_headstyle.typenamib)
	    m_free(pElement->u.u_headstyle.typenamib, wideCharacterString);
	if (pElement->u.u_headstyle.style)
	    m_free(pElement->u.u_headstyle.style, wideCharacterString);
	if (pElement->u.u_headstyle.spacing)
	    m_free(pElement->u.u_headstyle.spacing, wideCharacterString);
	if (pElement->u.u_headstyle.weight)
	    m_free(pElement->u.u_headstyle.weight, wideCharacterString);
	if (pElement->u.u_headstyle.slant)
	    m_free(pElement->u.u_headstyle.slant, wideCharacterString);
	if (pElement->u.u_headstyle.special)
	    m_free(pElement->u.u_headstyle.special, wideCharacterString);
	if (pElement->u.u_headstyle.l_margin)
	    m_free(pElement->u.u_headstyle.l_margin, wideCharacterString);
	if (pElement->u.u_headstyle.r_margin)
	    m_free(pElement->u.u_headstyle.r_margin, wideCharacterString);
	if (pElement->u.u_headstyle.t_margin)
	    m_free(pElement->u.u_headstyle.t_margin, wideCharacterString);
	if (pElement->u.u_headstyle.b_margin)
	    m_free(pElement->u.u_headstyle.b_margin, wideCharacterString);
	if (pElement->u.u_headstyle.border)
	    m_free(pElement->u.u_headstyle.border, wideCharacterString);
	if (pElement->u.u_headstyle.vjust)
	    m_free(pElement->u.u_headstyle.vjust, wideCharacterString);
	if (pElement->u.u_headstyle.justify)
	    m_free(pElement->u.u_headstyle.justify, wideCharacterString);
	break;
    case e_formstyle:
	if (pElement->u.u_formstyle.class)
	    m_free(pElement->u.u_formstyle.class, wideCharacterString);
	if (pElement->u.u_formstyle.ssi)
	    m_free(pElement->u.u_formstyle.ssi, wideCharacterString);
	if (pElement->u.u_formstyle.rlevel)
	    m_free(pElement->u.u_formstyle.rlevel, wideCharacterString);
	if (pElement->u.u_formstyle.phrase)
	    m_free(pElement->u.u_formstyle.phrase, wideCharacterString);
	if (pElement->u.u_formstyle.srch_wt)
	    m_free(pElement->u.u_formstyle.srch_wt, wideCharacterString);
	if (pElement->u.u_formstyle.colw)
	    m_free(pElement->u.u_formstyle.colw, wideCharacterString);
	if (pElement->u.u_formstyle.colj)
	    m_free(pElement->u.u_formstyle.colj, wideCharacterString);
	if (pElement->u.u_formstyle.pointsz)
	    m_free(pElement->u.u_formstyle.pointsz, wideCharacterString);
	if (pElement->u.u_formstyle.set_width)
	    m_free(pElement->u.u_formstyle.set_width, wideCharacterString);
	if (pElement->u.u_formstyle.color)
	    m_free(pElement->u.u_formstyle.color, wideCharacterString);
	if (pElement->u.u_formstyle.xlfd)
	    m_free(pElement->u.u_formstyle.xlfd, wideCharacterString);
	if (pElement->u.u_formstyle.xlfdi)
	    m_free(pElement->u.u_formstyle.xlfdi, wideCharacterString);
	if (pElement->u.u_formstyle.xlfdb)
	    m_free(pElement->u.u_formstyle.xlfdb, wideCharacterString);
	if (pElement->u.u_formstyle.xlfdib)
	    m_free(pElement->u.u_formstyle.xlfdib, wideCharacterString);
	if (pElement->u.u_formstyle.typenam)
	    m_free(pElement->u.u_formstyle.typenam, wideCharacterString);
	if (pElement->u.u_formstyle.typenami)
	    m_free(pElement->u.u_formstyle.typenami, wideCharacterString);
	if (pElement->u.u_formstyle.typenamb)
	    m_free(pElement->u.u_formstyle.typenamb, wideCharacterString);
	if (pElement->u.u_formstyle.typenamib)
	    m_free(pElement->u.u_formstyle.typenamib, wideCharacterString);
	if (pElement->u.u_formstyle.style)
	    m_free(pElement->u.u_formstyle.style, wideCharacterString);
	if (pElement->u.u_formstyle.spacing)
	    m_free(pElement->u.u_formstyle.spacing, wideCharacterString);
	if (pElement->u.u_formstyle.weight)
	    m_free(pElement->u.u_formstyle.weight, wideCharacterString);
	if (pElement->u.u_formstyle.slant)
	    m_free(pElement->u.u_formstyle.slant, wideCharacterString);
	if (pElement->u.u_formstyle.special)
	    m_free(pElement->u.u_formstyle.special, wideCharacterString);
	if (pElement->u.u_formstyle.l_margin)
	    m_free(pElement->u.u_formstyle.l_margin, wideCharacterString);
	if (pElement->u.u_formstyle.r_margin)
	    m_free(pElement->u.u_formstyle.r_margin, wideCharacterString);
	if (pElement->u.u_formstyle.t_margin)
	    m_free(pElement->u.u_formstyle.t_margin, wideCharacterString);
	if (pElement->u.u_formstyle.b_margin)
	    m_free(pElement->u.u_formstyle.b_margin, wideCharacterString);
	if (pElement->u.u_formstyle.border)
	    m_free(pElement->u.u_formstyle.border, wideCharacterString);
	if (pElement->u.u_formstyle.vjust)
	    m_free(pElement->u.u_formstyle.vjust, wideCharacterString);
	break;
    case e_frmtstyle:
	if (pElement->u.u_frmtstyle.class)
	    m_free(pElement->u.u_frmtstyle.class, wideCharacterString);
	if (pElement->u.u_frmtstyle.ssi)
	    m_free(pElement->u.u_frmtstyle.ssi, wideCharacterString);
	if (pElement->u.u_frmtstyle.rlevel)
	    m_free(pElement->u.u_frmtstyle.rlevel, wideCharacterString);
	if (pElement->u.u_frmtstyle.phrase)
	    m_free(pElement->u.u_frmtstyle.phrase, wideCharacterString);
	if (pElement->u.u_frmtstyle.f_margin)
	    m_free(pElement->u.u_frmtstyle.f_margin, wideCharacterString);
	if (pElement->u.u_frmtstyle.srch_wt)
	    m_free(pElement->u.u_frmtstyle.srch_wt, wideCharacterString);
	if (pElement->u.u_frmtstyle.pointsz)
	    m_free(pElement->u.u_frmtstyle.pointsz, wideCharacterString);
	if (pElement->u.u_frmtstyle.set_width)
	    m_free(pElement->u.u_frmtstyle.set_width, wideCharacterString);
	if (pElement->u.u_frmtstyle.color)
	    m_free(pElement->u.u_frmtstyle.color, wideCharacterString);
	if (pElement->u.u_frmtstyle.xlfd)
	    m_free(pElement->u.u_frmtstyle.xlfd, wideCharacterString);
	if (pElement->u.u_frmtstyle.xlfdi)
	    m_free(pElement->u.u_frmtstyle.xlfdi, wideCharacterString);
	if (pElement->u.u_frmtstyle.xlfdb)
	    m_free(pElement->u.u_frmtstyle.xlfdb, wideCharacterString);
	if (pElement->u.u_frmtstyle.xlfdib)
	    m_free(pElement->u.u_frmtstyle.xlfdib, wideCharacterString);
	if (pElement->u.u_frmtstyle.typenam)
	    m_free(pElement->u.u_frmtstyle.typenam, wideCharacterString);
	if (pElement->u.u_frmtstyle.typenami)
	    m_free(pElement->u.u_frmtstyle.typenami, wideCharacterString);
	if (pElement->u.u_frmtstyle.typenamb)
	    m_free(pElement->u.u_frmtstyle.typenamb, wideCharacterString);
	if (pElement->u.u_frmtstyle.typenamib)
	    m_free(pElement->u.u_frmtstyle.typenamib, wideCharacterString);
	if (pElement->u.u_frmtstyle.style)
	    m_free(pElement->u.u_frmtstyle.style, wideCharacterString);
	if (pElement->u.u_frmtstyle.spacing)
	    m_free(pElement->u.u_frmtstyle.spacing, wideCharacterString);
	if (pElement->u.u_frmtstyle.weight)
	    m_free(pElement->u.u_frmtstyle.weight, wideCharacterString);
	if (pElement->u.u_frmtstyle.slant)
	    m_free(pElement->u.u_frmtstyle.slant, wideCharacterString);
	if (pElement->u.u_frmtstyle.special)
	    m_free(pElement->u.u_frmtstyle.special, wideCharacterString);
	if (pElement->u.u_frmtstyle.l_margin)
	    m_free(pElement->u.u_frmtstyle.l_margin, wideCharacterString);
	if (pElement->u.u_frmtstyle.r_margin)
	    m_free(pElement->u.u_frmtstyle.r_margin, wideCharacterString);
	if (pElement->u.u_frmtstyle.t_margin)
	    m_free(pElement->u.u_frmtstyle.t_margin, wideCharacterString);
	if (pElement->u.u_frmtstyle.b_margin)
	    m_free(pElement->u.u_frmtstyle.b_margin, wideCharacterString);
	if (pElement->u.u_frmtstyle.border)
	    m_free(pElement->u.u_frmtstyle.border, wideCharacterString);
	if (pElement->u.u_frmtstyle.vjust)
	    m_free(pElement->u.u_frmtstyle.vjust, wideCharacterString);
	if (pElement->u.u_frmtstyle.justify)
	    m_free(pElement->u.u_frmtstyle.justify, wideCharacterString);
	break;
    case e_grphstyle:
	if (pElement->u.u_grphstyle.class)
	    m_free(pElement->u.u_grphstyle.class, wideCharacterString);
	if (pElement->u.u_grphstyle.ssi)
	    m_free(pElement->u.u_grphstyle.ssi, wideCharacterString);
	if (pElement->u.u_grphstyle.rlevel)
	    m_free(pElement->u.u_grphstyle.rlevel, wideCharacterString);
	if (pElement->u.u_grphstyle.phrase)
	    m_free(pElement->u.u_grphstyle.phrase, wideCharacterString);
	if (pElement->u.u_grphstyle.l_margin)
	    m_free(pElement->u.u_grphstyle.l_margin, wideCharacterString);
	if (pElement->u.u_grphstyle.r_margin)
	    m_free(pElement->u.u_grphstyle.r_margin, wideCharacterString);
	if (pElement->u.u_grphstyle.t_margin)
	    m_free(pElement->u.u_grphstyle.t_margin, wideCharacterString);
	if (pElement->u.u_grphstyle.b_margin)
	    m_free(pElement->u.u_grphstyle.b_margin, wideCharacterString);
	if (pElement->u.u_grphstyle.border)
	    m_free(pElement->u.u_grphstyle.border, wideCharacterString);
	if (pElement->u.u_grphstyle.vjust)
	    m_free(pElement->u.u_grphstyle.vjust, wideCharacterString);
	if (pElement->u.u_grphstyle.justify)
	    m_free(pElement->u.u_grphstyle.justify, wideCharacterString);
	break;
    default:
	fprintf(stderr,
		"internal error - unrecognized element type in DeleteElement");
	if (m_errfile && (m_errfile != stderr))
	    fprintf(m_errfile,
		"internal error - unrecognized element type in DeleteElement");
	exit(xxx);
    }
mb_free((char **) &pElement);
}


static ElementPtr *currentElementStack;
static int currentElementStackSize = 0;
static int currentElementStackTop = -1;

void PushCurrentElement(void)
{
currentElementStackTop++;
if (currentElementStackSize == 0)
    {
    currentElementStack =
	(ElementPtr *) mb_malloc(100 * sizeof(ElementPtr *));
    currentElementStackSize = 100;
    }
else if (currentElementStackTop == currentElementStackSize)
    {
    currentElementStackSize += 100;
    currentElementStack =
	(ElementPtr *) mb_realloc((char *) currentElementStack,
			      currentElementStackSize * sizeof(ElementPtr *));
    }
currentElementStack[currentElementStackTop] = pCurrentElement;
}


static ElementPtr *thisElementStack;
static int thisElementStackSize = 0;
static int thisElementStackTop = -1;

static void PushThisElement(ElementPtr pThis)
{
thisElementStackTop++;
if (thisElementStackSize == 0)
    {
    thisElementStack =
	(ElementPtr *) mb_malloc(100 * sizeof(ElementPtr *));
    thisElementStackSize = 100;
    }
else if (thisElementStackTop == thisElementStackSize)
    {
    thisElementStackSize += 100;
    thisElementStack =
	(ElementPtr *) mb_realloc((char *) thisElementStack,
			      thisElementStackSize * sizeof(ElementPtr *));
    }
thisElementStack[thisElementStackTop] = pThis;
}


void PopCurrentElement(void)
{
pCurrentElement = currentElementStack[currentElementStackTop];
if (currentElementStackTop < 0)
    {
    fprintf(stderr, "internal error - stack underflow in PopCurrentElement");
    if (m_errfile && (m_errfile != stderr))
	fprintf(m_errfile,
		"internal error - stack underflow in PopCurrentElement");
    exit(xxx);
    }
else
    --currentElementStackTop;
}


static ElementPtr PopThisElement(void)
{
ElementPtr pThisElement;

pThisElement = thisElementStack[thisElementStackTop];
if (thisElementStackTop < 0)
    {
    fprintf(stderr, "internal error - stack underflow in PopThisElement");
    if (m_errfile && (m_errfile != stderr))
	fprintf(m_errfile,
		"internal error - stack underflow in PopThisElement");
    exit(xxx);
    }
else
    --thisElementStackTop;
return pThisElement;
}


/*
 * A routine to remove occurrences of <BLOCK>\n</BLOCK>, if any, from
 * the .sdl file.  These occurrences are an artifact of translation
 * and seem to be easier to get rid of here than in the first pass.
*/
static void RemoveSuperfluousBlocks(ElementPtr *ppBlockOrForm)
{
ElementPtr  pElement, pFdata;
ElementPtr *ppBack;

if (!(pElement = *ppBlockOrForm))
    return;

ppBack = ppBlockOrForm;
while (pElement)
    {
    if (pElement->type == e_form)
	{
	if (pFdata = pElement->u.u_form.pFdata)
	    RemoveSuperfluousBlocks(&(pFdata->u.u_fdata.pBlockOrForm));
	}
    else if (!pElement->u.u_block.level    &&
	     !pElement->u.u_block.version  &&
	     !pElement->u.u_block.class    &&
	     !pElement->u.u_block.language &&
	     !pElement->u.u_block.charset  &&
	     !pElement->u.u_block.length   &&
	     !pElement->u.u_block.app      &&
	     !pElement->u.u_block.timing   &&
	     !pElement->u.u_block.ssi      &&
	     !pElement->u.u_block.pHead    &&
	     !pElement->u.u_block.pCpOrP)
	{
	*ppBack = pElement->pNext;
	DeleteElement(pElement);
	pElement = *ppBack;
	continue;
	}
    ppBack   = &(pElement->pNext);
    pElement = pElement->pNext;
    }
}


static void ReplaceFormWithBlock(ElementPtr *ppBlockOrForm)
{
ElementPtr  pElement, pFstyle, pFrowvec, pFdata, pSubElement;
ElementPtr *ppBack;
M_WCHAR    *pId, *wc_sdlReservedName, *wc_quote1;

if (!(pElement = *ppBlockOrForm))
    return;

wc_sdlReservedName = MakeWideCharString(sdlReservedName);
wc_quote1          = MakeWideCharString("\"1\"");

ppBack = ppBlockOrForm;
while (pElement)
    {
    if (pElement->type != e_form)
	{
	ppBack   = &(pElement->pNext);
	pElement = pElement->pNext;
	continue;
	}
    if (!(pFdata = pElement->u.u_form.pFdata))
	{ /* illegal <form> */
	ppBack   = &(pElement->pNext);
	pElement = pElement->pNext;
	continue;
	}
    ReplaceFormWithBlock(&(pFdata->u.u_fdata.pBlockOrForm));
    if ((pFstyle  = pElement->u.u_form.pFstyle)   &&
	(pFrowvec = pFstyle->u.u_fstyle.pFrowvec) &&
	!pFrowvec->pNext                          &&
        (!pFstyle->u.u_fstyle.ncols || !w_strcmp(pFstyle->u.u_fstyle.ncols,
						 wc_quote1)))
	{ /* found a <form> containing only one element */
	if (((pSubElement = pFdata->u.u_fdata.pBlockOrForm)->type == e_form))
	    {
	    if (!pSubElement->u.u_form.level    &&
		!pSubElement->u.u_form.version  &&
		!pSubElement->u.u_form.class    &&
		!pSubElement->u.u_form.language &&
		!pSubElement->u.u_form.charset  &&
		!pSubElement->u.u_form.length   &&
		!pSubElement->u.u_form.app      &&
		!pSubElement->u.u_form.ssi)
		{
		if ((pId = pSubElement->u.u_form.id) &&
		    w_strncmp(pId, wc_sdlReservedName, SDLNAMESIZ-1))
		    { /* user supplied id */
		    ppBack   = &(pElement->pNext);
		    pElement = pElement->pNext;
		    continue;
		    }
		if (pElement->u.u_form.pHead)
		    {
		    if (pSubElement->u.u_form.pHead)
			{
			ppBack   = &(pElement->pNext);
			pElement = pElement->pNext;
			continue;
			}
		    else
			{
			pSubElement->u.u_form.pHead =
						pElement->u.u_form.pHead;
			pElement->u.u_form.pHead = NULL;
			}
		    }
		if (pId)
		    m_free(pId, wideCharacterString);
		*ppBack = pSubElement;
		pSubElement->pNext             = pElement->pNext;
		pSubElement->u.u_form.id       = pElement->u.u_form.id;
		pSubElement->u.u_form.level    = pElement->u.u_form.level;
		pSubElement->u.u_form.version  = pElement->u.u_form.version;
		pSubElement->u.u_form.class    = pElement->u.u_form.class;
		pSubElement->u.u_form.language = pElement->u.u_form.language;
		pSubElement->u.u_form.charset  = pElement->u.u_form.charset;
		pSubElement->u.u_form.length   = pElement->u.u_form.length;
		pSubElement->u.u_form.app      = pElement->u.u_form.app;
		pSubElement->u.u_form.ssi      = pElement->u.u_form.ssi;
		pElement->u.u_form.id          = NULL;
		pElement->u.u_form.level       = NULL;
		pElement->u.u_form.version     = NULL;
		pElement->u.u_form.class       = NULL;
		pElement->u.u_form.language    = NULL;
		pElement->u.u_form.charset     = NULL;
		pElement->u.u_form.length      = NULL;
		pElement->u.u_form.app         = NULL;
		pElement->u.u_form.ssi         = NULL;
		DeleteElement(pFstyle);
		DeleteElement(pFrowvec);
		DeleteElement(pFdata);
		}
	    else
		{
		ppBack   = &(pElement->pNext);
		pElement = pElement->pNext;
		continue;
		}
	    }
	else
	    {
	    if (!pSubElement->u.u_block.level    &&
		!pSubElement->u.u_block.version  &&
		!pSubElement->u.u_block.class    &&
		!pSubElement->u.u_block.language &&
		!pSubElement->u.u_block.charset  &&
		!pSubElement->u.u_block.length   &&
		!pSubElement->u.u_block.app      &&
		!pSubElement->u.u_block.timing   &&
		!pSubElement->u.u_block.ssi)
		{
		if ((pId = pSubElement->u.u_block.id) &&
		    w_strncmp(pId, wc_sdlReservedName, SDLNAMESIZ-1))
		    { /* user supplied id */
		    ppBack   = &(pElement->pNext);
		    pElement = pElement->pNext;
		    continue;
		    }
		if (pElement->u.u_form.pHead)
		    {
		    if (pSubElement->u.u_block.pHead)
			{
			ppBack   = &(pElement->pNext);
			pElement = pElement->pNext;
			continue;
			}
		    else
			{
			pSubElement->u.u_block.pHead =
						pElement->u.u_form.pHead;
			pElement->u.u_form.pHead = NULL;
			}
		    }
		if (pId)
		    m_free(pId, wideCharacterString);
		*ppBack = pSubElement;
		pSubElement->pNext              = pElement->pNext;
		pSubElement->u.u_block.id       = pElement->u.u_form.id;
		pSubElement->u.u_block.level    = pElement->u.u_form.level;
		pSubElement->u.u_block.version  = pElement->u.u_form.version;
		pSubElement->u.u_block.class    = pElement->u.u_form.class;
		pSubElement->u.u_block.language = pElement->u.u_form.language;
		pSubElement->u.u_block.charset  = pElement->u.u_form.charset;
		pSubElement->u.u_block.length   = pElement->u.u_form.length;
		pSubElement->u.u_block.app      = pElement->u.u_form.app;
		pSubElement->u.u_block.timing   = NULL;
		pSubElement->u.u_block.ssi      = pElement->u.u_form.ssi;
		pElement->u.u_form.id           = NULL;
		pElement->u.u_form.level        = NULL;
		pElement->u.u_form.version      = NULL;
		pElement->u.u_form.class        = NULL;
		pElement->u.u_form.language     = NULL;
		pElement->u.u_form.charset      = NULL;
		pElement->u.u_form.length       = NULL;
		pElement->u.u_form.app          = NULL;
		pElement->u.u_form.ssi          = NULL;
		DeleteElement(pFstyle);
		DeleteElement(pFrowvec);
		DeleteElement(pFdata);
		}
	    else
		{
		ppBack   = &(pElement->pNext);
		pElement = pElement->pNext;
		continue;
		}
	    }
	ppBack      = &(pSubElement->pNext);
	pSubElement = pElement->pNext;
	*ppBack     = pSubElement;
	DeleteElement(pElement);
	pElement = pSubElement;
	continue;
	}
    ppBack   = &(pElement->pNext);
    pElement = pElement->pNext;
    continue;
    }
m_free(wc_sdlReservedName, wideCharacterString);
m_free(wc_quote1, wideCharacterString);
}


static void OptimizeSDL(ElementPtr *ppBlockOrForm)
{
ReplaceFormWithBlock(ppBlockOrForm);
RemoveSuperfluousBlocks(ppBlockOrForm);
}


/* build a linked list of the LOIDS, we'll need to postprocess it to
 * update the offsets when adding the INDEX and LOIDS; also when
 * compressing the VSTRUCT
*/
static void AddToLOIDS(char     *pElementName,
                       M_WCHAR  *pwcId,
                       M_WCHAR  *pwcSSI,
                       M_WCHAR  *pwcLevel,
		       long int  virpageOffset)
{
LoidsPtr pNewId;

if (!pwcId) return;

nIds++;

virpageOffset += vstructSize;

pNewId         = (LoidsPtr) mb_malloc(sizeof(LoidsRec));

pNewId->type   = mb_malloc(strlen(pElementName)+1);
strcpy(pNewId->type, pElementName);
pNewId->rid    = MakeMByteString(pwcId);
if (pwcSSI)
    pNewId->rssi   = MakeMByteString(pwcSSI);
else
    pNewId->rssi   = NULL;
if (pwcLevel)
    pNewId->rlevel = MakeMByteString(pwcLevel);
else
    pNewId->rlevel = NULL;
pNewId->offset     = virpageOffset;
pNewId->next       = NULL;

pLoidsEnd->next = pNewId;
pLoidsEnd = pNewId;
}


/* compress or decompress a file; return the size after (de)compression
*/
static int Compress(char *fileName, char *zFileName, LOGICAL doCompress)
{
       char cmd[BUFSIZ];
       int  errStatus;
       int  errCode;
static char sysStr[] = "system()";

if (doCompress)
    sprintf(cmd, "compress -f < %s > %s", fileName, zFileName);
else
    sprintf(cmd, "compress -d < %s > %s", zFileName, fileName);
errStatus = system(cmd);
if (errStatus)
    {
    fprintf(stderr, "%s: error executing \"%s\"\n", progName, cmd);
    if (m_errfile && (m_errfile != stderr))
	fprintf(m_errfile, "%s: error executing \"%s\"\n", progName, cmd);
    if (errStatus == -1)
	{
	perror(sysStr);
	if (m_errfile && (m_errfile != stderr))
	    fprintf(m_errfile, "%s: %s", sysStr, strerror(errno));
	}
    else
	{
	if (WIFEXITED(errStatus))
	    {
	    if ((errCode = WEXITSTATUS(errStatus)) == 127)
		{
		fprintf(stderr,
			"%s: %s\n",
			sysStr,
			"error executing command language interpreter");
		if (m_errfile && (m_errfile != stderr))
		    fprintf(m_errfile,
			    "%s: %s\n",
			    sysStr,
			    "error executing command language interpreter");
		}
	    else
		{
		fprintf(stderr, "%s: exited with status %d\n", cmd, errCode);
		if (m_errfile && (m_errfile != stderr))
		    fprintf(m_errfile,
			    "%s: exited with status %d\n",
			    cmd,
			    errCode);
		}
	    }
	else if (WIFSIGNALED(errStatus))
	    {
	    errCode = WTERMSIG(errStatus);
	    fprintf(stderr, "%s: exited due to signal %d\n", cmd, errCode);
	    if (m_errfile && (m_errfile != stderr))
		fprintf(m_errfile,
			"%s: exited due to signal %d\n",
			cmd,
			errCode);
	    if (WCOREDUMP(errStatus))
		{
		fputs("core dumped\n", stderr);
		if (m_errfile && (m_errfile != stderr))
		    fputs("core dumped\n", m_errfile);
		}
	    }
	else if (WIFSTOPPED(errStatus))
	    {
	    errCode = WSTOPSIG(errStatus);
	    fprintf(stderr, "%s: stopped due to signal %d\n", cmd, errCode);
	    if (m_errfile && (m_errfile != stderr))
		fprintf(m_errfile,
			"%s: stopped due to signal %d\n",
			cmd,
			errCode);
	    }
	}
    exit(1);
    }
if (doCompress)
    return FileSize(zFileName, xxx);
else
    return FileSize(fileName, xxx);
}


/* decompress an SDL file */
static void DecompressSDL(void)
{
int           outFd, zFd;
FILE         *inFile;
int           length, delta;
unsigned char buffer[BUFSIZ];

inFile = FopenFile(inFileName, O_RDONLY, xxx);
outFd  = OpenFile(tempFileName, O_WRONLY, xxx);

length = FreadFile(inFile, inFileName, (char *) buffer, 4, xxx);
if (length < 4)
    {
    fprintf(stderr, "%s: premature end of %s\n", progName, inFileName);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: premature end of %s\n",
		progName,
		inFileName);
	}
    exit(xxx);
    }

if (*buffer) /* file isn't compressed */
    {
    length += FreadFile(inFile, inFileName, (char *) (buffer+4), BUFSIZ-4, xxx);
    do  {
	WriteFile(outFd, tempFileName, (char *) buffer, length, xxx);
	length = FreadFile(inFile, inFileName, (char *) buffer, BUFSIZ, xxx);
	}
    while (length > 0);
    FcloseFile(inFile, inFileName, xxx);
    CloseFile(outFd, tempFileName, xxx);
    return;
    }

do  {
    delta  = buffer[1];
    delta *= 256;
    delta += buffer[2];
    delta *= 256;
    delta += buffer[3];

    unlink(compZFileName);
    zFd = OpenFile(compZFileName, O_WRONLY, xxx);
    while (length = FreadFile(inFile,
			      inFileName,
			      (char *) buffer,
			      MIN(delta, BUFSIZ),
			      xxx))
	{
	WriteFile(zFd, compZFileName, (char *) buffer, length, xxx);
	delta -= length;
	}
    if (delta != 0)
	{
	fprintf(stderr, "%s: premature end of %s\n", progName, inFileName);
	if (m_errfile && (m_errfile != stderr))
	    {
	    fprintf(m_errfile,
		    "%s: premature end of %s\n",
		    progName,
		    inFileName);
	    }
	exit(xxx);
	}
    CloseFile(zFd, compZFileName, xxx);
    delta = Compress(compFileName, compZFileName, FALSE);
    zFd = OpenFile(compFileName, O_RDONLY, xxx);
    while (length = ReadFile(zFd,
			     compFileName,
			     (char *) buffer,
			     MIN(delta, BUFSIZ),
			     xxx))
	{
	WriteFile(outFd, tempFileName, (char *) buffer, length, xxx);
	delta -= length;
	}
    if (delta != 0)
	{
	fprintf(stderr, "%s: premature end of %s\n", progName, compFileName);
	if (m_errfile && (m_errfile != stderr))
	    {
	    fprintf(m_errfile,
		    "%s: premature end of %s\n",
		    progName,
		    compFileName);
	    }
	exit(xxx);
	}
    CloseFile(zFd, compFileName, xxx);
    while ((delta = FreadFile(inFile,
			     inFileName,
			     (char *) buffer,
			     1,
			     xxx)) && (*buffer == 0xff));
    if (delta)
	{
	if (*buffer == '<') /* we're at the end, "</SDLDOC>\n" */
	    {
	    delta += FreadFile(inFile,
			       inFileName,
			       (char *) (buffer + 1),
			       9,
			       xxx);
	    if (delta != 10)
		delta = 1; /* force a premature end of document */
	    else
		{
		WriteFile(outFd, tempFileName, (char *) buffer, delta, xxx);
		delta = 0;
		}
	    }
	else
	    delta += FreadFile(inFile,
			       inFileName,
			       (char *) (buffer + 1),
			       3,
			       xxx);
	}
    }
while (delta >= 4);
if (delta != 0)
    {
    fprintf(stderr, "%s: premature end of %s\n", progName, inFileName);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: premature end of %s\n",
		progName,
		inFileName);
	}
    exit(xxx);
    }
FcloseFile(inFile, inFileName, xxx);
CloseFile(outFd, tempFileName, xxx);
unlink(compFileName);
unlink(compZFileName);
}


/* A function too lookup a TOSS element and mark it as used if the
 * lookup succeeds.
*/
static void MarkUsedStyle(M_WCHAR *level, M_WCHAR *class, M_WCHAR *ssi)
{
ElementPtr  pThis;
M_WCHAR    *rlevel = NULL, *rclass = NULL, *rssi = NULL;
LOGICAL    *pBeenUsed;

pThis = pTossChain;
while (pThis)
    {
    switch (pThis->type)
	{
	case e_keystyle:
	    rlevel    = pThis->u.u_keystyle.rlevel;
	    rclass    = pThis->u.u_keystyle.class;
	    rssi      = pThis->u.u_keystyle.ssi;
	    pBeenUsed = &pThis->u.u_keystyle.beenUsed;
	    break;
	case e_headstyle:
	    rlevel    = pThis->u.u_headstyle.rlevel;
	    rclass    = pThis->u.u_headstyle.class;
	    rssi      = pThis->u.u_headstyle.ssi;
	    pBeenUsed = &pThis->u.u_headstyle.beenUsed;
	    break;
	case e_formstyle:
	    rlevel    = pThis->u.u_formstyle.rlevel;
	    rclass    = pThis->u.u_formstyle.class;
	    rssi      = pThis->u.u_formstyle.ssi;
	    pBeenUsed = &pThis->u.u_formstyle.beenUsed;
	    break;
	case e_frmtstyle:
	    rlevel    = pThis->u.u_frmtstyle.rlevel;
	    rclass    = pThis->u.u_frmtstyle.class;
	    rssi      = pThis->u.u_frmtstyle.ssi;
	    pBeenUsed = &pThis->u.u_frmtstyle.beenUsed;
	    break;
	case e_grphstyle:
	    rlevel    = pThis->u.u_grphstyle.rlevel;
	    rclass    = pThis->u.u_grphstyle.class;
	    rssi      = pThis->u.u_grphstyle.ssi;
	    pBeenUsed = &pThis->u.u_grphstyle.beenUsed;
	    break;
	}
    if ((!rlevel || (w_strcmp(rlevel, level) == 0)) &&
	(!rssi   || (w_strcmp(rssi, ssi) == 0))     &&
	(w_strcmp(rclass, class) == 0))
	    {
	    if (*pBeenUsed == FALSE)
		{
		nStyles++;
		*pBeenUsed = TRUE;
		pThis->beenEmitted = FALSE;
		}
	    return;
	    }
    pThis = pThis->pNext;
    }
}


/* Look for "rid" in the "pSnb" list.  Return the element if found,
 * NULL otherwise.
*/
static ElementPtr LookupInSnb(ElementPtr pSnb, M_WCHAR *rid)
{
ElementPtr   pThis;
M_WCHAR    **pId;
static char errMess[] =
	    "internal error - unrecognized element type in LookupInSnb";

if (!pSnb) return NULL;

pThis = pSnb->u.u_snb.pSystemNotations;

while (pThis)
    {
    switch (pThis->type)
	{
	case e_graphic:
	    pId = &pThis->u.u_graphic.id;
	    break;
	case e_text:
	    pId = &pThis->u.u_text.id;
	    break;
	case e_audio:
	    pId = &pThis->u.u_audio.id;
	    break;
	case e_video:
	    pId = &pThis->u.u_video.id;
	    break;
	case e_animate:
	    pId = &pThis->u.u_animate.id;
	    break;
	case e_script:
	    pId = &pThis->u.u_script.id;
	    break;
	case e_crossdoc:
	    pId = &pThis->u.u_crossdoc.id;
	    break;
	case e_man_page:
	    pId = &pThis->u.u_man_page.id;
	    break;
	case e_textfile:
	    pId = &pThis->u.u_textfile.id;
	    break;
	case e_sys_cmd:
	    pId = &pThis->u.u_sys_cmd.id;
	    break;
	case e_callback:
	    pId = &pThis->u.u_callback.id;
	    break;
	case e_switch:
	    pId = &pThis->u.u_switch.id;
	    break;
	default:
          fprintf(stderr, "%s\n", errMess);
	    if (m_errfile && (m_errfile != stderr))
		fprintf(m_errfile, "%s\n", errMess);
	    exit(xxx);
	}
    if (w_strcmp(rid, *pId) == 0) return pThis;
    pThis = pThis->pNext;
    }
return NULL;
}


/* Clear the tossChecked field in the nodes of a sub-tree starting at
 * (and including) the element passed in. The assumption is that all
 * nodes in the sub-tree are either marked or all clear.  Once a clear
 * node is found, the tree is not descended further.
*/
static void ClearTree(ElementPtr pRoot)
{
ElementPtr  pThis;
static char errMess[] =
	    "internal error - unrecognized element type in ClearTree";

pThis = pRoot;

while (pThis)
    {
    switch (pThis->type)
	{
	case e_sdldoc:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_sdldoc.pHead &&
		pThis->u.u_sdldoc.pHead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_sdldoc.pHead;
		break;
		}
	    if (pThis->u.u_sdldoc.pSnb &&
	        pThis->u.u_sdldoc.pSnb->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_sdldoc.pSnb;
		break;
		}
	    pThis = NULL;
	    break;
	case e_virpage:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_virpage.pHead &&
		pThis->u.u_virpage.pHead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pHead;
		break;
		}
	    if (pThis->u.u_virpage.pSnb &&
	        pThis->u.u_virpage.pSnb->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pSnb;
		break;
		}
	    if (pThis->u.u_virpage.pBlockOrForm &&
	        pThis->u.u_virpage.pBlockOrForm->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pBlockOrForm;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = NULL;
	    break;
	case e_snb:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_snb.pHead &&
	        pThis->u.u_snb.pHead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pHead;
		break;
		}
	    if (pThis->u.u_snb.pSystemNotations)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_snb.pSystemNotations;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_block:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_block.pHead &&
	        pThis->u.u_block.pHead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_block.pHead;
		break;
		}
	    if (pThis->u.u_block.pCpOrP &&
	        pThis->u.u_block.pCpOrP->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_block.pCpOrP;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_form:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_form.pHead &&
	        pThis->u.u_form.pHead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_form.pHead;
		break;
		}
	    if (pThis->u.u_form.pFdata &&
	        pThis->u.u_form.pFdata->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_form.pFdata;
		break;
		}
	    if (pThis->u.u_form.pFstyle &&
	        pThis->u.u_form.pFstyle->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_form.pFstyle;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_fstyle:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_fstyle.pFrowvec &&
	        pThis->u.u_fstyle.pFrowvec->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_fstyle.pFrowvec;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_frowvec:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_fdata:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_fdata.pBlockOrForm &&
	        pThis->u.u_fdata.pBlockOrForm->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_fdata.pBlockOrForm;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_p:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_p.pHead &&
	        pThis->u.u_p.pHead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_p.pHead;
		break;
		}
	    if (pThis->u.u_p.pAtomicOrPCDATA &&
	        pThis->u.u_p.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_p.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_cp:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_cp.pCDATA &&
	        pThis->u.u_cp.pCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_cp.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_head:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_head.pAtomicOrPCDATA &&
	        pThis->u.u_head.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_head.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->u.u_head.pSubhead &&
	        pThis->u.u_head.pSubhead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_head.pSubhead;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_subhead:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_subhead.pAtomicOrPCDATA &&
	        pThis->u.u_subhead.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_subhead.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_anchor:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_link:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_link.pAtomicOrPCDATA &&
	        pThis->u.u_link.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_link.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_snref:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_snref.pRefitem &&
	        pThis->u.u_snref.pRefitem->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_snref.pRefitem;
		break;
		}
	    if (pThis->u.u_snref.pAlttext &&
	        pThis->u.u_snref.pAlttext->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_snref.pAlttext;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_refitem:
	    pThis->tossChecked = FALSE;
	    pThis->u.u_refitem.pReferencedItem = NULL;
	    if (pThis->u.u_refitem.pHead &&
	        pThis->u.u_refitem.pHead->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_refitem.pHead;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_alttext:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_alttext.pCDATA &&
	        pThis->u.u_alttext.pCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_alttext.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_rev:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_rev.pAtomicOrPCDATA &&
	        pThis->u.u_rev.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_rev.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_key:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_key.pAtomicOrPCDATA &&
	        pThis->u.u_key.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_key.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_sphrase:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_sphrase.pSpcOrPCDATA &&
	        pThis->u.u_sphrase.pSpcOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_sphrase.pSpcOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_if:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_if.pCond &&
	        pThis->u.u_if.pCond->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_if.pCond;
		break;
		}
	    if (pThis->u.u_if.pThen &&
	        pThis->u.u_if.pThen->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_if.pThen;
		break;
		}
	    if (pThis->u.u_if.pElse &&
	        pThis->u.u_if.pElse->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_if.pElse;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_cond:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_cond.pCDATA &&
	        pThis->u.u_cond.pCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_cond.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_then:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_then.pAtomicOrPCDATA &&
	        pThis->u.u_then.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_then.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_else:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_else.pAtomicOrPCDATA &&
	        pThis->u.u_else.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_else.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_spc:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_graphic:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_text:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_text.pCpOrP &&
	        pThis->u.u_text.pCpOrP->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_text.pCpOrP;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_audio:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_video:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_animate:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_script:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_script.pCDATA &&
	        pThis->u.u_script.pCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_script.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_crossdoc:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_man_page:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_textfile:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_sys_cmd:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_callback:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_switch:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_switch.pCDATA &&
	        pThis->u.u_switch.pCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_switch.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_cdata:
	    pThis->tossChecked = FALSE;
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_entry:
	    pThis->tossChecked = FALSE;
	    if (pThis->u.u_entry.pSimpleOrPCDATA &&
	        pThis->u.u_entry.pSimpleOrPCDATA->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_entry.pSimpleOrPCDATA;
		break;
		}
	    if (pThis->u.u_entry.pEntry &&
	        pThis->u.u_entry.pEntry->tossChecked)
		{
		PushThisElement(pThis);
		pThis = pThis->u.u_entry.pEntry;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis = PopThisElement();
	    break;
	case e_phrase:
	case e_keystyle:
	case e_headstyle:
	case e_formstyle:
	case e_frmtstyle:
	case e_grphstyle:
	    pThis = pThis->pNext;
	    break;
	default:
	    fprintf(stderr, "%s\n", errMess);
	    if (m_errfile && (m_errfile != stderr))
		fprintf(m_errfile, "%s\n", errMess);
	    exit(xxx);
	}
    }
}


/* A function to mark all elements in the TOSS that have been used
 * by the document up to this point.
*/
static void MarkUsedTOSS(ElementPtr pRoot)
{
ElementPtr pParent, pSnb, pItem, pThis;
static char errMess[] =
	    "internal error - unrecognized element type in MarkUsedTOSS";

pThis   = pRoot;
pParent = NULL;

while (pThis)
    {
    switch (pThis->type)
	{
	case e_sdldoc:
	    if (!pThis->tossChecked)
		{
		pThis->tossChecked = TRUE;
		pSnb = pThis->u.u_sdldoc.pSnb;
		}
	    if (pThis->u.u_sdldoc.pHead &&
		!pThis->u.u_sdldoc.pHead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_sdldoc.pHead;
		break;
		}
	    if (pThis->u.u_sdldoc.pSnb &&
	        !pThis->u.u_sdldoc.pSnb->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_sdldoc.pSnb;
		break;
		}
	    pThis = NULL;
	    break;
	case e_virpage:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pThis->u.u_virpage.level;
		pThis->inheritedClass = MakeWideCharString("TEXT");
		MarkUsedStyle(pThis->inheritedLevel,
			      pThis->inheritedClass,
			      pThis->u.u_virpage.ssi);
		pThis->tossChecked = TRUE;
		pSnb = pThis->u.u_virpage.pSnb;
		}
	    if (pThis->u.u_virpage.pHead &&
		!pThis->u.u_virpage.pHead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pHead;
		break;
		}
	    if (pThis->u.u_virpage.pSnb &&
	        !pThis->u.u_virpage.pSnb->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pSnb;
		break;
		}
	    if (pThis->u.u_virpage.pBlockOrForm &&
	        !pThis->u.u_virpage.pBlockOrForm->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pBlockOrForm;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    m_free(pThis->inheritedClass, "wide character inherited class");
	    pThis = NULL;
	    break;
	case e_snb:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_snb.pHead &&
	        !pThis->u.u_snb.pHead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_virpage.pHead;
		break;
		}
	    if (pThis->u.u_snb.pSystemNotations)
		{ /* don't do anything now; wait for <snref> */
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_block:
	    if (!pThis->tossChecked)
		{
		if (!(pThis->inheritedLevel = pThis->u.u_block.level))
		    pThis->inheritedLevel = pParent->inheritedLevel;
		if (!(pThis->inheritedClass = pThis->u.u_block.class))
		    pThis->inheritedClass = pParent->inheritedClass;
		if (pThis->u.u_block.level ||
		    pThis->u.u_block.class ||
		    pThis->u.u_block.ssi)
			MarkUsedStyle(pThis->inheritedLevel,
				      pThis->inheritedClass,
				      pThis->u.u_block.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_block.pHead &&
	        !pThis->u.u_block.pHead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_block.pHead;
		break;
		}
	    if (pThis->u.u_block.pCpOrP &&
	        !pThis->u.u_block.pCpOrP->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_block.pCpOrP;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_form:
	    if (!pThis->tossChecked)
		{
		if (!(pThis->inheritedLevel = pThis->u.u_form.level))
		    pThis->inheritedLevel = pParent->inheritedLevel;
		if (!(pThis->inheritedClass = pThis->u.u_form.class))
		    pThis->inheritedClass = pParent->inheritedClass;
		if (pThis->u.u_form.level ||
		    pThis->u.u_form.class ||
		    pThis->u.u_form.ssi)
			MarkUsedStyle(pThis->inheritedLevel,
				      pThis->inheritedClass,
				      pThis->u.u_form.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_form.pHead &&
	        !pThis->u.u_form.pHead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_form.pHead;
		break;
		}
	    if (pThis->u.u_form.pFdata &&
	        !pThis->u.u_form.pFdata->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_form.pFdata;
		break;
		}
	    if (pThis->u.u_form.pFstyle &&
	        !pThis->u.u_form.pFstyle->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_form.pFstyle;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_fstyle:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_fstyle.pFrowvec &&
	        !pThis->u.u_fstyle.pFrowvec->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_fstyle.pFrowvec;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_frowvec:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_fdata:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_fdata.pBlockOrForm &&
	        !pThis->u.u_fdata.pBlockOrForm->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_fdata.pBlockOrForm;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_p:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		if (pThis->u.u_p.ssi)
		    MarkUsedStyle(pThis->inheritedLevel,
				  pThis->inheritedClass,
				  pThis->u.u_p.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_p.pHead &&
	        !pThis->u.u_p.pHead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_p.pHead;
		break;
		}
	    if (pThis->u.u_p.pAtomicOrPCDATA &&
	        !pThis->u.u_p.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_p.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_cp:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		if (pThis->u.u_cp.ssi)
		    MarkUsedStyle(pThis->inheritedLevel,
				  pThis->inheritedClass,
				  pThis->u.u_cp.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_cp.pCDATA &&
	        !pThis->u.u_cp.pCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_cp.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_head:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		if (!(pThis->inheritedClass = pThis->u.u_head.class))
		    pThis->inheritedClass = MakeWideCharString("HEAD");
		MarkUsedStyle(pThis->inheritedLevel,
			      pThis->inheritedClass,
			      pThis->u.u_head.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_head.pAtomicOrPCDATA &&
	        !pThis->u.u_head.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_head.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->u.u_head.pSubhead &&
	        !pThis->u.u_head.pSubhead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_head.pSubhead;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    if (!pThis->u.u_head.class)
		m_free(pThis->inheritedClass,
		       "wide character inherited class");
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_subhead:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		if (!(pThis->inheritedClass = pThis->u.u_subhead.class))
		    pThis->inheritedClass = pParent->inheritedClass;
		if (pThis->u.u_subhead.class || pThis->u.u_subhead.ssi)
		    MarkUsedStyle(pThis->inheritedLevel,
				  pThis->inheritedClass,
				  pThis->u.u_subhead.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_subhead.pAtomicOrPCDATA &&
	        !pThis->u.u_subhead.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_subhead.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_anchor:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_link:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_link.pAtomicOrPCDATA &&
	        !pThis->u.u_link.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_link.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_snref:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_snref.pRefitem &&
	        !pThis->u.u_snref.pRefitem->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_snref.pRefitem;
		break;
		}
	    if (pThis->u.u_snref.pAlttext &&
	        !pThis->u.u_snref.pAlttext->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_snref.pAlttext;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_refitem:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pThis->u.u_refitem.class;
		MarkUsedStyle(pThis->inheritedLevel,
			      pThis->inheritedClass,
			      pThis->u.u_refitem.ssi);
		if ((pItem = LookupInSnb(pSnb, pThis->u.u_refitem.rid)) &&
		    (pItem->type == e_text))
			{
		        pThis->u.u_refitem.pReferencedItem = pItem;
		        ClearTree(pItem);
			}
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_refitem.pHead &&
	        !pThis->u.u_refitem.pHead->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_refitem.pHead;
		break;
		}
	    if (pThis->u.u_refitem.pReferencedItem &&
	        !pThis->u.u_refitem.pReferencedItem->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_refitem.pReferencedItem;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_alttext:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_alttext.pCDATA &&
	        !pThis->u.u_alttext.pCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_alttext.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_rev:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_rev.pAtomicOrPCDATA &&
	        !pThis->u.u_rev.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_rev.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_key:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pThis->u.u_key.class;
		MarkUsedStyle(pThis->inheritedLevel,
			      pThis->inheritedClass,
			      pThis->u.u_key.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_key.pAtomicOrPCDATA &&
	        !pThis->u.u_key.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_key.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_sphrase:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pThis->u.u_sphrase.class;
		MarkUsedStyle(pThis->inheritedLevel,
			      pThis->inheritedClass,
			      pThis->u.u_sphrase.ssi);
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_sphrase.pSpcOrPCDATA &&
	        !pThis->u.u_sphrase.pSpcOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_sphrase.pSpcOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_if:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_if.pCond &&
	        !pThis->u.u_if.pCond->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_if.pCond;
		break;
		}
	    if (pThis->u.u_if.pThen &&
	        !pThis->u.u_if.pThen->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_if.pThen;
		break;
		}
	    if (pThis->u.u_if.pElse &&
	        !pThis->u.u_if.pElse->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_if.pElse;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_cond:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_cond.pCDATA &&
	        !pThis->u.u_cond.pCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_cond.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_then:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_then.pAtomicOrPCDATA &&
	        !pThis->u.u_then.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_then.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_else:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_else.pAtomicOrPCDATA &&
	        !pThis->u.u_else.pAtomicOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_else.pAtomicOrPCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_spc:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_graphic:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_text:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_text.pCpOrP &&
	        !pThis->u.u_text.pCpOrP->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_text.pCpOrP;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_audio:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_video:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_animate:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_script:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_script.pCDATA &&
	        !pThis->u.u_script.pCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_script.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_crossdoc:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_man_page:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_textfile:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_sys_cmd:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_callback:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_switch:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_switch.pCDATA &&
	        !pThis->u.u_switch.pCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_switch.pCDATA;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_cdata:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_entry:
	    if (!pThis->tossChecked)
		{
		pThis->inheritedLevel = pParent->inheritedLevel;
		pThis->inheritedClass = pParent->inheritedClass;
		pThis->tossChecked = TRUE;
		}
	    if (pThis->u.u_entry.pSimpleOrPCDATA &&
	        !pThis->u.u_entry.pSimpleOrPCDATA->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_entry.pSimpleOrPCDATA;
		break;
		}
	    if (pThis->u.u_entry.pEntry &&
	        !pThis->u.u_entry.pEntry->tossChecked)
		{
		PushThisElement(pParent);
		pParent = pThis;
		PushThisElement(pThis);
		pThis = pThis->u.u_entry.pEntry;
		break;
		}
	    if (pThis->pNext)
		{
		pThis = pThis->pNext;
		break;
		}
	    pThis   = PopThisElement();
	    pParent = PopThisElement();
	    break;
	case e_phrase:
	case e_keystyle:
	case e_headstyle:
	case e_formstyle:
	case e_frmtstyle:
	case e_grphstyle:
	    pThis = pThis->pNext;
	    break;
	default:
	    fprintf(stderr, "%s\n", errMess);
	    if (m_errfile && (m_errfile != stderr))
		fprintf(m_errfile, "%s\n", errMess);
	    exit(xxx);
	}
    }
}


void EmitSDL(ElementPtr pRoot)
{
ElementPtr     pThis, pThat;
long int       virpageOffset;
LOGICAL        doCompression;
int            zFileSize, length;
unsigned char  zTemp[4];
int            zFd;
char           buffer[BUFSIZ];
FILE          *saveOutFile;
char           cCount[32];
static char    errMess[] =
		    "internal error - unrecognized element type in EmitSDL";

virpageOffset = 0;
doCompression = FALSE;
if (pRoot)
    {
    if (minimalTossFlag)
	MarkUsedTOSS(pRoot);
    if (pRoot->type == e_virpage)
	{
	virpageOffset = ftell(outFile);
	if (compressFlag)
	    {
	    doCompression = TRUE;
	    saveOutFile = outFile;
	    outFile = FopenFile(compFileName, O_WRONLY, xxx);
	    }
	}
    pThis = pRoot;
    }
else
    {
    pThis = pElementChain;
    sprintf(cCount, " COUNT=\"%d\">\n", nElements);
    PutString(cCount);
    pElementChain = NULL;
    nElements     = 0;

    /* push a null so the last <entry> pop will terminate the loop */
    if (pThis && (pThis->type == e_entry))
	PushThisElement(NULL);
    }

while (pThis)
    {
    switch (pThis->type)
	{
	case e_sdldoc:
	    /* we've already emitted the <sdldoc ...> stuff */
	    if (pThis->u.u_sdldoc.pHead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_sdldoc.pHead;
		pThat->u.u_sdldoc.pHead = NULL;
		break;
		}
	    if (pThis->u.u_sdldoc.pSnb)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_sdldoc.pSnb;
		pThat->u.u_sdldoc.pSnb = NULL;
		break;
		}
	    DeleteElement(pThis);
	    pThis = NULL;
	    break;
	case e_virpage:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("VIRPAGE", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_virpage.id);
		AddToLOIDS("VIRPAGE",
			   pThis->u.u_virpage.id,
			   pThis->u.u_virpage.ssi,
			   pThis->u.u_virpage.level,
			   virpageOffset);
		RequiredAttribute("LEVEL", pThis->u.u_virpage.level);
		ImpliedAttribute("VERSION", pThis->u.u_virpage.version);
		ImpliedAttribute("LANGUAGE", pThis->u.u_virpage.language);
		ImpliedAttribute("CHARSET", pThis->u.u_virpage.charset);
		RequiredAttribute("DOC-ID", pThis->u.u_virpage.doc_id);
		ImpliedAttribute("SSI", pThis->u.u_virpage.ssi);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_virpage.pHead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_virpage.pHead;
		pThat->u.u_virpage.pHead = NULL;
		break;
		}
	    if (pThis->u.u_virpage.pSnb)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_virpage.pSnb;
		pThat->u.u_virpage.pSnb = NULL;
		break;
		}
	    if (pThis->u.u_virpage.pBlockOrForm)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_virpage.pBlockOrForm;
		pThat->u.u_virpage.pBlockOrForm = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("VIRPAGE", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("VIRPAGE", TRUE);
	    pThis = NULL;
	    break;
	case e_snb:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SNB", TRUE, FALSE);
		ImpliedAttribute("VERSION", pThis->u.u_snb.version);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_snb.pHead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_virpage.pHead;
		pThat->u.u_virpage.pHead = NULL;
		break;
		}
	    if (pThis->u.u_snb.pSystemNotations)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_snb.pSystemNotations;
		pThat->u.u_snb.pSystemNotations = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("SNB", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("SNB", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_block:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("BLOCK", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_block.id);
		AddToLOIDS("BLOCK",
			   pThis->u.u_block.id,
			   pThis->u.u_block.ssi,
			   pThis->u.u_block.level,
			   virpageOffset);
		ImpliedAttribute("LEVEL", pThis->u.u_block.level);
		ImpliedAttribute("VERSION", pThis->u.u_block.version);
		ImpliedAttribute("CLASS", pThis->u.u_block.class);
		ImpliedAttribute("LANGUAGE", pThis->u.u_block.language);
		ImpliedAttribute("CHARSET", pThis->u.u_block.charset);
		ImpliedAttribute("LENGTH", pThis->u.u_block.length);
		ImpliedAttribute("APP", pThis->u.u_block.app);
		ImpliedAttribute("TIMING", pThis->u.u_block.timing);
		ImpliedAttribute("SSI", pThis->u.u_block.ssi);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_block.pHead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_block.pHead;
		pThat->u.u_block.pHead = NULL;
		break;
		}
	    if (pThis->u.u_block.pCpOrP)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_block.pCpOrP;
		pThat->u.u_block.pCpOrP = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("BLOCK", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("BLOCK", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_form:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("FORM", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_form.id);
		AddToLOIDS("FORM",
			   pThis->u.u_form.id,
			   pThis->u.u_form.ssi,
			   pThis->u.u_form.level,
			   virpageOffset);
		ImpliedAttribute("LEVEL", pThis->u.u_form.level);
		ImpliedAttribute("VERSION", pThis->u.u_form.version);
		ImpliedAttribute("CLASS", pThis->u.u_form.class);
		ImpliedAttribute("LANGUAGE", pThis->u.u_form.language);
		ImpliedAttribute("CHARSET", pThis->u.u_form.charset);
		ImpliedAttribute("LENGTH", pThis->u.u_form.length);
		ImpliedAttribute("APP", pThis->u.u_form.app);
		ImpliedAttribute("SSI", pThis->u.u_form.ssi);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_form.pHead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_form.pHead;
		pThat->u.u_form.pHead = NULL;
		break;
		}
	    if (pThis->u.u_form.pFdata)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_form.pFdata;
		pThat->u.u_form.pFdata = NULL;
		break;
		}
	    if (pThis->u.u_form.pFstyle)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_form.pFstyle;
		pThat->u.u_form.pFstyle = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("FORM", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("FORM", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_fstyle:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("FSTYLE", TRUE, FALSE);
		ImpliedAttribute("NCOLS", pThis->u.u_fstyle.ncols);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_fstyle.pFrowvec)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_fstyle.pFrowvec;
		pThat->u.u_fstyle.pFrowvec = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("FSTYLE", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("FSTYLE", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_frowvec:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("FROWVEC", TRUE, FALSE);
		ImpliedAttribute("HDR", pThis->u.u_frowvec.hdr);
		RequiredAttribute("CELLS", pThis->u.u_frowvec.cells);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_fdata:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("FDATA", FALSE, TRUE);
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_fdata.pBlockOrForm)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_fdata.pBlockOrForm;
		pThat->u.u_fdata.pBlockOrForm = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("FDATA", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("FDATA", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_p:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("P", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_p.id);
		AddToLOIDS("P",
			   pThis->u.u_p.id,
			   pThis->u.u_p.ssi,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("VERSION", pThis->u.u_p.version);
		ImpliedAttribute("TYPE", pThis->u.u_p.type);
		ImpliedAttribute("SSI", pThis->u.u_p.ssi);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_p.pHead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_p.pHead;
		pThat->u.u_p.pHead = NULL;
		break;
		}
	    if (pThis->u.u_p.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_p.pAtomicOrPCDATA;
		pThat->u.u_p.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("P", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("P", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_cp:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("CP", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_cp.id);
		AddToLOIDS("CP",
			   pThis->u.u_cp.id,
			   pThis->u.u_cp.ssi,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("VERSION", pThis->u.u_cp.version);
		ImpliedAttribute("TYPE", pThis->u.u_cp.type);
		ImpliedAttribute("SSI", pThis->u.u_cp.ssi);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_cp.pCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_cp.pCDATA;
		pThat->u.u_cp.pCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("CP", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("CP", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_head:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("HEAD", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_head.id);
		AddToLOIDS("HEAD",
			   pThis->u.u_head.id,
			   pThis->u.u_head.ssi,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("VERSION", pThis->u.u_head.version);
		ImpliedAttribute("CLASS", pThis->u.u_head.class);
		ImpliedAttribute("LANGUAGE", pThis->u.u_head.language);
		ImpliedAttribute("CHARSET", pThis->u.u_head.charset);
		ImpliedAttribute("TYPE", pThis->u.u_head.type);
		ImpliedAttribute("SSI", pThis->u.u_head.ssi);
		ImpliedAttribute("ABBREV", pThis->u.u_head.abbrev);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_head.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_head.pAtomicOrPCDATA;
		pThat->u.u_head.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->u.u_head.pSubhead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_head.pSubhead;
		pThat->u.u_head.pSubhead = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		pThat = PopThisElement();
		if (pThat->type == e_p)
		    CloseNamedTag("HEAD", FALSE);
		else
		    CloseNamedTag("HEAD", TRUE);
		PushThisElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    if (pThis->type == e_p)
		CloseNamedTag("HEAD", FALSE);
	    else
		CloseNamedTag("HEAD", TRUE);
	    break;
	case e_subhead:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SUBHEAD", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_subhead.id);
		AddToLOIDS("SUBHEAD",
			   pThis->u.u_subhead.id,
			   pThis->u.u_subhead.ssi,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("VERSION", pThis->u.u_subhead.version);
		ImpliedAttribute("CLASS", pThis->u.u_subhead.class);
		ImpliedAttribute("LANGUAGE", pThis->u.u_subhead.language);
		ImpliedAttribute("CHARSET", pThis->u.u_subhead.charset);
		ImpliedAttribute("TYPE", pThis->u.u_subhead.type);
		ImpliedAttribute("SSI", pThis->u.u_subhead.ssi);
		ImpliedAttribute("ABBREV", pThis->u.u_subhead.abbrev);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_subhead.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_subhead.pAtomicOrPCDATA;
		pThat->u.u_subhead.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("SUBHEAD", FALSE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("SUBHEAD", FALSE);
	    pThis = PopThisElement();
	    break;
	case e_anchor:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("ANCHOR", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_anchor.id);
		AddToLOIDS("ANCHOR",
			   pThis->u.u_anchor.id,
			   NULL,
			   NULL,
			   virpageOffset);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_link:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("LINK", TRUE, FALSE);
		RequiredAttribute("RID", pThis->u.u_link.rid);
		ImpliedAttribute("BUTTON", pThis->u.u_link.button);
		ImpliedAttribute("LINKINFO", pThis->u.u_link.linkinfo);
		ImpliedAttribute("DESCRIPT", pThis->u.u_link.descript);
		ImpliedAttribute("WINDOW", pThis->u.u_link.window);
		ImpliedAttribute("TRAVERSAL", pThis->u.u_link.traversal);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_link.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_link.pAtomicOrPCDATA;
		pThat->u.u_link.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("LINK", FALSE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("LINK", FALSE);
	    pThis = PopThisElement();
	    break;
	case e_snref:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SNREF", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_snref.id);
		AddToLOIDS("SNREF",
			   pThis->u.u_snref.id,
			   NULL,
			   NULL,
			   virpageOffset);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_snref.pRefitem)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_snref.pRefitem;
		pThat->u.u_snref.pRefitem = NULL;
		break;
		}
	    if (pThis->u.u_snref.pAlttext)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_snref.pAlttext;
		pThat->u.u_snref.pAlttext = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("SNREF", FALSE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("SNREF", FALSE);
	    pThis = PopThisElement();
	    break;
	case e_refitem:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("REFITEM", TRUE, FALSE);
		RequiredAttribute("RID", pThis->u.u_refitem.rid);
		RequiredAttribute("CLASS", pThis->u.u_refitem.class);
		ImpliedAttribute("BUTTON", pThis->u.u_refitem.button);
		ImpliedAttribute("SSI", pThis->u.u_refitem.ssi);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_refitem.pHead)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_refitem.pHead;
		pThat->u.u_refitem.pHead = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("REFITEM", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("REFITEM", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_alttext:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("ALTTEXT", FALSE, FALSE);
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_alttext.pCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_alttext.pCDATA;
		pThat->u.u_alttext.pCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("ALTTEXT", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("ALTTEXT", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_rev:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("REV", FALSE, FALSE);
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_rev.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_rev.pAtomicOrPCDATA;
		pThat->u.u_rev.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("REV", FALSE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("REV", FALSE);
	    pThis = PopThisElement();
	    break;
	case e_key:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("KEY", TRUE, FALSE);
		ImpliedAttribute("CHARSET", pThis->u.u_key.charset);
		RequiredAttribute("CLASS", pThis->u.u_key.class);
		ImpliedAttribute("SSI", pThis->u.u_key.ssi);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_key.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_key.pAtomicOrPCDATA;
		pThat->u.u_key.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("KEY", FALSE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("KEY", FALSE);
	    pThis = PopThisElement();
	    break;
	case e_sphrase:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SPHRASE", TRUE, FALSE);
		RequiredAttribute("CLASS", pThis->u.u_sphrase.class);
		ImpliedAttribute("SSI", pThis->u.u_sphrase.ssi);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_sphrase.pSpcOrPCDATA) 
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_sphrase.pSpcOrPCDATA;
		pThat->u.u_sphrase.pSpcOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("SPHRASE", FALSE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("SPHRASE", FALSE);
	    pThis = PopThisElement();
	    break;
	case e_if:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("IF", FALSE, FALSE);
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_if.pCond)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_if.pCond;
		pThat->u.u_if.pCond = NULL;
		break;
		}
	    if (pThis->u.u_if.pThen)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_if.pThen;
		pThat->u.u_if.pThen = NULL;
		break;
		}
	    if (pThis->u.u_if.pElse)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_if.pElse;
		pThat->u.u_if.pElse = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("IF", FALSE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("IF", FALSE);
	    pThis = PopThisElement();
	    break;
	case e_cond:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("COND", TRUE, FALSE);
		ImpliedAttribute("INTERP", pThis->u.u_cond.interp);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_cond.pCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_cond.pCDATA;
		pThat->u.u_cond.pCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("COND", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("COND", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_then:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("THEN", FALSE, FALSE);
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_then.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_then.pAtomicOrPCDATA;
		pThat->u.u_then.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("THEN", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("THEN", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_else:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("ELSE", FALSE, FALSE);
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_else.pAtomicOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_else.pAtomicOrPCDATA;
		pThat->u.u_else.pAtomicOrPCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("ELSE", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("ELSE", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_spc:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SPC", TRUE, FALSE);
		RequiredAttribute("NAME", pThis->u.u_spc.name);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_graphic:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("GRAPHIC", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_graphic.id);
		AddToLOIDS("GRAPHIC",
			   pThis->u.u_graphic.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("FORMAT", pThis->u.u_graphic.format);
		ImpliedAttribute("METHOD", pThis->u.u_graphic.method);
		RequiredAttribute("XID", pThis->u.u_graphic.xid);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_text:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("TEXT", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_text.id);
		AddToLOIDS("TEXT",
			   pThis->u.u_text.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("LANGUAGE", pThis->u.u_text.language);
		ImpliedAttribute("CHARSET", pThis->u.u_text.charset);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_text.pCpOrP)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_text.pCpOrP;
		pThat->u.u_text.pCpOrP = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("TEXT", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("TEXT", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_audio:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("AUDIO", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_audio.id);
		AddToLOIDS("AUDIO",
			   pThis->u.u_audio.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("FORMAT", pThis->u.u_audio.format);
		ImpliedAttribute("METHOD", pThis->u.u_audio.method);
		RequiredAttribute("XID", pThis->u.u_audio.xid);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_video:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("VIDEO", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_video.id);
		AddToLOIDS("VIDEO",
			   pThis->u.u_video.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("FORMAT", pThis->u.u_video.format);
		ImpliedAttribute("METHOD", pThis->u.u_video.method);
		RequiredAttribute("XID", pThis->u.u_video.xid);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_animate:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("ANIMATE", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_animate.id);
		AddToLOIDS("ANIMATE",
			   pThis->u.u_animate.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("FORMAT", pThis->u.u_animate.format);
		ImpliedAttribute("METHOD", pThis->u.u_animate.method);
		RequiredAttribute("XID", pThis->u.u_animate.xid);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_script:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SCRIPT", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_script.id);
		AddToLOIDS("SCRIPT",
			   pThis->u.u_script.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("INTERP", pThis->u.u_script.interp);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_script.pCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_script.pCDATA;
		pThat->u.u_script.pCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("SCRIPT", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("SCRIPT", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_crossdoc:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("CROSSDOC", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_crossdoc.id);
		AddToLOIDS("CROSSDOC",
			   pThis->u.u_crossdoc.id,
			   NULL,
			   NULL,
			   virpageOffset);
		RequiredAttribute("XID", pThis->u.u_crossdoc.xid);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_man_page:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("MAN-PAGE", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_man_page.id);
		AddToLOIDS("MAN-PAGE",
			   pThis->u.u_man_page.id,
			   NULL,
			   NULL,
			   virpageOffset);
		RequiredAttribute("XID", pThis->u.u_man_page.xid);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_textfile:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("TEXTFILE", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_textfile.id);
		AddToLOIDS("TEXTFILE",
			   pThis->u.u_textfile.id,
			   NULL,
			   NULL,
			   virpageOffset);
		RequiredAttribute("XID", pThis->u.u_textfile.xid);
		ImpliedAttribute("OFFSET", pThis->u.u_textfile.offset);
		ImpliedAttribute("FORMAT", pThis->u.u_textfile.format);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_sys_cmd:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SYS-CMD", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_sys_cmd.id);
		AddToLOIDS("SYS-CMD",
			   pThis->u.u_sys_cmd.id,
			   NULL,
			   NULL,
			   virpageOffset);
		RequiredAttribute("COMMAND", pThis->u.u_sys_cmd.command);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_callback:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("CALLBACK", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_callback.id);
		AddToLOIDS("CALLBACK",
			   pThis->u.u_callback.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("DATA", pThis->u.u_callback.data);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_switch:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("SWITCH", TRUE, FALSE);
		RequiredAttribute("ID", pThis->u.u_switch.id);
		AddToLOIDS("SWITCH",
			   pThis->u.u_switch.id,
			   NULL,
			   NULL,
			   virpageOffset);
		ImpliedAttribute("INTERP", pThis->u.u_switch.interp);
		RequiredAttribute("BRANCHES", pThis->u.u_switch.branches);
		PutString(">\n");
		pThis->beenEmitted = TRUE;
	    }
	    if (pThis->u.u_switch.pCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_switch.pCDATA;
		pThat->u.u_switch.pCDATA = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("SWITCH", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("SWITCH", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_cdata:
	    pCurrentElement = pThis;
	    SaveWChar(0);
	    PutWString(pThis->u.u_cdata.buffer);
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    DeleteElement(pThis);
	    pThis = PopThisElement();
	    break;
	case e_entry:
	    if (!pThis->beenEmitted)
		{
		OpenNamedTag("ENTRY", TRUE, FALSE);
		ImpliedAttribute("ID", pThis->u.u_entry.id);
		ImpliedAttribute("MAIN", pThis->u.u_entry.main);
		ImpliedAttribute("LOCS", pThis->u.u_entry.locs);
		ImpliedAttribute("SYNS", pThis->u.u_entry.syns);
		ImpliedAttribute("SORT", pThis->u.u_entry.sort);
		PutString(">");
		pThis->beenEmitted = TRUE;
		}
	    if (pThis->u.u_entry.pSimpleOrPCDATA)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_entry.pSimpleOrPCDATA;
		pThat->u.u_entry.pSimpleOrPCDATA = NULL;
		break;
		}
	    if (pThis->u.u_entry.pEntry)
		{
		PushThisElement(pThis);
		pThat = pThis;
		pThis = pThis->u.u_entry.pEntry;
		pThat->u.u_entry.pEntry = NULL;
		break;
		}
	    if (pThis->pNext)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		CloseNamedTag("ENTRY", TRUE);
		break;
		}
	    DeleteElement(pThis);
	    CloseNamedTag("ENTRY", TRUE);
	    pThis = PopThisElement();
	    break;
	case e_phrase:
	    OpenNamedTag("PHRASE", TRUE, FALSE);
	    RequiredAttribute("TEXT", pThis->u.u_phrase.text);
	    PutString(">\n");
	    pThat = pThis;
	    pThis = pThat->pNext;
	    DeleteElement(pThat);
	    break;
	case e_keystyle:
	    if (minimalTossFlag && !pThis->u.u_keystyle.beenUsed)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    OpenNamedTag("KEYSTYLE", TRUE, FALSE);
	    RequiredAttribute("CLASS", pThis->u.u_keystyle.class);
	    ImpliedAttribute("SSI", pThis->u.u_keystyle.ssi);
	    ImpliedAttribute("RLEVEL", pThis->u.u_keystyle.rlevel);
	    ImpliedAttribute("PHRASE", pThis->u.u_keystyle.phrase);
	    ImpliedAttribute("SRCH-WT", pThis->u.u_keystyle.srch_wt);
	    ImpliedAttribute("ENTER", pThis->u.u_keystyle.enter);
	    ImpliedAttribute("EXIT", pThis->u.u_keystyle.exit);
	    ImpliedAttribute("POINTSZ", pThis->u.u_keystyle.pointsz);
	    ImpliedAttribute("SET-WIDTH", pThis->u.u_keystyle.set_width);
	    ImpliedAttribute("COLOR", pThis->u.u_keystyle.color);
	    ImpliedAttribute("XLFD", pThis->u.u_keystyle.xlfd);
	    ImpliedAttribute("XLFDI", pThis->u.u_keystyle.xlfdi);
	    ImpliedAttribute("XLFDB", pThis->u.u_keystyle.xlfdb);
	    ImpliedAttribute("XLFDIB", pThis->u.u_keystyle.xlfdib);
	    ImpliedAttribute("TYPENAM", pThis->u.u_keystyle.typenam);
	    ImpliedAttribute("TYPENAMI", pThis->u.u_keystyle.typenami);
	    ImpliedAttribute("TYPENAMB", pThis->u.u_keystyle.typenamb);
	    ImpliedAttribute("TYPENAMIB", pThis->u.u_keystyle.typenamib);
	    ImpliedAttribute("STYLE", pThis->u.u_keystyle.style);
	    ImpliedAttribute("SPACING", pThis->u.u_keystyle.spacing);
	    ImpliedAttribute("WEIGHT", pThis->u.u_keystyle.weight);
	    ImpliedAttribute("SLANT", pThis->u.u_keystyle.slant);
	    ImpliedAttribute("SPECIAL", pThis->u.u_keystyle.special);
	    PutString(">\n");
	    pThat = pThis;
	    pThis = pThat->pNext;
	    pThat->pNext = NULL;
	    DeleteElement(pThat);
	    break;
	case e_headstyle:
	    if (minimalTossFlag && !pThis->u.u_headstyle.beenUsed)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    OpenNamedTag("HEADSTYLE", TRUE, FALSE);
	    RequiredAttribute("CLASS", pThis->u.u_headstyle.class);
	    ImpliedAttribute("SSI", pThis->u.u_headstyle.ssi);
	    ImpliedAttribute("RLEVEL", pThis->u.u_headstyle.rlevel);
	    ImpliedAttribute("PHRASE", pThis->u.u_headstyle.phrase);
	    ImpliedAttribute("SRCH-WT", pThis->u.u_headstyle.srch_wt);
	    ImpliedAttribute("ORIENT", pThis->u.u_headstyle.orient);
	    ImpliedAttribute("VORIENT", pThis->u.u_headstyle.vorient);
	    ImpliedAttribute("PLACEMENT", pThis->u.u_headstyle.placement);
	    ImpliedAttribute("HEADW", pThis->u.u_headstyle.headw);
	    ImpliedAttribute("STACK", pThis->u.u_headstyle.stack);
	    ImpliedAttribute("FLOW", pThis->u.u_headstyle.flow);
	    ImpliedAttribute("POINTSZ", pThis->u.u_headstyle.pointsz);
	    ImpliedAttribute("SET-WIDTH", pThis->u.u_headstyle.set_width);
	    ImpliedAttribute("COLOR", pThis->u.u_headstyle.color);
	    ImpliedAttribute("XLFD", pThis->u.u_headstyle.xlfd);
	    ImpliedAttribute("XLFDI", pThis->u.u_headstyle.xlfdi);
	    ImpliedAttribute("XLFDB", pThis->u.u_headstyle.xlfdb);
	    ImpliedAttribute("XLFDIB", pThis->u.u_headstyle.xlfdib);
	    ImpliedAttribute("TYPENAM", pThis->u.u_headstyle.typenam);
	    ImpliedAttribute("TYPENAMI", pThis->u.u_headstyle.typenami);
	    ImpliedAttribute("TYPENAMB", pThis->u.u_headstyle.typenamb);
	    ImpliedAttribute("TYPENAMIB", pThis->u.u_headstyle.typenamib);
	    ImpliedAttribute("STYLE", pThis->u.u_headstyle.style);
	    ImpliedAttribute("SPACING", pThis->u.u_headstyle.spacing);
	    ImpliedAttribute("WEIGHT", pThis->u.u_headstyle.weight);
	    ImpliedAttribute("SLANT", pThis->u.u_headstyle.slant);
	    ImpliedAttribute("SPECIAL", pThis->u.u_headstyle.special);
	    ImpliedAttribute("L-MARGIN", pThis->u.u_headstyle.l_margin);
	    ImpliedAttribute("R-MARGIN", pThis->u.u_headstyle.r_margin);
	    ImpliedAttribute("T-MARGIN", pThis->u.u_headstyle.t_margin);
	    ImpliedAttribute("B-MARGIN", pThis->u.u_headstyle.b_margin);
	    ImpliedAttribute("BORDER", pThis->u.u_headstyle.border);
	    ImpliedAttribute("VJUST", pThis->u.u_headstyle.vjust);
	    ImpliedAttribute("JUSTIFY", pThis->u.u_headstyle.justify);
	    PutString(">\n");
	    pThat = pThis;
	    pThis = pThat->pNext;
	    pThat->pNext = NULL;
	    DeleteElement(pThat);
	    break;
	case e_formstyle:
	    if (minimalTossFlag && !pThis->u.u_formstyle.beenUsed)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    OpenNamedTag("FORMSTYLE", TRUE, FALSE);
	    RequiredAttribute("CLASS", pThis->u.u_formstyle.class);
	    ImpliedAttribute("SSI", pThis->u.u_formstyle.ssi);
	    ImpliedAttribute("RLEVEL", pThis->u.u_formstyle.rlevel);
	    ImpliedAttribute("PHRASE", pThis->u.u_formstyle.phrase);
	    ImpliedAttribute("SRCH-WT", pThis->u.u_formstyle.srch_wt);
	    ImpliedAttribute("COLW", pThis->u.u_formstyle.colw);
	    ImpliedAttribute("COLJ", pThis->u.u_formstyle.colj);
	    ImpliedAttribute("POINTSZ", pThis->u.u_formstyle.pointsz);
	    ImpliedAttribute("SET-WIDTH", pThis->u.u_formstyle.set_width);
	    ImpliedAttribute("COLOR", pThis->u.u_formstyle.color);
	    ImpliedAttribute("XLFD", pThis->u.u_formstyle.xlfd);
	    ImpliedAttribute("XLFDI", pThis->u.u_formstyle.xlfdi);
	    ImpliedAttribute("XLFDB", pThis->u.u_formstyle.xlfdb);
	    ImpliedAttribute("XLFDIB", pThis->u.u_formstyle.xlfdib);
	    ImpliedAttribute("TYPENAM", pThis->u.u_formstyle.typenam);
	    ImpliedAttribute("TYPENAMI", pThis->u.u_formstyle.typenami);
	    ImpliedAttribute("TYPENAMB", pThis->u.u_formstyle.typenamb);
	    ImpliedAttribute("TYPENAMIB", pThis->u.u_formstyle.typenamib);
	    ImpliedAttribute("STYLE", pThis->u.u_formstyle.style);
	    ImpliedAttribute("SPACING", pThis->u.u_formstyle.spacing);
	    ImpliedAttribute("WEIGHT", pThis->u.u_formstyle.weight);
	    ImpliedAttribute("SLANT", pThis->u.u_formstyle.slant);
	    ImpliedAttribute("SPECIAL", pThis->u.u_formstyle.special);
	    ImpliedAttribute("L-MARGIN", pThis->u.u_formstyle.l_margin);
	    ImpliedAttribute("R-MARGIN", pThis->u.u_formstyle.r_margin);
	    ImpliedAttribute("T-MARGIN", pThis->u.u_formstyle.t_margin);
	    ImpliedAttribute("B-MARGIN", pThis->u.u_formstyle.b_margin);
	    ImpliedAttribute("BORDER", pThis->u.u_formstyle.border);
	    ImpliedAttribute("VJUST", pThis->u.u_formstyle.vjust);
	    PutString(">\n");
	    pThat = pThis;
	    pThis = pThat->pNext;
	    pThat->pNext = NULL;
	    DeleteElement(pThat);
	    break;
	case e_frmtstyle:
	    if (minimalTossFlag && !pThis->u.u_frmtstyle.beenUsed)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    OpenNamedTag("FRMTSTYLE", TRUE, FALSE);
	    RequiredAttribute("CLASS", pThis->u.u_frmtstyle.class);
	    ImpliedAttribute("SSI", pThis->u.u_frmtstyle.ssi);
	    ImpliedAttribute("RLEVEL", pThis->u.u_frmtstyle.rlevel);
	    ImpliedAttribute("PHRASE", pThis->u.u_frmtstyle.phrase);
	    ImpliedAttribute("F-MARGIN", pThis->u.u_frmtstyle.f_margin);
	    ImpliedAttribute("SRCH-WT", pThis->u.u_frmtstyle.srch_wt);
	    ImpliedAttribute("POINTSZ", pThis->u.u_frmtstyle.pointsz);
	    ImpliedAttribute("SET-WIDTH", pThis->u.u_frmtstyle.set_width);
	    ImpliedAttribute("COLOR", pThis->u.u_frmtstyle.color);
	    ImpliedAttribute("XLFD", pThis->u.u_frmtstyle.xlfd);
	    ImpliedAttribute("XLFDI", pThis->u.u_frmtstyle.xlfdi);
	    ImpliedAttribute("XLFDB", pThis->u.u_frmtstyle.xlfdb);
	    ImpliedAttribute("XLFDIB", pThis->u.u_frmtstyle.xlfdib);
	    ImpliedAttribute("TYPENAM", pThis->u.u_frmtstyle.typenam);
	    ImpliedAttribute("TYPENAMI", pThis->u.u_frmtstyle.typenami);
	    ImpliedAttribute("TYPENAMB", pThis->u.u_frmtstyle.typenamb);
	    ImpliedAttribute("TYPENAMIB", pThis->u.u_frmtstyle.typenamib);
	    ImpliedAttribute("STYLE", pThis->u.u_frmtstyle.style);
	    ImpliedAttribute("SPACING", pThis->u.u_frmtstyle.spacing);
	    ImpliedAttribute("WEIGHT", pThis->u.u_frmtstyle.weight);
	    ImpliedAttribute("SLANT", pThis->u.u_frmtstyle.slant);
	    ImpliedAttribute("SPECIAL", pThis->u.u_frmtstyle.special);
	    ImpliedAttribute("L-MARGIN", pThis->u.u_frmtstyle.l_margin);
	    ImpliedAttribute("R-MARGIN", pThis->u.u_frmtstyle.r_margin);
	    ImpliedAttribute("T-MARGIN", pThis->u.u_frmtstyle.t_margin);
	    ImpliedAttribute("B-MARGIN", pThis->u.u_frmtstyle.b_margin);
	    ImpliedAttribute("BORDER", pThis->u.u_frmtstyle.border);
	    ImpliedAttribute("VJUST", pThis->u.u_frmtstyle.vjust);
	    ImpliedAttribute("JUSTIFY", pThis->u.u_frmtstyle.justify);
	    PutString(">\n");
	    pThat = pThis;
	    pThis = pThat->pNext;
	    pThat->pNext = NULL;
	    DeleteElement(pThat);
	    break;
	case e_grphstyle:
	    if (minimalTossFlag && !pThis->u.u_grphstyle.beenUsed)
		{
		pThat = pThis;
		pThis = pThat->pNext;
		pThat->pNext = NULL;
		DeleteElement(pThat);
		break;
		}
	    OpenNamedTag("GRPHSTYLE", TRUE, FALSE);
	    RequiredAttribute("CLASS", pThis->u.u_grphstyle.class);
	    ImpliedAttribute("SSI", pThis->u.u_grphstyle.ssi);
	    ImpliedAttribute("RLEVEL", pThis->u.u_grphstyle.rlevel);
	    ImpliedAttribute("PHRASE", pThis->u.u_grphstyle.phrase);
	    ImpliedAttribute("L-MARGIN", pThis->u.u_grphstyle.l_margin);
	    ImpliedAttribute("R-MARGIN", pThis->u.u_grphstyle.r_margin);
	    ImpliedAttribute("T-MARGIN", pThis->u.u_grphstyle.t_margin);
	    ImpliedAttribute("B-MARGIN", pThis->u.u_grphstyle.b_margin);
	    ImpliedAttribute("BORDER", pThis->u.u_grphstyle.border);
	    ImpliedAttribute("VJUST", pThis->u.u_grphstyle.vjust);
	    ImpliedAttribute("JUSTIFY", pThis->u.u_grphstyle.justify);
	    PutString(">\n");
	    pThat = pThis;
	    pThis = pThat->pNext;
	    pThat->pNext = NULL;
	    DeleteElement(pThat);
	    break;
	default:
	    fprintf(stderr, "%s\n", errMess);
	    if (m_errfile && (m_errfile != stderr))
		fprintf(m_errfile, "%s\n", errMess);
	    exit(xxx);
	}
    }

if (doCompression)
    {
    FcloseFile(outFile, compFileName, xxx);
    outFile   = saveOutFile;
    zFileSize = Compress(compFileName, compZFileName, TRUE);

    /* write out the magic number and the file size */
    zTemp[3] = zFileSize % 256;
    zFileSize /= 256;
    zTemp[2] = zFileSize % 256;
    zFileSize /= 256;
    zTemp[1] = zFileSize % 256;
    zTemp[0] = '\0';
    FwriteFile(outFile, tempFileName, (char *) zTemp, 4, xxx);

    /* copy the compressed virpage to output */
    zFd = OpenFile(compZFileName, O_RDONLY, xxx);
    while (length = ReadFile(zFd, compZFileName, buffer, BUFSIZ, xxx))
	FwriteFile(outFile, tempFileName, buffer, length, xxx);
    CloseFile(zFd, compZFileName, xxx);
    unlink(compZFileName);
    unlink(compFileName);
    }
}


void OptimizeAndEmit(ElementPtr pVirpage)
{
if (optimizeFlag)
    OptimizeSDL(&(pVirpage->u.u_virpage.pBlockOrForm));
EmitSDL(pVirpage);
}


/* A routine to emit a help message and exit */
static void Help(void)
{
printf("The available options to %s are 'c', 'd', 'h', 'o' and 't'.\n\n",
       progName);
printf("Their meanings are:\n");
printf("    c: compress an SDL document (on a per virtual page basis)\n");
printf("    d: decompress an SDL document\n");
printf("    h: print this message and exit\n");
printf("    o: perform peephole optimizations on this SDL document\n");
printf("    t: eliminate any TOSS elements not used in this SDL document\n\n");
printf("You may specify either one filename or two.  If one file is\n");
printf("specified, it will be used as both the input and output file.\n");
printf("If the input file has no .sdl extension, one will be added.  If\n");
printf("an explicit output filename is specified, it will be used without\n");
printf("modification.\n\n");
exit(0);
}


/* A routine to emit a usage message and exit */
static void Usage(void)
{
fprintf(stderr,
	"Usage: %s [-cdhot] <inFileName> [<outFileName>]\n",
	progName);
if (m_errfile && (m_errfile != stderr))
    {
    fprintf(m_errfile,
	    "Usage: %s [-cdhot] <inFileName> [<outFileName>]\n",
	    progName);
    }
exit(1);
}


/*
 * A routine to pre-build all the input, output and intermediate file
 * names that we'll need.
*/
static void MakeFileNames(void)
{
int   argCount;
int   length, c;
char *cp;
char  pid[32];
char *tmpDir;

progName = strrchr(*m_argv, '/');
if (progName) progName++;
else progName = *m_argv;
argCount  = 1;

while ((c = getopt(m_argc, m_argv, "cdhot")) != -1)
    {
    switch (c)
	{
	case 'c':
	    compressFlag = TRUE;
	    break;
	case 'd':
	    decompressFlag = TRUE;
	    break;
	case 'h':
	    Help();
	case 'o':
	    optimizeFlag = TRUE;
	    break;
	case 't':
	    minimalTossFlag = TRUE;
	    break;
	case '?':
	    Usage();
	}
    }
argCount += (optind - 1);

if (((m_argc - argCount) < 1) || ((m_argc - argCount) > 2))
    Usage();

inFileName  = m_argv[argCount++];
outFileName = m_argv[argCount];     /* may be NULL */

cp = strrchr(inFileName, '.');
if (cp)
    {
    if (strcmp(cp, dotSDL) == 0)
	{
	length = cp - inFileName;
	cp = malloc(length + 1);
	strncpy(cp, inFileName, length);
	cp[length] = '\0';
	}
    else
	{
	cp = inFileName;
	}
    }
else
    cp = inFileName;
length = strlen(cp);
inFileName = malloc(length + DOTSDLSIZ);
strcpy(inFileName, cp);
strcat(inFileName, dotSDL);
if (!outFileName)
    outFileName = inFileName;

errFileName = malloc(length + DOTERRSIZ);
strcpy(errFileName, cp);
strcat(errFileName, dotERR);

idxFileName = malloc(length + DOTIDXSIZ);
strcpy(idxFileName, cp);
strcat(idxFileName, dotIDX);

snbFileName = malloc(length + DOTSNBSIZ);
strcpy(snbFileName, cp);
strcat(snbFileName, dotSNB);

tossFileName = malloc(length + DOTTSSSIZ);
strcpy(tossFileName, cp);
strcat(tossFileName, dotTSS);

tmpDir = getenv("TMPDIR");
if (!tmpDir)
    tmpDir = TMPDIR;
sprintf(pid, "%d", getpid());
length = DOTSDLSIZ - 1 + strlen(tmpDir) + strlen(pid) + 1; 
tempFileName = malloc(length);
strcpy(tempFileName, tmpDir);
strcat(tempFileName, "/");
strcat(tempFileName, dotSDL + 1); /* skip the "." */
strcat(tempFileName, pid);
compFileName = malloc(length + 4);
strcpy(compFileName, tempFileName);
strcat(compFileName, "comp");
compZFileName = malloc(length + 4 + 2);
strcpy(compZFileName, compFileName);
strcat(compZFileName, ".Z");
sortedIdxFileName = malloc(length + 3);
strcpy(sortedIdxFileName, tempFileName);
strcat(sortedIdxFileName, "idx");
vstructFileName = malloc(length + 4);
strcpy(vstructFileName, tempFileName);
strcat(vstructFileName, "vstr");
}



/*
 * Some small and obvious utility routines for opening,
 * reading, writing and closing files.  Most have versions for both
 * file descriptor and stream operations.  When possible, the file
 * descriptor (unbuffered) versions are used for performance reasons.
 * Of course, when using the unbuffered versions we attempt to read in
 * BUFSIZ characters at a time.
*/
int OpenFile(char *name, int type, int code)
{
int fd;
int oflag, mode;

mode = 0;
if ((oflag = type) == O_WRONLY)
    {
    oflag |= O_CREAT;
    mode = 0666;
    }

if ((fd = open(name, oflag, mode)) == -1)
    {
    fprintf(stderr,
	    "%s: error opening \"%s\" for %s\n",
	    progName,
	    name,
	    (type == O_RDONLY) ? "reading" : "writing");
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: error opening \"%s\" for %s\n",
		progName,
		name,
		(type == O_RDONLY) ? "reading" : "writing");
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
return fd;
}


int ReadFile(int fd, char *name, char *buffer, int amount, int code)
{
int length;

if ((length = read(fd, buffer, amount)) == -1)
    {
    fprintf(stderr, "%s: error reading from \"%s\"\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile, "%s: error reading from \"%s\"\n", progName, name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
return length;
}


int WriteFile(int fd, char *name, char *buffer, int amount, int code)
{
int length;

if ((length = write(fd, buffer, amount)) == -1)
    {
    fprintf(stderr, "%s: error writing to \"%s\"\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile, "%s: error writing to \"%s\"\n", progName, name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
return length;
}


int FileExists(char *name)
{
if (access(name, F_OK) == 0) return 1;
return 0;
}


void AssertFileIsReadable(char *name, int code)
{
if (access(name, R_OK) == -1)
    {
    fprintf(stderr, "%s: cannot open \"%s\" for reading\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: cannot open \"%s\" for reading\n",
		progName,
		name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
}


int FileSize(char *name, int code)
{
struct stat buf;

if (stat(name, &buf) == -1)
    {
    fprintf(stderr, "%s: error getting size of \"%s\"\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: error getting size of \"%s\"\n",
		progName,
		name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }

return buf.st_size;
}


void CloseFile(int fd, char *name, int code)
{
if (close(fd) == -1)
    {
    fprintf(stderr, "%s: error closing \"%s\"\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile, "%s: error closing \"%s\"\n", progName, name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
}


FILE * FopenFile(char *name, int type, int code)
{
FILE *file;
char *mode;

if (type == O_RDONLY)
    mode = "r";
else if (type == O_WRONLY)
    mode = "w";
else if (type == (O_WRONLY|O_APPEND))
    mode = "a";
else
    {
    fprintf(stderr,
	    "%s: bad type (%d) in opening \"%s\"",
	    progName,
	    type,
	    name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: bad type (%d) in opening \"%s\"",
		progName,
		type,
		name);
	}
    exit(code);
    }

if ((file = fopen(name, mode)) == NULL)
    {
    fprintf(stderr,
	    "%s: error opening \"%s\" for %s\n",
	    progName,
	    name,
	    (*mode == 'r') ? "reading" :
		(*mode == 'w') ? "writing" : "appending");
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: error opening \"%s\" for %s\n",
		progName,
		name,
		(*mode == 'r') ? "reading" :
		    (*mode == 'w') ? "writing" : "appending");
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
return file;
}


int GetALine(FILE *file, char *name, char *line, int max, int code)
{
char *pc;
int   length;

pc = fgets(line, max, file);
if (!pc)
    {
    if (!ferror(file))
	{
	return 0;
	}
    else
	{
	fprintf(stderr,
		"%s: error getting a line from \"%s\"\n",
		progName,
		name);
	perror(name);
	if (m_errfile && (m_errfile != stderr))
	    {
	    fprintf(m_errfile,
		    "%s: error getting a line from \"%s\"\n",
		    progName,
		    name);
	    fprintf(m_errfile, "%s: %s", name, strerror(errno));
	    }
	exit(code);
	}
    }

length = strlen(pc);
if ((length >= (max - 1)) && (line[max-1] != '\n'))
    {
    fprintf(stderr,
	    "%s: line longer than %d bytes in file \"%s\"\n",
	    progName,
	    max,
	    name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile,
		"%s: line longer than %d bytes in file \"%s\"\n",
		progName,
		max,
		name);
	}
    exit(code);
    }
return length;
}


int FreadFile(FILE *file, char *name, char *buffer, int amount, int code)
{
int length;

if ((length = fread(buffer, sizeof(char), amount, file)) == -1)
    {
    fprintf(stderr, "%s: error reading from \"%s\"\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile, "%s: error reading from \"%s\"\n", progName, name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
return length;
}


void FwriteFile(FILE *file, char *name, char *buffer, size_t amount, int code)
{
if (fwrite(buffer, 1, amount, file) != amount)
    {
    fprintf(stderr, "%s: error writing to \"%s\"\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile, "%s: error writing to \"%s\"\n", progName, name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
}


void FcloseFile(FILE *file, char *name, int code)
{
if (fclose(file) != 0)
    {
    fprintf(stderr, "%s: error closing \"%s\"\n", progName, name);
    perror(name);
    if (m_errfile && (m_errfile != stderr))
	{
	fprintf(m_errfile, "%s: error closing \"%s\"\n", progName, name);
	fprintf(m_errfile, "%s: %s", name, strerror(errno));
	}
    exit(code);
    }
}


int StringToUpper(char *string)
{
char *pc, c;

pc = string;
while (c = *pc)
    {
    if (isalpha(c) && islower(c)) *pc = toupper(c);
    pc++;
    }
return pc - string;
}


/*
 * A routine to build the index from the .idx file (if any).
*/
void BuildIndex(void)
{
char  buffer[BUFSIZ+1], *pFrom, *pTo, *pRestart, thisChar;
int   length, size;
int   lineCount;
char  line[LINEMAX];
int   status;
int   remnant;
int   found;
int   sortedIdxFd;
FILE *sdlIdxFile;
static char openIndex[]  = "<INDEX COUNT=\"%d\">\n";
static char closeIndex[] = "</INDEX>\n";
static char openEntry[]  = "<ENTRY LOCS=\"";
static char closeEntry[] = "</ENTRY>\n";
char sort[BUFSIZ], text[BUFSIZ], rid[BUFSIZ];
char lastsort[BUFSIZ], lasttext[BUFSIZ], lastrid[BUFSIZ];
char locs[BUFSIZ];

/* skip the index building step if no .idx file or .idx file is empty */
if (!FileExists(idxFileName) || (FileSize(idxFileName, xxx) == 0))
    return;

AssertFileIsReadable(idxFileName, xxx);

haveIndex = TRUE;

sortedIdxFd = OpenFile(sortedIdxFileName, O_WRONLY, xxx);
CloseFile(sortedIdxFd, sortedIdxFileName, xxx);

buffer[0] = 0;
if (operantLocale)
    sprintf(buffer, "env LANG=%s ", operantLocale);

sprintf(buffer + strlen(buffer),
	"sort -f %s > %s",
	idxFileName,
	sortedIdxFileName);
if (status = system(buffer))
    {
    if (status == -1)
	{
	fprintf(stderr,
		"%s: error forking to execute \"%s\"\n",
		progName,
		buffer);
	perror(buffer);
	if (m_errfile && (m_errfile != stderr))
	    {
	    fprintf(m_errfile,
		    "%s: error forking to execute \"%s\"\n",
		    progName,
		    buffer);
	    fprintf(m_errfile, "%s: %s", buffer, strerror(errno));
	    }
	exit(1);
	}
    else
	{
	fprintf(stderr, "%s: error executing \"%s\"\n", progName, buffer);
	if (m_errfile && (m_errfile != stderr))
	    {
	    fprintf(m_errfile,
		    "%s: error executing \"%s\"\n",
		    progName,
		    buffer);
	    }
	exit(1);
	}
    }

/* get the number of index entries */
lineCount = 0;
sortedIdxFd = OpenFile(sortedIdxFileName, O_RDONLY, xxx);
lastsort[0]    = '\0';
lasttext[0]    = '\0';
lastrid[0]     = '\0';
locs[0]        = '\0';
buffer[BUFSIZ] = '\0';
size    = 0;
remnant = 0;
pFrom = buffer;
while (size ||
       ((size = ReadFile(sortedIdxFd,
			 sortedIdxFileName,
			 buffer+remnant,
			 BUFSIZ-remnant,
			 xxx)) > 0))
    {
    /* add in remnant in case we just got a new buffer load */
    size += remnant;
    remnant = 0;

    /* First split out the "sort", "text" and "rid" fields.  They are
       separated by an ASCII Record Separator (036) character */
    pTo = sort;
    pRestart = pFrom;
    found = 0;
    while (--size >= 0)
	{
	if ((*pTo++ = *pFrom++) == RS)
	    {
	    found = 1;
	    break;
	    }
	}
    if (!found)
	{
	size = 0;
	pTo = buffer;
	pFrom = pRestart;
	while (*pTo++ = *pFrom++);
	remnant = pTo - buffer - 1;
	pFrom = buffer;
	continue;
	}
    *(pTo-1) = '\0';

    pTo = text;
    found = 0;
    while (--size >= 0)
	{
	thisChar = *pFrom++;
	if (thisChar == RS)
	    {
	    found = 1;
	    *pTo  = '\0';
	    break;
	    }
	if ((thisChar == '<') || (thisChar == '&'))
	    *pTo++ = '&';
	*pTo++ = thisChar;
	}
    if (!found)
	{
	size = 0;
	pTo = buffer;
	pFrom = pRestart;
	while (*pTo++ = *pFrom++);
	remnant = pTo - buffer - 1;
	pFrom = buffer;
	continue;
	}

    pTo = rid;
    found = 0;
    while (--size >= 0)
	{
	if ((*pTo++ = *pFrom++) == '\n')
	    {
	    found = 1;
	    break;
	    }
	}
    if (!found)
	{
	size = 0;
	pTo = buffer;
	pFrom = pRestart;
	while (*pTo++ = *pFrom++);
	remnant = pTo - buffer - 1;
	pFrom = buffer;
	continue;
	}
    *(pTo-1) = '\0';

    /* if buffer is consumed, reset ourselves for the next buffer load */
    if (size <= 0)
	{
	size    = 0;
	remnant = 0;
	pFrom   = buffer;
	}

    if (strcmp(text, lasttext) == 0)
	{
	if (strcmp(rid, lastrid) != 0)
	    {
	    strcat(locs, " ");
	    strcat(locs, rid);
	    }
	}
    else
	{
	if (strlen(locs) != 0)
	    lineCount++;
	strcpy(locs, rid);
	}
    strcpy(lasttext, text);
    strcpy(lastsort, sort);
    strcpy(lastrid,  rid);
    }
if (strlen(locs) != 0)
    lineCount++;
CloseFile(sortedIdxFd, sortedIdxFileName, xxx);

sortedIdxFd = OpenFile(sortedIdxFileName, O_RDONLY, xxx);
unlink(sortedIdxFileName);
sdlIdxFile = FopenFile(sortedIdxFileName, O_WRONLY, xxx);

fprintf(sdlIdxFile, openIndex, lineCount);

lastsort[0]    = '\0';
lasttext[0]    = '\0';
lastrid[0]     = '\0';
locs[0]        = '\0';
buffer[BUFSIZ] = '\0';
size    = 0;
remnant = 0;
pFrom = buffer;
while (size ||
       ((size = ReadFile(sortedIdxFd,
			 sortedIdxFileName,
			 buffer+remnant,
			 BUFSIZ-remnant,
			 xxx)) > 0))
    {
    /* add in remnant in case we just got a new buffer load */
    size += remnant;
    remnant = 0;

    /* First split out the "sort", "text" and "rid" fields.  They are
       separated by an ASCII Record Separator (036) character */
    pTo = sort;
    pRestart = pFrom;
    found = 0;
    while (--size >= 0)
	{
	if ((*pTo++ = *pFrom++) == RS)
	    {
	    found = 1;
	    break;
	    }
	}
    if (!found)
	{
	size = 0;
	pTo = buffer;
	pFrom = pRestart;
	while (*pTo++ = *pFrom++);
	remnant = pTo - buffer - 1;
	pFrom = buffer;
	continue;
	}
    *(pTo-1) = '\0';

    pTo = text;
    found = 0;
    while (--size >= 0)
	{
	thisChar = *pFrom++;
	if (thisChar == RS)
	    {
	    found = 1;
	    *pTo  = '\0';
	    break;
	    }
	if ((thisChar == '<') || (thisChar == '&'))
	    *pTo++ = '&';
	*pTo++ = thisChar;
	}
    if (!found)
	{
	size = 0;
	pTo = buffer;
	pFrom = pRestart;
	while (*pTo++ = *pFrom++);
	remnant = pTo - buffer - 1;
	pFrom = buffer;
	continue;
	}

    pTo = rid;
    found = 0;
    while (--size >= 0)
	{
	if ((*pTo++ = *pFrom++) == '\n')
	    {
	    found = 1;
	    break;
	    }
	}
    if (!found)
	{
	size = 0;
	pTo = buffer;
	pFrom = pRestart;
	while (*pTo++ = *pFrom++);
	remnant = pTo - buffer - 1;
	pFrom = buffer;
	continue;
	}
    *(pTo-1) = '\0';

    /* if buffer is consumed, reset ourselves for the next buffer load */
    if (size <= 0)
	{
	size    = 0;
	remnant = 0;
	pFrom   = buffer;
	}

    if (strcmp(text, lasttext) == 0)
	{
	if (strcmp(rid, lastrid) != 0)
	    {
	    strcat(locs, " ");
	    strcat(locs, rid);
	    }
	}
    else
	{
	if (strlen(locs) != 0)
	    {
	    if (strcmp(lastsort, lasttext) != 0)
		{
		length = fprintf(sdlIdxFile,
				 "%s%s\" SORT=\"%s\">%s%s",
			         openEntry,
			         locs,
			         lastsort,
			         lasttext,
			         closeEntry);
		}
	    else
		{
		length = fprintf(sdlIdxFile,
			         "%s%s\">%s%s",
			         openEntry,
			         locs,
			         lasttext,
			         closeEntry);
		}
	    if (length < 0)
		{
		fprintf(stderr,
			"%s: error writing to \"%s\"\n",
			progName,
			sortedIdxFileName);
		perror(sortedIdxFileName);
		if (m_errfile && (m_errfile != stderr))
		    {
		    fprintf(m_errfile,
			    "%s: error writing to \"%s\"\n",
			    progName,
			    sortedIdxFileName);
		    fprintf(m_errfile,
			    "%s: %s",
			    sortedIdxFileName,
			    strerror(errno));
		    }
		exit(1);
		}
	    }
	strcpy(locs, rid);
	}
    strcpy(lasttext, text);
    strcpy(lastsort, sort);
    strcpy(lastrid,  rid);
    }
if (strlen(locs) != 0)
    {
    if (strcmp(lastsort, lasttext) != 0)
	{
	length = fprintf(sdlIdxFile,
			 "%s%s\" SORT=\"%s\">%s%s",
			 openEntry,
			 locs,
			 lastsort,
			 lasttext,
			 closeEntry);
	}
    else
	{
	length = fprintf(sdlIdxFile,
			 "%s%s\">%s%s",
			 openEntry,
			 locs,
			 lasttext,
			 closeEntry);
	}
    if (length < 0)
	{
	fprintf(stderr,
		"%s: error writing to \"%s\"\n",
		progName,
		sortedIdxFileName);
	perror(sortedIdxFileName);
	if (m_errfile && (m_errfile != stderr))
	    {
	    fprintf(m_errfile,
		    "%s: error writing to \"%s\"\n",
		    progName,
		    sortedIdxFileName);
	    fprintf(m_errfile,
		    "%s: %s",
		    sortedIdxFileName,
		    strerror(errno));
	    }
	exit(1);
	}
    }
CloseFile(sortedIdxFd, sortedIdxFileName, xxx);
FwriteFile(sdlIdxFile,
	   sortedIdxFileName,
	   closeIndex,
	   sizeof(closeIndex)-1,
	   xxx);
FcloseFile(sdlIdxFile, sortedIdxFileName, xxx);
}


static void HandleSNB(void)
{
int   oldOffset, newOffset;
FILE *snbFile;
int   outFd, inFd;
char  line[LINEMAX], buffer[BUFSIZ];
int   length, delta;
char *realInFileName;
static char openSNB[]  = "<SNB>\n";
static char closeSNB[] = "</SNB>\n";

if (decompressFlag)
    realInFileName = tempFileName;
else
    realInFileName = inFileName;

oldOffset = 0;
newOffset = 0;
inFd  = OpenFile(realInFileName,   O_RDONLY, xxx);
unlink(tempFileName);
outFd = OpenFile(tempFileName, O_WRONLY, xxx);

if (FileExists(snbFileName))
    {
    snbFile = FopenFile(snbFileName, O_RDONLY, xxx);
    while ((length = GetALine(snbFile,
			      snbFileName,
			      line,
			      LINEMAX,
			      xxx)) > 0)
	{
	if (isdigit(*line))
	    {
	    if (newOffset != 0)
		WriteFile(outFd,
			  tempFileName,
			  closeSNB,
			  sizeof(closeSNB)-1,
			  xxx);
	    newOffset = atoi(line);
	    delta = newOffset - oldOffset;
	    while (delta)
		{
		length = ReadFile(inFd,
				  realInFileName,
				  buffer,
				  MIN(BUFSIZ,delta),
				  xxx);
		WriteFile(outFd, tempFileName, buffer, length, xxx);
		delta -= length;
		}
	    WriteFile(outFd, tempFileName, openSNB, sizeof(openSNB)-1, xxx);
	    oldOffset = newOffset;
	    }
	else
	    WriteFile(outFd, tempFileName, line, length, xxx);
	}
    if (newOffset != 0)
	WriteFile(outFd, tempFileName, closeSNB, sizeof(closeSNB)-1, xxx);
    FcloseFile(snbFile, snbFileName, xxx);
    }
while (length = ReadFile(inFd, realInFileName, buffer, BUFSIZ, xxx))
    WriteFile(outFd, tempFileName, buffer, length, xxx);
CloseFile(inFd, realInFileName, xxx);
CloseFile(outFd, tempFileName, xxx);
}


/* calculate the size of the TOSS if only the used elements are
 * emitted
*/
static int RestrictedTossSize(void)
{
char       buffer[32];
ElementPtr pThis;
int        size;

/* in size computations, the constant "4" is for the leading space,
   the equals sign and the two quotation marks; the constant preceding
   the "4" is the length of the attribute name itself */
size = 16 /* strlen("<TOSS COUNT=\"\">\n"); */;
size += sprintf(buffer, "%d", nStyles);
pThis = pTossChain;
while (pThis)
    {
    switch (pThis->type)
	{
	case e_keystyle:
	    if (!pThis->u.u_keystyle.beenUsed)
		break;
	    size += 9 /* strlen("<KEYSTYLE") */;
	    if (pThis->u.u_keystyle.class)
		size += 5 + 4 + w_strlen(pThis->u.u_keystyle.class);
	    if (pThis->u.u_keystyle.ssi)
	        size += 3 + 4 + w_strlen(pThis->u.u_keystyle.ssi);
	    if (pThis->u.u_keystyle.rlevel)
	        size += 6 + 4 + w_strlen(pThis->u.u_keystyle.rlevel);
	    if (pThis->u.u_keystyle.phrase)
	        size += 6 + 4 + w_strlen(pThis->u.u_keystyle.phrase);
	    if (pThis->u.u_keystyle.srch_wt)
	        size += 7 + 4 + w_strlen(pThis->u.u_keystyle.srch_wt);
	    if (pThis->u.u_keystyle.enter)
	        size += 5 + 4 + w_strlen(pThis->u.u_keystyle.enter);
	    if (pThis->u.u_keystyle.exit)
	        size += 4 + 4 + w_strlen(pThis->u.u_keystyle.exit);
	    if (pThis->u.u_keystyle.pointsz)
	        size += 7 + 4 + w_strlen(pThis->u.u_keystyle.pointsz);
	    if (pThis->u.u_keystyle.set_width)
	        size += 9 + 4 + w_strlen(pThis->u.u_keystyle.set_width);
	    if (pThis->u.u_keystyle.color)
	        size += 5 + 4 + w_strlen(pThis->u.u_keystyle.color);
	    if (pThis->u.u_keystyle.xlfd)
	        size += 4 + 4 + w_strlen(pThis->u.u_keystyle.xlfd);
	    if (pThis->u.u_keystyle.xlfdi)
	        size += 5 + 4 + w_strlen(pThis->u.u_keystyle.xlfdi);
	    if (pThis->u.u_keystyle.xlfdb)
	        size += 5 + 4 + w_strlen(pThis->u.u_keystyle.xlfdb);
	    if (pThis->u.u_keystyle.xlfdib)
	        size += 6 + 4 + w_strlen(pThis->u.u_keystyle.xlfdib);
	    if (pThis->u.u_keystyle.typenam)
	        size += 7 + 4 + w_strlen(pThis->u.u_keystyle.typenam);
	    if (pThis->u.u_keystyle.typenami)
	        size += 8 + 4 + w_strlen(pThis->u.u_keystyle.typenami);
	    if (pThis->u.u_keystyle.typenamb)
	        size += 8 + 4 + w_strlen(pThis->u.u_keystyle.typenamb);
	    if (pThis->u.u_keystyle.typenamib)
	        size += 9 + 4 + w_strlen(pThis->u.u_keystyle.typenamib);
	    if (pThis->u.u_keystyle.style)
	        size += 5 + 4 + w_strlen(pThis->u.u_keystyle.style);
	    if (pThis->u.u_keystyle.spacing)
	        size += 7 + 4 + w_strlen(pThis->u.u_keystyle.spacing);
	    if (pThis->u.u_keystyle.weight)
	        size += 6 + 4 + w_strlen(pThis->u.u_keystyle.weight);
	    if (pThis->u.u_keystyle.slant)
	        size += 5 + 4 + w_strlen(pThis->u.u_keystyle.slant);
	    if (pThis->u.u_keystyle.special)
	        size += 7 + 4 + w_strlen(pThis->u.u_keystyle.special);
	    size += 2 /* strlen(">\n") */;
	    break;
	case e_headstyle:
	    if (!pThis->u.u_headstyle.beenUsed)
		break;
	    size += 10 /* strlen("<HEADSTYLE") */;
	    if (pThis->u.u_headstyle.class)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.class);
	    if (pThis->u.u_headstyle.ssi)
	        size += 3 + 4 + w_strlen(pThis->u.u_headstyle.ssi);
	    if (pThis->u.u_headstyle.rlevel)
	        size += 6 + 4 + w_strlen(pThis->u.u_headstyle.rlevel);
	    if (pThis->u.u_headstyle.phrase)
	        size += 6 + 4 + w_strlen(pThis->u.u_headstyle.phrase);
	    if (pThis->u.u_headstyle.srch_wt)
	        size += 7 + 4 + w_strlen(pThis->u.u_headstyle.srch_wt);
	    if (pThis->u.u_headstyle.orient)
	        size += 6 + 4 + w_strlen(pThis->u.u_headstyle.orient);
	    if (pThis->u.u_headstyle.vorient)
	        size += 7 + 4 + w_strlen(pThis->u.u_headstyle.vorient);
	    if (pThis->u.u_headstyle.placement)
	        size += 9 + 4 + w_strlen(pThis->u.u_headstyle.placement);
	    if (pThis->u.u_headstyle.headw)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.headw);
	    if (pThis->u.u_headstyle.stack)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.stack);
	    if (pThis->u.u_headstyle.flow)
	        size += 4 + 4 + w_strlen(pThis->u.u_headstyle.flow);
	    if (pThis->u.u_headstyle.pointsz)
	        size += 7 + 4 + w_strlen(pThis->u.u_headstyle.pointsz);
	    if (pThis->u.u_headstyle.set_width)
	        size += 9 + 4 + w_strlen(pThis->u.u_headstyle.set_width);
	    if (pThis->u.u_headstyle.color)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.color);
	    if (pThis->u.u_headstyle.xlfd)
	        size += 4 + 4 + w_strlen(pThis->u.u_headstyle.xlfd);
	    if (pThis->u.u_headstyle.xlfdi)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.xlfdi);
	    if (pThis->u.u_headstyle.xlfdb)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.xlfdb);
	    if (pThis->u.u_headstyle.xlfdib)
	        size += 6 + 4 + w_strlen(pThis->u.u_headstyle.xlfdib);
	    if (pThis->u.u_headstyle.typenam)
	        size += 7 + 4 + w_strlen(pThis->u.u_headstyle.typenam);
	    if (pThis->u.u_headstyle.typenami)
	        size += 8 + 4 + w_strlen(pThis->u.u_headstyle.typenami);
	    if (pThis->u.u_headstyle.typenamb)
	        size += 8 + 4 + w_strlen(pThis->u.u_headstyle.typenamb);
	    if (pThis->u.u_headstyle.typenamib)
	        size += 9 + 4 + w_strlen(pThis->u.u_headstyle.typenamib);
	    if (pThis->u.u_headstyle.style)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.style);
	    if (pThis->u.u_headstyle.spacing)
	        size += 7 + 4 + w_strlen(pThis->u.u_headstyle.spacing);
	    if (pThis->u.u_headstyle.weight)
	        size += 6 + 4 + w_strlen(pThis->u.u_headstyle.weight);
	    if (pThis->u.u_headstyle.slant)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.slant);
	    if (pThis->u.u_headstyle.special)
	        size += 7 + 4 + w_strlen(pThis->u.u_headstyle.special);
	    if (pThis->u.u_headstyle.l_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_headstyle.l_margin);
	    if (pThis->u.u_headstyle.r_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_headstyle.r_margin);
	    if (pThis->u.u_headstyle.t_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_headstyle.t_margin);
	    if (pThis->u.u_headstyle.b_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_headstyle.b_margin);
	    if (pThis->u.u_headstyle.border)
	        size += 6 + 4 + w_strlen(pThis->u.u_headstyle.border);
	    if (pThis->u.u_headstyle.vjust)
	        size += 5 + 4 + w_strlen(pThis->u.u_headstyle.vjust);
	    if (pThis->u.u_headstyle.justify)
	        size += 7 + 4 + w_strlen(pThis->u.u_headstyle.justify);
	    size += 2 /* strlen(">\n") */;
	    break;
	case e_formstyle:
	    if (!pThis->u.u_formstyle.beenUsed)
		break;
	    size += 10 /* strlen("<FORMSTYLE") */;
	    if (pThis->u.u_formstyle.class)
	        size += 5 + 4 + w_strlen(pThis->u.u_formstyle.class);
	    if (pThis->u.u_formstyle.ssi)
	        size += 3 + 4 + w_strlen(pThis->u.u_formstyle.ssi);
	    if (pThis->u.u_formstyle.rlevel)
	        size += 6 + 4 + w_strlen(pThis->u.u_formstyle.rlevel);
	    if (pThis->u.u_formstyle.phrase)
	        size += 6 + 4 + w_strlen(pThis->u.u_formstyle.phrase);
	    if (pThis->u.u_formstyle.srch_wt)
	        size += 7 + 4 + w_strlen(pThis->u.u_formstyle.srch_wt);
	    if (pThis->u.u_formstyle.colw)
	        size += 4 + 4 + w_strlen(pThis->u.u_formstyle.colw);
	    if (pThis->u.u_formstyle.colj)
	        size += 4 + 4 + w_strlen(pThis->u.u_formstyle.colj);
	    if (pThis->u.u_formstyle.pointsz)
	        size += 7 + 4 + w_strlen(pThis->u.u_formstyle.pointsz);
	    if (pThis->u.u_formstyle.set_width)
	        size += 9 + 4 + w_strlen(pThis->u.u_formstyle.set_width);
	    if (pThis->u.u_formstyle.color)
	        size += 5 + 4 + w_strlen(pThis->u.u_formstyle.color);
	    if (pThis->u.u_formstyle.xlfd)
	        size += 4 + 4 + w_strlen(pThis->u.u_formstyle.xlfd);
	    if (pThis->u.u_formstyle.xlfdi)
	        size += 5 + 4 + w_strlen(pThis->u.u_formstyle.xlfdi);
	    if (pThis->u.u_formstyle.xlfdb)
	        size += 5 + 4 + w_strlen(pThis->u.u_formstyle.xlfdb);
	    if (pThis->u.u_formstyle.xlfdib)
	        size += 6 + 4 + w_strlen(pThis->u.u_formstyle.xlfdib);
	    if (pThis->u.u_formstyle.typenam)
	        size += 7 + 4 + w_strlen(pThis->u.u_formstyle.typenam);
	    if (pThis->u.u_formstyle.typenami)
	        size += 8 + 4 + w_strlen(pThis->u.u_formstyle.typenami);
	    if (pThis->u.u_formstyle.typenamb)
	        size += 8 + 4 + w_strlen(pThis->u.u_formstyle.typenamb);
	    if (pThis->u.u_formstyle.typenamib)
	        size += 9 + 4 + w_strlen(pThis->u.u_formstyle.typenamib);
	    if (pThis->u.u_formstyle.style)
	        size += 5 + 4 + w_strlen(pThis->u.u_formstyle.style);
	    if (pThis->u.u_formstyle.spacing)
	        size += 7 + 4 + w_strlen(pThis->u.u_formstyle.spacing);
	    if (pThis->u.u_formstyle.weight)
	        size += 6 + 4 + w_strlen(pThis->u.u_formstyle.weight);
	    if (pThis->u.u_formstyle.slant)
	        size += 5 + 4 + w_strlen(pThis->u.u_formstyle.slant);
	    if (pThis->u.u_formstyle.special)
	        size += 7 + 4 + w_strlen(pThis->u.u_formstyle.special);
	    if (pThis->u.u_formstyle.l_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_formstyle.l_margin);
	    if (pThis->u.u_formstyle.r_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_formstyle.r_margin);
	    if (pThis->u.u_formstyle.t_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_formstyle.t_margin);
	    if (pThis->u.u_formstyle.b_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_formstyle.b_margin);
	    if (pThis->u.u_formstyle.border)
	        size += 6 + 4 + w_strlen(pThis->u.u_formstyle.border);
	    if (pThis->u.u_formstyle.vjust)
	        size += 5 + 4 + w_strlen(pThis->u.u_formstyle.vjust);
	    size += 2 /* strlen(">\n") */;
	    break;
	case e_frmtstyle:
	    if (!pThis->u.u_frmtstyle.beenUsed)
		break;
	    size += 10 /* strlen("<FRMTSTYLE") */;
	    if (pThis->u.u_frmtstyle.class)
	        size += 5 + 4 + w_strlen(pThis->u.u_frmtstyle.class);
	    if (pThis->u.u_frmtstyle.ssi)
	        size += 3 + 4 + w_strlen(pThis->u.u_frmtstyle.ssi);
	    if (pThis->u.u_frmtstyle.rlevel)
	        size += 6 + 4 + w_strlen(pThis->u.u_frmtstyle.rlevel);
	    if (pThis->u.u_frmtstyle.phrase)
	        size += 6 + 4 + w_strlen(pThis->u.u_frmtstyle.phrase);
	    if (pThis->u.u_frmtstyle.f_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_frmtstyle.f_margin);
	    if (pThis->u.u_frmtstyle.srch_wt)
	        size += 7 + 4 + w_strlen(pThis->u.u_frmtstyle.srch_wt);
	    if (pThis->u.u_frmtstyle.pointsz)
	        size += 7 + 4 + w_strlen(pThis->u.u_frmtstyle.pointsz);
	    if (pThis->u.u_frmtstyle.set_width)
	        size += 9 + 4 + w_strlen(pThis->u.u_frmtstyle.set_width);
	    if (pThis->u.u_frmtstyle.color)
	        size += 5 + 4 + w_strlen(pThis->u.u_frmtstyle.color);
	    if (pThis->u.u_frmtstyle.xlfd)
	        size += 4 + 4 + w_strlen(pThis->u.u_frmtstyle.xlfd);
	    if (pThis->u.u_frmtstyle.xlfdi)
	        size += 5 + 4 + w_strlen(pThis->u.u_frmtstyle.xlfdi);
	    if (pThis->u.u_frmtstyle.xlfdb)
	        size += 5 + 4 + w_strlen(pThis->u.u_frmtstyle.xlfdb);
	    if (pThis->u.u_frmtstyle.xlfdib)
	        size += 6 + 4 + w_strlen(pThis->u.u_frmtstyle.xlfdib);
	    if (pThis->u.u_frmtstyle.typenam)
	        size += 7 + 4 + w_strlen(pThis->u.u_frmtstyle.typenam);
	    if (pThis->u.u_frmtstyle.typenami)
	        size += 8 + 4 + w_strlen(pThis->u.u_frmtstyle.typenami);
	    if (pThis->u.u_frmtstyle.typenamb)
	        size += 8 + 4 + w_strlen(pThis->u.u_frmtstyle.typenamb);
	    if (pThis->u.u_frmtstyle.typenamib)
	        size += 9 + 4 + w_strlen(pThis->u.u_frmtstyle.typenamib);
	    if (pThis->u.u_frmtstyle.style)
	        size += 5 + 4 + w_strlen(pThis->u.u_frmtstyle.style);
	    if (pThis->u.u_frmtstyle.spacing)
	        size += 7 + 4 + w_strlen(pThis->u.u_frmtstyle.spacing);
	    if (pThis->u.u_frmtstyle.weight)
	        size += 6 + 4 + w_strlen(pThis->u.u_frmtstyle.weight);
	    if (pThis->u.u_frmtstyle.slant)
	        size += 5 + 4 + w_strlen(pThis->u.u_frmtstyle.slant);
	    if (pThis->u.u_frmtstyle.special)
	        size += 7 + 4 + w_strlen(pThis->u.u_frmtstyle.special);
	    if (pThis->u.u_frmtstyle.l_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_frmtstyle.l_margin);
	    if (pThis->u.u_frmtstyle.r_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_frmtstyle.r_margin);
	    if (pThis->u.u_frmtstyle.t_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_frmtstyle.t_margin);
	    if (pThis->u.u_frmtstyle.b_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_frmtstyle.b_margin);
	    if (pThis->u.u_frmtstyle.border)
	        size += 6 + 4 + w_strlen(pThis->u.u_frmtstyle.border);
	    if (pThis->u.u_frmtstyle.vjust)
	        size += 5 + 4 + w_strlen(pThis->u.u_frmtstyle.vjust);
	    if (pThis->u.u_frmtstyle.justify)
	        size += 7 + 4 + w_strlen(pThis->u.u_frmtstyle.justify);
	    size += 2 /* strlen(">\n") */;
	    break;
	case e_grphstyle:
	    if (!pThis->u.u_grphstyle.beenUsed)
		break;
	    size += 10 /* strlen("<GRPHSTYLE") */;
	    if (pThis->u.u_grphstyle.class)
	        size += 5 + 4 + w_strlen(pThis->u.u_grphstyle.class);
	    if (pThis->u.u_grphstyle.ssi)
	        size += 3 + 4 + w_strlen(pThis->u.u_grphstyle.ssi);
	    if (pThis->u.u_grphstyle.rlevel)
	        size += 6 + 4 + w_strlen(pThis->u.u_grphstyle.rlevel);
	    if (pThis->u.u_grphstyle.phrase)
	        size += 6 + 4 + w_strlen(pThis->u.u_grphstyle.phrase);
	    if (pThis->u.u_grphstyle.l_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_grphstyle.l_margin);
	    if (pThis->u.u_grphstyle.r_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_grphstyle.r_margin);
	    if (pThis->u.u_grphstyle.t_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_grphstyle.t_margin);
	    if (pThis->u.u_grphstyle.b_margin)
	        size += 8 + 4 + w_strlen(pThis->u.u_grphstyle.b_margin);
	    if (pThis->u.u_grphstyle.border)
	        size += 6 + 4 + w_strlen(pThis->u.u_grphstyle.border);
	    if (pThis->u.u_grphstyle.vjust)
	        size += 5 + 4 + w_strlen(pThis->u.u_grphstyle.vjust);
	    if (pThis->u.u_grphstyle.justify)
	        size += 7 + 4 + w_strlen(pThis->u.u_grphstyle.justify);
	    size += 2 /* strlen(">\n") */;
	    break;
	}
    pThis = pThis->pNext;
    }
size += 8 /* strlen("</TOSS>\n") */;

return size;
}


/* iterate over the LOIDS computing the offsets as we remove the old
 * LOIDS and INDEX and replace them with the new
*/ 
void IterateLoids(void)
{
int       replacing, length, incr, tmp;
int       loidsSize;
char      buffer[BUFSIZ];
LoidsPtr  pLoids;

/* write the loids out */
pLoids = loids.next;
loidsSize = sprintf(buffer, openLoids, nIds);
while (pLoids)
    {
    loidsSize += sprintf(buffer,
			 "<ID TYPE=\"%s\" RID=\"%s\"",
			 pLoids->type,
			 pLoids->rid);
    if (pLoids->rssi)
	loidsSize += sprintf(buffer, " RSSI=\"%s\"", pLoids->rssi);
    if (pLoids->rlevel)
	loidsSize += sprintf(buffer, " RLEVEL=\"%s\"", pLoids->rlevel);
    loidsSize += sprintf(buffer, " OFFSET=\"%ld\">\n", pLoids->offset);
    pLoids = pLoids->next;
    }
loidsSize += sizeof(closeLoids) - 1;

/* compute how much we are deleting from and adding to the <vstruct> */
replacing = endOfLOIDS - startOfLOIDS;
if (haveIndex)
    {
    if (startOfIndex != 0)
	replacing += (endOfIndex - startOfIndex);
    replacing -=  FileSize(sortedIdxFileName, xxx);
    }
if (minimalTossFlag)
    {
    if (startOfTOSS != 0)
	replacing += (endOfTOSS - startOfTOSS);
    replacing -= RestrictedTossSize();
    }
incr = length = loidsSize - replacing;

/* Iterate over the <loids>, updating the offsets by "incr" until no
 * change in the size of the <loids>.  The "incr" may be positive or
 * negative based on original sizes of the elements and their
 * replacements.
*/
while (incr)
    {
    loidsSize = sprintf(buffer, openLoids, nIds);
    pLoids = loids.next; /* first one is a dummy (simplified some logic) */
    while (pLoids)
	{
	pLoids->offset += incr;
	loidsSize += sprintf(buffer,
			     "<ID TYPE=\"%s\" RID=\"%s\" ",
		             pLoids->type,
		             pLoids->rid);
	if (pLoids->rssi)
	    loidsSize += sprintf(buffer, "RSSI=\"%s\" ", pLoids->rssi);
	if (pLoids->rlevel)
	    loidsSize += sprintf(buffer, "RLEVEL=\"%s\" ", pLoids->rlevel);
	loidsSize += sprintf(buffer, "OFFSET=\"%ld\">\n", pLoids->offset);
	pLoids = pLoids->next;
	}
    loidsSize += sizeof(closeLoids) - 1;
    tmp    = loidsSize - replacing;
    incr   = tmp - length;
    length = tmp;
    }
}


/* iterate compressing vstruct until we have a stable loids; return
 * the amount of padding needed in between the vstruct and the first
 * virpage to make everything work
*/
static void CompressVstruct(void)
{
#define COMPRESS_PAD_LENGTH 10
char      buffer[BUFSIZ], zTemp[4];
char     *pc;
int       length, delta, change, pad;
int       newZsize, oldZsize;
int       inFd, idxFd;
LoidsPtr  pLoids;
static char      padString[COMPRESS_PAD_LENGTH];

pc = padString;
length = COMPRESS_PAD_LENGTH;
while (--length >= 0)
  *pc++ = (char)0xff;

FcloseFile(outFile, compFileName, xxx);
newZsize = Compress(compFileName, compZFileName, TRUE) + 4;
change   = FileSize(compFileName, xxx) - newZsize;

pad = 0;
change += COMPRESS_PAD_LENGTH; /* compensate for adding pad on first round */
while (1)
    {
    oldZsize = newZsize;

    /* recompute loids taking compression into account */
    pLoids = loids.next; /* first one is a dummy (simplified some logic) */
    while (pLoids)
	{
	pLoids->offset = pLoids->offset - change + COMPRESS_PAD_LENGTH;
	pLoids = pLoids->next;
	}

    inFd    = OpenFile(compFileName, O_RDONLY, xxx);
    unlink(compFileName);
    outFile = FopenFile(compFileName, O_WRONLY, xxx);

    /* copy inFd to outFile up to start of <loids> */
    delta = startOfLOIDS;
    while (length = ReadFile(inFd,
			     compFileName,
			     buffer,
			     MIN(BUFSIZ,delta),
			     xxx))
	{
	FwriteFile(outFile, compFileName, buffer, length, xxx);
	delta -= length;
	}

    /* write out the start-tag for <loids> */
    fprintf(outFile, openLoids, nIds);

    /* and write out the new <loids> */
    pLoids = loids.next; /* first one is a dummy (simplified some logic) */
    while (pLoids)
	{
	fprintf(outFile,
		"<ID TYPE=\"%s\" RID=\"%s\" ",
		pLoids->type,
		pLoids->rid);
	if (pLoids->rssi)
	    fprintf(outFile, "RSSI=\"%s\" ", pLoids->rssi);
	if (pLoids->rlevel)
	    fprintf(outFile, "RLEVEL=\"%s\" ", pLoids->rlevel);
	fprintf(outFile, "OFFSET=\"%ld\">\n", pLoids->offset);
	pLoids = pLoids->next;
	}

    /* followed by the end-tag for <loids> */
    FwriteFile(outFile,
	       compFileName,
	       closeLoids,
	       sizeof(closeLoids)-1,
	       xxx);

    /* and skip over the old <loids> in inFd */
    lseek(inFd, endOfLOIDS, SEEK_SET);

    /* save new <loids> end */
    endOfLOIDS = ftell(outFile);

    /* copy the remainder of the vstruct and doc head+snb to outFile */
    while (length = ReadFile(inFd, compFileName, buffer, BUFSIZ, xxx))
	FwriteFile(outFile, compFileName, buffer, length, xxx);
    CloseFile(inFd, compFileName, xxx);
    FcloseFile(outFile, compFileName, xxx);

    newZsize = Compress(compFileName, compZFileName, TRUE) + 4;

    if (newZsize <= (oldZsize + pad))
	{
	pad += (oldZsize - newZsize);
	break;
	}

    change = oldZsize - newZsize;
    pad += COMPRESS_PAD_LENGTH;
    }

/* open the final output file */
if ((*outFileName == '-') && (outFileName[1] == '\0'))
    {
    outFile = stdout;
    outFileName = "<stdout>";
    }
else
    {
    outFile = FopenFile(outFileName, O_WRONLY, xxx);
    }

/* write out the magic number and the file size */
newZsize -= 4; /* remove the magic number and file size byte count */
zTemp[3] = newZsize % 256;
newZsize /= 256;
zTemp[2] = newZsize % 256;
newZsize /= 256;
zTemp[1] = newZsize % 256;
zTemp[0] = '\0';
FwriteFile(outFile, outFileName, zTemp, 4, xxx);

/* copy the compressed vstruct, doc head+snb to output */
inFd = OpenFile(compZFileName, O_RDONLY, xxx);
while (length = ReadFile(inFd, compZFileName, buffer, BUFSIZ, xxx))
    FwriteFile(outFile, outFileName, buffer, length, xxx);
CloseFile(inFd, compZFileName, xxx);
unlink(compZFileName);
unlink(compFileName);

/* pad the output to keep the loids happy */
while (pad >= 0)
    {
    FwriteFile(outFile,
	       outFileName,
	       padString,
	       MIN(pad, COMPRESS_PAD_LENGTH),
	       xxx);
    pad -= COMPRESS_PAD_LENGTH;
    }
}


/* insert the new loids and index then close the output file */
static void IncorporateVstructElements(void)
{
char      buffer[BUFSIZ];
int       length, delta;
int       inFd, idxFd;
char     *realOutFileName;
LoidsPtr  pLoids;
int       newEndOfLOIDS;

inFd = OpenFile(vstructFileName, O_RDONLY, xxx);
if (compressFlag)
    {
    realOutFileName = compFileName;
    outFile = FopenFile(compFileName, O_WRONLY, xxx);
    }
else
    {
    if ((*outFileName == '-') && (outFileName[1] == '\0'))
	{
	outFile = stdout;
	realOutFileName = "<stdout>";
	}
    else
	{
	realOutFileName = outFileName;
	outFile = FopenFile(outFileName, O_WRONLY, xxx);
	}
    }

/* copy inFd to outFile up to start of <loids> */
delta = startOfLOIDS;
while (length = ReadFile(inFd, inFileName, buffer, MIN(BUFSIZ,delta), xxx))
    {
    FwriteFile(outFile, realOutFileName, buffer, length, xxx);
    delta -= length;
    }

/* write out the start-tag for <loids> */
fprintf(outFile, openLoids, nIds);

/* and write out the new <loids> */
pLoids = loids.next; /* first one is a dummy (simplified some logic) */
while (pLoids)
    {
    fprintf(outFile,
	    "<ID TYPE=\"%s\" RID=\"%s\" ",
	    pLoids->type,
	    pLoids->rid);
    if (pLoids->rssi)
	fprintf(outFile, "RSSI=\"%s\" ", pLoids->rssi);
    if (pLoids->rlevel)
	fprintf(outFile, "RLEVEL=\"%s\" ", pLoids->rlevel);
    fprintf(outFile, "OFFSET=\"%ld\">\n", pLoids->offset + delta);
    pLoids = pLoids->next;
    }

/* followed by the end-tag for <loids> */
FwriteFile(outFile,
	   realOutFileName,
	   closeLoids,
	   sizeof(closeLoids)-1,
	   xxx);

/* save the new end of <loids> */
newEndOfLOIDS = ftell(outFile);

/* and skip over the old <loids> in inFd */
lseek(inFd, endOfLOIDS, SEEK_SET);

delta = endOfTOSS;
if (!minimalTossFlag) /* use the full <toss> from the document */
    {
    if (delta) /* an old <toss> is in the document, copy it to outFile */
	{
	delta -= endOfLOIDS;
	while (length = ReadFile(inFd,
				 inFileName,
				 buffer,
				 MIN(BUFSIZ,delta),
				 xxx))
	    {
	    FwriteFile(outFile, realOutFileName, buffer, length, xxx);
	    delta -= length;
	    }
	}
    }
else /* emit only those <toss> elements actually used in the document */
    {
    if (delta) /* an old <toss> is in the document, blow it away */
	lseek(inFd, delta, SEEK_SET);

    /* and emit the restricted <toss> */
    OpenNamedTag("TOSS", TRUE, TRUE);
    pElementChain = pTossChain;
    nElements     = nStyles;
    EmitSDL(NULL);
    CloseNamedTag("TOSS", TRUE);
    }

/* if we have an <lophrases>, copy it over to outFile */
if (endOfLOPhrases)
    {
    delta = endOfLOPhrases - startOfLOPhrases;
    while (length = ReadFile(inFd,
			     inFileName,
			     buffer,
			     MIN(BUFSIZ,delta),
			     xxx))
	{
	FwriteFile(outFile, realOutFileName, buffer, length, xxx);
	delta -= length;
	}
    }

/* if we have a new index, write it out to outFile */
if (haveIndex)
    {
    idxFd = OpenFile(sortedIdxFileName, O_RDONLY, xxx);
    while (length = ReadFile(idxFd, sortedIdxFileName, buffer, BUFSIZ, xxx))
	FwriteFile(outFile, realOutFileName, buffer, length, xxx);
    unlink(sortedIdxFileName);
    }

/* if we have a new index and there was an old index, blow it away */
if (endOfIndex && haveIndex)
    lseek(inFd, endOfIndex, SEEK_SET);

/*  update the end of <loids> to reflect new size */
endOfLOIDS = newEndOfLOIDS;

/* copy the remainder of the vstruct and doc head+snb to outFile */
while (length = ReadFile(inFd, vstructFileName, buffer, BUFSIZ, xxx))
    FwriteFile(outFile, realOutFileName, buffer, length, xxx);
CloseFile(inFd, vstructFileName, xxx);
unlink(vstructFileName);

if (compressFlag)
    CompressVstruct();

/* then copy the virpages over to outFile */
inFd = OpenFile(tempFileName, O_RDONLY, xxx);
while (length = ReadFile(inFd, tempFileName, buffer, BUFSIZ, xxx))
    FwriteFile(outFile, outFileName, buffer, length, xxx);
CloseFile(inFd, tempFileName, xxx);

FcloseFile(outFile, outFileName, xxx);
unlink(tempFileName);
}

/* Below is a modified version of m_cyclent() that returns a pointer
 * to the entity content rather than its value.  Returning a pointer
 * to the entity's content field allows it to be modified.
*/
/* Cyclent.c contains procedure m_cyclent(), callable by interface
   designers, to cycle through all defined entities, returning information
   about them */
static M_WCHAR *CycleEnt(LOGICAL init,
		         unsigned char *type,
		         M_WCHAR ***content,
		         unsigned char *wheredef)
{
static M_TRIE *current ;
static M_TRIE *ancestor[M_NAMELEN + 1] ;
static length = 0 ;
static M_WCHAR name[M_NAMELEN + 1] ;

if (init)
    {
    current = m_enttrie->data ;
    length = 0 ;
    }

if (length < 0) return(NULL) ;

while (current->symbol)
    {
    ancestor[length] = current ;
    name[length++] = current->symbol ;
    current = current->data ;
    }
name[length] = M_EOS ;

*type	  =  ((M_ENTITY *)  current->data)->type ;
*content  = &(((M_ENTITY *) current->data)->content) ;
*wheredef =  ((M_ENTITY *)  current->data)->wheredef ;

while (length >= 0)
    {
    if (current->next)
	{
	current = current->next ;
	break ;
	}
    length-- ;
    if (length < 0) break ;
    current = ancestor[length] ;
    }

return(name) ;
}

/* A routine to examine all defined entities looking for ones of type
 * M_SYSTEM.  When found, if the entity was defined in the interface
 * file and its name is "TOSSFILE", change its content to be the
 * current file name base plus ".tss".
*/
void UpdateTossFileName(void)
{
unsigned char type;
unsigned char wheredef;
M_WCHAR **content;
M_WCHAR  *name;
M_WCHAR  *newContent;
char     *toss_name = "TOSSFILE";
char     *mb_name;

name = CycleEnt(TRUE, &type, &content, &wheredef);
if (!name) return;

do  {
    if ((type == M_SYSTEM) && (wheredef == M_DELTDEF) && name && *content)
	{
	mb_name = MakeMByteString(name);
	if (strcmp(mb_name, toss_name) == 0)
	    *content = MakeWideCharString(tossFileName);
	m_free(mb_name, "multi-byte toss file entity name");
	}
    }
while (name = CycleEnt(FALSE, &type, &content, &wheredef));
}


/* A function that takes a language/charset pair and:
 *     if they are standard, leave them unchanged but get local
 *                           versions and setlocale(3) using those
 *     if they are local, setlocale(3) with them and replace them with
 *			  standard versions.
*/
void
SetLocale(M_WCHAR *pLang, M_WCHAR *pCharset)
{
const char  cString[] = "C";
_DtXlateDb  myDb = NULL;
char        myPlatform[_DtPLATFORM_MAX_LEN+1];
char        myLocale[256]; /* arbitrarily large */
char       *mb_lang;
char       *mb_charset;
char       *locale;
char       *lang;
char       *charset;
int         execVer;
int         compVer;
int         isStd;

if (!pLang && !pCharset)
    return;

mb_charset = NULL;
if (pLang)
    mb_lang = MakeMByteString(pLang);
else
    mb_lang = (char *) cString;
if (pCharset)
    mb_charset = MakeMByteString(pCharset);
  
strcpy(myLocale, mb_lang);
if (mb_charset && *mb_charset)
    {
    strcat(myLocale, ".");
    strcat(myLocale, mb_charset);
    }

if ((_DtLcxOpenAllDbs(&myDb) != 0) ||
    (_DtXlateGetXlateEnv(myDb,myPlatform,&execVer,&compVer) != 0))
    {
    fprintf(stderr,
            "Warning: could not open locale translation database.\n");
    if (m_errfile != stderr)
	fprintf(m_errfile,
		"Warning: could not open locale translation database.\n");
    if (mb_lang != cString)
	mb_free(&mb_lang);
    if (mb_charset)
	mb_free(&mb_charset);
    if (myDb != 0)
	_DtLcxCloseDb(&myDb);
    return;
    }

isStd = !_DtLcxXlateOpToStd(myDb,
		            "CDE",
		            0,
		            DtLCX_OPER_STD,
		            myLocale,
		            NULL,
		            NULL,
		            NULL,
		            NULL);

if (isStd)
    { /* already standard - get local versions and set locale */
    if (_DtLcxXlateStdToOp(myDb,
		           myPlatform,
		           compVer,
		           DtLCX_OPER_SETLOCALE,
		           myLocale,
		           NULL,
		           NULL,
		           NULL,
		           &locale))
	{
	fprintf(stderr,
		"Warning: could not translate CDE locale to local\n");
	if (m_errfile != stderr)
	    fprintf(m_errfile,
		    "Warning: could not translate CDE locale to local\n");
	_DtLcxCloseDb(&myDb);
	if (mb_lang != cString)
	    mb_free(&mb_lang);
	if (mb_charset)
	    mb_free(&mb_charset);
	return;
	}
    else
	{
	setlocale(LC_CTYPE, locale);
	operantLocale = mb_malloc(strlen(locale)+1);
	strcpy(operantLocale, locale);
	}
    }
else
    { /* already local - just set locale */
    setlocale(LC_CTYPE, myLocale);
    operantLocale = mb_malloc(strlen(myLocale)+1);
    strcpy(operantLocale, myLocale);
    }

if (mb_lang != cString)
    mb_free(&mb_lang);
if (mb_charset)
    mb_free(&mb_charset);
_DtLcxCloseDb(&myDb);
}


/* a routine to do all the things we want to do before actually
 * parsing the SDL document.
*/
void OpenDocument(void)
{
MakeFileNames();
m_errfile = FopenFile(errFileName, O_WRONLY|O_APPEND, xxx);

if (decompressFlag)
    {
    DecompressSDL();
    }
HandleSNB();

inFile    = FopenFile(tempFileName, O_RDONLY, xxx);

unlink(tempFileName);

outFile   = FopenFile(vstructFileName, O_WRONLY, xxx);
}


/* a routine to do all those things we want to do after parsing the
 * document such as doing the fixups in the vstruct
*/
void CloseDocument(void)
{
FcloseFile(outFile, tempFileName, xxx);
BuildIndex();
IterateLoids();
IncorporateVstructElements();
if (m_errcnt)
    exit(xxx);
exit(0);
}
