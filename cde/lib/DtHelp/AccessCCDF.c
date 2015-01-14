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
/* $XConsortium: AccessCCDF.c /main/10 1996/11/01 10:09:50 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        AccessCCDF.c
 **
 **   Project:     Cde 1.0 Help Library
 **
 **   Description: This body of code handles the access routines to the
 **                legacy CCDF Help files.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#include <X11/Xos.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "CanvasError.h"
#include "AccessP.h"
#include "bufioI.h"
#include "AccessCCDFP.h"
#include "AccessCCDFI.h"
#include "FormatUtilI.h"
#include "StringFuncsI.h"
#include "HelpXlate.h"

#ifdef NLS16
#endif

/********    Private Defines      ********/
#define	LIST_INCREMENT	10
#define	BUFF_SIZE	256
/********    End Private Defines  ********/

/********    Private Function Declarations    ********/
static	char	 *CreateFileName(
			char	*path,
			char	*string );
static	int	  GetResourceInt (
			XrmDatabase  db, 
			char	*topic, 
			char	*resClass, 
			char	*resName,
			int	*ret_value);
static	char	 *GetResourceString (
			XrmDatabase	 db, 
			char		*topic, 
			char		*resClass, 
			char		*resName);
static	char	**GetResourceStringArray (
			XrmDatabase	 db, 
			char		*topic, 
			char		*resClass, 
			char		*resName);
static	int	  GetTopicMap (
			_DtHelpVolume	 vol,
			char		*target_id,
			int		 level,
			char		***ret_ids );
/********    End Private Function Declarations    ********/

/********    Private Macro Declarations        ********/
#define	GetCcdfVolumePtr(vol) \
		((CcdfVolumePtr)((vol)->vols.ccdf_vol))

#define	GetFilenameResource(vol, topic)	\
	GetResourceString((vol)->volDb, topic, "Filename", "filename")

/********    End Private Macro Declarations    ********/

/******************************************************************************
*
* Private variables used within this file.
*
*******************************************************************************/
static const char *Period    = ".";
static const char *Slash     = "/";
static const char *VolumeStr = "Volume.";
static const char *volumeStr = "Volume.";

static const struct _CcdfVolumeInfo DefaultCcdfVol =
  {
    NULL,		/* XrmDatabase volDb; */
    NULL,		/* char **topicList;  */
    NULL,		/* char *keywordFile; */
  };

/******************************************************************************
 *                             Private Functions
 ******************************************************************************/
/******************************************************************************
 * Function:	CreateFileName (char *path, char *string)
 *
 * Parameters:	path	Specifies the path to the volume.
 *		string	Specifies the file's name including the extension.
 *
 * Memory owned by caller:
 *		ptr returned
 *
 * Returns:		Non null ptr if successful, null if a failure occurs.
 *
 * errno Values:	CEErrorMalloc
 *
 * Purpose:	Creates a fully qualified path to a file based on the
 *		path given.
 *
 ******************************************************************************/
static	char *
CreateFileName (
	char	 *path,
	char	 *string )
{
    int    len = 0;
    char  *ptr;

    if ((MB_CUR_MAX == 1 || mblen (string, MB_CUR_MAX) == 1) && *string != '/'
				&& path)
      {
        /*
         * determine the length of the path.
         */
	_DtHelpCeStrrchr (path, Slash, MB_CUR_MAX, &ptr);
        if (ptr)
	    len = ptr - path + 1;
      }

    /*
     * malloc the room for the path and file name.
     */
    ptr = (char *) malloc (len + strlen(string) + 1);

    if (ptr)
      {
	/*
	 * copy the name into the destination string.
	 */
	ptr[0] = '\0';
	if (len && path)
	    strncat (ptr, path, len);
	strcat (ptr, string);
      }

    return ptr;
}

/*****************************************************************************
 * Function: GetTopicMap (_DtHelpVolume vol, char *target_id,
 *						int level, char ***ret_ids)
 *
 * Parameters:	vol		Specifies the loaded volume
 *		target_id 	The target location ID of a topic.
 *		level		The levels progressed so far. It is
 *				used to calculate how much memory
 *				is required to hold all the ids.
 *		ret_ids 	Returns a null terminated list of the
 *				location IDs of the topics between the
 *				target and the parent.
 * Memory own by caller:
 *		ret_ids
 *
 * Returns:		The number of ids put in the list so far,
 *			-1 for failure.
 *
 * errno Values:	CEErrorMalloc
 *
 *
 * Purpose:	Recursively build a list of id strings containing all of
 *		the target_id's ancestors.
 *
 *****************************************************************************/
static int
GetTopicMap (
	_DtHelpVolume	 vol,
	char		*target_id,
	int		 level,
	char		***ret_ids )
{
    int    result = -1;

    char   *idParent;

    if (_DtHelpCeGetCcdfTopicParent (vol, target_id, &idParent) == 0)
      {
	/*
	 * still not at the top
	 */
	if (idParent)
	  {
	    result = GetTopicMap (vol, idParent, level + 1, ret_ids);
	    if (result != -1)
	      {
		(*ret_ids)[result] = strdup (idParent);
		result++;
	      }
	  }
	else
	  {
	    *ret_ids = (char **) malloc (sizeof(char *) * (level + 2));
	    if ((*ret_ids) == NULL)
		return -1;

	    (*ret_ids)[level + 1] = NULL;
	    result = 0;
	  }
      }

   return result;

}  /* End GetTopicMap */

/******************************************************************************
 * Function:	char *GetResourceString (XrmDatabase db, char *topic,
 *					   char *resClass, char *resName)
 *
 * Parameters:	db		Specifies the handle to a resource database.
 *		topic		Specifies the topic whose resource value is
 *				desired.  If 'topic' is NULL, the
 *				desired resource is for the volume and
 *				not a specific topic.
 *		resClass	Specifies the resource class name.
 *		resName		Specifies the resource name.
 *
 * Return Value:	Returns the desired resource as string.
 *			This string is NOT owned by the caller and
 *			should only be read or copied.
 *
 *			Returns NULL if an error occurs.
 *
 * errno Values:	CEErrorMalloc
 *			CEErrorIllegalResource	If the resource is not in
 *						the database or if the
 *						resource NULL
 *
 * Purpose:	Get a resource value for a volume or topic.
 *
 ******************************************************************************/
static char * 
GetResourceString (
    XrmDatabase  db, 
    char	*topic, 
    char	*resClass, 
    char	*resName)
{
    int		 len;
    int		 topicLen = 0;
    char	*retVal   = NULL;
    char	*fullResName;
    char	*fullResClass;
    char	*resType;
    XrmValue	 resValue;


    if (topic != NULL)
	topicLen = strlen(topic) + strlen(Period);

    len          = strlen(volumeStr) + topicLen + 1;
    fullResName  = (char *) malloc (len + strlen (resName));
    fullResClass = (char *) malloc (len + strlen (resClass));
    if (fullResName != NULL && fullResClass != NULL)
      {
	strcpy (fullResName, volumeStr);
	strcpy (fullResClass, VolumeStr);

	if (topic != NULL)
	  {
	    strcat (fullResName, topic);
	    strcat (fullResName, Period);

	    strcat (fullResClass, topic);
	    strcat (fullResClass, Period);
	  }

	strcat (fullResName , resName);
	strcat (fullResClass, resClass);
      }
    else
	errno = CEErrorMalloc;


    if (fullResClass != NULL && fullResName != NULL)
      {
	if (XrmGetResource (db, fullResClass, fullResName, &resType, &resValue)
				&& strlen ((char *) resValue.addr))
	    retVal = (char *) resValue.addr;
	else
	    errno = CEErrorIllegalResource;
      }

    if (fullResName)
        free (fullResName);
    if (fullResClass)
        free (fullResClass);

    return (retVal);
}

/******************************************************************************
 * Function:	char **GetResourceStringArray (XrmDatabase db, char *topic,
 *					    char *resClass, char *resName)
 *
 * Parameters:	db		Specifies the handle to a resource database.
 *		topic		Specifies the topic whose resource value is
 *				desired.  If 'topic' is NULL, the
 *				desired resource is for the volume and
 *				not a specific topic.
 *		resClass	Specifies the resource class name.
 *		resName		Specifies the resource name.
 *
 * Return Value:	Returns a NULL-terminated string array containing the
 *			value of the desired resource.  The elements of the
 *			array are the strings of non-whitespace characters in
 *			the resource value.  This array is owned by the caller
 *			and should be freed (using _DtHelpCeFreeStringArray) when
 *			not needed.
 *
 * Purpose:	Get am array-valued resource for a volume or topic.
 *
 ******************************************************************************/
static char **
GetResourceStringArray (
    XrmDatabase  db, 
    char	*topic, 
    char	*resClass, 
    char	*resName)
{
    char	 *val;
    char	**valArray = NULL;
    char	 *token;
    char	 *nextC;

    /* Get the resource value which is a single string where the elements are 
       separated by white space. */
    val = GetResourceString (db, topic, resClass, resName);
    if (val != NULL)
      {
        nextC = val;
        while (nextC && *nextC != '\0')
	  {
	    nextC = _DtHelpGetNxtToken (nextC, &token);

	    if (token == NULL)
	      {
	        _DtHelpCeFreeStringArray (valArray);
	        return NULL;
	      }

	    /* If the token is a '\0' then we are at the end and we can quit. 
	       If the token is a '\n', then ignore it.  Otherwise the token
	       is an element of the array we are building. */
	    if (*token == '\0')
	        break;

	    if (*token != '\n')
	      {
	        valArray = (char **) _DtHelpCeAddPtrToArray (
					(void **) valArray, (void *) token);
	        /*
	         * If we malloc'ed ourselves out of existance...stop processing.
	         */
	        if (!valArray)
		    break;
	      }
	  }
      }

    return (valArray);
}

/******************************************************************************
 * Function:	char *GetNextKeyword (char *str, char *delimiter,
							char **ret_token)
 *
 * Parameters:	str		Specifies the string which is being parsed.
 *		delimiter	Specifies the delimiter string.
 *		ret_token	Returns the string found between the current
 *				position of the input string and the delimiter
 *				string.
 *
 *                              Newline or Null strings are
 *                              not owned by the caller.
 *
 *                              Otherwise, the memory for the returned
 *                              token is owned by the caller.
 *
 * Return Value: 	Returns a pointer to the next unparsed character
 *			in the input string. A NULL value indicates an error.
 *
 * Purpose:	Load the keywords associated with a volume.
 *
 ******************************************************************************/
static char * 
GetNextKeyword (
	char	 *str,
	char	 *delimiter,
	char	**ret_token )
{
    int		 len;
    char        *start;
    char        *token;
    short	 done;

    /* Find the next token in the string.  The parsing rules are:

         - The deliminater (except for \n) separate a keyword from
	   its list of location IDs.
         - \n is a token itself.
         - The \0 at the end of the string is a token.
     */

    /* Skip all of the whitespace and \n. */
    (void) _DtHelpCeStrspn (str, " \n", MB_CUR_MAX, &len);
    str += len;

    /* Str is pointing at the start of the next keyword.  Depending on the
       type of token, malloc the memory and copy the token value. */
    if (*str == '\0')
        token = str;

    else
      {
        /* We have some non-whitespace characters.  Find the end of */
        /* them and copy them into new memory. */
        start = str;
	done  = False;
	do
	  {
	    _DtHelpCeStrchr (str, delimiter, MB_CUR_MAX, &str);
	    if (str)
	      {
		if (strncmp (str, delimiter, strlen(delimiter)) == 0)
		    done = True;
		else
		    str++;
	      }
            else /* if (str == NULL) */
	      {
	        str = start + strlen (start);
		done = -1;
	      }
	  } while (!done);

        token = (char *) malloc ((str - start + 1) * sizeof (char));
	if (token)
	  {
            strncpy (token, start, str - start);
            *(token + (str - start)) = '\0';
	    if (done == True)
	        str += strlen (delimiter);
	  }
      }

    *ret_token = token;
    return (str);
}

/******************************************************************************
 * Function:	int TopicFilename (_DtHelpVolume vol, char *topic,
 *							char **retFname);
 *
 * Parameters:	vol		Specifies the loaded volume
 *		topic		Specifies locationID for the topic
 *		retFname	Returns the name of the file where the topic
 *				is located.
 * Memory own by caller:
 *		retFname
 *
 * Returns:	0 if successful, -1 if a failure occurs
 *
 * Purpose:	Get the name of the file where a topic is stored.
 *
 ******************************************************************************/
static int 
TopicFilename (
    _DtHelpVolume    vol, 
    char	 *topic, 
    char	**retFname)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    *retFname = GetFilenameResource (ccdfVol, topic);
    if (*retFname == NULL && errno == CEErrorIllegalResource)
	errno = CEErrorMissingFilenameRes;
    else
        *retFname = CreateFileName (vol->volFile, *retFname);

    if (*retFname == NULL)
	return (-1);

    return (0);
}

/******************************************************************************
 * Function:	int TopicFilepos (_DtHelpVolume vol, char *topic, int *retFpos);
 *
 * Parameters:	vol		Specifies the loaded volume
 *		topic		The locationID for the topic
 *		retFpos		Returns the byte offset of the start of the
 *				topic within the topic file.
 *
 * Return Value:	0 if successful, -1 if a failure occurs.
 *
 * Purpose:	Determine the position of the topic within the topic file.
 ******************************************************************************/
static int 
TopicFilepos (
    _DtHelpVolume   vol, 
    char	*topic, 
    int		*retFpos)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    if (GetResourceInt(ccdfVol->volDb, topic, "Filepos", "filepos", retFpos) == -1)
	return -1;

    return (0);
}

/******************************************************************************
 * Function:	int GetResourceInt (XrmDatabase db, char *topic,
 *			        char *resClass, char *resName, ret_value)
 *
 * Parameters:	db		Specifies the handle to a resource database.
 *		topic		Specifies the topic whose resource value is
 *				desired.  If 'topic' is NULL, the
 *				desired resource is for the volume and
 *				not a specific topic.
 *		resClass	Specifies the resource class name.
 *		resName		Specifies the resource name.
 *		ret_value	Returns an int containing the resource value.
 *
 * Return Value: 0 if successful, -1 if a failure occurs.
 *
 * Purpose:	Get an integer-valued resource for a volume or topic.
 ******************************************************************************/
static int 
GetResourceInt (
    XrmDatabase  db, 
    char	*topic, 
    char	*resClass, 
    char	*resName,
    int		*ret_value)
{
    char  *retValue;

    retValue = GetResourceString (db, topic, resClass, resName);
    if (retValue)
      {
	*ret_value = atoi(retValue);
	return 0;
      }

    return -1;
}

/******************************************************************************
 * Function:	static int LocationIDTopic (_DtHelpVolume vol, char *locId,
 *			                 char **retTopic);
 *
 * Parameters:	vol		Specifies the loaded volume
 *		locId		Specifies locationID desired.
 *		retTopic 	Returns the locationID of the topic with
 *				contains 'locId'.  This string IS owned by
 *				the caller and must be freed when no longer
 *				needed.
 *
 * Return Value:	0 if successful, -1 if a failure occurs
 *
 * Purpose:		Find which topic contains a specified locationID.
 ******************************************************************************/
static int 
LocationIDTopic (
     _DtHelpVolume   vol, 
     char	 *locId, 
     char	**retTopic)
{
    char	**allTopics;
    char	**nextTopic;
    char	**locIdList = NULL;
    char	**nextLocId;

    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    *retTopic = NULL;

    if (_DtHelpCeGetCcdfVolIdList (vol, &allTopics) != 0)
	return (-1);

    /* Check whether the locationID is a topic. */
    for (nextTopic = allTopics;
	nextTopic && *nextTopic != NULL && *retTopic == NULL; nextTopic++)
      {
	if (_DtHelpCeStrCaseCmpLatin1 (locId, *nextTopic) == 0)
	    *retTopic = strdup(*nextTopic);
      }

    /* For each topic in the volume, get its list of locationIDs and
       check them. NOTE: This code should be separated out into a public 
       _DtTopicLocationIDs function.  Then we would have a function 
       that returns all of the locationIDs in a topic, which might
       prove useful someday. */

    if (*retTopic == NULL)
      {
	for (nextTopic = allTopics;
		nextTopic && *nextTopic != NULL && *retTopic == NULL;
								nextTopic++)
	  {
	    /*
	     * valid to get a NULL on this resource, but not good
	     */
	    errno = 0;
	    locIdList = GetResourceStringArray  (ccdfVol->volDb, *nextTopic,
						"LocationIDs", "locationIDs");
	    if (locIdList == NULL && errno != CEErrorIllegalResource)
		break;

	    errno = CEErrorLocIdNotFound;
	    for (nextLocId = locIdList;
		nextLocId != NULL && *nextLocId != NULL && *retTopic == NULL;
								nextLocId++)
	      {
		if (_DtHelpCeStrCaseCmpLatin1 (locId, *nextLocId) == 0)
		    *retTopic = strdup(*nextTopic);
	      }

	    if (NULL != locIdList)
	        _DtHelpCeFreeStringArray (locIdList);
	  }
      }

    if (*retTopic == NULL)
	return (-1);

    return (0);
}

/******************************************************************************
 *                       Semi-Public CCDF Access Functions
 ******************************************************************************/
/*****************************************************************************
 * Function: int _DtHelpCeGetCcdfIdPath (_DtHelpVolume vol, char *target_id,
 *                                              char ***ret_ids)
 *
 * Parameters:  vol             Specifies the loaded volume
 *              target_id       The target location ID of a topic.
 *              ret_ids         Returns a null terminated list of the
 *                              location IDs of the topics between the
 *                              target and the parent.
 * Memory own by caller:
 *              ret_ids
 *
 * Returns:     The number of ids in the list, -1 for failure. If successful,
 *              the list will always contain at least the target_id.
 *
 * Purpose:     Get the list of ids between the top and the target id.
 *
 *****************************************************************************/
int
_DtHelpCeGetCcdfIdPath (
    _DtHelpVolume    vol,
    char              *target_id,
    char            ***ret_ids )
{
    int       idCount = 0;
    char     *topicId = NULL;

    if (LocationIDTopic (vol, target_id, &topicId) != 0)
        return -1;

    idCount = GetTopicMap (vol, topicId, 0, ret_ids);
    if (idCount != -1)
      {
        /*
         * include this entry in the count
         */
        (*ret_ids)[idCount] = topicId;
        idCount++;
      }

    return idCount;

}  /* End _DtHelpCeGetCcdfIdPath */

/******************************************************************************
 * Function:	int  _DtHelpCeGetCcdfTopicChildren (_DtHelpVolume vol,
 *							char *topic_id,
 *							char ***topics);
 *
 * Parameters:	vol		Specifies the loaded volume.
 *		topic_id	Specifies the topic for which children
 *				are desired.
 *		retTopics	Returns a NULL-terminated string array
 *				containing the list of children for a topic
 *				in the volume. This array is owned by the
 *				caller and should be freed (using
 *				_DtHelpCeFreeStringArray) when not needed.
 *
 * Memory own by caller:
 *	retTopics
 *
 * Return Value:		> 0 if successful, -1 if a failure occurs
 *
 * Purpose:	Get the list of children for a topic contained in a volume.
 *
 ******************************************************************************/
int 
_DtHelpCeGetCcdfTopicChildren (
    _DtHelpVolume   vol,
    char	  *topic_id,
    char	***retTopics)
{
    int     result;
    int     count = 0;
    char   *parent_id;
    char   *child_id;
    char   *topicId = NULL;
    char  **topicList;

    if (LocationIDTopic (vol, topic_id, &topicId) != 0)
	return -1;

    /*
     * initialize the return value
     */
    *retTopics = NULL;

    /*
     * get the list
     */
    result = _DtHelpCeGetCcdfVolIdList (vol, &topicList);
    if (result == 0)
      {
	while (*topicList && result == 0)
	  {
	    result = _DtHelpCeGetCcdfTopicParent (vol, *topicList, &parent_id);
	    if (result == 0)
	      {
		/*
		 * It's legal to get a NULL back - means the topic
		 * doesn't have a parent.
		 */
		if (parent_id &&
			_DtHelpCeStrCaseCmpLatin1 (parent_id, topicId) == 0)
	          {
		    child_id = strdup (*topicList);
		    if (child_id)
		      {
		        *retTopics = (char **) _DtHelpCeAddPtrToArray (
					(void **) (*retTopics), child_id);
		        if (*retTopics == NULL)
			    result = -1;
			count++;
		      }
		    else
		      {
			/*
			 * lost the previous data...stop processing.
			 */
			if (*retTopics)
			    _DtHelpCeFreeStringArray (*retTopics);
			*retTopics = NULL;
			result = -1;
			break;
		      }
	          }
	      }
	    else
	      {
		/*
		 * problems processing TopicParent...stop processing
		 */
		if (*retTopics)
		    _DtHelpCeFreeStringArray (*retTopics);
		*retTopics = NULL;
		break;
	      }
	    topicList++;
	  }
      }

    /*
     * free the duplicate string
     */
    if (topicId)
	free (topicId);

    if (result != 0)
	return -1;

    return count;
}

/******************************************************************************
 * Function:	int _DtHelpCeGetCcdfVolIdList (_DtHelpVolume vol, char ***topics);
 *
 * Parameters:	vol	Specifies the loaded volume.
 *		topics	Returns a NULL-terminated string array
 *			containing the ordered list of topics in the
 *			volume.  This array is NOT owned by the caller
 *			and should only be read or copied.
 *
 * Return Value:	0 if successful, -1 if a failure occurs
 *
 * Purpose:	Get the list of topics contained in a volume.
 *
 ******************************************************************************/
int 
_DtHelpCeGetCcdfVolIdList (
    _DtHelpVolume	vol,
     char	***retTopics)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    if (ccdfVol->topicList == NULL)
	ccdfVol->topicList = GetResourceStringArray (ccdfVol->volDb, NULL, 
						"TopicList", "topicList");
    *retTopics = ccdfVol->topicList;
    if (*retTopics == NULL)
      {
	if (errno == CEErrorIllegalResource)
	    errno = CEErrorMissingTopicList;
	return -1;
      }

    return 0;
}

/*****************************************************************************
 * Function: int _DtHelpCeFindCcdfId (_DtHelpVolume vol, char *target_id,
 *					char *ret_name,	int *ret_offset)
 *
 * Parameters:	vol		Specifies the loaded volume
 *		target_id	Specifies target location ID
 *		ret_name 	Returns a null terminated string
 *				containing a fully qualified path to
 *				the file that contains 'target_id'.
 *		ret_offset 	Returns the offset into 'ret_name'
 *				to the topic that contains 'target_id'.
 *
 * Memory own by caller:
 *		ret_name
 *
 * Returns:	True if successful, False if a failure occurs
 *
 * Purpose:	Find which topic contains a specified locationID.
 *****************************************************************************/
int
_DtHelpCeFindCcdfId (
    _DtHelpVolume	vol,
    char		*target_id,
    char		**ret_name,
    int		*ret_offset )
{
    char        newTarget[65];
    char       *topicId = NULL;
    int   found = False;

    strcpy (newTarget, target_id);
    _DtHelpCeUpperCase(newTarget);

    /*
     * find the location id for the topic that contains the
     * target_id (they may be the same). Then find the filename
     * and offset.
     */
    if (TopicFilename (vol, newTarget, ret_name) == -1)
      {
	/*
	 * if the reason TopicFilename failed was because we couldn't
	 * find a resource, try looking for it in the LocationIDs.
	 */
        if (errno == CEErrorMissingFilenameRes &&
		LocationIDTopic (vol, newTarget, &topicId) == 0 &&
			TopicFilename (vol, topicId, ret_name) == 0 &&
				TopicFilepos (vol, topicId, ret_offset) == 0)
	    found = True;
      }
    else if (TopicFilepos (vol, newTarget, ret_offset) != -1)
	found = True;

    /*
     * free the excess strings
     */
    if (topicId)
	free (topicId);

    return found;

}  /* End _DtHelpCeFindCcdfId */

/******************************************************************************
 * Function:   int _DtHelpCeGetCcdfTopicParent (_DtHelpVolume vol, char *topic,
 *					char **retParent)
 *
 * Parameters:	vol		Specifies the loaded volume
 *		topic		Specifies locationID for the topic
 *		retParent	Returns a string with the locationID for the
 *				topic which is the parent of the current
 *				topic.  If the current topic is at the top of
 *				the heirarchy, a NULL string is returned.
 *				This string is NOT owned by the caller and
 *				should only be read or copied.
 *
 * Return Value:	0 if successful, -1 if a failure occurs
 *
 * Purpose:	Find the parent for a topic.
 ******************************************************************************/
int 
_DtHelpCeGetCcdfTopicParent (
     _DtHelpVolume   vol, 
     char	 *topic, 
     char	**retParent)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    /* Don't return an error if we are asked for the parent of NULL, or if 
       the topic has no parent.  Both cases are valid (and used by
       _DtTopicPath). */

    *retParent = NULL;
    if (topic != NULL)
      {
	errno = 0;
	*retParent = GetResourceString(ccdfVol->volDb, topic,
							"Parent", "parent");
	if (*retParent == NULL && errno != CEErrorIllegalResource)
	    return -1;
      }

    return (0);
}

/*****************************************************************************
 * Function: int _DtHelpCeGetCcdfKeywordList (_DtHelpVolume vol,
 *
 * Parameters:	vol	Specifies the volume whose keywords need to be
 *			loaded from disk.  Once loaded, they can be 
 *			accessed through the fields of the volume structure.
 *
 * Return Value:	0 if successful, -1 if a failure occurs
 *
 * Purpose:	Load the keywords associated with a volume.
 *****************************************************************************/
int
_DtHelpCeGetCcdfKeywordList (
    _DtHelpVolume	vol)
{
    XrmDatabase	  kDb;
    char	 *keywordString;
    char	 *nextC;
    char	**topics;
    char	***topicList;
    char	 *token;
    char	 *currKeyword;
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    /* Generate the name of the keyword file.  Because volume files
       use the ".hv" suffix and keyword files use ".hvk", all we have 
       to do is append a "k". */
    /*
     * If the keywordFile is non-null, we've already tried once.
     * We want to try again, because the problem may have been
     * fixed without restarting this process.
     *
     * But don't malloc memory again, because we'll leak memory.
     * Just use what is given.
     */
    if (ccdfVol->keywordFile == NULL)
      {
        ccdfVol->keywordFile = (char *) malloc (strlen (vol->volFile) + 2);
        if (ccdfVol->keywordFile == NULL)
	    return -1;

        strcpy (ccdfVol->keywordFile, vol->volFile);
        strcat (ccdfVol->keywordFile, "k");
      }

    /*
     * check to see if it exists
     */
    if (access (ccdfVol->keywordFile, R_OK) == -1)
	return -1;

    /* Load the keyword file and get the "keywords" resource. */
    kDb = XrmGetFileDatabase (ccdfVol->keywordFile);
    if (kDb == NULL)
	return -1;

    keywordString = GetResourceString (kDb, NULL, "Keywords", "keywords");
    if (keywordString == NULL)
      {
	if (errno == CEErrorIllegalResource)
	    errno = CEErrorMissingKeywordsRes;
	XrmDestroyDatabase (kDb);
	return (-1);
      }

    /* Now parse the string into the appropriate arrays.  The string has the 
       following syntax:

       		keyword1<\>topic topic topic ... \n
		keyword2<\>topic topic topic ... \n
       
     */
    nextC = (char *) keywordString;

    while (nextC && *nextC)
      {

	/* Get the next keyword.  If we find newlines while looking for
	   the keyword, throw them away.  If the next token is the end-
	   of-file (\0), quit.
	 */
	nextC = GetNextKeyword (nextC, "<\\>", &token);

	if (token == NULL)
	  {
	    XrmDestroyDatabase (kDb);
	    if (vol->keywords)
	      {
	        _DtHelpCeFreeStringArray (vol->keywords);
	        vol->keywords = NULL;
	      }
	    if (vol->keywordTopics)
	      {
	        for (topicList = vol->keywordTopics;
						topicList; topicList++)
		    _DtHelpCeFreeStringArray (*topicList);
	        free (vol->keywordTopics);
	        vol->keywordTopics = NULL;
	      }
	    return -1;
	  }

	if (*token == '\0')
	    break;

	/* We have the next keyword.  Hang onto it and add it to the list
	   once we get the array of topics.  We don't add it yet because if
	   there are no topics we want to throw it away.  (Silently ignoring
	   keywords which specify no topics is an undocumented feature.) */

	currKeyword = token;

	/* Now get the list of topics. */
	topics = NULL;
	do
	  {
	    nextC = _DtHelpGetNxtToken (nextC, &token);

	    if (token == NULL)
	      {
		XrmDestroyDatabase (kDb);
		if (vol->keywords)
		  {
		    _DtHelpCeFreeStringArray (vol->keywords);
		    vol->keywords = NULL;
		  }
		if (vol->keywordTopics)
		  {
		    for (topicList = vol->keywordTopics;
						topicList; topicList++)
			_DtHelpCeFreeStringArray (*topicList);
		    free (vol->keywordTopics);
		    vol->keywordTopics = NULL;
		  }
		if (topics)
			_DtHelpCeFreeStringArray (topics);
		free (currKeyword);

		return -1;
	      }

	    /* If the next token is end-of-file (\0), then quit.  Otherwise
	       if the next token is a newline, then we have gotten all of
	       the topics and we need to add them to the array of topic 
	       arrays.  The final choice is that the token is a string so
	       we add it to the current array of topics. */
	    if (*token == '\0')
		break;

	    if (*token == '\n')
	      {
		/* We have all of the topics.  If the array of topics isn't
		   empty, add everything to the data structures.
		 */
		if (topics != NULL)
		  {
		    vol->keywords = (char **) _DtHelpCeAddPtrToArray (
			              (void **) vol->keywords,
				      (void *) currKeyword);
		    vol->keywordTopics = (char ***) _DtHelpCeAddPtrToArray (
					(void **) vol->keywordTopics,
					(void *) topics);
		    /*
		     * If we just malloc'ed ourselves out of existance...
		     * stop here.
		     */
		    if (vol->keywords == 0 || vol->keywordTopics == 0)
		      {
			XrmDestroyDatabase (kDb);
			if (vol->keywords)
			  {
			    free (currKeyword);
			    _DtHelpCeFreeStringArray (vol->keywords);
			    _DtHelpCeFreeStringArray (topics);
			    vol->keywords = NULL;
			  }
			if (vol->keywordTopics)
			  {
			    for (topicList = vol->keywordTopics;
							topicList; topicList++)
				_DtHelpCeFreeStringArray (*topicList);
			    free (vol->keywordTopics);
			    vol->keywordTopics = NULL;
			  }
			return -1;
		      }
		  }
		break;
	      }
	    else
	      {
		topics = (char **) _DtHelpCeAddPtrToArray ((void **) topics, 
						(void *) token);
		/*
		 * If we just malloc'ed ourselves out of existance
		 * stop here.
		 */
		if (topics == NULL)
		  {
		    free (currKeyword);
		    XrmDestroyDatabase (kDb);
		    if (vol->keywords)
		      {
			_DtHelpCeFreeStringArray (vol->keywords);
			vol->keywords = NULL;
		      }
		    if (vol->keywordTopics != NULL)
		      {
			for (topicList = vol->keywordTopics;
							topicList; topicList++)
			    _DtHelpCeFreeStringArray (*topicList);
			free (vol->keywordTopics);
			vol->keywordTopics = NULL;
		      }

		    return -1;
		  }
	      }

	  } while (nextC && *nextC);

	if (topics == NULL)
	    free (currKeyword);
      }

    XrmDestroyDatabase (kDb);

    return (0);

}  /* End _DtHelpCeGetCcdfKeywordList */

/******************************************************************************
 * Function:	int _DtHelpCeGetCcdfVolumeAbstract (_DtHelpVolume vol,
 *				char **abstract);
 *
 * Parameters:	vol	Specifies the loaded volume.
 *		abstract Returns the abstract of the volume.  This string
 *			 is owned by the caller and should be freed.
 *
 * Return Value: 0 if successful, -1 if a failure occurs
 *
 * Purpose:	Get the abstract of a volume.
 ******************************************************************************/
int 
_DtHelpCeGetCcdfVolumeAbstract (
    _DtHelpVolume	  vol,
    char		**retAbs)
{
    char          *abstract;
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    *retAbs = NULL;
    abstract = GetResourceString(ccdfVol->volDb, NULL, "Abstract", "abstract");
    if (abstract == NULL)
      {
	if (errno == CEErrorIllegalResource)
	    errno = CEErrorMissingAbstractRes;
      }
    else
        *retAbs = strdup(abstract);

    if (*retAbs == NULL)
	return (-1);

    return (0);
}

/*****************************************************************************
 * Function: int _DtHelpCeMapCcdfTargetToId (_DtHelpVolume vol,
 *					char *target_id,
 *					char *ret_id)
 *
 * Parameters:	vol		Specifies the loaded volume
 *		target_id	Specifies target location ID
 *		ret_id 		Returns the id containing the target_id.
 *				This memory *IS NOT* owned by the caller.
 *				And *MAY* point to target_id.
 *
 * Returns:	0 if successful, -1 if a failure occurs
 *
 * Purpose:	Find which topic contains a specified locationID.
 *
 *****************************************************************************/
int
_DtHelpCeMapCcdfTargetToId (
    _DtHelpVolume	vol,
    const char		*target_id,
    char		**ret_id)
{
    char        newTarget[128];
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    strcpy (newTarget, target_id);
    _DtHelpCeUpperCase(newTarget);

    /*
     * find the location id for the topic that contains the
     * target_id (they may be the same). Then find the filename
     * and offset.
     */
    *ret_id = (char *) target_id;
    if (GetFilenameResource (ccdfVol, newTarget) == NULL)
      {
	/*
	 * if the reason TopicFilename failed was because we couldn't
	 * find a resource, try looking for it in the LocationIDs.
	 */
    	if (errno == CEErrorIllegalResource &&
		LocationIDTopic (vol, newTarget, ret_id) == 0 &&
				GetFilenameResource(ccdfVol, *ret_id) != NULL)
	    return 0;

	return -1;
      }

    return 0;

}  /* End _DtHelpCeMapCcdfTargetToId */

/*****************************************************************************
 * Function: char * _DtHelpCeGetCcdfVolLocale (_DtHelpVolume vol)
 *
 * Parameters:	vol		Specifies the loaded volume
 *
 * Returns:	The pointer to the locale string if successful. Otherwise
 *		NULL.
 *
 * Purpose:	Get the locale of the specified volume.
 *		Returns the locale in a unix specific format
 *		- locale[_ter][.charset] - This memory is owned by
 *		the caller.
 *
 *****************************************************************************/
char *
_DtHelpCeGetCcdfVolLocale (
	_DtHelpVolume	vol)
{
    char	  *locale = NULL;
    char	  *charSet;
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    errno  = 0;
    locale = GetResourceString(ccdfVol->volDb, NULL, "CharSet", "charSet");
    if (_DtHelpCeStrchr(locale, ".", 1, &charSet) != 0)
      {
	charSet = locale;
        _DtHelpCeXlateOpToStdLocale(DtLCX_OPER_CCDF,charSet,&locale,NULL,NULL); 
        /* charset is owned by the volume Xrm database; don't free */
      }
    else if (NULL != locale)
        locale = strdup(locale);

    return locale;

}  /* End _DtHelpCeGetCcdfVolLocale */

/*****************************************************************************
 * Function: int _DtHelpCeGetCcdfDocStamp (_DtHelpVolume vol, char **ret_doc,
 *						char **ret_time)
 *
 * Parameters:	volume		Specifies the loaded volume
 *		ret_doc		Returns the doc id.
 *		ret_time	Returns the time stamp.
 *
 * Memory:	Caller owns the string in ret_doc and ret_time.
 *
 * Returns:	0 if successful, -2 if the volume does not contain
 *		one or the other, -1 if any other failure.
 *
 * Purpose:	Get doc id and time stamp of a volume.
 *
 *****************************************************************************/
int
_DtHelpCeGetCcdfDocStamp (
    _DtHelpVolume	vol,
    char		**ret_doc,
    char		**ret_time)
{
    int  result;
    struct stat buf;

    result = -2;
    if (ret_doc != NULL)
	*ret_doc = NULL;

    if (ret_time != NULL)
      {
	result = -1;
	*ret_time = NULL;
	if (stat(vol->volFile, &buf) == 0)
	  {
	    *ret_time = (char *) malloc (sizeof(time_t) * 3 + 1);
	    if (*ret_time != NULL)
	      {
		sprintf(*ret_time, "%u", (unsigned) buf.st_mtime);
		return -2;
	      }
	  }
      }
    return result;

}  /* End _DtHelpCeGetCcdfDocStamp */

/******************************************************************************
 * Function:	static int _DtHelpCeGetCcdfTopTopic (_DtHelpVolume vol,
 *					char **topic);
 *
 * Parameters:	vol	Specifies the loaded volume.
 *		topic	Returns the locationID for the top topic in
 *			the volume heirarchy.  This string is NOT
 *			owned by the caller and should only be read or
 *			copied.
 *
 * Return Value:	0 if successful, -1 if a failure occurs
 *
 * Purpose:	Get the top topic of a volume.
 ******************************************************************************/
int 
_DtHelpCeGetCcdfTopTopic (
    _DtHelpVolume   vol,
    char	**retTopic)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    *retTopic = GetResourceString(ccdfVol->volDb, NULL, "TopTopic", "topTopic");
    if (*retTopic == NULL)
      {
	if (errno == CEErrorIllegalResource)
	    errno = CEErrorMissingTopTopicRes;
	return (-1);
      }

    return (0);
}

/******************************************************************************
 * Function:	char  *_DtHelpCeGetCcdfVolTitle (_DtHelpVolume vol);
 *
 * Parameters:	vol		Specifies the loaded volume.
 *
 * Return Value:	The title if successful, NULL otherwise.
 *			The caller *DOES NOT* own the memory returned
 *			and *MUST NOT* modify the memory.
 *
 * Purpose:	Get the title of a volume.
 *
 ******************************************************************************/
char * 
_DtHelpCeGetCcdfVolTitle (
    _DtHelpVolume	  vol)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    return (GetResourceString (ccdfVol->volDb, NULL, "Title", "title"));
}

/******************************************************************************
 * Function:	int  _DtHelpCeGetCcdfVolumeTitle (_DtHelpVolume vol);
 *
 * Parameters:	vol		Specifies the loaded volume.
 *
 * Return Value:	The title if successful, NULL otherwise.
 *			The caller *DOES NOT* own the memory returned
 *			and *MUST NOT* modify the memory.
 *
 * Purpose:	Get the title of a volume.
 *
 ******************************************************************************/
int 
_DtHelpCeGetCcdfVolumeTitle (
    _DtHelpVolume	  vol,
    char		**ret_title)
{
    *ret_title = _DtHelpCeGetCcdfVolTitle(vol);

    if (*ret_title == NULL)
      {
	if (errno == CEErrorIllegalResource)
	    errno = CEErrorMissingTitleRes;
	return (-1);
      }

    *ret_title = strdup(*ret_title);
    if (*ret_title == NULL)
      {
	errno = CEErrorMalloc;
	return (-1);
      }
    return (0);
}

/******************************************************************************
 * Function:	int  _DtHelpCeOpenCcdfVolume (_DtHelpVolume vol);
 *
 * Parameters:	vol		Specifies the loaded volume.
 *
 * Return Value:
 *
 * Purpose:	Open a CCDF help volume
 *
 ******************************************************************************/
int 
_DtHelpCeOpenCcdfVolume (
    _DtHelpVolume	  vol)
{
    struct stat buf;
    CcdfVolumePtr ccdfVol;

    ccdfVol = (struct _CcdfVolumeInfo *) malloc(sizeof(struct _CcdfVolumeInfo));
    if (ccdfVol != NULL)
      {
	*ccdfVol = DefaultCcdfVol;
        ccdfVol->volDb = XrmGetFileDatabase(vol->volFile);
        if (ccdfVol->volDb != NULL)
          {
	    (void) stat(vol->volFile, &buf);
	    vol->check_time = buf.st_mtime;
	    vol->vols.ccdf_vol = (CcdfVolumeHandle) ccdfVol;
	    return 0;
          }
	free(ccdfVol);
      }

    return -1;
}

/******************************************************************************
 * Function:	void  _DtHelpCeCloseCcdfVolume (_DtHelpVolume vol);
 *
 * Parameters:	vol		Specifies the loaded volume.
 *
 * Return Value:
 *
 * Purpose:	Open a CCDF help volume
 *
 ******************************************************************************/
void 
_DtHelpCeCloseCcdfVolume (
    _DtHelpVolume	  vol)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    if (ccdfVol->volDb != NULL)
	XrmDestroyDatabase (ccdfVol->volDb);

    if (ccdfVol->topicList != NULL)
	_DtHelpCeFreeStringArray (ccdfVol->topicList);

    if (ccdfVol->keywordFile != NULL)
	free (ccdfVol->keywordFile);

    free(ccdfVol);
}

/******************************************************************************
 * Function:	int  _DtHelpCeRereadCcdfVolume (_DtHelpVolume vol);
 *
 * Parameters:	vol		Specifies the loaded volume.
 *
 * Return Value:
 *
 * Purpose:	Reread a CCDF volume.
 *
 ******************************************************************************/
int 
_DtHelpCeRereadCcdfVolume (
    _DtHelpVolume	  vol)
{
    CcdfVolumePtr  ccdfVol = GetCcdfVolumePtr(vol);

    if (ccdfVol->volDb != NULL)
	XrmDestroyDatabase (ccdfVol->volDb);

    if (ccdfVol->topicList != NULL)
	_DtHelpCeFreeStringArray (ccdfVol->topicList);

    if (ccdfVol->keywordFile != NULL)
	free (ccdfVol->keywordFile);

    ccdfVol->topicList   = NULL;
    ccdfVol->keywordFile = NULL;
    ccdfVol->volDb       = XrmGetFileDatabase(vol->volFile);

    if (ccdfVol->volDb != NULL)
	return 0;

    return -1;
}

/******************************************************************************
 * Function:	char *_DtHelpCeGetResourceString (XrmDatabase db, char *topic,
 *					   char *resClass, char *resName)
 *
 * Parameters:	db		Specifies the handle to a resource database.
 *		topic		Specifies the topic whose resource value is
 *				desired.  If 'topic' is NULL, the
 *				desired resource is for the volume and
 *				not a specific topic.
 *		resClass	Specifies the resource class name.
 *		resName		Specifies the resource name.
 *
 * Return Value:	Returns the desired resource as string.
 *			This string is NOT owned by the caller and
 *			should only be read or copied.
 *
 *			Returns NULL if an error occurs.
 *
 * errno Values:	CEErrorMalloc
 *			CEErrorIllegalResource	If the resource is not in
 *						the database or if the
 *						resource NULL
 *
 * Purpose:	Get a resource value for a volume or topic.
 *
 ******************************************************************************/
char * 
_DtHelpCeGetResourceString (
    XrmDatabase  db, 
    char	*topic, 
    char	*resClass, 
    char	*resName)
{
    return (GetResourceString(db, topic, resClass, resName));
}

/******************************************************************************
 * Function:	char **_DtHelpCeGetResourceStringArray (XrmDatabase db,
 *				char *topic, char *resClass, char *resName)
 *
 * Parameters:	db		Specifies the handle to a resource database.
 *              topic           Specifies the topic whose resource value
 *				is desired.  If 'topic' is NULL, the
 *				desired resource is for the volume and
 *				not a specific topic.
 *		resClass	Specifies the resource class name.
 *		resName		Specifies the resource name.
 *
 * Return Value:        Returns a NULL-terminated string array
 *			containing the value of the desired resource.
 *			The elements of the array are the strings of
 *			non-whitespace characters in the resource value.
 *			This array is owned by the caller and should be
 *			freed (using _DtHelpCeFreeStringArray) when not
 *			needed.
 *
 * Purpose:	Get am array-valued resource for a volume or topic.
 *
 ******************************************************************************/
char **
_DtHelpCeGetResourceStringArray (
    XrmDatabase  db, 
    char	*topic, 
    char	*resClass, 
    char	*resName)
{
    return (GetResourceStringArray(db, topic, resClass, resName));
}
