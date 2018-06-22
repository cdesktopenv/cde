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
/* $XConsortium: fproto.h /main/3 1995/11/08 10:46:14 rswiston $ */
/* Copyright (c) 1988, 1989, 1990 Hewlett-Packard Co. */
/* Function prototypes for HP Tag/TeX translator */


int m_lower(int c);

void options(LOGICAL filelenonly);

void setopt(char *string, LOGICAL filelenonly);

LOGICAL setvalopt(int thisopt, char *string, LOGICAL filelenonly);

char *strstr(const char *s1, const char *s2);

int m_upper(int c);

char *MakeMByteString(const M_WCHAR *from);

M_WCHAR *MakeWideCharString(const char *from);

void PutString(char *string);

void PutWString(M_WCHAR *string);

void PutWChar(M_WCHAR wchar);

void SaveWChar(M_WCHAR wchar);

void OpenTag(LOGICAL hasAttributes,
  LOGICAL newlineOK);

void CloseTag(LOGICAL newlineOK);

void ImpliedlAttribute(char    *name,
  M_WCHAR *value);

void RequiredAttribute(char    *name,
  M_WCHAR *value);

M_WCHAR *CopyWString(M_WCHAR *string);

void MakeNewElement(ElementTypes type);

void DeleteElement(ElementPtr pElement);

void OptimizeAndEmit(ElementPtr pVirpage);

void PushCurrentElement(void);

void PopCurrentElement(void);

char *mb_realloc(  char *ptr, long size);

char *mb_malloc(  long size);

void mb_free(  char **pptr);

void EmitSDL(ElementPtr pVirpage);

int OpenFile(char *name, int type, int code);

int ReadFile(int fd, char *name, char *buffer, int amount, int code);

int WriteFile(int fd, char *name, char *buffer, int amount, int code);

int FileExists(char *name);

void AssertFileIsReadable(char *name, int code);

int FileSize(char *name, int code);

void CloseFile(int fd, char *name, int code);

FILE *FopenFile(char *name, int type, int code);

int GetALine(FILE *file, char *name, char *line, int max, int code);

int FreadFile(FILE *file, char *name, char *buffer, int amount, int code);

void FwriteFile(FILE *file, char *name, char *buffer, size_t amount, int code);

void FcloseFile(FILE *file, char *name, int code);

int StringToUpper(char *string);

void BuildIndex(void);

void OpenDocument(void);

void CloseDocument(void);

void UpdateTossFileName(void);

void SetLocale(M_WCHAR *pLang, M_WCHAR *pCharset);
