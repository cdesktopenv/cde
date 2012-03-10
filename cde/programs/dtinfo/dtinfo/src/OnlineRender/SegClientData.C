/* $XConsortium: SegClientData.cc /main/3 1996/07/18 16:07:02 drk $ */

#include <stdlib.h>
#include <string.h>

#include "SegClientData.hh"

SegClientData::SegClientData(int type) : f_type(type)
{
    init();
}

void
SegClientData::init()
{
    if (f_type == _DtCvREGION)
	f_client_data.region.GraphicHandle = NULL;
    else if (f_type == _DtCvSTRING) {
	// (unsigned int)-1 represents invalid/uninitialized vcc and vclen
	f_client_data.string.vcc   = (unsigned int)-1;
	f_client_data.string.vclen = (unsigned int)-1;
	f_client_data.string.hilite_type = 0;

	f_client_data.string.bg_color = NULL;
	f_client_data.string.fg_color = NULL;
	f_client_data.string.bg_pixel = (unsigned long)-1;
	f_client_data.string.fg_pixel = (unsigned long)-1;
    }
    else {
	assert( f_type == _DtCvNOOP );
	memset(&f_client_data, 0, sizeof(_DtCvClientData));
    }
}

SegClientData::SegClientData(SegClientData& src) : f_type(src.f_type)
{
    if (f_type == _DtCvREGION) {
	f_client_data.region.GraphicHandle =
				src.f_client_data.region.GraphicHandle;
    }
    else if (f_type == _DtCvSTRING) {
	f_client_data.string.vcc   = src.f_client_data.string.vcc;
	f_client_data.string.vclen = src.f_client_data.string.vclen;
	f_client_data.string.hilite_type
				   = src.f_client_data.string.hilite_type;
	if (src.f_client_data.string.bg_color) {
	    f_client_data.string.bg_color =
				strdup(src.f_client_data.string.bg_color);
	}
	else {
	    f_client_data.string.bg_color = NULL;
	}
	if (src.f_client_data.string.fg_color) {
	    f_client_data.string.fg_color =
				strdup(src.f_client_data.string.fg_color);
	}
	else {
	    f_client_data.string.fg_color = NULL;
	}
	f_client_data.string.bg_pixel = src.f_client_data.string.bg_pixel;
	f_client_data.string.fg_pixel = src.f_client_data.string.fg_pixel;
    }
    else {
	assert( f_type == _DtCvNOOP );
	memset(&f_client_data, 0, sizeof(_DtCvClientData));
    }
}

// invalidate every field
void
SegClientData::clear(int type)
{
    // free referencing strings
    if (f_type == _DtCvSTRING) {
	if (f_client_data.string.bg_color) {
	    free(f_client_data.string.bg_color);
	    f_client_data.string.bg_color = NULL;
	}
	if (f_client_data.string.fg_color) {
	    free(f_client_data.string.fg_color);
	    f_client_data.string.fg_color = NULL;
	}
    }

    // initialize with specified type
    f_type = type;
    init();
}

SegClientData::~SegClientData()
{
    if (f_type == _DtCvREGION) {
	if (f_client_data.region.GraphicHandle)
	    f_client_data.region.GraphicHandle = NULL;
    }
    else if (f_type == _DtCvSTRING) {
	if (f_client_data.string.bg_color) {
	    free(f_client_data.string.bg_color);
	    f_client_data.string.bg_color = NULL;
	}
	if (f_client_data.string.fg_color) {
	    free(f_client_data.string.fg_color);
	    f_client_data.string.fg_color = NULL;
	}
    }
    else {
	assert( f_type == _DtCvNOOP );
    }
}

