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
/* $XConsortium: config.h /main/1 1996/07/29 17:08:30 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef config_INCLUDED
#define config_INCLUDED 1

#define SP_INCLUDE_UNISTD_H
#define SP_POSIX_FILENAMES

#if defined(__GNUG__) || defined(__SunOS)
// It's not missing, but it pulls in libg++
#if !defined(linux) && !defined(CSRG_BASED) && !defined(sun)
#define SP_NEW_H_MISSING
// set_new_handler() has to be declared extern "C"
#define SP_SET_NEW_HANDLER_EXTERN_C
#endif
#ifndef SP_MANUAL_INST
#define SP_MANUAL_INST
#endif
#ifndef SP_ANSI_CLASS_INST
#define SP_ANSI_CLASS_INST
#endif
#ifndef SP_HAVE_BOOL
#define SP_HAVE_BOOL
#endif
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define SP_ANSI_FOR_SCOPE
#endif

#endif /* __GNUG__ */

#if defined(sun) || defined(__sun)
// struct stat has st_blksize member
#define SP_STAT_BLKSIZE
#endif

#ifdef __EMX__
// EMX 0.9a for OS/2
#undef SP_POSIX_FILENAMES
#define SP_MSDOS_FILENAMES
#endif

#ifdef _MSC_VER
// Microsoft Visual C++ 4.0
#undef SP_INCLUDE_UNISTD_H
#define SP_INCLUDE_IO_H
#ifndef SP_ANSI_CLASS_INST
#define SP_ANSI_CLASS_INST
#endif
#undef SP_POSIX_FILENAMES
#define SP_MSDOS_FILENAMES
#define SP_SHORT_HEADERS
#pragma warning ( disable : 4660 ) // already instantiated
#pragma warning ( disable : 4661 ) // missing def for decl member
#pragma warning ( disable : 4786 ) // debug symbol truncated (>255 chars)
#pragma warning ( disable : 4018 ) // signed/unsigned mismatch
#pragma warning ( disable : 4251 ) // __declspec(dllexport)
#pragma warning ( disable : 4275 )
#pragma warning ( disable : 4237 ) // future reserved keyword
#define huge verybig
#if _MSC_VER == 900
#define SP_DECLARE_PLACEMENT_OPERATOR_NEW
#endif
#define set_new_handler _set_new_handler
// Function passed to set_new_handler() returns int and takes size_t argument.
#define SP_FANCY_NEW_HANDLER

#define SP_HAVE_SETMODE
#define SP_DLLEXPORT __declspec(dllexport)
#define SP_DLLIMPORT __declspec(dllimport)

#ifdef _DLL
#define SP_USE_DLL
#endif

#ifdef SP_USE_DLL
#ifndef BUILD_LIBSP
// It's not possible to export templates using __declspec(dllexport),
// so instead we include the template definitions in the headers,
// which allows Visual C++ to instantiate any needed templates
// in the client.
#define SP_DEFINE_TEMPLATES
#endif
#endif /* SP_USE_DLL */

#ifndef SP_MANUAL_INST
#ifndef SP_DEFINE_TEMPLATES
#define SP_MANUAL_INST
#endif
#endif /* not SP_MANUAL_INST */

// Make sure both _UNICODE and UNICODE are defined if either is.

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif /* not UNICODE */
#endif /* _UNICODE */

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif /* not _UNICODE */
#endif /* UNICODE */

#ifdef _UNICODE
#define SP_WIDE_SYSTEM
#endif

// wchar_t's base type is an unsigned short
#define SP_WCHAR_T_USHORT

// Enable precompiled header support.
#define SP_PCH
// Don't compile in message text.
#define SP_NO_MESSAGE_TEXT
#endif /* _MSC_VER */

#ifdef __WATCOMC__
// Watcom C++ 10.0a
#define SP_MANUAL_INST
#undef SP_POSIX_FILENAMES
#define SP_MSDOS_FILENAMES
#undef SP_INCLUDE_UNISTD_H
#define SP_INCLUDE_IO_H
#pragma warning 004 9
#undef huge
// Cannot handle T::~T in template.
#define SP_QUAL_TEMPLATE_DTOR_BROKEN
#define SP_HAVE_SETMODE
#define _setmode setmode
#if __WATCOMC__ < 1050
#define _O_BINARY O_BINARY
#endif
#define SP_WCHAR_T_USHORT
#endif /* __WATCOMC__ */

#ifdef __BORLANDC__
// Borland C++ 5.0
#define SP_ANSI_FOR_SCOPE
#define SP_HAVE_RTTI
#define SP_HAVE_SETMODE
#undef SP_INCLUDE_UNISTD_H
#define SP_INCLUDE_IO_H
#undef SP_POSIX_FILENAMES
#define SP_MSDOS_FILENAMES
#define SP_HAVE_BOOL
#define SP_SHORT_HEADERS
#define _O_BINARY O_BINARY
#define _setmode setmode
#define SP_ANSI_CLASS_INST
#define SP_MANUAL_INST
// Building as a DLL doesn't work with Borland C++ yet.
#define SP_DLLEXPORT __declspec(dllexport)
#define SP_DLLIMPORT __declspec(dllimport)
#ifdef SP_USE_DLL
#ifndef BUILD_LIBSP
#define SP_DEFINE_TEMPLATES
#endif
#endif /* SP_USE_DLL */
#define SP_WCHAR_T_USHORT
#endif /* __BORLANDC__ */

#ifdef __IBMCPP__
// IBM CSet++ 2.1 from Horst Szillat <szillat@berlin.snafu.de>.
#undef SP_POSIX_FILENAMES
#define SP_MANUAL_INST
#define SP_SHORT_HEADERS
#define SP_MSDOS_FILENAMES
#undef SP_INCLUDE_UNISTD_H
#define SP_INCLUDE_IO_H
#define S_IFMT (S_IFDIR|S_IFCHR|S_IFREG)
#endif

#ifndef SP_ANSI_FOR_SCOPE
// This simulates the new ANSI "for" scope rules
#define for if (0); else for
#endif

#ifndef SP_DLLEXPORT
#define SP_DLLEXPORT /* as nothing */
#endif

#ifndef SP_DLLIMPORT
#define SP_DLLIMPORT /* as nothing */
#endif

#ifdef SP_USE_DLL

#ifdef BUILD_LIBSP
#define SP_API SP_DLLEXPORT
#else
#define SP_API SP_DLLIMPORT
#endif

#else /* not SP_USE_DLL */

#define SP_API /* as nothing */

#endif /* not SP_USE_DLL */

// SP_WIDE_SYSTEM says that your OS provides wide character interfaces
// SP_WIDE_SYSTEM currently works only with Visual C++ and Windows NT
// SP_WIDE_SYSTEM implies SP_MULTI_BYTE
#ifdef SP_WIDE_SYSTEM
#define SP_MULTI_BYTE
#endif

#ifdef SP_NAMESPACE
#define SP_NAMESPACE_SCOPE SP_NAMESPACE::
#else
#define SP_NAMESPACE_SCOPE
#endif

#endif /* not config_INCLUDED */
