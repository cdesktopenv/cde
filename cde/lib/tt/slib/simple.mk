# %%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
# %%  (c) Copyright 1993, 1994 International Business Machines Corp.	
# %%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
# %%  (c) Copyright 1993, 1994 Novell, Inc. 				
# %%  $XConsortium: simple.mk /main/3 1995/10/23 12:02:59 rswiston $ 			 				
# @(#)simple.mk	1.6 93/09/07

TT_DIR	= ..

include $(TT_DIR)/simple-macros.mk

all:	libstt.a

LIBSTT_OBJECTS =\
mp_ce_attrs.o \
mp_observer.o \
mp_observer_utils.o \
mp_otype.o \
mp_otype_utils.o \
mp_ptype.o \
mp_ptype_utils.o \
mp_rpc_implement.o \
mp_rpc_server.o \
mp_rpc_server_utils.o \
mp_s_file.o \
mp_s_file_utils.o \
mp_s_message.o \
mp_s_message_utils.o \
mp_s_mp.o \
mp_s_msg_context.o \
mp_s_pat_context.o \
mp_s_pattern.o \
mp_s_pattern_utils.o \
mp_s_procid.o \
mp_s_procid_utils.o \
mp_s_session.o \
mp_s_session_prop.o \
mp_s_session_utils.o \
mp_s_xdr_functions.o \
mp_signature.o \
mp_signature_utils.o \
mp_typedb.o \
mp_typedb_utils.o

libstt.a:	$(LIBSTT_OBJECTS)
	$(RM) libstt.a; $(AR) libstt.a $(LIBSTT_OBJECTS)
	$(RANLIB) libstt.a

mp_ce_attrs.o:	mp_ce_attrs.C
	$(CCC) $(CCCFLAGS) -o mp_ce_attrs.o -c mp_ce_attrs.C

mp_observer.o:	mp_observer.C
	$(CCC) $(CCCFLAGS) -o mp_observer.o -c mp_observer.C

mp_observer_utils.o:	mp_observer_utils.C
	$(CCC) $(CCCFLAGS) -o mp_observer_utils.o -c mp_observer_utils.C

mp_otype.o:	mp_otype.C
	$(CCC) $(CCCFLAGS) -o mp_otype.o -c mp_otype.C

mp_otype_utils.o:	mp_otype_utils.C
	$(CCC) $(CCCFLAGS) -o mp_otype_utils.o -c mp_otype_utils.C

mp_ptype.o:	mp_ptype.C
	$(CCC) $(CCCFLAGS) -o mp_ptype.o -c mp_ptype.C

mp_ptype_utils.o:	mp_ptype_utils.C
	$(CCC) $(CCCFLAGS) -o mp_ptype_utils.o -c mp_ptype_utils.C

mp_rpc_implement.o:	mp_rpc_implement.C
	$(CCC) $(CCCFLAGS) -o mp_rpc_implement.o -c mp_rpc_implement.C

mp_rpc_server.o:	mp_rpc_server.C
	$(CCC) $(CCCFLAGS) -o mp_rpc_server.o -c mp_rpc_server.C

mp_rpc_server_utils.o:	mp_rpc_server_utils.C
	$(CCC) $(CCCFLAGS) -o mp_rpc_server_utils.o -c mp_rpc_server_utils.C

mp_s_file.o:	mp_s_file.C
	$(CCC) $(CCCFLAGS) -o mp_s_file.o -c mp_s_file.C

mp_s_file_utils.o:	mp_s_file_utils.C
	$(CCC) $(CCCFLAGS) -o mp_s_file_utils.o -c mp_s_file_utils.C

mp_s_message.o:	mp_s_message.C
	$(CCC) $(CCCFLAGS) -o mp_s_message.o -c mp_s_message.C

mp_s_message_utils.o:	mp_s_message_utils.C
	$(CCC) $(CCCFLAGS) -o mp_s_message_utils.o -c mp_s_message_utils.C

mp_s_mp.o:	mp_s_mp.C
	$(CCC) $(CCCFLAGS) -o mp_s_mp.o -c mp_s_mp.C

mp_s_msg_context.o:	mp_s_msg_context.C
	$(CCC) $(CCCFLAGS) -o mp_s_msg_context.o -c mp_s_msg_context.C

mp_s_pat_context.o:	mp_s_pat_context.C
	$(CCC) $(CCCFLAGS) -o mp_s_pat_context.o -c mp_s_pat_context.C

mp_s_pattern.o:	mp_s_pattern.C
	$(CCC) $(CCCFLAGS) -o mp_s_pattern.o -c mp_s_pattern.C

mp_s_pattern_utils.o:	mp_s_pattern_utils.C
	$(CCC) $(CCCFLAGS) -o mp_s_pattern_utils.o -c mp_s_pattern_utils.C

mp_s_procid.o:	mp_s_procid.C
	$(CCC) $(CCCFLAGS) -o mp_s_procid.o -c mp_s_procid.C

mp_s_procid_utils.o:	mp_s_procid_utils.C
	$(CCC) $(CCCFLAGS) -o mp_s_procid_utils.o -c mp_s_procid_utils.C

mp_s_session.o:	mp_s_session.C
	$(CCC) $(CCCFLAGS) -o mp_s_session.o -c mp_s_session.C

mp_s_session_prop.o:	mp_s_session_prop.C
	$(CCC) $(CCCFLAGS) -o mp_s_session_prop.o -c mp_s_session_prop.C

mp_s_session_utils.o:	mp_s_session_utils.C
	$(CCC) $(CCCFLAGS) -o mp_s_session_utils.o -c mp_s_session_utils.C

mp_s_xdr_functions.o:	mp_s_xdr_functions.C
	$(CCC) $(CCCFLAGS) -o mp_s_xdr_functions.o -c mp_s_xdr_functions.C

mp_signature.o:	mp_signature.C
	$(CCC) $(CCCFLAGS) -o mp_signature.o -c mp_signature.C

mp_signature_utils.o:	mp_signature_utils.C
	$(CCC) $(CCCFLAGS) -o mp_signature_utils.o -c mp_signature_utils.C

mp_typedb.o:	mp_typedb.C
	$(CCC) $(CCCFLAGS) -o mp_typedb.o -c mp_typedb.C

mp_typedb_utils.o:	mp_typedb_utils.C
	$(CCC) $(CCCFLAGS) -o mp_typedb_utils.o -c mp_typedb_utils.C

clean:
	$(RM) $(LIBSTT_OBJECTS) libstt.a

install:
