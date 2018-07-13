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
/* $XConsortium: rtable3.c /main/4 1995/11/09 12:51:59 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*
  veneer layered on top of the real data structures for abstraction.
	implements Version 3 in terms of Version 4 types	
 */



#include <EUSCompat.h>
#include <stdio.h>
#include "rtable4.h"
#include "rtable3.h"
#include <sys/param.h>
#include <sys/time.h>
#include <rpc/rpc.h>
#include "rpcextras.h"
#include "convert3-4.h"
#include "convert4-3.h"
#include "rtable3_tbl.i"


/*************** V3 PROTOCOL IMPLEMENTATION PROCS *****************/
extern void *
_DtCm_rtable_ping_3_svc(void *args, struct svc_req *svcrq)
{
	return(NULL); /* for RPC reply */
}

/*	PROC #1		*/
extern Table_Res_3 *
_DtCm_rtable_lookup_3_svc (Table_Args_3 *args, struct svc_req *svcrq)
{
	static Table_Res_3 *res = NULL;
	Table_Args_4 *newargs;
	Table_Res_4 *newres;

	if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

	newargs = _DtCm_tableargs3_to_tableargs4(args);
	if(newargs) {
		newres = _DtCm_rtable_lookup_4_svc(newargs, svcrq);
		res = _DtCm_tableres4_to_tableres3(newres);

		xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
	}

	return(res);
}

/*	PROC #2		*/
extern Table_Res_3 *
_DtCm_rtable_lookup_next_larger_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
	static Table_Res_3 *res = NULL;
	Table_Args_4 *newargs;   
	Table_Res_4 *newres; 
 
	if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;
 
	newargs = _DtCm_tableargs3_to_tableargs4(args);
	if(newargs) {
		newres = _DtCm_rtable_lookup_next_larger_4_svc(newargs, svcrq);
		res = _DtCm_tableres4_to_tableres3(newres);
 
		xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
	}
 
	return(res);
}

/*	PROC #3		*/
extern Table_Res_3 *
_DtCm_rtable_lookup_next_smaller_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
	static Table_Res_3 *res = NULL; 
	Table_Args_4 *newargs;    
	Table_Res_4 *newres; 
  
	if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

	newargs = _DtCm_tableargs3_to_tableargs4(args);
	if(newargs) {
		newres = _DtCm_rtable_lookup_next_smaller_4_svc(newargs, svcrq);
		res = _DtCm_tableres4_to_tableres3(newres);

		xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
	}

	return(res);
}

/*	PROC #4		*/
extern Table_Res_3 *
_DtCm_rtable_lookup_range_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL;  
        Table_Args_4 *newargs;     
        Table_Res_4 *newres; 
   
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

        newargs = _DtCm_tableargs3_to_tableargs4(args); 
        if(newargs) {
                newres = _DtCm_rtable_lookup_range_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res);
}

/*	PROC #5		*/
extern Table_Res_3 *
_DtCm_rtable_abbreviated_lookup_range_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL;   
        Table_Args_4 *newargs;      
        Table_Res_4 *newres; 

        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

        newargs = _DtCm_tableargs3_to_tableargs4(args);  
        if(newargs) {
                newres = _DtCm_rtable_abbreviated_lookup_range_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res);
}

/*	PROC #6		*/
extern Table_Res_3 *
_DtCm_rtable_insert_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL;
        Table_Args_4 *newargs;
        Table_Res_4 *newres;

        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

        newargs = _DtCm_tableargs3_to_tableargs4(args);
        if(newargs) {
                newres = _DtCm_rtable_insert_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res);
}

/*	PROC #7	*/
extern Table_Res_3 *
_DtCm_rtable_delete_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL; 
        Table_Args_4 *newargs;
        Table_Res_4 *newres; 

        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

	newargs = _DtCm_tabledelargs3_to_tabledelargs4(args, do_all_4);
        if(newargs) {
                newres = _DtCm_rtable_delete_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res); 
}

/*	PROC #8		*/
extern Table_Res_3 *
_DtCm_rtable_delete_instance_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL; 
        Table_Args_4 *newargs;  
        Table_Res_4 *newres;  

        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

	newargs = _DtCm_tabledelargs3_to_tabledelargs4(args, do_one_4);
        if(newargs) {
                newres = _DtCm_rtable_delete_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res);
}

/*	PROC #9	*/
extern Table_Res_3 *
_DtCm_rtable_change_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL;
        Table_Args_4 *newargs; 
        Table_Res_4 *newres;  

        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL; 

        newargs = _DtCm_tableargs3_to_tableargs4(args);
        if(newargs) {
                newres = _DtCm_rtable_change_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res); 
}


/*	PROC #10	*/
extern Table_Res_3 *
_DtCm_rtable_change_instance_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL; 
        Table_Args_4 *newargs;   
        Table_Res_4 *newres;  

        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

        newargs = _DtCm_tableargs3_to_tableargs4(args);
        if(newargs) {
                newargs->args.Args_4_u.apptid.option = do_one_4;
                newres = _DtCm_rtable_change_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res);
}

/*	PROC #11	*/
extern Table_Res_3 *
_DtCm_rtable_lookup_next_reminder_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL; 
        Table_Args_4 *newargs;    
        Table_Res_4 *newres;   

        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

        newargs = _DtCm_tableargs3_to_tableargs4(args);
        if(newargs) {
                newres = _DtCm_rtable_lookup_next_reminder_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(res); 
}

/*	PROC #12	*/
extern Table_Status_3 *
_DtCm_rtable_check_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Status_3 res; 
        Table_Args_4 *newargs;     
        Table_Status_4 *newres;    

        res = 0;

        newargs = _DtCm_tableargs3_to_tableargs4(args);    
        if(newargs) {
                newres = _DtCm_rtable_check_4_svc(newargs, svcrq);
                res = _DtCm_tablestat4_to_tablestat3(*newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(&res); 
}

/*	PROC #13	*/
extern Table_Status_3 *
_DtCm_rtable_flush_table_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Status_3 res;  
        Table_Args_4 *newargs;     
        Table_Status_4 *newres;     

        res = 0;

        newargs = _DtCm_tableargs3_to_tableargs4(args);     
        newres = _DtCm_rtable_flush_table_4_svc(newargs, svcrq);     
        res = _DtCm_tablestat4_to_tablestat3(*newres);    

        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);      

        return(&res);
}

/*	PROC #14	*/
extern int *
_DtCm_rtable_size_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static int size;   
        Table_Args_4 *newargs;      

        size = 0;

        newargs = _DtCm_tableargs3_to_tableargs4(args);
        if(newargs) {
                size = (*(_DtCm_rtable_size_4_svc(newargs, svcrq)));

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }

        return(&size);
}

/*	PROC #15	*/
Registration_Status_3 *
_DtCm_register_callback_3_svc(Registration_3 *r, struct svc_req *svcrq)
{
        static Registration_Status_3 stat;    
        Registration_4 *newreg;       
	Registration_Status_4 *newstat;

        stat = 0;

        newreg = _DtCm_reg3_to_reg4(r);
        if(newreg) {
                newstat = _DtCm_register_callback_4_svc(newreg, svcrq);
                stat = _DtCm_regstat4_to_regstat3(*newstat);

                xdr_free((xdrproc_t)_DtCm_xdr_Registration_4, (char*)newreg);
        }
	return(&stat);
}

/*	PROC #16	*/
Registration_Status_3 *
_DtCm_deregister_callback_3_svc(Registration_3 *r, struct svc_req *svcrq)
{
        static Registration_Status_3 stat;     
        Registration_4 *newreg;          
        Registration_Status_4 *newstat; 

        stat = 0;

        newreg = _DtCm_reg3_to_reg4(r);
        if(newreg) {
                newstat = _DtCm_deregister_callback_4_svc(newreg, svcrq);
                stat = _DtCm_regstat4_to_regstat3(*newstat);

                xdr_free((xdrproc_t)_DtCm_xdr_Registration_4, (char*)newreg);
        }
        return(&stat);
}

/*	PROC #17	*/
extern Access_Status_3 *
_DtCm_rtable_set_access_3_svc(Access_Args_3 *args, struct svc_req *svcrq)
{
	static Access_Status_3 stat;
	Access_Args_4 *newargs;
	Access_Status_4 *newstat;

        stat = 0;

        newargs = _DtCm_accargs3_to_accargs4(args);
        if(newargs) {
                newstat = _DtCm_rtable_set_access_4_svc(newargs, svcrq);
                stat = _DtCm_accstat4_to_accstat3(*newstat);

                xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_4, (char*)newargs);
        }
	return(&stat);
}

/*	PROC #18	*/
extern Access_Args_3 *
_DtCm_rtable_get_access_3_svc(Access_Args_3 *args, struct svc_req *svcrq)
{
        static Access_Args_3 *res = NULL;
        Access_Args_4 *newargs;
        Access_Args_4 *newres;

	if (res!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_3, (char*)res); res = NULL;

        newargs = _DtCm_accargs3_to_accargs4(args);
        if(newargs) {
                newres = _DtCm_rtable_get_access_4_svc(newargs, svcrq);
                res = _DtCm_accargs4_to_accargs3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_4, (char*)newargs);
        }
        return(res); 
}

/*	PROC #19	*/
extern Table_Res_3 *
_DtCm_rtable_abbreviated_lookup_key_range_3_svc(Table_Args_3 *args, struct svc_req *svcrq)
{
        static Table_Res_3 *res = NULL;
        Table_Args_4 *newargs;   
        Table_Res_4 *newres;  

	if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res); res = NULL;

        newargs = _DtCm_tableargs3_to_tableargs4(args);
        if(newargs) {
                newres = _DtCm_rtable_abbreviated_lookup_key_range_4_svc(newargs, svcrq);
                res = _DtCm_tableres4_to_tableres3(newres);

                xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
        }
        return(res); 
}

/*	PROC #20	*/
extern long *
_DtCm_rtable_gmtoff_3_svc(void *args, struct svc_req *svcrq)
{
	static long gmtoff;

        gmtoff = (*(_DtCm_rtable_gmtoff_4_svc(NULL, svcrq)));

	return(&gmtoff);
}

void initrtable3(program_handle ph)
{
        ph->program_num = TABLEPROG;
        ph->prog[TABLEVERS_3].vers = &tableprog_3_table[0];
        ph->prog[TABLEVERS_3].nproc = sizeof(tableprog_3_table)/sizeof(tableprog_3_table[0]);
}

