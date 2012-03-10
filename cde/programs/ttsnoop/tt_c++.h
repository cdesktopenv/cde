//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
//%%  (c) Copyright 1993, 1994 Novell, Inc.
//%%  $XConsortium: tt_c++.h /main/3 1995/11/06 17:14:37 rswiston $

#ifndef TT_CXX_H
#define TT_CXX_H

#include <iostream.h>
#include <Tt/tt_c.h>

ostream &	operator<<( ostream &, Tt_status );
ostream &	operator<<( ostream &, Tt_scope );
ostream &	operator<<( ostream &, Tt_class );
ostream &	operator<<( ostream &, Tt_mode );
ostream &	operator<<( ostream &, Tt_address );
ostream &	operator<<( ostream &, Tt_callback_action );
char *		tt_status_string( Tt_status );
Tt_message	tt_message_copy( Tt_message );
Tt_message	tttk_message_create( const char * );
int		tt_procid_fd( const char * );

#endif
