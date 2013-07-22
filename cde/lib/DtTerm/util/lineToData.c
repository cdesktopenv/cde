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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: lineToData.c /main/1 1996/04/21 19:20:58 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "TermPrimLineFontP.h"

int ParseChar(char **str);
int parseCoord(char **str, char *val, signed char *offset);
char * parseToken(char **str);
void displayErrorString(FILE *f, char *orig, char *remain);
static void DumpChar(FILE *f, int charNum, charType *charList, int charListMax,
	rect *rectList, int rectListMax, line *lineList, int lineListMax,
	line *stippleList, int stippleListMax);
static void DumpGlyphs(FILE *f, char *prefix,
	GlyphInfo glyphList, int glyphListMax);
static int parseLine(char **bufPtr, line *line);
static int parseRectangle(char **bufPtr, rect *rectangle);
static void order(char *p1, char *p2, signed char *offset1, signed char *offset2);
static char *vis(char val);

int
main(argc, argv)
int argc;
char **argv;
{
    int charNum = 0;
    char buffer[BUFSIZ];
    char *bufPtr;
    char orig[BUFSIZ];
    register int i;
    register char *c;
    FILE *f;
    FILE *tmp;
    extern int getopt();
    extern int optind;
    extern char *optarg;
    extern int opterr;
    int resolution = 75;		/* dots per inch		*/
    int points;
    char *prefix = "_Term";

    charType *charList = (charType *) 0;
    int charListSize = 0;
    int charListMax = 0;

    rect *rectList = (rect *) 0;
    int rectListSize = 0;
    int rectListMax = 0;

    line *stippleList = (line *) 0;
    int stippleListSize = 0;
    int stippleListMax = 0;

    line *lineList = (line *) 0;
    int lineListSize = 0;
    int lineListMax = 0;

    GlyphInfo glyphList = (GlyphInfo) 0;
    int glyphListSize = 0;
    int glyphListMax = 0;

    int charCount = 0;			/* total number of chars in font*/

    char *datafilename = "linegen.data";
    int dummyInt;
    int error;

    while (EOF != (i = getopt(argc, argv, "p:f:"))) {
	switch (i) {
	case 'f' :
	    datafilename = optarg;
	    break;

	case 'p' :
	    prefix = optarg;
	    break;
	}
    }

    if (NULL == (f = fopen(datafilename, "r"))) {
	(void) perror(datafilename);
	(void) exit(1);
    }

    /* read through the file and generate each character...
     */
    while (fgets(buffer, sizeof(buffer), f)) {
	/* null out '\n'... */
	buffer[strlen(buffer) - 1] = '\0';

	/* if the line begins with '#', ignore it... */
	if (*buffer == '#')
	    continue;

	/* if the line begins with a '!', just print it out (with the
	 * '!' removed... */

	if (*buffer == '!') {
	    (void) fputs(buffer + 1, stdout);
	    (void) putc('\n', stdout);
	    continue;
	}

	/* back up buffer... */
	(void) strcpy(orig, buffer);

	bufPtr = buffer;

	/* pull off token... */
	c = parseToken(&bufPtr);

	/* ignore blank lines... */
	if (!c || !*c)
	    continue;

	/* process the token... */
	if (!strcmp(c, "char")) {
	    /* dump the previous character... */
	    if (charListMax > 0) {
		(void) DumpChar(stdout, charCount++, charList, charListMax,
			 rectList, rectListMax, lineList, lineListMax,
			 stippleList, stippleListMax);
		lineListMax = 0;
		rectListMax = 0;
		stippleListMax = 0;
	    }

	    /* grow the glyph list if necessary... */
	    if (glyphListMax + 1 > glyphListSize) {
		glyphListSize += 10;
		glyphList = (GlyphInfo) realloc(glyphList,
			glyphListSize * sizeof(GlyphInfoRec));
	    }
	    /* bump count.  We need to remember that this is one high
	     * when we use it as an index...
	     */
	    glyphList[glyphListMax].numRects = 0;
	    glyphList[glyphListMax].numLines = 0;
	    glyphList[glyphListMax].numStipples = 0;
	    (void) glyphListMax++;

	    /* this is a list of characters... */
	    charListMax = 0;
	    charListSize = 10;
	    charList = (charType *)
		    malloc((unsigned) (charListSize * sizeof(charType)));

	    while (*bufPtr) {
		/* extend the charList if we need to (the 2 is 1 for this
		 * entry and 1 for the terminating 0...
		 */
		if (charListMax + 2 > charListSize) {
		    charListSize += 10;
		    charList = (charType *) realloc(charList,
			    charListSize * sizeof(charType));
		}

		if (0 == (charList[charListMax++] = ParseChar(&bufPtr))) {
		    (void) fprintf(stderr, "invalid \"char\" specification:\n");
		    (void) displayErrorString(stderr, orig, bufPtr);
		    (void) exit(1);
		}

		if (bufPtr && *bufPtr) {
		    if (*bufPtr == ',') {
			/* additional stuff... */
			(void) bufPtr++;
			/* skip whitespace... */
			while (*bufPtr && strchr(" \t", *bufPtr))
			    (void) bufPtr++;
		    } else {
			/* error -- invalid character... */
			(void) fprintf(stderr,
				"invalid \"char\" specification:\n");
			(void) displayErrorString(stderr, orig, bufPtr);
			(void) exit(1);
		    }
		}
	    }

	    /* null term the list... */
	    charList[charListMax] = 0;

	    /* save away the char list... */
	    glyphList[glyphListMax - 1].chars = charList;

	} else if (!strcmp(c, "rect")) {
	    if (rectListMax + 1 > rectListSize) {
		rectListSize += 10;
		rectList = (rect *) realloc(rectList,
			rectListSize * sizeof(rect));
	    }
		
	    if (parseRectangle(&bufPtr, &rectList[rectListMax])) {
		(void) fprintf(stderr, "invalid \"rect\" specification:\n");
		(void) displayErrorString(stderr, orig, bufPtr);
		(void) exit(1);
	    }

	    /* increment line count... */
	    (void) rectListMax++;

	    /* save away the rect list... */
	    glyphList[glyphListMax - 1].rects = rectList;
	    glyphList[glyphListMax - 1].numRects = rectListMax;

	} else if (!strcmp(c, "line")) {
	    if (lineListMax + 1 > lineListSize) {
		lineListSize += 10;
		lineList = (line *) realloc(lineList,
			lineListSize * sizeof(line));
	    }

	    /* did we error?... */
	    if (parseLine(&bufPtr, &lineList[lineListMax])) {
		(void) fprintf(stderr, "invalid \"line\" specification:\n");
		(void) displayErrorString(stderr, orig, bufPtr);
		(void) exit(1);
	    }

	    /* increment line count... */
	    (void) lineListMax++;

	    /* save away the line list... */
	    glyphList[glyphListMax - 1].lines = lineList;
	    glyphList[glyphListMax - 1].numLines = lineListMax;

	} else if (!strcmp(c, "stipple")) {
	    if (stippleListMax + 1 > stippleListSize) {
		stippleListSize += 10;
		stippleList = (line *) realloc(stippleList,
			stippleListSize * sizeof(line));
	    }
		
	    if (parseLine(&bufPtr, &stippleList[stippleListMax])) {
		(void) fprintf(stderr, "invalid \"stipple\" specification:\n");
		(void) displayErrorString(stderr, orig, bufPtr);
		(void) exit(1);
	    }


	    /* increment line count... */
	    (void) stippleListMax++;

	    /* save away the line list... */
	    glyphList[glyphListMax - 1].stipples = stippleList;
	    glyphList[glyphListMax - 1].numStipples = stippleListMax;

	} else {
	    /* unknown token... */
	    (void) fprintf(stderr, "invalid token:\n");
	    (void) displayErrorString(stderr, orig, orig);
	    (void) exit(1);
	}
    }

    /* dump out the last character(s)... */
    if (charListMax > 0) {
	(void) DumpChar(stdout, charCount++, charList, charListMax,
		 rectList, rectListMax, lineList, lineListMax,
		 stippleList, stippleListMax);
	lineListMax = 0;
	rectListMax = 0;
	stippleListMax = 0;
	charListMax = 0;
    }

    /* build the final structure... */
    (void) DumpGlyphs(stdout, prefix, glyphList, glyphListMax);

    (void) exit(0);
}

int
ParseChar(char **str)
{
    register char *c = *str;
    char *ptr;
    int ret = 0;

    while (*c && strchr(" \t", *c))
	c++;

    if (*c == '\'') {
	/* char format (i.e., 'c')...
	 */

	/* skip over '\''... */
	(void) c++;
	if (*c == '\\') {
	    /* quoted character... */
	    /* skip over '\\'... */
	    (void) c++;
	    switch (*c++) {
	    case 'n' :
		ret = '\n';
		break;
	    case 'f' :
		ret = '\f';
		break;
	    case 'r' :
		ret = '\r';
		break;
	    case 'b' :
		ret = '\b';
		break;
	    case 't' :
		ret = '\t';
		break;
	    case '\'' :
		ret = '\'';
		break;
	    case '\\' :
		ret = '\\';
		break;
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
		/* backup... */
		(void) c--;
		ret = (int) strtol(c, &ptr, 8);
		c = ptr;
		break;
	    default :
		/* error... */
		*str = c - 1;
		return(0);
		break;
	    }
	} else {
	    /* unquoted char... */
	    ret = *c++;
	}

	/* need to finish parsing the closing quote... */
	if (*c++ != '\'') {
	    *str = c - 1;
	    return(0);
	}
    } else if ((*c >= '0') || (*c <= '9')) {
	/* a number...
	 */
	ret = (int) strtol(c, &ptr, 0);
    }

    /* skip over whitespace... */
    while (*c && strchr(" \t", *c))
	c++;

    /* update the pointer... */
    *str = c;
    return(ret);
}

int
parseCoord(char **str, char *val, signed char *offset)
{
    register char *c = *str;
    char *ptr;
    int sign = 1;

    /* skip over whitespace... */
    while (*c && strchr(" \t", *c))
	c++;

    /* end of line?... */
    if (!*c) {
	*str = (char *) 0;
	return(-1);
    }

    /* parse off number... */
    *val = (char) strtol(c, &ptr, 0);
    c = ptr;

    /* skip over whitespace... */
    while (c && *c && strchr(" \t", *c))
	c++;

    /* parse off offset... */
    if (c && ((*c == '-') || (*c == '+'))) {
	if (*c == '-')
	    sign = -1;
	/* skip over sign... */
	(void) c++;
	/* skip over shitespace... */
	while (*c && strchr(" \t", *c))
	    c++;

	/* parse off offset... */
	*offset = (char) strtol(c, &ptr, 0);
	c = ptr;

	/* set sign of offset... */
	*offset *= sign;

	/* skip over whitespace... */
	while (c && *c && strchr(" \t", *c))
	    c++;
    } else {
	*offset = 0;
    }

    /* set up return ptr... */
    *str = c;

    return(0);
}

char *
parseToken(char **str)
{
    register char *c = *str;
    char *ret;

    /* skip white space... */
    while (*c && strchr(" \t", *c))
	c++;

    /* we hit start of token... */
    ret = c;
    while (*c && !strchr(" \t", *c))
	c++;

    /* null out the first whitespace... */
    if (*c)
	*c++ = '\0';

    /* skip white space... */
    while (*c && strchr(" \t", *c))
	c++;

    *str = c;
    return(ret);
}

void
displayErrorString(FILE *f, char *orig, char *remain)
{
    register int col;

    (void) fprintf(f, "        %s\n", orig);
    if (!remain || !*remain) {
	col = strlen(orig);
    } else {
	col = strlen(orig) - strlen(remain);
	if (col < 0)
	    /* this should not happen... */
	    col = 0;
    }

    /* add 8 for the indent... */
    col += 8;

    /* output space... */
    while (col-- > 0)
	(void) putc(' ', f);
    /* output an '^'... */
    (void) putc('^', f);
    (void) putc('\n', f);
}

static int
parseLine(char **bufPtr, line *line)
{
    signed char dummyChar;
    int error = 0;

    /* set the bits that correspond to this line... */
    /* parse off x1... */
    error = parseCoord(bufPtr, &(line->x1),
	    &(line->x1Offset));

    /* parse off y1... */
    if (!error)
	error = parseCoord(bufPtr, &(line->y1),
		&(line->y1Offset));

    /* parse off x2... */
    if (!error)
	error = parseCoord(bufPtr, &(line->x2),
		&(line->x2Offset));

    /* parse off y2... */
    if (!error)
	error = parseCoord(bufPtr, &(line->y2),
		&(line->y2Offset));

    /* parse off lineWidth... */
    if (!error)
	error = parseCoord(bufPtr, &(line->width),
		&dummyChar);

    /* did we error?... */
    if (error) {
	return(1);
    }

    /* order x and y coords... */
    (void) order(&(line->x1),
	    &(line->x2),
	    &(line->x1Offset),
	    &(line->x2Offset));
    (void) order(&(line->y1),
	    &(line->y2),
	    &(line->y1Offset),
	    &(line->y2Offset));

    return(0);
}

static int
parseRectangle(char **bufPtr, rect *rectangle)
{
    int error = 0;

    /* parse off x1... */
    error = parseCoord(bufPtr, &(rectangle->x1),
	    &(rectangle->x1Offset));

    /* parse off y1... */
    if (!error)
	error = parseCoord(bufPtr, &(rectangle->y1),
		&(rectangle->y1Offset));

    /* parse off x2... */
    if (!error)
	error = parseCoord(bufPtr, &(rectangle->x2),
		&(rectangle->x2Offset));

    /* parse off y2... */
    if (!error)
	error = parseCoord(bufPtr, &(rectangle->y2),
		&(rectangle->y2Offset));

    /* did we error?... */
    if (error) {
	return(1);
    }

    /* order x and y coords... */
    (void) order(&(rectangle->x1),
	    &(rectangle->x2),
	    &(rectangle->x1Offset),
	    &(rectangle->x2Offset));
    (void) order(&(rectangle->y1),
	    &(rectangle->y2),
	    &(rectangle->y1Offset),
	    &(rectangle->y2Offset));

    return(0);
}

static void
order(char *p1, char *p2, signed char *offset1, signed char *offset2)
{
    char swap;

    /* order coords... */
    if (*p1 > *p2) {
	/* swap p1 & p1 */
	swap = *p1;
	*p1 = *p2;
	*p2 = swap;

	swap = *offset1;
	*offset1 = *offset2;
	*offset2 = swap;
    }
}

static void
DumpGlyphs(FILE *f, char *prefix, GlyphInfo glyphList, int glyphListMax)
{
    int i1;
    int i2;

    /* print the full glyph array... */

    (void) fprintf(f, "int %sNumGlyphs = %d;\n", prefix, glyphListMax);
    (void) fprintf(f, "GlyphInfoRec %sGlyphs[] = {\n", prefix);
    for (i1 = 0; i1 < glyphListMax; i1++) {
	/* print the characters this is for... */
	(void) fputs("    /* data for ", f);
	for (i2 = 0; glyphList[i1].chars[i2] != 0; i2++) {
	    if (i2 > 0) {
		(void) putc(',', f);
	    }
	    (void) fputs(vis(glyphList[i1].chars[i2]), f);
	    (void) putc(' ', f);
	}
	(void) fputs("*/\n", f);
	(void) fprintf(f, "    {\n");

	(void) fprintf(f, "        chars%03d,           /* char list */\n",
		i1);

	if (glyphList[i1].numRects > 0) {
	    (void) fprintf(f, "        rects%03d,    %2d,", i1,
		    glyphList[i1].numRects);
	} else {
	    (void) fprintf(f, "        (rect *) 0,   0,");
	}
	(void) fprintf(f, "    /* rectangle list */\n");

	if (glyphList[i1].numLines > 0) {
	    (void) fprintf(f, "        lines%03d,    %2d,", i1,
		    glyphList[i1].numLines);
	} else {
	    (void) fprintf(f, "        (line *) 0,   0,");
	}
	(void) fprintf(f, "    /* line list */\n");

	if (glyphList[i1].numStipples > 0) {
	    (void) fprintf(f, "        stipples%03d, %2d,", i1,
		    glyphList[i1].numStipples);
	} else {
	    (void) fprintf(f, "        (line *) 0,   0,");
	}
	(void) fprintf(f, "    /* stipple list */\n");
	(void) printf("    },\n\n");
    }
    (void) printf("};\n");
}

static void
DumpChar(FILE *f, int charCount, charType *charList, int charListMax,
	rect *rectList, int rectListMax, line *lineList, int lineListMax,
	line *stippleList, int stippleListMax)
{
    int i;

    /* print the characters this is for... */
    (void) fputs("/* data for ", f);
    for (i = 0; i < charListMax; i++) {
	if (i > 0) {
	    (void) putc(',', f);
	}
	(void) fputs(vis(charList[i]), f);
	(void) putc(' ', f);
    }
    (void) fputs("*/\n", f);

    (void) fprintf(f, "static charType chars%03d[] = {", charCount);
    for (i = 0; i < charListMax; i++) {
	(void) fprintf(f, "0%03o, ", (int) charList[i]);
    }
    (void) fprintf(f, "0};\n");

    if (rectListMax > 0) {
	(void) fprintf(f, "static rect rects%03d[] = {\n", charCount);
	for (i = 0; i < rectListMax; i++) {
	    (void) fprintf(f, "    {%d, %d, %d, %d, %d, %d, %d, %d},\n",
		    rectList[i].x1, rectList[i].x1Offset,
		    rectList[i].y1, rectList[i].y1Offset,
		    rectList[i].x2, rectList[i].x2Offset,
		    rectList[i].y2, rectList[i].y2Offset);
	}
	(void) fprintf(f, "};\n");
    }

    if (lineListMax > 0) {
	(void) fprintf(f, "static line lines%03d[] = {\n", charCount);
	for (i = 0; i < lineListMax; i++) {
	    (void) fprintf(f, "    {%d, %d, %d, %d, %d, %d, %d, %d, %d},\n",
		    lineList[i].x1, lineList[i].x1Offset,
		    lineList[i].y1, lineList[i].y1Offset,
		    lineList[i].x2, lineList[i].x2Offset,
		    lineList[i].y2, lineList[i].y2Offset,
		    lineList[i].width);
	}
	(void) fprintf(f, "};\n");
    }

    if (stippleListMax > 0) {
	(void) fprintf(f, "static line stipples%03d[] = {\n", charCount);
	for (i = 0; i < stippleListMax; i++) {
	    (void) fprintf(f, "    {%d, %d, %d, %d, %d, %d, %d, %d, %d},\n",
		    stippleList[i].x1, stippleList[i].x1Offset,
		    stippleList[i].y1, stippleList[i].y1Offset,
		    stippleList[i].x2, stippleList[i].x2Offset,
		    stippleList[i].y2, stippleList[i].y2Offset,
		    stippleList[i].width);
	}
	(void) fprintf(f, "};\n");
    }
    (void) fprintf(f, "\n");
}

static char *
vis(char val)
{
    char buffer[BUFSIZ];

    if (isprint(val)) {
	if (val == '^') {
	    (void) strcpy(buffer, "'^^'");
	} else if (val == '\\') {
	    (void) strcpy(buffer, "'\\\\'");
	} else if (val == '\'') {
	    (void) strcpy(buffer, "'\\\''");
	} else {
	    (void) sprintf(buffer, "'%c'", val);
	}
    } else if (iscntrl(val)) {
	(void) sprintf(buffer, "'^%c'", val);
    } else {
	(void) sprintf(buffer, "'\\%03o'", val);
    }

    return(strdup(buffer));
}
