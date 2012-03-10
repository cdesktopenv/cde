#! /bin/sh
# %%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
# %%  (c) Copyright 1993, 1994 International Business Machines Corp.	
# %%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
# %%  (c) Copyright 1993, 1994 Novell, Inc. 				
# %%  $XConsortium: mk_libtt_mapfile.sh /main/3 1995/10/23 09:49:33 rswiston $ 			 				
# @(#)mk_libtt_mapfile.sh	1.10 93/09/07
# List sections for reordering.  Args are names of object files or
# names of directories containing object files.
# output written to "mapfile".

trap "rm /tmp/listsec.$$.nawk" 0 1 2 3 4 5 6 7 8 19 12 13 15 16 17
rm -f mapfile-*
# Ensure -leftovers exist even if rule set is perfect.
touch mapfile-leftovers
while test "$1"
do	if 	test -d $1
	then	generator="ls $1/*.o"
	else	generator="echo $1"
	fi
	$generator | \
	  xargs dump -h | \
	   egrep -v '.rela.text%' | egrep '.text%' | \
	     cut -f5 -d'	' >> mapfile-in
	shift
done

# Now split up the functions into groups to some rather ad-hoc rules.
# mapfile-measured: from actual run time measurements
# mapfile-print: print functions (used only in debuggging)
# mapfile-oo: spec, etc. functions (used only for OO messaging)
# mapfile-file: file, etc.functions (used only for file scoping)
# mapfile-sess: functions needed for session scope messaging
# mapfile-init: functions needed only during startup
# mapfile-context: functions used only if contexts are
# mapfile-rare: functions that are expected to only be rarely used.
# mapfile-leftovers: functions not classified (should aim to reduce this)

cat >/tmp/listsec.$$.nawk <<'END_OF_NAWK'
function mfput(mf) { print "text:", $1, ";" >mf;}
# Mapfile-measured comes from actual benchmarking and so should
# be trusted more than the ad-hoc naming rules.
# Suck it in first for comparisons, and copy it in the order
# given by the measurements.
BEGIN {
	while(getline <"Mapfile-measured") {
		if ($0 !~ "^[ 	]*#") {
		        print "text:", $3, ";" >"mapfile-measured";
			measured[$3] = 1;
		}
	}
}
		
		{ if ($1 in measured ) { next ;}}
# For some reason cfront has started generating static versions of
# _Tt_object::_Tt_object() in a few functions.  Since we don't
# handle static functions in this script, just ignore them.
/__ct__10_Tt_objectFv/ { next; }
# print and enumname functions used only when tracing and debugging
/print/		{ mfput( "mapfile-rare"); next; }
/%_tt_enumname/	{ mfput( "mapfile-rare"); next; }
# _Tt_trace objects are constructed/destructed even if tracing is not on
/_Tt_trace/	{ mfput( "mapfile-sess"); next; }
/tt_status_message/{ mfput("mapfile-rare"); next; }
# session properties are a rarely used feature
/prop.*session/	{ mfput("mapfile-rare"); next; }
/_Tt_mp_prop/	{ mfput("mapfile-rare"); next; }
# host redirection is rare, too
/redirect/	{ mfput("mapfile-rare"); next; }
# Functions for talking to old dbservers should be rare soon
/_Tt_old/	{ mfput("mapfile-rare"); next; }
/_Tt_magic/	{ mfput("mapfile-rare"); next; }
/_Tt_table_desc/{ mfput("mapfile-rare"); next; }
/_Tt_server/	{ mfput("mapfile-rare"); next; }
/_Tt_buffer/	{ mfput("mapfile-rare"); next; }
/_Tt_trans_list/{ mfput("mapfile-rare"); next; }
/_Tt_client_isam/{ mfput("mapfile-rare"); next; }
/_old_tt_get_real/{ mfput("mapfile-rare"); next; }

# We hope nobody uses the drag-n-drop library
/dnd/		{ mfput( "mapfile-rare"); next; }
/tt_string_list/{ mfput( "mapfile-rare"); next; }
# desktop stuff is used only during startup
/desktop/ 	{ mfput( "mapfile-init"); next; }
/load_xlib/	{ mfput( "mapfile-init"); next; }
# _Tt_object classes are NOT oo messaging, but are the base utility classes
/_Tt_object/	{ mfput( "mapfile-sess"); next; }
# a _Tt_file_ptr gets constructed and destructed every time a message
# is sent, even a session scoped one.
/__(ct|dt)__12_Tt_file_ptrFv/ { mfput( "mapfile-sess"); next; }
# and _Tt_message::set_file is called to null out file attribute whenever
# a message is created.
/set_file__11_Tt_messageF10_Tt_string/ { mfput( "mapfile-sess"); next; }
# (empty) context lists are constructed/destructed even if no context in msg
/__(ct|dt)__.*context_list/ { mfput( "mapfile-sess"); next; }
/spec/		{ mfput( "mapfile-oo"); next; }
/node/		{ mfput( "mapfile-oo"); next; }
/oid|otype|super|obj/{ mfput("mapfile-oo"); next; }
/_Tt_key/	{ mfput("mapfile-oo"); next; }
/_Tt_mp_lock/	{ mfput("mapfile-oo"); next; }
/tt_file_(move|copy|destroy)/{ mfput("mapfile-oo"); next; }
/tt_(message|pattern)_file/{ mfput("mapfile-file"); next; }
/file/		{ mfput( "mapfile-file"); next; }
/_Tt_db/	{ mfput( "mapfile-file"); next; }
/_Tt_exports/	{ mfput( "mapfile-file"); next; }
/%_tt_is/	{ mfput( "mapfile-file"); next; }
/_Tt_mfs/	{ mfput( "mapfile-file"); next; }
/_Tt_exports/	{ mfput( "mapfile-file"); next; }
/_Tt_record/	{ mfput( "mapfile-file"); next; }
/_Tt_map_entry/	{ mfput( "mapfile-file"); next; }
/context/	{ mfput( "mapfile-context"); next; }
/_Tt_string/	{ mfput( "mapfile-sess"); next; }
/message|msg|notice|request/{ mfput( "mapfile-sess"); next; }
/session|procid/{ mfput( "mapfile-sess"); next; }
/pattern|patlist/{ mfput( "mapfile-sess"); next; }
/_pat_/		{ mfput( "mapfile-sess"); next; }
/%tt_|%_tt_|api/{ mfput("mapfile-sess"); next; }
/stream_socket/{ mfput("mapfile-sess"); next; }
/xdr/ 		{ mfput( "mapfile-sess"); next; }
/rpc_client/	{ mfput( "mapfile-sess"); next; }
/arg/		{ mfput( "mapfile-sess"); next; }
/_Tt_(int|pid_t)_rec/ { mfput( "mapfile-sess"); next; }
/_Tt_(new|allocated)/ { mfput( "mapfile-sess"); next; }
/_Tt_mp|_Tt_c_mp|_Tt_global|_Tt_host/	{ mfput( "mapfile-sess"); next; }
/_Tt_dirname|_Tt_basename/ { mfput( "mapfile-file"); next; }
		{ mfput( "mapfile-leftovers"); next; }
END_OF_NAWK

nawk -f /tmp/listsec.$$.nawk mapfile-in

rm mapfile-in
wc -l mapfile-* >&2

cat <<END_OF_PREFIX - mapfile-measured mapfile-sess mapfile-file mapfile-context mapfile-leftovers mapfile-oo mapfile-rare mapfile-init >Mapfile
text=LOAD ?RXO;
END_OF_PREFIX
exit 0
