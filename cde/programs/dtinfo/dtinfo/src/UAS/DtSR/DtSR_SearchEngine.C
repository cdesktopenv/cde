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
// $TOG: DtSR_SearchEngine.C /main/19 1998/04/17 11:41:48 mgreess $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
using namespace std;

#include "Registration.hh"
#include "Managers/CatMgr.hh"

#include "Util_Classes/Dict.hh"
#include "Util_Classes/DictIter.hh"

#include "UAS_Factory.hh"
#include "UAS_Exceptions.hh"
#include "UAS_Collection.hh"
#include "UAS_ObjList.hh"
#include "DtSR_SearchEngine.hh"
#include "DtSR_BookcaseEntry.hh"
#include "DtSR_SearchResultsEntry.hh"

enum fine_scopes { scope_section, scope_book };

#ifndef True
#define True	1
#endif
#ifndef False
#define False	0
#endif

DtSR_SearchEngine *DtSR_SearchEngine::f_search_engine = NULL;

int DtSR_SearchEngine::f_init_count = 0;

// interface to DtSearchEngine constructor
DtSR_SearchEngine &
DtSR_SearchEngine::search_engine(UAS_PtrList<const char> *bcases)
{
    if (f_search_engine == (DtSR_SearchEngine  *)NULL) {
	// constructor does only what it really needs to do.
	// let init do the rest of initialization.
	f_search_engine = new DtSR_SearchEngine();
    }

    // init was originally introduced to prevent circular instantiation
    // But now we need init anyway to implement re-initialization
    if (bcases != NULL && bcases->numItems() > 0)
	f_search_engine->init(bcases);

    return *f_search_engine;
}

#ifndef KOSHER

// build UAS_Common object give a mmdb section id
static UAS_Pointer<UAS_Common>
uas_common(const UAS_String id)
{
    UAS_String url = "mmdb:LOCATOR=";

    url = url + id; 

    UAS_Pointer<UAS_Common> doc = UAS_Common::create(url);

    return doc;
}

// build url given mmdb section id
static UAS_String
build_url(const UAS_String id)
{
    UAS_Pointer<UAS_Common> doc = uas_common(id);
    UAS_String url(doc->locator());
#ifdef DEBUG
    fprintf(stderr, "(DEBUG) URL=\"%s\"\n", (char*)url);
#endif

    return url;
}

static UAS_String
build_bc_url(UAS_String bookcase_path)
{
    const char* str = (char*)bookcase_path;
    if (str == NULL || *str == '\0')
	return UAS_String("");

    char* bc_path = strdup((char*)bookcase_path);
    char* sep = bc_path + strlen(bc_path) - 1;
    for (; sep > bc_path && *sep != '/'; --sep);
    
    if (sep == bc_path) {
	free(bc_path);
	return UAS_String("");
    }

    *sep++ = '\0';
    UAS_String bc_locator = "mmdb:INFOLIB=";
    bc_locator = bc_locator + bc_path + "&BOOKCASE=" + sep;
#ifdef DEBUG
    fprintf(stderr, "(DEBUG) URL=\"%s\"\n", (char*)bc_locator);
#endif

    if (bc_path)
	free(bc_path);

    return bc_locator;
}

#endif

static int
IsAccessible(const char* path, mode_t type = S_IFREG)
{
    if (path == NULL || *path == '\0')
	return False;

    struct stat stat_buf;

    if (stat(path, &stat_buf) < 0) {
#ifdef DEBUG
	fprintf(stderr, "(ERROR) cannot stat %s\n", (char*)path);
#endif
	return False;
    }
    else if (type == S_IFDIR) {
	if ((stat_buf.st_mode & S_IFMT & S_IFDIR) == 0) {
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) %s not a directory\n",
							(char*)path);
#endif
	    return False;
	}
    }
    else {
	assert( type == S_IFREG );
	if ((stat_buf.st_mode & S_IFMT & S_IFREG) == 0) {
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) %s not a ordinary file\n",
							(char*)path);
#endif
	    return False;
	}
    }

    if (access((char*)path, R_OK) < 0) {
#ifdef DEBUG
	fprintf(stderr, "(ERROR) you do not have right to read %s\n",
								(char*)path);
#endif
	return False;
    }
    return True;
}

static unsigned int
UpdateConfigFile(UAS_PtrList<const char> *bcases, UAS_String ocf_path)
{
    if (bcases == NULL || bcases->numItems() == 0)
	return 0;

    if ((char*)ocf_path == NULL || *(char*)ocf_path == '\0')
	return 0;

    ofstream dtiocf((char*)ocf_path,
			ios::out | ios::trunc);
    if (! dtiocf) { // could not open ocf_path in specified mode
#ifdef DEBUG
	fprintf(stderr, "(ERROR) could not open %s\n", (char*)ocf_path);
#endif
	return 0;
    }

    unsigned int bitfield = 0;

    // dbnames_dict is used to ensure uniqueness of dbnames
    // NOTE: dbnames should really be ensured to be unique
    //       at infolib build-time.
    Dict<UAS_String, int> dbnames_dict(NULL, False);

    for (int i = 0; i < bcases->numItems(); i++) {
	UAS_String bcase_path = (*bcases)[i];
	UAS_String dtsrpath   = bcase_path + "/dtsearch";
	UAS_String ocfpath    = dtsrpath + "/dtsearch.ocf";

	char* bcase = (char*)bcase_path + bcase_path.length();
	for (; *bcase != '/' && bcase > (char*)bcase_path; bcase--);
	++bcase;

	// check if austext files are in place
	if (IsAccessible((char*)bcase_path, S_IFDIR) != True ||
	    IsAccessible((char*)dtsrpath, S_IFDIR) != True ||
	    IsAccessible((char*)ocfpath) != True ||
	    dbnames_dict[bcase] == True) // name has been used already
	{
	    bitfield &= ~(0x01 << i);
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) search files are not installed "
				"correctly in %s\n", (char*)bcase_path);
#endif
	    continue;
	}
	else {
	    dbnames_dict[bcase] = True;
	    bitfield |= 0x01 << i;
	}

	// bcase : book case name; e.g. "OLIASDOC"
	dtiocf << "PATH " << bcase <<  " = " << (char*)dtsrpath << '\n';

	// append the contents of "dtsearch.ocf" to dtinfo's ocf
	ifstream dtsrocf((char*)ocfpath);
	char buf[256]; // buffer to get a line from "dtsearch.ocf"
	while (dtsrocf.get(buf, 256, '\n')) {
	    dtsrocf.ignore(1);
	    dtiocf << buf << '\n';
	}
	dtsrocf.close();
    }

    dtiocf.close();

    return bitfield;
}

DtSR_SearchEngine::~DtSR_SearchEngine()
{
    if (*(char*)f_config_path != '\0') {
	unlink((char*)f_config_path);
#ifdef DEBUG
	fprintf(stderr, "(DEBUG) %s removed\n", (char*)f_config_path);
#endif
	f_config_path = NULL;
    }
    if (f_oql_parser != NULL) {
	delete f_oql_parser;
	f_oql_parser = NULL;
    }
}

// DtSearchEngine constructor
DtSR_SearchEngine::DtSR_SearchEngine()
    : f_dbnames(NULL), f_dbcount(0), f_valid_bc_map(0)
{
    string ocf_tmpl = "/tmp/.DtSR_XXXXXX";
    int len = ocf_tmpl.size();
    char* ocf_path = new char [len + 1];
    *((char *) memcpy(ocf_path, ocf_tmpl.c_str(), len) + len) = '\0';
    ocf_path = mktemp(ocf_path);
    f_config_path = ocf_path;

    if (ocf_path)
	free(ocf_path);

    ofstream dtiocf((char*)f_config_path, ios::out);  // TODO
    if (! dtiocf) // could not open ocf_path in specified mode
	throw(CASTEXCEPT Exception());

    f_oql_parser = new DtSR_Parser();
}

void
DtSR_SearchEngine::init(UAS_PtrList<const char> *bcases)
{
    if (bcases->numItems() == 0 || *(char*)f_config_path == '\0')
	return;

    if (bcases->numItems() ==
		(int) DtSR_BookcaseSearchEntry::bcases().length()) {
	// if sets of bookcases are same, you do not have to (re)init
    }

    if ((f_valid_bc_map = UpdateConfigFile(bcases, f_config_path)) == 0) {
	// no valid bookcases available
	return;
    }

    int status;

    if (f_init_count == 0) {
#ifdef DEBUG
	fprintf(stderr, "(DEBUG) DtSearch is being initialized.\n");
#endif
	status = DtSearchInit((char*)"DtSearch", NULL, 0, f_config_path,
					NULL, &f_dbnames, &f_dbcount);
    }
    else {
#ifdef DEBUG
	fprintf(stderr, "(DEBUG) DtSearch is being re-initialized.\n");
#endif
	status = DtSearchReinit(&f_dbnames, &f_dbcount);
    }

    UAS_String msg;
    if (status == DtSrOK || status == DtSrREINIT) {
	if (f_init_count++ == 0) {
	    DtSearchSetMaxResults(INT_MAX);
	    assert( DtSearchGetMaxResults() == INT_MAX );
	}
    }
    else { // DtSearch (re)initialization failed

	if (f_init_count == 0) {
	    msg = "(ERROR) DtSearchInit:\n";
	}
	else {
	    msg = "(ERROR) DtSearchReinit:\n";
	}

	msg = msg + DtSearchGetMessages();
	DtSearchFreeMessages();

	// zero f_valid_bc_map should represent bad condition
	f_valid_bc_map = 0;
    }

    // make DtSR_BookcaseSearchEntry::bcases() empty
    while (DtSR_BookcaseSearchEntry::bcases().length() > 0)
	DtSR_BookcaseSearchEntry::bcases().remove_item(0);

    if (f_valid_bc_map == 0) {
#if 0
	throw(CASTUASEXCEPT UAS_Exception(msg));
#else
	return;
#endif
    }

    int i;
#ifdef DEBUG
    fprintf(stderr, "(DEBUG) valid_bc_map = 0x%x\n", f_valid_bc_map);
    fprintf(stderr, "\tdbcount = %d\n", f_dbcount);
    for (i = 0; i < f_dbcount; i++)
	fprintf(stderr, "\tdb[%d]=%s\n", i, f_dbnames[i]);
#endif

    int dbn = 0;
    for (i = 0; i < bcases->numItems(); i++) {
	if ((f_valid_bc_map & (0x01 << i)) == 0)
	    continue;
	else {
	    UAS_String bookcase_path((*bcases)[i]);
	    UAS_String bookcase_url = build_bc_url(bookcase_path);
	    if ((char*)bookcase_url == NULL || *(char*)bookcase_url == '\0')
		continue;

	    UAS_Pointer<UAS_Common> obj;
	    if ((obj = UAS_Factory::create(bookcase_url)) == (int)0)
		continue;
	    UAS_Pointer<UAS_Common> bookcase;
	    if ((bookcase =
		    ((UAS_Collection*)(UAS_Common *)obj)->root()) == (int)0)
		continue;

	    new DtSR_BookcaseSearchEntry(dbn++, bookcase, True);
	}
    }
}

#ifndef KOSHER

// parse url (mmdb) out into infolib,infobase,id fields
static const char*
parse_url(char* locator, const char* &infolib, const char* &infobase,
		const char* &id)
{
    if (locator == NULL || *locator == '\0')
	return NULL;

    char *head, *tail;

    if ((head = strchr(locator, '=')) == NULL)
	return NULL;
    if ((tail = strchr(++head, '&')) == NULL)
	return NULL;
    assert( head < tail );
    *tail = '\0';
    infolib = head;

    head = tail + 1;
    if ((head = strchr(head, '=')) == NULL)
	return NULL;
    if ((tail = strchr(++head, '&')) == NULL)
	return NULL;
    assert( head < tail );
    *tail = '\0';
    infobase = head;

    head = tail + 1;
    if ((head = strchr(head, '=')) == NULL)
	return NULL;
    if ((tail = strchr(++head, '&')) == NULL)
	return NULL;
    assert( head < tail );
    *tail = '\0';
    id = head;

    return id;
}

#endif

typedef int (*Scan_Abstract)(char*, const char*);

static DtSrResult*
apply_scope(DtSrResult* res, fine_scopes scope, const char* id, long &count)
{
    if (res == NULL || id == NULL || *id == '\0') // garbage input
	return NULL;

    Scan_Abstract scan_abstract = DtSR_SearchResultsEntry::section_in_abstract;

    DtSrResult *rval = NULL, *tail = NULL, *iter;

    for (iter = res, count = 0; iter; iter = iter->link) {
	if (scan_abstract(iter->abstractp, id)) {
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) match found in scan_abstract\n");
#endif
	    DtSrResult *item = (DtSrResult *)malloc(sizeof(DtSrResult));
	    *item = *iter; // copy as it is
	    int len = strlen(iter->abstractp);
	    item->abstractp = (char*)malloc(len + 1);
	    *((char *) memcpy(item->abstractp,
			      iter->abstractp, len) + len) = '\0';
	    item->link = NULL;
	    if (rval == NULL)
		rval = tail = item;
	    else {
		tail->link = item;
		tail = item;
	    }
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) abstract=%s\n", item->abstractp);
#endif

	    count++;

	    if (scope == scope_section)
		break;
	}
    }

    return rval;
}

// resolve bookid (NOTE: serial is 1-based)
static UAS_String
resolve_bookid(UAS_Pointer<UAS_Common> &bcase, int serial)
{
    UAS_String rval;

    if (bcase == (int)0 || bcase->type() != UAS_BOOKCASE)
	return rval;

    if (serial < 1) // apparently wrong serial number
	return rval;

    UAS_Pointer<UAS_Common> book = bcase->children()[serial - 1];
#ifdef DEBUG
    fprintf(stderr, "(DEBUG) resolved bookid=%s, type=%s\n",
			(char*)book->id(), (char*)book->content_type());
#endif

    return rval = book->id();
}

UAS_Pointer<UAS_SearchResults>
DtSR_SearchEngine::search(UAS_String oql, UAS_SearchScope& scope,
					    unsigned int /* maxdocs */)
{
    UAS_Pointer<DtSR_SearchResults> DtSR_result = NULL ;
    UAS_Pointer<UAS_SearchResults>  UAS_result = NULL;

    if (f_valid_bc_map == 0) // DtSearch initialization failed
	return UAS_result;

    UAS_PtrList<UAS_BookcaseEntry>& targets = scope.bookcases();
    if (scope.search_zones().section() == False && targets.numItems() == 0) {
	// no bookcases to search against
	UAS_result = new UAS_SearchResults(new UAS_String(oql),
						new UAS_String(scope.name()));
#ifdef DEBUG
	fprintf(stderr, "(WARNING) no bookcases specified to search against, "
			"returning an empty search result...\n");
#endif
	return UAS_result;
    }

    UAS_Pointer<UAS_BookcaseEntry> current_bc = NULL;
    if (scope.search_zones().section()) { // search for current section

      if (targets.numItems() == 0) {

	UAS_String url = build_url(scope.search_zones().search_section());

	const char *infolib, *infobase, *section_id;
	if (parse_url((char*)url, infolib, infobase, section_id) == NULL) {
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) could not parse url\n");
	    abort();
#endif
	    // NOTE: parse_url is tampering url,
	    //	     that's why it's cast to (char*)
	    throw(CASTEXCEPT Exception());
	}

	UAS_String bc_path = infolib;
	bc_path = bc_path + "/" + infobase;

	UAS_String bookcase_url = build_bc_url(bc_path);
	assert( (char*)bookcase_url && *(char*)bookcase_url );

	UAS_Pointer<UAS_Common> obj = UAS_Factory::create(bookcase_url);
	UAS_Pointer<UAS_Common> bookcase =
			((UAS_Collection*)(UAS_Common *)obj)->root();

	// NOTE: hack! tampering bookcases
	current_bc = new UAS_BookcaseEntry(bookcase, True);
	targets.append(current_bc);

      }
#ifdef DEBUG
      else
	assert( targets.numItems() == 1 );
#endif

	scope.search_zones().all(True); // hack! tampering zones
    }

    UAS_String aus_query;
    mtry {
	aus_query = f_oql_parser->parse((char*)oql);
    }
    mcatch_any() { // OQL parse failed
	rethrow;
    }
    end_try;

    DtSR_BookcaseSearchEntry::search_zones(scope.search_zones());

    // do search for each bookcase
    UAS_List<DtSR_BookcaseSearchEntry>& bookcases =
				DtSR_BookcaseSearchEntry::bcases();
    int n;
    DtSrResult* DtSr_res = NULL;
    long rescount = 0;
    // for each bookcase specified in scope
    for (n = 0; n < targets.numItems(); n++, DtSr_res = NULL, rescount = 0) {
	int index;
	// look for the correspondent index
	for (index = 0; index < f_dbcount; index++) {
	    if (bookcases[index]->bid() == targets[n]->bid() &&
		bookcases[index]->lid() == targets[n]->lid())
		break;
	}
	if (index == f_dbcount) {
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) cannot not find bookcase, bid=\"%s\", "
				"just ignore\n", (char*)targets[n]->bid());
#endif
	    continue;
	}

	bookcases[index]->stems()->clear();

#if 1
	// switch austext search option based on completion being specified
	int stype = ((DtSR_Parser*)f_oql_parser)->stemming_suggested() ? 'S' : 'W';
#else
	// switch austext search option with regards to languages
	int stype = (bookcases[index]->language() == DtSrLaJPN)? 'W' : 'S';
#endif

	UAS_String eff_query = aus_query;
#ifdef DEBUG
	fprintf(stderr, "(DEBUG) effective query=\"%s\"\n", (char*)eff_query);
#endif

	int status = DtSearchQuery(
			(char*)eff_query, f_dbnames[index], stype, NULL, NULL,
			&DtSr_res, &rescount,
			(char*)(bookcases[index]->stems()->stems()),
			&(bookcases[index]->stems()->count())
		     );

	if (status != DtSrOK && status != DtSrNOTAVAIL) { // error

	    if (DtSr_res)
		DtSearchFreeResults(&DtSr_res);

	    UAS_String msg(CATGETS(Set_DtSR_SearchEngine, 1,
				"DtSearch does not support the query."));
	    DtSearchFreeMessages();

	    if (current_bc != 0) {
		targets.remove(current_bc);
		current_bc = NULL;
	    }
	    throw(CASTUASEXCEPT UAS_Exception(msg));

	    continue;
	}

	if (scope.search_zones().section()) {

	    DtSrResult* res = apply_scope(DtSr_res, scope_section,
			(char*) scope.search_zones().search_section(),
			rescount);

	    if (DtSr_res)
		DtSearchFreeResults(&DtSr_res);

	    DtSr_res = res; // replace the results with an artifact
	}

	if (DtSr_res == NULL)
	    continue;

	UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > res;

	if ((res = compress_DtSrResult(DtSr_res, rescount)) == (int)0)
	    continue;

	// book#s specified, apply book-level scope here
	if (targets[n]->book_list().numItems() > 0 && res->length() > 0) {
	    Dict<UAS_String, int> bookid_dict(NULL, False);
	    UAS_ObjList<int> &books = targets[n]->book_list();
	    // register bookids in Dict<UAS_String, int>
	    int i;
	    for (i = 0; i < books.numItems(); i++) {
		UAS_Pointer<UAS_Common> bcase(bookcases[index]->bcase());
		UAS_String bookid = resolve_bookid(bcase, books[i]);
		if ((char*)bookid == (int)NULL || *(char*)bookid == '\0')
		    continue;

		bookid_dict[bookid] = True;
	    }
	    for (i = 0; i < (int) res->length(); i++) {

		// all these temporary variables are needed to get this 
		// code to compile on novell

		UAS_List<UAS_SearchResultsEntry> * temp_lst = res;
		
		UAS_Pointer<UAS_SearchResultsEntry> tmp_sre = temp_lst->item(i);

		UAS_String temp_id = tmp_sre->id();

		UAS_Pointer<UAS_Common> uas_book =
				UAS_Common::create(temp_id);

		while (uas_book->type() != UAS_BOOK)
		    uas_book = uas_book->parent();

		UAS_String uas_book_id = uas_book->id();

		if (bookid_dict[uas_book_id] == False)
		    res->set_item(NULL, i);
	    }

	    for (i = 0; i < (int) res->length(); i++) {
		if (res->item(i) == (int)0)
		    res->remove_item(i--);
	    }
	}

	// take over stems from DtSR_BookcaseSearchEntry
	UAS_Pointer<DtSR_Stems> stems = bookcases[index]->takeover_stems();

	UAS_Pointer<UAS_String> q = new UAS_String(oql);
	UAS_Pointer<UAS_String> s = new UAS_String(scope.name());

	UAS_Pointer<DtSR_SearchResults> DtSR_res =
			new DtSR_SearchResults(q, s, res, res->length(),
					stems, scope.search_zones(), stype);

	if (DtSR_result == (int)0)
	    DtSR_result = DtSR_res;
	else  // merge uas_res into result
	    DtSR_result->merge(DtSR_res);
    }

    if (DtSR_result == (int)0)
	UAS_result = new UAS_SearchResults(new UAS_String(oql),
					new UAS_String(scope.name()));


    if (UAS_result == (int)0) {
	assert( DtSR_result != (int)0 );
	UAS_result = (UAS_SearchResults*)(DtSR_SearchResults*)DtSR_result;
    }

    assert( UAS_result != (int)0 );

    if (current_bc != 0) {
	targets.remove(current_bc);
	current_bc = NULL;
    }

    return UAS_result;
}

UAS_Pointer<UAS_List<UAS_SearchResultsEntry> >
DtSR_SearchEngine::compress_DtSrResult(DtSrResult*& res, long& count)
{
    UAS_Pointer<UAS_List<UAS_SearchResultsEntry> >
	result_list = new UAS_List<UAS_SearchResultsEntry>;
	
    // garbage inputs
    if (res == NULL) {
	assert( count == 0 );
	return result_list; // return empty list
    }
    else if (! count) { // should never enter here
	DtSearchFreeResults(&res);
	assert( res == NULL );
	return result_list; // return empty list
    }

    Dict<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
						map(NULL, NULL);

    DtSrResult* iter = res;
    for (iter = res; iter; iter = iter->link) {
	// may we change abstract in DtSrResult before free it?
	char* abstract = iter->abstractp;
	UAS_Pointer<UAS_String> id, book, section;
	if (DtSR_SearchResultsEntry::
		parse_abstract(abstract, id, book, section) == (int)0) {
#ifdef DEBUG
	    fprintf(stderr, "parse_abstract failed\n");
	    abort();
#endif
	    continue;
	}
	UAS_String Id(*(UAS_String*)id);
	UAS_Pointer<DtSR_SearchResultsEntry>& sre = map[Id];
	if (sre == (int)0) { // not found in map, create one
#ifdef DEBUG
	    cerr << "(DEBUG) " << (char*)Id <<  ' ' <<
				"not found in map" << '\n' << flush;
#endif
	    sre = new DtSR_SearchResultsEntry(
				(char*)*(UAS_String*)id,
				(char*)*(UAS_String*)book,
				(char*)*(UAS_String*)section,
			iter->dbn, iter->language, NULL);
	}

	if (sre->set_proximity(DtSR_SearchZones::keytype2zone(*iter->reckey),
						iter->proximity) >= 0) {
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) keytype=%c, proximity=%d\n", *iter->reckey,
		sre->get_proximity(DtSR_SearchZones::
						keytype2zone(*iter->reckey)));
#endif
	}
	else if (sre->overlay_proximity
			(DtSR_SearchZones::keytype2zone(*iter->reckey),
			 iter->proximity) >= 0) {
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) keytype=%c, overlayed proximity=%d\n", *iter->reckey,
		sre->get_proximity(DtSR_SearchZones::
						keytype2zone(*iter->reckey)));
#endif
	}
	else { // fail
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) set_proximity failed\n");
	    abort();
#endif
	    // NOTE: need to delete sre here
	    continue;
	}
    }

    assert( map.size() > 0 );

    if (! map.size())
	return NULL;

    DictIter<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> > mapiter;
    for (mapiter = map.first(); mapiter(); mapiter++) {
	mapiter.value()->relevance(); // initialize relevance
	// NASTY code to convert UAS_Pointer<DtSR_SearchResultsEntry>
	// to UAS_Pointer<UAS_SearchResultsEntry>
	DtSR_SearchResultsEntry *Cptr_DtSR_sre = mapiter.value();
	UAS_SearchResultsEntry  *Cptr_UAS_sre  = Cptr_DtSR_sre;
	UAS_Pointer<UAS_SearchResultsEntry> uas_res(Cptr_UAS_sre);
	result_list->insert_item(uas_res);
    }

    DtSR_SearchResults::sort(result_list);

    DtSearchFreeResults(&res);
    count = 0;

#ifdef DEBUG
    fprintf(stderr, "(DEBUG) # of results = %d\n", result_list->length());
#endif

    return result_list;
}


UAS_Pointer<UAS_String>
DtSR_SearchEngine::db_name(int n)
{
    if (n < 0 || n >= f_dbcount)
	return NULL;

    UAS_Pointer<UAS_String> rval = new UAS_String(f_dbnames[n]);

    return rval;
}


char *
DtSR_SearchEngine::char_db_name(int n)
{
    if (n < 0 || n >= f_dbcount) {
	return NULL;
    }
    else {
	return f_dbnames[n];
    }
}

