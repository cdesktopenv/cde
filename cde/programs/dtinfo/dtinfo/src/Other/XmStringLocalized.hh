/*	Copyright (c) 1996 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __XM_STRING_LOCALIZED_HH__
#define __XM_STRING_LOCALIZED_HH__

#include <Xm/Xm.h>

class XmStringLocalized
{
public:
    XmStringLocalized(char* = NULL);
    ~XmStringLocalized();

    // conv operator
    operator XmString () { return f_string; }

    XmStringLocalized& operator= (char*);

private:
    XmString f_string;
};

inline
XmStringLocalized::XmStringLocalized(char* string) : f_string(NULL)
{
    if (string)
        f_string = XmStringCreateLocalized(string);
}

inline
XmStringLocalized::~XmStringLocalized()
{
    if (f_string) {
        XmStringFree(f_string);
        f_string = NULL;
    }   
}

inline XmStringLocalized&
XmStringLocalized::operator = (char* string)
{
    if (f_string) {
	XmStringFree(f_string);
	f_string = NULL;
    }
    if (string)
	f_string = XmStringCreateLocalized(string);

    return *this;
}

#endif
