/* $XConsortium: out.c /main/3 1995/11/08 10:46:48 rswiston $ */
/*   Copyright (c) 1994 Hewlett-Packard Co. */
/* Functions relevant to output of data characters */

#include "userinc.h"
#include "globdec.h"

#if defined(M_PROTO)
void PutString(char *string)
#else
void PutString(string)
char *string;
#endif
{
fputs(string, outFile);
}

#if defined(M_PROTO)
void PutWString(M_WCHAR *string)
#else
void PutWString(string)
M_WCHAR *string;
#endif
{
char *mb_string;

mb_string = MakeMByteString(string);
fputs(mb_string, outFile);
m_free(mb_string, "Multi-byte string");
}

#if defined(M_PROTO)
void PutWChar(M_WCHAR wchar)
#else
void PutWChar(wchar)
M_WCHAR wchar;
#endif
{
M_WCHAR wc_string[2];
char *mb_string;

wc_string[0] = wchar;
wc_string[1] = 0;
mb_string = MakeMByteString(wc_string);
fputs(mb_string, outFile);
m_free(mb_string, "Multi-byte string");
}

#if defined(M_PROTO)
void SaveWChar(M_WCHAR wchar)
#else
void SaveWChar(wchar)
M_WCHAR wchar;
#endif
{
CDATA_el *pCdata;

if (pCurrentElement->type != e_cdata)
    return; /* we're in an error condition */

pCdata = &(pCurrentElement->u.u_cdata);

pCdata->buffPos++;
if (pCdata->buffPos == pCdata->buffSize)
    {
    pCdata->buffSize += 256;
    pCdata->buffer = m_realloc(pCdata->buffer,
			       pCdata->buffSize,
			       "u_cdata.buffer");
    }
pCdata->buffer[pCdata->buffPos] = wchar;
}
