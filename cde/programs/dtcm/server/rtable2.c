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
/* $XConsortium: rtable2.c /main/4 1995/11/09 12:51:38 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*
  veneer layered on top of the real data structures for abstraction.
	implements Version 2 in terms of Version 4 types	
 */



#include <EUSCompat.h>
#include <stdio.h>
#include "rtable4.h"
#include "rtable2.h"
#include <sys/param.h>
#include <sys/time.h>
#include <rpc/rpc.h>
#include "rpcextras.h"
#include "convert2-4.h"
#include "convert4-2.h"
#include "rtable2_tbl.i"


/*************** V2 PROTOCOL IMPLEMENTATION PROCS *****************/
extern void *
_DtCm_rtable_ping_2_svc(args, svcrq)
void *args;
struct svc_req *svcrq;
{
	return(NULL); /* for RPC reply */
}

/*	PROC #1		*/
extern Table_Res_2 *
_DtCm_rtable_lookup_2_svc (args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
	static Table_Res_2 *res = NULL;
	Table_Args_4 *newargs;
	Table_Res_4 *newres;

	if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res);

	newargs = _DtCm_tableargs2_to_tableargs4(args);
	newres = _DtCm_rtable_lookup_4_svc(newargs, svcrq); 
	
	res = _DtCm_tableres4_to_tableres2(newres);

	if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);

	return(res);
}

/*	PROC #2		*/
extern Table_Res_2 *
_DtCm_rtable_lookup_next_larger_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL;
        Table_Args_4 *newargs;   
        Table_Res_4 *newres; 
 
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res);
 
        newargs = _DtCm_tableargs2_to_tableargs4(args);
        newres = _DtCm_rtable_lookup_next_larger_4_svc(newargs, svcrq);
        
        res = _DtCm_tableres4_to_tableres2(newres);
 
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
 
        return(res);

}

/*	PROC #3		*/
extern Table_Res_2 *
_DtCm_rtable_lookup_next_smaller_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL; 
        Table_Args_4 *newargs;    
        Table_Res_4 *newres; 
  
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res);
  
        newargs = _DtCm_tableargs2_to_tableargs4(args);
        newres = _DtCm_rtable_lookup_next_smaller_4_svc(newargs, svcrq); 
         
        res = _DtCm_tableres4_to_tableres2(newres);
  
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
 
        return(res);

}

/*	PROC #4		*/
extern Table_Res_2 *
_DtCm_rtable_lookup_range_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL;  
        Table_Args_4 *newargs;     
        Table_Res_4 *newres; 
   
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res);
   
        newargs = _DtCm_tableargs2_to_tableargs4(args); 
        newres = _DtCm_rtable_lookup_range_4_svc(newargs, svcrq);  
          
        res = _DtCm_tableres4_to_tableres2(newres); 
   
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs); 
   
        return(res);
}

/*	PROC #5		*/
extern Table_Res_2 *
_DtCm_rtable_abbreviated_lookup_range_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL;   
        Table_Args_4 *newargs;      
        Table_Res_4 *newres; 
    
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res); 
   
        newargs = _DtCm_tableargs2_to_tableargs4(args);  
        newres = _DtCm_rtable_abbreviated_lookup_range_4_svc(newargs, svcrq);  
           
        res = _DtCm_tableres4_to_tableres2(newres);  
   
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);  
    
        return(res);
}

/*	PROC #6		*/
extern Table_Res_2 *
_DtCm_rtable_insert_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL;
        Table_Args_4 *newargs;
        Table_Res_4 *newres;
    
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res);
  
        newargs = _DtCm_tableargs2_to_tableargs4(args);
        newres = _DtCm_rtable_insert_4_svc(newargs, svcrq);
         
        res = _DtCm_tableres4_to_tableres2(newres);
  
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);
    
        return(res);
}

/*	PROC #7	*/
extern Table_Res_2 *
_DtCm_rtable_delete_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL; 
        Table_Args_4 *newargs;
        Table_Res_4 *newres; 
     
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res);
   
	newargs = _DtCm_tabledelargs2_to_tabledelargs4(args, do_all_4);
        newres = _DtCm_rtable_delete_4_svc(newargs, svcrq); 
        res = _DtCm_tableres4_to_tableres2(newres); 
  
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs); 
     
        return(res); 
}

/*	PROC #8		*/
extern Table_Res_2 *
_DtCm_rtable_delete_instance_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL; 
        Table_Args_4 *newargs;  
        Table_Res_4 *newres;  
       
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res); 

	newargs = _DtCm_tabledelargs2_to_tabledelargs4(args, do_one_4);
	newres = _DtCm_rtable_delete_4_svc(newargs, svcrq);  
        res = _DtCm_tableres4_to_tableres2(newres);   
   
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);   
       
        return(res);
}

/*	PROC #9	*/
extern Table_Res_2 *
_DtCm_rtable_change_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL;
        Table_Args_4 *newargs; 
        Table_Res_4 *newres;  
      
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res); 
    
        newargs = _DtCm_tableargs2_to_tableargs4(args); 
        newres = _DtCm_rtable_change_4_svc(newargs, svcrq); 
        res = _DtCm_tableres4_to_tableres2(newres);  
  
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);  
      
        return(res); 
}


/*	PROC #10	*/
extern Table_Res_2 *
_DtCm_rtable_change_instance_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL; 
        Table_Args_4 *newargs;   
        Table_Res_4 *newres;  
       
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res); 
     
        newargs = _DtCm_tableargs2_to_tableargs4(args);  
	newargs->args.Args_4_u.apptid.option = do_one_4;
        newres = _DtCm_rtable_change_4_svc(newargs, svcrq);  
        res = _DtCm_tableres4_to_tableres2(newres);   
   
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);   
       
        return(res);
}

/*	PROC #11	*/
extern Table_Res_2 *
_DtCm_rtable_lookup_next_reminder_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Res_2 *res = NULL; 
        Table_Args_4 *newargs;    
        Table_Res_4 *newres;   
        
        if (res!=NULL) xdr_free ((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res);  
      
        newargs = _DtCm_tableargs2_to_tableargs4(args);   
        newres = _DtCm_rtable_lookup_next_reminder_4_svc(newargs, svcrq);   
             
        res = _DtCm_tableres4_to_tableres2(newres);   
    
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);    
        
        return(res); 
}

/*	PROC #12	*/
extern Table_Status_2 *
_DtCm_rtable_check_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Status_2 res; 
        Table_Args_4 *newargs;     
        Table_Status_4 *newres;    
       
        newargs = _DtCm_tableargs2_to_tableargs4(args);    
        newres = _DtCm_rtable_check_4_svc(newargs, svcrq);    
              
        res = _DtCm_tablestat4_to_tablestat2(*newres);   
     
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);     
         
        return(&res); 
}

/*	PROC #13	*/
extern Table_Status_2 *
_DtCm_rtable_flush_table_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static Table_Status_2 res;  
        Table_Args_4 *newargs;     
        Table_Status_4 *newres;     
        
        newargs = _DtCm_tableargs2_to_tableargs4(args);     
        newres = _DtCm_rtable_flush_table_4_svc(newargs, svcrq);     
               
        res = _DtCm_tablestat4_to_tablestat2(*newres);    
     
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);      
          
        return(&res);
}

/*	PROC #14	*/
extern int *
_DtCm_rtable_size_2_svc(args, svcrq)
Table_Args_2 *args;
struct svc_req *svcrq;
{
        static int size;   
        Table_Args_4 *newargs;      
         
        newargs = _DtCm_tableargs2_to_tableargs4(args);     
        size = (*(_DtCm_rtable_size_4_svc(newargs, svcrq)));      
      
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Table_Args_4, (char*)newargs);       
           
        return(&size);
}

/*	PROC #15	*/
Registration_Status_2 *
_DtCm_register_callback_2_svc(r, svcrq)
Registration_2 *r;
struct svc_req *svcrq;
{
        static Registration_Status_2 stat;    
        Registration_4 *newreg;       
	Registration_Status_4 *newstat;
          
        newreg = _DtCm_reg2_to_reg4(r);      
        newstat = _DtCm_register_callback_4_svc(newreg, svcrq);      
                 
        stat = _DtCm_regstat4_to_regstat2(*newstat);      
      
        if (newreg!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Registration_4, (char*)newreg);        
	return(&stat);
}

/*	PROC #16	*/
Registration_Status_2 *
_DtCm_deregister_callback_2_svc(r, svcrq)
Registration_2 *r;
struct svc_req *svcrq;
{
        static Registration_Status_2 stat;     
        Registration_4 *newreg;          
        Registration_Status_4 *newstat; 
           
        newreg = _DtCm_reg2_to_reg4(r);       
        newstat = _DtCm_deregister_callback_4_svc(newreg, svcrq);       
                  
        stat = _DtCm_regstat4_to_regstat2(*newstat);       
       
        if (newreg!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Registration_4, (char*)newreg);        
        return(&stat);
}

/*	PROC #17	*/
extern Access_Status_2 *
_DtCm_rtable_set_access_2_svc(args, svcrq)
Access_Args_2 *args;
struct svc_req *svcrq;
{
	static Access_Status_2 stat;
	Access_Args_4 *newargs;
	Access_Status_4 *newstat;
        
        newargs = _DtCm_accargs2_to_accargs4(args);     
        newstat = _DtCm_rtable_set_access_4_svc(newargs, svcrq);     
               
        stat = _DtCm_accstat4_to_accstat2(*newstat);    
     
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_4, (char*)newargs);      
	return(&stat);
}

/*	PROC #18	*/
extern Access_Args_2 *
_DtCm_rtable_get_access_2_svc(args, svcrq)
Access_Args_2 *args;
struct svc_req *svcrq;
{
        static Access_Args_2 *res = NULL;
        Access_Args_4 *newargs;
        Access_Args_4 *newres;
        
	if (res!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_2, (char*)res);

        newargs = _DtCm_accargs2_to_accargs4(args);
        newres = _DtCm_rtable_get_access_4_svc(newargs, svcrq);
               
        res = _DtCm_accargs4_to_accargs2(newres);
        
        if (newargs!=NULL) xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_4, (char*)newargs); 

        return(res); 
}

void initrtable2(ph)
        program_handle ph;
{
	int ver = TABLEVERS_2;
        ph->program_num = TABLEPROG;
        ph->prog[TABLEVERS_2].vers = &tableprog_2_table[0];
        ph->prog[TABLEVERS_2].nproc = sizeof(tableprog_2_table)/sizeof(tableprog_2_table[0]);
}

