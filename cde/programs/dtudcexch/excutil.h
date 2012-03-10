/* $XConsortium: excutil.h /main/6 1996/12/23 08:48:26 barstow $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#define	EXPORT	1
#define	IMPORT	2
#define EXCERRMALLOC	101
#define EXCERRNOUDC	102

void	Ecs();
void	Ecd();
/*void	getpcffn();*/
void	getbdffn();
void	selcharcd();
void	getcharcd();
void	createbdf();
int	bigger();
int	smaller();
int	c2h();
char	*renge2num();
void	freeExcdata();
void	checkdata();
void	excexit();
void	excterminate();
void	freeld();
void	getexistcode();
void	AskUser();
void	excerror();
Widget	excCreatePushButton();

typedef struct {
    char * charset;
    char * udcrenge;
} UDCcsREC;

typedef struct {
    int		function;
    XtAppContext	app;
    Widget	toplevel;
    char	*locale;
    int		csnum;
    UDCcsREC	*cslist;
    char	*xlfdname;
    char	*style;
    char	*size;
    char	*fontfile;
    char	*bdffile;
    char	bdfmode[2];
    char	*udcrenge;
    int		code_num;
    int		*gpf_code_list;
    int		*bdf_code_list;
    int		comment_num;
    char	**comment_list;
} Exc_data;

typedef struct {
    Widget	list;
    int		allcode_num;
    int		*allcode;
    int		existcode_num;
    int		*existcode;
    char	**existcode_c;
    Exc_data	*ed;
} ListData;

#ifndef NO_MESSAGE_CATALOG
# ifdef __ultrix
#  define _CLIENT_CAT_NAME "dtudcexch.cat"
# else  /* __ultrix */
#  define _CLIENT_CAT_NAME "dtudcexch"
# endif /* __ultrix */
# ifdef _NO_PROTO
extern char *_DtGetMessage();
# else  /* _NO_PROTO */
extern char *_DtGetMessage(
                        char *filename,
                        int set,
                        int n,
                        char *s );
# endif /* _NO_PROTO */
#define GETMESSAGE(set, number, string) GetMessage(set, number, string)
static char *
GetMessage(set, number, string)
int set, number;
char *string;
{
    char *tmp;
    static char * point[100];
    static int first = True;

    if (first) {
	memset(point, 0, sizeof(char *) * 100);
	first = False;
    }
    if (point[number])
	return(point[number]);
    tmp = _DtGetMessage(_CLIENT_CAT_NAME, set, number, string);
    point[number] = (char *) malloc(strlen(tmp) + 1);
    strcpy(point[number], tmp);
    return (point[number]);
} 
#else /* NO_MESSAGE_CATALOG */
# define GETMESSAGE(set, number, string)\
    string
#endif /* NO_MESSAGE_CATALOG */
