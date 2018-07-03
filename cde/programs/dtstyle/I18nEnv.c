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
/* $TOG: I18nEnv.c /main/4 1997/08/11 12:31:10 samborn $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        I18nEnv.c
 **
 **   Description: Controls the Dtstyle I18N component interaction with 
 **                the environment.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <pwd.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include <DtHelp/LocaleXlate.h>  /* for locale equivalence between platforms */

#include "Main.h"
#include "I18nEnv.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static int GetUserEnv(I18nEnv *env);
static int FindCDELocaleName(UserEnv *uenv);
static int GetUserIMSelectionFile(I18nEnv *env);
static int GetUserFileName(I18nEnv *env);
static int ReadImSelectionFile(FileSel *fsel, FILE *fp);
static int GetImsList(I18nEnv *env, char *hostname);
static void TimeOutProc(XtPointer client_data, XtIntervalId* timer);
static void ReadPipe(XtPointer client_data, int *fd, XtInputId *id);
static int ProcessBuf(char *savebuf, I18nEnv *env);
static int CheckHostname(I18nEnv *env, char *hostname);
static void PutSelectMode(FILE *fp, int start_mode);
static void PutImsName(FILE *fp, char *im_name);
static void PutHostname(FILE *fp, char *hostname);
static void PutSelectionEntry(FILE *fp, char *tag, char *value);


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++*/
/* Global Variables                      */
/*+++++++++++++++++++++++++++++++++++++++*/

I18nEnv i18n_env;


/*+++++++++++++++++++++++++++++++++++++++*/
/* strcasecmp                            */
/*+++++++++++++++++++++++++++++++++++++++*/

#ifdef NEED_STRCASECMP
/*
 * In case strcasecmp is not provided by the system here is one
 * which does the trick.
 */
static int
strcasecmp(const char *s1,
	   const char *s2)
{
    int c1, c2;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}
#endif


/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtI18nGetEnvValues                   */
/*+++++++++++++++++++++++++++++++++++++++*/

int
_DtI18nGetEnvValues(
    I18nEnv *env
)
{
    int ret = NoError;

    /* Get the user environment */
    env->user_env = (UserEnv *) XtMalloc(sizeof(UserEnv));    
    ret = GetUserEnv(env);

    /* Get the user selection stored in the IM Selection File */
    if (ret == NoError) {
	env->file_sel = (FileSel *) XtMalloc(sizeof(FileSel));    
	ret = GetUserIMSelectionFile(env);
    }

    /* Alloc the user selection */
    env->ims_sel = (ImsSel *) XtMalloc(sizeof(ImsSel)); 
    env->ims_sel->ims_list = NULL;
    env->ims_sel->ims_list_size = 0 ;
    env->ims_sel->host_name = NULL ;

    /* Get the Input Method available on the selected host.
     * The selected host is the one stored in the IMS Selection File.
     */
    if (ret == NoError)
	ret = _DtI18nGetImList(env, env->file_sel->hostname);

    return ret;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* GetUserEnv                            */
/*+++++++++++++++++++++++++++++++++++++++*/
static int
GetUserEnv(
     I18nEnv *env
)
{
    UserEnv *uenv = env->user_env ;
    int ret = NoError;
    char buf[BUFSIZ], *p;

    /* get the host name */
    gethostname(buf, BUFSIZ);
    uenv->localhostname = XtNewString(buf);

    /* get the user name */
    if ((p = getlogin()) == NULL) {
      struct passwd *pw;

      pw = getpwuid(getuid());
      p = pw->pw_name;
    }
    
    uenv->username = XtNewString(p);

    /* get the display name */
    uenv->displayname = XtNewString(XDisplayString(XtDisplay(env->shell)));
    
    /* get the locale */
    if ((p = getenv("LANG")) && *p)
	uenv->locale = XtNewString(p);
    else
	return ErrNoLocale;
    
    /* find the CDE generic locale name */
    if (FindCDELocaleName(uenv) != NoError)
	return ErrNoCDELocale;
    
    /* get the home directory */
    if ((p = getenv("HOME")) && *p)
	uenv->homedir = XtNewString(p);
    else 
	ret = ErrNoHome;

    return ret;
}

static int
FindCDELocaleName(
    UserEnv *uenv
)
{
   _DtXlateDb db = NULL;
   int  ret = NoError;
   char plat[_DtPLATFORM_MAX_LEN];
   int  execver;
   int  compver;

   ret = _DtLcxOpenAllDbs(&db);

   if (ret == NoError)
       ret = _DtXlateGetXlateEnv(db, plat, &execver, &compver);

   if (ret == NoError)
       ret = _DtLcxXlateOpToStd(db, plat, compver, DtLCX_OPER_SETLOCALE,
				uenv->locale, &uenv->CDE_locale,
				NULL, NULL, NULL);
   if (ret == NoError)
       ret = _DtLcxCloseDb(&db);

   return ret;
}

static int
GetUserIMSelectionFile(
    I18nEnv *env
)
{
    int         ret = NoError;
    FILE	*fp = NULL;

    ret = GetUserFileName(env);
    
	if (ret == NoError) {
		/* Look if this file is readable */
		if ((fp = fopen(env->file_sel->fname, "r")) == NULL) {
			env->file_sel->start_mode = -1;
			return ErrNoSelectionFile;
		}
	}

    start_tag_line(env->file_sel->fname);
    ret = ReadImSelectionFile(env->file_sel, fp);

    if(fp) {
        fclose(fp);
    }

    return ret;
}

static int 
GetUserFileName(
    I18nEnv *env
)
{
    int                status, ret = NoError;
    char               *path, *tmp_path;
    int                len = 0;
    struct stat        buf;

    /* The user IMS Selection File should be of the following form:
     * $HOME/.dt/ims/[display-name]/CDE-locale
     */

    path = (char *) XtMalloc((MAXPATHLEN + 1) * sizeof(char));

    strcpy(path, env->user_env->homedir);
    strcat(path, DtUSER_IMSFS_DIR);

    /* Look if there is a display specific directory */

    tmp_path = (char *) XtMalloc((MAXPATHLEN + 1) * sizeof(char));
    
    strcpy(tmp_path, path);
    strcat(tmp_path, "/");
    strcat(tmp_path, env->user_env->displayname);

    if ((status = stat (tmp_path, &buf)) == 0) {
	strcat(path, "/");
	strcat(path, env->user_env->displayname);
    }

    XtFree(tmp_path);

    /* Now add the CDE-specific locale name */

    strcat(path, "/");
    strcat(path, env->user_env->CDE_locale);

    env->file_sel->fname = path;

    return ret;
}	    

static int
ReadImSelectionFile(
    FileSel	*fsel,
    FILE	*fp
)
{
    int         ret = NoError;
    char	*lp, *valp, *vp, *p;
    int         select_mode = 0;
    char        *imsname, *hostname;
    int         line_num, i;

    imsname = hostname = NULL;

	while ((line_num = read_tag_line(fp, &lp, &valp)) > 0) {
		if (!valp) {
			continue;
		}
		if (lp[0] != STR_PREFIX_CHAR) {
			continue;
		}
		if (strncmp(lp + 1, STR_SELECTMODE, 3) == 0) {
			if (str_to_int(valp, &i) && i >= 0) {
				select_mode = i;
			}
		} else if (strncmp(lp + 1, STR_IMSNAME, 4) == 0) {
			vp = valp; cut_field(valp);
			if (*vp) {
				XtFree(imsname);
				imsname = XtNewString(vp);
			}
		} else if (strncmp(lp + 1, STR_HOSTNAME, 4) == 0) {
			vp = valp; cut_field(valp);
			if (*vp) {
				XtFree(hostname);
				if (strcmp(vp, NAME_LOCAL)) {
					hostname = XtNewString(vp);
				}
			}
		}
	}

    fsel->im_name = imsname;
    fsel->hostname = hostname;
    fsel->start_mode = select_mode;

    return ret;

}

int 
_DtI18nGetImList(
    I18nEnv *env,
    char    *hostname )
{
    int         ret = NoError;
    int         host_type = HOST_LOCAL;

    if (hostname)
	host_type = CheckHostname(env, hostname);

    switch (host_type) {
	case HOST_UNKNOWN:
	    ret = ErrUnknownHost;
	    break;

	case HOST_REMOTE:
	    /* Put the host name in the ImsSel structure */
	    env->ims_sel->host_name = hostname;
	    ret = GetImsList(env, hostname);
	    break;

	case HOST_LOCAL:
	    if (hostname && strcasecmp(hostname, "local") != 0)
		env->ims_sel->host_name = hostname;
	    ret = GetImsList(env, env->user_env->localhostname);
	    break;
    }

    return ret;
}


static int 
GetImsList(
    I18nEnv     *env,
    char        *hostname )
{
    int		ret = NoError;
    char        pipe_command[255];
    FILE        *fp;
    struct stat  buf;
    unsigned long timeout;

    /* First check if dtimsstart is installed correctly */

    if ((ret = stat ("/usr/dt/bin/dtimsstart", &buf)) != NoError) {
	return ErrNoDtimsstart;
    }
    
    sprintf(pipe_command, "/usr/dt/bin/dtimsstart -listname -hostname %s", 
	    hostname);

    if (fp = popen(pipe_command, "r")) {

	/* Set the sensitivity of the InputMethod Title Box to False until we
	 * are done reading the new information. */
	_DtI18nSetSensitiveImTB(env, False);

	/* Initialize the pipe record. */
	if (!(env->pipe_info))
	    env->pipe_info = (PipeRec *) XtMalloc(sizeof(PipeRec));
	
	env->pipe_info->pipe = fp;  /* to close it */
 	env->pipe_info->input_id =  /* to remove it */
	    XtAppAddInput (XtWidgetToApplicationContext(env->shell), 
			   fileno(fp), (XtPointer) XtInputReadMask, 
			   ReadPipe, (XtPointer) env); 

	/* Also add a timeout in case the pipe ain't talk */
	timeout = (unsigned long) (style.xrdb.pipeTimeOut) * 1000;
	env->pipe_info->timer_id = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(env->shell), 
			    timeout, TimeOutProc, (XtPointer)env);
    } else {
	ret = ErrNoPopen;
    }

    return ret;
}
    
static void TimeOutProc (XtPointer client_data, 
			 XtIntervalId* timer)
{
    I18nEnv *env = (I18nEnv *) client_data;

    /* the command is not fast enough,  but calling pclose blocks
       and then print "Broken Pipe". I need to kill the child somehow 
    pclose(env->pipe_info->pipe);*/

    XtRemoveInput(env->pipe_info->input_id);

    /* Set the sensitivity of the InputMethod Title Box back to True. */
    _DtI18nSetSensitiveImTB(env, True);

    _DtI18nErrorDialog(ErrTimeOut) ;
}

static void
ReadPipe (
    XtPointer    client_data,
    int          *fd,
    XtInputId    *id )

{
    char      buf[512];
    int       i, nbytes;
    int       status = NoError;
    static    char * savebuf = NULL; 
    static    int savebuf_bytes = 0 ;
    I18nEnv   *env = (I18nEnv *) client_data;
	
    nbytes = read (*fd, buf, 512);

    if (nbytes > 0) {
	savebuf = XtRealloc(savebuf, savebuf_bytes + nbytes);
	memcpy(savebuf+savebuf_bytes, buf, nbytes);
	savebuf_bytes += nbytes ;
    } else {
	if (savebuf)
	    *(savebuf + savebuf_bytes)  = '\0';
	status = ProcessBuf(savebuf, client_data);
	savebuf_bytes = 0;
	if (savebuf) {
	    XtFree(savebuf);
	    savebuf = NULL;
	}

	pclose(env->pipe_info->pipe);
	XtRemoveInput(*id);

	/* Set the sensitivity of the InputMethod Title Box back to True. */
	_DtI18nSetSensitiveImTB(env, True);

	/* Remove timer too */
	XtRemoveTimeOut(env->pipe_info->timer_id);

	if (status != NoError)
	    _DtI18nErrorDialog(status) ;
    }

}

static int
ProcessBuf(
     char    *savebuf, 
     I18nEnv *env )
{
    int    i, n = 0;
    int    ret = NoError;
    ImsEnt *ims_ent;
    char * filename, * label ;

    /* The dtimsstart execution gave back an empty buffer */
    if (!savebuf)
	return(ErrRemoteFailed);
	
    /* parse savebuf: The lines have the following syntax: 
     * [#](im_filename) im_label  or
     * [#]im_filename im_label
     * where "#" if present means the default IM,
     *       im_filename is the file name where all the IM info is stored,
     *       im_label is the label to present to the user.
     */
    while (*savebuf) {
	/* We're at the beginning of a new line */

	/* grow the array */
	env->ims_sel->ims_list = 
	    (ImsEnt *) XtRealloc((char *) env->ims_sel->ims_list,
				 sizeof(ImsEnt)*(n+1));

	ims_ent = &(env->ims_sel->ims_list[n]);

	/* Look to see if this is the default one. */
	if (*savebuf == '#') {
	    ims_ent->im_default = 1;
	    savebuf++;
	} else
	    ims_ent->im_default = 0;

	/* Look to see if it is not reachable. */
	if (*savebuf == '(') {
	    ims_ent->inactive = 1;
	    savebuf++ ;
	} else 
	     ims_ent->inactive = 0 ;

	/* Mark begin of im filename */
	filename = savebuf ;
	/* Go to the end of it */
	while (!isspace(*savebuf++)) ;
	/* We're on the first space, mark the end of the im filename,
	 * don't forget to count the ')' if inactive. */
	*(savebuf - ims_ent->inactive - 1) = '\0';

	/* Mark the beginning of the im name */
	label = savebuf ;
	while (*savebuf != '\n') savebuf++ ;
	/* mark the end */
	*savebuf = '\0';
	savebuf++;

	/* copy the string data here. it's gonna be freed after that */
	ims_ent->im_name = XtNewString(filename) ;
	ims_ent->im_label = XtNewString(label) ;
	n++;
    }	   

    env->ims_sel->ims_list_size = n;

    return(ret);
}


static int	
CheckHostname(
    I18nEnv     *env,
    char	*hostname
)
{
    int		host_type = HOST_UNKNOWN;
    char	*local = env->user_env->localhostname;
    struct hostent	*hp;
    unsigned long 	addr = 0L;
    static unsigned long  local_addr = 0L;

    if (!hostname || !*hostname || strcasecmp(hostname, "local") == 0
			|| strcasecmp(hostname, local) == 0) {
	host_type =  HOST_LOCAL;
    } else {		/* compare inet address */
	if (!local_addr) {
	    if ((hp = gethostbyname(local)) && hp->h_addrtype == AF_INET) {
		local_addr = *((unsigned long *) hp->h_addr_list[0]);
	    } else {
		host_type = HOST_REMOTE;
	    }
	}
	if (host_type == HOST_UNKNOWN) {
	    if ((hp = gethostbyname(hostname)) && hp->h_addrtype == AF_INET) {
		addr = *((unsigned long *) hp->h_addr_list[0]);
		if (addr == local_addr)
		    host_type = HOST_LOCAL;
		else
		    host_type = HOST_REMOTE;
	    } else {
		host_type = HOST_UNKNOWN;
	    }
	}
    }

    return host_type;
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtI18nWriteImSelectionFile - writes  */
/* the values saved in the FileSel       */
/* structure to the IMS Selection File.  */
/*+++++++++++++++++++++++++++++++++++++++*/
int
_DtI18nWriteImSelectionFile(
     I18nEnv *env
)
{
    FILE	*fp;

    
    /* Look if this file is writable */
    if ((fp = fopen(env->file_sel->fname, "w")) == NULL)
	return ErrFileCreate;

    /* Write the select mode */
    PutSelectMode(fp, env->file_sel->start_mode);

    /* Write the IM selected if not null */
    PutImsName(fp, env->file_sel->im_name);

    /* Write the hostname if not null */
    PutHostname(fp, env->file_sel->hostname);
    
    /* Close the file */
    fclose(fp);

	return NoError;
}

static void
PutSelectMode(
     FILE *fp,
     int  start_mode
)
{
    char val[20];
    
    sprintf(val, "%ld", (long)start_mode);
    PutSelectionEntry(fp, STR_SELECTMODE, val);
}

static void
PutImsName(
     FILE *fp,
     char *im_name
)
{
    char *valp;

    if ((valp = im_name) && *valp)
	PutSelectionEntry(fp, STR_IMSNAME, valp);
}

static void
PutHostname(
     FILE *fp,
     char *hostname
)
{
    char *valp;

    if ((valp = hostname) && *valp)
	PutSelectionEntry(fp, STR_HOSTNAME, valp);
}


static void 
PutSelectionEntry(
     FILE *fp,
     char *tag,
     char *value
)
{
    fprintf(fp, "%c%s%c\t%s\n", STR_PREFIX_CHAR, tag, TAG_END_CHAR, value);
}
