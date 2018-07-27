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
/* $XConsortium: bdftocpf.c /main/6 1996/11/08 02:01:17 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include 	<stdio.h>
#include 	<signal.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<string.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	"bdfgpf.h"
#include	"udcutil.h"

static void put_error(struct btophead *head, int er_no, char *prog_name);
static void put_help(char *prog_name);
static void Usage(char *prog_name) ;
static	void	sigint_out(void) ;
static	int	CnvBDFtoGPF(struct btophead *head) ;

static struct btophead Head;

static	void
sigint_out(void)
{
    if (Head.out_file) {
	unlink(Head.out_file);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int	rtn, i;
    char   *GetTmpPath();
    struct stat	st;

    for (i=1; i<argc; i++) {
	if (!strcmp(argv[i], "-help")) {
	    put_help(argv[0]);
	    exit(0);
	}
    }

    if (!(argc % 2)) {
	Usage(argv[0]);
    }

    Head.in_file = Head.out_file = Head.text_file = NULL;
    Head.p_width = Head.p_height = USE_ORG_SIZE;


    Head.start_code = MIN_CODE ;
    Head.end_code = MAX_CODE ;
    Head.code_category = ALL_CODE;

    for (i=1; i<argc; i+=2) {
	if (!strcmp(argv[i], "-p")) {
	    Head.out_file = argv[i+1];
	} else if (!strcmp(argv[i], "-bdf")) {
	    Head.in_file = argv[i+1];
	} else if (!strcmp(argv[i], "-width")) {
	    Head.p_width = atoi(argv[i+1]);
	    if ((Head.p_width > MAX_FONT_WIDTH) ||
		(Head.p_width < MIN_FONT_WIDTH)) {
		USAGE2("%s: The specification of the character width is improper.\"%s\"\n",
			argv[0], argv[i+1]);
		exit(PARAM_ERROR * (-1));
	    }
	} else if (!strcmp(argv[i], "-height")) {
	    Head.p_height = atoi(argv[i+1]);
	    if ((Head.p_height > MAX_FONT_HEIGHT) ||
		(Head.p_height < MIN_FONT_HEIGHT)) {
		USAGE2("%s: The specification of the character height is improper.\"%s\"\n",
			argv[0], argv[i+1]);
		exit(PARAM_ERROR * (-1));
	    }
	} else {
	    Usage(argv[0]);
	}
    }

    if (Head.in_file == NULL) {
	Head.input = stdin;
    } else {
	char	*spacing ;
	char	*xlfdname ;
	spacing = xlfdname = NULL ;
	/* refuse proportional fonts */
	if ( GetUdcFontName( NULL, Head.in_file, &xlfdname ) ) {
	    USAGE1("%s : This font cannot get XLFD. Terminates abnormally.\n", argv[0]);
	    exit( 1 );
	}
	GETSPACINGSTR( spacing, xlfdname ) ;
	if ( !strcmp( "p", spacing ) || !strcmp( "P", spacing ) ) {
	    USAGE2("%s cannot edit proportional fonts.(SPACING \"%s\")\n", argv[0], spacing );
	    exit( 1 );
	}
	if ((Head.input = fopen(Head.in_file, "r")) == NULL) {
	    USAGE2("%s:  The BDF file cannot be opened.\"%s\"\n",
		argv[0], Head.in_file);
	    exit(BDF_OPEN * (-1));
	}
    }

    signal(SIGHUP, (void(*)())sigint_out);
    signal(SIGINT, (void(*)())sigint_out);
    signal(SIGQUIT, (void(*)())sigint_out);
    signal(SIGTERM, (void(*)())sigint_out);

    if (Head.out_file == NULL) {
	Head.output = stdout;
    } else {
	if (stat(Head.out_file, &st) != 0) {
	    if ((Head.output = fopen(Head.out_file, "w")) == NULL) {
		USAGE2("%s: The character pattern file cannot be opened. \"%s\"\n",
		    argv[0], Head.out_file);
		exit(GPF_OPEN *(-1));
	    }
	} else {
	    Head.text_file = Head.out_file;
	    if ((Head.out_file = GetTmpPath( Head.out_file )) == NULL) {
		USAGE1("%s: Failed convert.\n", argv[0]);
		exit(FATAL_ERROR * (-1));
	    }
	    if ((Head.output = fopen(Head.out_file, "w")) == NULL) {
		USAGE2("%s: The character pattern file cannot be opened. \"%s\"\n",
		    argv[0], Head.out_file);
		exit(GPF_OPEN * (-1));
	    }
	}
    }

    if ((rtn = CnvBDFtoGPF(&Head))) {
	if (Head.in_file != NULL) {
	    fclose(Head.input);
	}
	if (Head.out_file != NULL) {
	    fclose(Head.output);
	    unlink(Head.out_file);
	}
	put_error(&Head, rtn, argv[0]);
	exit(rtn * (-1));
    }

    if (Head.in_file != NULL) {
	fclose(Head.input);
    }
    if (Head.out_file != NULL) {
	fclose(Head.output);
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	if (Head.text_file != NULL) {
	    Link_NewFile(Head.out_file, Head.text_file);
	    unlink(Head.out_file);
	}
    }
    exit(0);
}

static
CnvBDFtoGPF( struct btophead *head)
{
    char    bdfbuf[BUFSIZE];
    int	    rtn;

    if ((rtn = ReadBdfHeader(head, bdfbuf)) < 0) {
	return(rtn);
    }

    if (head->p_width == USE_ORG_SIZE) {
	head->p_width = head->bdf_width;
    }
    if (head->p_height == USE_ORG_SIZE) {
	head->p_height = head->bdf_height;
    }
    if ((head->p_width != head->bdf_width) ||
	 (head->p_height != head->bdf_height)) {
	head->zoomf = 1;
    } else {
	head->zoomf = 0;
    }

    if ((head->code = (int *)malloc(sizeof(int)*head->num_chars)) == NULL) {
	return(FATAL_ERROR);
    }

    if ((head->ptn = (char **)malloc(sizeof(char *)*head->num_chars)) == NULL) {
	return(FATAL_ERROR);
    }

    if ((rtn = ReadBdfToMemory(head, bdfbuf))) {
	return(rtn);
    }

    WriteGpfHeader(head);

    if ((rtn = WritePtnToGpf(head))) {
	return(rtn);
    }

    return(0);
}

static void
put_error(struct btophead *head, int er_no, char *prog_name)
{
    switch(er_no) {
	case FATAL_ERROR :
	    USAGE1("%s: Failed convert.\n", prog_name);
	    break;

	case BDF_OPEN :
	    USAGE2("%s: The BDF file cannot be opened. \"%s\"\n",
		prog_name, head->in_file);
	    break;

	case BDF_READ :
	    USAGE2("%s:  The BDF file cannot be read. \"%s\"\n",
		prog_name, head->in_file);
	    break;

	case BDF_INVAL :
	    USAGE2("%s: The format of the BDF file is illegal. \"%s\"\n",
		prog_name, head->in_file);
	    break;

	case GPF_OPEN :
	    USAGE2("%s: The character pattern file cannot be opened. \"%s\"\n",
		prog_name, head->out_file);
	    break;

	case GPF_WRITE :
	    USAGE2("%s: It is not possible to write to the character pattern file. \"%s\"\n",
		prog_name, head->out_file);
	    break;

	default :
	    break;
    }
}

static	void
put_help(char *prog_name)
{
    USAGE1("Usage: %s [-p character_pattern_file_name] [-bdf BDF_file_name]\n", prog_name);
    USAGE("\t\t[-width character_width] [-height character_height] [-help]\n\n");
}

static	void
Usage(char *prog_name)
{
    put_help(prog_name);
    exit(PARAM_ERROR * (-1));
}
