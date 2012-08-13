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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: convert_str_2_char
 *		gen_vec
 *		hc_encode
 *		main
 *		process_char
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1990,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************** HENCODE.C ***********************
 * $XConsortium: hencode.c /main/9 1996/11/21 19:50:29 drk $
 * Huffman encoder program.
 *
 * $Log$
 * Revision 2.3  1996/03/13  22:56:39  miker
 * Changed char to UCHAR several places.
 *
 * Revision 2.2  1995/10/26  15:11:42  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  20:50:40  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  18:07:00  miker
 * Name changes for DtSearch.
 */
#include "SearchP.h"
#include <errno.h>
#include <stdlib.h>

#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

#define PROGNAME	"HENCODE"
#define MS_huff		30	/* message catalog set number */
#define DELIMITERS	"\t\n"
#define LAST_BIT	'-'
#define LITERAL_NUM	256
#define	NO_SPACE	0


/*------------------------ GLOBALS ---------------------------*/
long            gen_vec_hufid = 0L;

static struct or_blobrec blobrec;
static char    *huff_code[257];
static int      code_length[257];
static char     coded_bits_str[9];
static char     bit_pos = 0;
static char     bits_left;
static int      total_num_chars = 0;
static int      num_char_coded = 0;
static char     zero_str[] = "00000000";

#define MAX_NUM_CHAR	(sizeof(blobrec.or_blob) - 1)
#define	MAX_NUM_BITS	(8 * MAX_NUM_CHAR)

/****************************************/
/*					*/
/*	      GENERATE VECTORS		*/
/*					*/
/****************************************/
void            gen_vec (char *fname_huffcode_tab)
{
    char            temp[40];
    int             i, j;
    char            tab_filebuf[128];
    unsigned char   ch;
    FILE           *tab_stream;
    _Xstrtokparams  strtok_buf;

    if ((tab_stream = fopen (fname_huffcode_tab, "r")) == NULL) {
	printf (catgets(dtsearch_catd, MS_huff, 1,
	    "%s: Cannot open huffman encode file '%s':\n"
	    "  %s\n  Exit Code = 2\n"),
	    PROGNAME"222", fname_huffcode_tab, strerror (errno));
	DtSearchExit (2);
    }
    memset (huff_code, 0, sizeof(huff_code));
    memset (code_length, 0, sizeof(code_length));
    /*
     * First line in the file contains time stamp. We have to read
     * it separately. First token on first line is hufid. Save it
     * in a global for optional use by caller. 
     */
    fgets (tab_filebuf, sizeof (tab_filebuf) - 1, tab_stream);
    gen_vec_hufid = atol (tab_filebuf);

    /*-------------- READ IN HUFFMAN FILE ------------*/
    /*
     * We are only interested in the character itself (index) and
     * its Huffman Code 
     */
    while (fgets (tab_filebuf, sizeof (tab_filebuf) - 1, tab_stream)
	!= NULL) {
	i = atoi (_XStrtok (tab_filebuf, DELIMITERS, strtok_buf)); /* char */
	/* read current huff code */
	strcpy (temp, _XStrtok (NULL, DELIMITERS, strtok_buf));
	if (temp[0] == ' ') {
	    /* Empty huffcode associated with LITERAL CODE.
	     * Either this is literal char itself and literal
	     * encodeing has been turned off, or this char is
	     * so rare that it is coded using the literal char.
	     */
	    if (i == 256)
		continue;

	    /* current character has LITERAL CODE */
	    strcpy (temp, huff_code[LITERAL_NUM]);
	    *(code_length + i) = *(code_length + LITERAL_NUM) + 8;
	    ch = (unsigned char) i;
	    for (j = 0; j < 8; j++) {
		if (ch & 0x80) {
		    temp[*(code_length + LITERAL_NUM) + j] =
			'1';
		}
		else {
		    temp[*(code_length + LITERAL_NUM) + j] =
			'0';
		}
		ch = ch << 1;
	    }
	    temp[*(code_length + LITERAL_NUM) + 8] = '\0';
	    huff_code[i] =
		(char *) malloc (*(code_length + i) + 1);
	    strcpy (huff_code[i], temp);
	}
	else {
	    /* regular HUFFMAN code */
	    *(code_length + i) = strlen (temp);
	    huff_code[i] =
		(char *) malloc (*(code_length + i) + 1);
	    strcpy (huff_code[i], temp);
	}
    }
    fclose (tab_stream);
}  /* end of function gen_vec */

/********************************************************/
/*							*/
/*	Convert Coded String to Coded Character		*/
/*							*/
/********************************************************/
void            convert_str_2_char (char *code)
{
    int             i, j;

    *code = 0;
    j = 1;
    for (i = 0; i < 8; i++) {
	if (*(coded_bits_str + (7 - i)) == '1') {
	    *code += j;
	}
	j = j * 2;
    }
    total_num_chars++;
    return;
}

/****************************************/
/*					*/
/*	Process Current Character	*/
/*					*/
/****************************************/
int             process_char (UCHAR ch, char *bitstr)
{
    char            temp_code[40];
    char            coded_char;
    int             i, j;
    int             num_of_bits_in_code;

    i = (int) ch;
    num_of_bits_in_code = *(code_length + i);
    if ((MAX_NUM_BITS - total_num_chars * 8 - bit_pos) <
	num_of_bits_in_code) {
	return NO_SPACE;
    }
    strcpy (temp_code, huff_code[i]);
    while (TRUE) {
	/* fill new character with Huffman Code */
	if (bit_pos == 0) {
	    if (num_of_bits_in_code == 8) {
		strcpy (coded_bits_str, temp_code);
		convert_str_2_char (&coded_char);
		bitstr[total_num_chars - 1] = coded_char;
		return TRUE;
	    }
	    if (num_of_bits_in_code < 8) {
		strcpy (coded_bits_str, temp_code);
		bit_pos = num_of_bits_in_code;
		bits_left = 8 - bit_pos;
		return TRUE;
	    }
	    if (num_of_bits_in_code > 8) {
		strncpy (coded_bits_str, temp_code, 8);
		coded_bits_str[8] = '\0';
		convert_str_2_char (&coded_char);
		bitstr[total_num_chars - 1] = coded_char;
		num_of_bits_in_code -= 8;
		strcpy (temp_code, &temp_code[8]);
	    }
	}	/* end of bit_pos == 0 loop */
	else {
	    j = bit_pos + num_of_bits_in_code;
	    if (j == 8) {
		bit_pos = 0;
		strcat (coded_bits_str, temp_code);
		convert_str_2_char (&coded_char);
		bitstr[total_num_chars - 1] = coded_char;
		return TRUE;
	    }
	    if (j < 8) {
		strcat (coded_bits_str, temp_code);
		bit_pos = j;
		bits_left = 8 - bit_pos;
		return TRUE;
	    }
	    if (j > 8) {
		strncat (coded_bits_str, temp_code,
		    (size_t) bits_left);
		convert_str_2_char (&coded_char);
		bitstr[total_num_chars - 1] = coded_char;
		num_of_bits_in_code -= bits_left;
		strcpy (temp_code, &huff_code[i][bits_left]);
		bit_pos = 0;
	    }
	}	/* end of else loop */
    }	/* end of while(TRUE) loop */
}

/************************************************/
/*						*/
/*		   HC Encode			*/
/*						*/
/************************************************/
int             hc_encode (struct or_blobrec * targblobrec,
                    UCHAR *charbuf,
                    int charcount,
                    int file_pos)
{
/********** replaced by blobrec above...
	union charint
		{
		char ch[2];
		INT	 orig_char_count;
		} un1;
	static char temp1 [MAX_NUM_CHAR+1];  ...repl by blobrec;
************/
    char           *ptr, *targ, *src;
    int             i, j;
    char            temp;
    char            ret_code = TRUE;
    char            write = FALSE;
    char            last_call = FALSE;

    if (charcount == 0) {
	last_call = TRUE;
	charcount = 1;
    }
    for (i = 0; i < charcount; i++) {
	if (!last_call) {
	    ret_code = process_char (charbuf[i], (char *) blobrec.or_blob);
	}
	if ((ret_code == NO_SPACE) ||
	    (file_pos && (i == (charcount - 1)))) {
	    if (!last_call) {
		if (file_pos && (i == (charcount - 1))) {
		    num_char_coded++;
		}
	    }
	    if (bit_pos) {
		strncat (coded_bits_str, zero_str,
		    (size_t) bits_left);
		convert_str_2_char (&temp);
		blobrec.or_blob[total_num_chars - 1][0] = temp;
	    }
	    write = TRUE;
	    /**********
		un1.orig_char_count = num_char_coded;
		bitstring[0] = un1.ch[0];
		bitstring[1] = un1.ch[1];
		for (j = 0; j <= total_num_chars; j++) {
		       *(bitstring + j + 2) = *(temp1 + j);
		       };
		**************/
	    targblobrec->or_bloblen = num_char_coded;
	    targ = (char *) targblobrec->or_blob;
	    src = (char *) blobrec.or_blob;
	    for (j = 0; j < total_num_chars; j++)
		*targ++ = *src++;

	    num_char_coded = 0;
	    bit_pos = 0;
	    total_num_chars = 0;
	    if (file_pos && (i == (charcount - 1))) {
		return write;
	    }
	    i--;
	}
	else {
	    num_char_coded++;
	}
    }
    return write;
}

#ifdef DEBUG_HENCODE
/****************************************/
/*					*/
/*		   Main			*/
/*					*/
/****************************************/
main (int argc, char *argv[])
{
    FILE           *stream;
    char            bitstring[MAX_NUM_CHAR + 2];
    char            charbuf[MAX_NUM_CHAR + 1];
    int             charcount = 0;
    int             mychar;

    if (argc < 2) {
	fprintf (stderr, "Usage: try filename\n");
	exit (1);
    }
    if ((stream = fopen (argv[1], "rb")) == NULL) {
	fprintf (stderr, "Could not open input file '%s'\n", argv[1]);
	exit (2);
    }
    fp = fopen ("codefile.dat", "wb");
    gen_vec ();
    while ((mychar = getc (stream)) != EOF) {
	charbuf[charcount] = mychar;
	charcount++;
	if (charcount == MAX_NUM_CHAR) {
	    hc_encode (bitstring, charbuf, charcount, 0);
	    /*
	     * for (j = 0; j < charcount; j++) {
	     * fputc(bitstring[j], fp); } 
	     */
	    charcount = 0;
	}
    }
    hc_encode (bitstring, charbuf, charcount, 1);
    printf ("Total Number of Characters = %ld\n", total_num_chars);
    fclose (fp);
    fclose (stream);
    return;
}

#endif
/********************** HENCODE.C ***********************/
