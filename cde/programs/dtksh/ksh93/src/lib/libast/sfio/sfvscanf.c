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
/* $XConsortium: sfvscanf.c /main/3 1995/11/01 18:38:41 rswiston $ */
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

/*	The main engine for reading formatted data
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#define	S_NORMAL	0	/* argument is a pointer to a normal object */
#define S_LONG		1	/* argument is a pointer to a long object */
#define S_SHORT		2	/* argument is a pointer to a short object */
#define S_LONGDOUBLE	3	/* argument is a pointer to a long double object */

#if !_typ_long_double
#ifndef _typ_long_double
#define _typ_long_double	0
#endif
#endif

#define MAXLENGTH	(int)(((uint)~0)>>1)

#define a2f(s)	strtod(s,NIL(char**))

#define A_SIZE		(1<<8)	/* alphabet */
#if __STD_C
static char *setclass(reg char* form, reg char* accept)
#else
static char *setclass(form,accept)
reg char	*form;		/* format string */
reg char	*accept;	/* accepted characters are set to 1 */
#endif
{
	reg int		fmt, c, yes;

	if((fmt = *form++) == '^')
	{	/* we want the complement of this set */
		yes = 0;
		fmt = *form++;
	}
	else	yes = 1;

	for(c = 0; c < A_SIZE; ++c)
		accept[c] = !yes;

	if(fmt == ']' || fmt == '-')
	{	/* special first char */
		accept[fmt] = yes;
		fmt = *form++;
	}

	for(; fmt != ']'; fmt = *form++)
	{	/* done */
		if(!fmt)
			return (form-1);

		/* interval */
		if(fmt != '-' || form[0] == ']' || form[-2] > form[0])
			accept[fmt] = yes;
		else for(c = form[-2]+1; c < form[0]; ++c)
			accept[c] = yes;
	}

	return form;
}

#define SETARG(val,argf,args,type,fmt,form) \
	{ if(!argf) \
		*va_arg(args,type*) = (type)val; \
	  else if((*argf)(fmt,(char*)(&val),sizeof(val)) < 0) \
		form = ""; \
	}

#if __STD_C
static void _sfbuf(Sfio_t* f, int* rs)
#else
static void _sfbuf(f, rs)
Sfio_t*	f;
int*	rs;
#endif
{
	if(f->next >= f->endb)
	{	if(*rs > 0)	/* try peeking for a share stream if possible */
		{	f->mode |= SF_RV;
			if(SFFILBUF(f,-1) > 0)
			{	f->mode |= SF_PEEK;
				return;
			}
			*rs = -1;	/* can't peek, back to normal reads */
		}
		(void)SFFILBUF(f,-1);
	}
}

#if __STD_C
int sfvscanf(Sfio_t* f, reg const char* form, va_list args)
#else
sfvscanf(f,form,args)
Sfio_t		*f;		/* file to be scanned */
reg char	*form;		/* scanning format */
va_list		args;
#endif
{
	reg uchar	*d, *endd, *data;
	reg int		inp, shift, base, length;
	reg int		fmt, skip, size, n_assign;
	reg char	*sp;
	Sfio_t		*sf;
	int		n_input, len;
	char		accept[SF_MAXDIGITS];
	Argf_s		argf;
	Extf_s		extf;
	Fa_t		*fa, *fast;
	va_list*	argsp;
	int		rs = (f->extent < 0 && (f->flags&SF_SHARE)) ? 1 : 0;

#define SFBUF(f)	(_sfbuf(f,&rs), \
			 (data = d = f->next), (endd = f->endb), (d < endd) )
#define SFLEN(f)	(d-data)
#define SFEND(f)	((n_input += d-data), \
			 (rs > 0 ? SFREAD(f,(Void_t*)data,d-data) : ((f->next = d), 0)) )
#define SFGETC(f,c)	((c) = (d < endd || (SFEND(f),SFBUF(f))) ? (int)(*d++) : -1 )
#define SFUNGETC(f,c)	(--d)

	if(f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0)
		return -1;
	SFLOCK(f,0);

	SFBUF(f);
	n_assign = n_input = 0;
	inp = -1;
	sf = NIL(Sfio_t*);
	argf = NIL(Argf_s);
	extf = NIL(Extf_s);
	fast = NIL(Fa_t*);

loop_fa :
	while((fmt = *form++) )
	{
		if(fmt != '%')
		{	/* matching white space directive */
			if(isspace(fmt))
			{	if(fmt != '\n' || !(f->flags&SF_LINE))
					fmt = -1;
				for(;;)
				{	if(SFGETC(f,inp) < 0)
						goto done;
					else if(!isspace(inp))
					{	/* put back unmatched byte */
						SFUNGETC(f,inp);
						break;
					}
					else if(inp == fmt)	/* match only one \n */
						break;
				}
			}
			else
			{
			literal : /* literal match */
				if(SFGETC(f,inp) != fmt)
				{	if(inp >= 0)
						SFUNGETC(f,inp);
					goto done;
				}
			}
			continue;
		}

		/* matching some pattern */
		skip = length = 0;
		base = 10;
		switch((fmt = *form++) )
		{
		case '%' : /* match % literally */
			goto literal;

		case '@' : /* set argument getting function */
			argf = va_arg(args,Argf_s);
			continue;

		case '&' : /* set extension function */
			extf = va_arg(args,Extf_s);
			continue;

		case 'n' : /* return number of bytes read */
			len = n_input + SFLEN(f);
			SETARG(len,argf,args,int,'n',form);
			continue;

		case ':' : /* stack a pair of format/arglist */
			if(!FAMALLOC(fa))
				goto done;
			fa->form = (char*)form;
			if(!(form = va_arg(args,char*)))
				form = "";
#ifdef __ppc
			__va_copy( argsp, va_arg(args,va_list*) );
			__va_copy( fa->args, args );
			__va_copy( args, argsp );
#else
			argsp = va_arg(args,va_list*);
			memcpy((Void_t*)(&(fa->args)), (Void_t*)(&args), sizeof(va_list));
			memcpy((Void_t*)(&args), (Void_t*)argsp, sizeof(va_list));
#endif
			fa->extf.s = extf;
			fa->argf.s = argf;
			fa->next = fast;
			fast = fa;
			continue;

		case '*' :	/* skip one argument */
			skip = 1;
			fmt = *form++;

		default :
			/* scan length */
			while(isdigit(fmt))
			{	length = length*10 + (fmt - '0');
				fmt = *form++;
			}

			if(fmt == '.')
			{	/* defining a base */
				fmt = *form++;
				base = 0;
				while(isdigit(fmt))
				{	base = base*10 + (fmt - '0');
					fmt = *form++;
				}
				if(base < 2 || base > SF_RADIX)
					base = 10;
			}

			/* size of object to be assigned */
			if(fmt == 'L')
				{ size = S_LONGDOUBLE; fmt = *form++; }
			else if(fmt == 'l')
				{ size = S_LONG; fmt = *form++; }
			else if(fmt == 'h')
				{ size = S_SHORT; fmt = *form++; }
			else	size = S_NORMAL;

			/* canonicalize format */
			if(fmt == 'e' || fmt == 'g')
				fmt = 'f';

			if(size == S_LONGDOUBLE && (fmt != 'f' || !_typ_long_double))
				size = S_LONG;
		}

		/* scan length */
		if(length == 0)
			length = fmt == 'c' ? 1 : MAXLENGTH;

		/* define the first input character */
		if(fmt == 'c' || fmt == '[')
			SFGETC(f,inp);
		else
		{	/* skip starting blanks */
			do	{ SFGETC(f,inp); }
			while(isspace(inp))
				;
		}
		if(inp < 0)
			goto done;

		if(fmt == 'd' || fmt == 'o' || fmt == 'x' )
			goto dec_convert;
		else if(fmt == 'f')
		{	/* a float or double */
			reg char*	val;
			reg int		dot, exponent;
			Double_t	dval;

			val = accept;
			if(length >= SF_MAXDIGITS)
				length = SF_MAXDIGITS-1;
			dot = exponent = 0;
			do
			{	if(isdigit(inp))
					*val++ = inp;
				else if(inp == '.')
				{	/* too many dots */
					if(dot++ > 0)
						break;
					*val++ = '.';
				}
				else if(inp == 'e' || inp == 'E')
				{	/* too many e,E */
					if(exponent++ > 0)
						break;
					*val++ = inp;
					if(--length <= 0 || SFGETC(f,inp) < 0 ||
					   (inp != '-' && inp != '+' && !isdigit(inp)) )
						break;
					*val++ = inp;
				}
				else if(inp == '-' || inp == '+')
				{	/* too many signs */
					if(val > accept)
						break;
					*val++ = inp;
				}
				else	break;

			} while(--length > 0 && SFGETC(f,inp) >= 0);

			if(!skip && val > accept)
			{	/* there is something to convert */
				*val = '\0';
				n_assign += 1;
				dval = a2f(accept);

				switch(size)
				{
#if _typ_long_double
				case S_LONGDOUBLE:
					SETARG(dval,argf,args,long double,'G',form);
					break;
#endif
				case S_LONG:
					SETARG(dval,argf,args,double,'F',form);
					break;
				case S_SHORT :
				case S_NORMAL:
					SETARG(dval,argf,args,float,'f',form);
					break;
				}
			}
		}
		else if(fmt == 's' || fmt == 'c' || fmt == '[')
		{	/* get buffer to copy to */
			sp = NIL(char*);
			if(skip)
				size = 0;
			else
			{	if(!argf)
				{	sp = va_arg(args,char*);
					if(size != S_LONG)
						size = -1;
					else	/* buffer size */
					{	size = va_arg(args,int);
						if(fmt != 'c' && size > 0)
							size -= 1;
					}
					if(!sp)
						size = 0;
				}
				else
				{	size = 0;
					if(!sf)
						sf = sfnew(NIL(Sfio_t*),NIL(char*),
							   -1,-1, SF_STRING|SF_WRITE);
					else	sfseek(sf,0L,0);
				}
			}

			if(fmt == 's')
			{	/* copy a string */
				do
				{	if(isspace(inp))
						break;
					if(size < 0)
						*sp++ = inp;
					else if(size > 0)
					{	*sp++ = inp;
						size -= 1;
					}
					else if(!skip && sf)
						sfputc(sf,inp);
				} while(--length > 0 && SFGETC(f,inp) >= 0);
			}
			else if(fmt == 'c')
			{	/* copy characters */
				do
				{	if(size < 0)
						*sp++ = inp;
					else if(size > 0)
					{	*sp++ = inp;
						size -= 1;
					}
					else if(!skip && sf)
						sfputc(sf,inp);
				} while(--length > 0 && SFGETC(f,inp) >= 0);
			}
			else
			{	/* copy characters from a class */
				form = setclass((char*)form,accept);
				if(!accept[inp])
				{	SFUNGETC(f,inp);
					continue;
				}

				do
				{	if(!accept[inp])
						break;
					if(size < 0)
						*sp++ = inp;
					else if(size > 0)
					{	*sp++ = inp;
						size -= 1;
					}
					else if(!skip && sf)
						sfputc(sf,inp);
				} while(--length > 0 && SFGETC(f,inp) >= 0);
			}

			if(!skip)
			{	n_assign += 1;
				if(sp)
				{	if(fmt != 'c')
						*sp = '\0';
				}
				else if(sf)
				{	sfputc(sf,'\0');
					if((*argf)('s',(char*)sf->data,
						(sf->next-sf->data)-1) < 0)
							form = "";
				}
			}
		}
		else if(fmt == 'p' || fmt == 'u')
		{	/* make sure this is unsigned */
			if(inp == '-')
			{	SFUNGETC(f,inp);
				goto done;
			}
			goto dec_convert;
		}
		else if(fmt == 'i')
		{	/* some integer type */
			long	lval;
			reg int	sign;

		dec_convert:
			if(inp == '-' || inp == '+')
			{	/* get the sign */
				sign = inp == '-' ? -1 : 1;

				/* skip until a non-blank */
				while(--length > 0 && SFGETC(f,inp) >= 0)
					if(!isspace(inp))
						break;
			}
			else	sign = 1;
			if(inp < 0)
				goto done;

			if(fmt == 'i')
			{	/* data type is self-described */
				if(inp == '0')
				{	if(--length > 0)
						SFGETC(f,inp);
					if(inp == 'x' || inp == 'X')
					{	base = 16;
						if(--length > 0)
							SFGETC(f,inp);
					}
					else	base = 8;
				}
				else	base = 10;
			}
			else if(fmt == 'o')
				base = 8;
			else if(fmt == 'x' || fmt == 'p')
				base = 16;

			/* now convert */
			lval = 0;
			if(base == 16)
			{	sp = _Sfv36;
				shift = 4;
				if(sp[inp] >= 16)
				{	SFUNGETC(f,inp);
					goto done;
				}
				if(inp == '0' && --length > 0)
				{	/* skip leading 0x or 0X */
					SFGETC(f,inp);
					if((inp == 'x' || inp == 'X') && --length > 0)
						SFGETC(f,inp);
				}
				if(inp >= 0 && sp[inp] < 16)
					goto base_shift;
			}
			else if(base == 10)
			{	/* fast base 10 conversion */
				if(inp < '0' || inp > '9')
				{	SFUNGETC(f,inp);
					goto done;
				}

				do	{ lval = (lval<<3) + (lval<<1) + (inp - '0'); }
				while(--length > 0 &&
				      SFGETC(f,inp) >= '0' && inp <= '9');

				if(fmt == 'i' && inp == '#' &&
				   lval >= 2 && lval <= SF_RADIX)
				{	base = (int)lval;
					lval = 0;
					sp = base <= 36 ? _Sfv36 : _Sfvmax;
					if(--length > 0 &&
					   SFGETC(f,inp) >= 0 && sp[inp] < base)
						goto base_conv;
				}
			}
			else
			{	/* other bases */
				sp = base <= 36 ? _Sfv36 : _Sfvmax;
				if(base < 2 || base > SF_RADIX || sp[inp] >= base)
				{	SFUNGETC(f,inp);
					goto done;
				}

			base_conv: /* check for power of 2 conversions */
				if((base & ~(base-1)) == base)
				{	if(base < 8)
						shift = base <  4 ? 1 : 2;
					else if(base < 32)
						shift = base < 16 ? 3 : 4;
					else	shift = base < 64 ? 5 : 6;

			base_shift:	/* fast conversion with shifting */
					do	{ lval = (lval << shift) + sp[inp]; }
					while(--length > 0 &&
					      SFGETC(f,inp) >= 0 && sp[inp] < base);
				}
				else
				{	do	{ lval = (lval * base) + sp[inp]; }
					while(--length > 0 &&
					      SFGETC(f,inp) >= 0 && sp[inp] < base);
				}
			}

			if(!skip)
			{	/* assign */
				n_assign += 1;
				if(sign < 0)
					lval = -lval;
				if(fmt == 'p')
				{	/* pointer conversion */
					SETARG(lval,argf,args,char*,'p',form);
				}
				else switch(size)
				{
				case S_SHORT :
					SETARG(lval,argf,args,short,'h',form);
					break;
				case S_NORMAL :
					SETARG(lval,argf,args,int,'d',form);
					break;
				case S_LONG :
					SETARG(lval,argf,args,long,'D',form);
					break;
				}
			}
		}
		else /* undefined pattern */
		{	/* return the read byte to the stream */
			SFUNGETC(f,inp);
			if(extf)
			{	/* call extension function */
				char	*rv;
				int	n;
				SFEND(f);
				n = (*extf)(f,fmt,length,&rv);
				SFBUF(f);
				if(n >= 0 && !skip)
				{	n_assign += 1;
					if(!argf)
					{	sp = va_arg(args,char*);
						while(n--)
							*sp++ = *rv++;
					}
					else if((*argf)(fmt,rv,n) < 0)
						form = "";
				}
			}
			continue;
		}

		if(length > 0 && inp >= 0)
			SFUNGETC(f,inp);
	}

	if((fa = fast) )
	{	/* check for stacked formats/arglists */
		form = fa->form;
		memcpy((Void_t*)(&args), (Void_t*)(&(fa->args)), sizeof(va_list));
		argf = fa->argf.s;
		extf = fa->extf.s;
		fast = fa->next;
		FAFREE(fa);
		goto loop_fa;
	}

done:
	if(sf)
		sfclose(sf);
	SFEND(f);
	SFOPEN(f,0);
	return (n_assign == 0 && inp < 0) ? -1 : n_assign;
}
