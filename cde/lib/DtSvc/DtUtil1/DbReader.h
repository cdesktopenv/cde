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
/*
 * File:        DbReader.h $XConsortium: DbReader.h /main/4 1995/10/26 15:03:35 rswiston $
 *
 * Description: Public include file for the database reader.
 *
 * (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */


#ifndef _Dt_DbReader_h
#define _Dt_DbReader_h

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <Dt/DbUtil.h>

#define DTRECORDIDENTIFIER NULL
#define DTUNLIMITEDFIELDS  0

/* one set of attribute/pair */
typedef struct
{
	XrmQuark                fieldName;
	char                    *fieldValue;
} DtDtsDbField;

/*
 * Opaque identifier used for identifying a database file name.  The opaque
 * identifier can be converted to an ascii string, using the function
 * _DtDbPathIdToString().
 */
#define DtDbPathId long

/*
 * All record converters should expect to be called with the parameters
 * indicated below:
 *
 *   fields:    This is an array of keyword/value pair strings, each
 *              representing one field value for this database record.  NOTE:
 *              the keywords have not been validated; this is the
 *              responsibility of the converter, and is usually accomplished
 *              by calling _DtValidateFieldNames().  The array is NULL
 *              terminated, with the last entry having both the 'fieldName'
 *              and 'fieldValue' pointers set to NULL.  The first entry always
 *              represents the record identifier, with the 'fieldName' set
 *              to the record type string, and the 'fieldValue' set to any
 *              remaining data specified on the record identifier line.
 *
 *   pathId:    Is an opaque identifier, which can be used to retrieve the name
 *              of the database file from which this record was obtained.  By
 *              calling _DtDbPathIdToString(), the database file, in
 *              "host:/path" format, can be obtained.
 *
 *   hostPrefix: If the database file was loaded from a machine other than the
 *               local machine, then this string indicates the host prefix, in
 *               "/nfs/<host>" format.  If the host was the local host, then
 *               this value is set to NULL.
 *
 *   rejectionStatus: This flag indicates whether any of the earlier record
 *                    converters had rejected this entry.  Certain classes
 *                    of converters may choose to ignore entries if they
 *                    had already been rejected.
 *
 * It is the responsibility of the converter to perform any necessary
 * verification of the passed in set of keywords; the function
 * _DtValidateFieldNames() is provided as a convenience function for performing
 * this type of validation.  Typically, the converter will write an error to
 * the user's errorlog file, if an invalid record is encountered.
 *
 * It is also the responsibility of the converter to allocate any memory needed
 * to store the new record.  The strings contained within the 'fields' array
 * will all be freed when the converter returns to _DtDbRead(), so the
 * converter should make copies of any information it wishes to use in the
 * future.  The converter is also responsible for adding the new record to
 * whatever storage array it is using to save record information.
 *
 * If the converter decides to reject the record, then it should return a
 * 'True' value.  If the record was acceptable, then 'False' should be
 * returned.
 */
typedef Boolean (*DtDbConverter) (DtDtsDbField * fields,
                                  DtDbPathId pathId,
                                  char * hostPrefix,
                                  Boolean rejectionStatus);

/*
 * This structure defines each record type which should be loaded by
 * _DtDbRead().  Since _DtDbRead() is capable of loading multiple record types
 * (i.e ACTION, DATA_CRITERIA, etc) in a single pass, the invoking function will
 * pass in an array of these structures, for which each element will define a
 * record type which should be loaded.  Each record definition requires the
 * following information:
 *
 *   recordKeyword: This is the keyword which uniquely identifies the record
 *                  which matches this definition.  Examples are "ACTION" and
 *                  "DATA_CRITERIA".
 *
 *   maxFields:     Indicates the maximum number of fields which is expected
 *                  by this record type; typically, this corresponds to the
 *                  number of distinct field keywords supported by this type
 *                  of record.  This feature is used to prevent runaway database
 *                  records from getting out of hand; a runaway database record
 *                  typically occurs when the user forgets to include line
 *                  continuation characters, and a long 'description' field
 *                  is entered.  If this parameter is set to
 *                  DTUNLIMITEDFIELDS, then the check for runaway records
 *                  will be disabled for records of this type.
 *
 *  converters:     This is a NULL-terminated array of function pointers, 
 *                  corresponding to the set of record converters which will 
 *                  be called, whenever a record of this type is encountered.  
 *                  The converters are called in order, and all converters will
 *                  be called, even if an earlier one rejects the record.  The
 *                  last entry in the array must be set to NULL.
 */
typedef struct {
   char * recordKeyword;
   int maxFields;
   DtDbConverter * converters;
} DtDbRecordDesc;


/*
 * _DtDbRead() is the function which causes the specified set of directories to
 * be searched for files ending with the specified suffix.  Although not
 * required, the set of directories to search is typically obtained by calling
 * _DtGetDatabaseDirPaths().  This function supports the loading of multiple
 * field types, all in a single pass of the database files.  This function does
 * not attempt to manage the memory used to store the information extracted from
 * the database files; it is the responsibility of the function calling this
 * procedure to initialize any storage, and it is the responsibility of the
 * record converters to allocate any memory needed to store and record, along
 * with adding the record to the appropriate storage array.
 *
 *  dirs:        The set of directories to be searched for database files.
 *
 *  suffix:      The file suffix (i.e. ".vf") used to qualify which files within
 *               the database directories should be loaded.
 *
 *  recordDescriptions: An array, where each entry describes a database record
 *                      which should be loaded during this pass of _DtDbRead().
 *                      See the definition for this structure, for more details.
 *
 *  int numRecordDescriptions:  The number of entries in the above array.
 */
extern void _DtDbRead (DtDirPaths * dirs,
                       char * suffix,
                       DtDbRecordDesc * recordDescriptions,
                       int numRecordDescriptions);


/*
 * _DtDbPathIdToString() is used to map a filename identifier, represented
 * by an opaque DtDbPathId value, into its corresponding ascii string
 * representation.  The returned string is owned by the calling application,
 * which should free it up when no longer needed.
 */
extern char * _DtDbPathIdToString ( DtDbPathId pathId );

#endif /* _Dt_DbReader_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
