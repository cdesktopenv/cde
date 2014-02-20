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
// $TOG: DtSR_SearchResultsEntry.C /main/13 1998/04/17 11:42:06 mgreess $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include <assert.h>
#include <limits.h>
#include <string.h>

#include <sstream>

#include <StyleSheet/DocParser.h>
#include <StyleSheet/Resolver.h>
#include <StyleSheet/StyleSheet.h>
#include "Tml_TextRenderer.hh"

#include "config.h"
#include "Basic/Error.hh"
#include "Basic/FolioObject.hh"
#include "Basic/List.hh"
#include "Basic/Long_Lived.hh"
#include "Managers/MessageMgr.hh"
#include "Managers/StyleSheetMgr.hh"

#include "DtSR_SearchResultsEntry.hh"
#include "DtSR_SearchEngine.hh"

#include "TextParser.hh"

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#ifndef True
#define True	1
#endif
#ifndef False
#define False	0
#endif

DtSR_SearchResultsEntry::DtSR_SearchResultsEntry(const char* id,
						 const char* book,
						 const char* section,
						 int dbn, short language,
					UAS_Pointer<DtSR_SearchResults>)
	: UAS_SearchResultsEntry(id, book, section, Inv_Relevance),
	  f_dbn(dbn), f_language(language), f_zone(0)
{
    int i;
    for (i=0; i<=UAS_SearchZones::uas_all; i++)
	f_proximity[i] = 0;

    UAS_String url("mmdb:LOCATOR=");
    url = url + id;
    UAS_Pointer<UAS_Common> sec = UAS_Common::create(url);

    f_id = sec->locator();
}

DtSR_SearchResultsEntry::~DtSR_SearchResultsEntry()
{
}

UAS_Pointer<UAS_List<UAS_TextRun> >
DtSR_SearchResultsEntry::matches() const
{
    if (f_matches == 0) {
	// preserve logical constness
	((DtSR_SearchResultsEntry*)this)->f_matches
		= ((DtSR_SearchResultsEntry*)this)->create_matches();
    }

    return f_matches;
}

int
DtSR_SearchResultsEntry::unset_proximity(DtSR_SearchZones::uas_zones uas_zone)
{
    if (uas_zone > UAS_SearchZones::uas_all) { // range error
	return Beyond_Range;
    }
    f_zone &= ~(0x01<<uas_zone);
    return f_proximity[uas_zone] = 0;
}

int
DtSR_SearchResultsEntry::set_proximity(DtSR_SearchZones::uas_zones uas_zone,
					int proximity)
{
    if (uas_zone > UAS_SearchZones::uas_all) { // range error
	return Beyond_Range;
    }
    else if (f_zone & 0x01<<uas_zone) { // already set, reject
	return Conflict;
    }

    f_zone |= 0x01<<uas_zone;

    return f_proximity[uas_zone] = proximity;
}

int
DtSR_SearchResultsEntry::overlay_proximity(DtSR_SearchZones::uas_zones uas_zone,
						int proximity)
{
    if (uas_zone > UAS_SearchZones::uas_all) // range error
	return Beyond_Range;

    if (f_zone & (0x01<<uas_zone == 0))
	return set_proximity(uas_zone, proximity);

    return f_proximity[uas_zone] = (f_proximity[uas_zone] + proximity) / 2;
}

int
DtSR_SearchResultsEntry::get_proximity(DtSR_SearchZones::uas_zones uas_zone) const
{
    if (uas_zone > UAS_SearchZones::uas_all) { // range error
	return Beyond_Range;
    }
    else if ((f_zone & 0x01<<uas_zone) == 0) { // proximity not set yet
	return Unspecified;
    }

    return f_proximity[uas_zone];
}

UAS_Pointer<UAS_String>
DtSR_SearchResultsEntry::parse_abstract(const char* abs,
		UAS_Pointer<UAS_String> &id, UAS_Pointer<UAS_String> &book,
		UAS_Pointer<UAS_String> &section)
{

    char* abstract = (char *)abs; // logical constness

    assert( abstract && *abstract );

    if (abstract == NULL || *abstract == '\0')
	return NULL;

    char *p = abstract, *head = p;
    if ((p = strchr(p, '\t')) == NULL) {
	return NULL;
    }
    else {
	*p = '\0';
	id = new UAS_String(head);
	*p++ = '\t'; // essential to maintain logical constness
	
	head = p;
	if (*p == '\0' || (p = strchr(p, '\t')) == NULL) {
	    return NULL;
	}
	else {
	    *p = '\0';
	    book = new UAS_String(head);
	    *p++ = '\t'; // essential to maintain logical constness
	    head = p;
	    section = new UAS_String(p);
	}
    }

#ifdef DEBUG
    fprintf(stderr, "id=%s, book=%s, section=%s\n", (char*)*(UAS_String*)id,
		(char*)*(UAS_String*)book, (char*)*(UAS_String*)section);
#endif

    return section;
}

int
DtSR_SearchResultsEntry::section_in_abstract(char* abstract, const char* id)
{
    if (abstract == NULL || *abstract == '\0' || id == NULL || *id == '\0')
	return False;

    UAS_Pointer<UAS_String> section_id, book, section;

    if (parse_abstract(abstract, section_id, book, section) == (const int)0) {
#ifdef DEBUG
	fprintf(stderr, "(ERROR) parse_abstract failed\n");
#endif
	return False;
    }

    if (strcmp(id, (char*)*(UAS_String*)section_id))
	return False;
    else
	return True;
}

static unsigned int proximity2relevance(int prox)
{
    if (prox == 0)
	return DtSR_SearchResultsEntry::Utmost_Relevance;

    float inv_prox;

    if (prox == (int)INT_MAX)
	inv_prox = 0;
    else
	inv_prox = 1 / (float)prox; // 0 to 1;

    return (unsigned int)(inv_prox * DtSR_SearchResultsEntry::Utmost_Relevance);
}

unsigned int
DtSR_SearchResultsEntry::relevance()
{
    if ((int)f_relevance != Inv_Relevance)
	return f_relevance;

    int prox;
    if ((prox = get_proximity(DtSR_SearchZones::uas_all)) >= 0) {
	return f_relevance = proximity2relevance(prox);
    }

    int nzones = 0;
    unsigned int relevance = 0;
    for (int i=0; i<DtSR_SearchZones::uas_all; i++) {
	if ((prox = get_proximity((DtSR_SearchZones::uas_zones) i)) >= 0) {
	    relevance += proximity2relevance(prox);
	    nzones++;
	}
    }
    assert( nzones > 0 );
    return f_relevance = relevance / nzones;
}

UAS_Pointer<UAS_List<UAS_TextRun> >
DtSR_SearchResultsEntry::create_matches()
{

#ifdef DEBUG
    fprintf(stderr, "(DEBUG) UAS_Common is being created from id=\"%s\"\n",
							(char*)f_id);
#endif
    UAS_Pointer<UAS_Common> doc = UAS_Common::create(f_id);

#ifdef DEBUG
    fprintf(stderr,
		"(DEBUG) id=%s\n\t"
		"book_name=%s title=%s\n",
		(char*)(doc->id()),
		(char*)(doc->book_name()), (char*)(doc->title()));
#endif

#ifdef DUMP_NODES
    {
	ofstream out("OriginalText");
	out << (char *) doc->data();
    }
#endif

    mtry
	{
	    style_sheet_mgr().initOnlineStyleSheet(doc);
	}
//  catch_noarg (StyleSheetSyntaxError)
    mcatch_any()
	{
#ifdef JOE_HATES_THIS
	    message_mgr().error_dialog(
		(char*)UAS_String(CATGETS(Set_Messages, 39, "File a Bug")));
#else
	    throw(CASTEXCEPT Exception());
#endif
	}
    end_try;

    istringstream input((char *)doc->data());
    ostringstream output;
    
    mtry
	{
	    Tml_TextRenderer	renderer(output, f_search_res->search_zones());
	    Resolver resolver(*gPathTab, renderer);
	    DocParser docparser(resolver);

	    docparser.parse(input);
	}
    mcatch_any()
	{
	    ON_DEBUG(cerr << "DtSR_SearchResultsEntry::create_matches...exception thrown" << '\n' << flush);
	    rethrow;
	}
    end_try;

    char* text = (char*)output.str().c_str();
    *(text + output.str().size()) = '\0';

#ifdef DUMP_NODES
    {
	ofstream out("ParsedText");
	out << text;
    }
#endif

#ifdef DEBUG
    fprintf(stderr, "(DEBUG) stems=0x%p, count=%d\n",
			(char*)f_search_res->stems(f_dbn)->stems(),
			f_search_res->stems(f_dbn)->count());

    int n_of_stems = 0;
    for (; n_of_stems < f_search_res->stems(f_dbn)->count(); n_of_stems++) {
	fprintf(stderr, "(DEBUG) %dth stem = %s\n", n_of_stems,
			(f_search_res->stems(f_dbn)->stems())[n_of_stems]);
    }
#endif

    int stype = f_search_res->search_type();

    DtSrHitword* kwics = NULL;
    long n_kwics = 0;

    char* parseout = NULL;

    // hack! overwrite f_language, since austext's value is wrong
    // In future, the next lines should be removed.
    const char* lang = getenv("LANG");
    if (lang && !strncmp(lang, "ja", strlen("ja")))
	f_language = DtSrLaJPN;
    else
	f_language = DtSrLaENG;
	
    if (f_language == DtSrLaJPN) { // do not trust DtSearchHighlight!
	int count        = f_search_res->stems(f_dbn)->count();

	ostringstream stemsbuf;
	for (int i = 0; i < count; i++) {
	    stemsbuf << (f_search_res->stems(f_dbn)->stems())[i] << '\n';
	}
	char* stems = (char*)stemsbuf.str().c_str();
	*(stems + stemsbuf.str().size()) = '\0';

	parseout = StringParser::hilite(text, count, stems);

	assert( parseout != NULL );

	delete[] stems;
    }
    else {

	static DtSR_SearchEngine& search_engine = DtSR_SearchEngine::search_engine();
	if (DtSearchHighlight(
		search_engine.char_db_name(f_dbn),
		text, &kwics, &n_kwics, stype,
		(char*)f_search_res->stems(f_dbn)->stems(),
		f_search_res->stems(f_dbn)->count()) != DtSrOK) {

	    fprintf(stderr, "(ERROR) DtSearchHighlight failed\n");
#ifdef DEBUG
	    abort();
#endif
	}

#ifdef DEBUG
	fprintf(stderr, "(DEBUG) %ld hit found in %s\n", n_kwics, (char*)f_id);
#endif
    }

    UAS_Pointer<UAS_List<UAS_TextRun> >
				matches = new UAS_List<UAS_TextRun>;

    // convert kwics to textrun
    if (parseout == NULL && kwics) {
	ostringstream textrunbuf;
	for (int i = 0; i < n_kwics; i++)
	    textrunbuf << kwics[i].offset << '\t' << kwics[i].length << '\n';
	parseout = (char*)textrunbuf.str().c_str();
	*(parseout + textrunbuf.str().size()) = '\0';
    }
    else if (parseout == NULL)
    {
	if (text)
	    delete[] text;
	return matches;
    }

#ifdef DEBUG
    fprintf(stderr, "(DEBUG) byte offset and length\n%s", parseout);
#endif

    istringstream textruns(parseout);

    char linebuf[128];
    while (textruns.get(linebuf, 128, '\n')) {
	char newline;
	textruns.get(newline);
	assert( newline == '\n');

	char* off_str = linebuf;
	char* len_str = strchr(linebuf, '\t');
	assert( len_str && *len_str == '\t' );
	*len_str++ = '\0';

	int mode = True;

	const char* cursor = (const char*)text;
	assert( *cursor == ShiftIn || *cursor == ShiftOut );

	int off = atoi(off_str);
	int vcc = 0;
	
	while (off > 0) {

	    int scanned = 0;
	    if (*cursor == '\n' || *cursor == '\t' || *cursor == ' '  ||
		*cursor == 0x0D || (unsigned char)*cursor == 0xA0) {
		scanned++;
	    }
	    else if (*cursor == ShiftIn || *cursor == ShiftOut) {
		if (*cursor == ShiftIn)
		    mode = True;
		else
		    mode = False;
		scanned++;
	    }
	    else {
		scanned = mblen(cursor, MB_CUR_MAX);
		assert( scanned >= 0 );
		vcc++;
	    }

	    off -= scanned;
	    cursor += scanned;
	}

	if (mode == False)
	    continue;

	assert( off == 0 );

	int len = atoi(len_str);
	// remove leading white-spaces
	for (; len && (*cursor == ' ' || *cursor == '\t' ||
		       *cursor == '\n'|| *cursor == 0x0D); cursor++, len--);

	// remove trailing white-spaces
	if (len > 0) {
	    for (const char*  p = cursor + len - 1;
		 *p==' ' || *p=='\t' || *p=='\n' || *p==0x0D; p--, len--);
	}

	if (len == 0)
	    continue;

	int vlen = 0;
	for (; len > 0; vlen++) {
	    int scanned = mblen(cursor, MB_CUR_MAX);
	    assert( scanned >= 0 );
	    len -= scanned;
	    cursor += scanned;
	}

	UAS_Pointer<UAS_TextRun> textrun = new UAS_TextRun(vcc, vlen);
	matches->insert_item(textrun);
    }

    if (text)
	delete[] text;
    if (parseout)
	delete[] parseout;

    return matches;
}

UAS_Pointer<DtSR_SearchResults>
DtSR_SearchResultsEntry::search_result(UAS_Pointer<DtSR_SearchResults>& res)
{
    UAS_Pointer<DtSR_SearchResults> rval = f_search_res;

    f_search_res = res;

    return rval;
}

void
DtSR_SearchResultsEntry::unreference()
{
    static int nest = 0;
    if (nest++ == 0)
	UAS_Base::unreference();
    nest--;
}
