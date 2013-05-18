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
/* $XConsortium: sfvprintf.c /main/3 1995/11/01 18:38:26 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*         THIS IS PROPRIETARY SOURCE CODE LICENSED BY          *
*                          AT&T CORP.                          *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*                     All Rights Reserved                      *
*                                                              *
*           This software is licensed by AT&T Corp.            *
*       under the terms and conditions of the license in       *
*       http://www.research.att.com/orgs/ssr/book/reuse        *
*                                                              *
*               This software was created by the               *
*           Software Engineering Research Department           *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                     gsf@research.att.com                     *
*                                                              *
***************************************************************/
#include	"sfhdr.h"

/*	The engine for formatting data
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#ifdef ulong_hibit
#define HIGHBIT		ulong_hibit
#else
#define HIGHBIT		(~(((ulong)~0L) >> 1))
#endif

#define F_LEFT		000001	/* left justification (-)		*/
#define F_SIGN		000002	/* must set a sign - or +		*/
#define F_BLANK		000004	/* if not - and +, then prepend a blank */
#define F_ZERO		000010	/* zero padding on the left side	*/
#define F_ALTER		000020	/* various alternative formats (#)	*/
#define F_PAD		000040	/* there will be some sort of padding	*/

#define F_REPEAT	000100	/* repeat pattern up to precision	*/
#define F_MINUS		000200	/* has a minus sign			*/
#define F_PREFIX	(F_MINUS|F_SIGN|F_BLANK)

#define F_LONG		001000	/* object is long			*/
#define F_FLOAT		002000	/* %fFeEgG format			*/
#define F_GFORMAT	004000	/* a %gG format				*/
#define F_LDOUBLE	010000	/* object is long double		*/

#define FPRECIS		6	/* default precision for floats 	*/

/* elt:		element to be assigned value.
** arge:	if argf is used, &arge can be passed on to argf to get value.
** argf:	function to get argument if any.
** args:	is the va_list being processed.
** etype:	The type of the element.
** type:	The type of the object (int, double, ...) being gotten out of args.
** fmt:		the format character.
** t_user,n_user: stuff between parens.
*/
#define GETARG(elt,arge,argf,args,etype,type,fmt,t_user,n_user) \
	{ if(!argf) \
		elt = (etype)va_arg(args,type); \
	  else if((*argf)(fmt,(char*)(&arge),t_user,n_user) < 0) \
		goto pop_fa; \
	  else	elt = (etype)arge; \
	}
#define GETARGL(elt,arge,argf,args,etype,type,fmt,t_user,n_user) \
	{ if(!argf) \
		__va_copy( elt, va_arg(args,type) ); \
	  else if((*argf)(fmt,(char*)(&arge),t_user,n_user) < 0) \
		goto pop_fa; \
	  else	__va_copy( elt, arge ); \
	}

#if __STD_C
sfvprintf(Sfio_t* f, const char* form, va_list args)
#else
sfvprintf(f,form,args)
Sfio_t*	f;		/* file to print to	*/
char*	form;		/* format to use	*/
va_list	args;		/* arg list if !argf	*/
#endif
{
	reg long	n, lval, base;
	reg char	*sp, *ssp, *d;
	reg long	v;
	reg int		flags;
	reg char	*ep, *endep, *endsp, *endd;
	reg int		precis, width, n_output, r;
	int		fmt, sign, decpt, dot;
	Double_t	dval;	/* could be long double */
	ulong		along, *alp;
	uint		aint, *aip;
	uchar		achar;
	char*		astr;
	Argf_p		argf;
	Extf_p		extf;
	va_list*	argsp;
	reg Fa_t	*fa, *fast;
	char		buf[SF_MAXDIGITS];
	char		data[SF_GRAIN];
	char*		t_user;	/* stuff between ()	*/
	int		n_user;	/* its length		*/
#if _lib_locale
	int		dc = 0;
	struct lconv*	lv;
#endif

	/* fast io system */
#define SFBUF(f)	(d = (char*)f->next, endd = (char*)f->endb)
#define SFINIT(f)	(SFBUF(f), n_output = 0)
#define SFEND(f)	((n_output += (uchar*)d - f->next), (f->next = (uchar*)d))
#define SFputc(f,c) \
	{ if(d >= endd) \
		{ SFEND(f); if(SFFLSBUF(f,c) <  0) break; n_output += 1; SFBUF(f); } \
	  else	{ *d++ = (char)c; } \
	}
#define SFnputc(f,c,n) \
	{ if((endd-d) < n) \
		{ SFEND(f); if(SFNPUTC(f,(int)c,(int)n) != n) break; \
		  n_output += (int)n; SFBUF(f); } \
	  else	{ while(n--) *d++ = (char)c; } \
	}
#define SFwrite(f,s,n) \
	{ if((endd-d) < n) \
		{ SFEND(f); if(SFWRITE(f,(Void_t*)s,(int)n) != n) break; \
		  n_output += (int)n; SFBUF(f); } \
	  else	MEMCPY(d,s,(int)n); \
	}

	/* make sure stream is in write mode and buffer is not NULL */
	if(f->mode != SF_WRITE && _sfmode(f,SF_WRITE,0) < 0)
		return -1;

	SFLOCK(f,0);

	if(!f->data )
	{	f->data = f->next = (uchar*)data;
		f->endw = f->endb = f->data+sizeof(data);
	}
	SFINIT(f);

	lval = 0;
	precis = 0;
	ep = endep = NIL(char*);
	argf = NIL(Argf_p);
	extf = NIL(Extf_p);
	fast = NIL(Fa_t*);

loop_fa :
	while((n = *form++) )
	{
		flags = 0;
		if(n != '%')
		{	/* collect the non-pattern chars */
			sp = ssp = (char*)(form-1);
			while((n = *++ssp) && n != '%')
				;
			form = endsp = ssp;
			goto do_output;
		}

		t_user = NIL(char*);
		n_user = 0;
		endep = ep = NIL(char*);
		endsp = sp = buf+(sizeof(buf)-1);
		width = precis = -1;
		dot = 0;
		base = 10;

	loop_flags:	/* LOOP FOR FLAGS, WIDTH AND PRECISION */
#define LEFTP	'('
#define RIGHTP	')'
		switch(fmt = *form++)
		{
		case LEFTP : /* get the type which is enclosed in balanced () */
			t_user = (char*)form;
			for(aint = 1;;)
			{	switch(*form++)
				{
				case 0 :	/* not balancable, retract */
					form = t_user;
					t_user = NIL(char*);
					n_user = 0;
					goto loop_flags;
				case LEFTP :	/* increasing nested level */
					aint += 1;
					continue;
				case RIGHTP :	/* decreasing nested level */
					if((aint -= 1) != 0)
						continue;
					n_user = (form-1) - t_user;
					goto loop_flags;
				}
			}

		case '-' :
			flags |= F_LEFT;
			goto loop_flags;
		case ' ' :
			flags |= F_BLANK;
			goto loop_flags;
		case '+' :
			flags |= F_SIGN;
			goto loop_flags;
		case '#' :
			flags |= F_ALTER;
			goto loop_flags;
		case '.' :	/* argument count */
			if((dot += 1) > 2)
			{	form -= 1;
				continue;
			}
			goto loop_flags;
		case '*' :	/* variable width, precision, or base */
			if((dot == 0 && width >= 0) || (dot == 1 && precis >= 0) )
			{	form -= 1;	/* bad pattern specification */
				continue;
			}
			GETARG(lval,aint,argf,args,long,uint,'d',t_user,n_user);
			goto set_args;
		case '0' :	/* defining width or precision */
			if(dot == 0)
			{	flags |= F_ZERO;
				goto loop_flags;
			}
			/* fall thru */
		case '1' : case '2' : case '3' :
		case '4' : case '5' : case '6' :
		case '7' : case '8' : case '9' :
			lval = fmt - '0';
			for(n = *form; isdigit(n); n = *++form)
				lval = (lval<<3) + (lval<<1) + (n - '0');
		set_args:
			if(dot == 0)
			{	if((width = (int)lval) < 0)
				{	width = -width;
					flags |= F_LEFT;
				}
				flags |= F_PAD;
			}
			else if(dot == 1)
				precis = (int)lval;
			else	base = (int)lval;
			goto loop_flags;

			/* modifier for object's length */
		case 'l' :
			flags |= F_LONG;
			goto loop_flags;
		case 'h' :
			goto loop_flags;
		case 'L' :
			flags |= F_LDOUBLE;
			goto loop_flags;

			/* PRINTF DIRECTIVES */

		case '&' : /* change extension function */
			if(!argf)
				extf = va_arg(args,Extf_p);
			else if((*argf)('&',(char*)(&extf),t_user,n_user) < 0)
				goto pop_fa;
			continue;
		case '@' : /* change argument getting function */
			if(!argf)
				argf = va_arg(args,Argf_p);
			else if((*argf)('@',(char*)(&argf),t_user,n_user) < 0)
				goto pop_fa;
			continue;
		case ':' : /* stack a pair of format/arglist */
			if(!FAMALLOC(fa))
				goto done;
			fa->form = (char*)form;
			GETARG(form,form,argf,args,char*,char*,'1',t_user,n_user);
			if(!form)
				form = "";
#if (defined(CSRG_BASED) && !defined(__LP64__)) || (defined(linux) && !defined(__LP64__))
			GETARG(argsp,argsp,argf,args,va_list*,va_list*,'2',t_user,n_user);
			memcpy((Void_t*)(&(fa->args)), (Void_t*)(&args), sizeof(va_list));
			memcpy((Void_t*)(&args), (Void_t*)argsp, sizeof(va_list));
#else
			GETARGL(argsp,argsp,argf,args,va_list*,va_list*,'2',t_user,n_user);
			__va_copy( fa->args, args );
			__va_copy( args, argsp );
#endif
			fa->argf.p = argf;
			fa->extf.p = extf;
			fa->next = fast;
			fast = fa;
			continue;

		default :	/* unknown directive */
			if(extf)
			{
#if defined(CSRG_BASED) && !defined(__LP64__)
				va_list savarg = args;  /* is this portable? */
#else
				va_list	savarg; 	/* is this portable?   Sorry .. NO. */
				__va_copy( savarg, args );
#endif

				GETARG(sp,astr,argf,args,char*,char*,fmt,t_user,n_user);
				astr = NIL(char*);
				n = (*extf)(sp,fmt,precis,&astr,(int)base,t_user,n_user);
				if((sp = astr) )
					goto s_format;

#if defined(CSRG_BASED) && !defined(__LP64__)
				args = savarg;  /* extf failed, treat as if unmatched */
#else
				__va_copy( args, savarg ); /* extf failed, treat as if unmatched */
#endif
			}

			/* treat as text */
			form -= 1;
			continue;

		case 's':	/* a string */
			GETARG(sp,astr,argf,args,char*,char*,'s',t_user,n_user);
			n = -1;
			if(!sp)
			{	/* standard error string for null pointer */
				endsp = (sp = "(null)") + 6;
				flags = 0;
			}
			else
			{	/* set other bound */
			s_format:
				if(n < 0)
				{	ssp = sp;
					if((n = precis) < 0)
						while(*ssp++) ;
					else	while(*ssp++ && --n >= 0) ;
					n = (ssp - sp) - 1;
				}
				else if(precis >= 0 && precis < n)
					n = precis;
				endsp = sp+n;
			}
			flags &= ~(F_SIGN|F_BLANK|F_ALTER);
			precis = 0;
			break;

		case 'n':	/* return current output length */
			SFEND(f);
			if(flags&F_LONG)
			{	GETARG(alp,alp,argf,args,
					ulong*,ulong*,'N',t_user,n_user);
				*alp = n_output;
			}
			else
			{	GETARG(aip,aip,argf,args,
					uint*,uint*,'n',t_user,n_user);
				*aip = n_output;
			}
			continue;
		case 'c':	/* a character */
			GETARG(fmt,achar,argf,args,int,uint,'c',t_user,n_user);
		case '%':
			flags = (flags&~(F_SIGN|F_BLANK|F_ZERO))|F_REPEAT;
			if(precis <= 0)
				precis = 1;
			break;
		case 'p':	/* pointer value */
			GETARG(ssp,astr,argf,args,char*,char*,'p',t_user,n_user);
			lval = (long)ssp;
			flags = (flags&~(F_SIGN|F_BLANK|F_ZERO))|F_ALTER;
			dot = 0;
			fmt = 'x';
			goto unsigned_cvt;
		case 'o':
		case 'x':
		case 'X':
			dot = 0;
		case 'u':
			flags &= ~(F_SIGN|F_BLANK);
			if(flags&F_LONG)
			{	GETARG(lval,along,argf,args,
					long,ulong,'U',t_user,n_user);
			}
			else
			{	GETARG(lval,aint,argf,args,
					long,uint,'u',t_user,n_user); 
			}
			goto i_format;
		case 'i':
		case 'd':
			if(flags&F_LONG)
			{	GETARG(lval,along,argf,args,
					long,long,'D',t_user,n_user);
			}
			else
			{	GETARG(lval,aint,argf,args,
					long,int,'d',t_user,n_user); 
			}

		i_format:
			if(lval == 0 && precis == 0)
				goto done_cvt;

			if(lval < 0 && (fmt == 'd' || fmt == 'i'))
			{	flags |= F_MINUS;
				if(lval == HIGHBIT)
				{	/* avoid overflow */
					if(base < 2 || base > SF_RADIX)
						base = 10;
					lval = ((ulong)HIGHBIT)/base;
					*--sp = _Sfdigits[
						(ulong)HIGHBIT - ((ulong)lval)*base];
				}
				else	lval = -lval;
			}

		unsigned_cvt:
			ssp = _Sfdigits;
			switch(fmt)
			{
			case 'o' :
				base = 8;
				n = 3;
				goto power_cvt;
			case 'X' :
				ssp = "0123456789ABCDEF";
			case 'x' :
				base = 16;
				n = 4;
				goto power_cvt;
			default :
				if(base < 2 || base > SF_RADIX)
					base = 10;
				break;
			}

			if(base == 10)
			{	/* special fast conversion for base 10 */
				sfucvt(lval,sp,n,ssp);
			}
			else if((base & (base-1)) == 0)
			{	/* calculate shift amount for power-of-2 base */
				if(base < 8)
					n = base <  4 ? 1 : 2;
				else if(base < 32)
					n = base < 16 ? 3 : 4;
				else	n = base < 64 ? 5 : 6;
			power_cvt:
				do
				{	*--sp = ssp[lval&(base-1)];
				} while(lval = ((ulong)lval) >> n);
			}
			else
			{	do
				{	*--sp = ssp[((ulong)lval)%base];
				} while(lval = ((ulong)lval)/((ulong)base));
			}

			/* zero padding for precision */
			for(precis -= (endsp-sp); precis > 0; --precis)
				*--sp = '0';

			if(flags&F_ALTER)
			{	/* prefix */
				if(fmt == 'o')
				{	if(*sp != '0')
						*--sp = '0';
				}
				else
				{	if(width > 0 && (flags&F_ZERO))
					{	/* do 0 padding first */
						if(dot == 2)
							n = base < 10 ? 2 : 3;
						else if(fmt == 'x' || fmt == 'X')
							n = 0;
						else	n = width;
						n += (flags&(F_MINUS|F_SIGN)) ? 1 : 0;
						n = width - (n + (endsp-sp));
						while(n-- > 0)
							*--sp = '0';
					}
					if(dot == 2)
					{	/* base#value notation */
						*--sp = '#';
						if(base < 10)
							*--sp = (char)('0'+base);
						else
						{	*--sp = _Sfdec[(base <<= 1)+1];
							*--sp = _Sfdec[base];
						}
					}
					else if(fmt == 'x' || fmt == 'X')
					{	*--sp = (char)fmt;
						*--sp = '0';
					}
				}
			}

		done_cvt:
			break;

		case 'g': /* %g and %G ultimately become %e or %f */
		case 'G':
		case 'e':
		case 'E':
		case 'f':
		case 'F':
#if _typ_long_double
			if(flags&F_LDOUBLE)
			{	GETARG(dval,dval,argf,args,
					Double_t,Double_t,'G',t_user,n_user);
			}
			else
#endif
			{
#if _typ_long_double
				double	sdval;
				GETARG(sdval,sdval,argf,args,
					double,double,'F',t_user,n_user);
				dval = sdval;
#else
				GETARG(dval,dval,argf,args,
					double,double,'F',t_user,n_user);
#endif
			}

			if(fmt == 'e' || fmt == 'E')
			{	n = (precis = precis < 0 ? FPRECIS : precis)+1;
				ep = _sfcvt(dval,(int)min(n,SF_FDIGITS),&decpt,&sign,1);
				goto e_format;
			}
			else if(fmt == 'f' || fmt == 'F')
			{	precis = precis < 0 ? FPRECIS : precis;
				ep = _sfcvt(dval,min(precis,SF_FDIGITS),&decpt,&sign,0);
				goto f_format;
			}

			/* 'g' or 'G' format */
			precis = precis < 0 ? FPRECIS : precis == 0 ? 1 : precis;
			ep = _sfcvt(dval,min(precis,SF_FDIGITS),&decpt,&sign,1);
			if(dval == 0.)
				decpt = 1;
			else if(*ep == 'I')
				goto infinite;

			if(!(flags&F_ALTER))
			{	/* zap trailing 0s */
				if((n = sfslen()) > precis)
					n = precis;
				while((n -= 1) >= 1 && ep[n] == '0')
					;
				n += 1;
			}
			else	n = precis;

			flags = (flags & ~F_ZERO) | F_GFORMAT;
			if(decpt < -3 || decpt > precis)
			{	precis = (int)(n-1);
				goto e_format;
			}
			else
			{	precis = (int)(n - decpt);
				goto f_format;
			}

		e_format: /* build the x.yyyy string */
			if(isalpha(*ep))
				goto infinite;
			sp = endsp = buf+1;	/* reserve space for sign */
			*endsp++ = *ep ? *ep++ : '0';

			if(precis > 0 || (flags&F_ALTER))
				*endsp++ = GETDECIMAL(dc,lv);
			ssp = endsp;
			endep = ep+precis;
			while((*endsp++ = *ep++) && ep <= endep)
				;
			precis -= (endsp -= 1) - ssp;

			/* build the exponent */
			ep = endep = buf+(sizeof(buf)-1);
			if(dval != 0.)
			{	if((n = decpt - 1) < 0)
					n = -n;
				while(n > 9)
				{	lval = n; n /= 10;	
					*--ep = (char)('0' + (lval - n*10));
				}
			}
			else	n = 0;
			*--ep = (char)('0' + n);
			if(endep-ep <= 1)	/* at least 2 digits */
				*--ep = '0';

			/* the e/Exponent separator and sign */
			*--ep = (decpt > 0 || dval == 0.) ? '+' : '-';
			*--ep = isupper(fmt) ? 'E' : 'e';

			flags = (flags&~F_ZERO)|F_FLOAT;
			goto end_efg;

		f_format: /* data before the decimal point */
			if(isalpha(*ep))
			{
			infinite:
				endsp = (sp = ep)+sfslen();
				ep = endep;
				precis = 0;
				goto end_efg;
			}

			endsp = sp = buf+1;	/* save a space for sign */
			endep = ep+decpt;
			while(ep < endep && (*endsp++ = *ep++))
				;
			if(endsp == sp)
				*endsp++ = '0';

			if(precis > 0 || (flags&F_ALTER))
				*endsp++ = GETDECIMAL(dc,lv);

			if((n = -decpt) > 0)
			{	/* output zeros for negative exponent */
				ssp = endsp + min(n,precis);
				precis -= (int)n;
				while(endsp < ssp)
					*endsp++ = '0';
			}

			ssp = endsp;
			endep = ep+precis;
			while((*endsp++ = *ep++) && ep <= endep)
				;
			precis -= (endsp -= 1) - ssp;
			ep = endep;
			flags |= F_FLOAT;
		end_efg:
			if(sign)
			{	/* if a %gG, output the sign now */
				if(flags&F_GFORMAT)
				{	*--sp = '-';
					flags &= ~(F_SIGN|F_BLANK);
				}
				else	flags |= F_MINUS;
			}
			break;
		}

		if(!flags)
			goto do_output;
		else if(flags&(F_MINUS|F_SIGN|F_BLANK))
			fmt = (flags&F_MINUS) ? '-' : (flags&F_SIGN) ? '+' : ' ';

		n = (endsp-sp) + (endep-ep) + (precis <= 0 ? 0 : precis) +
		    ((flags&F_PREFIX) ? 1 : 0);
		if((lval = width-n) > 0)
		{	/* check for padding */
			if(!(flags&F_ZERO))
			{	/* right padding */
				if(flags&F_LEFT)
					lval = -lval;
				else if(flags&F_PREFIX)
				{	/* blank padding, output prefix now */
					*--sp = fmt;
					flags &= ~F_PREFIX;
				}
			}
		}
		else	lval = 0;

		if(flags&F_PREFIX)
		{	/* output prefix */
			SFputc(f,fmt);
			if(fmt != ' ')
				flags |= F_ZERO;
		}

		if((n = lval) > 0)
		{	/* left padding */
			v = (flags&F_ZERO) ? '0' : ' ';
			SFnputc(f,v,n);
		}

		if((n = precis) > 0 && ((flags&F_REPEAT) || !(flags&F_FLOAT)))
		{	/* repeated chars or padding for integer precision */
			v = (flags&F_REPEAT) ? fmt : '0';
			SFnputc(f,v,n);
			precis = 0;
		}

	do_output:
		if((n = endsp-sp) > 0)
			SFwrite(f,sp,n);

		if(flags&(F_FLOAT|F_LEFT))
		{	/* F_FLOAT: right padding for float precision */
			if((n = precis) > 0)
				SFnputc(f,'0',n);

			/* F_FLOAT: the exponent of %eE */
			if((n = endep-(sp=ep)) > 0)
				SFwrite(f,sp,n);

			/* F_LEFT: right padding */
			if((n = -lval) > 0)
				SFnputc(f,' ',n);
		}
	}

pop_fa:	if(fa = fast)
	{	/* pop the format stack and continue */
		form = fa->form;
		memcpy((Void_t*)(&args), (Void_t*)(&(fa->args)), sizeof(va_list));
		argf = fa->argf.p;
		extf = fa->extf.p;
		fast = fa->next;
		FAFREE(fa);
		goto loop_fa;
	}

done:
	SFEND(f);

	r = f->next - f->data;
	if((d = (char*)f->data) == data)
		f->endw = f->endr = f->endb = f->data = NIL(uchar*);
	f->next = f->data;

	if(((f->flags&SF_SHARE) && !(f->flags&SF_PUBLIC)) ||
	   (r > 0 && (d == data || ((f->flags&SF_LINE) && !(f->flags&SF_STRING)))) )
		(void)SFWRITE(f,(Void_t*)d,r);
	else	f->next += r;

	SFOPEN(f,0);
	return n_output;
}
