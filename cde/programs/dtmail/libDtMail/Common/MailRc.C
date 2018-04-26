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
/*
 *+SNOTICE
 *
 *	$TOG: MailRc.C /main/5 1998/07/23 18:02:46 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <EUSCompat.h>
#include <unistd.h>
#include <pwd.h>

#include <DtMail/DtMail.hh>
#include <DtMail/Threads.hh>
#include <DtMail/IO.hh>

#define MAXIMUM_PATH_LENGTH	2048

DtMail::MailRc::MailRc(DtMailEnv & error, Session * session)
: _vars(20)
{
    _session = session;

    error.clear();

    passwd pw;
    GetPasswordEntry(pw);

    char *rc_name = new char[MAXIMUM_PATH_LENGTH];

    strcpy(rc_name, pw.pw_dir);
    strcat(rc_name, "/.mailrc");

    _obj_mutex = MutexInit();

    _rc_file = fopen(rc_name, "r");

    if (!_rc_file) { // No rc file. This will be easy!
	delete [] rc_name;
	return;
    }

    parse_file();
    delete [] rc_name;
}

DtMail::MailRc::~MailRc(void)
{
    if (_rc_file) {
	fclose(_rc_file);
    }

    MutexLock lock_scope(_obj_mutex);

    if (_vars.length()) {
	for (int i = 0; i < _vars.length(); i++) {
	    delete _vars[i]->variable;
	    delete _vars[i]->value;
	    delete _vars[i];
	}
    }
}

void
DtMail::MailRc::getValue(DtMailEnv & error, const char * var, const char ** value)
{
    MutexLock lock_scope(_obj_mutex);

    error.clear();

    char * table_val;

    if (_vars.length()) {
	table_val = getVar(var);
    }
    else {
	table_val = NULL;
    }

    if (!table_val) {
	// Check the environment.
	table_val = getenv(var);
	if (!table_val) {
	    error.setError(DTME_NoObjectValue);
	    return;
	}
    }

    *value = table_val;
}

void
DtMail::MailRc::setVar(const char * variable, const char * value)
{
    MutexLock lock_scope(_obj_mutex);

    // See if we can find this variable.
    for (int i = 0; i < _vars.length(); i++) {
	if (strcmp(variable, _vars[i]->variable) == 0) {
	    break;
	}
    }

    // Create only if necessary.
    if (i == _vars.length()) {
	Variable * var = new Variable;
	var->variable = strdup(variable);
	var->value = strdup(value);
	_vars.append(var);
	return;
    }

    free(_vars[i]->value);
    _vars[i]->value = strdup(value);
}

char *
DtMail::MailRc::getVar(const char * var)
{
    for (int i = 0; i < _vars.length(); i++) {
	if (strcmp(var, _vars[i]->variable) == 0) {
	    return(_vars[i]->value);
	}
    }

    return(NULL);
}

void
DtMail::MailRc::parse_file(void)
{
    char line[2000]; // Big enough, I think.

    // Loop through each line in the file.
    while(fgets(line, sizeof(line), _rc_file)) {

	// If the first character is a #, then ignore it and go on.
	if (line[0] == '#') {
	    continue;
	}

	// If this starts with "set", then parse it as a variable.
	if (strncmp(line, "set ", 4) == 0) {
	    parse_var(line);
	}

	// There are lot's more options, but we don't do them yet.
    }
}

void
DtMail::MailRc::parse_var(const char * line)
{
    const char	*cur_c;
    char *variable = new char[100];
    char *value = new char[2000];

    // Find the start of the variable.
    for (cur_c = &line[3]; isspace(*cur_c); cur_c++) {
	continue;
    }

    const char * var_start = cur_c;

    // The rules say the variable name ends at = or the end of the line.
    for (;*cur_c && *cur_c != '=' && (!isspace(*cur_c)); cur_c++) {
	continue;
    }

    memcpy(variable, var_start, (cur_c - var_start));
    variable[cur_c - var_start] = 0;

    // If there is no equal, then we are done.
    if (*cur_c != '=') {
	value[0] = 0;
	setVar(variable, value);
	delete [] variable;
	delete [] value;
	return;
    }

    // Oh well, more work to do. It is easy though. The value is the next character
    // after the = to the end of the line.

    strcpy(value, (cur_c + 1));
    value[strlen(value) - 1] = 0; // Strip newline.

    // Okay, finally, if we have quotes, remove those too.
    if (*value == '\'' || *value == '"') {
	memmove(value, &value[1], strlen(value));
	value[strlen(value) - 1] = 0;
    }

    setVar(variable, value);
    delete [] variable;
    delete [] value;
}

const char *
DtMail::MailRc::getAlias(DtMailEnv & error, const char * name)
{

    error.clear();

    return(NULL);
}

const char *
DtMail::MailRc::getAlternates(DtMailEnv & error)
{
    error.clear();
    return(NULL);
}
