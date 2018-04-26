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
// $XConsortium: XpmLib.cc /main/3 1996/06/11 16:30:30 cde-hal $
/*	Copyright (c) 1994,1995 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include <unistd.h>

#define C_XpmLib
#define C_WindowSystem
#define L_Other

#include <assert.h>
#include <stdlib.h>

#include <Prelude.h>

static XpmLib g_xpmlib;

#define XPMLIB_PREFIX		"libXpm.so"
#define XPMLIB_DEFAULT_PATH	"libXpm.so.4.1"

static char* default_path = XPMLIB_DEFAULT_PATH;

XpmLib* XpmLib::f_xpm_lib;

XpmLib::XpmLib(const char* arg_path, int open_mode) : f_handle(0)
{
	const char* path = 0;

	f_xpm_lib = this;

	if (arg_path)
	    path = arg_path;

	if (! path || *path == '\0')
	    path = default_path;

	if (! (f_handle = dlopen(path, open_mode)))
	    f_handle = dlopen(XPMLIB_PREFIX, open_mode);
}

XpmLib::~XpmLib()
{
	if (f_handle)
	    dlclose(f_handle);
}

void* XpmLib::symbol(const char* name)
{
	if (! f_handle || ! name || *name == '\0')
	    return 0;

	return dlsym(f_handle, name);
}

int XpmLib::CreatePixmapFromData(Display *display, Drawable d, char **data,
				Pixmap *pixmap_return, Pixmap *shapemask_return,
				XpmAttributes *attributes)
{
    int status;

    int (*func)(Display*, Drawable, char**, Pixmap*, Pixmap*, XpmAttributes*);

    func = (int (*)(Display*, Drawable, char**, Pixmap*, Pixmap*, XpmAttributes*)) symbol("XpmCreatePixmapFromData");

    if (func) {
	status = (*func)(display, d, data, pixmap_return,
				shapemask_return, attributes);
	return status;
    }
    else
	return 1;
}

int XpmLib::CreatePixmapFromBuffer(Display *display, Drawable d, char *buffer,
			Pixmap *pixmap_return, Pixmap *shapemask_return,
			XpmAttributes *attributes)
{
    int status;

    int (*func)(Display*, Drawable, char*, Pixmap*, Pixmap*, XpmAttributes*);

    func = (int (*)(Display*, Drawable, char*, Pixmap*, Pixmap*, XpmAttributes*)) symbol("XpmCreatePixmapFromBuffer");

    if (func) {
	status = (*func)(display, d, buffer, pixmap_return,
				shapemask_return, attributes);
	return status;
    }
    else
	return 1;
}

int XpmLib::ReadFileToPixmap(Display *display, Drawable d, char *filename,
		Pixmap *pixmap_return, Pixmap *shapemask_return,
		XpmAttributes *attributes)
{
    int status;

    int (*func)(Display*, Drawable, char*, Pixmap*, Pixmap*, XpmAttributes*);

    func = (int (*)(Display*, Drawable, char*, Pixmap*, Pixmap*, XpmAttributes*)) symbol("XpmReadFileToPixmap");

    if (func) {
	status = (*func)(display, d, filename, pixmap_return,
				shapemask_return, attributes);
	return status;
    }
    else
	return 1;
}
