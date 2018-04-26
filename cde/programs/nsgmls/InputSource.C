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
/* $XConsortium: InputSource.C /main/1 1996/07/29 16:55:11 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "InputSource.h"
#include "MarkupScan.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

InputSource::InputSource(InputSourceOrigin *origin, const Char *start,
			 const Char *end)
: origin_(origin), start_(start), end_(end), cur_(start), accessError_(0),
  startLocation_(origin, 0), multicode_(0), scanSuppress_(0),
  scanSuppressSingle_(false), scanSuppressIndex_(0)
{
}

void InputSource::reset(const Char *start,
			const Char *end)
{
  origin_ = origin_->copy();
  start_ = start;
  end_ = end;
  cur_ = start_;
  startLocation_ = Location(origin_.pointer(), 0);
  multicode_ = 0;
  scanSuppress_ = 0;
  markupScanTable_.clear();
}

InputSource::~InputSource()
{
}

void InputSource::advanceStartMulticode(const Char *to)
{
  while (start_ < to) {
    switch (markupScanTable_[*start_]) {
    case MarkupScan::normal:
      break;
    case MarkupScan::in:
      scanSuppress_ = 0;
      break;
    case MarkupScan::out:
      if (!scanSuppress()) {
	scanSuppress_ = 1;
	scanSuppressSingle_ = 0;
      }
      break;
    case MarkupScan::suppress:
      // what's the effect of MSSCHAR followed by MSSCHAR
      if (!scanSuppress()) {
	scanSuppress_ = 1;
	scanSuppressSingle_ = 1;
	scanSuppressIndex_ = startLocation_.index() + 1;
      }
      break;
    }
    start_++;
    startLocation_ += 1;
  }
}

void InputSource::willNotRewind()
{
}


#ifdef SP_NAMESPACE
}
#endif
