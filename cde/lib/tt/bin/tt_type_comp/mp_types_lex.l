/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_types_lex.l /main/3 1995/10/20 16:38:50 rswiston $ 			 				 */
%{
#include <stdlib.h>
#include "mp_types_gram.h"
#include "mp_types_table.h"
extern "C" {
int input(void);
void output(int);
void unput(int);
}
static void skip_c_comments(void);
static void skip_cplusplus_comments(void);
%}

%%

"/*"				skip_c_comments();
"//"				skip_cplusplus_comments();
otype				{ return OTYPE; }
inherit				{ return INHERIT; }
from				{ return FROM; }
ptype				{ return PTYPE; }
per_file			{ return PER_FILE; }
per_session			{ return PER_SESSION; }
start				{ return START; }
observe				{ return OBSERVE; }
handle				{ return HANDLE; }
handle_push			{ return HANDLE_PUSH; }
handle_rotate			{ return HANDLE_ROTATE; }
file				{ return FILE_SCOPE; }
session				{ return SESSION_SCOPE; }
file_in_session                 { return FILE_IN_SESSION; }
request				{ return REQUEST; }
notification			{ return NOTIFICATION; }
void				{ return VOID_ARGS; }
in				{ return IN; }
out				{ return OUT; }
inout				{ return INOUT; }
queue				{ return QUEUE; }
opnum				{ return OPNUM; }
context				{ return CONTEXT; }
":"				{ return COLON; }
";"				{ return SEMICOLON; }
"{"				{ return LCURL; }
"}"				{ return RCURL; }
"=>"				{ return INFER; }
"("				{ return LPAREN; }
")"				{ return RPAREN; }
","				{ return COMMA; }
"="				{ return EQUAL; }

[a-zA-Z][-_a-zA-Z0-9]*		{ return TT_IDENTIFIER; }
\"[^"]*\" 			{ return TT_STRING; }
[0-9]+				{ return TT_NUMBER; }

[ \t\n]	;

%%

					     

