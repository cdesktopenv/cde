#!/bin/csh -f
# %%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
# %%  (c) Copyright 1993, 1994 International Business Machines Corp.	
# %%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
# %%  (c) Copyright 1993, 1994 Novell, Inc. 				
# %%  $XConsortium: mktrcfiles.sh /main/3 1995/10/23 10:36:11 rswiston $ 			 				

# C-shell script to write the tt_entry_pt.C and tt_entry_pt.h files
# based on the TT api calls in tt_c.h

# Edits to mktrcfiles will be lost -- only edit mktrcfiles.sh!

# To be called from lib/mp/Makefile only.  Usage:  mktrcfiles $TT_DIR
 
set TT_C_DIR = $1/lib/api/c
set BASENAME = tt_entry_pt

# Initialize:  overwrite any existing files

echo '#include "util/tt_string.h"' > $BASENAME.C
echo '#include "tt_entry_pt.h"' >> $BASENAME.C
echo "" >> $BASENAME.C
echo "_Tt_string _tt_entrypt_to_string(_Tt_entry_pt fun)" >> $BASENAME.C
echo "{" >> $BASENAME.C
echo "	switch (fun) {" >> $BASENAME.C
echo "#ifndef _TT_ENTRY_PT_H" >> $BASENAME.h
echo "#define _TT_ENTRY_PT_H" >> $BASENAME.h
echo "enum _Tt_entry_pt {" > $BASENAME.h
echo "static const char* _tt_entries[] = {" > ${BASENAME}_names.h

set nm_count = 0

# Grab the API calls from tt_c.h and format them

foreach line (`cat $TT_C_DIR/tt_c.h | grep "^_TT_EXTERN_FUNC" | awk -F, '{ print $2 }' | sort`)
	set upline = `echo $line | tr "[a-z]" "[A-Z]"`
	echo "        case $upline :" >> $BASENAME.C
	echo -n "                return " >> $BASENAME.C
	echo -n '"' >> $BASENAME.C
	echo -n $line >> $BASENAME.C
	echo '";' >> $BASENAME.C
	echo "     $upline ," >> $BASENAME.h
	echo -n '     "' >> ${BASENAME}_names.h
	echo -n $line >> ${BASENAME}_names.h
	echo '",' >> ${BASENAME}_names.h
	set nm_count = `expr $nm_count + 1`
end

# Write the tails of the files
				
echo "	case TT_API_CALL_LAST: return NULL; }}" >> $BASENAME.C
echo "     TT_API_CALL_LAST };" >> $BASENAME.h
echo "#endif" >> $BASENAME.h
echo "};" >> ${BASENAME}_names.h
echo "const int _tt_entries_count = $nm_count;" >> ${BASENAME}_names.h

exit 0
