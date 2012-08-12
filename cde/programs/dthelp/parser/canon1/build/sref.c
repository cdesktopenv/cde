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
/* $XConsortium: sref.c /main/3 1995/11/08 09:26:46 rswiston $ */
/* Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */

/* Sref.c contains procedures related to short reference map declarations */

#include <string.h>
#include <stdlib.h>
#include "build.h"
#include "sref.h"
#include "entdef.h"

static M_WCHAR *wc_prefix = NULL;

/* Add an entity with the default name constructed by adding a suffix
   to the name of the short reference map in which it is invoked, and
   a prefix m- */
void adddefent(mapname)
M_WCHAR *mapname;
{
M_WCHAR *p;
int n, length;
char c;

if (!wc_prefix)
    {
    wc_prefix = MakeWideCharString(M_PREFIX);
    w_strcpy(genname, wc_prefix);
    }

w_strcpy(&genname[w_strlen(wc_prefix)], mapname);
p = &genname[w_strlen(genname)];
for (n = egensuf ; n ; n /= 10)
    {
    if (p - genname > M_NAMELEN)
	{
	m_err1("Entity name based on %s too long", mapname);
	return;
	}
    c = ('0' + (n % 10));
    mbtowc(p, &c, 1);
    *p++;
    }
*p = M_EOS;
egensuf++;
if (! addent(genname))
    {
    m_err1("Default entity name %s already in use", genname);
    return;
    }
thissref->entidx = entity->index;
}

/* Add an entity, return FALSE if already there, TRUE if adding it.
   Pointer to the entity structure is in global M_STRUCT *entity. */
LOGICAL addent(name)
M_WCHAR *name;
{
M_ENTITY *new;

new = (M_ENTITY *) m_malloc(sizeof(M_ENTITY), "entity");
if (entity = (M_ENTITY *) m_ntrtrie(name, m_enttrie, (M_TRIE *) new))
    {
    m_free((M_POINTER) new, "entity");
    return(FALSE);
    }
entity = new;
if (lastent) lastent->next = entity;
else firstent = entity;
lastent = entity;
entity->type = M_GENERAL;
entity->wheredef = FALSE;
entity->content = NULL;
entity->name = (M_WCHAR *) m_malloc(w_strlen(name) + 1, "entity name");
entity->index = ++m_entcnt;
entity->next = NULL;
w_strcpy(entity->name, name);
return(TRUE);
}

/* Add a short reference map name */
#if defined(M_PROTO)
LOGICAL addmapname(M_WCHAR* p, LOGICAL define)
#else
LOGICAL addmapname(p, define)
M_WCHAR *p;
LOGICAL define;
#endif
{
MAP *old;
MAP *new;
LOGICAL retval;

new = (MAP *) m_malloc(sizeof(MAP), "map");
if (old = (MAP *) m_ntrtrie(p, &maptree, (M_TRIE *) new))
    {
    m_free(new, "map");
    curmap = old->map;
    retval = old->defined;
    if (define) old->defined = TRUE;
    return((LOGICAL) (retval ? FALSE : TRUE));
    }
curmap = ++mapcnt;
new->map = mapcnt;
new->mapname = (M_WCHAR *) m_malloc(w_strlen(p) + 1, "map name");
w_strcpy(new->mapname, p);
new->defined = define;
new->nextptr = NULL;
*nextmap = new;
nextmap = &new->nextptr;
return(TRUE);
}

/* Add a named entity to a short reference map */
void addndent(p)
M_WCHAR *p;
{
addent(p);
thissref->entidx = entity->index;
}

/* Add a short reference delimiter */
void addsref(p)
M_WCHAR *p;
{
SREFSTRUCT *delim;
SREFSTRUCT *prevsr;
M_WCHAR *q;
M_WCHAR c;
int noseq = 0;

/* Define the delimiter */
delim = (SREFSTRUCT *)
      m_malloc(sizeof(SREFSTRUCT), "short reference delimiter");
if (prevsr = (SREFSTRUCT *) m_ntrtrie(p, &sreftree, (M_TRIE *) delim))
    {
    m_free(delim, "short reference delimiter");
    delim = prevsr;
    }
else
    {
    for (q = p ; *q ; q++)
	{
	c = m_ctupper(*q);
	if (m_cttype(c) == M_NMSTART)
	    {
	    if (c != BLANKSEQ && c != WSSEQ && c != RS)
		{
		m_free(delim, "short reference delimiter");
		m_err1(
		    "Letters not permitted in short reference delimiters: %s",
		       p);
		return;
		}
	    if (c != RS) noseq++;
	    }
	}
    if (w_strlen(p) > maxsr) maxsr = w_strlen(p);
    if (noseq > maxseq) maxseq = noseq;
    delim->srefcnt = ++sreflen;
    *nextsref = delim;
    nextsref = &delim->next;
    delim->next = NULL;
    delim->data = NULL;
    }

/* Add the delimiter to this map and prepare for the associated
   entity */
thissref = (SREFDATA *)
	   m_malloc(sizeof(SREFDATA), "short reference delimiter");
thissref->map = curmap;
thissref->entidx = M_NULLVAL;
thissref->next = delim->data;
delim->data = thissref;
}

/* Prefix left-hand side with "M-" to create default short reference map
   name, truncating to M_NAMELEN characters if necessary */
void defmapname(M_NOPAR)
{
if (!wc_prefix)
    {
    wc_prefix = MakeWideCharString(M_PREFIX);
    w_strcpy(genname, wc_prefix);
    }

w_strcpy(&genname[w_strlen(wc_prefix)], thisrule);
genname[M_NAMELEN] = M_EOS;
}
