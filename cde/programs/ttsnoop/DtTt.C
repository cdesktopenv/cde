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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
//%%  (c) Copyright 1993, 1994 Novell, Inc.
//%%  $XConsortium: DtTt.C /main/4 1996/03/19 10:47:59 barstow $
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <strstream>
#else
#include <strstream.h>
#endif

#include <Xm/TextF.h>
#include <Dt/SpinBox.h>
#include <Dt/HelpDialog.h>
#include "tt_c++.h"
#include "DtTt.h"
#include "messageProps_ui.h"
#include "patternProps_ui.h"

static const char **	dtTtProcids		= 0;
static const void **	dtTtProcidClientDatums	= 0;
static unsigned int	dtTtProcidsCount	= 0;
static Tt_pattern *	dtTtPatterns		= 0;
static unsigned int	dtTtPatternsCount	= 0;
static Tt_message *	dtTtMessages		= 0;
static unsigned int	dtTtMessagesCount	= 0;
static Tt_pattern **	dtTtDtSessions		= 0;
static unsigned int	dtTtDtSessionsCount	= 0;
static Tt_pattern **	dtTtDtFiles		= 0;
static unsigned int	dtTtDtFilesCount	= 0;

int		_DtTtPatsNameKey	= (int) (long) DtTtNth;
extern Tt_pattern	snoopPat;

int
listGrow(
	void **		pList,
	unsigned int &	listCount,
	size_t		elemSize
)
{
	void *newList;
	newList = realloc( *pList, (1 + listCount) * elemSize );
	if (newList == 0) {
		return 0;
	}
	*pList = newList;
	listCount++;
	return 1;
}

XmString
DtTtStatusString(
	Tt_status	status
)
{
	char *s = tt_status_string( status );
	XmString s2 = XmStringCreateLocalized( s );
	tt_free( s );
	return s2;
}

Tt_status
DtTtCreated(
	DtTtType	type,
	const void *	entity,
	const void *	clientData
)
{
	Tt_status status = tt_ptr_error( entity );
	if ((tt_is_err( status )) || (entity == 0)) {
		return TT_OK;
	}
	switch (type) {
		char *proc;
	    case DTTT_PROCID:
		proc = strdup( (const char *)entity );
		if (proc == 0) {
			return TT_ERR_NOMEM;
		}
		if (! listAppend( dtTtProcids, dtTtProcidsCount, const char *,
				  proc ))
		{
			return TT_ERR_NOMEM;
		}
		dtTtProcidsCount--; // shared by both lists
		if (! listAppend( dtTtProcidClientDatums, dtTtProcidsCount,
				  const void *, clientData ))
		{
			return TT_ERR_NOMEM;
		}
		break;
	    case DTTT_MESSAGE:
		if (DtTtIndex( type, entity ) >= 0) {
			return TT_OK;
		}
		if (! listAppend( dtTtMessages, dtTtMessagesCount, Tt_message,
				  (const Tt_message)entity ))
		{
			return TT_ERR_NOMEM;
		}
		break;
	    case DTTT_PATTERN:
		if (DtTtIndex( type, entity ) >= 0) {
			return TT_OK;
		}
		if (! listAppend( dtTtPatterns, dtTtPatternsCount, Tt_pattern,
				  (const Tt_pattern)entity ))
		{
			return TT_ERR_NOMEM;
		}
		break;
	    case DTTT_PTYPE:
	    case DTTT_OTYPE:
	    case DTTT_OBJECT:
	    case DTTT_SESSION:
	    case DTTT_DTSESSION:
	    case DTTT_DTFILE:
	    case DTTT_OP:
		abort();
		break;
	}
	return TT_OK;
}

Tt_status
DtTtCreated(
	DtTtType	type,
	Tt_pattern *	entity,
	const char *	name
)
{
	if ((tt_is_err( tt_ptr_error( entity ) )) || (entity == 0)) {
		return TT_OK;
	}
	if ((tt_is_err( tt_ptr_error( name ) )) || (name == 0)) {
		return TT_OK;
	}
	switch (type) {
	    case DTTT_PROCID:
	    case DTTT_MESSAGE:
	    case DTTT_PATTERN:
	    case DTTT_PTYPE:
	    case DTTT_OTYPE:
	    case DTTT_OBJECT:
	    case DTTT_SESSION:
	    case DTTT_OP:
		abort();
		break;
	    case DTTT_DTSESSION:
		tt_pattern_user_set( *entity, _DtTtPatsNameKey,
				     strdup( name ));
		if (DtTtIndex( type, entity ) >= 0) {
			return TT_OK;
		}
		if (! listAppend( dtTtDtSessions, dtTtDtSessionsCount,
				  Tt_pattern *, (Tt_pattern *)entity ))
		{
			return TT_ERR_NOMEM;
		}
		break;
	    case DTTT_DTFILE:
		tt_pattern_user_set( *entity, _DtTtPatsNameKey,
				     strdup( name ));
		if (DtTtIndex( type, entity ) >= 0) {
			return TT_OK;
		}
		if (! listAppend( dtTtDtFiles, dtTtDtFilesCount,
				  Tt_pattern *, (Tt_pattern *)entity ))
		{
			return TT_ERR_NOMEM;
		}
		break;
	}
	while (*entity != 0) {
		DtTtCreated( DTTT_PATTERN, *entity );
		entity++;
	}
	return TT_OK;
}

void *
DtTtNth(
	DtTtType	type,
	int		n
)
{
	switch (type) {
	    case DTTT_PROCID:
		if ((n < 0) || (n >= dtTtProcidsCount)) {
			return 0;
		}
		return (void *)dtTtProcids[ n ];
	    case DTTT_MESSAGE:
		if ((n < 0) || (n >= dtTtMessagesCount)) {
			return 0;
		}
		return dtTtMessages[ n ];
	    case DTTT_PATTERN:
		if ((n < 0) || (n >= dtTtPatternsCount)) {
			return 0;
		}
		return dtTtPatterns[ n ];
	    case DTTT_DTSESSION:
		if ((n < 0) || (n >= dtTtDtSessionsCount)) {
			return 0;
		}
		return dtTtDtSessions[ n ];
	    case DTTT_DTFILE:
		if ((n < 0) || (n >= dtTtDtFilesCount)) {
			return 0;
		}
		return dtTtDtFiles[ n ];
	    case DTTT_OP:
		return tttk_op_string( (Tttk_op)(n+1) );
	}
}

void *
DtTtNthClientDatum(
	DtTtType	type,
	int		n
)
{
	switch (type) {
	    case DTTT_PROCID:
		if ((n < 0) || (n >= dtTtProcidsCount)) {
			return 0;
		}
		return (void *)dtTtProcidClientDatums[ n ];
	    default:
		abort();
		return 0;
	}
}

void
DtTtNthClientDatumSet(
	DtTtType	type,
	int		n,
	const void *	clientData
)
{
	switch (type) {
	    case DTTT_PROCID:
		if ((n < 0) || (n >= dtTtProcidsCount)) {
			return;
		}
		dtTtProcidClientDatums[ n ] = clientData;
		return;
	    default:
		abort();
	}
}

int
DtTtIndex(
	DtTtType	type,
	const void *	entity
)
{
	Tt_status status = tt_ptr_error( entity );
	if ((tt_is_err( status )) || (entity == 0)) {
		return -1;
	}
	switch (type) {
		int i;
	    case DTTT_PROCID:
		for (i = dtTtProcidsCount - 1; i >= 0; i--) {
			if (strcmp( dtTtProcids[i], (char *)entity) == 0) {
				return i;
			}
		}
		return -1;
	    case DTTT_MESSAGE:
		for (i = dtTtMessagesCount - 1; i >= 0; i--) {
			if (dtTtMessages[i] == entity) {
				return i;
			}
		}
		return -1;
	    case DTTT_PATTERN:
		for (i = dtTtPatternsCount - 1; i >= 0; i--) {
			if (dtTtPatterns[i] == entity) {
				return i;
			}
		}
		return -1;
	    case DTTT_DTSESSION:
		for (i = dtTtDtSessionsCount - 1; i >= 0; i--) {
			if (dtTtDtSessions[i] == entity) {
				return i;
			}
		}
		return -1;
	    case DTTT_DTFILE:
		for (i = dtTtDtFilesCount - 1; i >= 0; i--) {
			if (dtTtDtFiles[i] == entity) {
				return i;
			}
		}
		return -1;
	    case DTTT_OP:
		for (i = 1; i < TTDT_OP_LAST; i++) {
			if (0 == strcmp( tttk_op_string( (Tttk_op)i ),
					 (char *)entity ))
			{
				return i;
			}
		}
		return -1;
	}
}

Tt_status
DtTtDestroyed(
	DtTtType	type,
	const void *	entity
)
{
	Tt_status status = tt_ptr_error( entity );
	if ((tt_is_err( status )) || (entity == 0)) {
		return TT_OK;
	}
	switch (type) {
		int i, j;
		Tt_pattern *pats;
	    case DTTT_PROCID:
		for (i = dtTtProcidsCount - 1; i >= 0; i--) {
			if (strcmp( dtTtProcids[i], (char *)entity) == 0) {
				break;
			}
		}
		if (i < 0) {
			return TT_WRN_NOTFOUND;
		}
		for (j = i; j < dtTtProcidsCount - 1; j++) {
			dtTtProcids[j] = dtTtProcids[j+1];
		}
		for (j = i; j < dtTtProcidsCount - 1; j++) {
			dtTtProcidClientDatums[j] =
				dtTtProcidClientDatums[j+1];
		}
		dtTtProcidsCount--;
		break;
	    case DTTT_MESSAGE:
		for (i = dtTtMessagesCount - 1; i >= 0; i--) {
			if (dtTtMessages[i] == entity) {
				break;
			}
		}
		if (i < 0) {
			return TT_WRN_NOTFOUND;
		}
		for (j = i; j < dtTtMessagesCount - 1; j++) {
			dtTtMessages[j] = dtTtMessages[j+1];
		}
		dtTtMessagesCount--;
		break;
	    case DTTT_PATTERN:
		if (snoopPat == entity) snoopPat = 0;
		for (i = dtTtPatternsCount - 1; i >= 0; i--) {
			if (dtTtPatterns[i] == entity) {
				break;
			}
		}
		if (i < 0) {
			return TT_WRN_NOTFOUND;
		}
		for (j = i; j < dtTtPatternsCount - 1; j++) {
			dtTtPatterns[j] = dtTtPatterns[j+1];
		}
		dtTtPatternsCount--;
		break;
	    case DTTT_DTSESSION:
		for (i = dtTtDtSessionsCount - 1; i >= 0; i--) {
			if (dtTtDtSessions[i] == entity) {
				break;
			}
		}
		if (i < 0) {
			return TT_WRN_NOTFOUND;
		}
		pats = (Tt_pattern *)entity;
		while (*pats != 0) {
			DtTtDestroyed( DTTT_PATTERN, *pats );
			pats++;
		}
		for (j = i; j < dtTtDtSessionsCount - 1; j++) {
			dtTtDtSessions[j] = dtTtDtSessions[j+1];
		}
		dtTtDtSessionsCount--;
		break;
	    case DTTT_DTFILE:
		for (i = dtTtDtFilesCount - 1; i >= 0; i--) {
			if (dtTtDtFiles[i] == entity) {
				break;
			}
		}
		if (i < 0) {
			return TT_WRN_NOTFOUND;
		}
		pats = (Tt_pattern *)entity;
		while (*pats != 0) {
			DtTtDestroyed( DTTT_PATTERN, *pats );
			pats++;
		}
		for (j = i; j < dtTtDtFilesCount - 1; j++) {
			dtTtDtFiles[j] = dtTtDtFiles[j+1];
		}
		dtTtDtFilesCount--;
		break;
	}
	return TT_OK;
}

Tt_status
DtTtSetLabel(
	Widget labelWidget,
	const char *string
)
{
    if (labelWidget == 0) {
	    return TT_OK;
    }
    XmString labelXmString = XmStringCreateLocalized( (String)string );
    XtVaSetValues( labelWidget, XmNlabelString, labelXmString, NULL );
    XmStringFree( labelXmString );
    return TT_OK;
}

Tt_status
DtTtSetLabel(
	Widget labelWidget,
	const char *func,
	void *val
)
{
    Tt_status status = tt_ptr_error( val );
    std::ostrstream errStream;
    errStream << func << " = " << val << " (" << status << ")" << ends;
    char *label = errStream.str();
    DtTtSetLabel( labelWidget, label );
    delete label;
    return status;
}

Tt_status
DtTtSetLabel(
	Widget labelWidget,
	const char *func,
	Tt_status status
)
{
    std::ostrstream errStream;
    errStream << func << " = " << status << ends;
    char *label = errStream.str();
    DtTtSetLabel( labelWidget, label );
    delete label;
    return status;
}

int
DtTtSetLabel(
	Widget labelWidget,
	const char *func,
	int returnVal
)
{
    std::ostrstream errStream;
    errStream << func << " = " << returnVal << ends;
    char *label = errStream.str();
    DtTtSetLabel( labelWidget, label );
    delete label;
    return returnVal;
}


static XmString *
_DtTtChoices(
	Tt_pattern **	pPats,
	int		count
)
{
	// XXX when to free?
	XmString *items = (XmString *)XtMalloc( count * sizeof( XmString ));
	if (items == 0) {
		return 0;
	}
	for (int i = 0; i < count; i++) {
		std::ostrstream itemStream;
		itemStream << (void *)pPats[ i ];
		char *name = (char *)
			tt_pattern_user( *pPats[ i ], _DtTtPatsNameKey );
		if (! tt_is_err( tt_ptr_error( name ))) {
			itemStream << " " << name;
			tt_free( name );
		}
		itemStream << ends;
		char *string = itemStream.str();
		items[ i ] = XmStringCreateLocalized( string );
		delete string;
	}
	return items;
}

XmString *
_DtTtChoices(
	DtTtType type,
	int *    itemCount
)
{
	*itemCount = 0;
	switch (type) {
		XmString *items;
		int i;
		int opCount;
	    case DTTT_PROCID:
		items = (XmString *)
			XtMalloc( dtTtProcidsCount * sizeof( XmString ));
		if (items == 0) {
			return 0;
		}
		*itemCount = dtTtProcidsCount;
		for (i = 0; i < dtTtProcidsCount; i++) {
			items[ i ] = XmStringCreateLocalized(
					(String)dtTtProcids[ i ] );
		}
		return items;
	    case DTTT_MESSAGE:
		items = (XmString *)
			XtMalloc( dtTtMessagesCount * sizeof( XmString ));
		if (items == 0) {
			return 0;
		}
		*itemCount = dtTtMessagesCount;
		for (i = 0; i < dtTtMessagesCount; i++) {
			std::ostrstream itemStream;
			itemStream << (void *)dtTtMessages[ i ];
			char *op = tt_message_op( dtTtMessages[ i ] );
			if (! tt_is_err( tt_ptr_error( op ))) {
				itemStream << " " << op;
				tt_free( op );
			}
			char *id = tt_message_id( dtTtMessages[ i ] );
			if (! tt_is_err( tt_ptr_error( id ))) {
				itemStream << " " << id;
				tt_free( id );
			}
			itemStream << ends;
			char *string = itemStream.str();
			items[ i ] = XmStringCreateLocalized( string );
			delete string;
		}
		return items;
	    case DTTT_PATTERN:
		items = (XmString *)
			XtMalloc( dtTtPatternsCount * sizeof( XmString ));
		if (items == 0) {
			return 0;
		}
		*itemCount = dtTtPatternsCount;
		for (i = 0; i < dtTtPatternsCount; i++) {
			std::ostrstream itemStream;
			itemStream << (void *)dtTtPatterns[ i ] << ends;
			items[ i ] = XmStringCreateLocalized(
					itemStream.str() );
			delete itemStream.str();
		}
		return items;
	    case DTTT_DTSESSION:
		*itemCount = dtTtDtSessionsCount;
		return _DtTtChoices( dtTtDtSessions, dtTtDtSessionsCount );
	    case DTTT_DTFILE:
		*itemCount = dtTtDtFilesCount;
		return _DtTtChoices( dtTtDtFiles, dtTtDtFilesCount );
	    case DTTT_OP:
		// XXX when to free? ditto for each case
		opCount = ((int)TTDT_OP_LAST) - 1;
		items = (XmString *)
			XtMalloc( opCount * sizeof( XmString ));
		if (items == 0) {
			return 0;
		}
		*itemCount = opCount;
		for (i = 1; i <= opCount; i++) {
			items[ i-1 ] = XmStringCreateLocalized(
					(String)tttk_op_string( (Tttk_op)i ));
		}
		return items;
	}
}

#if defined(aix)
#define AIX_CONST_STRING	(char *)
#else
#define AIX_CONST_STRING
#endif

void
_DtOpen(
	Widget		label,
	const char *	cmd,
	const char *	tempnamTemplate
)
{
    char *file = tempnam( 0, AIX_CONST_STRING tempnamTemplate );
    std::ostrstream cmdStream;
    cmdStream << cmd << " > " << file << ends;
    int sysStat = system( cmdStream.str() );
    if (! WIFEXITED( sysStat )) {
	    std::ostrstream func;
	    func << "system( \"" << cmdStream.str() << "\" )" << ends;
	    DtTtSetLabel( label, func.str(), sysStat );
	    delete cmdStream.str();
	    delete func.str();
	    return;
    }
    if (WEXITSTATUS( sysStat ) != 0) {
	    DtTtSetLabel( label, cmdStream.str(), WEXITSTATUS( sysStat ));
	    delete cmdStream.str();
	    return;
    }
    delete cmdStream.str();
    _DtOpen( label, file );
}

void
_DtOpen(
	Widget		label,
	const char *	file
)
{
    std::ostrstream labelStream;
    labelStream << "dtaction Open " << file << ends;
    DtTtSetLabel( label, labelStream.str() );
    delete labelStream.str();

    std::ostrstream cmd;
    cmd << "( unset TT_TRACE_SCRIPT; if dtaction Open " << file
	<< "; then :; else textedit " << file << "; fi; sleep 600; rm -f "
	<< file << " ) &" << ends;
    system( cmd.str() );
    delete cmd.str();
}

void
_DtOpen(
	Widget		label,
	void *		buf,
	size_t		len,
	const char *	tempnamTemplate
)
{
    char *file = tempnam( 0, AIX_CONST_STRING tempnamTemplate );
    int fd = open( file, O_WRONLY|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR );
    if (write( fd, buf, len ) < 0) {
	    DtTtSetLabel( label, file, errno );
	    return;
    }
    close( fd );
    _DtOpen( label, file );
}

void
_DtMan(
	Widget		label,
	const char *	topic
)
{
    std::ostrstream labelStream;
    labelStream << "dtaction Dtmanpageview " << topic << ends;
    DtTtSetLabel( label, labelStream.str() );
    delete labelStream.str();

    std::ostrstream cmd;
    cmd << "unset TT_TRACE_SCRIPT; if dtaction Dtmanpageview " << topic
	<< "; then :; else cmdtool -c man " << topic << "; fi &" << ends;
    system( cmd.str() );
    delete cmd.str();
}

Boolean
_DtCanHelp(
	const char *topics
)
{
	if (topics == 0) return False;
	if (strchr( topics, ' ' ) != 0) {
		// Must not be a list of man pages
		return False;
	}
	return True;
}

Boolean
_DtHelped(
	Widget helpDialog
)
{
	char *topics;
	XtVaGetValues( helpDialog, DtNstringData, &topics, NULL );
	if (! _DtCanHelp( topics )) {
		return False;
	}
	char *newTopics = strdup( topics );
	const char *whiteSpace = "(12345689) \t:-,.*\n";
	const char *topic = strtok( newTopics, whiteSpace );
	while (topic != 0) {
		_DtMan( 0, topic );
		topic = strtok( 0, whiteSpace );
	}
	free( newTopics );
	return True;
}
