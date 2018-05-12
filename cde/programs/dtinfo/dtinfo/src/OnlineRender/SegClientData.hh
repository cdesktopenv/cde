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
/*	$TOG: SegClientData.hh /main/5 1998/04/17 11:52:25 mgreess $ */
/*	Copyright (c) 1996 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __SEGCLIENTDATA_HH__
#define __SEGCLIENTDATA_HH__

#include <assert.h>
#include <stdlib.h>
#include <Dt/CanvasP.h>
#include <Dt/CanvasSegP.h>

#include "Exceptions.hh"

class SegClientData {

    _DtCvClientData f_client_data;

    int f_type;

    void init();

public:
    SegClientData(int type = _DtCvNOOP);
    SegClientData(SegClientData &);
    ~SegClientData();

    int type() { return f_type; }

    void clear(int type);

    void GraphicHandle(void* handle);
    void vcc(unsigned int n);
    void vclen(unsigned int);
    void hilite_type(unsigned int);
    void bg_color(char*);
    void fg_color(char*);
    void bg_pixel(unsigned long);
    void fg_pixel(unsigned long);

    void* GraphicHandle();
    unsigned int& vcc();
    unsigned int& vclen();
    unsigned int hilite_type();
    char* bg_color();
    char* fg_color();
    unsigned long bg_pixel();
    unsigned long fg_pixel();
};

inline void
SegClientData::GraphicHandle(void* handle)
{
    if (f_type != _DtCvREGION)
	throw(CASTEXCEPT Exception());
    else
	f_client_data.region.GraphicHandle = handle;
}

inline void*
SegClientData::GraphicHandle()
{
    if (f_type != _DtCvREGION) {
	throw(CASTEXCEPT Exception());
	return NULL;
    }
    else
	return f_client_data.region.GraphicHandle;
}

inline void
SegClientData::vcc(unsigned int n)
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else
	f_client_data.string.vcc = n;
}

inline unsigned int&
SegClientData::vcc()
{
    if (f_type != _DtCvSTRING) {
	throw(CASTEXCEPT Exception());
	return f_client_data.string.vcc;
    }
    else
	return f_client_data.string.vcc;
}

inline void
SegClientData::vclen(unsigned int n)
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else
	f_client_data.string.vclen = n;
}

inline unsigned int&
SegClientData::vclen()
{
    if (f_type != _DtCvSTRING) {
	throw(CASTEXCEPT Exception());
	return f_client_data.string.vclen;
    }
    else
	return f_client_data.string.vclen;
}

inline void
SegClientData::hilite_type(unsigned int type)
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else
	f_client_data.string.hilite_type = type;
}

inline unsigned int
SegClientData::hilite_type()
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else
	return f_client_data.string.hilite_type;
}

inline void
SegClientData::bg_color(char* col)
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else {
	if (f_client_data.string.bg_color != NULL)
	    free(f_client_data.string.bg_color);
	f_client_data.string.bg_color = col;
    }
}

inline char*
SegClientData::bg_color()
{
    if (f_type != _DtCvSTRING) {
#if 1
	abort();
#else
	throw(CASTEXCEPT Exception());
#endif
	return f_client_data.string.bg_color;
    }
    else
	return f_client_data.string.bg_color;
}

inline void
SegClientData::fg_color(char* col)
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else {
	if (f_client_data.string.fg_color != NULL)
	    free(f_client_data.string.fg_color);
	f_client_data.string.fg_color = col;
    }
}

inline char*
SegClientData::fg_color()
{
    if (f_type != _DtCvSTRING) {
	throw(CASTEXCEPT Exception());
	return f_client_data.string.fg_color;
    }
    else
	return f_client_data.string.fg_color;
}

inline void
SegClientData::bg_pixel(unsigned long pixel)
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else
	f_client_data.string.bg_pixel = pixel;
}

inline unsigned long
SegClientData::bg_pixel()
{
    if (f_type != _DtCvSTRING) {
	throw(CASTEXCEPT Exception());
	return f_client_data.string.fg_pixel;
    }
    else
	return f_client_data.string.bg_pixel;
}

inline void
SegClientData::fg_pixel(unsigned long pixel)
{
    if (f_type != _DtCvSTRING)
	throw(CASTEXCEPT Exception());
    else
	f_client_data.string.fg_pixel = pixel;
}

inline unsigned long
SegClientData::fg_pixel()
{
    if (f_type != _DtCvSTRING) {
	throw(CASTEXCEPT Exception());
	return f_client_data.string.fg_pixel;
    }
    else
	return f_client_data.string.fg_pixel;
}

#endif











