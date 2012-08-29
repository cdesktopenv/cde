/* $XConsortium: parser.y /main/6 1996/11/11 11:50:41 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */


%{
#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if (defined(sun) && defined(_XOPEN_SOURCE)) || defined(linux)
#include <time.h>
#endif
#include "csa.h"
#include "rtable4.h"
#include "cm.h"
#include "lexer.h"
#include "cmscalendar.h"
#include "appt4.h"
#include "cmsdata.h"
#include "attr.h"
#include "v4ops.h"
#include "v5ops.h"
#include "delete.h"
#include "log.h"
#include "iso8601.h"

extern char *pgname;
extern _DtCmsCalendar *currentCalendar;
extern int yylineno;
extern void yyerror(char*);
extern void report_err(char*);
extern char *str_to_cr(char*);
extern time_t convert_2_tick(char *);

typedef struct {
	int	total;
	int	current;
	cms_attribute *attrs;
} _attr_array;

static int		currentVersion;
static _attr_array	currentAttrs;


/*****************************************************************************
 * forward declaration of static functions used within the file
 *****************************************************************************/
static void add_hashed_attr_to_array(_attr_array *attrs, int num, char *value);
static void add_to_attr_array(_attr_array *attrs, char *name,
		char *tag, char *value);
static void grow_attr_array(_attr_array *attrs);
static CSA_enum get_tag_from_string(char *tagstr);
static CSA_return_code get_attr_value(int type, char *valstr,
		cms_attribute_value **attrval);
static CSA_return_code get_access_list_value(char *valstr,
		cms_attribute_value **attrval);
static CSA_return_code get_user_access_entry(char *string,
		cms_access_entry **a);
static CSA_return_code get_date_time_list_value(char *valstr,
		cms_attribute_value **attrval);
static int extend_entry_table(int hashednum, char *name, char *typestr);

%}

%start calendar

%union	{
	int number;
	char *s;
	Interval_4 periodVal;
	Appt_4 *appointment;
	Attribute_4 *attributelist_4;
	Except_4 *exceptionlist;
	Access_Entry_4 *accesslist;
	Event_Type_4 tagsVal;
	Tag_4 *taglist;
	Appt_Status_4 apptstatVal;
	Privacy_Level_4 privacyVal;
	void *entry;
	}

%token VERSION COLON NUMBER QUOTEDSTRING DETAILS DURATION PERIODVAL NTIMES COMMA
%token OPENPAREN CLOSEPAREN WHAT REMOVE ADD ADDENTRY PERIOD EXCEPTION MAILTO NTH ENDDATE
%token AUTHOR ATTRIBUTES_4 ACCESS DENY PRIVACY PRIVACYENUM
%token KEY READ WRITE DELETE EXEC CONNECT BROWSE INSERT UPDATE
%token TAGS TAGSVAL
%token APPTSTATUS APPTSTATUSVAL
%token PRIVACYLEVEL PRIVACYVAL
%token ATTRIBUTES_5 CALATTRS
%token HASHEDATTRS TABLE

%type <s> quotedString what details mailto author
%type <number> number tick key ntimes duration accesstype nth enddate
%type <entry> appointmentBody 
%type <attributelist_4> attributes_4 attributelist_4 attributeitem_4
%type <periodVal> period 
%type <exceptionlist> exceptions exceptionlist exceptitem
%type <accesslist> accesses accesslist accessitem
%type <taglist> tags taglist tagitem 
%type <apptstatVal> apptstat
%type <privacyVal> privacy

%% /* Begining of rules section */

calendar	: versionStamp eventlist
		| versionStamp
		;

versionStamp	: VERSION COLON number
			{
				if ($3 > _DtCMS_VERSION4) { 
					report_err("Unable to read callog file, unsupported version\n");
					return -1;
				} else {
					currentVersion = $3;
					if (_DtCmsSetFileVersion(
					    currentCalendar, $3))
						return -1;
				}
			}
		| error OPENPAREN
			{
				yyerror("Error reading callog file");
				return -1;
			}
		;

number		: NUMBER
			{
				$$ = externNumberVal;
			}
		;

eventlist	: event
		| eventlist event
		;

event		: name_type_table
		| add 
		| remove 
		| access
		| calendar_attrs
		;

add		: OPENPAREN ADD appointmentBody CLOSEPAREN
			{
				if (currentVersion == _DtCMS_VERSION1) {
                               		if (_DtCmsInsertAppt(currentCalendar,
					    (Appt_4 *)$3) != CSA_SUCCESS) {
						_DtCm_free_appt4((Appt_4 *)$3);

						fprintf(stderr,
							"Failed to insert appt at line %d\n",
							yylineno);
						return (-1);
					}
				} else {
                               		if (_DtCmsInsertEntry4Parser(
					    currentCalendar, (cms_entry *)$3)
					    != CSA_SUCCESS) {

						fprintf(stderr, "Failed to insert entry at line %d\n",
							yylineno);
						return (-1);
					}
					_DtCm_free_cms_attributes(
						((cms_entry *)$3)->num_attrs,
						((cms_entry *)$3)->attrs);
					free($3);
				}
			}
		;

remove		: OPENPAREN REMOVE tick key CLOSEPAREN
			{
				Appt_4 *appt4;
				Id_4 key;
				cms_key ckey;

				if (currentVersion == _DtCMS_VERSION1) {
					key.tick = $3;
					key.key = $4;
					_DtCmsDeleteAppt(currentCalendar,
						NULL, 0, &key, &appt4);
					if (appt4)
						_DtCm_free_appt4(appt4);
				} else {
					ckey.time = $3;
					ckey.id = $4;
					_DtCmsDeleteEntry(currentCalendar, NULL,
						0, &ckey, NULL);
				}
			}
		;

access		: OPENPAREN ACCESS accesstype accesses CLOSEPAREN
			{
				_DtCmsSetAccess4Parser(currentCalendar, $4, $3);
			}
		;

calendar_attrs:	OPENPAREN CALATTRS attributelist_5 CLOSEPAREN
			{
				_DtCmsSetCalendarAttrs4Parser(currentCalendar,
					currentAttrs.current,
					currentAttrs.attrs);

				_DtCm_free_cms_attributes(currentAttrs.current,
					currentAttrs.attrs);
				currentAttrs.total = 0;
				currentAttrs.current = 0;
				currentAttrs.attrs = NULL;
			}
		;

/*
 * The attribute name and type hash table, each item contains
 * the hash number for the corresponding attribute, the attribute name,
 * and the value type of the attribute.
 */
name_type_table : OPENPAREN TABLE name_type_list CLOSEPAREN
			{
				currentCalendar->hashed = B_TRUE;
			}
		;

name_type_list	: name_type_item
		| name_type_item name_type_list
		;

name_type_item	: OPENPAREN number quotedString quotedString CLOSEPAREN
			{
				if (extend_entry_table($2, $3, $4)) {
					fprintf(stderr, "Failed to parse entry table at line # %d\n",
						yylineno);
					return (-1);
				}
			}
		;

/*		  1	2   3	4	5	 6	7	8	9  10
	11   12		13	 14	15	16	17
*/
appointmentBody	: tick key what details duration period nth enddate ntimes exceptions mailto author attributes_4 tags apptstat privacy attributes_5 hashedattrs
			{
				char *temp=NULL;
				Attr_4 item;
				Appt_4 *newp;
				cms_entry *newe;
				void	*aptr;

				if (currentVersion == _DtCMS_VERSION1) {
					newp = _DtCm_make_appt4(B_TRUE);
					newp->appt_id.tick = $1;
					newp->appt_id.key = $2;
					newp->what = $3;
					newp->client_data = $4;
					newp->duration = $5;
					newp->period.period = $6;
					newp->period.nth = $7;
					newp->period.enddate = $8;
					newp->ntimes = $9;
					newp->exception = $10;  

					temp = $11;

					newp->author = $12;
					newp->attr = $13;

					if (temp != NULL && (*temp)!=NULL) {
						item = newp->attr;
						while(item!=NULL) {
							if(strcmp(item->attr, "ml")==0) {
								item->clientdata= temp;
								break;
							}
							item = item->next;
						}
					}

					newp->tag = $14;
					newp->appt_status = $15;
					newp->privacy = $16;
					newp->next=NULL;
					aptr = (void *)newp;
				} else {
					newe = (cms_entry *)calloc(1,
						sizeof(cms_entry));
					newe->key.time = $1;
					newe->key.id = $2;
					newe->num_attrs = currentAttrs.current;
					newe->attrs = currentAttrs.attrs;
					currentAttrs.total = 0;
					currentAttrs.current = 0;
					currentAttrs.attrs = NULL;
					aptr = (void *)newe;
				}

				$$ = aptr;
			}
		;

tick		: /* empty */
			{
				$$ = 0;
			}
		| quotedString 
			{
				time_t	time;

				if (strlen($1) == 24)
					$$ = convert_2_tick($1);
				else {
					_csa_iso8601_to_tick($1, &time);
					$$ = time;
				}
					
			}
		;

quotedString	: QUOTEDSTRING
			{
				$$ = externQuotedString;
			}
		;

key		: /* empty */
			{
				$$ = 0;
			}
		| KEY COLON number
			{
				$$ = $3;
			}
		;

what		: /* empty */
			{
				$$ = strdup("");
			}
		| WHAT COLON quotedString
			{
				$$ = str_to_cr($3);
				free($3);
			}
		;
				
details		: /* empty */
			{
				$$ = "";
			}
		| DETAILS COLON quotedString
			{
				$$ = $3;
			}
		;

mailto		: /* empty */
			{
				$$ = "";
			}
		| MAILTO COLON quotedString
			{
				$$ = $3;
			}
		;

duration	: /* empty */
			{
				$$ = NULL;
			}
		| DURATION COLON number
			{
				$$ = $3;
			}
		;

period		: /* empty */
			{
				$$ = single_4;
			}
		| PERIOD COLON PERIODVAL
			{
				$$ = externPeriod.period;
			}
		;

nth		: /* empty */
			{
				$$ = 0;
			}
		| NTH COLON number
			{
				$$ = $3;
			}
		;

enddate		: /* empty */
			{
				$$ = 0;
			}
		| ENDDATE COLON quotedString
			{
				$$ = convert_2_tick($3);
				free($3);
			}
		;
				
privacy		: /* empty */
			{
				$$ = public_4;
			}
		| PRIVACY COLON PRIVACYVAL
			{
				$$ = externPrivacy;
			}
		;

apptstat	: /* empty */
			{
				$$ = active_4;
			}
		| APPTSTATUS COLON APPTSTATUSVAL
			{
				$$ = externApptStatus;
			}
		;

ntimes		: /* empty */
			{
				$$ = 0;
			}
		| NTIMES COLON number
			{
				$$ = $3;
			}
		;

author		: /* empty */
			{
				$$ = "";
			}
		| AUTHOR COLON quotedString
			{
				$$ = $3;
			}
		;

accesstype	: READ
			{
				$$ = access_read_4;
			}
		| WRITE
			{
				$$ = access_write_4;
			}
		| DELETE
			{
				$$ = access_delete_4;
			}
		| EXEC
			{
				$$ = access_exec_4;
			}
		;

accesses	: /* empty */
			{
				$$ = NULL;
			}
		| accesslist
			{
				$$ = $1;
			}
		;

accesslist	: accessitem
			{
				$$ = $1;
			}

		| accessitem accesslist
			{
				Access_Entry_4 *e = $1;
				e->next = $2;
				$$ = e;
			}
		;

accessitem	: quotedString 
			{
				Access_Entry_4 *e =
					(Access_Entry_4 *)malloc(
						sizeof(Access_Entry_4));
				e->who = $1;
				e->next = (Access_Entry_4 *)NULL;
				$$ = e;
			}
		;

hashedattrs	: /* empty */
		| HASHEDATTRS COLON OPENPAREN hashedattrlist CLOSEPAREN
		;

hashedattrlist : hashedattritem
		| hashedattritem hashedattrlist
		;

hashedattritem :OPENPAREN number quotedString CLOSEPAREN
			{	
				add_hashed_attr_to_array(&currentAttrs, $2, $3);
			}
                ;

attributes_5	: /* empty */
		| ATTRIBUTES_5 COLON OPENPAREN attributelist_5 CLOSEPAREN
		;

attributelist_5 : attributeitem_5
		| attributeitem_5 attributelist_5
		;

attributeitem_5 :OPENPAREN quotedString COMMA quotedString COMMA quotedString CLOSEPAREN
			{	
				if (currentAttrs.total == currentAttrs.current)
				{
					grow_attr_array(&currentAttrs);
				}
				add_to_attr_array(&currentAttrs, $2, $4, $6);
			}
                ;

attributes_4	: /* empty */
			{
				$$ = NULL;
			}
		| ATTRIBUTES_4 COLON OPENPAREN attributelist_4 CLOSEPAREN
			{
				$$ = $4;
			}
		;

attributelist_4  : attributeitem_4
			{
				$$ = $1;
			}
		| attributeitem_4 attributelist_4
			{
				Attr_4 p = $1;
				p->next = $2;
				$$ = p;
			}
		;
attributeitem_4	:OPENPAREN quotedString COMMA quotedString COMMA quotedString CLOSEPAREN
			{	
				Attr_4 newattr = _DtCm_make_attr4();
				newattr->next = (Attr_4)NULL;
				newattr->attr = $2;
				newattr->value = $4;
				newattr->clientdata = $6;
				$$ = newattr;
			}
		| OPENPAREN quotedString COMMA quotedString CLOSEPAREN
                        {
                                Attr_4 newattr = _DtCm_make_attr4();
                                newattr->next = (Attr_4)NULL;
                                newattr->attr = $2;
                                newattr->value = $4;
                                $$ = newattr;
                        }
                ;

tags		: /* empty */
			{
				Tag_4 *t = (Tag_4 *)malloc(sizeof(Tag_4));
				t->next = (Tag_4 *)NULL;
				t->tag = appointment_4;
				t->showtime = 1;
				$$ = t;
			}
		| TAGS COLON OPENPAREN taglist CLOSEPAREN
			{
				$$ = $4;
			}
		;

taglist		: tagitem
			{
				$$ = $1;
			}
		| tagitem taglist
			{
				Tag_4 *t = $1;
				t->next = $2;
				$$ = t;
			}
		;

tagitem		: OPENPAREN TAGSVAL COMMA number CLOSEPAREN
			{
				Tag_4 *t = (Tag_4 *)malloc(sizeof(Tag_4));
				t->next = (Tag_4 *)NULL;
				t->tag = externTag.tag;
				t->showtime = $4;
				$$ = t;
			}
		;


exceptions	: /* empty */
			{
				$$ = NULL;
			}
		| EXCEPTION COLON OPENPAREN exceptionlist CLOSEPAREN
			{
				$$ = $4;
			}
		;

exceptionlist : exceptitem
			{
				$$ = $1;
			}
		| exceptitem exceptionlist
			{
				Exception_4 p = $1;
				p->next = $2;
				$$ = p;
			}
		;
exceptitem : number
		{
				Exception_4 newexcept =
					(Exception_4)malloc(sizeof(Except_4));
				newexcept->next = (Exception_4)NULL;
				newexcept->ordinal = $1;
				$$ = newexcept;
		}
		;


%% /* Begining of subroutine section */

void
yyerror(char *s)
{
        (void)fprintf (stderr, "%s: %s\n", pgname, s);
        (void)fprintf (stderr, "at line %d\n", yylineno);
}
 
void
report_err(char *s)
{
         (void)fprintf (stderr, "%s", s);
}

char *
str_to_cr(char *s)
{
        int i, j, k;
        char *newstr;

        if (s==NULL) return(NULL);
        i = strlen(s);

        newstr= (char *)calloc(1, (unsigned)i + 1);
        k = 0;
        for (j=0; j<i; j++) {
                if (s[j]=='\\') {
                        if (s[j+1]=='n') {
                                newstr[k] = '\n';
                                j++;
                        }
                        else if (s[j+1]=='\\') {
                                newstr[k] = '\\';
                                j++;
                        }
			else if (s[j+1]=='\"') {
				newstr[k] = '\"';
				j++;
			}
                        else {
                                newstr[k] = s[j];
                        }
                }
                else {
                        newstr[k] = s[j];
                }
                k++;
        }
        newstr[k] = NULL;
        return(newstr);
}

int
get_wday(char *wday)
{
	if (wday == NULL)
		return (-1);

	if (strncmp(wday, "Sun", 3) == 0)
		return (0);
	else if (strncmp(wday, "Mon", 3) == 0)
		return (1);
	else if (strncmp(wday, "Tue", 3) == 0)
		return (2);
	else if (strncmp(wday, "Wed", 3) == 0)
		return (3);
	else if (strncmp(wday, "Thu", 3) == 0)
		return (4);
	else if (strncmp(wday, "Fri", 3) == 0)
		return (5);
	else if (strncmp(wday, "Sat", 3) == 0)
		return (6);
	else
		return (-1);
}

int
get_month(char *month)
{
	if (month == NULL)
		return (-1);

	if (strncmp(month, "Jan", 3) == 0)
		return (0);
	if (strncmp(month, "Feb", 3) == 0)
		return (1);
	if (strncmp(month, "Mar", 3) == 0)
		return (2);
	if (strncmp(month, "Apr", 3) == 0)
		return (3);
	if (strncmp(month, "May", 3) == 0)
		return (4);
	if (strncmp(month, "Jun", 3) == 0)
		return (5);
	if (strncmp(month, "Jul", 3) == 0)
		return (6);
	if (strncmp(month, "Aug", 3) == 0)
		return (7);
	if (strncmp(month, "Sep", 3) == 0)
		return (8);
	if (strncmp(month, "Oct", 3) == 0)
		return (9);
	if (strncmp(month, "Nov", 3) == 0)
		return (10);
	if (strncmp(month, "Dec", 3) == 0)
		return (11);
}

time_t
convert_2_tick(char *datestr)
{
	struct tm tmstr;
	char	datebuf[BUFSIZ];
	char	*ptr, *tptr;

	if (datestr == NULL)
		return (-1);

	memset((void *)&tmstr, NULL, sizeof(struct tm));

	strcpy(datebuf, datestr);

	/* get week day */
	ptr = strtok(datebuf, " ");
	if ((tmstr.tm_wday = get_wday(ptr)) == -1)
		return (-1);

	/* get month */
	ptr = strtok(NULL, " ");
	if ((tmstr.tm_mon = get_month(ptr)) == -1)
		return (-1);

	/* get day of month */
	ptr = strtok(NULL, " ");
	if (ptr != NULL)
		tmstr.tm_mday = atoi(ptr);
	else
		return (-1);

	/* get time */
	ptr = strtok(NULL, " ");
	if (ptr != NULL) {
		/* get hour */
		if ((tptr = strchr(ptr, ':')) == NULL)
			return (-1);
		else {
			*tptr = NULL;
			tmstr.tm_hour = atoi(ptr);
		}

		ptr = tptr + 1;
		if ((tptr = strchr(ptr, ':')) == NULL)
			return (-1);
		else {
			*tptr = NULL;
			tmstr.tm_min = atoi(ptr);
		}

		tmstr.tm_sec = atoi(tptr + 1);
	} else
		return (-1);

	ptr = strtok(NULL, " ");
	if (ptr != NULL)
		tmstr.tm_year = atoi(ptr) - 1900;
	else
		return (-1);

	tmstr.tm_isdst = -1;

	return(mktime(&tmstr));
}

static void
add_hashed_attr_to_array(_attr_array *attrs, int num, char *value)
{
	int		type;

	if (num > currentCalendar->entry_tbl->size)
		return;

	if (attrs->current == attrs->total) {
		grow_attr_array(attrs);
	}

	attrs->attrs[attrs->current].name.num = num;
	attrs->attrs[attrs->current].name.name =
		strdup(currentCalendar->entry_tbl->names[num]);

	if (get_attr_value(currentCalendar->types[num], value,
	    &attrs->attrs[attrs->current].value) == CSA_SUCCESS)
		attrs->current++;

	return;
}

static void
add_to_attr_array(_attr_array *attrs, char *name, char *tag, char *value)
{
	int		type;

	if (attrs->current == attrs->total) {
		grow_attr_array(attrs);
	}

	attrs->attrs[attrs->current].name.name = name;
	if ((type = get_tag_from_string(tag)) == -1)
		return;

	if (get_attr_value(type, value, &attrs->attrs[attrs->current].value)
	    == CSA_SUCCESS)
		attrs->current++;

	return;
}

static CSA_enum
get_tag_from_string(char *tagstr)
{
	int	i;

	if (*tagstr < '0' || *tagstr > '9')
		return (-1);

	i = atoi(tagstr);

	if (i < CSA_VALUE_BOOLEAN || i > CSA_VALUE_OPAQUE_DATA ||
	    i == CSA_VALUE_ATTENDEE_LIST)
		return (-1);
	else
		return (i);
}

static CSA_return_code
get_attr_value(int type, char *valstr, cms_attribute_value **attrval)
{
	char 		buf1[BUFSIZ], buf2[BUFSIZ];
	char		*ptr1, *ptr2, *ptr3, *ptr4;
	uint		unum;
	CSA_reminder	remval;
	CSA_opaque_data opqval;
	CSA_return_code stat;

	switch (type) {
	case CSA_VALUE_ENUMERATED:
	case CSA_VALUE_SINT32:
		stat = _DtCm_set_sint32_attrval(atoi(valstr), attrval);
		break;
	case CSA_VALUE_BOOLEAN:
	case CSA_VALUE_FLAGS:
	case CSA_VALUE_UINT32:
		sscanf(valstr, "%u", &unum);
		stat = _DtCm_set_uint32_attrval(unum, attrval);
		break;
	case CSA_VALUE_STRING:
	case CSA_VALUE_DATE_TIME:
	case CSA_VALUE_DATE_TIME_RANGE:
	case CSA_VALUE_TIME_DURATION:
	case CSA_VALUE_CALENDAR_USER:
		stat = _DtCm_set_string_attrval(valstr, attrval, type);
		break;
	case CSA_VALUE_REMINDER:
		memset((void *)&remval, NULL, sizeof (CSA_reminder));
		if (ptr1 = strchr(valstr, ':')) {
			*ptr1++ = NULL;
			if (ptr2 = strchr(ptr1, ':')) {
				*ptr2++ = NULL;
				ptr3 = strchr(ptr2, ':');
			}
		}

		if (ptr1 == NULL || ptr2 == NULL) {
			stat = CSA_E_INVALID_PARAMETER;
			break;
		}

		if (ptr3 == NULL) {
			/* format = "string:number:string" */
			remval.lead_time = valstr;
			remval.reminder_data.size = atoi(ptr1);
			remval.reminder_data.data = (unsigned char *)ptr2;
			stat = _DtCm_set_reminder_attrval(&remval, attrval);
		} else {
			/* format = "string:string:number:number:string" */
			*ptr3++ = NULL;
			if (ptr4 = strchr(ptr3, ':')) {
				*ptr4++ = NULL;
				remval.lead_time = valstr;
				remval.snooze_time = ptr1;
				remval.repeat_count = atoi(ptr2);
				remval.reminder_data.size = atoi(ptr3);
				remval.reminder_data.data =
					(unsigned char *)ptr4;
				stat = _DtCm_set_reminder_attrval(&remval,
					attrval);
			} else
				stat = CSA_E_INVALID_PARAMETER;
		}

		break;
	case CSA_VALUE_ACCESS_LIST:
		stat = get_access_list_value(valstr, attrval);
		break;
	case CSA_VALUE_DATE_TIME_LIST:
		stat = get_date_time_list_value(valstr, attrval);
		break;
	case CSA_VALUE_OPAQUE_DATA:
		if (ptr1 = strchr(valstr, ':')) {
			*ptr1++ = NULL;
			opqval.size = atoi(valstr);
			opqval.data = (unsigned char *)ptr1;
			stat = _DtCm_set_opaque_attrval(&opqval, attrval);
		} else
			stat = CSA_E_INVALID_PARAMETER;
		break;
	}

	/* need to set type here since type may not be set correctly up there */
	(*attrval)->type = type;

	free(valstr);
	return (stat);
}

static CSA_return_code
get_access_list_value(char *valstr, cms_attribute_value **attrval)
{
	char *ptr;
	cms_access_entry *a, *head, *prev;
	CSA_return_code stat = CSA_SUCCESS;
	cms_attribute_value *val;

	if ((val = (cms_attribute_value *)calloc(1,
	    sizeof(cms_attribute_value))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	val->type = CSA_VALUE_ACCESS_LIST;

	if (*valstr == '\0') {
		*attrval = val;
		return (CSA_SUCCESS);
	}

	head = NULL;
	while (ptr = strchr(valstr, ' ')) {

		*ptr = 0;
		stat = get_user_access_entry(valstr, &a);
		*ptr = ' ';

		if (stat != CSA_SUCCESS)
			break;

		if (head == NULL)
			head = a;
		else
			prev->next = a;
		prev = a;

		valstr = ptr + 1;
	}

	if (stat == CSA_SUCCESS) {
		if ((stat = get_user_access_entry(valstr, &a))
		    == CSA_SUCCESS) {
			if (head == NULL)
				head = a;
			else
				prev->next = a;
		}
	}

	if (stat == CSA_SUCCESS) {
		val->item.access_list_value = head;
		*attrval = val;
	} else {
		_DtCm_free_cms_access_entry(head);
		free(val);
	}

	return (stat);
}

static CSA_return_code
get_user_access_entry(char *string, cms_access_entry **a)
{
	char *ptr;
	cms_access_entry *item;

	if ((ptr = strchr(string, ':')) == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((item = (cms_access_entry *)malloc(sizeof(cms_access_entry)))
	    == NULL) {
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	*ptr = NULL;
	if ((item->user = strdup(string)) == NULL) {
		free(item);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}
	item->rights = atoi(ptr+1);
	item->next = NULL;

	*a = item;

	return (CSA_SUCCESS);
}

#define	_DEFAULT_ARRAY_SIZE	30

static void
grow_attr_array(_attr_array *attrs)
{
	cms_attribute *newptr;

	if ((newptr = (cms_attribute *)realloc(attrs->attrs,
	    sizeof(cms_attribute) * (attrs->total + _DEFAULT_ARRAY_SIZE)))
	    == NULL)
		return;

	attrs->total += _DEFAULT_ARRAY_SIZE;
	attrs->attrs = newptr;

	memset((void *)&attrs->attrs[attrs->current], NULL,
		sizeof(cms_attribute)*_DEFAULT_ARRAY_SIZE);
}

static CSA_return_code
get_date_time_list_value(char *valstr, cms_attribute_value **attrval)
{
	char *ptr;
	CSA_date_time_entry *a, *head, *prev;
	CSA_return_code stat = CSA_SUCCESS;
	cms_attribute_value *val;

	if (*valstr == '\0') {
		*attrval = NULL;
		return (CSA_SUCCESS);
	}

	if ((val = (cms_attribute_value *)calloc(1,
	    sizeof(cms_attribute_value))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	val->type = CSA_VALUE_DATE_TIME_LIST;

	head = NULL;
	while (ptr = strchr(valstr, ' ')) {

		*ptr = 0;
		if (!(a = calloc(1, sizeof(CSA_date_time_entry))) ||
		    !(a->date_time = strdup(valstr))) {
			*ptr = ' ';
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}
		*ptr = ' ';

		if (head == NULL)
			head = a;
		else
			prev->next = a;
		prev = a;

		valstr = ptr + 1;
	}

	if (stat == CSA_SUCCESS) {
		if ((a = calloc(1, sizeof(CSA_date_time_entry))) &&
		    (a->date_time = strdup(valstr))) {
			if (head == NULL)
				head = a;
			else
				prev->next = a;
		} else
			stat = CSA_E_INSUFFICIENT_MEMORY;
	}

	if (stat == CSA_SUCCESS) {
		val->item.date_time_list_value = head;
		*attrval = val;
	} else {
		_DtCm_free_date_time_list(head);
		free(val);
	}

	return (stat);
}

static int
extend_entry_table(int hashednum, char *name, char *typestr)
{
	int	type;

	if (hashednum > _DtCM_DEFINED_ENTRY_ATTR_SIZE) {
		if ((type = get_tag_from_string(typestr)) == -1)
			return (-1);

		(void)_DtCmExtendNameTable(name, hashednum, type,
			_DtCm_entry_name_tbl, _DtCM_DEFINED_ENTRY_ATTR_SIZE,
			_CSA_entry_attribute_names, &currentCalendar->entry_tbl,
			&currentCalendar->types);
	}
	return (0);
}

