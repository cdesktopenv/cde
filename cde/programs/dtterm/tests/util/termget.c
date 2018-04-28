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
/* $XConsortium: termget.c /main/3 1995/10/31 12:03:12 rswiston $ */
#include <stdio.h>
#include <fcntl.h>
#include <sys/termios.h>

FILE *Ptr;

LogError(Str)
char *Str;
{
    fprintf(Ptr, "%s \n", Str);
}

main()
{
  int FilePtr, i, Error=0;
  struct termios termio_orig;

    Ptr = fopen("term.log", "a");
    if ((FilePtr = open("/dev/tty", O_RDWR)) < 0) 
       {LogError("Could not open tty "); exit(-1);}
    if(tcgetattr(FilePtr, &termio_orig) < 0) 
       {LogError("tcgetattr failed "); exit(-1);}
    if (termio_orig.c_cc[VINTR] != '!') 
       {LogError("intr not set"); Error = 1;}
    if (termio_orig.c_cc[VQUIT] != '@') 
       {LogError("quit not set"); Error = 1;}
    if (termio_orig.c_cc[VERASE] != '#') 
       {LogError("erase not set"); Error = 1;}
    if (termio_orig.c_cc[VKILL] != '$') 
       {LogError("kill not set"); Error = 1;}
    if (termio_orig.c_cc[VEOF] != '%') 
       {LogError("eof not set"); Error = 1;}
    if (termio_orig.c_cc[VEOL] != '^') 
       {LogError("eol not set"); Error = 1;}
    if (termio_orig.c_cc[VSWTCH] != '&') 
       {LogError("swtch not set"); Error = 1;}
    if (termio_orig.c_cc[VSTART] != '*') 
       {LogError("start not set"); Error = 1;}
    if (termio_orig.c_cc[VSTOP] != '(') 
       {LogError("stop not set"); Error = 1;}
    if (termio_orig.c_cc[VSUSP] != ')') 
       {LogError("susp not set"); Error = 1;}
    if (Error == 1) LogError("Test Failed");
    else LogError("Test Passed");
    fclose(Ptr);
}

