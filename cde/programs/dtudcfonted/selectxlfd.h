/* selectxlfd.h 1.1 - Fujitsu source for CDEnext    96/01/06 16:57:23      */
/* $XConsortium: selectxlfd.h /main/3 1996/04/08 16:02:27 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#define BUTTONITEMS     4
#define PUSHBUTTONS     2
#define COPYLABELS	2

#define CX		12
#define CY		-4

#define XLFD_COLUMNS	38
#define KEY_COLUMNS	15

#define PIXELSIZE	7

/*
 *	put data from resource database
 */
typedef struct _Rsrc {
	char	* xlfd_label ;      
	char	* copy_xlfd_label ;      
	char	* code_label ;      
	char	* style_label ;      
	char	* size_label ;       
	char	* exec_label ;     
	char	* quit_label ;     
	char	* ok_label ;       
	char	* cancel_label ;   
	char	* copy_orgin ;      
	char	* copy_target ;      
	char	* copy_label ;   
	char	* overlay_label ;   
} Rsrc ;
/***************< end of selectxlfd.h >***************/
