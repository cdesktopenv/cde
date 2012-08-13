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
 * File:         spc-termio.c $TOG: spc-termio.c /main/5 1998/04/03 17:08:32 mgreess $
 * Language:     C
 *
 * (c) Copyright 1990, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h>
#include <bms/stringbuf.h>

#include <termios.h>
#if !defined(linux) && !defined(CSRG_BASED)
#include <sys/termio.h>
#endif
#include <codelibs/stringx.h>

#include <SPC/spcP.h>

#ifdef __cplusplus
#define STRTOKX(b,f) strtokx((b), (f))
#else
#define STRTOKX(b,f) strtokx((char **)(&b), (f))
#endif

/***************************************************************************************

The routines in this file decode and encode a termios struct back and forth between the
actual binary representation of the structure and an ASCII string representation which
may be portably sent across the network.  

 <version>!<i-flags>!<o-flags>!<c-flags>!<l-flags>!<i-speed>!<o-speed>!<cc-array>

  Where:

     <version> == the termios struct protocal version (currently XPG3)

     <i-flags> == comma separated list of input mode flags as defined in
                  the termios.h header file.

     <o-flags> == comma separated list of output mode flags as defined in
                  the termios.h header file.

     <c-flags> == comma separated list of control mode flags as defined in
                  the termios.h header file.  The terminal speed info is
                  not part of this list.

     <i-speed> == The input speed as defined in the termios.h header file.  

     <o-speed> == The output speed as defined in the termios.h header file.  

     <cc-array> == comma separated list of control control character names and 
                   values formatted as (<name>=<value),<name>=<value>...)

******************************************************************************************/

#define SPC_TERMIO_VER_FIELD 	1	/* Define field indicies for above string */
#define SPC_TERMIO_IMODE_FIELD 	2
#define SPC_TERMIO_OMODE_FIELD 	3
#define SPC_TERMIO_CMODE_FIELD 	4
#define SPC_TERMIO_LMODE_FIELD 	5
#define SPC_TERMIO_ISPEED_FIELD 6
#define SPC_TERMIO_OSPEED_FIELD 7
#define SPC_TERMIO_CC_FIELD 	8
#define SPC_TERMIO_LAST_FIELD 	SPC_TERMIO_CC_FIELD

#define SPC_TERMIO_SEP "!"	     	/* These three MUST be only 1 char */
#define SPC_TCFLAG_SEP ","
#define SPC_CC_SEP     "="		

#define SPC_CC_FORMAT  "%u"		/* CC values are printed with this format */

struct modes_s
{
    XeString	name;
    tcflag_t	value;
};

struct baud_s
{
    XeString	name;
    speed_t	speed;
};

struct cc_s
{
    XeString	name;
    cc_t	value;
};

/* The following lists are the valid componants of a "struct termios"   */
/* as defined in XPG3.  These MUST be kept up to date with the X/Open   */
/* standard.  No platform specific items can exist here as we pass this */
/* data to other systems on the network and they be from any vendor.    */
/* -------------------------------------------------------------------- */

#define SPC_TERMIO_VERSION "XPG3"

#define END_OF_LIST "End_Of_List"
    
static struct modes_s Input_modes[] = {
    "BRKINT",	BRKINT,
    "ICRNL",	ICRNL,
    "IGNBRK",	IGNBRK,
    "IGNCR",	IGNCR,
    "IGNPAR",	IGNPAR,
    "INLCR",	INLCR,
    "INPCK",	INPCK,
    "ISTRIP",	ISTRIP,
    "IXOFF",	IXOFF,
    "IXON",	IXON,
    "PARMRK",	PARMRK,
    END_OF_LIST, 0
};

static struct modes_s Output_modes[] = {    
    "OPOST",	OPOST,
    "ONLCR",	ONLCR,
    "OCRNL",	OCRNL,
    "ONOCR",	ONOCR,
    "ONLRET",	ONLRET,
#if !defined(CSRG_BASED)
    "OFILL",	OFILL,
    "OFDEL",	OFDEL,
    "NLDLY",	NLDLY,
    "NL0",	NL0,
    "NL1",	NL1,
    "CRDLY",	CRDLY,
    "CR0",	CR0,
    "CR1",	CR1,
    "CR2",	CR2,
    "CR3",	CR3,
    "TABDLY",	TABDLY,
    "TAB0",	TAB0,
    "TAB1",	TAB1,
    "TAB2",	TAB2,
    "TAB3",	TAB3,
    "BSDLY",	BSDLY,
    "BS0",	BS0,
    "BS1",	BS1,
    "VTDLY",	VTDLY,
    "VT0",	VT0,
    "VT1",	VT1,
    "FFDLY",	FFDLY,
    "FF0",	FF0,
    "FF1",	FF1,
#endif
    END_OF_LIST, 0
};


static struct baud_s Baud_rates[] = {    
    "B0",	B0,
    "B50",	B50,
    "B75",	B75,
    "B110",	B110,
    "B134",	B134,
    "B150",	B150,
    "B200",	B200,
    "B300",	B300,
    "B600",	B600,
    "B1200",	B1200,
    "B1800",	B1800,
    "B2400",	B2400,
    "B4800",	B4800,
    "B9600",	B9600,
    "B19200",	B19200,
    "B38400",	B38400,
    END_OF_LIST, 0
};


static struct modes_s Control_modes[] = {    
    "CSIZE",	CSIZE,
    "CS5",	CS5,
    "CS6",	CS6,
    "CS7",	CS7,
    "CS8",	CS8,
    "CSTOPB",	CSTOPB,
    "CREAD",	CREAD,
    "PARENB",	PARENB,
    "PARODD",	PARODD,
    "HUPCL",	HUPCL,
    "CLOCAL",	CLOCAL,
    END_OF_LIST, 0
};


static struct modes_s Local_modes[] = {    
    "ECHO",	ECHO,
    "ECHOE",	ECHOE,
    "ECHOK",	ECHOK,
    "ECHONL",	ECHONL,
    "ICANON",	ICANON,
    "IEXTEN",	IEXTEN,
    "ISIG",	ISIG,
    "NOFLSH",	NOFLSH,
    "TOSTOP",	TOSTOP,
    END_OF_LIST, 0
};


static struct cc_s CC_Array[] = {    
    "VEOF",	VEOF,
    "VEOL",	VEOL,
    "VERASE",	VERASE,
    "VINTR",	VINTR,
    "VKILL",	VKILL,
    "VMIN",	VMIN,
    "VQUIT",	VQUIT,
    "VSTART",	VSTART,
    "VSTOP",	VSTOP,
    "VSUSP",	VSUSP,
    "VTIME",	VTIME,
    END_OF_LIST, 0
};

    
/*----------------------------------------------------------------------+*/
static void 
    SPC_Decode_TCflag(tcflag_t       flag, 
		      struct modes_s modes[], 
		      XeStringBuffer buff)
/*----------------------------------------------------------------------+*/
{
    Boolean first_time = TRUE;
    
    /* Given a binary representation of flag (tcflag_t), convert */
    /* it into a comma separated list of ASCII flag names.            */

    while (strcmp(modes->name, END_OF_LIST) != 0)
    {
	if ( (modes->value & flag) == modes->value)
	{
	    if (first_time) 
		first_time = FALSE;
	    else
		XeAppendToStringBuffer(buff, SPC_TCFLAG_SEP);

	    XeAppendToStringBuffer(buff, modes->name);
	}
	modes++;
    }
    
    /* We need at least one blank for strtokx to work when */
    /* we take this string apart on the other side.        */
    /* --------------------------------------------------- */
    if (first_time)
	XeAppendToStringBuffer(buff, (XeString)" ");
}

/*----------------------------------------------------------------------+*/
static tcflag_t 
    SPC_Encode_TCflag(XeString       buff,  
		      struct modes_s modes[])
/*----------------------------------------------------------------------+*/
{
    /* Given a comma separated list of flag names, convert the list */
    /* into a "tcflag_t" (binary) representation of that list.      */

    XeString item;
    tcflag_t flag = (tcflag_t)0;
    struct modes_s *orig_modes = modes;
    
    if (!*buff) 
	return flag;
    
    while (item = (XeString)STRTOKX(buff, SPC_TCFLAG_SEP))
    {
	Boolean found = FALSE;
	
	modes = orig_modes;
	while (strcmp(modes->name, END_OF_LIST) != 0)
	{
	    if (strcmp(modes->name, item) == 0)
	    {
		flag |= modes->value;
		found = TRUE;
		break;
	    }
	    modes++;
	}

	if (!found)
	    SPC_Error(SPC_Bad_Termios_Mode, item);
    }

    return flag;
}

/*----------------------------------------------------------------------+*/
static void 
    SPC_Decode_Speed(speed_t        speed, 
		     struct baud_s  bauds[], 
		     XeStringBuffer buff)
/*----------------------------------------------------------------------+*/
{
    /* Given a binary "speed_t" speed specification, convert it into */
    /* a single ASCII string.					     */

    while (strcmp(bauds->name, END_OF_LIST) != 0)
    {
	if (bauds->speed == speed)
	{
	    XeAppendToStringBuffer(buff, bauds->name);
	    return;
	}
	bauds++;
    }

    /* We need at least one blank for strtokx to work when */
    /* we take this string apart on the other side.        */
    /* --------------------------------------------------- */
    XeAppendToStringBuffer(buff, (XeString)" ");
}

/*----------------------------------------------------------------------+*/
static speed_t 
    SPC_Encode_Speed(XeString      buff,  
		     struct baud_s bauds[])
/*----------------------------------------------------------------------+*/
{
#define DEFAULT_SPEED B9600
    
    /* Given a single ASCII name of a termio speed item, convert it into */
    /* a binary (speed_t) representation.			         */
    
    if (!*buff) 
	return DEFAULT_SPEED;
    
    while (strcmp(bauds->name, END_OF_LIST) != 0)
    {
	if (strcmp(bauds->name, buff) == 0)
	    return bauds->speed;
	bauds++;
    }
    
    SPC_Error(SPC_Bad_Termios_Speed, buff);
    return DEFAULT_SPEED;
}

/*----------------------------------------------------------------------+*/
static void 
    SPC_Decode_CC(cc_t           ccs[], 
		  struct cc_s    cc_array[], 
		  XeStringBuffer buff)
/*----------------------------------------------------------------------+*/
{
    Boolean first_time = TRUE;

    /* Given an array of cc_s from a termios struct (binary	   */
    /* representation), convert it into a comma sepearted list of  */
    /* CC names and values of the form <name>=<value>		   */
    
    while (strcmp(cc_array->name, END_OF_LIST) != 0)
    {
	int index = ccs[cc_array->value];
	if ( index != 0 )
	{
	    XeChar num[30];

	    if (first_time) 
		first_time = FALSE;
	    else
		XeAppendToStringBuffer(buff, SPC_TCFLAG_SEP);

	    XeAppendToStringBuffer(buff, cc_array->name);
	    XeAppendToStringBuffer(buff, SPC_CC_SEP);
	    sprintf(num, SPC_CC_FORMAT, index);
	    XeAppendToStringBuffer(buff, num);
	}
	cc_array++;
    }

    /* We need at least one blank for strtokx to work when */
    /* we take this string apart on the other side.        */
    /* --------------------------------------------------- */
    if (first_time)
	XeAppendToStringBuffer(buff, (XeString)" ");
}

/*----------------------------------------------------------------------+*/
static void 
    SPC_Encode_CC(XeString    buff,  
		  cc_t        ccs[], 
		  struct cc_s cc_array[])
/*----------------------------------------------------------------------+*/
{
    /* we should have gotten a string that looks like this:		  */
    /*									  */
    /* <V-Name>=<value>,<V-Name>=<value>...				  */
    /*									  */
    /* For example:   VEOF=4,VKILL=8					  */
    /*									  */
    /* Parse this array and store the values in the "ccs" array passed in */
    /* to us.  It is assumed that the <V-name> strings are all defined in */
    /* the "cc_array" list passed to us.                                  */
    /* ------------------------------------------------------------------ */

    XeString cc_name, cc_value;
    int i;
    unsigned int a_cc;
    struct cc_s *orig_cc_array = cc_array;

    for(i=0; i<NCCS; i++) 
	ccs[i] = 0;
    
    if (!*buff) return;
    
    while (cc_name = (XeString)STRTOKX(buff, SPC_TCFLAG_SEP))
    {
	Boolean found = FALSE;

        cc_value = strchr(cc_name, SPC_CC_SEP[0]);	/* Go find the "=" */
	if (!cc_value)
	{
	    SPC_Error(SPC_Bad_Termios_CC, cc_name);
	    continue;
	}
	*cc_value++ = XeChar_NULL;			/* Replace "=" with null */
	

	/* Look for "V-Name" in table ... */
	/* ------------------------------ */
	cc_array = orig_cc_array;
	while (strcmp(cc_array->name, END_OF_LIST) != 0)
	{
	    if (strcmp(cc_array->name, cc_name) == 0)
	    {
		if (sscanf(cc_value, SPC_CC_FORMAT, &a_cc) != 1)
		{
		    *(cc_value-1) = SPC_CC_SEP[0];
		    SPC_Error(SPC_Bad_Termios_CC, cc_name);
		}
		else
		    ccs[cc_array->value] = a_cc;

		found = TRUE;
		break;
	    }
	    cc_array++;
	}

	if (!found)
	{
	    *(cc_value-1) = SPC_CC_SEP[0];
	    SPC_Error(SPC_Bad_Termios_CC, cc_name);
	}
    }
}

/*----------------------------------------------------------------------+*/
XeString SPC_Decode_Termios(struct termios *tio)
/*----------------------------------------------------------------------+*/
{
    /* Given a termios struct, return an ascii string representation of */
    /* it as defined at the front of this file.				*/

#define RESULT_BUFF_SIZE 32

    XeString all;
    speed_t speed;
    XeStringBuffer result = XeMakeStringBuffer(RESULT_BUFF_SIZE);

    XeAppendToStringBuffer(result, SPC_TERMIO_VERSION);
    XeAppendToStringBuffer(result, SPC_TERMIO_SEP);
    
    SPC_Decode_TCflag(tio->c_iflag, Input_modes, result);
    XeAppendToStringBuffer(result, SPC_TERMIO_SEP);
    
    SPC_Decode_TCflag(tio->c_oflag, Output_modes, result);
    XeAppendToStringBuffer(result, SPC_TERMIO_SEP);

    SPC_Decode_TCflag(tio->c_cflag, Control_modes, result);
    XeAppendToStringBuffer(result, SPC_TERMIO_SEP);

    SPC_Decode_TCflag(tio->c_lflag, Local_modes, result);
    XeAppendToStringBuffer(result, SPC_TERMIO_SEP);
    
    speed = cfgetispeed(tio);
    SPC_Decode_Speed(speed, Baud_rates, result);
    XeAppendToStringBuffer(result, SPC_TERMIO_SEP);

    speed = cfgetospeed(tio);
    SPC_Decode_Speed(speed, Baud_rates, result);
    XeAppendToStringBuffer(result, SPC_TERMIO_SEP);

    SPC_Decode_CC(tio->c_cc, CC_Array, result);
    
    all = strdup(result->buffer);

    XeFree(result->buffer);
    XeFree(result);

    return all;
}

/*----------------------------------------------------------------------+*/
void SPC_Encode_Termios(XeString buff, struct termios *tio)
/*----------------------------------------------------------------------+*/
{
    /* Decodes the ascii representation of the termios struct.  The format */
    /* of the string is defined at the front of this file.                 */

    XeString	item;
    XeString	protocol_version;
    int		item_cnt = 0;
    speed_t     speed;    
    
    while (item = (XeString)STRTOKX(buff, SPC_TERMIO_SEP))
    {
	/* We can possibly have an all blank field.  Walk past them  */
	/* because the routines we will be calling don't expect any  */
	/* blanks in the string (but they do check for empty stings).*/
	while (*item == (XeChar)' ') item++;
	
	switch(++item_cnt) 
        {
	case SPC_TERMIO_VER_FIELD : 
	    protocol_version = item; 
	    /* Check this some day ??? */
	    break;

	case SPC_TERMIO_IMODE_FIELD : 
	    tio->c_iflag = SPC_Encode_TCflag(item, Input_modes); 
	    break;
	    
	case SPC_TERMIO_OMODE_FIELD : 
	    tio->c_oflag = SPC_Encode_TCflag(item, Output_modes); 
	    break;
	    
	case SPC_TERMIO_CMODE_FIELD : 
	    tio->c_cflag = SPC_Encode_TCflag(item, Control_modes); 
	    break;
	    
	case SPC_TERMIO_LMODE_FIELD : 
	    tio->c_lflag = SPC_Encode_TCflag(item, Local_modes); 
	    break;
	    
	case SPC_TERMIO_ISPEED_FIELD : 
	    speed = SPC_Encode_Speed(item, Baud_rates);
	    cfsetispeed(tio, speed);
	    break;
	    
	case SPC_TERMIO_OSPEED_FIELD : 
	    speed = SPC_Encode_Speed(item, Baud_rates);
	    cfsetospeed(tio, speed);
	    break;
	    
	case SPC_TERMIO_CC_FIELD : 
	    SPC_Encode_CC(item, tio->c_cc, CC_Array); 
	    break;

	default : 
	    break;
	} /* switch */


    } /* while */

    if (item_cnt != SPC_TERMIO_LAST_FIELD)
    	SPC_Error(SPC_Bad_Termios_Proto, 
		  (XeString) ((item_cnt < SPC_TERMIO_LAST_FIELD) ? "Too Few" : "Too Many"));

}    

#ifdef TESTING
/*----------------------------------------------------------------------+*/
main()
/*----------------------------------------------------------------------+*/
{
    XeString s, s1, s2;
    int i;
    struct termios tio, tio2;
    speed_t speed;
    
    tio.c_iflag = BRKINT | IGNPAR | ICRNL | IXON;
    tio.c_oflag = OPOST | ONLCR;
    tio.c_cflag = CS7 | CREAD | CLOCAL;
    tio.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK;

    tio.c_reserved = 0;    
    for(i=0; i<NCCS; i++) 
	tio.c_cc[i] = 0;
    tio.c_cc[VERASE] = 101;
    tio.c_cc[VEOF]   = 102;
    tio.c_cc[VSTOP]  = 103;
    

    tio2.c_iflag = 0;
    tio2.c_oflag = 0;
    tio2.c_cflag = 0;
    tio2.c_lflag = 0;

    tio2.c_reserved = 0;    
    for(i=0; i<NCCS; i++) 
	tio2.c_cc[i] = 0;

    cfsetispeed(&tio, B9600);
    cfsetospeed(&tio, B1200);
    
    s1 = strdup(s = SPC_Decode_Termios(&tio));
    printf("Decoded string=\n<%s>\n\n", s);


    SPC_Encode_Termios(s, &tio2);
    s2 = strdup(s = SPC_Decode_Termios(&tio2));
    printf("String after Encoding/decoding=\n<%s>\n\n", s2);

    if (strcmp(s1, s2) == 0)
	printf("...Identical ...\n");
    else
        printf("...Mismatch ...\n");
    
}    
#endif /* TESTING */
