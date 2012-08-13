/* $XConsortium: reparser.y /main/2 1996/11/11 11:52:15 drk $ */
%{
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "rerule.h"
#include "iso8601.h"

/* The parsed rule is stored in this structure */
RepeatEvent	*_DtCm_repeat_info;

extern int		 yylex();
extern void		 yyerror(char*);

static int CompareNums(const void *, const void *);
static int CompareDayTime(const void *, const void *);
static int CompareWeekDayTime(const void *, const void *);
static unsigned int *ConvertNumList(NumberList *, unsigned int * /* Return */);
static WeekDayTime *ConvertWeekDayTime(WeekDayTimeList *, unsigned int *);
static NumberList *AllocNumber(unsigned int);
static WeekDayTimeList *AllocWeekDayTimeList(NumberList*, NumberList *,
			NumberList *);
static RepeatEvent *HandleEndDate(RepeatEvent *, time_t);
static RepeatEvent *DeriveMinuteEvent(unsigned int, unsigned int);
static RepeatEvent *DeriveDailyEvent(unsigned int, NumberList *,
			unsigned int, RepeatEvent *);
static RepeatEvent *DeriveWeeklyEvent(unsigned int, DayTimeList *,
			unsigned int, RepeatEvent *);
static RepeatEvent *DeriveMonthlyEvent(RepeatType, unsigned int, void *,
			unsigned int, RepeatEvent *);
static RepeatEvent *DeriveYearlyEvent(RepeatType, unsigned int, NumberList *,
			unsigned int, RepeatEvent *);

%}

%token ERROR ENDMARKER DURATION NUMBER FIRSTWEEK SECONDWEEK THIRDWEEK
%token FOURTHWEEK FIFTHWEEK LASTWEEK SECONDLAST THIRDLAST FOURTHLAST            
%token FIFTHLAST MINUTECOMMAND DAILYCOMMAND WEEKLYCOMMAND MONTHPOSCOMMAND       
%token MONTHDAYCOMMAND YEARDAYCOMMAND YEARMONTHCOMMAND LASTDAY SUNDAY
%token MONDAY TUESDAY WEDNESDAY THURSDAY FRIDAY SATURDAY DATE 

%union	{
		int		 number;
		RepeatEvent	*re;
		NumberList	*nl;
		WeekDay		 weekday;
		WeekNumber	 weeknum;
		DayTime		*dt;
		DayTimeList	*dtl;
		WeekDayTimeList	*wdtl;
		time_t		 enddate;
		char		 date[64];
	}

%type <re>	start begin minuteEvent dailyEvent weeklyEvent monthlyPosEvent
		monthlyDayEvent yearlyByMonth yearlyByDay
%type <nl>	time0List timeList dayOfMonthList monthOfYearList
		dayOfYearList occurrenceList genericNumberList 
		generic0NumberList weekdayList
%type <weekday>	SUNDAY MONDAY TUESDAY WEDNESDAY THURSDAY FRIDAY SATURDAY
		weekday
%type <number>	duration NUMBER occurrence endMarker time
%type <dtl>	weekdayTimeList
%type <wdtl>	weekDayTime
%type <dt>	weekdayTimePair
%type <date>	DATE
%type <enddate> endDate

%% /* Beginning of rules section */

start 		: begin 
		    {
			_DtCm_repeat_info = $$;
		    }
		| error
		    {
			/*
			 * XXX: Memory leak: We need to free up any portion
			 * of the re struct that has be allocated before the
			 * error was encountered.
			 */
			_DtCm_repeat_info = NULL;
		    }
		;

begin		: minuteEvent endDate
		    {
			$$ = HandleEndDate($1, $2);
		    }
		| dailyEvent endDate
		    {
			$$ = HandleEndDate($1, $2);
		    }
		| weeklyEvent endDate
		    {
			$$ = HandleEndDate($1, $2);
		    }
		| monthlyPosEvent endDate
		    {
			$$ = HandleEndDate($1, $2);
		    }
		| monthlyDayEvent endDate
		    {
			$$ = HandleEndDate($1, $2);
		    }
		| yearlyByMonth endDate
		    {
			$$ = HandleEndDate($1, $2);
		    }
		| yearlyByDay endDate
		    {
			$$ = HandleEndDate($1, $2);
		    }
		;

minuteEvent	: /* empty */
		    {
			$$ = NULL;
		    }
		| MINUTECOMMAND NUMBER duration
		    {
#ifdef MINUTE
			$$ = DeriveMinuteEvent($2, $3);
#else
			$$ = NULL;
#endif /* MINUTE */
	  	    }
		;

dailyEvent
		: DAILYCOMMAND NUMBER time0List duration minuteEvent
		    {
			$$ = DeriveDailyEvent($2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		;

weeklyEvent
		: WEEKLYCOMMAND NUMBER weekdayTimeList duration minuteEvent
		    {
			$$ = DeriveWeeklyEvent($2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| WEEKLYCOMMAND NUMBER duration minuteEvent 
		    {
			$$ = DeriveWeeklyEvent($2, NULL, $3, $4);
			if ($4) $$ = NULL;
		    }
		;

monthlyPosEvent
		: MONTHPOSCOMMAND NUMBER weekDayTime duration minuteEvent
		    {
			$$ = DeriveMonthlyEvent(RT_MONTHLY_POSITION,
						$2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| MONTHPOSCOMMAND NUMBER weekDayTime duration dailyEvent 
		    {
			$$ = DeriveMonthlyEvent(RT_MONTHLY_POSITION,
						$2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| MONTHPOSCOMMAND NUMBER weekDayTime duration weeklyEvent
		    {
			$$ = DeriveMonthlyEvent(RT_MONTHLY_POSITION,
						$2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		;

monthlyDayEvent	: MONTHDAYCOMMAND NUMBER dayOfMonthList duration minuteEvent
		    {
			$$ = DeriveMonthlyEvent(RT_MONTHLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| MONTHDAYCOMMAND NUMBER dayOfMonthList duration dailyEvent
		    {
			$$ = DeriveMonthlyEvent(RT_MONTHLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| MONTHDAYCOMMAND NUMBER dayOfMonthList duration weeklyEvent
		    {
			$$ = DeriveMonthlyEvent(RT_MONTHLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		;

yearlyByMonth	: YEARMONTHCOMMAND NUMBER monthOfYearList duration minuteEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_MONTH, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARMONTHCOMMAND NUMBER monthOfYearList duration dailyEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_MONTH, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARMONTHCOMMAND NUMBER monthOfYearList duration weeklyEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_MONTH, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARMONTHCOMMAND NUMBER monthOfYearList duration
								monthlyPosEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_MONTH, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARMONTHCOMMAND NUMBER monthOfYearList duration
								monthlyDayEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_MONTH, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		;

yearlyByDay	: YEARDAYCOMMAND NUMBER dayOfYearList duration minuteEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARDAYCOMMAND NUMBER dayOfYearList duration dailyEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARDAYCOMMAND NUMBER dayOfYearList duration weeklyEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARDAYCOMMAND NUMBER dayOfYearList duration monthlyPosEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		| YEARDAYCOMMAND NUMBER dayOfYearList duration monthlyDayEvent 
		    {
			$$ = DeriveYearlyEvent(RT_YEARLY_DAY, $2, $3, $4, $5);
			if ($5) $$ = NULL;
		    }
		;

		/* e.g. 1W MO TU 1300 1400 */
weekDayTime	: /* empty */
		    {
			$$ = NULL;
		    }
		| weekDayTime occurrenceList weekdayList
		    {
			WeekDayTimeList	*wdtl;

			wdtl = AllocWeekDayTimeList($2, $3, NULL);

			if ($1 == NULL) {
				$$ = wdtl;
			} else {
				wdtl->wdtl_next = $$->wdtl_next;
				$$->wdtl_next = wdtl;
			}
		    }
		| weekDayTime occurrenceList weekdayList timeList
		    {
			WeekDayTimeList	*wdtl;

			wdtl = AllocWeekDayTimeList($2, $3, $4);

			if ($1 == NULL) {
				$$ = wdtl;
			} else {
				wdtl->wdtl_next = $$->wdtl_next;
				$$->wdtl_next = wdtl;
			}
		    }
		;

		/* e.g. MO TU WE TH */
weekdayList	: weekday
		    {
			$$ = AllocNumber($1);
		    }
		| weekdayList weekday
		    {
			NumberList	*nl;

			nl = AllocNumber($2);

			if ($1 == NULL) {
				$$ = nl;
			} else {
				nl->nl_next = $$->nl_next;
				$$->nl_next = nl;
			}
		    }
		;

		/* e.g. 1W 3W 2L LW */
occurrenceList	: occurrence
		    {
			$$ = AllocNumber($1);
		    }
		| occurrenceList occurrence
		    {
			NumberList	*nl;

			nl = AllocNumber($2);

			if ($1 == NULL) {
				$$ = nl;
			} else {
				nl->nl_next = $$->nl_next;
				$$->nl_next = nl;
			}
		   }
		;

		/* e.g. MO 1300 1500 2000 */
weekdayTimePair	: weekday time0List
		    {
			DayTime		*dt;

			dt = (DayTime *)calloc(1, sizeof(DayTime));

			dt->dt_day = $1;
				/* Convert from list to array, sort */

			dt->dt_time =
				(Time *)ConvertNumList($2, &(dt->dt_ntime));
			$$ = dt;
		    }
		;

		/* e.g. MO 1300 1500 2000 TU 1200 1400 */
weekdayTimeList	: weekdayTimePair  
		    {
			DayTimeList	*dtl;

			dtl = (DayTimeList *)calloc(1, sizeof(DayTimeList));

			dtl->dtl_daytime = $1;
			dtl->dtl_next = NULL;

			$$ = dtl;

		    }
		| weekdayTimeList weekdayTimePair
		    {
			DayTimeList	*dtl,
					*dtl_end;

			dtl = (DayTimeList *)calloc(1, sizeof(DayTimeList));

			dtl->dtl_daytime = $2;
			dtl->dtl_next = NULL;

			if ($1 == NULL) {
				$$ = dtl;
			} else {
				/* Insert the new entry at the end.  This is
				 * so that MO TU 200 300 TH will maintain the
				 * same order in the list since MO uses the
				 * times next to TU and TH is dependent on the
				 * appt time.
				 */
				dtl_end = $$;
				while (dtl_end->dtl_next)
					dtl_end = dtl_end->dtl_next;
				
				dtl_end->dtl_next = dtl;
			}
		   }
		;

		/* e.g. 1+ 2+ 3- 1- */
occurrence	: FIRSTWEEK endMarker
		    {
			$$ = WK_F1;
			if ($2) RE_SET_FLAG($$);
		    }
		| SECONDWEEK endMarker
		    {
			$$ = WK_F2;
			if ($2) RE_SET_FLAG($$);
		    }
		| THIRDWEEK endMarker
		    {
			$$ = WK_F3;
			if ($2) RE_SET_FLAG($$);
		    }
		| FOURTHWEEK endMarker
		    {
			$$ = WK_F4;
			if ($2) RE_SET_FLAG($$);
		    }
		| FIFTHWEEK endMarker
		    {
			$$ = WK_F5;
			if ($2) RE_SET_FLAG($$);
		    }
		| LASTWEEK endMarker
		    {
			$$ = WK_L1;
			if ($2) RE_SET_FLAG($$);
		    }
		| SECONDLAST endMarker
		    {
			$$ = WK_L2;
			if ($2) RE_SET_FLAG($$);
		    }
		| THIRDLAST endMarker
		    {
			$$ = WK_L3;
			if ($2) RE_SET_FLAG($$);
		    }
		| FOURTHLAST endMarker
		    {
			$$ = WK_L4;
			if ($2) RE_SET_FLAG($$);
		    }
		| FIFTHLAST endMarker
		    {
			$$ = WK_L5;
			if ($2) RE_SET_FLAG($$);
		    }
		;

endDate	: /* empty */
		    {
			$$ = '\0';
		    }
	| DATE
		    {
			if (_csa_iso8601_to_tick($1, &$$) == -1)
				$$ = '\0';
		    }
		;

		/* e.g. MO TU WE */
weekday		: SUNDAY endMarker
		    {
			$$ = WD_SUN;
			if ($2) RE_SET_FLAG($$);
		    }
		| MONDAY endMarker
		    {
			$$ = WD_MON;
			if ($2) RE_SET_FLAG($$);
		    }
		| TUESDAY endMarker
		    {
			$$ = WD_TUE;
			if ($2) RE_SET_FLAG($$);
		    }
		| WEDNESDAY endMarker
		    {
			$$ = WD_WED;
			if ($2) RE_SET_FLAG($$);
		    }
		| THURSDAY endMarker
		    {
			$$ = WD_THU;
			if ($2) RE_SET_FLAG($$);
		    }
		| FRIDAY endMarker
		    {
			$$ = WD_FRI;
			if ($2) RE_SET_FLAG($$);
		    }
		| SATURDAY endMarker
		    {
			$$ = WD_SAT;
			if ($2) RE_SET_FLAG($$);
		    }
		;

		/* e.g. 0100 1200 1300 or NULL */
time0List	: /* empty */
		   {
			$$ = NULL;
		   }
		| time0List time
		   {
			NumberList	*nl;

			nl = AllocNumber($2);

			if ($1 == NULL) {
				$$ = nl;
			} else {
				nl->nl_next = $$->nl_next;
				$$->nl_next = nl;
			}
		   }
		;

		/* e.g. 1000 */
time		: NUMBER endMarker
		    {
			if ($2)
				RE_SET_FLAG($1);
			$$ = $1;
		    }

		;

		/* e.g. $ */
endMarker	: /* empty */
		    {
			$$ = FALSE;
		    }
		| ENDMARKER
		    {
			$$ = TRUE;
		    }
		;

		/* e.g. #10 */
duration	: /* empty */
		    {
			/* If no duration then default to 2 - set later if 
			 * end date not used.
			 */
			$$ = RE_NOTSET;
		    }
		| DURATION NUMBER
		    {
			/* If duration == 0 then repeat forever */
			if ($2 == 0)
				$$ = RE_INFINITY; 
			else
				$$ = $2;
		    }
		;

timeList	: genericNumberList
dayOfMonthList	: generic0NumberList
monthOfYearList	: generic0NumberList
dayOfYearList	: generic0NumberList

generic0NumberList: /* empty */
		    {
			$$ = NULL;
		    }
		| genericNumberList
		    {
			$$ = $1;
		    }

genericNumberList
		: NUMBER endMarker
		    {
			if ($2)
				RE_SET_FLAG($1);

			$$ = AllocNumber($1);
		    }
		| LASTDAY endMarker
		    {
			int	number = RE_LASTDAY;

			if ($2)
				RE_SET_FLAG(number);

			$$ = AllocNumber(number);
		    }
		| genericNumberList NUMBER endMarker
		    {
			NumberList	*nl;

			if ($3)
				RE_SET_FLAG($2);

			nl = AllocNumber($2);

			if ($1 == NULL) {
				$$ = nl;
			} else {
				nl->nl_next = $$->nl_next;
				$$->nl_next = nl;
		    	}
		    }
		| genericNumberList LASTDAY endMarker
		    {
			NumberList	*nl;
			int		 number = RE_LASTDAY;

			if ($3)
				RE_SET_FLAG(number);

			nl = AllocNumber(number);

			if ($1 == NULL) {
				$$ = nl;
			} else {
				nl->nl_next = $$->nl_next;
				$$->nl_next = nl;
		    	}
		    }
		;

%%

/*
 * Convert the NumberList (linked list) to an array, sort the array.
 */
static unsigned int *
ConvertNumList(
	NumberList	*nl,
	unsigned int	*count)
{
	NumberList	*nl_head = nl;
	unsigned int	*array;
	int		 i = 0;

	if (!nl) return (unsigned int *)NULL;

	while (nl) {
		i++;
		nl = nl->nl_next;
	}

	array = (unsigned int *) calloc(i, sizeof(unsigned int));
	i = 0;
			/* Convert the list into an array */
	nl = nl_head;
	while (nl) {
		NumberList	*nl_prev;

		array[i] = nl->nl_number;
		i++;

		nl_prev = nl;
		nl = nl->nl_next;

		free(nl_prev);
	}

	*count = i;

			/* Sort the array */
	qsort((void *)array, i, sizeof(unsigned int), CompareNums);

	return array;
}

/*
 * Convert the DayTimeList to an array, sort the array.
 */
static DayTime *
ConvertDayTime(
	DayTimeList	*dtl,
	unsigned int	*count)
{
	DayTimeList	*dtl_head = dtl;
	DayTime		*daytime_array;
	int		 i = 0,
			 no_time = -1;

	if (!dtl) return (DayTime *)NULL;

	while (dtl) {
		i++;
		dtl = dtl->dtl_next;
	}

	daytime_array = (DayTime *) calloc(i, sizeof(DayTime));
	i = 0;
			/* Convert the list into an array */
	dtl = dtl_head;
	while (dtl) {
		DayTimeList	*dtl_prev;

		daytime_array[i].dt_day = dtl->dtl_daytime->dt_day;
		daytime_array[i].dt_ntime = dtl->dtl_daytime->dt_ntime;
		daytime_array[i].dt_time = dtl->dtl_daytime->dt_time;
		i++;

		dtl_prev = dtl;
		dtl = dtl->dtl_next;
			/* alloc'ed in <weekdayTimeList> */
		free(dtl_prev);
	}

	*count = i;

	for (i = 0; i < *count; i++) {
		if (daytime_array[i].dt_time == NULL) {
			if (no_time == -1)
				no_time = i;
		} else {
			if (no_time != -1) {
				int j;

				for (j = no_time; j < i; j++) {
					daytime_array[j].dt_ntime = 
						daytime_array[i].dt_ntime;
					daytime_array[j].dt_time = 
						(Time *)calloc(
						     daytime_array[j].dt_ntime,
						     sizeof(Time));
					memcpy(daytime_array[j].dt_time,
					       daytime_array[i].dt_time,
					       daytime_array[j].dt_ntime *
								sizeof(Time));
				}
				no_time = -1;
			}
		}
	}

			/* Sort the array */
	qsort((void *)daytime_array, *count, sizeof(DayTime), CompareDayTime);

	return daytime_array;
}

/*
 * Used by qsort()
 */
static int
CompareNums(
	const void	*data1,
	const void	*data2)
{
	const unsigned int	*i = (const unsigned int *)data1;
	const unsigned int	*j = (const unsigned int *)data2;

	if ((unsigned )RE_MASK_STOP(*i) > (unsigned )RE_MASK_STOP(*j))
		return(1);
	if ((unsigned )RE_MASK_STOP(*i) < (unsigned )RE_MASK_STOP(*j))
		return(-1);
	return (0);
}

/*
 * Used by qsort()
 */
static int
CompareDayTime(
	const void	*data1,
	const void	*data2)
{
	const DayTime	*i = (const DayTime *)data1;
	const DayTime	*j = (const DayTime *)data2;

	if ((unsigned )RE_MASK_STOP(i->dt_day) >
					(unsigned )RE_MASK_STOP(j->dt_day))
		return(1);
	if ((unsigned )RE_MASK_STOP(i->dt_day) <
					(unsigned )RE_MASK_STOP(j->dt_day))
		return(-1);
	return (0);
}

/*
 * Used by qsort()
 */
static int
CompareWeekDayTime(
	const void	*data1,
	const void	*data2)
{
	const WeekDayTime	*i = (const WeekDayTime *)data1;
	const WeekDayTime	*j = (const WeekDayTime *)data2;

	if ((unsigned )RE_MASK_STOP(i->wdt_week[0]) >
					(unsigned )RE_MASK_STOP(j->wdt_week[0]))
		return(1);
	if ((unsigned )RE_MASK_STOP(i->wdt_week[0]) <
					(unsigned )RE_MASK_STOP(j->wdt_week[0]))
		return(-1);
	return (0);
}

static NumberList *
AllocNumber(
	unsigned int num)
{
	NumberList	*nl;

	nl = (NumberList *)calloc(1, sizeof(NumberList));

	nl->nl_number = num;
	nl->nl_next = NULL;

	return nl;
}

/*
 * Given three NumberLists convert them into arrays and return a WeekDayTime.
 */
static WeekDayTimeList *
AllocWeekDayTimeList(
	NumberList	*week_list,
	NumberList	*day_list,
	NumberList	*time_list)
{
	WeekDayTime	*wdt;
	WeekDayTimeList	*wdtl;

	wdt = (WeekDayTime *)calloc(1, sizeof(WeekDayTime));
	wdtl = (WeekDayTimeList *)calloc(1, sizeof(WeekDayTimeList));

	wdt->wdt_week =
		(WeekNumber *)ConvertNumList(week_list, &(wdt->wdt_nweek));
	wdt->wdt_day =
		(WeekDay *)ConvertNumList(day_list, &(wdt->wdt_nday));
	wdt->wdt_time =
		(Time *)ConvertNumList(time_list, &(wdt->wdt_ntime));
	wdtl->wdtl_weektime = wdt;

	return wdtl;
}

/*
 * Convert the DayTimeList to an array, sort the array.
 */
static WeekDayTime *
ConvertWeekDayTime(
	WeekDayTimeList	*wdtl,
	unsigned int	*count)
{
	WeekDayTimeList	*wdtl_head = wdtl;
	WeekDayTime	*array;
	int		 i = 0;

	if (!wdtl) return (WeekDayTime *)NULL;

	while (wdtl) {
		i++;
		wdtl = wdtl->wdtl_next;
	}

	array = (WeekDayTime *) calloc(i, sizeof(WeekDayTime));
	i = 0;
			/* Convert the list into an array */
	wdtl = wdtl_head;
	while (wdtl) {
		WeekDayTimeList	*wdtl_prev;

		array[i].wdt_day = wdtl->wdtl_weektime->wdt_day;
		array[i].wdt_nday = wdtl->wdtl_weektime->wdt_nday;
		array[i].wdt_time = wdtl->wdtl_weektime->wdt_time;
		array[i].wdt_ntime = wdtl->wdtl_weektime->wdt_ntime;
		array[i].wdt_week = wdtl->wdtl_weektime->wdt_week;
		array[i].wdt_nweek = wdtl->wdtl_weektime->wdt_nweek;
		i++;

		wdtl_prev = wdtl;
		wdtl = wdtl->wdtl_next;

		free(wdtl_prev);
	}

	*count = i;

			/* Sort the array */
	qsort((void *)array, i, sizeof(WeekDayTime), CompareWeekDayTime);

	return array;
}

static RepeatEvent *
HandleEndDate(
	RepeatEvent *re,
	time_t	     enddate)
{
	if (re) {
		if (enddate) {
			re->re_end_date = enddate;
		} else if (re->re_duration == RE_NOTSET) {
			re->re_duration = 2;
		}
	}
	return re;
}

/*
 * Create a RepeatEvent for the minute portion of a rule.
 */
static RepeatEvent *
DeriveMinuteEvent(
	unsigned int	 interval,
	unsigned int	 duration)
{
	RepeatEvent	*re;

	if (interval < 1)
		return NULL;

	re = (RepeatEvent *)calloc(1, sizeof(RepeatEvent));
				
	re->re_interval = interval;
	re->re_duration = duration;
	re->re_type = RT_MINUTE;

	return re;
}

/*
 * Create a RepeatEvent for the daily portion of a rule.
 */
static RepeatEvent *
DeriveDailyEvent(
	unsigned int	 interval,
	NumberList	*time_list,
	unsigned int	 duration,
	RepeatEvent	*other_event)
{
	RepeatEvent	*re;
	DailyData	*dd;
	NumberList	*nl;
	int		 i = 0;

	dd = (DailyData *)calloc(1, sizeof(DailyData));

		/* Convert from list to array, sort */
	dd->dd_time = (Time *)ConvertNumList(time_list, &(dd->dd_ntime));

	if (interval < 1)
		return NULL;

	re = (RepeatEvent *)calloc(1, sizeof(RepeatEvent));
				
	re->re_interval = interval;
	re->re_duration = duration;
	re->re_type = RT_DAILY;
	re->re_data.re_daily = dd;

		/* If there is a minuteEvent, tack it on */
	if (other_event) {
		re->re_next = other_event;
		other_event->re_prev = re;
	}

	return re;
}

/*
 * Create a RepeatEvent for the weekly portion of a rule.
 */
static RepeatEvent *
DeriveWeeklyEvent(
	unsigned int	 interval,
	DayTimeList	*dtl,
	unsigned int	 duration,
	RepeatEvent	*other_event)
{
	RepeatEvent	*re;
	WeeklyData	*wd;

	wd = (WeeklyData *)calloc(1, sizeof(WeeklyData));

		/* Convert from list to array, sort */
	wd->wd_daytime = ConvertDayTime(dtl, &(wd->wd_ndaytime));

	if (interval < 1)
		return NULL;

	re = (RepeatEvent *)calloc(1, sizeof(RepeatEvent));
				
	re->re_interval = interval;
	re->re_duration = duration;
	re->re_type = RT_WEEKLY;
	re->re_data.re_weekly = wd;

	if (other_event) {
		re->re_next = other_event;
		other_event->re_prev = re;
	}

	return re;
}

/*
 * Create a RepeatEvent for the monthly portion of a rule.
 */
static RepeatEvent *
DeriveMonthlyEvent(
	RepeatType	 type,
	unsigned int	 interval,
	void		*data_list,
	unsigned int	 duration,
	RepeatEvent	*other_event)
{
	RepeatEvent	*re;
	MonthlyData	*md;

	md = (MonthlyData *)calloc(1, sizeof(MonthlyData));

			/* Convert from list to array, sort */
	if (type == RT_MONTHLY_POSITION) {
		md->md_weektime = ConvertWeekDayTime(
			(WeekDayTimeList *)data_list, &(md->md_nitems));
	} else {
		md->md_days = ConvertNumList(
			(NumberList *)data_list, &(md->md_nitems));
	}

	if (interval < 1)
		return NULL;

	re = (RepeatEvent *)calloc(1, sizeof(RepeatEvent));
				
	re->re_interval = interval;
	re->re_duration = duration;
	re->re_type = type;
	re->re_data.re_monthly = md;

		/* If there is an another event, tack it on */
	if (other_event) {
		re->re_next = other_event;
		other_event->re_prev = re;
	}

	return re;
}

/*
 * Create a RepeatEvent for the yearly portion of a rule.
 */
static RepeatEvent *
DeriveYearlyEvent(
	RepeatType	 type,
	unsigned int	 interval,
	NumberList	*nl,
	unsigned int	 duration,
	RepeatEvent	*other_event)
{
	RepeatEvent	*re;
	YearlyData	*yd;

	yd = (YearlyData *)calloc(1, sizeof(YearlyData));

			/* Convert from list to array, sort */
	yd->yd_items = ConvertNumList(nl, &(yd->yd_nitems));

	if (interval < 1)
		return NULL;

	re = (RepeatEvent *)calloc(1, sizeof(RepeatEvent));
				
	re->re_interval = interval;
	re->re_duration = duration;
	re->re_type = type;
	re->re_data.re_yearly = yd;

		/* If there is an another event, tack it on */
	if (other_event) {
		re->re_next = other_event;
		other_event->re_prev = re;
	}

	return re;
}

void
yyerror(
	char 	*str)
{
	/* Don't do anything */
}
