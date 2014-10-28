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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: pr.c /main/21 1998/02/06 11:10:30 kaleb $ */
/*

Copyright (c) 1993, 1994, 1998 The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/

#include "def.h"

extern struct	inclist	inclist[ MAXFILES ],
			*inclistp;
extern char	*objprefix;
extern char	*objsuffix;
extern int	width;
extern boolean	printed;
extern boolean	verbose;
extern boolean	show_where_not;

void
add_include(filep, file, file_red, include, dot, failOK)
	struct filepointer	*filep;
	struct inclist	*file, *file_red;
	char	*include;
	boolean	dot;
	boolean	failOK;
{
	register struct inclist	*newfile;
	register struct filepointer	*content;

	/*
	 * First decide what the pathname of this include file really is.
	 */
	newfile = inc_path(file->i_file, include, dot);
	if (newfile == NULL) {
		if (failOK)
		    return;
		if (file != file_red)
			warning("%s (reading %s, line %d): ",
				file_red->i_file, file->i_file, filep->f_line);
		else
			warning("%s, line %d: ", file->i_file, filep->f_line);
		warning1("cannot find include file \"%s\"\n", include);
		show_where_not = TRUE;
		newfile = inc_path(file->i_file, include, dot);
		show_where_not = FALSE;
	}

	if (newfile) {
		included_by(file, newfile);
		if (!(newfile->i_flags & SEARCHED)) {
			newfile->i_flags |= SEARCHED;
			content = getfile(newfile->i_file);
			find_includes(content, newfile, file_red, 0, failOK);
			freefile(content);
		}
	}
}

void
pr(ip, file, base)
	register struct inclist  *ip;
	char	*file, *base;
{
	static char	*lastfile;
	static int	current_len;
	register int	len, i;
	char	buf[ BUFSIZ ];

	printed = TRUE;
	len = strlen(ip->i_file)+1;
	if (current_len + len > width || file != lastfile) {
		lastfile = file;
		snprintf(buf, BUFSIZ, "\n%s%s%s: %s", objprefix, base,
				objsuffix, ip->i_file);
		len = current_len = strlen(buf);
	}
	else {
		buf[0] = ' ';
		strncpy(buf+1, ip->i_file, BUFSIZ - 1);
		current_len += len;
	}
	fwrite(buf, len, 1, stdout);

	/*
	 * If verbose is set, then print out what this file includes.
	 */
	if (! verbose || ip->i_list == NULL || ip->i_flags & NOTIFIED)
		return;
	ip->i_flags |= NOTIFIED;
	lastfile = NULL;
	printf("\n# %s includes:", ip->i_file);
	for (i=0; i<ip->i_listlen; i++)
		printf("\n#\t%s", ip->i_list[ i ]->i_incstring);
}

void
recursive_pr_include(head, file, base)
	register struct inclist	*head;
	register char	*file, *base;
{
	register int	i;

	if (head->i_flags & MARKED)
		return;
	head->i_flags |= MARKED;
	if (head->i_file != file)
		pr(head, file, base);
	for (i=0; i<head->i_listlen; i++)
		recursive_pr_include(head->i_list[ i ], file, base);
}
