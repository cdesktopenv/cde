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
// $TOG: CatMgr.C /main/7 1998/04/20 12:53:56 mgreess $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#include <assert.h>
#include <string.h>

#include <locale.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

#include <Dt/Dt.h>
#include <Dt/EnvControlP.h>

#include "CatMgr.hh"

#define DEFAULT_MSG	1

CatMgr* CatMgr::f_msg_catalog_mgr = NULL;

CatMgr::CatMgr() : f_msg(NULL), f_default(NULL)
{
#ifdef DEBUG
    char* nlspath = getenv("NLSPATH");
    if (nlspath)
	cerr << "(DEBUG) NLSPATH=" << nlspath << '\n' << flush;
    else
	cerr << "(WARNING) NLSPATH not specified" <<  '\n' << flush;
#endif

    f_catd = catopen(CATALOG_PREFIX, NL_CAT_LOCALE);

    // setup default message
    if (is_open(f_catd)) {
#ifdef DEBUG	
	cerr << "(DEBUG) catopen succeeded." << '\n' << flush;
#endif
	char* msg = ::catgets(f_catd, Set_CatMgr, DEFAULT_MSG,
						"default message not found.");
	f_default = new char[strlen(msg) + 1];
	strcpy(f_default, msg);
    }
    else {
#ifdef DEBUG	
	cerr << "(WARNING) catopen failed." << '\n' << flush;
	static char* cat_not_found = (char*)"default message not found.";
#else
	static char* cat_not_found = (char*)"";
#endif
	f_default = new char[strlen(cat_not_found) + 1];
	strcpy(f_default, cat_not_found);
    }
#ifdef DEBUG	
    cerr << "(DEBUG) default msg=" << f_default << '\n' << flush;
#endif

    f_msg_catalog_mgr = this;
}

CatMgr::~CatMgr()
{
    if (f_msg)
	delete[] f_msg;

    if (f_default)
	delete[] f_default;

    if (is_open(f_catd)) {
	int status = catclose(f_catd);
#ifdef DEBUG
	if (status < 0) {
	    cerr << "(ERROR) catclose failed." << '\n' << flush;
	    abort();
	}
	else {
	    cerr << "(DEBUG) catclose succeeded" << '\n' << flush;
	}
#endif
    }
#ifdef DEBUG
    else {
	cerr << "(DEBUG) catclose not attempted" << '\n' << flush;
    }
#endif
}

char*
CatMgr::catgets(int set_num, int msg_num, const char* def)
{
    if (f_msg)
	delete[] f_msg;

    f_msg = NULL;

    if (is_open(f_catd)) {
	char* msg;
	if (def)
	    msg = ::catgets(f_catd, set_num, msg_num, def);
	else
	    msg = ::catgets(f_catd, set_num, msg_num, f_default);
#if defined(UXPDS) && defined(GENCAT_BUG)
	for (; *msg == ' ' || *msg == '\t'; msg++);
#endif
	int msglen = strlen(msg);
	f_msg = new char[msglen + 1];
#if defined(UXPDS) && defined(GENCAT_BUG)
#ifdef DEBUG
    cerr << "(DEBUG) catgets msg=<" << msg << ">\n" << flush;
#endif
	// trim embracing double quotes on uxpds
	if (msglen > 1 && *(msg + msglen - 1) == '"') {
	    if (*msg == *(msg + msglen - 1)) {
		*(msg + msglen - 1) = '\0';
		msg++;
		msglen -= 2;
	    }
	}
	else if (*msg == '"') {
	    msg++;
	    msglen--;
	}
#endif
	strcpy(f_msg, msg);
    }
    else {
	if (def) {
	    f_msg = new char[strlen(def) + 1];
	    strcpy(f_msg, def);
	}
	else {
	    f_msg =new char[strlen(f_default) + 1];
	    strcpy(f_msg, f_default);
	}
    }

    assert( f_msg != NULL );

#ifdef DEBUG
    cerr << "(DEBUG) retrieved msg=" << f_msg << '\n' << flush;
#endif

    return f_msg;
}

