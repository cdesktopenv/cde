/* 
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: UilLstLst.c /main/20 1999/07/21 09:03:16 vipin $"
#endif
#endif

/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This module contains the procedures for managing the UIL listing.
**
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <stdarg.h>

#include "UilDefI.h"


/*
**
**  EXTERNAL storage used by the listing
**
*/


/*
**
**  OWN storage used by the listing
**
*/

externaldef(uil_comp_glbl) char		Uil_lst_c_title2[132];

static	    int			lst_l_usable_lines;
static	    int			lst_l_lines_left;
static	    int			lst_l_page_no;
static	    char		lst_c_title1[132];
static	    uil_fcb_type	*lst_az_fcb;
static	     boolean		lst_v_listing_open = FALSE;


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function opens the UIL listing file to be written and
**	initializes OWN storage used by the listing package.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      lst_l_usable_lines	    number of usable lines per page
**      lst_l_lines_left	    number of lines left on page
**      lst_l_page_no		    current page number
**      lst_c_title1		    title line 1
**      Uil_lst_c_title2		    title line 2
**      lst_v_listing_open	    can the listing be written to
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      listing file is opened
**
**--
**/

void	lst_open_listing()
{
    status  open_status;
    _Xctimeparams	ctime_buf;

    /* allocate fcb */

    lst_az_fcb = (uil_fcb_type *)_get_memory( sizeof( uil_fcb_type ) );

    /* open the listing file */

    open_status = 
	create_listing_file( lst_az_fcb );

    if ( open_status == src_k_open_error )
    {
	diag_issue_diagnostic( d_listing_open,
			       diag_k_no_source, diag_k_no_column,
			       lst_az_fcb->expanded_name );
        /* should never return - error is fatal */

	return;
    }

    lst_l_lines_left = 0;
    lst_l_page_no = 0;
    lst_v_listing_open = TRUE;

    sprintf(lst_c_title1, 
	    "%s %s \t%s\t\t Page ",
	    _host_compiler, _compiler_version,
	    current_time(&ctime_buf));

    /*
    **	Haven't parsed the module yet.
    **  UilSarMod.c routines will fill it in.
    */

    Uil_lst_c_title2[ 0 ] = 0;

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function does the clean up processing for the listing facility.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  IMPLICIT INPUTS:
**
**      lst_c_title1		    title line 1
**      Uil_lst_c_title2		    title line 2
**      lst_v_listing_open	    can the listing be written to
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      dynamic memory is freed
**
**--
**/

void	Uil_lst_cleanup_listing()
{
    /*
    **	Check that there is a listing file requested and that
    **	it is not already in error.
    */

    if (!(lst_v_listing_open && Uil_cmd_z_command.v_listing_file))
	return;

    /*
    ** free fcb 
    */

    _free_memory((char*)lst_az_fcb);
    lst_az_fcb = NULL;
}







/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      routine to create the listing file.
**
**  FORMAL PARAMETERS:
**
**	az_fcb		    file control block for the file
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      src_k_open_normal
**      src_k_open_error
**
**  SIDE EFFECTS:
**
**      file is created
**
**--
**/

status	create_listing_file( az_fcb )

uil_fcb_type			*az_fcb;

{
    /* place the file name in the expanded_name buffer */

    strcpy(az_fcb->expanded_name, Uil_cmd_z_command.ac_listing_file);

    /* open the file */

    az_fcb->az_file_ptr = fopen(Uil_cmd_z_command.ac_listing_file, "w");

    if (az_fcb->az_file_ptr == NULL)
	return src_k_open_error;

    /* assume 66 lines on a page */

    lst_l_usable_lines = 66 - 9;

    return src_k_open_normal;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      function to write a line to the listing file.
**
**  FORMAL PARAMETERS:
**
**      ac_line		text of the line to output
**
**  IMPLICIT INPUTS:
**
**      lst_l_lines_left
**      lst_l_usable_lines
**	lst_az_fcb
**	lst_c_title1
**	lst_c_title1
**
**  IMPLICIT OUTPUTS:
**
**      lst_l_lines_left
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      line output to file
**
**--
**/


void	lst_output_line( ac_line, v_new_page )

char	*ac_line;
boolean	v_new_page;

{
    status	error_status;

    if (!lst_v_listing_open)
	return;

    /*
    **	Update the current file and call the Status callback routine to report
    **	our progress.
    */
    Uil_current_file = lst_az_fcb->expanded_name;
    if (Uil_cmd_z_command.status_cb != (Uil_continue_type(*)())NULL)
	diag_report_status();    


    if ((lst_l_lines_left <= 0) || v_new_page)
    {
	lst_l_page_no ++;
	lst_l_lines_left = lst_l_usable_lines;

	fprintf(lst_az_fcb->az_file_ptr, 
		"\f\n%s%d\n%s\n\n", 
		lst_c_title1, lst_l_page_no, Uil_lst_c_title2);
    }

    error_status = fprintf(lst_az_fcb->az_file_ptr, "%s\n", ac_line);

    if (error_status == EOF)
    {
	lst_v_listing_open = FALSE;
        diag_issue_diagnostic( d_listing_write,
                               diag_k_no_source, diag_k_no_column,
                               lst_az_fcb->expanded_name );
    }

    lst_l_lines_left --;

    return;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**     function to return ASCII date and time
**
**  FORMAL PARAMETERS:
**
**      void
**
**  IMPLICIT INPUTS:
**
**      void
**
**  IMPLICIT OUTPUTS:
**
**      void
**
**  FUNCTION VALUE:
**
**      pointer to a null terminated string
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

char	*current_time(_Xctimeparams *ctime_buf)
{
    time_t	time_location;
    char	*ascii_time;

    time_location = time( 0 );

    ascii_time = _XCtime( &time_location, *ctime_buf );

    ascii_time[24] = 0;

    return ascii_time;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function outputs the UIL listing file
**
**  FORMAL PARAMETERS:
**
**
**  IMPLICIT INPUTS:
**
**
**  IMPLICIT OUTPUTS:
**
**
**  SIDE EFFECTS:
**
**
**--
**/

void	lst_output_listing()

{
    src_source_record_type  *az_src_rec;
    char		    src_buffer[ src_k_max_source_line_length+12 ];
    char		    *src_ptr;
    int			    i;

    /*
    **	Check that there is a listing file requested and that
    **	it is not already in error.
    */

    if (!(lst_v_listing_open &&
	  Uil_cmd_z_command.v_listing_file)
       )
	return;

    /*
    **	Walk the list of source records.
    */

    for (az_src_rec = src_az_first_source_record;  
	 az_src_rec != NULL;  
	 az_src_rec = az_src_rec->az_next_source_record)
    {

	/*
	**  place the line and file number in the output buffer
	*/

	sprintf(src_buffer, "%5d (%d)\t", 
		az_src_rec->w_line_number, 
		az_src_rec->b_file_number);

	src_ptr = &(src_buffer[ strlen( src_buffer ) ]);
	
	src_retrieve_source( az_src_rec, src_ptr );

	/*
	**  filter standard unprintable characters if necessary
	*/

	if ( az_src_rec->b_flags & src_m_unprintable_chars)
	    lex_filter_unprintable_chars( (unsigned char*)src_ptr, strlen( src_ptr ), 0 );

	/*
	**  replace leading formfeed with a blank
	*/

	if ( az_src_rec->b_flags & src_m_form_feed)
	    *src_ptr = ' ';

	lst_output_line( src_buffer, 
			 (az_src_rec->b_flags & src_m_form_feed) != 0 );

	/*
	**  if the line has messages, get them displayed
	*/

	if (az_src_rec->az_message_list != NULL)
	{
	    lst_output_message_ptr_line( az_src_rec, src_ptr );
	    lst_output_messages( az_src_rec->az_message_list );
	}

	/*
	**  if the line has machine code, get it displayed if requested
	*/

	if ( (Uil_cmd_z_command.v_show_machine_code) &&
	     (az_src_rec->w_machine_code_cnt > 0) )
	{
	    lst_output_machine_code( az_src_rec );
	}

    }

    /*
    **  output the orphan messages
    */

    if (src_az_orphan_messages != NULL)
	lst_output_messages( src_az_orphan_messages );

    /*
    **  output the file summary
    */

    lst_output_line( " ", FALSE );

    for (i = 0; i <= src_l_last_source_file_number; i++) {

	uil_fcb_type	*az_fcb;	    /* file control block ptr */
	char		buffer [132];

	az_fcb = src_az_source_file_table [i];
	sprintf (buffer,
		 "     File (%d)   %s",
		 i, az_fcb->expanded_name );
	lst_output_line( buffer, FALSE );
    }    

    lst_output_line( " ", FALSE );

    return;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function outputs a list of messages to the UIL listing file
**
**  FORMAL PARAMETERS:
**
**      az_message_item	    ptr to start of the message list
**
**  IMPLICIT INPUTS:
**
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      messages are written to the listing file
**
**--
**/

void	lst_output_messages( az_message_item )

src_message_item_type	*az_message_item;

{
    src_message_item_type	*az_msg;
    char			buffer[132];
    int				msg_no;
    int				last_pos;
    int				current_pos;

    last_pos = -1;
    msg_no = 9;

    for (az_msg = az_message_item;  
	 az_msg != NULL;  
	 az_msg = az_msg->az_next_message)
    {
	current_pos = az_msg->b_source_pos;

	if (last_pos < current_pos)
	{
	    last_pos = current_pos;
	    if (last_pos == diag_k_no_column)
		msg_no = 0;
	    else
		msg_no = (msg_no % 9) + 1;
	}


	sprintf(buffer, "%s (%d) %s", 
		diag_get_message_abbrev( az_msg->l_message_number ),
		msg_no, 
		az_msg->c_text);

	lst_output_line( buffer, FALSE );
    }    

    lst_output_line( " ", FALSE );

    return;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function outputs a list of machine code items to the
**	UIL listing file
**
**  FORMAL PARAMETERS:
**
**      az_src_rec	    ptr to a source record
**
**  IMPLICIT INPUTS:
**
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      machine code is written to the listing file
**
**--
**/

void	lst_output_machine_code ( az_src_rec )

src_source_record_type	*az_src_rec;

{

    static	src_machine_code_type * * mc_array = NULL;
    static	unsigned short mc_cnt = 0;

    src_machine_code_type	*az_code;
    int				code_cnt, mc_i;

/*	Go through the machine code list, and save the entries in
	the array; traverse them in reverse order.  Reuse the vector
	if it is large enough.	*/



    code_cnt = az_src_rec->w_machine_code_cnt;

    if ((int)mc_cnt < code_cnt) {
	if (mc_array != NULL) {
	    _free_memory ((char*)mc_array);
	}
	mc_array = 
	    (src_machine_code_type * *)_get_memory (sizeof (char *) * code_cnt);
	mc_cnt = code_cnt;
    }

    for (az_code = az_src_rec->az_machine_code_list, mc_i = 0;
	 az_code != NULL;  
	 az_code = az_code->az_next_machine_code, mc_i++) {
	mc_array [mc_i] = az_code;
    }
   
    for (mc_i = code_cnt - 1; mc_i >= 0; mc_i--)
    {

#define BIT_64_LONG   ((sizeof(long)*8)==64)

#define         OFFSET_COL      (BIT_64_LONG ? 75 : 43) /*should be 75 on 64 bit mach, 43 on 32*/
#define         TEXT_COL        (BIT_64_LONG ? 82 : 50) /*82 on 64 bit mach, 50 on 32*/
#define         BUF_LEN         (BIT_64_LONG ? 164 : 132) /*164 on 64 bit mach. 132 on 32 bit mach.*/
#define         HEX_PER_WORD    4
#define         HEX_PER_LONG    (BIT_64_LONG ? 16 : 8) /*should be 16 on 64 bit mach., 8 on 32 bit mach*/

#define		LONG_PER_LINE	4
#define		ASCII_PER_LINE	(LONG_PER_LINE * sizeof (long))

	unsigned short 	long_cnt, extra_byte_cnt, text_len, code_len,
			line_cnt, extra_long_cnt, i, j, code_offset;
	unsigned char	buffer[ BUF_LEN + 1 ], * text_ptr,
			hex_longword [HEX_PER_LONG + 1], line_written;
	unsigned long	* code_ptr;

	unsigned long temp_long;
        static unsigned short start_hex_long [4];
	/*if 64-bit environment, it should have vals { 55, 38, 21, 4 };
	  if 32 bit environment, { 31, 22, 13, 4 };
	*/
	start_hex_long[0]=(BIT_64_LONG ? 55 : 31);
	start_hex_long[1]=(BIT_64_LONG ? 38 : 22);
	start_hex_long[2]=(BIT_64_LONG ? 21 : 13);
	start_hex_long[3]=4;


	az_code = mc_array [mc_i];

	code_ptr = (unsigned long *)az_code -> data.c_data;
	code_len = az_code -> w_code_len;
	code_offset = az_code -> w_offset;
	text_ptr = (unsigned char *)(& az_code -> data.c_data [code_len]);
	text_len = strlen ((char *)text_ptr);
	if (text_len > (unsigned short) (BUF_LEN - TEXT_COL + 1))
	    text_len = BUF_LEN - TEXT_COL + 1;

	long_cnt = code_len / sizeof (char *);
	line_cnt = long_cnt / LONG_PER_LINE;
	extra_long_cnt = long_cnt % LONG_PER_LINE;
	extra_byte_cnt = code_len % sizeof (char *);

	_fill (buffer, ' ', sizeof buffer - 1);


	sprintf ((char *)hex_longword, "%04X", code_offset);
	_move  (& buffer [OFFSET_COL - 1], hex_longword, HEX_PER_WORD);

	_move (& buffer [TEXT_COL - 1], text_ptr, text_len);
	buffer [TEXT_COL + text_len] = '\0';

	line_written = FALSE;

/*
**	Write out entire lines.  Clear the text after the first line.
**	Filter all non-printable characters.
*/

	for (i = 0; i < line_cnt; i++) {

	    if (text_len == 0) {
		_move  (& buffer [TEXT_COL - 1], code_ptr, ASCII_PER_LINE);
		lex_filter_unprintable_chars ((unsigned char*)
			& buffer [TEXT_COL - 1], ASCII_PER_LINE,
			lex_m_filter_tab );
		buffer [TEXT_COL - 1 + ASCII_PER_LINE] = '\0';
	    }

	    for (j = 0; j < LONG_PER_LINE; j++, code_ptr++) {


	      if (BIT_64_LONG){

		sprintf ((char *)hex_longword, "%lX", (* code_ptr));
	      }
	      else{
                sprintf ((char *)hex_longword, "%08X", (* code_ptr));
	      }

		_move (& buffer [start_hex_long [j]],
			hex_longword, HEX_PER_LONG);

	    }


	    lst_output_line((char*) buffer, FALSE );
	    line_written = TRUE;

	    code_offset += LONG_PER_LINE * sizeof (long);
	    sprintf ((char *)hex_longword, "%04X", code_offset);
	    _move  (& buffer [OFFSET_COL - 1], hex_longword, HEX_PER_WORD);

	    if (i == 0 && text_len > 0) {
		_fill (& buffer [TEXT_COL - 1], ' ', text_len);
	    }
	}

/*	Write out a partial line.	*/

	if (extra_long_cnt > 0 || extra_byte_cnt > 0) {

	    if (text_len == 0) {
		int	ascii_cnt;

		ascii_cnt = (extra_long_cnt * sizeof (long)) + extra_byte_cnt;
		_move  (& buffer [TEXT_COL - 1], code_ptr, ascii_cnt);
		lex_filter_unprintable_chars ((unsigned char*)
			& buffer [TEXT_COL - 1], ascii_cnt,
			lex_m_filter_tab );
		buffer [TEXT_COL - 1 + ascii_cnt] = '\0';
	    }

/*	Clear code from previous lines, keeping the offset and text if
	it is there. */

	    _fill (buffer, ' ', OFFSET_COL - 1);

	    if (extra_long_cnt > 0) {

/*	Format the code longwords.	*/

		for (i = 0; i < extra_long_cnt; i++, code_ptr++) {
                    unsigned long temp_long;
		    if (BIT_64_LONG){
/*		      _move( (char*) &temp_long, (char*) code_ptr, sizeof(temp_long));*/
		      sprintf ((char *)hex_longword, "%lX", (* code_ptr));
		    }
		    else{
		      sprintf ((char *)hex_longword, "%08X", (*code_ptr));
		    }

		    _move (& buffer [start_hex_long [i]],
			hex_longword, HEX_PER_LONG);
		}
	    }

/*	Format the extra code bytes.	*/

	    if (extra_byte_cnt > 0) {
		int		l;
		unsigned char	extra_bytes [sizeof (long)];

		_move (extra_bytes, code_ptr, extra_byte_cnt);
		_fill (hex_longword, ' ', HEX_PER_LONG);
		for (l = extra_byte_cnt - 1; l >= 0; l--) {
		if (BIT_64_LONG)
		    sprintf ((char *)
			     & hex_longword [HEX_PER_LONG - (2 * (l + 1))],
			     "%02X", extra_bytes [l]);
		else
		    sprintf ((char *)
			     & hex_longword [HEX_PER_LONG - (2 * (l + 1))],
			     "%02X", extra_bytes [extra_byte_cnt-l-1]);
		
		}
		_move (& buffer [start_hex_long [extra_long_cnt]],
			hex_longword, HEX_PER_LONG);
	    }

/*	Output the partial line.	*/

	    lst_output_line( (char*)buffer, FALSE );

	    line_written = TRUE;

	}

	if (! line_written) {
	    if (text_len > 0) {
		lst_output_line((char*) buffer, FALSE );
	    } else {
		lst_output_line( " ", FALSE );
	    }
	}

    }    



    return;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function outputs a pointer line showing the position of
**	diagnostics to the UIL listing file.
**
**  FORMAL PARAMETERS:
**
**      az_src_rec	    ptr to a source record
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      message ptr line is written to the listing file
**
**--
**/

void	lst_output_message_ptr_line( az_src_rec, src_buffer )

src_source_record_type	*az_src_rec;
char			*src_buffer;

{
    src_message_item_type	*az_msg;
    char			buffer[ src_k_max_source_line_length + 3 ];
    char			*ptr_buffer;
    int				msg_no;
    int				pos;
    int				msg_pos;
    char			c_char;
    boolean			v_output_line;

    if (_src_null_access_key( az_src_rec->z_access_key) )
	return;

    msg_no = 9;

    buffer[ 0 ] = '\t';
    buffer[ 1 ] = '\t';
    ptr_buffer = &buffer[ 2 ];

    az_msg = az_src_rec->az_message_list;
    if (az_msg == NULL)
	return;
    msg_pos = az_msg->b_source_pos;
    if (msg_pos == diag_k_no_column)
	return;

    v_output_line = FALSE;
    
    for (pos = 0;  c_char = src_buffer[ pos ], c_char != 0; )
    {
	if (pos < msg_pos)
	{
	    if (c_char == '\t')
		ptr_buffer[ pos++ ] = '\t';
	    else
		ptr_buffer[ pos++ ] = ' ';

	    continue;
	}
    
	msg_no = (msg_no % 9) + 1;
	ptr_buffer[ pos++ ] = msg_no + '0';
	v_output_line = TRUE;

next_message:
	az_msg = az_msg->az_next_message;
	if (az_msg == NULL)
	    goto finished_scan;
	msg_pos = az_msg->b_source_pos;
	if ((pos-1) == msg_pos)			/* pos already advanced */
	    goto next_message;
	if (msg_pos == diag_k_no_column)
	    goto finished_scan;

    }

finished_scan:
    ptr_buffer[ pos ] = 0;

    if (v_output_line)
	lst_output_line( buffer, FALSE );

    return;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      lst_debug_output sends debugging info to the listing file if
**	there is one; if not the output goes to standard output.
**
**  FORMAL PARAMETERS:
**
**      standard Xprintf argument list
**
**  IMPLICIT INPUTS:
**
**      lst_v_listing_open
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**      print lines either in the listing file or to standard output
**
**--
**/

static	int	cur_pos=0;
static	char	buffer[132];

void	lst_debug_output

	(char *format, ...)
{
    va_list	ap;			/* ptr to variable length parameter */

    /*
    **	establish the start of the parameter list
    */

    va_start(ap,format);

    /*
    **	check if the listing file is open for output
    */

    if (lst_v_listing_open)
    {
	int	count;
	char	*ptr;

	vsprintf( &(buffer[cur_pos]), format, ap );

	for ( ptr=buffer; ptr[0] != '\0'; ptr += (count+1) )
	{
	    _assert( ptr <= &(buffer[132]), "Overflowed debug listing buffer" );
	    count = strcspn( ptr, "\n" );
	    if (count == strlen( ptr )) 
	    {
		cur_pos = ptr - buffer + count;
		return;
	    } 
	    else 
	    {
		ptr[ count ] = '\0';
	    }
	    lst_output_line( ptr, FALSE );
	}
	cur_pos = 0;

    }
    else
 	vprintf( format, ap );

    va_end(ap);
}
