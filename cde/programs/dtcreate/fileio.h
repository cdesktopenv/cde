/* $XConsortium: fileio.h /main/5 1995/11/01 16:14:03 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  fileio.h                                                               */
/*                                                                         */
/***************************************************************************/

#ifndef _FILEIO_H_INCLUDED
#define _FILEIO_H_INCLUDED

#ifndef STORAGECLASS
#ifdef  NOEXTERN
#define STORAGECLASS
#else
#define STORAGECLASS extern
#endif
#endif

/***************************************************************************/
/*                                                                         */
/*  Prototypes for functions                                               */
/*                                                                         */
/***************************************************************************/

ushort  WriteDefinitionFile(char *, ActionData *);
ushort  WriteActionFile(ActionData *);
Boolean check_file_exists(char *);
int     OpenDefinitionFile(char *, ActionData *);
ushort  CopyIconFiles(ActionData *);
char *  CreateIconName(char *, char *, enum icon_size_range, char *, Boolean);
Boolean ActionHasIcon (void);
Boolean FiletypeHasIcon (FiletypeData *);


#endif /* _FILEIO_H_INCLUDED */
