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
/* $TOG: dttypes.c /main/6 1998/04/20 13:01:30 mgreess $ */
/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */
#include	<sys/types.h>
#include	<locale.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<Xm/Xm.h>
#include	<X11/Shell.h>
#define INIT          register char *sp = instring;
#define GETC()        (*sp++)
#define PEEKC()       (*sp)
#define UNGETC(c)     (--sp)
#define RETURN(c)     return(c)
#define ERROR(c)      {rexp_errno = c; return((char *)0);}
static	int	rexp_errno = 0;
#if defined(CSRG_BASED)
#include	<regex.h>
#else
#include	<regexp.h>
#endif
#include	<nl_types.h>
#include	<Dt/Dt.h>
#include	<Dt/DtsMM.h>
#include	<Dt/Dts.h>
#include	<Dt/EnvControlP.h>

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE  0
#endif 

extern	char **	_DtsMMListDb(void);
static	enum	errors
{
	BAD_DB,
	NO_ARGUMENT,
	REC_INFO_IN_STATE,
	NO_STATE,
	INVALID_ARG
} MyErrors;

static	int	read_errors = 0;
static	char	*error_str[] =
{
	"Invalid DataBase\n",			 /* BAD_DB */
	"Not a valid argument\n",		 /* NO_ARGUMENT */
	"invalid rec_info field in -w option\n", /* REC_INFO_IN_STATE */
	"Not in -w or -l option\n",		 /* NO_STATE */
	"Unknown option\n"			 /* INVALIDE_ARG */
};

static	enum
{
	r_info,
	r_name,
	f_name,
	f_value
} list_shift;

typedef struct
{
	int	display_list;
	int	*rec_list;
	int	rec_count;
	int	rec_total;
	char	*db_name;
	char	*rec_name;
	char	*fld_name;
	char	*fld_value;
	char	*display_fld;
} List;

static	nl_catd dtcatd = 0;

static int
init(int *argc, char **argv)
{
	Widget		toplevel;
	XtAppContext	appContext;
	Arg 		args[20];


	toplevel = XtInitialize(argv[0], "Dttype", NULL, 0,
		(int *) argc, argv);

	XtSetArg(args[0], XmNallowShellResize, 1);
	XtSetArg(args[1], XmNmappedWhenManaged, 0);
	XtSetArg(args[2], XmNheight, 1);
	XtSetArg(args[3], XmNwidth, 1);
	XtSetValues(toplevel, args, 4);
	XtRealizeWidget(toplevel);

	if (DtInitialize(XtDisplay(toplevel), toplevel, argv[0],
			  "Dttype") == False)
	{
		fprintf(stderr, "couldn't initialize everthing\n");
		return(0);
	}

	/* Load the filetype/action dbs; DtActionInvoke() requires this */
	_DtDtsMMInit(0);
	return(1);
}

void
add_rec(int rec, List *l)
{
	int	i;

	if(l->rec_total == 0 || l->rec_count >= l->rec_total-2)
	{
		l->rec_total += 10;
		l->rec_list = (int *)realloc(l->rec_list, (l->rec_total)*sizeof(int));
	}
	l->rec_list[l->rec_count] = rec;
	l->rec_count++;
	l->rec_list[l->rec_count] = -1;
}

void
rec_list(List *l)
{
	int		i;
	int		db;
	int		rec;
	int		fld;
	DtDtsMMDatabase	*db_ptr;
	DtDtsMMDatabase	*db_ptr_list;
	DtDtsMMRecord	*rec_ptr;
	DtDtsMMRecord	*rec_ptr_list;
	DtDtsMMField	*fld_ptr;
	DtDtsMMField	*fld_ptr_list;

#if defined(CSRG_BASED)
	regex_t		regex_rn;
	regex_t		regex_fn;
	regex_t		regex_fv;
	regex_t		regex_df;
#else
	char		expbuf_rn[2000];
	char		expbuf_fn[2000];
	char		expbuf_fv[2000];
	char		expbuf_df[2000];

	memset(expbuf_rn, '\0', sizeof(expbuf_rn));
	memset(expbuf_fn, '\0', sizeof(expbuf_fn));
	memset(expbuf_fv, '\0', sizeof(expbuf_fv));
	memset(expbuf_df, '\0', sizeof(expbuf_df));
#endif

#if defined(CSRG_BASED)
	if(regcomp(&regex_rn, l->rec_name?l->rec_name:"^.*", 0) != 0)
#else
	if((compile(l->rec_name?l->rec_name:"^.*",
			expbuf_rn,
			&expbuf_rn[sizeof(expbuf_rn)],
			0)) == (char *)0)
#endif
	{
		/* error */
		fprintf(stderr, catgets(dtcatd, 1, 36, "error in regular expression %s\n"), l->rec_name?l->rec_name:"(NULL)");
		exit(1);
	}

#if defined(CSRG_BASED)
	if(regcomp(&regex_fn, l->fld_name?l->fld_name:"^.*", 0) != 0)
#else
	if((compile(l->fld_name?l->fld_name:"^.*",
			expbuf_fn,
			&expbuf_fn[sizeof(expbuf_fn)],
			0)) == (char *)0)
#endif
	{
		/* error */
		fprintf(stderr, catgets(dtcatd, 1, 36, "error in regular expression %s\n"), l->fld_name?l->fld_name:"(NULL)");
		exit(1);
	}

#if defined(CSRG_BASED)
	if(regcomp(&regex_fv, l->fld_value?l->fld_value:"^.*", 0) != 0)
#else
	if((compile(l->fld_value?l->fld_value:"^.*",
			expbuf_fv,
			&expbuf_fv[sizeof(expbuf_fv)],
			0)) == (char *)0)
#endif
	{
		/* error */
		fprintf(stderr, catgets(dtcatd, 1, 36, "error in regular expression %s\n"), l->fld_value?l->fld_value:"(NULL)");
		exit(1);
	}

#if defined(CSRG_BASED)
	if(regcomp(&regex_df, l->display_fld?l->display_fld:"^.*", 0) != 0)
#else
	if((compile(l->display_fld?l->display_fld:"^.*",
			expbuf_df,
			&expbuf_df[sizeof(expbuf_df)],
			0)) == (char *)0)
#endif
	{
		/* error */
		fprintf(stderr, catgets(dtcatd, 1, 36, "error in regular expression %s\n"), l->display_fld?l->display_fld:"(NULL)");
		exit(1);
	}

	db_ptr = _DtDtsMMGet(l->db_name);
	rec_ptr_list = _DtDtsMMGetPtr(db_ptr->recordList);
	for(rec = 0; rec < db_ptr->recordCount; rec++)
	{
		rec_ptr = &rec_ptr_list[rec];
		fld_ptr_list = _DtDtsMMGetPtr(rec_ptr->fieldList);
#if defined(CSRG_BASED)
		if(regexec(&regex_rn,
			  (char *)_DtDtsMMBosonToString(rec_ptr->recordName),
			  0, NULL, 0) == 0)
#else
		if(advance((char *)_DtDtsMMBosonToString(rec_ptr->recordName), expbuf_rn) != 0)
#endif
		{
			for(fld = 0; fld < rec_ptr->fieldCount; fld++)
			{
				char *fn;
				char *fv;

				fld_ptr = &fld_ptr_list[fld];

				fn = _DtDtsMMExpandValue(_DtDtsMMBosonToString(fld_ptr->fieldName));
				fv = _DtDtsMMExpandValue(_DtDtsMMBosonToString(fld_ptr->fieldValue));

#if defined(CSRG_BASED)
				if((regexec(&regex_fn, fn, 0, NULL, 0) == 0) &&
				   ((fld_ptr->fieldValue==0?
					regexec(&regex_fv,
						catgets(dtcatd, 1, 4, "NULL"),
						0, NULL, 0):
					regexec(&regex_fv,
						fv,
						0, NULL, 0)) == 0))
#else
				if((advance(fn, expbuf_fn) != 0) &&
				   ((fld_ptr->fieldValue==0?advance(catgets(dtcatd, 1, 4, "NULL"), expbuf_fv):advance(fv, expbuf_fv)) != 0))
#endif
				{
					add_rec(rec, l);
					_DtDtsMMSafeFree(fn);
					_DtDtsMMSafeFree(fv);
					break;
				}
				_DtDtsMMSafeFree(fn);
				_DtDtsMMSafeFree(fv);
			}
		}
	}

	for(i = 0; l->rec_list && l->rec_list[i] != -1; i++)
	{
		rec = l->rec_list[i];
		rec_ptr =  &rec_ptr_list[rec];
		if(l->display_list&(1<<r_info) ||
		   l->display_list&(1<<r_name) &&
		   (l->display_list&(1<<f_name) ||
		    l->display_list&(1<<f_value)))
		{
			printf(catgets(dtcatd, 1, 5, "=============== %s ===============\n"), 
				rec_ptr->recordName?(char *)_DtDtsMMBosonToString(rec_ptr->recordName):catgets(dtcatd, 1, 6, ""));
		}
		else if (l->display_list&(1<<r_name))
		{
			printf(catgets(dtcatd, 1, 7, "%s\n"), 
				rec_ptr->recordName?(char *)_DtDtsMMBosonToString(rec_ptr->recordName):catgets(dtcatd, 1, 8, ""));
		}
		if(l->display_list&(1<<r_info))
		{
			char	*path = (char *)_DtDtsMMBosonToString(rec_ptr->pathId);

			printf(catgets(dtcatd, 1, 9, "loaded from %s\n"), path?path:catgets(dtcatd, 1, 10, "Unknown"));
		}

		fld_ptr_list = _DtDtsMMGetPtr(rec_ptr->fieldList);
		for(fld = 0; fld < rec_ptr->fieldCount; fld++)
		{
			int	term = 0;
			char *fn;
			char *fv;

			fld_ptr = &fld_ptr_list[fld];

			fn = _DtDtsMMExpandValue(_DtDtsMMBosonToString(fld_ptr->fieldName));
			fv = _DtDtsMMExpandValue(_DtDtsMMBosonToString(fld_ptr->fieldValue));

			if(l->display_fld)
			{
#if defined(CSRG_BASED)
				if(regexec(&regex_df, fn, 0, NULL, 0) == 0)
#else
				if(advance(fn, expbuf_df) !=0)
#endif
				{
					printf(catgets(dtcatd, 1, 11, "\t%s"), 
						fn?fn:catgets(dtcatd, 1, 12, ""));
					printf(catgets(dtcatd, 1, 13, " :\t%s"), 
						fld_ptr->fieldValue?fv:
							catgets(dtcatd, 1, 14, ""));
					printf("%s", catgets(dtcatd, 1, 15, "\n"));
				}

			}
			else
			{
				if(l->display_list&(1<<f_name))
				{
					printf(catgets(dtcatd, 1, 11, "\t%s"), fld_ptr->fieldName?fn:catgets(dtcatd, 1, 12, ""));
					term++;
				}
				if(l->display_list&(1<<f_value))
				{
					printf(catgets(dtcatd, 1, 13, " :\t%s"), fld_ptr->fieldValue?fv:catgets(dtcatd, 1, 14, ""));
					term++;
				}
				if(term)printf("%s", catgets(dtcatd, 1, 15, "\n"));
			}
			_DtDtsMMSafeFree(fn);
			_DtDtsMMSafeFree(fv);
		}
	}
}

int
parse_args(List *l, int argc, char **argv)
{
	int	*rl;
	int	df = 0;
	int	i;
	int	error = 0;
	enum	st
	{
		none,
		where,
		list
	};
	enum	st	state = none;

	l->display_list = 0;
	l->rec_list = 0;
	l->rec_count = 0;
	l->db_name = 0;
	l->rec_name = 0;
	l->fld_name = 0;
	l->fld_value = 0;

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], catgets(dtcatd, 1, 16, "-db")) == 0)
		{
			if(argv[i+1])
			{
				if(!_DtDtsMMGet(argv[i+1]))
				{
					error |= 1<<BAD_DB;
				}
				l->db_name = argv[++i];
			}
			else
			{
				error |= 1<<NO_ARGUMENT;
			}
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 17, "-w")) == 0)
		{
			state = where;
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 18, "-l")) == 0)
		{
			state = list;
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 19, "rec_info")) == 0)
		{
			if(state == where)
			{
				error |= 1<<REC_INFO_IN_STATE;
			}
			else if(state == list)
			{
				l->display_list|= 1<<r_info;
			}
			else
			{
				error |= 1<<NO_STATE;
			}
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 20, "rec_name")) == 0)
		{
			if(state == where)
			{
				if(!argv[i+1])
				{
					error |= 1<<NO_ARGUMENT;
				}
				else
				{
					l->rec_name = argv[++i];
				}
			}
			else if(state == list)
			{
				l->display_list |= 1<<r_name;
			}
			else
			{
				error |= 1<<NO_STATE;
			}
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 21, "fld_name")) == 0)
		{
			if(state == where)
			{
				if(!argv[i+1])
				{
					error |= 1<<NO_ARGUMENT;
				}
				else
				{
					l->fld_name = argv[++i];
				}
			}
			else if(state == list)
			{
				if(!argv[i+1])
				{
					error |= 1<<NO_ARGUMENT;
				}
				else
				{
					l->display_list |= 1<<f_name;
					if(strcmp(argv[i+1], catgets(dtcatd, 1, 22, "fld_value")) &&
					   strcmp(argv[i+1], catgets(dtcatd, 1, 21, "fld_name"))  &&
					   strcmp(argv[i+1], catgets(dtcatd, 1, 20, "rec_name"))  &&
					   strcmp(argv[i+1], catgets(dtcatd, 1, 19, "rec_info"))  )
					{
						l->display_fld = argv[++i];
					}
				}
			}
			else
			{
				error |= 1<<NO_STATE;
			}
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 22, "fld_value")) == 0)
		{
			if(state == where)
			{
				if(!argv[i+1])
				{
					error |= 1<<NO_ARGUMENT;
				}
				else
				{
					l->fld_value = argv[++i];
				}
			}
			else if(state == list)
			{
				l->display_list |= 1<<f_value;
			}
			else
			{
				error |= 1<<NO_STATE;
			}
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 23, "all")) == 0)
		{
			if(state == where)
			{
				if(!argv[i+1])
				{
					error |= 1<<NO_ARGUMENT;
				}
				else
				{
					l->fld_value = argv[++i];
				}
			}
			else if(state == list)
			{
				l->display_list|= 1<<r_info;
				l->display_list |= 1<<r_name;
				l->display_list |= 1<<f_name;
				l->display_list |= 1<<f_value;
			}
			else
			{
				error |= 1<<NO_STATE;
			}
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 26, "-type")) == 0)
		{
			char	*type, *new;

			if(!argv[i+1])
			{
				error |= 1<<NO_ARGUMENT;
			}
			else
			{
				printf(catgets(dtcatd, 1, 27, "%s is of type %s\n"),
					argv[i+1],
					type = DtDtsFileToDataType(argv[i+1]));
				new = (char *)malloc(strlen(type)+5);
				strcpy(new, type);
				strcat(new, catgets(dtcatd, 1, 28, "$"));
				l->rec_name = new;
				l->db_name = "DATA_ATTRIBUTES";
				l->display_list|= 1<<r_info;
				l->display_list |= 1<<r_name;
				l->display_list |= 1<<f_name;
				l->display_list |= 1<<f_value;
			}
			i++;
		}
		else if(strcmp(argv[i], catgets(dtcatd, 1, 35, "-help")) == 0)
		{
			usage();
		}
		else
		{
			error |= 1<<INVALID_ARG;
		}
		if(error)
		{
			int	j;

			if(!read_errors)
			{
				for(j = 0; j < XtNumber(error_str); j++)
				{
					error_str[j] = catgets(dtcatd, 1, 30+j, error_str[j]);
				}
				read_errors = ~0;
			} 
			fprintf(stderr, catgets(dtcatd, 1, 24, "Arg = %s\n"), argv[i]);
			for(j = 0; error; j++)
			{
				if(error & 1<<j)
				{
					fprintf(stderr, "%s", error_str[j]);
					error = error & ~(1<<j);
				}
			}
			usage();
			break;
		}
	}
	if(error)
	{
		return(0);
	}
	return(1);
}

usage()
{
	fprintf(stderr, "%s", catgets(dtcatd, 1, 25, "usage:  dttypes [-help]\n\tdttypes [-type filename]\n\tdttypes [-db database] [-w [rec_name regexp] [fld_name regexp]\n\t\t\t[fld_value regexp]]\n\t\t[-l [rec_name] [rec_info] [fld_name regexp] [fld_value]]\n"));
	exit(1);
}

main(int argc, char **argv)
{
	List	l;
	char	**dbs;
	int	*rl;
	int	df = 0;
	int	i;
	int	error = 0;
	char	*locale;
	enum	st
	{
		none,
		where,
		list
	};
	enum	st	state = none;

	locale = setlocale(LC_ALL, "");
	if(!locale)
	{
		perror("setlocale");
	}
	_DtEnvControl(DT_ENV_SET);

	dtcatd = catopen("dttypes", NL_CAT_LOCALE);
	if(dtcatd == (nl_catd)-1)
	{
		perror("catopen");
	}

	memset(&l, '\0', sizeof(l));

	if(!init(&argc, argv))
	{
		exit(1);
	}

	if(!parse_args(&l, argc, argv))
	{
		exit(1);
	}
	if(!(l.display_list&(1<<r_info)  ||
	     l.display_list&(1<<f_name)  ||
	     l.display_list&(1<<f_value) ||
	     l.display_list&(1<<r_name)  ))
	{
		l.display_list |= 1<<r_name;
		l.display_list |= 1<<r_info;
		l.display_list |= 1<<f_name;
		l.display_list |= 1<<f_value;
	}
	if(l.display_list&(1<<r_info)) l.display_list |= 1<<r_name;
	if(l.display_list&(1<<f_name)) l.display_list |= 1<<r_name;
	if(l.display_list&(1<<f_value)) l.display_list |= 1<<r_name;
	if(l.display_list == 0)  l.display_list |= 1<<r_name;

	if(l.db_name)
	{
		rec_list(&l);
	}
	else
	{
		dbs = _DtsMMListDb();
		for(i = 0; dbs[i]; i++)
		{
			printf(catgets(dtcatd, 1, 29, "--------------------- %s ----------------\n"),
				dbs[i]);
			l.db_name = dbs[i];
			rec_list(&l);
			free(l.rec_list);
			l.rec_list = 0;
			l.rec_count = 0;
			l.rec_total = 0;
		}
	}


	return(0);
}
