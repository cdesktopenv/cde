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
/* Copyright 1992     Hewlett-Packard Co. */
static char *version = "$XConsortium: helpcomp.c /main/3 1995/11/08 11:10:34 rswiston $";

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

typedef char  LineBuff[BUFSIZ];
typedef char  FileBuff[BUFSIZ];
typedef char *FileName;

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

static char     *prog_name;
static char      fileNamePrefix[] = "hc";
static FileName  newHvFileName, newHtFileName,
		 topicFileName, topicZFileName;


/* issues an error message, cleans up temp files and exits */
void
ErrorExit(const char *who, const char *how)
{
fprintf(stderr, "%s -> %s: %s\n", prog_name, who, how);

if (newHvFileName)  unlink(newHvFileName);
if (newHtFileName)  unlink(newHtFileName);
if (topicFileName)  unlink(topicFileName);
if (topicZFileName) unlink(topicZFileName);

exit(1);
}


/* converts errno into a string and calls ErrorExit() */
void
ErrorIntExit(const char *who, const int how)
{
ErrorExit(who, strerror(how));
}


/* copies a file by name to another file by name */
int
CopyFile(const char *toName, const char *fromName)
{
int      toFile, fromFile;
int      bytesRead;
FileBuff fileBuff;

toFile = open(toName, O_WRONLY|O_CREAT, 0666);
if (toFile < 0)
    {
    ErrorIntExit(toName, errno);
    }

fromFile = open(fromName, O_RDONLY);
if (fromFile < 0)
    {
    ErrorIntExit(fromName, errno);
    }

while ((bytesRead = read(fromFile, fileBuff, sizeof(fileBuff))) > 0)
    {
    if (write(toFile, fileBuff, bytesRead) != bytesRead)
	{
	ErrorIntExit(toName, errno);
	}
    if (bytesRead < sizeof(fileBuff))
	break;
    }

close(fromFile);
close(toFile);

if (bytesRead < 0)
    return 1;

return 0;
}


/*
 * Here's the idea:
 *
 * Open the old .hv file and a temp file to contain the new .hv file.
 *
 * Copy lines from the old to the new up to and including the line
 * "# Topic Locations".
 *
 * Then, iteratively look for lines containing the string ".filename:",
 * copying lines from the old .hv to the new until such a line is found.
 * Copy the found line to the new .hv as well.
 *
 * If the file name specified after the found string specifies a new
 * file, finish writing the last open .ht if one exists (i.e., we are
 * not in the first pass through the loop.  Close the previous old .ht
 * file and the previous new .ht file and move the new one to replace
 * the old one.  Open the recent .ht file specified and open a temp file
 * to contain the new .ht file.
 *
 * Read the next line from the old .hv file to get the offset into the
 * file of the next topic.  Subtract the current offset from the next
 * offset to get the size of the current topic.  Read that many bytes
 * from the old .ht file into a newly created topic file.  Close the topic
 * file and execute compress(1) on it.  Attempt to open the topic file
 * this time with a .Z extension.  If possible, the topic was compressed.
 * If not possible, reopen the topic file as an uncompressed topic.
 *
 * Append either a 0x0 (compressed) or 0xff (uncompressed) byte to the
 * new .ht file followed by three bytes holding the size of the topic
 * determined by fstat'ing the open topic file.  The three bytes are
 * computed using div/mod rather than right shift and mask to avoid byte
 * sex problems.
 * 
 * Append the contents of the topic file to the new .ht file.
 *
 * Continue opening, compressing and appending compressed topic files
 * until a blank line is found in the old .hv file.
 *
 * When the blank line is found, copy the remaining lines of the old .hv
 * file to the new .hv file.  When finished, move the new .hv file to 
 * replace the old one.
*/
int
main(int argc, char **argv)
{
char         *pc;
FILE         *oldHvFile, *newHvFile;
int           oldHtFile,  newHtFile, topicZFile;
FileName      oldHvFileName;
LineBuff      lineBuff, string1, string2, lastString2, command;
FileBuff      fileBuff;
int           oldOffset, newOffset;
int           bytesToRead, bytesRead, totalBytes, zFileSize;
unsigned char zTmp[4];
int           doBreak, firstPass, result;
struct stat   statBuf;

pc = strrchr(argv[0], '/');
if (pc) pc++;
else pc = argv[0];
prog_name = malloc(strlen(pc) + 1);
if (!prog_name)
    {
    fprintf(stderr, "%s: could not copy the program name (no memory)", pc);
    return 1;
    }
strcpy(prog_name, pc);

if (argc != 2)
    {
    fprintf(stderr, "usage: %s <help volume base name>\n", prog_name);
    return 1;
    }

oldHvFileName = malloc(strlen(argv[1]) + 3 + 1); /* add ".hv" and null */
if (!oldHvFileName)
    {
    ErrorExit("malloc", "could not create a temporary file name");
    }
strcpy(oldHvFileName, argv[1]);
strcat(oldHvFileName, ".hv");
oldHvFile = fopen(oldHvFileName, "r");
if (!oldHvFile)
    {
    ErrorIntExit(oldHvFileName, errno);
    }

newHvFileName = tempnam(NULL, fileNamePrefix);
if (!newHvFileName)
    {
    ErrorExit("tempnam", "could not create a temporary file name");
    }
newHvFile = fopen(newHvFileName, "w");
if (!newHvFile)
    {
    ErrorIntExit(newHvFileName, errno);
    }

do  {
    if (!fgets(lineBuff, sizeof(lineBuff), oldHvFile))
	{
	ErrorExit(oldHvFileName, "premature end of input file");
	}
    fputs(lineBuff, newHvFile);
    }
while (strncmp(lineBuff, "# Topic Locations", sizeof("# Topic Locations") - 1));


firstPass = 1;

topicFileName = tempnam(NULL, fileNamePrefix);
if (!topicFileName)
    {
    ErrorExit("tempnam", "could not create a temporary file name");
    }
strcpy(command, "compress ");
strcat(command, topicFileName);

topicZFileName = malloc(strlen(topicFileName) + 2 + 1); /* add ".Z" and null */
if (!topicZFileName)
    {
    ErrorExit("malloc", "could not create a temporary file name");
    }
strcpy(topicZFileName, topicFileName);
strcat(topicZFileName, ".Z");

newHtFileName = tempnam(NULL, fileNamePrefix);
if (!newHtFileName)
    {
    ErrorExit("tempnam", "could not create a temporary file name");
    }
newHtFile = open(newHtFileName, O_WRONLY|O_CREAT, 0666);
if (!newHtFile)
    {
    ErrorIntExit(newHtFileName, errno);
    }
totalBytes = 0;

doBreak = 0;
result  = 0;
while (1)
    {
    if (!fgets(lineBuff, sizeof(lineBuff), oldHvFile))
	{
	ErrorExit(oldHvFileName, "premature end of input file");
	}
    if (*lineBuff != '\n')
	{
	char *pc;

	fputs(lineBuff, newHvFile);
	sscanf(lineBuff, "%s %s", string1, string2);
	pc = strrchr(string1, '.');
	if (!pc || (strcmp(pc, ".filename:") != 0))
	    continue;
	if (firstPass)
	    {
	    firstPass = 0;
	    oldOffset = 0;
	    strcpy(lastString2, string2);
	    if ((oldHtFile = open(string2, O_RDONLY)) < 0)
		{
		ErrorIntExit(string2, errno);
		}
	    }
	}
    else
	{
	doBreak = 1;
	string2[0] = 0;
	}
    if (strcmp(string2, lastString2) != 0)
	{
	topicZFile = open(topicFileName, O_WRONLY|O_CREAT, 0666);
	if (topicZFile < 0)
	    {
	    ErrorIntExit(topicFileName, errno);
	    }
	while (bytesRead = read(oldHtFile, fileBuff, sizeof(fileBuff)))
	    {
	    if (write(topicZFile, fileBuff, bytesRead) != bytesRead)
		{
		ErrorIntExit(string2, errno);
		}
	    if (bytesRead < sizeof(fileBuff))
		break;
	    }
	close(topicZFile);
	system(command);

	zTmp[0] = 0;
	if ((topicZFile = open(topicZFileName, O_RDONLY)) < 0)
	    {
	    zTmp[0] = ~0;
	    topicZFile = open(topicFileName, O_RDONLY);
	    }
	if (topicZFile < 0)
	    {
	    char *who;

	    who = (char *) malloc(strlen(topicFileName)  +
				  strlen(topicZFileName) +
				  sizeof(" or "));
	    strcpy(who, topicFileName);
	    strcat(who, " or ");
	    strcat(who, topicZFileName);
	    ErrorIntExit(who, errno);
	    }

	if (fstat(topicZFile, &statBuf) < 0)
	    {
	    ErrorIntExit(topicZFileName, errno);
	    }

	zFileSize = statBuf.st_size;

	zTmp[3] = zFileSize % 256;
	zFileSize /= 256;
	zTmp[2] = zFileSize % 256;
	zFileSize /= 256;
	zTmp[1] = zFileSize % 256;
	if (write(newHtFile, zTmp, 4) != 4)
	    {
	    ErrorIntExit(newHtFileName, errno);
	    }

	while (bytesRead = read(topicZFile, fileBuff, sizeof(fileBuff)))
	    {
	    if (write(newHtFile, fileBuff, bytesRead) != bytesRead)
		{
		ErrorIntExit(string2, errno);
		}
	    if (bytesRead < sizeof(fileBuff))
		break;
	    }
	close(topicZFile);
	unlink(topicFileName);
	unlink(topicZFileName);

	unlink(lastString2);
	result = CopyFile(lastString2, newHtFileName);
	unlink(newHtFileName);

	if (doBreak || result)
	    break;

	newHtFile = open(newHtFileName, O_WRONLY|O_CREAT, 0666);
	if (!newHtFile)
	    {
	    ErrorIntExit(newHtFileName, errno);
	    }
	totalBytes = 0;

	strcpy(lastString2, string2);
	oldOffset = 0;
	if ((oldHtFile = open(string2, O_RDONLY)) < 0)
	    {
	    ErrorIntExit(string2, errno);
	    }
	}
    if (!fgets(lineBuff, sizeof(lineBuff), oldHvFile))
	{
	ErrorIntExit(oldHvFileName, errno);
	}
    sscanf(lineBuff, "%s %d", string1, &newOffset);
    if (newOffset != oldOffset)
	{
	bytesToRead = newOffset - oldOffset;
	topicZFile = open(topicFileName, O_WRONLY|O_CREAT, 0666);
	if (topicZFile < 0)
	    {
	    ErrorIntExit(topicFileName, errno);
	    }
	while (bytesRead = read(oldHtFile,
				fileBuff,
				MIN(bytesToRead, sizeof(fileBuff))))
	    {
	    if (write(topicZFile, fileBuff, bytesRead) != bytesRead)
		{
		ErrorIntExit(topicFileName, errno);
		}
	    if ((bytesToRead -= bytesRead) == 0)
		break;
	    }
	close(topicZFile);
	system(command);

	zTmp[0] = 0;
	if ((topicZFile = open(topicZFileName, O_RDONLY)) < 0)
	    {
	    zTmp[0] = ~0;
	    topicZFile = open(topicFileName, O_RDONLY);
	    }
	if (topicZFile < 0)
	    {
	    char *who;

	    who = (char *) malloc(strlen(topicFileName)  +
				  strlen(topicZFileName) +
				  sizeof(" or "));
	    strcpy(who, topicFileName);
	    strcat(who, " or ");
	    strcat(who, topicZFileName);
	    ErrorIntExit(who, errno);
	    }

	if (fstat(topicZFile, &statBuf) < 0)
	    {
	    ErrorIntExit(topicZFileName, errno);
	    }

	zFileSize = statBuf.st_size;

	zTmp[3] = zFileSize % 256;
	zFileSize /= 256;
	zTmp[2] = zFileSize % 256;
	zFileSize /= 256;
	zTmp[1] = zFileSize % 256;
	if (write(newHtFile, zTmp, 4) != 4)
	    {
	    ErrorIntExit(newHtFileName, errno);
	    }

	totalBytes += statBuf.st_size + 4;

	while (bytesRead = read(topicZFile, fileBuff, sizeof(fileBuff)))
	    {
	    if (write(newHtFile, fileBuff, bytesRead) != bytesRead)
		{
		ErrorIntExit(newHtFileName, errno);
		}
	    if (bytesRead < sizeof(fileBuff))
		break;
	    }
	close(topicZFile);
	unlink(topicFileName);
	unlink(topicZFileName);

	fprintf(newHvFile, "%s\t%d\n", string1, totalBytes);
	oldOffset = newOffset;
	}
    else
	fputs(lineBuff, newHvFile);
    }

if (result)
    {
    ErrorExit(lastString2, "bad file copy");
    }

putc('\n', newHvFile);
while (!feof(oldHvFile))
    {
    fgets(lineBuff, sizeof(lineBuff), oldHvFile);
    if (feof(oldHvFile))
	break;
    fputs(lineBuff, newHvFile);
    }
fclose(oldHvFile);
fclose(newHvFile);

unlink(oldHvFileName);
result = CopyFile(oldHvFileName, newHvFileName);
if (result)
    {
    ErrorExit(oldHvFileName, "bad file copy");
    }
unlink(newHvFileName);

return 0;
}
