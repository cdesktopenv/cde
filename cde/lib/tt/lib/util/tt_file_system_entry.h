/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_file_system_entry.h /main/3 1995/10/23 10:38:32 rswiston $ 			 				 */
/* @(#)tt_file_system_entry.h	1.9 93/09/07
 * Tool Talk Utility - tt_file_system_entry.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declares a class representing an entry in the file system mount table.
 *
 */

#ifndef  _TT_FILE_SYSTEM_ENTRY_H
#define  _TT_FILE_SYSTEM_ENTRY_H

#include "util/tt_object.h"
#include "util/tt_string.h"

class _Tt_file_system_entry : public _Tt_object {
    public:
				_Tt_file_system_entry(
					const _Tt_string &hostname,
					const _Tt_string &mountpt,
					const _Tt_string &partition,
					int		  islocal,
					int		  isloopback
				);
				_Tt_file_system_entry();
				~_Tt_file_system_entry();

	_Tt_string      getHostname()	const { return entryHostname; }
	_Tt_string      getMountPoint()	const { return entryMountPoint; }
	_Tt_string      getPartition()	const { return entryPartition; }
	_Tt_string      getLoopBackMountPoint() const {
		return loopBackMountPoint; }
	bool_t		isLocal() const { return localFlag; }
	bool_t		isLoopBack() const{ return loopBackFlag; }

    private:
	_Tt_string		entryHostname;
	_Tt_string		entryMountPoint;
	_Tt_string		entryPartition;
	_Tt_string		loopBackMountPoint;
	bool_t			localFlag;
	bool_t			loopBackFlag;

    friend class _Tt_file_system;
};

#endif /* _TT_FILE_SYSTEM_ENTRY_H */
