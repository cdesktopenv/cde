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
/* $XConsortium: StorageObjectPosition.h /main/1 1996/07/29 17:05:06 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef StorageObjectPosition_INCLUDED
#define StorageObjectPosition_INCLUDED 1

#include "Boolean.h"
#include "types.h"
#include "Owner.h"
#include "CodingSystem.h"
#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct StorageObjectPosition {
  StorageObjectPosition();
  // the number of RSs preceding line 1 of this storage object
  // or -1 if this hasn't been computed yet.
  size_t line1RS;
  Owner<Decoder> decoder;
  // Does the storage object start with an RS?
  PackedBoolean startsWithRS;
  // Were the RSs other than the first in the storage object inserted?
  PackedBoolean insertedRSs;
  Offset endOffset;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not StorageObjectPosition_INCLUDED */
