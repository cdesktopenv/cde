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
/* $XConsortium: rescan.c /main/1 1996/04/21 19:24:17 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "rerule.h"
#include "reparser.h"

#define EOL			0

#define ENDMARKERSYMBOL		'$'
#define DURATIONSYMBOL		'#'
#define FRONTWEEKSYMBOL		'+'
#define REARWEEKSYMBOL		'-'
#define MINUTESYMBOL		'M'
#define DAILYSYMBOL		'D'
#define WEEKLYSYMBOL		'W'
#define LASTDAYSYMBOL		"LD"
#define MONTHPOSSYMBOL		"MP"
#define MONTHDAYSYMBOL		"MD"
#define YEARDAYSYMBOL		"YD"
#define YEARMONTHSYMBOL		"YM"
#define SUNSYMBOL		"SU"
#define MONSYMBOL		"MO"
#define TUESYMBOL		"TU"
#define WEDSYMBOL		"WE"
#define THUSYMBOL		"TH"
#define FRISYMBOL		"FR"
#define SATSYMBOL		"SA"

static int scanbuf(char **inbuf, char *, int *);

/* The rule that needs to be parsed is passed to yylex() through this var */
char     *_DtCm_rule_buf;

char      _DtCm_yytext[128];

int
_DtCm_yylex(void)
{
	int	  token = 0;
	int	  yylen = 128;

	_DtCm_yylval.number = 0;

	token = scanbuf(&_DtCm_rule_buf, _DtCm_yytext, &yylen);
	if (token == NUMBER)
		sscanf(_DtCm_yytext, "%d", &_DtCm_yylval.number);
	if (token == DATE)
		strcpy(_DtCm_yylval.date, _DtCm_yytext);

	return (token);
}

static int
scanbuf(
	char	 **buf,
	char	 *yytext,
	int	 *yylen)
{
	int	  token = 0;
	int	  state = 0;
	char	  c = '\0';
	char	  lastchar = '\0';
	char	 *yystart = yytext;

	memset(yytext, '\0', *yylen);
	yytext[*yylen - 1] = '\0';
	(*yylen)--;  /* Leave room for trailing '\0' */

	while (token == 0) {
		
		lastchar = c;
		c = *(*buf)++;

		if (*yylen > 0) {
			*yytext++ = c;
			(*yylen)--;
		}

                switch (state) {

                /* State 0 */
                case 0:
                        if (isspace(c)) {
                                /* Keep whitespace out of text */
				yytext--;
				(*yylen)++;
                                /* State remains 0 */
                        } else if (isdigit(c)) {
				if (**buf == '+' || **buf == '-') /* 1+ or 2- */
                                	state = 3;
				else 
                                	state = 1;
                        } else if (isalpha(c)) {
				if (isalpha(**buf))
                                	state = 2;
				else 
                                	state = 4;
                        } else if (c == ENDMARKERSYMBOL) {
				return(ENDMARKER);
			} else if (c == DURATIONSYMBOL) {
				return(DURATION);
			} else if (c == '\0') {
				return(EOL);
			} else
				return(ERROR);
			
			break;
		
		case 1:
			/* Get number */
			if (isdigit(c)) {
				/* Stay in state 1 and get rest of number */
				;
			} else if (isspace(c) || c == '\0') {
				/* Hit a delimiter.  Put it back into the
				 * input buffer and keep it out of the token
				 * text.
				 */
				 (*buf)--;
				 yytext--; (*yylen)++;
				 *yytext = '\0'; 
				 return(NUMBER);
			} else
				state = 5;
			break;
		
		case 2:
			/* Get a command or weekday */
			if (strcmp(yystart, MONTHPOSSYMBOL) == 0) { 
				return(MONTHPOSCOMMAND);
			} else if (strcmp(yystart, MONTHDAYSYMBOL) == 0) { 
				return(MONTHDAYCOMMAND);
			} else if (strcmp(yystart, YEARDAYSYMBOL) == 0) { 
				return(YEARDAYCOMMAND);
			} else if (strcmp(yystart, YEARMONTHSYMBOL) == 0) { 
				return(YEARMONTHCOMMAND);
			} else if (strcmp(yystart, LASTDAYSYMBOL) == 0) { 
				return(LASTDAY);
			} else if (strcmp(yystart, SUNSYMBOL) == 0) { 
				return(SUNDAY);
			} else if (strcmp(yystart, MONSYMBOL) == 0) { 
				return(MONDAY);
			} else if (strcmp(yystart, TUESYMBOL) == 0) { 
				return(TUESDAY);
			} else if (strcmp(yystart, WEDSYMBOL) == 0) { 
				return(WEDNESDAY);
			} else if (strcmp(yystart, THUSYMBOL) == 0) { 
				return(THURSDAY);
			} else if (strcmp(yystart, FRISYMBOL) == 0) { 
				return(FRIDAY);
			} else if (strcmp(yystart, SATSYMBOL) == 0) { 
				return(SATURDAY);
			} else
				return(ERROR);

		case 3:
			/* Get a weeknumber */
			if (c == FRONTWEEKSYMBOL) {
				int	num = lastchar - '0';

				switch (num) {

				case 1: 
					return(FIRSTWEEK);
				case 2: 
					return(SECONDWEEK);
				case 3: 
					return(THIRDWEEK);
				case 4: 
					return(FOURTHWEEK);
				case 5: 
					return(FIFTHWEEK);
				default:
					return(ERROR);
				}
			} else if (c == REARWEEKSYMBOL) {
				int	num = lastchar - '0';

				switch (num) {

				case 1: 
					return(LASTWEEK);
				case 2: 
					return(SECONDLAST);
				case 3: 
					return(THIRDLAST);
				case 4: 
					return(FOURTHLAST);
				case 5: 
					return(FIFTHLAST);
				default:
					return(ERROR);
				}
			}

		case 4:
			/* Found a single letter...probably a command */

			/* We expect an interval to follow a command */
			if (isdigit(c) == 0) return(ERROR);

			/* Backup to before digit */
			(*buf)--;
			yytext--; (*yylen)++;
			*yytext = '\0'; 

			switch (lastchar) {

			case DAILYSYMBOL:
				return(DAILYCOMMAND);
			case MINUTESYMBOL:
				return(MINUTECOMMAND);
			case WEEKLYSYMBOL:
				return(WEEKLYCOMMAND);
			default:
				return(ERROR);
			}
		case 5:
			/* Reading an ISO 8601 date */
			if (isspace(c) || c == '\0') {
				/* Hit a delimiter.  Put it back into the
				 * input buffer and keep it out of the token
				 * text.
				 */
				 (*buf)--;
				 yytext--; (*yylen)++;
				 *yytext = '\0'; 
				 return(DATE);
			}
			break;
		}
	}

	/* Should never get to */
	return (ERROR);
}
