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
/* $XConsortium: ilerrors.h /main/3 1995/10/23 15:47:31 rswiston $ */
/**---------------------------------------------------------------------
***	
***    file:           ilerrors.h
***
***    description:    Public error codes for IL
***
***	
***    (c)Copyright 1991 Hewlett-Packard Co.
***    
***                             RESTRICTED RIGHTS LEGEND
***    Use, duplication, or disclosure by the U.S. Government is subject to
***    restrictions as set forth in sub-paragraph (c)(1)(ii) of the Rights in
***    Technical Data and Computer Software clause in DFARS 252.227-7013.
***                             Hewlett-Packard Company
***                             3000 Hanover Street
***                             Palo Alto, CA 94304 U.S.A.
***    Rights for non-DOD U.S. Government Departments and Agencies are as set
***    forth in FAR 52.227-19(c)(1,2).
***
***-------------------------------------------------------------------*/



#ifndef ILERRORS_H
#define ILERRORS_H

#ifndef IL_H
#include "il.h"
#endif



        /*  Error codes for the Image Library (IL), of type ilError.
            User-defined error codes (e.g. from user pipe elements)
            should be negative.

            Do "man ilerrors" for a description of the error codes.
            NOTE: IL_OK (0) in il.h means "no error".
        */

#define IL_ERROR_LAST_STRIP                 1

#define IL_ERROR_ELEMENT_AGAIN              2

#define IL_ERROR_ELEMENT_COMPLETE           3

#define IL_ERROR_PAR_NOT_ZERO               4

#define IL_ERROR_VERSION_MISMATCH           5

#define IL_ERROR_BITS_PER_SAMPLE            6

#define IL_ERROR_SAMPLES_PER_PIXEL          7

#define IL_ERROR_LEVELS_PER_SAMPLE          8

#define IL_ERROR_SAMPLE_ORDER               9

#define IL_ERROR_BYTE_ORDER                 10

#define IL_ERROR_RESERVED_NOT_ZERO          11

#define IL_ERROR_ROW_BIT_ALIGN              12

#define IL_ERROR_COMPRESSION                13

#define IL_ERROR_IMAGE_TYPE                 16

#define IL_ERROR_PIPE_STATE                 17

#define IL_ERROR_MALLOC                     19

#define IL_ERROR_OBJECT_TYPE                20

#define IL_ERROR_CONTEXT_MISMATCH           21

#define IL_ERROR_CIRCULAR_PIPE              22

#define IL_ERROR_IMAGE_SIZE                 23

#define IL_ERROR_PRODUCER_DATA              24

#define IL_ERROR_USER_PIPE_ELEMENT          25

#define IL_ERROR_PIPE_ELEMENT               26

#define IL_ERROR_CANT_CONVERT               27

#define IL_ERROR_X_GET_IMAGE                28

#define IL_ERROR_X_RESOURCE                 29

#define IL_ERROR_NOT_IMPLEMENTED            30

#define IL_ERROR_ELEMENT_TYPE               31

#define IL_ERROR_ZERO_SIZE_IMAGE            32

#define IL_ERROR_NULL_RECT_PTR              34

#define IL_ERROR_MAP_IMAGE                  35

#define IL_ERROR_EXECUTE_STACK_OVERFLOW     36

#define IL_ERROR_EXECUTE_STACK_UNDERFLOW    37

#define IL_ERROR_X_DRAWABLE                 38

#define IL_ERROR_X_COLORMAP_VISUAL          40

#define IL_ERROR_UNSUPPORTED_VISUAL         41

#define IL_ERROR_SCALE_BITONAL_TO_GRAY      42

#define IL_ERROR_INVALID_OPTION             43

#define IL_ERROR_FILE_IO                    44

#define IL_ERROR_FILE_NOT_TIFF              45

#define IL_ERROR_FILE_MALFORMED_TAG         46

#define IL_ERROR_FILE_MISSING_TAG           47

#define IL_ERROR_FILE_TAG_VALUE             48

#define IL_ERROR_FILE_TAG_TYPE              49

#define IL_ERROR_FILE_STRIP_TAGS            50

#define IL_ERROR_FILE_WRITE_METHOD          51

#define IL_ERROR_NULL_COMPRESSED_IMAGE      52
      
#define IL_ERROR_OPTION_DATA                55

#define IL_ERROR_SUBSAMPLE                  56

#define IL_ERROR_COMPRESSED_DATA            60

#define IL_ERROR_MALFORMED_FILE_WRITE       61

#define IL_ERROR_YCBCR_DATA                 65

#define IL_ERROR_YCBCR_FORMAT               66

#define IL_ERROR_YCBCR_TIFF                 67

#define IL_ERROR_CONVERT_COMPRESSION        68

#define IL_ERROR_CONVERT_TO_PALETTE         69

#define IL_ERROR_EFS_IO                     70

#define IL_ERROR_EFS_OPEN                   71

#define IL_ERROR_EFS_NOT_MINE               72

#define IL_ERROR_EFS_EMPTY_FILE             73

#define IL_ERROR_EFS_EOF                    74

#define IL_ERROR_EFS_PRIVATE                75

#define IL_ERROR_EFS_OPEN_MODE              76

#define IL_ERROR_EFS_NO_FILE_TYPE           77

#define IL_ERROR_EFS_FILE_TYPE_INFO         78

#define IL_ERROR_EFS_READ_MODE              79

#define IL_ERROR_EFS_FORMAT_ERROR           80

#define IL_ERROR_EFS_UNSUPPORTED            81

#define IL_ERROR_PIPE_NOT_FED               82

#define IL_ERROR_RAW_MODE                   83

#define IL_ERROR_PALETTE_INFO               84

#define IL_ERROR_MALFORMED_IMAGE_WRITE      85

#define IL_ERROR_FEED_PIPE                  86

#endif
