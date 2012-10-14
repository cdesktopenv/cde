%{
/* $XConsortium: msgsets.l /main/4 1996/11/19 16:55:24 drk $ */
/*	Copyright (c) 1996 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#include <string.h>
#include <sstream>
using namespace std;

#define BUFFER_INCR_UNIT	64

ostringstream **sets;
int sets_cnt = 0; /* number of sets slots occupied */
int sets_max = 0; /* total number of sets slots */

int cur_set = -1;

int  set_num  = 0;
int* set_nums;

int* sorted;

%}

%s SETNUM MSGID

%%

^\$set	{

    if (sets_cnt == sets_max) {
	int i;

	sets_max += BUFFER_INCR_UNIT;
	if (sets_cnt == 0) {
	    sets = (ostringstream **)
			malloc(sizeof(ostringstream *) * sets_max);
	    set_nums = (int *)
			malloc(sizeof(int) * sets_max);
	}
	else {
	    sets = (ostringstream **)
			realloc(sets, sizeof(ostringstream *) * sets_max);
	    set_nums = (int *)
			realloc(set_nums, sizeof(int) * sets_max);
	}

	for (i = sets_cnt; i < sets_max; i++) {
	    sets[i] = NULL;
	    set_nums[i] = -1;
	}
    }
    if (cur_set >= 0)
	*sets[cur_set] << '\0';
    sets[cur_set = sets_cnt++] = new ostringstream;

    *sets[cur_set] << (char*)yytext;

    BEGIN SETNUM;
}

<SETNUM>[0-9]	{
    set_num = 10 * set_num + atoi((char*)yytext);

    *sets[cur_set] << *(char*)yytext;
}

<SETNUM>\n	{

    set_nums[cur_set] = set_num;
    set_num = 0;

    *sets[cur_set] << *(char*)yytext;

    BEGIN 0;
}

^[0-9]+		{
    if (cur_set < 0) { // maybe not a message id
	cout << (char*)yytext;
    }
    else { // must be a message id
	*sets[cur_set] << (char*)yytext;
	BEGIN MSGID;
    }
}

<MSGID>[ \t]+	{
    // supress multiple white spaces between msg# and message
    *sets[cur_set] << ' ';
    BEGIN 0;
}

<MSGID>[^ \t]	{
    *sets[cur_set] << *(char*)yytext;
    BEGIN 0;
}

.	|
\n	{
    if (cur_set < 0)
	cout << *(char*)yytext;
    else
	*sets[cur_set] << *(char*)yytext;
}

<<EOF>> {
    if (cur_set >= 0)
	*sets[cur_set] << '\0';
    return 1;
}

%%

main()
{
    int i,j;

    yylex();

    sorted = (int *)malloc(sizeof(int) * sets_cnt);

    for (i = 0; i < sets_cnt; i++)
	sorted[i] = i;

    for (i = 0; i < sets_cnt - 1; i++) {
	int idx = i;
	for (j = i + 1; j < sets_cnt; j++) {
	    if (set_nums[sorted[idx]] > set_nums[sorted[j]])
		idx = j;
	}
	if (i < idx) {
	    int saved = sorted[i];
	    sorted[i] = sorted[idx];
	    sorted[idx] = saved;
	}
    }

    for (i = 0; i < sets_cnt; i++) {
	const char* record = sets[sorted[i]]->str().c_str();
	cout << record << '\n' << flush;
    }

    free(sorted);
    free(set_nums);
    for (i = 0; i < sets_cnt; i++)
	delete sets[i];
    free(sets);

    return 0;
}

