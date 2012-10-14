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
// $XConsortium: sdrv.C /main/5 1996/09/27 19:02:52 drk $
#include <stdio.h>
#include <sstream>
using namespace std;
#include <strings.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Exceptions.hh"
#include "UAS_Exceptions.hh"
#include "UAS_Msgs.hh"
#include "StringPref.hh"

#if 0
// note: bogus, need to do this in a more reasonable way
#include "MMDB.hh"
#include <api/info_lib.h>
#include <api/info_base.h>

extern void terminate_mmdb();
#endif

#include "UAS_String.hh"
#include "UAS_PtrList.hh"
#include "UAS_SearchResults.hh"
#include "UAS_SearchScope.hh"
#include "UAS_Receiver.hh"

#ifdef DTSEARCH
#include "DtSR_SearchEngine.hh"
#else
#include "Fulcrum_SearchEngine.hh"
#endif

#ifdef DEBUG
#define ON_DEBUG(stmt)  stmt
#else
#define ON_DEBUG(stmt)
#endif

static int g_nres = 50;
static int g_maxndocs = 2000;

class ReportMsg : public UAS_Receiver<UAS_SearchMsg>
{
    public:
        void receive (UAS_SearchMsg &m);
};

void
ReportMsg::receive(UAS_SearchMsg &m)
{
    if (strcmp (m.fSearchMsg, "TooManyIbases") != 0)
        return;

    char ch;

    cout << "\n\nWARNING: Search attempted over " << m.fNumBcases << " bookcases!\n\n";
    cout << "         If continued, this search will be truncated to only\n";
    cout << "         the first " << m.fMaxNumBcases << " bookcases (the maximum number allowed).\n\n";
    cout << "Continue [n]? " << flush;
    cin.sync_with_stdio();
    cin.get(ch);

    if (ch == 0x79) {
        cout << "\nSearch continuing...";
        m.fContFlag = 1;
    }
    else {
        cout << "\nSearch cancelled.\n";
        m.fContFlag = 0;
    }

    return;
}

static void
print_usage_and_exit( char *pname, char *arg )
{
    if (arg)
        cout << "Unknown argument: " << arg << endl;
    cout << "Usage: " << pname << " [-help] [-l infolib_path]\n" << endl;
    exit(-1);
}

static void
config_environment( int argc, char **argv )
{
    char   *ilib, *tbuf;
    int     i;

    // is it already in the environment?
    ilib = getenv ("DTINFO_INFOLIB_PATH");

    // if not then parse it out of the arguments
    if (!ilib) {
        for (i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                if (!(strncmp(argv[i], "-help", 2)))
                    print_usage_and_exit(argv[0], NULL);

                else if (!(strncmp(argv[i], "-l", 2))) {
                    if (argv[++i])
                        ilib = argv[i];
                    else
                        print_usage_and_exit(argv[0], NULL);
                }
                else
                    print_usage_and_exit(argv[0], argv[i]);
            }
            else
                print_usage_and_exit(argv[0], argv[i]);
        }
        if (!ilib)
            print_usage_and_exit(argv[0], NULL);
        else {
            tbuf = new char[128];
            sprintf(tbuf, "DTINFO_INFOLIB_PATH=%s", ilib);
            putenv(tbuf);
        }
    }
}

static void
loadBasepathList(UAS_PtrList<const char> &basepathList)
{
    char *libenv = getenv ("DTINFO_INFOLIB_PATH");
    if (!libenv)
        return;

    UAS_String libstr (libenv);
    UAS_List<UAS_String> fields = libstr.splitFields (':');

    char names_buf[256];
    char base_name[128];
    int  new_len;
    struct stat stat_buf;
    FILE *fp;
    char *new_buf;
    const char *ilib_path;

    for (int i = 0; i < fields.length(); i ++) {
        ilib_path = (char *)*(fields[i]);
        sprintf(names_buf, "%s/names.mmdb", ilib_path);
        if (stat(names_buf, &stat_buf) == -1) {
            cout << "\n\nWarning: library does not appear to be valid;\n";
            cout << "         couldn't stat file: " << names_buf << ";\n";
            cout << "         error code is: " << errno << "\n\n";
            break;
        }

        fp = fopen(names_buf, "r");
        if (!fp) {
            cout << "\n\nWarning: library does not appear to be valid;\n";
            cout << "         couldn't open file: " << names_buf << ";\n";
            cout << "         error code is: " << errno << "\n\n";
            break;
        }

        while (fscanf(fp, "%s%*[^\n]\n", base_name) != EOF) {
            if (base_name[0] != '#') {
                new_len = strlen(ilib_path) + 1 + strlen(base_name) + 1;
                new_buf = new char[new_len];
                sprintf(new_buf, "%s/%s", ilib_path, base_name);
                basepathList.append(new_buf);
            }
        }

        fclose(fp);
    }

}

static void
setPreferences()
{
    char    ch;

    cout << "Current values:" << endl;
    if (g_maxndocs == 0)
        cout << " Maximum documents: unlimited" << endl;
    else
        cout << " Maximum documents: " << g_maxndocs << endl;
    cout << " Number of results: " << g_nres << endl;

    cout << "\nModify:\n";
    cout << " Maximum documents [d]\n";
    cout << " Number of results [r]\n";
    cout << " Cancel            [c]\n";
    do {
        cout << "\nd, r, c? " << flush;
        cin >> ch;
    }
    while (ch != 0x64 && ch != 0x72 && ch != 0x63);

    cout << endl;

    switch (ch) {
        // [ d ]
        case 0x64:
            cout << "Maximum documents [" << g_maxndocs << "]? " << flush;
            cin >> g_maxndocs;

            if (g_maxndocs < 0 || g_maxndocs > 10000)
                g_maxndocs = 0;

            if (g_maxndocs == 0)
                cout << "\nMaximum documents set to: unlimited" << endl;
            else
                cout << "\nMaximum documents set to: " << g_maxndocs << endl;
        break;

        // [ r ]
        case 0x72:
            cout << "Number results [" << g_nres << "]? " << flush;
            cin >> g_nres;

            if (g_nres < 0 || (g_maxndocs != 0 && g_nres > g_maxndocs))
                g_nres = g_maxndocs;

            cout << "\nNumber results set to: " << g_nres << endl;
        break;

        // [ c ]
        case 0x63:
            cout << "Cancelled." << endl;
        break;
    }
    cout << endl;
}

static UAS_Pointer<UAS_SearchResultsEntry>
getSearchResults(UAS_Pointer<UAS_SearchResults> res)
{
    int i;
    char ch;
    UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > results;
    UAS_Pointer<UAS_SearchResultsEntry> re = NULL;

    if (res->num_docs()) {
        cout << "\n" << res->num_docs() << " documents found." << endl;

        cout << "\nGet result list [y]? " << flush;
        cin.sync_with_stdio();
        cin.get(ch);

        if (ch == 0x79 || ch == 0x0d || ch == 0x0a) {
            cout << "\nGetting result list for " << (res->num_docs() < g_nres ? res->num_docs() : g_nres);
            cout << " documents...";

            try
             {
                results = res->create_results(0, g_nres);

                cout << "\n";
                for (i = 0; i < results->length(); i++) {
                    cout << i+1 << ".\t" << "[" << results->item(i)->relevance() << "]\t";
                    cout << (char *)results->item(i)->book() << ": ";
                    cout << (char *)results->item(i)->section() << endl;
                }
                do {
                    cout << "\nChoose a title: " << flush;
                    cin.sync_with_stdio();
                    cin >> i;
                }
                while (i <= 0 || i > results->length());
                i--;
                re = results->item(i);
             }
            // fatal error during result retrieval occurred
            catch(UAS_Exception&, e)
             {
                 cout << "\nERROR: " << e->message() << "\n\n";
             }
            end_try;

        }
     }
     else
         cout << "\nNothing found." << endl;

     return re;
}

static void
getSearchMatches(UAS_Pointer<UAS_SearchResultsEntry> &re)
{
    UAS_Pointer<UAS_List<UAS_TextRun> > matches;
    UAS_Pointer<UAS_TextRun> tr;

    matches = re->create_matches();

    cout << "\nFound " << matches->length() << " matches in ";
    cout << (char *)re->book() << " : ";
    cout << (char *)re->section() << "\n\n";

    for (int i = 0; i < matches->length(); i++) {
        tr = (*matches)[i];
        cout << i+1 << ".\toffset: " << tr->offset();
        cout << "\tlength: " << tr->length();
        if (tr->type() == WithTags)
            cout << "\ttype: WithTags" << endl;
        else
            cout << "\ttype: WithoutTags" << endl;
    }
    cout << "\n\n";
}

static void
printHistory(UAS_List<UAS_SearchResults> &hst)
{
    int i;
    UAS_Pointer<UAS_SearchResults>   res;

    cout << "Current search history is:" << endl;
    for (i = 0; i < hst.length(); i++) {
        cout << "  " << i+1 << ".\t";
        res = hst[i];
        cout << "[" << *(res->scope_name()) << " : "<< *(res->query()) << "]: ";
        cout << res->num_docs() << " matches" << endl;
    }
}

static void
doHistory(UAS_List<UAS_SearchResults> &hst)
{
    int nhst;
    UAS_Pointer<UAS_SearchResultsEntry> re;

    if (hst.length())
        printHistory(hst);
    else {
        cout << "No current history.\n" << endl;
        return;
    }

    cout << "\nSelect a past query: " << flush;
    cin >> nhst;

    if (nhst <= 0 || nhst > (hst.length())+1) {
        cout << "Cancelled.\n" << endl;
        return;
    }

    cout << endl;

    re = getSearchResults(hst[nhst-1]);
    if (re)
        getSearchMatches(re);
}

static const char *
basename(const char *path)
{
    if (!path)
        return NULL;

    const char *p;

    for (p = path + strlen(path) - 1; *p != '/' && p != path; p--);

    if (*p == '/')
        p++;

    return p;
}

// this routine is a rather egregious hack... but it'll get the job done
static void
readScopesFromPrefs(UAS_PtrList<const char> &baseList,
                    UAS_List<UAS_SearchScope> &scopeList)
{
  UAS_PtrList<UAS_BookcaseEntry>  tmp_bcases;
  UAS_PtrList<UAS_BookcaseEntry>  bcases;
  UAS_BookcaseEntry              *bce;
  char                            scratch[1024];
  char                            bc_name[256];
  int                             i;

  // create a tmp bookcase list for entire library
  // we need this list to do some bc name matching later
  for (i = 0; i < baseList.numItems(); i++) {
      bce = new UAS_BookcaseEntry(baseList[i]);
      tmp_bcases.append(bce);
      bce->set_base_num(i);

  }

  // brennan's base code to get scopes from preferences below,
  // heisted directly from the browser; modified to build the
  // uas scope objects... hang onto your hat, gets pretty ugly;
  StringPref scopes ("Scopes");
  char *name;
  char *names = strdup (scopes.value());
  char *name_point = names;
  const char *scope_point;
  ON_DEBUG (printf ("NAMES = 0x%p\n", names));

  bool is_valid;

  while (*name_point != '\0') {
      is_valid = TRUE;
      // NULL-terminate the next name. 
      name = name_point;
      while (*name_point != ',' && *name_point != '\0')
        name_point++;
      if (*name_point != '\0') {
        assert( *name_point == ',' ); // delimiter ',' encountered
        *name_point++ = '\0';
      }
      assert( strlen(name) > 0 );
      // Get the specified preference.
      sprintf (scratch, "Scope.%s", name);
      StringPref scope (scratch);
      ON_DEBUG (printf ("RESTORING scope: %s\n", name));
      ON_DEBUG (printf ("  value = %s\n", scope.value()));
      ON_DEBUG (printf ("  name_point now = %s\n", name_point));

      // Grab the component mask.
      unsigned int mask = 0;
      // NOTE: Add error checking!! 
      sscanf (scope.value(), "%d%n", &mask, &i);
      scope_point = scope.value() + i;
      ON_DEBUG (printf ("  Scope mask: %d\n", mask));
      ON_DEBUG (printf ("    (advancing %d chars)\n", i));
      if (mask == 0) {
          // rtp - 4/24/95 : bad mask means invalid scope
          is_valid = FALSE;
          continue;
        }

      assert( *scope_point == ';' );

      while (*scope_point == ';' &&
             sscanf (scope_point, ";%[^,;]%n", bc_name, &i) == 1) {
          scope_point += i;
          ON_DEBUG (printf ("  Considering base %s\n", bc_name));
          ON_DEBUG (printf ("    (advancing %d chars to %s)\n",i,scope_point));

          bce = NULL;
          // Find the corresponding BookcaseEntry
          for (i = 0; i < tmp_bcases.numItems(); i++) {
              ON_DEBUG (printf ("  checking against <%s>\n", tmp_bcases[i]->name()));
              if (strcmp (tmp_bcases[i]->name(), (const char *)bc_name) == 0) {
                  ON_DEBUG (printf ("  Got base: %s\n", bc_name));
                  bce = new UAS_BookcaseEntry(baseList[i]);
                  // note: browser code sorts this list by base_num
                  bcases.append(bce);
                  break;
              }

          }

          // if we can't find it, punt... on to the next one
          if (bce == NULL) {
              is_valid = FALSE;
              continue;
           }

          // Now pull out the books
          int book_num;
          UAS_ObjList<int> booklist;
          while (sscanf (scope_point, ",%d%n", &book_num, &i) == 1) {
              ON_DEBUG (printf ("  Got book: %d\n", book_num));
              ON_DEBUG (printf ("    (advancing %d chars)\n", i));
              scope_point += i;
              booklist.append(book_num);
          }
          bce->set_book_list(booklist);
      }
      // if valid scope then create it
      if (is_valid)
          scopeList.insert_item (new UAS_SearchScope(name, bcases, mask));
      // now, reset bcases
      bcases.clear();
  }

  // clean up memory
  free (names);
  for (i = 0; i < tmp_bcases.numItems(); i++)
      delete tmp_bcases[i];

}

static void
createScopes(UAS_PtrList<const char> &baseList,
             UAS_List<UAS_SearchScope> &scopeList)
{
    UAS_PtrList<UAS_BookcaseEntry>  bcases;
    UAS_BookcaseEntry              *bce;

    // create the scope for the current section
    scopeList.insert_item (
        new UAS_SearchScope("Current Section", bcases, UAS_SCOPE_SECTION)
                          );

    // create the bookcase list for entire library
    for (int i = 0; i < baseList.numItems(); i++) {
        bce = new UAS_BookcaseEntry(baseList[i]);
        bcases.append(bce);
    }

    // create a scope for entire library
    scopeList.insert_item (
        new UAS_SearchScope("Entire Library", bcases, UAS_SCOPE_ALL)
                          );
    scopeList[0]->set_read_only();

    // clear the list for reuse
    bcases.clear();

    // now, create a scope for each bookcase
    for (i = 0; i < baseList.numItems(); i++) {
        bce = new UAS_BookcaseEntry(baseList[i]);
        bcases.append(bce);
        scopeList.insert_item (
            new UAS_SearchScope(bce->name(), bcases, UAS_SCOPE_ALL)
                              );
        scopeList[i+1]->set_read_only();
        bcases.clear();
    }

    readScopesFromPrefs(baseList, scopeList);

#ifdef DEBUG
    for (i = 0; i < scopeList.length(); i++) {
        scopeList[i]->dump();
        cout << "\n\n";
    }
#endif

}

static int
getScopeIndex(UAS_List<UAS_SearchScope> &scopeList, int idx)
{
    cout << "Current search scope is: " << scopeList[idx]->name() << endl;
    cout << "\nAvailable search scopes are:" << endl;
    for (int i = 0; i < scopeList.length(); i++) {
        cout << "  " << i+1 << ".\t";
        cout << scopeList[i]->name() << endl;
    }

    int nsel, ret_idx;

    cout << "\nSelect a search scope: " << flush;
    cin >> nsel;

    // index out of range, punt
    if (nsel <= 0 || nsel > (scopeList.length())+1) {
        cout << "Cancelled.\n" << endl;
        ret_idx = idx;
    }
    else {
        if (nsel == 1) {
            char locator[128];

            if ( scopeList[0]->search_zones().search_section().length() )
                cout << "\nLocator for current section: " <<
                         scopeList[0]->search_zones().search_section() << endl;
            cout << "Enter a new locator: " << flush;
            locator[0] = '\0';
            while (!strlen(locator))
                cin.getline(locator, 128);
            scopeList[0]->search_zones().section(locator);
        }
        ret_idx = nsel-1;
        cout << "\nSetting scope to: " << scopeList[ret_idx]->name() << endl;
        cout << "\n";
    }

    return ret_idx;
}

int
main( int argc, char **argv )
{
    INIT_EXCEPTIONS();

    UAS_Common::initialize ();

    // first, configure environment to open an MMDB
    config_environment(argc, argv);

    UAS_PtrList<const char> *basepathList = new UAS_PtrList<const char>;

#if 0
    try
     {
        // next, extract the paths to the fulcrum indices
        // note: bogus, need to do this in a more reasonable way
        info_lib *ilib = the_mmdb().database()->infolib_ptr();

        // if not a valid infolib then punt
        int num_bases = ilib->num_of_bases();
        if (!num_bases)
            throw(Exception());

        // traverse the list and grab the infobase paths...
        int key = ilib->first();
        const info_base *ibase;
        while (num_bases--) {
            ibase = (*ilib)(key);
            basepathList->append(ibase->get_base_path());
            ilib->next (key);
        }
     }
    catch_any()
     {
        // fatal error, couldn't open the infolib
        cout << "Failed to open library at: " << getenv ("DTINFO_INFOLIB_PATH") << endl;
        cout << "Check path, is it correct?" << endl;
        exit (-1);
     }
    end_try;
#else
    loadBasepathList(*basepathList);
#endif

    // next, read the search scopes from the user prefs file
    UAS_List<UAS_SearchScope> scopeList;
    createScopes(*basepathList, scopeList);

    // create the search engine
#ifdef DTSEARCH
    UAS_Pointer<DtSR_SearchEngine> se =
			&DtSR_SearchEngine::search_engine(basepathList);
#else
    UAS_Pointer<Fulcrum_SearchEngine> se =
			&Fulcrum_SearchEngine::search_engine(basepathList);
#endif

    ReportMsg reportMsg;

    se->request((UAS_Receiver<UAS_SearchMsg> *) &reportMsg);

    delete basepathList;

    char                                query[128];
    char                                ch;
    int                                 scope_idx = 1;
    bool                                bail;
    UAS_String                          query_str;
    UAS_Pointer<UAS_SearchResults>      res;
    UAS_List<UAS_SearchResults>         hst;
    UAS_Pointer<UAS_SearchResultsEntry> re;


    bail = FALSE;

    do {
        cout << " Search          [s]\n";
        cout << " Choose scope    [c]\n";
        cout << " Search history  [h]\n";
        cout << " Preferences     [p]\n";
        cout << " Quit            [q]\n";
        cout << "\ns, c, h, p or q? " << flush;
        cin >> ch;

        if (ch != 0x73 && ch != 0x63 && ch != 0x68 &&
            ch != 0x70 && ch != 0x71) {
            cout << "\nChoice not valid, try again.\n" << endl;
            cin.sync_with_stdio();
            continue;
        }
        
        cout << "\n";

        switch (ch) {

            //  [ s ]
            case 0x73:
                cout << "Enter query: " << flush;

                query[0] = '\0';
                while (!strlen(query)) {
                    cin.getline(query, 128);
                }

                query_str = query;

                cout << "\n";
                cout << "Finding [" << (char *)query_str << "]..." << flush;

                try
                 {
                    res = se->search(query_str, scopeList[scope_idx], g_maxndocs);
                    if (res) {
                        hst.insert_item(res, -1);
                        re = getSearchResults(res);
                        if (re)
                            getSearchMatches(re);
                    }
                 }
                // fatal error during search occurred
                catch(UAS_Exception&, e)
                 {
                    cout << "\nSEARCH FAILED: " << e->message() << endl;
                 }
                end_try;
                cout << "\n\n";
            break;

            //  [ c ]
            case 0x63:
                scope_idx = getScopeIndex(scopeList, scope_idx);
            break;

            //  [ h ]
            case 0x68:
                doHistory(hst);
            break;

            //  [ p ]
            case 0x70:
                setPreferences();
            break;


            //  [ q ]
            case 0x71:
                bail = TRUE;
            break;

            default:
            break;
        }
        cin.sync_with_stdio();
    }
    while (!bail);

#if 0
    terminate_mmdb();
#endif

    UAS_Common::finalize();

}
