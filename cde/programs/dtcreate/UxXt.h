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
/* @(#)95       1.3  com/config/UxXt.h, aic, aic324, 9322324a 6/1/93 16:14:00 */
/*
 *  COMPONENT_NAME: AIC           AIXwindows Interface Composer
 *
 *  ORIGINS: 58
 *
 *
 *                   Copyright IBM Corporation 1991, 1993
 *
 *                         All Rights Reserved
 *
 *   Permission to use, copy, modify, and distribute this software and its
 *   documentation for any purpose and without fee is hereby granted,
 *   provided that the above copyright notice appear in all copies and that
 *   both that copyright notice and this permission notice appear in
 *   supporting documentation, and that the name of IBM not be
 *   used in advertising or publicity pertaining to distribution of the
 *   software without specific, written prior permission.
 *
 *   IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *   PURPOSE. IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 *   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 *   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 *   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 *   OR PERFORMANCE OF THIS SOFTWARE.
*/
/*---------------------------------------------------------------------
 * $XConsortium: UxXt.h /main/4 1995/11/01 16:09:11 rswiston $
 *---------------------------------------------------------------------
 *
 *
 *             Copyright (c) 1991, Visual Edge Software Ltd.
 *
 * ALL  RIGHTS  RESERVED.  Permission  to  use,  copy,  modify,  and
 * distribute  this  software  and its documentation for any purpose
 * and  without  fee  is  hereby  granted,  provided  that the above
 * copyright  notice  appear  in  all  copies  and  that  both  that
 * copyright  notice and this permission notice appear in supporting
 * documentation,  and that  the name of Visual Edge Software not be
 * used  in advertising  or publicity  pertaining to distribution of
 * the software without specific, written prior permission. The year
 * included in the notice is the year of the creation of the work.
 *-------------------------------------------------------------------*/

/*****************************************************************************/
/*                              UxXt.h                                       */
/*****************************************************************************/

#ifndef _UX_XT_H_INCLUDED
#define _UX_XT_H_INCLUDED

#include <Xm/Xm.h>

#ifdef UIL_CODE
#include <Mrm/MrmPublic.h>
#endif /* UIL_CODE */

#if 0
#ifdef __STDC__
typedef char *caddr_t;
#endif
#endif

/*-----------------------------------------------------
 * UXORB_HEADER, if defined, is the include form for
 * the header that defines the CORBA Environment type
 * and exception type codes.
 *
 * You can specify a file with a compile option like
 *      -DUXORB_HEADER='<SomeOrb.h>'
 *-----------------------------------------------------*/
#ifdef UXORB_HEADER
#include UXORB_HEADER
#else
        /*
         * In the absence of an ORB implementation,
         * these minimal definitions satisfy our method dispatch code.
         */
        typedef enum {
                NO_EXCEPTION,
                USER_EXCEPTION,
                SYSTEM_EXCEPTION
        } exception_type;

        typedef struct Environment {
                exception_type  _major;
        } Environment;
#endif  /* UXORB_HEADER */

/*
 * UxEnv is provided as a convenience for use in interface methods.
 */
extern  Environment     UxEnv;


/* The following macros are used in converting string values to the form
   required by the widgets */

#define RES_CONVERT( res_name, res_value) \
        XtVaTypedArg, (res_name), XmRString, (res_value), strlen(res_value) + 1

#define UxPutStrRes( wgt, res_name, res_value ) \
        XtVaSetValues( wgt, RES_CONVERT( res_name, res_value ), NULL )


#ifndef UX_INTERPRETER  /* Omit this section when interpreting the code */

/* The following macros are supplied for compatibility with swidget code */
#define swidget                 Widget
#define UxWidgetToSwidget(w)    (w)
#define UxGetWidget(sw)         (sw)
#define UxIsValidSwidget(sw)    ((sw) != NULL)
#define NO_PARENT               ((Widget) NULL)
#define UxThisWidget            (UxWidget)

/* Macros needed for the method support code */
#define UxMalloc(a)             (malloc(a))
#define UxRealloc(a,b)          (realloc((a), (b)))
#define UxCalloc(a,b)           (calloc((a), (b)))
#define UxStrEqual(a,b)         (!strcmp((a),(b)))
#define UxGetParent(a)          (XtParent((a)))

#define no_grab                 XtGrabNone
#define nonexclusive_grab       XtGrabNonexclusive
#define exclusive_grab          XtGrabExclusive


/* The following global variables are defined in the main() function */
extern  XtAppContext    UxAppContext;
extern  Widget          UxTopLevel;
extern  Display         *UxDisplay;
extern  int             UxScreen;


/* The following are error codes returned by the functions in UxXt.c */
#define UX_ERROR           -1
#define UX_NO_ERROR        0

#ifdef UIL_CODE
extern  void            UxMrmFetchError(MrmHierarchy, char *, Widget, Cardinal);
extern  MrmHierarchy    UxMrmOpenHierarchy( char *);
extern  void            UxMrmRegisterClass( char *, Widget (*)(Widget, String, Arg *, Cardinal));
#endif /* UIL_CODE */



/* The following are declarations of the functions in UxXt.c */


extern  void*		UxNewContext (size_t size, int isSubclass);
extern  int             UxPopupInterface( Widget wgt, XtGrabKind grab_flag );
extern  int             UxPopdownInterface( Widget wgt );
extern  int             UxDestroyInterface( Widget wgt);
extern  int             UxPutContext( Widget wgt, caddr_t context );
extern  caddr_t         UxGetContext( Widget wgt );
extern  void            UxFreeClientDataCB( Widget wgt, XtPointer client_data,
                                                 XtPointer call_data );
extern  void            UxLoadResources( char *fname );
extern  XmFontList      UxConvertFontList( char *fontlist_str );
extern  Pixmap          UxConvertPixmap( char *file_name );
extern  Pixmap          UxConvert_bitmap( char *file_name );
extern  wchar_t *       UxConvertValueWcs( char *value_str );

extern  void            UxDestroyContextCB(Widget, XtPointer, XtPointer);
extern  void            UxDeleteContextCB( Widget, XtPointer, XtPointer);
extern  XtArgVal        UxRemoveValueFromArgList( Arg *args,
                                                Cardinal *ptr_num_args,
                                                String res_name );
extern  Widget          UxChildSite( Widget );
extern  Widget          UxRealWidget( Widget );

extern  Widget          GetTrueToplevel( Widget );


#ifdef __cplusplus
class _UxCInterface {

public:

        virtual swidget childSite (Environment * pEnv) {
                if (pEnv)
                        pEnv->_major = NO_EXCEPTION;
                return 0;
        }
        virtual swidget UxChildSite (swidget sw);

protected:
        swidget UxThis;
};

#define CPLUS_ADAPT_CONTEXT(CLASS) \
        static inline \
                CLASS* UxGetContext(CLASS*self) {return self;} \
        static inline\
                void* UxGetContext(swidget any) {return ::UxGetContext(any);}

#endif /* _cplusplus */

#endif /* ! UX_INTERPRETER */

#endif /* ! _UX_XT_H_INCLUDED */

