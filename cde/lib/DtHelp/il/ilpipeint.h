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
/* $XConsortium: ilpipeint.h /main/3 1995/10/23 15:58:54 rswiston $ */
/**---------------------------------------------------------------------
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

#ifndef ILPIPEINT_H
#define ILPIPEINT_H

    /*  Private definitions for special hooks into /ilc/ilpipe.c .  Not needed
        to access normal pipe functions!
    */
#ifndef ILINT_H
#include "ilint.h"
#endif
#ifndef ILIMAGE_H
#include "ilimage.h"
#endif

    /*  Values for pipe producerCode, defines what producer was */
#define IL_PIPE_FEED_IMAGE      0               /* previous was ilFeedFromImage() */
#define IL_PIPE_IMAGE           1               /* previous was ilReadImage() */
#define IL_PIPE_NOT_IMAGE       2               /* previous element not an image */

        /*  In /ilc/ilpipe.c : */

    /*  Called by ilReadImage() to set *pImage as the producer to this pipe. 
        The pipe MUST be in the empty state.  "needProducerThrottle" is true iff a 
        throttle must be inserted before next element added by ilAddPipeElement().
    */
IL_EXTERN ilBool _ilAddProducerImage (
    ilPipe              pipe,
    ilImagePtr pImage,
    unsigned int        producerCode,
    long                height,
    long                stripHeight,
    ilBool              constantStrip,
    ilBool              needProducerThrottle
    );

        /*  Called by ilFeedPipe() to set info for the first element in the given pipe,
            which will be ilFeedProducerThrottleExecute().  If feeding compressed data:
            "start" is the byte offset into the compressed data, and "nCompBytes" is
            the number of bytes at that offset.  If uncompressed data: "start" is the
            starting line, and "nCompBytes" is ignored.  Returns FALSE if the pipe was 
            not started with a ilFeedFromImage() producer.
        */
IL_EXTERN ilBool _ilSetFeedPipeData (
    ilPipe              pipe,
    long                start,
    long                nLines,
    long                nCompBytes
    );

        /*  Called by ilConvert() to "typecast" the pipe image to the given des and/or
            format (ignored if pDes/pFormat null).  No validation is done on the result!
        */
IL_EXTERN void _ilSetPipeDesFormat (
    ilPipe              pipe,
    ilImageDes         *pDes,
    ilImageFormat      *pFormat
    );

        /*  Add the given object to a list of objects to be destroyed when this pipe
            is emptied.  The object's refCount should be inc'd before this call, so that
            the object cannot be freed until this pipe is destroyed.
            The pipe must be in the forming or complete state.
            NOTE: a hard limit exists on the # of objects which can be added; not intended
            for unlimited use, e.g. by filters!
            Sets context->error to error code or success.
        */
IL_EXTERN ilBool _ilAddPipeDestroyObject (
    ilPipe              pipe,
    ilObject            object
    );


        /*  In /ilc/ilrwimage.c : */

        /*  Insert a "filter" which merely copies the pipe image to the dest image.
            This is necessary when the pipe is "<Read Image> <WriteImage>", because both
            ilReadImage() allows the next filter to read directly from its image, and 
            ilWriteImage() allows the previous filter to write directly to its image,
            but together, there needs to be a filter in between.
                "height" is the height of the pipe image.
        */
IL_EXTERN ilBool _ilInsertCopyFilter (
    ilPipe              pipe,
    long                height
    );

        /*  Insert a "filter" which copies the compressed pipe image to the dest image.
            This is the equivalent to ilInsertCopyFilter(), except for compressed images.
        */
IL_EXTERN ilBool _ilInsertCompressedCopyFilter (
    ilPipe              pipe
    );

    /*  Called by ilAddPipeElement to add a "throttle" pipe element to feed
        strips of height "stripHeight" or zero (0) to the next element.  
            For uncompressed images only: if "constantStrip" is true, the strips out of
        this filter must all be "stripHeight" or zero lines high, except for the last 
        strip;  if false, the height of each strip must be <= "stripHeight".
            If "producerCode" is not IL_PIPE_NOT_IMAGE, then the throttle is a 
        logical producer: the pipe began with an ilReadImage() or ilFeedFromImage()
        and must signal IL_ERROR_LAST_STRIP; else the throttle is not the producer.
        This flag is used in place of "tempImage" in pipe info which IS INCORRECT -
        ilExecutePipe() changes it to avoid infinite recursion.
            The strip height from this filter is returned to "*pStripHeight", and 
        "*pConstantStrip" is set true if constant strips are output.
            Returns: true if success, else false.
    */
IL_EXTERN ilBool _ilAddThrottlePipeElement (
    ilPipe              pipe,
    long                stripHeight,
    ilBool              constantStrip,
    unsigned int        producerCode,
    long               *pStripHeight,
    ilBool             *pConstantStrip
    );

#endif
