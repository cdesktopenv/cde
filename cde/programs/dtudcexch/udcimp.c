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
/* $XConsortium: udcimp.c /main/4 1996/09/02 18:51:02 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "excutil.h"
#include "FaLib.h"
void addcharptn();
void setcodelists();

extern ListData	*ld;
extern int num_gr;
extern FalGlyphRegion  *gr;
extern FalFontData fullFontData;

void udcimp(Exc_data * ed)
{
    ed->function = IMPORT;
    strcpy(ed->bdfmode,"r");
    getbdffn(ed);
}

int checkcode(int code)
{
    int i, j;
    int code1, code2;

    for (i = 0; i < num_gr; i++) {
	code1 = smaller(gr[i].start, gr[i].end);
	code2 = bigger(gr[i].start, gr[i].end);
	if ((code >= code1) && (code <= code2)) { /*in udc area */
	    for (j = 0; j < ld->existcode_num; j++) {
		if (code == ld->existcode[j])
		    return (1); /* the font has same glyph index */
	    }
	    return (0);
	}
    }
    return (-1); /* out of udc area */
}


int checkcodes(int code_num, int * code_list)
{
    int *codep;
    int i, ans;
    int exist = 0;

    codep = code_list;
    for (i = 0; i < code_num; i++) {
	ans = checkcode(*codep);
	if (ans == -1) {
	    return (-1);
	} else if (ans == 1) {
	    exist = 1;
	}
	codep++;
    }
    return (exist);
}

void getcharcd(Exc_data * ed)
{
    int	code_num;
    int	*code_list;
    int	i;
    char *msg;
    char *msg2;
    char *msg3;
    int	ans, ans2;

    msg = GETMESSAGE(12, 2, "Glyph images in this BDF file can't be added to the font.");
    msg2 = GETMESSAGE(12, 4, "Failed to open the selected BDF font. You have no right to access for the font file, or the format of the file is not consistent.");
    msg3 = GETMESSAGE(12, 6, "There are one or more glyph images being overwritten. Overwrite?");

    i = ImpBDFCodeList(ed->bdffile, &code_num, &code_list);

    if (i != 0) { /* bad BDF format */
	AskUser(ed->toplevel, ed, msg2, &ans, "error");
	freeld(ld);
	excterminate(ed);
    }

    ans2 = checkcodes(code_num, code_list);
    if (ans2 == -1) { /* out of UDC code exists */
	AskUser(ed->toplevel, ed, msg, &ans, "error");
	freeld(ld);
	excterminate(ed);
    } else if (ans2 == 1) { /* font file has same glyph index */
	AskUser(ed->toplevel, ed, msg3, &ans, "warning");
	if (ans != 1) {
	    freeld(ld);
	    excterminate(ed);
	}
    }

/* set twe codelists */
    setcodelists(ld, code_num, code_list);

    if (ImpBDFCodeListFree(&code_list) != 0) {
	fprintf(stderr,"error in ImpBDFCodeListFree\n");
    }

/* add gryph images to font */
    addcharptn(ed);
}

void addcharptn(Exc_data * ed)
{
    int i;
    int ans;
    char *msg;
    char *msg2;
    char *msg3;

    int mask;
    FalFontDataList	*fulllist = NULL;
    FalFontID		fontid;

    msg = GETMESSAGE(12, 8, "Failed to open the selected font. You have no right to access for the font file, or the format of the file is not consistent.");
    msg2 = GETMESSAGE(12, 10, "Glyph images in this BDF file can't be added to the font.");
    msg3 = GETMESSAGE(12, 12, "Failed in the registration of the font file.");

    mask =  FAL_FONT_MASK_XLFDNAME | FAL_FONT_MASK_GLYPH_INDEX |
	    FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UPDATE |
	    FAL_FONT_MASK_UNDEFINED | FAL_FONT_MASK_CODE_SET;

    i = ImpBDFCheck(ed->bdffile, ed->fontfile);
    if (i != 0) {
	AskUser(ed->toplevel, ed, msg2, &ans, "error");
	excterminate(ed);
    }

    fontid = FalOpenSysFont(&fullFontData, mask, &fulllist);
    if (fontid == 0) {
	AskUser(ld->ed->toplevel, ld->ed, msg, &ans, "error");
	excterminate(ed);
    }
    i = 0;
    i = ImpBDFtoGpf(ed->bdffile, ed->fontfile,
		    ed->code_num, ed->bdf_code_list);
    FalCloseFont( fontid );
    if (i != 0) {
	AskUser(ed->toplevel, ed, msg3, &ans, "error");
    }

    excterminate(ed);
}

void setcodelists(ListData *ld, int code_num, int *code_list)
{
    int		*c1, *c2;
    int		i;
    Exc_data	*ed;

    ld->ed->code_num = code_num;

    if ((c1 = (int *) calloc(code_num, sizeof(int))) == NULL) {
	ed = ld->ed;
	freeld(ld);
	excerror(ed, EXCERRMALLOC, "setcodelists", "exit");
    }
    ld->ed->bdf_code_list = c1;
    c2 = code_list;
    for (i = 0; i < code_num; i++)
    {
	*c1 = *c2;
	c1++;
	c2++;
    }

    if ((c1 = (int *) calloc(code_num, sizeof(int))) == NULL) {
	ed = ld->ed;
	freeld(ld);
	excerror(ed, EXCERRMALLOC, "setcodelists", "exit");
    }
    ld->ed->gpf_code_list = c1;
    c2 = code_list;
    for (i = 0; i < code_num; i++)
    {
	*c1 = *c2;
	c1++;
	c2++;
    }
}
