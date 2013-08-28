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
    int len;

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
	len = strlen(msg);
	f_default = new char[len + 1];
	*((char *) memcpy(f_default, msg, len) + len) = '\0';
    }
    else {
#ifdef DEBUG	
	cerr << "(WARNING) catopen failed." << '\n' << flush;
	static char* cat_not_found = (char*)"default message not found.";
#else
	static char* cat_not_found = (char*)"";
#endif
	len = strlen(cat_not_found);
	f_default = new char[len + 1];
	*((char *) memcpy(f_default, cat_not_found, len) + len) = '\0';
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
	if (status < 0) {
#ifdef DEBUG
	    cerr << "(ERROR) catclose failed." << '\n' << flush;
	    abort();
#endif
	}
	else {
#ifdef DEBUG
	    cerr << "(DEBUG) catclose succeeded" << '\n' << flush;
#endif
	}
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
    int len;

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
	len = msglen;
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
	*((char *) memcpy(f_msg, msg, len) + len) = '\0';
    }
    else {
	if (def) {
	    len = strlen(def);
	    f_msg = new char[len + 1];
	    *((char *) memcpy(f_msg, def, len) + len) = '\0';
	}
	else {
	    len = strlen(f_default);
	    f_msg =new char[len + 1];
	    *((char *) memcpy(f_msg, f_default, len) + len) = '\0';
	}
    }

    assert( f_msg != NULL );

#ifdef DEBUG
    cerr << "(DEBUG) retrieved msg=" << f_msg << '\n' << flush;
#endif

    return f_msg;
}

