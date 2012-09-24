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
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 *+SNOTICE
 *
 *	$TOG: Dts.c /main/18 1999/10/15 12:18:39 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#if defined (__hpux)
/*
 * On HP MAXINT is defined in both <values.h> and <sys/param.h>
 */
#undef MAXINT
#elif defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include <values.h>
#endif
#include <sys/param.h>
#include <sys/errno.h>
#ifdef	_SUN_OS
#include <libgen.h>
#endif
#include <sys/utsname.h>
#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <Dt/DtNlUtils.h>
#include "Dt/DtsMM.h"
#include "Dt/Dts.h"
#include "DtSvcLock.h"
#include <Dt/UserMsg.h>

#include <X11/Xosdefs.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

struct	list
{
	int boson;
	DtDtsMMRecord *rec;
};

typedef	struct	type_info
{
	char			*file_path;
	char			*name;
	const struct stat	*file_stat;
	const struct stat	*file_lstat;
	int			file_fd;
	const char		*buffer;
	int			buff_size;
	int			mmap_size_to_free;
	int			size_to_free;
	const char		*opt_name;
	int			error;
	const char		*link_path;
	const char		*link_name;
	int			set_datatype;
	char			*ot;
	char			*mb;
	int			mb_size;
	int			name_type;
	int			*name_prev;
	int			name_count;
	char			*orig_attr;
} type_info_t;

static	DtShmBoson	dtdts_path_pattern = 0;
static	DtShmBoson	dtdts_name_pattern = 0;
static	DtShmBoson	dtdts_mode = 0;
static	DtShmBoson	dtdts_link_name = 0;
static	DtShmBoson	dtdts_link_path = 0;
static	DtShmBoson	dtdts_content = 0;
static	DtShmBoson	dtdts_data_attributes_name = 0;
static	DtShmBoson	dtdts_da_is_action = 0;
static	DtShmBoson	dtdts_da_icon = 0;
static	DtShmBoson	dtdts_da_description = 0;
static	DtShmBoson	dtdts_da_label = 0;

#define	MB_SIZE	100
#define _MBLEN(p) (mblen(p, MB_CUR_MAX) > 1 ? mblen(p, MB_CUR_MAX) : 1)
#define _MBADV(p) ((p) += _MBLEN(p))

/* external functions */
extern	char	*strdup(const char *);
extern	FILE	*popen(const char *, const char *);
extern	char	*DtActionIcon(const char *);
extern	char	*DtActionDescription(const char *);
extern	char	*DtActionLabel(const char *);
extern  XtAppContext _DtAppContext;

/* local functions */
static	DtDtsMMRecord *	name_list(type_info_t *linfo,
		DtDtsMMDatabase	*db,
		DtDtsMMRecord *rec_ptr);
static	const struct stat *get_stat();

static int      csh_match(const char *, const char *);
static int      csh_match_star(const char *, const char *);

/* filetype comparison function for sorting */
extern	int	cde_dc_compare(DtDtsMMRecord **entry1, DtDtsMMRecord **entry2);
extern	int	cde_da_compare(DtDtsMMRecord **entry1, DtDtsMMRecord **entry2);
extern	int	cde_dc_field_compare(DtDtsMMField **entry1, DtDtsMMField **entry2);


_DtDtsClear()
{
        _DtSvcProcessLock();
	dtdts_path_pattern = 0;
	dtdts_name_pattern = 0;
	dtdts_mode = 0;
	dtdts_link_name = 0;
	dtdts_link_path = 0;
	dtdts_content = 0;
	dtdts_data_attributes_name = 0;
	dtdts_da_is_action = 0;
	dtdts_da_icon = 0;
	dtdts_da_description = 0;
	dtdts_da_label = 0;
	_DtSvcProcessUnlock();
}

static int
gmatch(const char *string, const char *pattern)
{
	wchar_t         string_ch;
	int             k;
	wchar_t         pattern_ch;
	wchar_t         lower_bound;
	char            *p;

top:
	for (; 1; _MBADV(pattern), _MBADV(string))
	{
		lower_bound = (wchar_t)MAXINT;
		mbtowc(&string_ch, string, MB_CUR_MAX);
		mbtowc(&pattern_ch, pattern, MB_CUR_MAX);
		switch (pattern_ch)
		{
		case L'[':
			k = 0;
			for(mbtowc(&pattern_ch, _MBADV(pattern), MB_CUR_MAX);
                            pattern_ch != L'\0';
                            mbtowc(&pattern_ch, _MBADV(pattern), MB_CUR_MAX))
			{
				switch (pattern_ch)
				{
				case L']':
					if (!k)
					{
						return 0;
					}
					_MBADV(string);
					_MBADV(pattern);
					goto top;
				case L'-':
					if(lower_bound <= string_ch) {
					    p = (char *)pattern;
					    mbtowc(&pattern_ch, _MBADV(pattern),
						   MB_CUR_MAX);
					    k |= (string_ch <= pattern_ch);
					    pattern = p;
					}
					else
					    k |= 0;
					/* Fall through... */
				default:
					if (string_ch ==
						 (lower_bound = pattern_ch))
					{
						k++;
					}
				}
			}
			return 0;
		case L'*':
			return csh_match_star(string, _MBADV(pattern));
		case L'\0':
			return ((string_ch != L'\0') ? 0 : 1);
		case L'?':
			if (string_ch == L'\0')
			{
				return 0;
			}
			break;
		default:
			if (pattern_ch != string_ch)
			{
				return 0;
			}
			break;
		}
	}
	/* NOTREACHED */
}

/*
 * csh_match_star(string, pattern) This matches the '*' portion of a pattern.
 */
static int
csh_match_star(const char *string, const char *pattern)
{
	wchar_t         pattern_ch;
	wchar_t         string_ch;

	mbtowc(&pattern_ch, pattern, MB_CUR_MAX);
	switch (pattern_ch)
	{
	case L'\0':
		return 1;
	case L'[':
	case L'?':
	case L'*':
		while (*string)
		{
			if(gmatch(string, pattern)) {
			    _MBADV(string);
			    return 1;
			}
			else
			    _MBADV(string);
		}
		break;
	default:
		mbtowc(&pattern_ch, pattern, MB_CUR_MAX);
		_MBADV(pattern);
		while (*string)
		{
			mbtowc(&string_ch, string, MB_CUR_MAX);
			_MBADV(string);
			if(string_ch == pattern_ch && gmatch(string, pattern))
			{
				return 1;
			}
		}
		break;
	}
	return 0;
}

static char *
max_buf(size_t size, type_info_t *info)
{
	if(!info->mb)
	{
		info->mb_size += MB_SIZE;
		info->mb = (char *)calloc(info->mb_size, 1);
	}
	if(!size)
	{
		return(info->mb);
	}
	if(size > info->mb_size)
	{
		info->mb = (char *)realloc(info->mb, info->mb_size);
		info->mb_size = size;
	}
	return(info->mb);
}

static type_info_t *
set_vals(const char		*fn,
	const char		*buf,
	const int		bs,
	const struct stat	*fs,
	const char		*ln,
	const struct stat	*ls,
	const char		*on)
{
	char		*tmp;
	type_info_t	*linfo = (type_info_t *)calloc(1, sizeof(type_info_t));

	linfo->buff_size = -1;
	linfo->file_fd = -1;
	linfo->error = 0;
	linfo->mmap_size_to_free = -1;
	linfo->size_to_free = -1;
	linfo->set_datatype = 0;
	linfo->ot = 0;
	linfo->mb = 0;
	linfo->mb_size = 0;
	linfo->orig_attr = 0;

	if(fn)
	{
		linfo->file_path = strdup(fn);
		linfo->name = strrchr(linfo->file_path, '/');
		if(linfo->name)
		{
			linfo->name++;
		}
		else
		{
			char	path[MAXPATHLEN];
			char	*tmp;

			getcwd(path, MAXPATHLEN);
			strcat(path, "/");
			strcat(path, linfo->file_path);

			tmp = linfo->file_path;
			linfo->file_path = strdup(path);
			linfo->name = strstr(linfo->file_path, tmp);
			free(tmp);
		}
	}
	else if(buf)
	{
		if(!fs)
		{
			struct	stat	*buf;
			buf = malloc(sizeof(struct stat));
			memset(buf, '\0', sizeof(struct stat));
			buf->st_mode = 	S_IFREG | S_IROTH | S_IRGRP | 
					S_IRUSR | S_IWOTH | S_IWGRP | 
					S_IWUSR;
			linfo->file_stat = (const struct stat *)buf;
		}
	}
			
	if(buf)
	{
		linfo->buffer = buf;
	}
	/*
	 * 04/30/96 - What should this if() REALLY be?  Chances are
	 * pretty good that bs will either not equal 0 or not equal -1!
	 */
	if(bs != 0 || bs != -1)
	{
		linfo->buff_size = bs;
	}
	if(fs)
	{
		linfo->file_stat = (struct stat *)malloc(sizeof(struct stat));
		memcpy((void *)linfo->file_stat,
			(struct stat *)fs,
			sizeof(struct stat));
	}
	if(ln)
	{
		if(*ln == '/')
		{
			linfo->link_path = (char *)strdup(ln);
			linfo->link_name = strrchr(ln, '/');
			if(linfo->link_name)
			{
				linfo->link_name++;
			}
		}
		else
		{
			linfo->link_name = (char *)ln;
		}
	}
	if(ls)
	{
		linfo->file_lstat = (struct stat *)malloc(sizeof(struct stat));
		memcpy((void*)linfo->file_lstat,
			(struct stat *)ls,
			sizeof(struct stat));
	}
	if(on)
	{
		linfo->opt_name = strdup(on);
	}

	return(linfo);
}

static char *
cleanup(char *ot, type_info_t *info)
{
	if(ot)
	{
		ot = strdup(ot);
	}
	else if(info->ot)
	{
		ot = strdup(info->ot);
	}
	else
	{
		ot = strdup(DtDTS_DT_UNKNOWN);
	}
	if(info->ot)
	{
		free(info->ot);
	}

	if(info->file_stat)
	{
		free((void *)info->file_stat);
	}
	if(info->file_lstat)
	{
		free((void *)info->file_lstat);
	}

	info->file_stat = 0;
	info->file_lstat = 0;
	if(info->buffer)
	{
		if(info->mmap_size_to_free != -1)
		{
			if(munmap((caddr_t)info->buffer, 
				info->mmap_size_to_free) != 0)
			{
			    _DtSimpleError(
					DtProgName, DtError, NULL,
					"munmap", NULL);
			}
			info->buffer = 0;
			info->mmap_size_to_free = -1;
		}
		else if(info->size_to_free != -1)
		{
			free((void *)info->buffer);
			info->buffer = 0;
			info->size_to_free = -1;
		}
	}
	if(info->file_fd != -1)
	{
		close(info->file_fd);
		info->file_fd = -1;
	}
	info->buffer = 0;
	if(info->file_path)
	{
		free((void *)info->file_path);
		info->file_path = 0;
		info->name = 0;
	}
	if(info->name)
	{
		free((void *)info->name);
		info->name = 0;
	}
	info->file_path = 0;
	info->name = 0;
	info->file_fd = -1;
	info->buff_size = -1;
	info->error = 0;
	if(info->link_path != (char *)0 && info->link_path != (char *)-1)
	{
		free((void *)info->link_path);
	}

	if(info->opt_name)
	{
		free((void *)info->opt_name);
	}
	if(info->mb)
	{
		free(info->mb);
	}
	if(info->orig_attr)
	{
		_DtDtsMMSafeFree(info->orig_attr);
	}

	free(info);
	info = 0;
	return(ot);
}

static	const char*
get_opt_name(type_info_t *info)
{
	return(info->opt_name);
}

static	const char*
get_name(type_info_t *info)
{
	return(info->name);
}

static	const char*
get_file_path(type_info_t *info)
{
	if(info->file_path)
	{
		return(info->file_path);
	}
	else
	{
		return(0);
	}
}

static int
get_fd(type_info_t *info)
{
	if(!info->error && info->file_fd == -1)
	{
		if(info->file_path == 0)
		{
			return(-1);
		}
		if((info->file_fd = open(info->file_path, O_RDONLY|O_NOCTTY, 0)) == -1)
		{
			info->error = errno;
			return(-1);
		}
	}
	return(info->file_fd);
}

static const struct stat *
get_lstat(type_info_t *info)
{
	struct	stat	buf;

	if(!info->file_lstat)
	{
		if(NULL == info->file_path)
		{
			info->error = ENOENT;
			info->file_lstat = 0;
			return(0);
		}
		else if(lstat(info->file_path, &buf) == -1)
		{
			info->error = errno;
			info->file_lstat = 0;
			return(0);
		}
		else
		{
			info->file_lstat = (struct stat *)malloc(sizeof(struct stat));
			memcpy((char *)info->file_lstat, &buf, sizeof(buf));
		}
	}
	return(info->file_lstat);
}

static const struct stat *
get_stat(type_info_t *info)
{
	struct	stat	buf;
	int		fd;

	if (!info->file_stat)
	{
		if(NULL == get_file_path(info))
		{
			info->error = ENOENT;
			info->file_stat = 0;
		}
		else if(stat(get_file_path(info), &buf) == -1)
		{
			if(errno == ENOENT)
			{
				if(get_lstat(info))
				{
					info->ot = strdup(DtDTS_DT_BROKEN_LINK);
				}
			}
			info->error = errno;
			info->file_stat = 0;
		}
		else
		{
			info->file_stat = (struct stat *)malloc(sizeof(struct stat));
			memcpy((char *)info->file_stat, &buf, sizeof(buf));
		}
	}

	return(info->file_stat);
}

static int
islink(type_info_t *info)
{
	if(info->file_lstat || info->link_path || info->link_name)
	{
		return(1);
	}
	return(0);
}

void
get_link_info(type_info_t *info)
{
	char		buff[MAXPATHLEN];
	const	char	*name = 0;
	int		n;

	if(info->link_path == 0)
	{
		if((name = get_file_path(info)) == 0)
		{
			info->link_path = (char *)-1;
			info->link_name = (char *)-1;
			return;
		}
		else
		{
			name = strdup(name);
		}
		while((n = readlink(name, buff, MAXPATHLEN)) > 0)
		{
	
			buff[n] = 0;
			free((void *)name);
			name = strdup(buff);
		}
		if(errno == EINVAL || errno == ENOENT)
		{
			info->link_path = name;
			info->link_name = strrchr(info->link_path, '/');
			if(info->link_name == 0)
			{
				info->link_name = info->link_path;
			}
			else
			{
				info->link_name++;
			}
		}
		else
		{
			info->error = errno;
			info->link_path = (char *)-1;
			info->link_name = (char *)-1;
		}
	}
	return;
}

static const char *
get_link_path(type_info_t *info)
{
	get_link_info(info);
	return(info->link_path);
}

static const char *
get_link_name(type_info_t *info)
{
	get_link_info(info);
	return(info->link_name);
}

static const char *
get_buff(type_info_t *info)
{
	const struct	stat	*buf;

	if(!info->buffer && info->buffer != (char *)-1)
	{
		buf = get_stat(info);
		if(buf && buf->st_size)
		{
			if ((info->file_fd == -1) && (get_fd(info) == -1))
			   return 0;
			
			info->mmap_size_to_free = buf->st_size;
			if((info->buffer = mmap(NULL,
				buf->st_size,
				PROT_READ, MAP_PRIVATE,
				info->file_fd, 0)) == (char *)-1)
			{
				info->mmap_size_to_free = -1;
				info->size_to_free = buf->st_size+1;
				info->buffer = malloc(info->size_to_free);
				if(read(info->file_fd, (void *)info->buffer,
						info->size_to_free) == -1)
				{
					return(0);
				}
			}
			info->buff_size = buf->st_size;
		}
		else
		{
			return((char *)0);
		}

	}
	return(info->buffer);
}

int
get_buff_size(type_info_t *info)
{
	return(info->buff_size);
}
static DtDtsMMDatabase *
get_dc_db()
{
	DtDtsMMDatabase *dc_db = 0;

	if(!dc_db)
	{
#ifdef	DEBUG
fprintf(stderr, "Load DataCriteria\n");
#endif
		dc_db = _DtDtsMMGet(DtDTS_DC_NAME);
		if(!dc_db)
		{
			_DtSimpleError(
				DtProgName, DtError, NULL,
				"No DataBase loaded\n", NULL);
			return(NULL);
		}
	}
#ifdef NO_MMAP
	if(dc_db->compare != cde_dc_compare)
	{
		int	i;
		for(i = 0; i < dc_db->recordCount; i++)
		{
			if(dc_db->recordList[i]->compare !=cde_dc_field_compare)
			{
				_DtDtsMMFieldSort(dc_db->recordList[i], 
					cde_dc_field_compare);
			}
		}
		_DtDtsMMRecordSort(dc_db, cde_dc_compare);
#ifdef DEBUG
_DtDtsMMPrint(stdout);
#endif
	}
#endif
	return(dc_db);
}

static DtDtsMMDatabase *
get_da_db()
{
	DtDtsMMDatabase *da_db = 0;
	if(!da_db)
	{
#ifdef	DEBUG
fprintf(stderr, "Load DataAttributes\n");
#endif
		da_db = _DtDtsMMGet(DtDTS_DA_NAME);
		if(!da_db)
		{
			_DtSimpleError(
				DtProgName, DtError, NULL,
				"No DataBase loaded\n", NULL);
			return(NULL);
		}
	}

#ifdef NO_MMAP
	if(da_db->compare != _DtDtsMMCompareRecordNames)
	{
		int	i;
		for(i = 0; i < da_db->recordCount; i++)
		{
			if(da_db->recordList[i]->compare !=
					_DtDtsMMCompareFieldNames)
			{
				_DtDtsMMFieldSort(da_db->recordList[i], 
					_DtDtsMMCompareFieldNames);
			}
		}

		_DtDtsMMRecordSort(da_db, _DtDtsMMCompareRecordNames);
	}
#endif
	return(da_db);
}

#define	DTSATTRVAL(attr_name) if(_DtDtsMMStringToBoson(attr_name) == fld_ptr->fieldName)

static int
type_content(char *attr, type_info_t *info)
{
	int	match = 0;
	char	*c;
	int	end = 0, s;
	const	char	*buff = 0;
	const	struct	stat	*buf;
	char		*p;
	_Xstrtokparams	strtok_buf;

	if(buf = get_stat(info))
	{
		if((buf->st_mode&S_IFMT) == S_IFDIR && 
			(c = strstr(attr, "filename")))
		{
			char *file;
			const char *path;

			c = _XStrtok(c, " \t\n", strtok_buf);
			c = _XStrtok(NULL, " \t\n", strtok_buf);
			path = get_file_path(info);
			file = calloc(1, strlen(path)+strlen(c)+2);
			sprintf(file, "%s/%s", path, c);
			if(access(file, F_OK))
			{
				free(file);
				return(0);
			}
			else
			{
				free(file);
				return(1);
			}
		}
		else if((buf->st_mode&S_IFMT) != S_IFREG)
		{
			return(0);
		}
	}
	buff = get_buff(info);
	if((long)buff == 0 || (long)buff == -1)
	{
		return(0);
	}

	attr = strdup(attr);
	c = _XStrtok(attr, " \t\n", strtok_buf);
	if(!isdigit(*c) && *c != '-')
	{
		/* find where c is in buff */
		printf("start as string not ready yet\n");
		match = 0;
	}
	else
	{
		if(*c == '-')
		{
			end = 1;
			c++;
		}

		s = atoi(c);
		if(end) 
		{
			if (buf)
			{
				if (s > buf->st_size)
				{
					free(attr);
					return(match);
				}
				s = buf->st_size - s;
			}
			else
			{
				if (!info->buff_size || (s > info->buff_size))
				{
					free(attr);
					return(match);
				}
				s = info->buff_size - s;
			}
		}
		c = _XStrtok(NULL, " \t\n", strtok_buf);
		switch(c[1])
		{
		case	't': /* string */
		{
			int	cl;

			c = _XStrtok(NULL, "\n", strtok_buf);
			cl = strlen(c);
			if((info->buff_size-s) >= cl && memcmp(&buff[s], c, cl) == 0)
			{
				match = 1;
			}
			else
			{
				match = 0;
			}
			break;
		}
		case	'y': /* byte */
		{
			unsigned char	lv;
			unsigned char	num;
			int	i = 0;

			match = 0;
			while(c = _XStrtok(NULL, "\t \n", strtok_buf))
			{
				if(s+i*sizeof(lv)+sizeof(lv) > get_buff_size(info))
				{
					match = 0;
					break;
				}
				memcpy(&lv, &buff[s+i*sizeof(lv)], sizeof(lv));
				num = (unsigned char)strtol(c, &p, 0);
				if (num != lv || c == p)
				{
					match = 0;
					break;
				}
				else
				{
					match = 1;
				}
				i++;
			}
			break;
		}
		case	'h': /* short */
		{
			unsigned short	lv;
			unsigned short	num;
			int		i = 0;
			const unsigned char *bufPtr;

			match = 0;
			for(i = 0;(c = _XStrtok(NULL, "\t \n", strtok_buf)); i++)
			{
				if(s+i*sizeof(lv)+sizeof(lv) > get_buff_size(info))
				{
					match = 0;
					break;
				}
				bufPtr = (unsigned char *)
				  &buff[s + (i * sizeof(lv))];
				lv = (((unsigned short)*bufPtr) << 8) |
				     ((unsigned short)*(bufPtr + 1));
				num = (unsigned short)strtol(c, &p, 0);
				if (num != lv || c == p)
				{
					match = 0;
					break;
				}
				else
				{
					match = 1;
				}
			}
			break;
		}
		case	'o': /* long */
		{
			/* Not true long - really 4 bytes. */
			unsigned int	lv;
			unsigned int	num;
			int		i = 0;
			const unsigned char *bufPtr;

			match = 0;
			for(i = 0;(c = _XStrtok(NULL, "\t \n", strtok_buf)); i++)
			{
				if(s+i*sizeof(lv)+sizeof(lv) > get_buff_size(info))
				{
					match = 0;
					break;
				}
				bufPtr = (unsigned char *)
				  &buff[s + (i * sizeof(lv))];
				lv = (((unsigned int)*bufPtr) << 24) |
				     (((unsigned int)*(bufPtr + 1)) << 16) |
				     (((unsigned int)*(bufPtr + 2)) << 8) |
				     ((unsigned int)*(bufPtr + 3));
				num = (unsigned int)strtol(c, &p, 0);
				if (num != lv || c == p)
				{
					match = 0;
					break;
				}
				else
				{
					match = 1;
				}
			}
			break;
		}
		default:
			match = 0;
		}
		
	}
	free(attr);
	return(match);
}

static char *
_DtDtsGetDataType(const char *file)
{
	int		fd;
	char		*name;
	struct stat	file_stat;
	u_char		*buff = 0;
	int		start;
	char		*dt = 0;
	int		end;

	name = calloc(1, MAXPATHLEN+1);
	sprintf(name, "%s/%s", file, DtDTS_DT_DIR);
	if((fd = open(name, O_RDONLY, 0644)) != -1)
	{
		if(fstat(fd, &file_stat) == 0)
		{
			buff = (u_char *)calloc((size_t)1, file_stat.st_size+1);
			read(fd, buff, file_stat.st_size);
		}
		dt = strstr((char *)buff, DtDTS_DATA_ATTRIBUTES_NAME);
		if(dt != NULL)
		{
			start = dt-(char*)buff;
			while(!isspace(buff[start])) start++;
			while(isspace(buff[start]))start++;
			end = start;
			while(!isspace(buff[end])) end++;
			buff[end] = '\0';
			dt = strdup((char *)&buff[start]);
		}
		close(fd);
	}
	if(buff)free(buff);
	if(name)free(name);
	return(dt);
}

static int
type_mode(char *attr, type_info_t *info)
{
	int		match = 0;
	const	struct	stat	*buf = get_stat(info);
	const	struct	stat	*lbuf;

	if (!buf || !*attr)
	{
		return(0);
	}

	do
	{
		match = 0;
		switch(*attr)
		{
		case	'f':
			match = (buf->st_mode&S_IFMT) == S_IFREG;
			break;
		case	'c':
			match = (buf->st_mode&S_IFMT) == S_IFCHR;
			break;
		case	'b':
			match = (buf->st_mode&S_IFMT) == S_IFBLK;
			break;
		case	'd':
			if((buf->st_mode&S_IFMT) == S_IFDIR)
			{
				int	n;
				int	fd;

				match = 1;
				if(!info->set_datatype)
				{
					info->set_datatype = 1;
					info->ot = 
					      _DtDtsGetDataType(get_file_path(info));
					if(info->ot) return(match);
				}
			}
			else
			{
				match = 0;
			}
	
			break;
		case	'l':
			if(islink(info))
			{
				match = 1;
			}
			else if(lbuf = get_lstat(info))
			{
				match = (lbuf->st_mode&S_IFMT) == S_IFLNK;
			}
			else
			{
				match = 0;
			}
			break;
		case	'r':
			if(buf->st_mode&S_IROTH || \
			   buf->st_mode&S_IRGRP || \
			   buf->st_mode&S_IRUSR )
			{
				match = 1;
			}
			else
			{
				match = 0;
			}
			break;
		case	'w':
			if(buf->st_mode&S_IWOTH || \
			   buf->st_mode&S_IWGRP || \
			   buf->st_mode&S_IWUSR )
			{
				match = 1;
			}
			else
			{
				match = 0;
			}
			break;
		case	'x':
			if(buf->st_mode&S_IXOTH || \
			   buf->st_mode&S_IXGRP || \
			   buf->st_mode&S_IXUSR ) 
			{
				match = 1;
			}
			else
			{
				match = 0;
			}
			break;
		case	'g':
			match = buf->st_mode&S_ISGID;
			break;
		case	'u':
			match = buf->st_mode&S_ISUID;
			break;
		}
		attr++;
	} while(*attr && match);
	return(match);
}

static int
type_name(const char *name, char *attr)
{
	int	match = 0;

	if(name && name != (char *)-1)
	{
#ifdef USE_FNMATCH
		match = !fnmatch(attr, name, 0);
#else
		match =  gmatch(name, attr);
#endif
	}
	return(match);
}

static int
type_path(const char *path, char *attr)
{
	char	*c;
	int	match = 0;

	if(path && (int)path != -1)
	{
#ifdef USE_FNMATCH
		match = !fnmatch(attr, path, 0);
#else
		match = gmatch(path, attr);
#endif
	}
	return(match);
}

static char *
next_sep(char *str, char *sep)
{
	char	*c;
	char	*prev;

	c = str;
	while(*c)
	{
		c = _dt_strpbrk(c, sep);
		if(!c)
		{
			return(c);
		}
		prev = c-1;
		if((c == str) || (_is_previous_single(str, c) && (*prev != '\\')))
		{
			return(c);
		}
		_MBADV(c);
	}
	return((char *)0);
}

char	*
DtDtsDataToDataType(const char *fp,
	const void		*buf,
	const int		bs,
	const struct stat	*fs,
	const char		*ln,
	const struct stat	*ls,
	const char		*on)
{
	DtDtsMMDatabase	*db;
	DtDtsMMRecord	*rec_ptr = 0;
	DtDtsMMRecord	*rec_ptr_list;
	DtDtsMMField	*fld_ptr;
	DtDtsMMField	*fld_ptr_list;
	type_info_t	*info = 0;
	int	i;
	int	j;
	int	rec_m = 0;
	int	fld_m = 0;
	int	atr_m = 0;
	int	p_atr_m = 1;
	int	c_atr_m = 1;
	char	*ot = NULL;
	char	*file;
	char	*attr;
	char	op;

	_DtSvcAppLockDefault();
	_DtSvcProcessLock();
	db = get_dc_db();
	if(!db)
	{
		_DtSvcProcessUnlock();
		_DtSvcAppUnlockDefault();
		return(strdup("UNKNOWN"));
	}

	info = set_vals(fp, buf, bs, fs, ln, ls, on);
	if(!dtdts_path_pattern)
	{
		dtdts_path_pattern = _DtDtsMMStringToBoson(DtDTS_PATH_PATTERN);
		dtdts_name_pattern = _DtDtsMMStringToBoson(DtDTS_NAME_PATTERN);
		dtdts_mode = _DtDtsMMStringToBoson(DtDTS_MODE);
		dtdts_link_name = _DtDtsMMStringToBoson(DtDTS_LINK_NAME);
		dtdts_link_path = _DtDtsMMStringToBoson(DtDTS_LINK_PATH);
		dtdts_content = _DtDtsMMStringToBoson(DtDTS_CONTENT);
		dtdts_data_attributes_name = _DtDtsMMStringToBoson(DtDTS_DATA_ATTRIBUTES_NAME);

		dtdts_da_is_action = _DtDtsMMStringToBoson(DtDTS_DA_IS_ACTION);
		dtdts_da_icon = _DtDtsMMStringToBoson(DtDTS_DA_ICON);
		dtdts_da_description = _DtDtsMMStringToBoson(DtDTS_DA_DESCRIPTION);
		dtdts_da_label = _DtDtsMMStringToBoson(DtDTS_DA_LABEL);
	}

	for(i = 0; !rec_m && (rec_ptr = name_list(info, db, rec_ptr)); i++)
	{
		fld_ptr_list = _DtDtsMMGetPtr(rec_ptr->fieldList);
		fld_m = 1;
		for(j=0; fld_m && j < rec_ptr->fieldCount; j++)
		{
			char	sep = '&';
			DtDtsMMField	*fld_ptr = &fld_ptr_list[j];

			p_atr_m = 1;
			c_atr_m = 1;
			info->orig_attr = _DtDtsMMExpandValue(
					_DtDtsMMBosonToString(fld_ptr->fieldValue));
			attr = info->orig_attr;
			do
			{
				int	neg = 0;
				char	*new_sep;
				int	n;
				char	*c;

				atr_m = 1;
				while(*attr == '!')
				{
					neg = !neg;
					attr++;
				}
				new_sep = next_sep(attr, "&|\0");
				if(new_sep == 0)
				{
					new_sep = attr + strlen(attr);
				}
				n = new_sep-attr+1;
				c = max_buf((size_t)n, info);

				strncpy(c, attr, new_sep-attr);
				c[new_sep-attr] = '\0';
				attr = new_sep;
				new_sep = c;
/*
				while(new_sep = (char *)_dt_strpbrk(new_sep, "&|"))
				{
					new_sep--;
					strcpy(new_sep, &new_sep[1]);
					new_sep++;
				}
*/

				if(fld_ptr->fieldName == dtdts_path_pattern)
				{
					atr_m = type_path(
							get_file_path(info),
							max_buf((size_t)0, info));
				}
				else if(fld_ptr->fieldName == dtdts_name_pattern)
				{
					if(get_file_path(info))
					{
						atr_m = type_name(
							get_name(info),
							max_buf((size_t)0, info));
					}
					else
					{
						atr_m = type_name(
							get_opt_name(info),
							max_buf((size_t)0, info));
					}
				}
				else if(fld_ptr->fieldName == dtdts_mode)
				{
					if(get_file_path(info))
					{
						atr_m = type_mode(
							max_buf((size_t)0, info), info);
					}
					else
					{
						atr_m = !neg;
					}
				}
				else if(fld_ptr->fieldName == dtdts_link_name)
				{
					atr_m = type_name(
							get_link_name(info),
							max_buf((size_t)0, info));

				}
				else if(fld_ptr->fieldName == dtdts_link_path)
				{
					atr_m = type_path(
							get_link_path(info),
							max_buf((size_t)0, info));
				}
				else if(fld_ptr->fieldName == dtdts_content)
				{
					atr_m = type_content(
							max_buf((size_t)0, info), info);
				}
				else if(fld_ptr->fieldName == dtdts_data_attributes_name)
				{
					info->ot = strdup(max_buf((size_t)0, info));
				}

				if(info->error == ELOOP)
				{
				        _DtSvcProcessUnlock();
					_DtSvcAppUnlockDefault();
					return(
					   cleanup(DtDTS_DT_RECURSIVE_LINK, info)
						);
				}
				if(info->ot)
				{
				        _DtSvcProcessUnlock();
					_DtSvcAppUnlockDefault();
					return(cleanup(0, info));
				}

				atr_m=(neg)?!atr_m:atr_m;

				switch(sep)
				{
				case	'&':
					p_atr_m = atr_m && p_atr_m;
					break;
				case	'|':
					p_atr_m = atr_m || p_atr_m;
					break;
				}
				if(attr && *attr)
				{
					sep = *attr;
					attr++;
					switch(sep)
					{
					case	'&':
						c_atr_m = p_atr_m?1:0;
						break;
					case	'|':
						c_atr_m = p_atr_m?0:1;
						break;
					}
				}
				else
				{
					c_atr_m = 0;
				}
			} while ( c_atr_m );
			if(info->orig_attr)
			{
				_DtDtsMMSafeFree(info->orig_attr);
				info->orig_attr = 0;
			}

			if(!p_atr_m)
			{
				fld_m = 0;
			}
		}
		if(fld_m)
		{
			rec_m = 1;
		}
	}
	_DtSvcProcessUnlock();
	_DtSvcAppUnlockDefault();
	return(cleanup(0, info));
}

char	*
DtDtsFileToDataType(const char *filepath)
{
	char *dt = DtDtsDataToDataType(filepath, 0, -1, 0, 0, 0, 0);
	return(dt);
}

char	*
DtDtsBufferToDataType(const void *buffer, const int size, const char *name)
{
	char *dt = DtDtsDataToDataType(0, buffer, size, 0, 0, 0, name);
	return(dt);
}

static char *
expand_keyword(const char *attr_in, const char *in_pathname)
{
	char	*buf;
	char	*netPath;
	char	*c;
	char	*tmp;
	int	i;
	int	n;
	char	*p;
	char	*attr = (char *)attr_in;


	if(!attr)
	{
		return(attr);
	}
	if(in_pathname)
	{
		n = strlen(attr)+1;
		buf = (char *) calloc(1, n);
		netPath = strdup(in_pathname);

		for(c = attr, i= 0; *c && i < n; c++)
		{
			if ( *c != '%' )
			{
				buf[i++] = *c;
				buf[i]=0;   /* ensure null string termination */
				continue;
			}
			/* check for keyword matches */
			if ( !strncmp(c,"%file%",6) )
			{
				n += strlen(netPath) - 6;
				buf = (char *)realloc(buf, n);
				strcpy((buf+i), netPath);
				i += strlen(netPath);
				c += 5;
				continue;
			}
			else if ( !strncmp(c,"%dir%",5) )
			{
				tmp = strrchr(netPath, '/');
				*tmp = '\0';
				n += strlen(netPath) - 5;
				buf = (char *)realloc(buf, n);
				strcpy((buf+i),netPath);
				i += strlen(netPath);
				*tmp = '/';
				c += 4;
			}
			else if ( !strncmp(c,"%name%",6) )
			{
				tmp = strrchr(netPath, '/');
				tmp ++;
				n += strlen(tmp) - 6;
				buf = (char *)realloc(buf, n);
				strcpy((buf+i),tmp);
				i +=strlen(tmp);
				c += 5;
			}
			else if ( !strncmp(c,"%suffix%",8) )
			{
				if ((p = strrchr(netPath,'.')) != NULL)
				{
					p++;
					n += strlen(p) - 8;
					buf = (char *)realloc(buf, n);
					strcpy((buf+i),p);
					i +=strlen(p);
				}
				c += 7;
			}
			else if ( !strncmp(c,"%base%",6) )
			{
				tmp = strrchr(netPath, '/');
				tmp ++;
				if ((p = strrchr(tmp,'.')) != NULL)
				{
					n += p-tmp - 6;
					buf = (char *)realloc(buf, n);
					strncpy((buf+i),tmp,p-tmp);
					buf[i+p-tmp] = '\0';
					i += p-tmp;
				}
				c += 5;
			}
			else
			{ /* no match -- just copy the character */
				buf[i++] = *c;
				buf[i]=0;   /* ensure null string termination */
				continue;
			}
		}
		if(netPath)free(netPath);
		return(buf);
	}
	else
	{
		return(strdup(attr));
	}
}

char *
append(char *old, char *add)
{
	char	*new;

	if(old)
	{
		new = realloc(old, strlen(old)+strlen(add)+1);
	}
	else
	{
		new = malloc(strlen(add)+1);
		*new = '\0';
	}

	strcat(new, add);

	return(new);
}

char *
expand_shell(const char *attr)
{
	char	*srch;
	char	*nattr;
	char	*start, *end;
	int	size;
	FILE	*fd;
	char	buff[200];
	char	*results = 0;

	if(!attr)
	{
		return(NULL);
	}
	nattr = strdup(attr);
	srch = nattr;
	while((start = DtStrchr(srch, '`')) != NULL)
	{
		if((srch != start) && _is_previous_single(srch, start) && 
		   (*(start-1) == '`'))
		{
			srch = start + 1;
			continue;
		}
		if((end = DtStrchr(&start[1], '`')) == NULL)
		{
			srch = start + 1;
			continue;
		}
		*start = '\0';
		*end = '\0';
		results = append(results, srch);


		if((fd = popen(&start[1], "r")) == NULL)
		{
			_DtSimpleError(
				DtProgName, DtError, NULL,
				(char*) &start[1], NULL);
			if(nattr)free(nattr);
			if(results)free(results);
			return(NULL);
		}

		memset(buff, '\0', sizeof(buff));
		while(size = fread(buff, 1, sizeof(buff)-1, fd))
		{
			buff[sizeof(buff)-1] = '\0';
			results = append(results, buff);
			memset(buff, '\0', sizeof(buff));
		}
		srch = end+1;
	}
	results = append(results, srch);
	if(nattr) free(nattr);
	return(results);
}

static char *
expand_value(DtShmBoson attr, char *in_pathname)
{
	char *tmp;
	char *exp;
	char *shell_exp;

	if(attr && attr != -1)
	{
		tmp = _DtDtsMMExpandValue(_DtDtsMMBosonToString(attr));
		exp = expand_keyword(tmp, in_pathname);
		shell_exp = expand_shell(exp);

		_DtDtsMMSafeFree(tmp);
		if(shell_exp)
		{
			free(exp);
			exp = expand_keyword(shell_exp, in_pathname);
			free(shell_exp);
		}
	}
	else
	{
		exp = NULL;
	}

	return(exp);
}

char	*
DtDtsDataTypeToAttributeValue(const char *obj_type, const char *attr, const char *filename)
{
	DtDtsMMDatabase	*db;
	DtDtsMMRecord	*entry;
	char		*value = 0;

	_DtSvcAppLockDefault();
	_DtSvcProcessLock();
	db = get_da_db();
	if(db && obj_type)
	{
		entry = _DtDtsMMGetRecordByName(db, (char *)obj_type);

		if(entry)
		{
			char	*name = 0;
			DtDtsMMField *fld = _DtDtsMMGetField(entry,(char *)attr);


			if ( fld )
			{
				value = expand_value(fld->fieldValue,
					(char *)filename);
			}
		}
	}
	_DtSvcProcessUnlock();
	_DtSvcAppUnlockDefault();
	return(value);
}

DtDtsAttribute	**
DtDtsDataTypeToAttributeList(const char *obj_type, const char *filename)
{
	DtDtsMMDatabase	*db;
	DtDtsMMRecord	*entry;
	DtDtsMMField	*fld_ptr_list;
	DtDtsMMField	*fld_ptr;
	DtDtsAttribute	**list = NULL;
	int		i;
	int		action_flag = 0;
	int		sort_flag = 0;
	int		found_flag = 0;
	DtDtsMMField	*fld;

	_DtSvcAppLockDefault();
	_DtSvcProcessLock();    
	db = get_da_db();
	entry = _DtDtsMMGetRecordByName(db, (char *)obj_type);

	if(!db || !obj_type || !entry)
	{
		_DtSvcProcessUnlock();
		_DtSvcAppUnlockDefault();
		return (list);
	}

	list = (DtDtsAttribute	**)calloc(entry->fieldCount+1, 
						sizeof(DtDtsAttribute *));
	fld_ptr_list = _DtDtsMMGetPtr(entry->fieldList);
	for(i = 0; i < entry->fieldCount; i++)
	{
		char	*tmp;
		fld_ptr = &fld_ptr_list[i];

		list[i] = (DtDtsAttribute *)malloc(sizeof(DtDtsAttribute));
		list[i]->name = 
			expand_value(fld_ptr->fieldName,
					 (char *)filename);

		list[i]->value = 
			expand_value(fld_ptr->fieldValue, (char *)filename);
	}
	list[i] = 0;
	_DtSvcProcessUnlock();
	_DtSvcAppUnlockDefault();
	return(list);
}

void
DtDtsFreeAttributeList(DtDtsAttribute **list)
{
	DtDtsAttribute **item = list;
	int i = 0;

	if(list)
	{
		while(list[i] && list[i]->name)
		{
			if(list[i]->name)free(list[i]->name);
			if(list[i]->value)free(list[i]->value);
			if(list[i]) free(list[i]);
			i++;
		}
		free(list);
	}
}

DtDtsAttribute	**
DtDtsFileToAttributeList(const char *filepath)
{
	char	*ot = DtDtsFileToDataType(filepath);
	DtDtsAttribute **al = DtDtsDataTypeToAttributeList(ot, filepath);

	DtDtsFreeDataType(ot);
	return(al);
}

DtDtsAttribute	**
DtDtsBufferToAttributeList(const void *buffer, const int size, const char *name)
{
	char	*ot = DtDtsBufferToDataType(buffer, size, name);
	DtDtsAttribute **al = DtDtsDataTypeToAttributeList(ot, NULL);

	DtDtsFreeDataType(ot);
	return(al);
}

char	*
DtDtsFileToAttributeValue(const char *filepath, const char *attr)
{
	char	*ot = DtDtsFileToDataType(filepath);
	char	*value = DtDtsDataTypeToAttributeValue(ot, attr, filepath);

	DtDtsFreeDataType(ot);
	return(value);
}

char	*
DtDtsBufferToAttributeValue(const void *buffer, const int size, const char *attr, const char *name)
{
	char	*ot = DtDtsBufferToDataType(buffer, size, name);
	char	*value = DtDtsDataTypeToAttributeValue(ot, attr, name);

	DtDtsFreeDataType(ot);
	return(value);
}

void
DtDtsFreeAttributeValue(char *value)
{
	if(value) free(value);
}

void
DtDtsFreeDataType(char *datatype)
{
	if(datatype) free(datatype);
}

int
DtDtsDataTypeIsAction(const char *datatype)
{
	char	*val;
	if(val = DtDtsDataTypeToAttributeValue(datatype, "IS_ACTION", NULL))
	{
		DtDtsFreeAttributeValue(val);
		return(1);
	}
	else
	{
		return(0);
	}
}

char **
DtDtsDataTypeNames(void)
{
	DtDtsMMDatabase	*db;
	DtDtsMMRecord	*rec_list;
	DtDtsMMRecord	*rec_ptr;
	int		i = 0, j=0;
	char		**names;

	_DtSvcAppLockDefault();
	_DtSvcProcessLock();    
	db = get_da_db();
	if(!db)
	{
		_DtSvcProcessUnlock();
		_DtSvcAppUnlockDefault();
		return(NULL);
	}

	names = (char **)malloc((db->recordCount+1)*sizeof(char *));

	rec_list = _DtDtsMMGetPtr(db->recordList);
	for(i = 0; i < db->recordCount; i++)
	{
		rec_ptr = &rec_list[i];
		names[j] = strdup((char *)_DtDtsMMBosonToString(rec_ptr->recordName));
		j++;
	}
	names[j] = 0;
	_DtSvcProcessUnlock();
	_DtSvcAppUnlockDefault();
	return(names);
}

void
DtDtsRelease(void)
{
}

char **
DtDtsFindAttribute(const char *name, const char *value)
{
	DtDtsMMDatabase	*ot;
	int		i;
	int		j = 0;
	char		**list;
	char		*v;
	DtDtsMMRecord	*rec_ptr;
	DtDtsMMRecord	*rec_ptr_list;

	_DtSvcAppLockDefault();
	_DtSvcProcessLock();    /* To avoid deadlock with DtDtsMMDatabase mutex
				   lock */
	ot = get_da_db();
	if(!ot || ot->recordCount == 0)
	{
		_DtSvcProcessUnlock(); 
		_DtSvcAppUnlockDefault();
		return(NULL);
	}

	list = (char **)calloc(ot->recordCount, sizeof(char *));
	rec_ptr_list = _DtDtsMMGetPtr(ot->recordList);
	for(i = 0; i < ot->recordCount; i++)
	{
		rec_ptr = &rec_ptr_list[i];
		v = _DtDtsMMExpandValue(_DtDtsMMGetFieldByName(rec_ptr, (char *)name));
		if(v && !strcmp(value, v))
		{
			list[j++] = strdup(_DtDtsMMBosonToString(rec_ptr->recordName));
		}
		_DtDtsMMSafeFree(v);
	}
	list[j] = 0;
	_DtSvcProcessUnlock();
	_DtSvcAppUnlockDefault();
	return(list);
}

void
DtDtsFreeDataTypeNames(char **list)
{
	int	i = 0;

	while(list[i])
	{
		free(list[i]);
		list[i++] = 0;
	}
	free(list);
}

#define	DIR_INFO	"DIR_INFO\n{\n\t%s\t%s\n}\n"
char *
DtDtsSetDataType(const char *filename, const char *datatype_in, const int overide)
{
	int		fsize;
	char		*file;
	int		fd;
	char		*dt;
	int		size;
	u_char		*buff;
	struct stat	file_stat;
	char		*datatype = 0;

	fsize = strlen(filename)+strlen(DtDTS_DT_DIR)+2;
	file = (char *)calloc(1,fsize);
	sprintf(file, "%s/%s", filename, DtDTS_DT_DIR);

	if((fd = open(file, O_EXCL|O_CREAT|O_RDWR, 0644)) != -1)
	{
		int	write_size;

		size = strlen(DIR_INFO);
		size += strlen(DtDTS_DATA_ATTRIBUTES_NAME);
		size += strlen(datatype_in);
		size-=3;
		buff = (u_char *)calloc((size_t)1, size);
		sprintf((char *)buff, DIR_INFO, DtDTS_DATA_ATTRIBUTES_NAME, datatype_in);

		write_size = write(fd, buff, size);
		if(write_size != size)
		{
		    _DtSimpleError(
				DtProgName, DtError, NULL,
				(char*) buff, NULL);
		}
		datatype = strdup(datatype_in);
		free(buff);
		close(fd);
	}
	else if(overide && (fd = open(file, O_RDWR, 0644)) != -1)
	{
		int	start;
		int	end;
		int	dtsize = strlen(datatype_in);
		int	write_size;

		if(fstat(fd, &file_stat) == 0)
		{
			buff = (u_char *)calloc((size_t)1, file_stat.st_size+1);
			read(fd, buff, file_stat.st_size);
		}
		dt = strstr((char *)buff, DtDTS_DATA_ATTRIBUTES_NAME);
		if(dt == NULL)
		{
			free(buff);
			size = strlen(DIR_INFO);
			size += strlen(DtDTS_DATA_ATTRIBUTES_NAME);
			size += dtsize;
			size-=3;
			buff = (u_char *)calloc((size_t)1, size);
			sprintf((char *)buff, DIR_INFO,
					DtDTS_DATA_ATTRIBUTES_NAME,
					datatype_in);
			write_size = write(fd, buff, size);
			if(write_size != size)
			{
			    _DtSimpleError(
					DtProgName, DtError, NULL,
					(char*) buff, NULL);
			}
			datatype = strdup(datatype_in);
		}
		else
		{
			int	off, total;

			start = dt-(char *)buff;
			while(!isspace(buff[start])) start++;
			while(isspace(buff[start]))start++;
			end = start;
			while(!isspace(buff[end])) end++;
			lseek(fd, start, SEEK_SET);
			if(write(fd, datatype_in, dtsize) != dtsize)
			{
			    _DtSimpleError(
					DtProgName, DtError, NULL,
					(char*) file, NULL);
			}
			off = write(fd, &buff[end], strlen((char *)&buff[end])+1);
			if(off != strlen((char *)&buff[end])+1)
			{
			    _DtSimpleError(
					DtProgName, DtError, NULL,
					(char*) file, NULL);
			}
			else
			{
				total = file_stat.st_size-(end-start)+dtsize;
				ftruncate(fd, total);
				datatype = strdup(datatype_in);
			}
		}
		free(buff);
		close(fd);
	}
	else if (access(file, R_OK) == 0)
	{
		datatype = _DtDtsGetDataType(filename);
	}
	else
	{
		datatype = 0;
	}
	free(file);
	return(datatype);
}

static int
srch(const void *a, const void *b)
{
	int results = ((struct list *)a)->boson - ((struct list *)b)->boson;

	if(results == 0)
	{
		results = ((struct list *)a)->rec - ((struct list *)b)->rec;
	}
	return(results);
}

int *
get_name_list(char *name, int *count)
{
	int	boson = name?_DtDtsMMStringToBoson((const char *)name):-1;
	int	*results;
	int	*list;
	int	size;

	*count = 0;
	if(boson == -1)
	{
		return((int *)-1);
	}
	results = (int *)_DtShmFindIntTabEntry(_DtDtsMMGetDCNameIndex(&size), boson);
	if (!results)
	{
		return((int *)-1);
	}
	if(*results < 0)
	{
		list = (int *)_DtDtsMMGetPtr(-(*results));
		*count = _DtDtsMMGetPtrSize(-(*results));
	}
	else
	{
		list = results;
		*count = 1;
	}
	return(list);
}

static DtDtsMMRecord *
name_list(type_info_t *linfo, DtDtsMMDatabase	*db, DtDtsMMRecord *rec_ptr)
{
	int			i;
	char			*src_str;
	char			*name;
	int			size;
	int			isnew;
	DtDtsMMRecord		*record_list;
	char			*suffix = 0;

	if(rec_ptr == 0)
	{
		/* if this is the first time */
		if(linfo->name)
		{
			/* get the name */
			name = linfo->name;
		}
		else
		{
			/* if not name use opt name */
			name = (char *)linfo->opt_name;
		}

		/* initialize name count */
		linfo->name_count = 0;
		if(!name)
		{
			/* a name could not be found so this must be a buffer */
			linfo->name_prev = (int *)
				_DtDtsMMGetBufferIndex(&linfo->name_count);
			linfo->name_type = 3;
		}
		else
		{
			if(name && *name)
			{
				/* now find suffix if any */
				suffix = strrchr(name, '.');
			}
		}

		if(!linfo->name_count && name)
		{
			/* find if name exist in our list */
			linfo->name_prev = get_name_list(name, 
					&linfo->name_count);
			linfo->name_type = 1;
		}
		if(!linfo->name_count && suffix)
		{
			/* find if suffix exist in our list */
			linfo->name_prev = get_name_list(suffix, 
					&linfo->name_count);
			linfo->name_type = 2;
		}
		if(!linfo->name_count)
		{
			/* neither exist so check ambugious ones */
			linfo->name_prev = (int *)_DtDtsMMGetNoNameIndex(&linfo->name_count);
			linfo->name_type = 3;
		}
	}
	else
	{
		/* we found one befor so . . . */
		if(linfo->name_count > 0)
		{
			/* if there are more on the list get the next one */
			linfo->name_prev++;
		}
		else
		{
			/* go to the next list */
			switch(linfo->name_type)
			{
			case	1:
				/* if we were in a name portion then we need to
					check for a suffix */
				suffix = strrchr(linfo->name, '.');
				if(suffix)
				{
					/* if found us it */
					linfo->name_prev = get_name_list(suffix, 
						&linfo->name_count);
					linfo->name_type = 2;
					break;
				}
				/* other wise */
			case	2:
				/* use the ambigous list */
				linfo->name_prev = 
					_DtDtsMMGetNoNameIndex(&linfo->name_count);
				linfo->name_type = 3;
				break;
			case	3:
				/* start now with the buffer list */
				linfo->name_prev = (int *)
					_DtDtsMMGetBufferIndex(&linfo->name_count);
				linfo->name_type = 3;

			default:
				return(0);
			}
		}
	}
	record_list = _DtDtsMMGetPtr(db->recordList);
	if(linfo->name_count == 0)
	{
		return(0);
	}
	else
	{
		linfo->name_count--;
		return(&record_list[*linfo->name_prev]);
	}
}
		
#ifdef NEED_STRCASECMP
/*
 * In case strcasecmp is not provided by the system here is one
 * which does the trick.
 */
static int
strcasecmp(register const char *s1,
	   register const char *s2)
{
    register int c1, c2;

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

Boolean
DtDtsIsTrue(const char *str)
{
	if (str && ((strcasecmp(str, "true") == 0)
	 || (strcasecmp(str, "yes") == 0)
	 || (strcasecmp(str, "on") == 0)
	 || (strcasecmp(str, "1") == 0)))
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

