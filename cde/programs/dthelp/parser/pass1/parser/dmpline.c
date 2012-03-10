/* $XConsortium: dmpline.c /main/3 1995/11/08 10:16:42 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"

/* Write input file and line number for an error message */
void m_dumpline(file, line)
M_WCHAR *file ;
int line ;
{
char buffer[10] ;

m_errline("Line ") ;
sprintf(buffer, "%d", line) ;
m_errline(buffer) ;
if (file)
    {
    char mb_file;

    m_errline(" of File ") ;
    mb_file = MakeMByteString(file);
    m_errline(mb_file) ;
    m_free(mb_file,"multi-byte string");
    }
}
