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
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 *+SNOTICE
 *
 *	$XConsortium: DtsSort.c /main/6 1996/11/21 19:56:08 drk $
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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <Dt/DbReader.h>
#include "Dt/DtsDb.h"
#include "Dt/Dts.h"

#if !defined(__linux__)
extern	char	*strdup(const char *);
#endif

static char *
get_value(DtDtsDbRecord *ce_entry, char *value)
{
	int	i=0;
	XrmQuark	tmp = XrmStringToQuark (value);

	for(i = 0; i < ce_entry->fieldCount; i++)
	{
		if(ce_entry->fieldList[i]->fieldName == tmp)
		{
			return(ce_entry->fieldList[i]->fieldValue);
		}
	}
	return(NULL);
}

void
parts_of_pattern(char *c, int *spec, int *count, int *front)
{
	int	nested = 0;
	int	found = 0;
        int     len;

	(*count) = 0;
	(*spec) = 0;
	(*front) = 0;

	while(*c)
	{
                if((len = mblen(c, MB_CUR_MAX)) > 1) {
                    (*count) += len;
                    if(!found)
                        (*front) += len;
                    c += len;
                    continue;
                }
		switch(*c)
		{
		case	'*':
			if(!nested)
			{
				(*spec) += 100;
				found = 1;
			}
			break;
		case	'?':
			if(!nested)
			{
				(*spec) += 1;
				found = 1;
			}
			break;
		case	'[':
			(*spec) += 10;
			nested ++;
			found = 1;
			break;
		case	']':
			if(nested)
			{
				nested --;
			}
			break;
		case	'\\':
			if(!nested)
			{
                            if((len = mblen(c + 1, MB_CUR_MAX)) > 1) {
                                (*count) += len + 1;
                                c += len;
                            }
                            else {
                                (*count) += 2;
                                c++;
                            }
			}
			break;
		default:
			(*count)++;
			if(!found)
			{
				(*front)++;
			}
		}
		c++;
	}
}

int
check_pattern(char *value1, char *value2)
{
	int		spec1 = 0,count1 = 0, front1 = 0;
	int		spec2 = 0,count2 = 0, front2 = 0;

	if(value1 && value2)
	{
		parts_of_pattern(value1, &spec1, &count1, &front1);
		parts_of_pattern(value2, &spec2, &count2, &front2);
	}
	else if(!value1 && !value2)
	{
		return(0);
	}
	else if(value1)
	{
		return(-1);
	}
	else
	{
		return(1);
	}


	if(front1 != front2)
	{
		return(front2 - front1);
	}

	if(spec1 != spec2)
	{
		return(spec1 - spec2);
	}
	if(count1 != count2)
	{
		return(count2-count1);
	}

	return (0);
}

int
mode_count(char *c)
{
	int	count = 0;

	while(c && *c)
	{

		if(strchr("fcbdlrwx", *c))
		{
			count++;
		}
		c++;
	}
	return(count);
}

#ifdef DEBUG
static int
bool(int i)
{
	if(i > 0) return(1);
	if(i < 0) return(-1);
	else return(0);
}

#endif

static int check_content(char *val1, char *val2)
{
	char	*v1 = strdup(val1);
	char	*v2 = strdup(val2);
	char	*loc1;
	char	*loc2;
	char	*type1;
	char	*type2;
	char	*cnt1;
	char	*cnt2;
	int	ret = 0;
	_Xstrtokparams	strtok_buf;

	loc1 = _XStrtok(v1, "\t \n", strtok_buf);
	type1 = _XStrtok(NULL, "\t \n", strtok_buf);
	cnt1 = &type1[strlen(type1)+1];
	while(isspace((u_char)*cnt1))cnt1++;

	loc2 = _XStrtok(v2, "\t \n", strtok_buf);
	type2 = _XStrtok(NULL, "\t \n", strtok_buf);
	cnt2 = &type2[strlen(type2)+1];
	while(isspace((u_char)*cnt2))cnt2++;

	ret = strcmp(type1, type2);
	if(loc1 && loc2 && ret == 0)
	{
		int	l1 = atoi(loc1);
		int	l2 = atoi(loc2);
		int	sl1 = strlen(cnt1);
		int	sl2 = strlen(cnt2);
		char	sym;

		     if (sl1 > sl2) ret = -1;
		else if (sl1 < sl2) ret =  1;
		else if (l1  > l2 ) ret =  1;
		else if (l1  < l2 ) ret = -1;
		else ret = strcmp(cnt1,cnt2);

#ifdef DEBUG
		switch(bool(ret))
		{
		case	0:
			sym = '=';
			break;
		case	-1:
			sym = '>';
			break;
		case	1:
			sym = '<';
		}
		printf("cc (%d,%d) for \"%s\" %c \"%s\"\n", l1, l2, cnt1, sym, cnt2);
#endif
	}
	free(v1);
	free(v2);
	return(ret);
}

static int
sfe(DtDtsDbRecord * item1, DtDtsDbRecord * item2)
{
	int             test1 = 0;
	int             test2 = 0;
	int             loc1;
	int             loc2;
	char           *value1, *value2;
	DtDtsDbRecord  *rec;
	DtDtsDbField   *fld;
	int		val;

	test1 |= get_value(item1, DtDTS_CONTENT) ? 2 : 0;
	test2 |= get_value(item2, DtDTS_CONTENT) ? 2 : 0;

	value1 = get_value(item1, DtDTS_NAME_PATTERN);
	if (value1)
	{
		if ((strlen(value1) == 1) && (*value1 == '*'))
		{
			test1 |= 0;
		}
		else
		{
			test1 |= 1;
		}
	}

	value2 = get_value(item2, DtDTS_NAME_PATTERN);
	if (value2)
	{
		if ((strlen(value2) == 1) && (*value2 == '*'))
		{
			test2 |= 0;
		}
		else
		{
			test2 |= 1;
		}
	}

	if (!(test1 & 1))
	{
		test1 |= get_value(item1, DtDTS_PATH_PATTERN) ? 1 : 0;
	}
	if (!(test2 & 1))
	{
		test2 |= get_value(item2, DtDTS_PATH_PATTERN) ? 1 : 0;
	}

	switch (test1)
	{
	case 0:
		loc1 = 4;
		break;
	case 1:
		loc1 = 2;
		break;
	case 2:
		loc1 = 3;
		break;
	case 3:
		loc1 = 1;
		break;
	}

	switch (test2)
	{
	case 0:
		loc2 = 4;
		break;
	case 1:
		loc2 = 2;
		break;
	case 2:
		loc2 = 3;
		break;
	case 3:
		loc2 = 1;
		break;
	}

	if (loc1 - loc2)
	{
		return (loc1 - loc2);
	}

	if (loc1 == 2)		/* loc1 == loc2 */
	{
		val = check_pattern(get_value(item1, DtDTS_PATH_PATTERN),
					get_value(item2, DtDTS_PATH_PATTERN));
		if(val)
		{
			return(val);
		}
		val = check_pattern(get_value(item1, DtDTS_NAME_PATTERN),
					get_value(item2, DtDTS_NAME_PATTERN));
		if(val)
		{
			return(val);
		}
	}

	if( loc1 == 3)
	{
		val = check_content(get_value(item1, DtDTS_CONTENT),
					get_value(item2, DtDTS_CONTENT));
		if(val)
		{
			return(val);
		}
	}
	/*
	 * neither pattern nor content, only needs to compare the
	 * number of fields. If they are equal then which has shorter
	 * record name will be more specific
	 */
	if (item2->fieldCount - item1->fieldCount)
	{
		return (item2->fieldCount - item1->fieldCount);
	}

	val =   mode_count(get_value(item2, DtDTS_MODE)) -
		mode_count(get_value(item1, DtDTS_MODE));
	if(val)
	{
		return(val);
	}


	val = strlen(XrmQuarkToString(item1->recordName)) - 
	      strlen(XrmQuarkToString(item2->recordName));
	if(val)
	{
		return(val);
	}
	else
	{
		return(item1->recordName - item2->recordName);
	}
}

int
cde_dc_compare(DtDtsDbRecord ** a, DtDtsDbRecord ** b)
{
	DtDtsDbRecord **x = (DtDtsDbRecord **) a;
	DtDtsDbRecord **y = (DtDtsDbRecord **) b;
	int		results;
	char		c;

	results = sfe(*x, *y);
	return(results);
}

static int
cde_ft_field_value(XrmQuark  name_quark)
{
	if (name_quark == XrmStringToQuark(DtDTS_PATH_PATTERN))
		return (1);
	else if (name_quark == XrmStringToQuark(DtDTS_NAME_PATTERN))
		return (2);
	else if (name_quark == XrmStringToQuark(DtDTS_MODE))
		return (3);
	else if (name_quark == XrmStringToQuark(DtDTS_LINK_PATH))
		return (4);
	else if (name_quark == XrmStringToQuark(DtDTS_LINK_NAME))
		return (5);
	else if (name_quark == XrmStringToQuark(DtDTS_CONTENT))
		return (6);
	else if (name_quark == XrmStringToQuark(DtDTS_DATA_ATTRIBUTES_NAME))
		return (7);
	else if (name_quark == XrmStringToQuark(DtDTS_DA_IS_SYNTHETIC))
		return (8);
	else
		return (9);

}

int
cde_dc_field_compare(DtDtsDbField ** a, DtDtsDbField ** b)
{
	return (cde_ft_field_value((*a)->fieldName) -
		cde_ft_field_value((*b)->fieldName));
}

int cde_da_compare(DtDtsDbRecord ** a, DtDtsDbRecord ** b)
{
	int             results = ((*a)->recordName) - ((*b)->recordName);

	if (!results)
	{
		results = (*a)->seq - (*b)->seq;
	}
	return (results);
}
