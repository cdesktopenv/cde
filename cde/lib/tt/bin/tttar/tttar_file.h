/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tttar_file.h /main/3 1995/10/20 17:00:17 rswiston $ 			 				 */
/*
 * tttar_file.h - Interface to hash table of files for the Link Service archive
 *		 tool.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#ifndef _LSTAR_FILE_H
#define _LSTAR_FILE_H

#include <util/tt_object.h>
#include <util/tt_list.h>
#include <util/tt_string.h>
#include <util/tt_table.h>

class tttar_file : public _Tt_object {
    public:
	tttar_file();
	tttar_file( _Tt_string path );
	~tttar_file();

	_Tt_string		path();
	void			print(FILE *fs = stdout) const;
    private:
	_Tt_string		_path;
};

declare_list_of(tttar_file)
declare_table_of(tttar_file,path,_Tt_string)

#endif /* _LSTAR_FILE_H */
