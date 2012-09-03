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
/* $XConsortium: merge.c /main/4 1995/12/08 09:44:13 rswiston $ */
/*
    merge.c

    Merge international messages back into CDE files.

    Syntax:
        merge [-bs][-lang language][-dfile Default.tmsg] Primary.tmsg\
              < File.nls > File
  
        -bs                 : The backslash+n is left alone and not interpreted.
        -lang language      : Language of messages. (If not specified, the value
                              of LANG environment variable is used.)
        -dfile Default.tmsg : Default message file.

        Primary.tmsg        : Primary message file.
        File.nls            : Template file.
        File                : Merged file.


    June 93 Hatim Amro  -Updated this tool to have support for a default .tmsg
                         file in case the primery one is not found or missing
                         a message whose number is in the .nls file. I, also,
                         rewrote the option stripping and handling code for
                         better flexibility. I find the Syntax above very
                         confusing, I provide a simpler one below, which,
                         includes the new -dfile option.
    New Syntax:
     merge [-lang Ja_JP][-dfile Default.tmsg] Primary.tmsg < File.nls > File

    July 93 Hatim Amro  -In order to comply with request from Ann Barnette, the
                         backslash n is left alone and not interpreted if the
                         new -bs option is specified in the parameter list.
                         (this option will not be published)

    New Syntax:
     merge [-bs][-lang Ja_JP][-dfile Default.tmsg]\
           Primary.tmsg < File.nls > File

    8/2/93 Masato Suzuki - -lang option is generalized. (This options doesn't
                           necessarily need to be specified for single-byte
                           languages.) Error messages are enhanced. And bug
                           fixes are applied.

    12/13/93 Masato Suzuki -Modified so that the format of message file(*.tmsg)
                            is compliant to XPG message catalog file. (But it's
                            subset.)

    Format of message file.
        $set n [comment] ... n must be 1.
        $ [comment]
        m message-text ... Message text may contain following spcial characters
                           and escape sequences.
                             \\                  backslash
                             \n                  newline
                             \t                  horizontal tab
                             \ (at end of line)  continue on same line

    Following XPG format and escape sequences are not supported.
        $delset, $quote, $len
        \v, \b, \r, \f, \ddd, \xdddd
*/
/*
 * Following pattern in proforma will be replaced by the matched message in
 * catalogue. This pattern must be in one line.
 *
 *  %|nls-???-###|       : ??? must be numerics.
 *                         ### is comment, can be nothing.
 */


#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <nl_types.h>

nl_catd catfile[2] = {NULL, NULL};	/* [0] for primary, [1] for default */
char *big_buff;
char *lang = NULL;
char envvar[100];
char *pfile = NULL;
char *dfile = NULL;
int bs = 0;
int crt_line[3] = {0, 0, 1}; /* current line  [0]: Primary message file */
                             /*               [1]: Default message file */
                             /*               [2]: Template file        */

void process_message ();
int get_char ();
void cat_open ();
int find_message ();
int find_msg_in_file ();
void get_message ();
void fatal ();
void get_option ();


void main (argc, argv)
int argc;
char *argv [];
{
    int c;

    get_option(&argc, argv);

    if(pfile == NULL)
        fatal("Usage: merge [-lang language][-dfile Default.tmsg] Primary.tmsg < File.nls > File \n", 0, 9);

    if(lang != NULL) {
        setlocale(LC_ALL, lang);
        /* LC_CTYPE need to be set to make "gencat" command work correctly */
        sprintf( envvar, "LC_CTYPE=%s", lang );
        putenv( envvar );
    } else
        setlocale(LC_ALL, "");

    cat_open();

    c = get_char ();
    while (c != EOF) {
        while (c == '%') {
            char *s;
            char *s0;

            for (s = s0 = "|nls-"; *s != 0; s++)
                if (c = get_char (), c != *s)
                    break;

/* if the string matchs "%|nls-", go to process_message(). */
            if (*s == 0) {
                process_message ();
                c = get_char ();
            }
            else {
                putchar ('%');
                while (s0 != s) {
                    putchar (*s0);
                    s0++;
                }
            }
        }
        putchar (c);
        c = get_char ();
    }

    if ( catfile[0] )
       catclose(catfile[0]);

    if ( catfile[1] )
       catclose(catfile[1]);

    unlink("./.dt_pfile.cat");
    unlink("./.dt_dfile.cat");

    exit (0);
}

/*
 * If the pattern "%|nls-???-###|" is found in the template file, replace it
 * by big_buff.
 */
void process_message ()
{
    int c;
    int m = 0;

    while (c = get_char (), c != '-')
        switch (c)
        {
        case EOF: fatal ("Unterminated NLS sequence.\n", crt_line[2]-1, 2);
        case '\n': fatal ("Unterminated NLS sequence.\n", crt_line[2]-1, 2);
        default:  fatal ("Bad character in NLS sequence.\n", crt_line[2], 2);
        case '0': m = m * 10 + 0; break;
        case '1': m = m * 10 + 1; break;
        case '2': m = m * 10 + 2; break;
        case '3': m = m * 10 + 3; break;
        case '4': m = m * 10 + 4; break;
        case '5': m = m * 10 + 5; break;
        case '6': m = m * 10 + 6; break;
        case '7': m = m * 10 + 7; break;
        case '8': m = m * 10 + 8; break;
        case '9': m = m * 10 + 9; break;
        }

    while (c = get_char (), c != '|')
        if (c == '\n' || c == EOF)
            fatal ("Unterminated NLS sequence.\n", crt_line[2]-1, 2);

    if(find_message(m))
        printf ("%s", big_buff);
    else {
        printf ("....Missing message #%d", m);
        fprintf (stderr, "*** Error: Missing message #%d\n", m);
    }
}

/*
 * Get a character from template. Incriment line count if new line is found.
 */
int get_char ()
{
    int c;

    c = getchar();
    if(c == '\n')
        crt_line[2]++;
    return c;
}

/*
 * Open message files
 */
void cat_open ()
{
    char line[255];

    unlink("./.dt_pfile.cat");
    unlink("./.dt_dfile.cat");

    if(pfile != NULL)
    {
#if defined(USL) || defined(__uxp__)
        sprintf(line,"/usr/bin/gencat -m ./.dt_pfile.cat %s",pfile);
#else
        sprintf(line,"/usr/bin/gencat ./.dt_pfile.cat %s",pfile);
#endif
        if ( system(line) != 0 )
	{
           fatal("primary .tmsg file would not gencat\n",0,9);
	}
    }

    catfile[0] =  catopen("./.dt_pfile.cat",0);

    if(dfile != NULL)
    {
#if defined(USL) || defined(__uxp__)
        sprintf(line,"/usr/bin/gencat -m ./.dt_dfile.cat %s",dfile);
#else
        sprintf(line,"/usr/bin/gencat ./.dt_dfile.cat %s",dfile);
#endif
        if ( system(line) != 0 )
	{
           fatal("default .tmsg file would not gencat\n",0,9);
	}

    }

    catfile[1] = catopen("./.dt_dfile.cat",0);

    /* if all fails */
    if(catfile[0] == NULL && catfile[1] == NULL)
        fatal("Can't open message files.\n", 0, 9);

}

/*
 * Search a message by specified number. If found, returns 1 and the message
 * will be set in big_buff. If not found, returns 0.
 */
int find_message (msg)
int msg; /* message number to be searched */
{
    int ret = 0;

    if(catfile[0] != NULL)
        ret = find_msg_in_file(msg, 0);
    if(ret == 0 && catfile[1] != NULL)
        ret = find_msg_in_file(msg, 1);
    return ret;
}

/*
 * Search a line starts with the message number in specified file. If found,
 * the line will be passed to get_message() and returns 1.
 * If not found, returns 0.
 */
int find_msg_in_file (msg, file)
int msg; /* message number to be searched */
int file; /* 0: Primary message file, 1: Default message file */
{
        big_buff = catgets(catfile[file],1,msg,"MSG_NOT_FOUND");
        if ( strcmp(big_buff,"MSG_NOT_FOUND") )
           return(1);
        else
           return(0);
}

/*
 * Display error message and exit program.
 */
void fatal (m, line, file)
char *m;
int line; /* line where the error found */
int file; /* file in which the error found  0: Primary message file */
          /*                                1: Default message file */
          /*                                2: Template file        */
          /*                                9: N/A                  */
{
    fprintf (stderr, "*** Fatal: ");
    fprintf (stderr, m);
    switch(file)
    {
    case 0:
        fprintf(stderr, "           [Line %d in Primary message file]\n", line);
        break;
    case 1:
        fprintf(stderr, "           [Line %d in Default message file]\n", line);
        break;
    case 2:
        fprintf(stderr, "           [Line %d in Template file]\n", line);
    }
    exit (1);
}

/*
 * Parse command line options.
 */
void get_option (argc, argv)
int *argc;
char *argv[];
{
    int i;

    for(i = 1; i < *argc; i++) {
        if(strcmp(argv[i], "-bs") == 0) {
            bs = 1;
        }
        else if(strcmp(argv[i], "-lang") == 0) {
            if(argv[i+1] != NULL && strlen(argv[i+1]) > 0) {
                lang = (char *)malloc(sizeof(char) * (strlen(argv[i+1]) + 1));
                strcpy(lang, argv[i+1]);
                i++;
            }
        }
        else if(strcmp(argv[i], "-dfile") == 0) {
            if(argv[i+1] != NULL && strlen(argv[i+1]) > 0) {
                dfile = (char *)malloc(sizeof(char) * (strlen(argv[i+1]) + 1));
                strcpy(dfile, argv[i+1]);
                i++;
            }
        }
        else {
            if(strlen(argv[i]) > 0) {
                pfile = (char *)malloc(sizeof(char) * (strlen(argv[i]) + 1));
                strcpy(pfile, argv[i]);
            }
        }
    }
}
