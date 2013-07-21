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
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/******************************************************************************
 ******************************************************************************
 **
 ** DtEnvMap.c
 **
 ** $TOG: DtEnvMap.c /main/7 1998/07/30 12:11:59 mgreess $
 **
 ** Map the path elements of environment strings as specified by yet
 ** another environment string from the local host to a remote host.
 ** When possible, cache the results since filename mapping is expensive.
 **
 ** DTENVMAPFORREMOTE="NAME1[:NAME2[...]]"
 **
 ** NAME1=path1:path2
 ** NAME2=path3:path4:...
 **
 ******************************************************************************
 *****************************************************************************/

#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <DtNlUtils.h>
#include <Tt/tt_c.h>
#include "DtSvcLock.h"

/******************************************************************************
 ******************************************************************************
 **
 ** Data Structures and Defines.
 **
 **/

/**********************************************************
 *
 * What to map...
 *
 * ... is specified by either an environment variable or 
 * a global X resource.  If present, the X resource acts
 * globally, and is referred to on every attempt to map.
 * If an environment variable is set, it overrides the X
 * resource setting, thereby allowing for local control
 * of individual process threads (by way of propagated
 * environments).
 */

#define _DTENV_MAP_RESOURCE_NAME   "dtEnvMapForRemote"
#define _DTENV_MAP_RESOURCE_CLASS  "DtEnvMapForRemote"
#define _DTENV_MAP_ENV_VAR         "DTENVMAPFORREMOTE"

/**********************************************************
 *
 * The primary caching/mapping data structures.
 */

typedef struct {
    char          *localEnvVarPtr;	/* original ptr, used for restoring */
    char          *localEnvVarCopy;	/* eg, /users/foo:/usr/bar */
    char          *mappedEnvVarPtr;	/* eg, PATH=/nfs/.../foo:/nfs/.../bar */
} cachedEnvVar;

typedef struct {
    char          *remoteHost;		/* host to map env vars to */
    int            cacheHit;		/* usage rate for this mapping info */
    char          *mapListStr;		/* copy of DTENVMAPFORREMOTE=... */

    int            mapListCnt;		/* how many _real_ env vars to map */
    char         **mapList;		/* simple list of env var names to map*/
    cachedEnvVar  *mapListDetails;	/* list of map info per env var name */
} cacheForTargetHost; 

/*
 * Cache information.  Keep mapping information for MAX_HOSTS_CACHED
 * targetHosts.
 */
#define MAX_HOSTS_CACHED 8
static cacheForTargetHost cachePoolG[MAX_HOSTS_CACHED];

/*
 * Track the most recent targetHost.
 */
static char *mostRecentRemoteHostG;

/**********************************************************
 *
 * Misc defines.
 *
 * To reduce malloc calls when creating lists, malloc
 * "list" in blocks:
 *
 *      char *list[0..15 , 16..31, 32..63, ...]
 */
#define MALLOC_BUMP_SIZE 16

#define freeAndNull(ptr)   if (ptr) free(ptr);    ptr = NULL;
#define ttfreeAndNull(ptr) if (ptr) tt_free(ptr); ptr = NULL;

/**********************************************************
 *
 * Functionality hooks.
 *
 *    - _DTENV_SUPPORT_COMMA_SEPARATED - turn on code that
 *           special cases some environment variables and
 *           treats them as comma separated and possibly
 *           host qualified.
 *
 *    - _DTENV_SUPPORT_MAPERROR_CACHING - turn on code that
 *           would cache error codes when mapping files in
 *           addition to caching successfully mapped files.
 *           In configurations where mapping errors are
 *           common, this could be a big help, but on the
 *           downside, a mapping error that is the result
 *           of a temporary network error (for example)
 *           could get locked into a cache.
 *
 *    - _DTENV_OPTIMIZATION_LOCALHOST - turn on code that
 *           would prevent even trying to map/cache environment
 *           variables if the targetHost was the localhost.
 */           
#define _DTENV_SUPPORT_COMMA_SEPARATED  1
#undef  _DTENV_SUPPORT_MAPERROR_CACHING
#undef  _DTENV_OPTIMIZATION_LOCALHOST


/**********************************************************
 *
 * Performance hooks.
 *
 *    - DTENV_PERF_HOOK - instrunment file mapping code for
 *                        performance measuring.
 */
#undef DTENV_PERF_HOOK

#ifdef DTENV_PERF_HOOK
#include <time.h>

int stopwatch_repeat_rate = 10;
unsigned long stopwatch_tt_file_netfile;
unsigned long stopwatch_tt_netfile_file;
#endif /* DTENV_PERF_HOOK */


/******************************************************************************
 ******************************************************************************
 **
 ** _DtEnv_tt_file_netfile()
 ** _DtEnv_tt_netfile_file()
 **
 ** Both are caching versions built on the non-caching Tooltalk versions.
 **
 ** The one flaw in both is that if the filesystem topology changes
 ** (eg, NFS mounts, symlinks, etc), old and incorrect mapping information
 ** may be returned. To minimize this, a cached mapping will only be used
 ** so many times before being recomputed.  An alternative (not implemented
 ** here) would be to recompute based on age of the mapping via
 ** gettimeofday().
 **
 **     ******************* XXX Alert **************************
 **
 **     Tooltalk currently allocates a range of char*'s
 **     and then associates them with a range of error
 **     messages (ala tt_ptr_error()).  In effect, the
 **     allocated char*'s become constants within Tooltalk.
 **
 **     This code takes this into account when returning
 **     values.   Non-reserved char*'s are strdup()ed
 **     often, while reserved char*'s are not to preserve
 **     their ptr value.
 **
 ** tjg: Other than differing cache repositories, all the onion-skin
 ** routines below look the same.   Probably better to have one
 ** core routine to minimize code size.
 **/

/*
 * Per cache setup (ie, one for _DtEnv_tt_file_netfile(), one for
 * _DtEnv_tt_netfile_file()), start off by caching SIZE_START
 * mappings, growing by SIZE_BUMP as needed, while limiting the
 * total cache growth to SIZE_MAX mappings.
 */
#define CACHE_FILEFRAG_SIZE_START     15
#define CACHE_FILEFRAG_SIZE_BUMP      15
#define CACHE_FILEFRAG_SIZE_MAX       45

/*
 * After a cached mapping is used REMAP_AFTER times, age it out
 * and force a recomputation of the mapping from scratch.
 */
#define CACHE_FILEFRAG_REMAP_AFTER    25

/*
 * After RESET_PRI hits on a cache of size SIZE_MAX, reset all
 * cacheHit counters.  This will allow new mappings to work into
 * the cache against mappings that might have been popular long
 * ago.
 */
#define CACHE_FILEFRAG_RESET_PRI     200


/******************************************************************************
 *
 * _DtEnv_tt_file_netfile()
 *
 * A caching version of tt_file_netfile().
 */
typedef struct {
    char          *pathFragOrig;	/* eg, /usr/dt */
    char          *pathFragMapped;	/* eg, <from tt_file_netfile> */
    int           cacheHit;		/* usage of */
} cachedFileFrag;

char *_DtEnv_tt_file_netfile(
    const char *filename)
{
    static int             first_time  = 1;
    static int             fragListAvail = CACHE_FILEFRAG_SIZE_START;
    static int             fragListCnt = 0;
    static cachedFileFrag *fragList;
    static int             cacheGen = 0;
    static int             hitIdxStart = 0;

    char           *netpath;
    int             hitval, hitIdx, i;
    cachedFileFrag *tmpCffP;
    char           *tmpStr;
    int			   newCount = fragListCnt;

    _DtSvcProcessLock();

    if (first_time) {
	fragList = (cachedFileFrag *) calloc( fragListAvail,
						sizeof(cachedFileFrag) );
	first_time = 0;
    }

    /*
     * Take care of the obvious.
     */
    if (!filename) {
	_DtSvcProcessUnlock();
	return( (char *) NULL );
    }

    /*
     * Look for existing answer in cache.
     *
     * While at it, also look for least used entry just in case.
     */
    if (fragListCnt)
	hitIdxStart = (hitIdxStart + 7) % fragListCnt;
    else
	hitIdxStart = 0;

    hitIdx = hitIdxStart;
    hitval = fragList[hitIdx].cacheHit;
    tmpCffP = fragList;				/* walk rather than index */

    for ( i = 0; i < fragListCnt; i++ ) {
	if (tmpCffP->cacheHit && !strcmp( filename, tmpCffP->pathFragOrig ) ) {
	    break;
	}

	if (tmpCffP->cacheHit < hitval) {
	    hitIdx = i;
	    hitval = tmpCffP->cacheHit;
	}
	tmpCffP++;
    }

    /*
     * Decide what was found.
     */
    if ( i != fragListCnt ) {
	/*
	 * Found a cached entry.
	 */
	hitIdx = i;
	if ( fragList[hitIdx].cacheHit++ > CACHE_FILEFRAG_REMAP_AFTER ) {
	    /*
	     * This looks like an old entry, so re-compute it.
	     */
	    freeAndNull( fragList[hitIdx].pathFragOrig );
	    ttfreeAndNull( fragList[hitIdx].pathFragMapped );
	    fragList[hitIdx].cacheHit = 0;	/* 0 means remap below */
	}
    }
    else {
	/*
         * Did not find a cache entry, so scrounge around for
	 * a new entry.
	 */
	if ( fragListCnt < fragListAvail ) {
	    /*
	     * Use next already-malloc'ed cacheEntry.
	     */
	    hitIdx = fragListCnt;
            newCount = fragListCnt + 1;
	}
	else if ( fragListCnt < CACHE_FILEFRAG_SIZE_MAX ) {
	    /*
	     * Can grow fragList[]
	     */
	    fragListAvail += CACHE_FILEFRAG_SIZE_BUMP;
	    fragList = (cachedFileFrag *) realloc( (char *) fragList,
					sizeof(cachedFileFrag) * fragListAvail);
	    /*
	     * Zero out new memory.
	     */
	    memset( fragList + (fragListAvail-CACHE_FILEFRAG_SIZE_BUMP),
		    0, CACHE_FILEFRAG_SIZE_BUMP*sizeof(cachedFileFrag) );
	    hitIdx = fragListCnt;
            newCount = fragListCnt + 1;
	}
	else {
	    /*
	     * Last resort - bump out the least used entry.
	     */
	    freeAndNull( fragList[hitIdx].pathFragOrig );
	    ttfreeAndNull( fragList[hitIdx].pathFragMapped );

	    /*
	     * Since the cache is 100% full, ocassionally reset
	     * everyone's cacheHit rate so entries that were only
	     * popular long ago don't get locked in.
	     */
	    if ( cacheGen++ > CACHE_FILEFRAG_RESET_PRI ) {
		cacheGen = 0;
		tmpCffP = fragList;
		for ( i = 0; i < fragListCnt; i++ ) {
		    tmpCffP->cacheHit = 1;
		    tmpCffP++;
		}
	    }
	}

	fragList[hitIdx].cacheHit = 0;		/* 0 means remap below */
    }

    if ( ! fragList[hitIdx].cacheHit ) {
	/*
	 * Need to perform mapping.
	 */
	netpath = tt_file_netfile( filename );

#ifdef _DTENV_SUPPORT_MAPERROR_CACHING
	fragList[hitIdx].pathFragOrig = strdup( filename );
	fragList[hitIdx].cacheHit = 1;
	fragList[hitIdx].pathFragMapped = netpath;

        fragListCnt = newCount;
#else
	if ( tt_ptr_error(netpath) == TT_OK ) {
	    fragList[hitIdx].pathFragOrig = strdup( filename );
	    fragList[hitIdx].cacheHit = 1;
	    fragList[hitIdx].pathFragMapped = netpath;
 
            /*
             * Only change the count if we are successful in adding 
             * a new entry.
             */
            fragListCnt = newCount;
	}
	else {
	    /*
	     * Don't cache errors.   Leave this cache slot empty
	     * and it will be rediscovered and used in the future.
	     */
	    fragList[hitIdx].cacheHit = 0;
            /*
             * Do not change the fragListCount since we don't want to 
             * add in error entries.
             */
	}
#endif /* _DTENV_SUPPORT_MAPERROR_CACHING */
    }

    /*
     * Dig out answer and return it.
     */
#ifdef _DTENV_SUPPORT_MAPERROR_CACHING
    if ( tt_ptr_error(netpath) == TT_OK )
#else
    if ( fragList[hitIdx].cacheHit )
#endif /* _DTENV_SUPPORT_MAPERROR_CACHING */
    {
	/*
	 * Return a tt_free-able copy of the answer.
	 */
	tmpStr = tt_malloc( strlen(fragList[hitIdx].pathFragMapped) + 1 );
	strcpy( tmpStr, fragList[hitIdx].pathFragMapped );

	_DtSvcProcessUnlock();
	return(tmpStr);
    }
    else {
	/*
	 * See XXX comment.
	 *
	 * Since netpath is an error code, return as is.
	 */ 
	_DtSvcProcessUnlock();
	return(netpath);
    }
}


/******************************************************************************
 *
 * _DtEnv_tt_netfile_file()
 *
 * A caching version of tt_netfile_file().
 */
typedef struct {
    char          *targetHost;
    char          *pathFragOrig;	/* eg, <from tt_file_netfile> */
    char          *pathFragMapped;	/* eg, /nfs/hostb/usr/dt */
    int           cacheHit;		/* usage of */
} cachedNetfileFrag;

char *_DtEnv_tt_host_netfile_file(
    const char *host,
    const char *filename)
{
    static int             first_time  = 1;
    static int             fragListAvail = CACHE_FILEFRAG_SIZE_START;
    static int             fragListCnt = 0;
    static cachedNetfileFrag *fragList;
    static int             cacheGen = 0;
    static int             hitIdxStart = 0;

    char           *newfile;
    int             hitval, hitIdx, i;
    cachedNetfileFrag *tmpCffP;
    char           *tmpStr;
    int            newCount = fragListCnt;

    _DtSvcProcessLock();
    if (first_time) {
	fragList = (cachedNetfileFrag *) calloc( fragListAvail,
						sizeof(cachedNetfileFrag) );
	first_time = 0;
    }

    /*
     * Take care of the obvious.
     */
    if (!filename) {
	_DtSvcProcessUnlock();
	return( (char *) NULL );
    }

    if (!host) {
	/*
	 * Return a tt_free-able un-mapped copy.
	 */
	tmpStr = tt_malloc( strlen(filename) + 1 );
	strcpy( tmpStr, filename );

	_DtSvcProcessUnlock();
	return(tmpStr);
    }

    /*
     * Look for existing answer in cache.
     *
     * While at it, also look for least used entry just in case.
     */
    if (fragListCnt)
	hitIdxStart = (hitIdxStart + 7) % fragListCnt;
    else
	hitIdxStart = 0;

    hitIdx = hitIdxStart;
    hitval = fragList[hitIdx].cacheHit;
    tmpCffP = fragList;				/* walk rather than index */

    for ( i = 0; i < fragListCnt; i++ ) {
	if (tmpCffP->cacheHit && !strcmp( filename, tmpCffP->pathFragOrig ) ) {
	    if (!strcmp( host, tmpCffP->targetHost ) ) {
		break;
	    }
	}

	/*
	 * Save index of least used entry
	 */
	if (tmpCffP->cacheHit < hitval) {
	    hitIdx = i;
	    hitval = tmpCffP->cacheHit;
	}
	tmpCffP++;
    }

    /*
     * Decide what was found.
     */
    if ( i != fragListCnt ) {
	/*
	 * Found a cached entry.
	 */
	hitIdx = i;
	if ( fragList[hitIdx].cacheHit++ > CACHE_FILEFRAG_REMAP_AFTER ) {
	    /*
	     * This looks like an old entry, so re-compute it.
	     */
	    freeAndNull( fragList[hitIdx].targetHost );
	    freeAndNull( fragList[hitIdx].pathFragOrig );
	    ttfreeAndNull( fragList[hitIdx].pathFragMapped );
	    fragList[hitIdx].cacheHit = 0;	/* 0 means remap below */
	}
    }
    else {
	/*
         * Did not find a cache entry, so scrounge around for
	 * a new entry.
	 */
	if ( fragListCnt < fragListAvail ) {
	    /*
	     * Use next already-malloc'ed cacheEntry.
	     */
	    hitIdx = fragListCnt;
            newCount = fragListCnt + 1;
	}
	else if ( fragListCnt < CACHE_FILEFRAG_SIZE_MAX ) {
	    /*
	     * Can grow fragList[]
	     */
	    fragListAvail += CACHE_FILEFRAG_SIZE_BUMP;
	    fragList = (cachedNetfileFrag *) realloc( (char *) fragList,
					sizeof(cachedNetfileFrag) * fragListAvail);
	    /*
	     * Zero out new memory.
	     */
	    memset( fragList + (fragListAvail-CACHE_FILEFRAG_SIZE_BUMP),
		    0, CACHE_FILEFRAG_SIZE_BUMP*sizeof(cachedNetfileFrag) );
	    hitIdx = fragListCnt;
            newCount = fragListCnt + 1;
	}
	else {
	    /*
	     * Last resort - bump out the least used entry.
	     */
	    freeAndNull( fragList[hitIdx].targetHost );
	    freeAndNull( fragList[hitIdx].pathFragOrig );
	    ttfreeAndNull( fragList[hitIdx].pathFragMapped );

	    /*
	     * Since the cache is 100% full, ocassionally reset
	     * everyone's cacheHit rate so entries that were only
	     * popular long ago don't get locked in.
	     */
	    if ( cacheGen++ > CACHE_FILEFRAG_RESET_PRI ) {
		cacheGen = 0;
		tmpCffP = fragList;
		for ( i = 0; i < fragListCnt; i++ ) {
		    tmpCffP->cacheHit = 1;
		    tmpCffP++;
		}
	    }
	}

	fragList[hitIdx].cacheHit = 0;		/* 0 means remap below */
    }

    if ( ! fragList[hitIdx].cacheHit ) {
	/*
	 * Need to perform mapping.
	 */
	newfile = tt_host_netfile_file( host, filename );

#ifdef _DTENV_SUPPORT_MAPERROR_CACHING
	fragList[hitIdx].targetHost = strdup ( host );
	fragList[hitIdx].pathFragOrig = strdup( filename );
	fragList[hitIdx].cacheHit = 1;
	fragList[hitIdx].pathFragMapped = newfile;

        fragListCnt = newCount;
#else
	if ( tt_ptr_error(newfile) == TT_OK ) {
	    fragList[hitIdx].targetHost = strdup ( host );
	    fragList[hitIdx].pathFragOrig = strdup( filename );
	    fragList[hitIdx].cacheHit = 1;
	    fragList[hitIdx].pathFragMapped = newfile;
            /*
             * Only change the count if we are successful in adding 
             * a new entry.
             */
            fragListCnt = newCount;
	}
	else {
	    /*
	     * Don't cache errors.   Leave this cache slot empty
	     * and it will be rediscovered and used in the future.
	     */
	    fragList[hitIdx].cacheHit = 0;

	    /*
	     * Do not change the fragListCount since we are not saving
	     * error entries.
	     */
	}
#endif /* _DTENV_SUPPORT_MAPERROR_CACHING */
    }

    /*
     * Dig out answer and return it.
     */
#ifdef _DTENV_SUPPORT_MAPERROR_CACHING
    if ( tt_ptr_error(newfile) == TT_OK )
#else
    if ( fragList[hitIdx].cacheHit )
#endif /* _DTENV_SUPPORT_MAPERROR_CACHING */
    {
	/*
	 * Return a tt_free-able copy of the answer.
	 */
	tmpStr = tt_malloc( strlen(fragList[hitIdx].pathFragMapped) + 1 );
	strcpy( tmpStr, fragList[hitIdx].pathFragMapped );

	_DtSvcProcessUnlock();
	return(tmpStr);
    }
    else {
	/*
	 * See XXX comment.
	 *
	 * Since newfile is an error code, return as is.
	 */ 
	_DtSvcProcessUnlock();
	return(newfile);
    }
}


/******************************************************************************
 ******************************************************************************
 **
 ** Environment variable mapping code.
 **
 */

/******************************************************************************
 *
 * _DtEnvGetMapList()
 *
 * Fetch the environment variable who's value is a colon separated list
 * of environment variable names who's values need to be mapped.   Then
 * break down the list into an indexable array.
 */
static char **_DtEnvGetMapList(
    char  *mapListStr,
    int   *mapListCount)
{
    char **mapList;		/* decomposition of above */
    char  *tmpPtr, *tmpPtr2;
    int    availListSize;
    int    firstTime;
    _Xstrtokparams	strtok_buf;

    /*
     * Handle NULL mapListStr.
     */
    *mapListCount = 0;
    if ( !mapListStr ) {
	return( (char **) NULL );
    }

    /*
     * Create a block of string pointers - remalloc() as needed.
     */
    availListSize = MALLOC_BUMP_SIZE;
    mapList = (char **) malloc( sizeof(char *) * (availListSize) );

    /*
     * Break up the colon seperated string into an indexable array.
     */
    tmpPtr = strdup(mapListStr);		/* work copy for strtok */

    firstTime = 1;
    while (1) {
	if (firstTime) {
	    tmpPtr2 = _XStrtok( tmpPtr, ":", strtok_buf );
	    firstTime = 0;
	}
	else
	    tmpPtr2 = _XStrtok( (char *) NULL, ":", strtok_buf );

	if (tmpPtr2) {
	    /*
	     * Have possible env var name to map - make sure it exists.
	     */
	    if ( getenv(tmpPtr2) ) {
		(*mapListCount)++;
		if (*mapListCount > availListSize) {
		    availListSize += MALLOC_BUMP_SIZE;
		    mapList = (char **) realloc( (char *) mapList,
					     sizeof(char *) * (availListSize) );
	        }
		mapList[*mapListCount-1] = strdup(tmpPtr2);
	    }
	}
	else {
	    break;
	}
    }

    free(tmpPtr);

    return( (char **) mapList );
}


/******************************************************************************
 *
 * _DtEnvCleanCacheSlot()
 *
 * Free up all memory associated with a cache slot for a targetHost.
 */
static void _DtEnvCleanCacheSlot( cacheForTargetHost *targetCache )
{
    int i;


    freeAndNull( targetCache->remoteHost );

    targetCache->cacheHit = 1;

    freeAndNull( targetCache->mapListStr );

    for ( i = 0; i < targetCache->mapListCnt; i++ ) {
	freeAndNull( targetCache->mapList[i] );
	freeAndNull( targetCache->mapListDetails[i].localEnvVarCopy );
	freeAndNull( targetCache->mapListDetails[i].mappedEnvVarPtr );
	/* do not free .localEnvVarPtr - belongs to environ */
    }
    freeAndNull( targetCache->mapList );
    targetCache->mapListCnt = 0;
}


/******************************************************************************
 *
 * _DtEnvGetTargetCache()
 *
 * For a specified targetHost, find existing cache information and
 * optionally create a cache for a targetHost if one doesn't exist.
 *
 * The define MAX_HOSTS_CACHED controls how many targetHosts can
 * be cached.
 */
static cacheForTargetHost *_DtEnvGetTargetCache(
    char *targetHost,
    int   createIfNeeded)
{
    static int cacheHitGen = 0;
    static int hitIdxStart;
    int i, hitidx, hitval;


    /*
     * Handle obvious.
     */
    if (!targetHost) {
	return( (cacheForTargetHost *) NULL );
    }

    _DtSvcProcessLock();
    /*
     * Look for targetHost in current cache pool.
     */
    for ( i = 0; i < MAX_HOSTS_CACHED; i++ ) {
	if ( cachePoolG[i].remoteHost ) {
	    if ( !strcmp( targetHost, cachePoolG[i].remoteHost ) ) {
		cachePoolG[i].cacheHit++;
		break;
	    }
	}
    }

    if ( i != MAX_HOSTS_CACHED ) {
	/*
	 * targetHost is in a cache slot already.
	 */
	_DtSvcProcessUnlock();
	return( &cachePoolG[i] );
    }
    else if ( !createIfNeeded ) {
	/*
	 * No cache slot for, and we shouldn't create one either.
	 */
	_DtSvcProcessUnlock();
	return( (cacheForTargetHost *) NULL );
    }
    else {
	/*
	 * Find an empty cache slot or take over a rarely used slot.
	 */
	hitIdxStart = (hitIdxStart + 7) % MAX_HOSTS_CACHED;

	hitidx = hitIdxStart;
	hitval = cachePoolG[hitidx].cacheHit;

	for ( i = 0; i < MAX_HOSTS_CACHED; i++ ) {
	    if ( ! cachePoolG[i].remoteHost ) {
		/*
		 * Empty slot - take it.
		 */
		hitidx = i;
		break;
	    }
	    else if ( cachePoolG[i].cacheHit < hitval ) {
		hitidx = i;
		hitval = cachePoolG[i].cacheHit;
	    }
	}

	if ( cachePoolG[hitidx].remoteHost ) {
	    /*
	     * Cache was in use, clean first.
	     */
	    _DtEnvCleanCacheSlot( &cachePoolG[hitidx] );
	    cachePoolG[hitidx].remoteHost = strdup( targetHost );

	    /*
	     * Since all the slots are full, occasionally reset everyones
	     * cacheHit counters.   This gives new targetHosts a chance
	     * to compete with targetHosts that were popular long ago.
	     */
	    if ( cacheHitGen++ > 50 ) {
		for ( i = 0; i < MAX_HOSTS_CACHED; i++ ) {
		    cachePoolG[i].cacheHit = 1;
		}
		cacheHitGen = 1;
	    }
	}
	_DtSvcProcessUnlock();
	return( &cachePoolG[hitidx] );
    }
}


/******************************************************************************
 *
 * _DtEnvMapIt()
 *
 * Fill out a map cache for a single environment variable.
 */
static void _DtEnvMapIt(
    char *envVar,
    cachedEnvVar *envVarCache,
    char *targetHost)
{
    char  *separator, *tmpPtr, *tmpPtr2, swapout, *netpath;
    char  *prePend, *postPend, *newPrePend;

    char **pathList;

    int    availPathListSize, pathListCount, availEnvStrSize, len, tmpi, i;
    int    considerMapping;
    _Xstrtokparams	strtok_buf;

    /*
     * Information Layout:
     *
     *    localEnvVarPtr  = ptr to original "PATH=/users/foo:/users/bar"
     *    localEnvVarCopy = copy of original "/users/foo:/users/bar"
     *    mappedEnvVarPtr = mapped "PATH=/nfs/.../users/foo:/nfs/.../users/bar"
     */
    if ( (envVarCache->localEnvVarPtr = getenv( envVar )) ) {
	envVarCache->localEnvVarCopy = strdup( envVarCache->localEnvVarPtr );

	/* sneak back past "NAME=" portion. */
	envVarCache->localEnvVarPtr -= strlen( envVar ) + 1;
    }
    else {
	/*
	 * Nothing to map.   Punt.
	 */
	envVarCache->localEnvVarCopy = (char *) NULL;
	envVarCache->localEnvVarPtr  = (char *) NULL;
	return;
    }

#ifdef _DTENV_SUPPORT_COMMA_SEPARATED
    /*
     * Pick between colon-separated and comma-separated host-qualified
     * mapping code.
     */
    if ( !strcmp(envVar, "DTDATABASESEARCHPATH") ) {
	/*
	 * comma-separated and host-qualified mapping.
	 */
	separator = ",";
    }
    else {
	/*
	 * colon-separated mapping.
	 */
	separator = ":";
    }
#else
    separator = ":";
#endif /* _DTENV_SUPPORT_COMMA_SEPARATED */

    /*
     * Break path list into elements
     */
    availPathListSize = MALLOC_BUMP_SIZE;
    pathListCount = 0;
    pathList = (char **) malloc( sizeof(char *) * availPathListSize );

    /*
     * Break up path list into an array of path elements.
     */
    tmpPtr = strdup( envVarCache->localEnvVarCopy );		/* work copy */

    while (1) {
	if (!pathListCount)
	    tmpPtr2 = _XStrtok( tmpPtr, separator, strtok_buf );
	else
	    tmpPtr2 = _XStrtok( (char *) NULL, separator, strtok_buf );

	if (tmpPtr2) {
	    pathListCount++;
	    if (pathListCount > availPathListSize) {
		availPathListSize += MALLOC_BUMP_SIZE;
		pathList = (char **) realloc( (char *) pathList,
					sizeof(char *) * availPathListSize );
	    }
	    pathList[pathListCount-1] = strdup( tmpPtr2 );
	}
	else {
	    break;
	}
    }
    free( tmpPtr );

    /*
     * Setup new "NAME=....." string.
     */
    availEnvStrSize = strlen( envVar ) + 64;
    envVarCache->mappedEnvVarPtr = (char *) calloc( availEnvStrSize, sizeof(char) );
    strcpy( envVarCache->mappedEnvVarPtr, envVar );
    strcat( envVarCache->mappedEnvVarPtr, "=" );

    /*
     * Start mapping each path element.
     */
    for ( i = 0; i < pathListCount; i++ ) {
	prePend  = pathList[i];
	postPend = (char *) NULL;
	newPrePend = (char *) NULL;

	/*
	 * Assume we need to map this path element.
	 */
	considerMapping = 1;

#ifdef _DTENV_SUPPORT_COMMA_SEPARATED
	if ( !strcmp( separator, "," ) ) {
	    if ( DtStrchr(prePend, ':' ) ) {
		/*
		 * Host qualified elements in a comma separated list
		 * will NOT be mapped.
		 */
		considerMapping = 0;
	    }
	}
#endif /* _DTENV_SUPPORT_COMMA_SEPARATED */

	if (considerMapping) {
	    /*
	     * Tear apart and check for so called substitution characters.
	     */
	    if (( tmpPtr = DtStrchr(prePend, '%') )) {
		/*
		 * Temporarly shorten path up to substitution character.
		 */
		swapout = *tmpPtr;
		*tmpPtr = '\0';

		/*
		 * Move the dividing point back to a directory element.
		 */
		tmpPtr2 = DtStrrchr( prePend, '/' );

		/*
		 * Restore the send half of the string.
		 */
		*tmpPtr = swapout;

		if (tmpPtr2) {
		    /*
		     * Can do a split around the "/".
		     *
		     * Will have "<prePath>/" and "/<postPath>".
		     */
		    postPend = strdup( tmpPtr2 );
		    *(tmpPtr2 + mblen(tmpPtr2, MB_CUR_MAX)) = '\0';
		}
	    }

#ifdef DTENV_PERF_HOOK
	    {
		int tpi;
		extern unsigned long stopwatch_tt_file_netfile;
		extern int stopwatch_repeat_rate;

		struct timeval  start, stop;
		struct timezone junk;

		gettimeofday( &start, &junk );

		for ( tpi = 0; tpi < stopwatch_repeat_rate-1; tpi++ ) {
		    netpath = _DtEnv_tt_file_netfile( prePend );
		    if ( tt_ptr_error(netpath) == TT_OK )
			ttfreeAndNull( netpath );
		}
		netpath = _DtEnv_tt_file_netfile( prePend );

		gettimeofday( &stop, &junk );

		if (start.tv_usec > stop.tv_usec) {
		    stop.tv_usec += 1000000;
		    stop.tv_sec--;
		}

		stopwatch_tt_file_netfile += (stop.tv_usec - start.tv_usec);
		stopwatch_tt_file_netfile += (stop.tv_sec  - start.tv_sec) * 1000000;
	    }
#else
	    netpath = _DtEnv_tt_file_netfile( prePend );
#endif /* DTENV_PERF_HOOK */
	    if ( tt_ptr_error(netpath) != TT_OK ) {
		newPrePend = (char *) NULL;
	    }
	    else {
#ifdef DTENV_PERF_HOOK
		{
		    int tpi;
		    extern unsigned long stopwatch_tt_netfile_file;
		    extern int stopwatch_repeat_rate;

		    struct timeval  start, stop;
		    struct timezone junk;

		    gettimeofday( &start, &junk );

		    for ( tpi = 0; tpi < stopwatch_repeat_rate-1; tpi++ ) {
			newPrePend = _DtEnv_tt_host_netfile_file (targetHost, netpath);
			if ( tt_ptr_error(newPrePend) == TT_OK )
			    ttfreeAndNull( newPrePend );

		    }
		    newPrePend = _DtEnv_tt_host_netfile_file (targetHost, netpath);

		    gettimeofday( &stop, &junk );

		    if (start.tv_usec > stop.tv_usec) {
			stop.tv_usec += 1000000;
			stop.tv_sec--;
		    }

		    stopwatch_tt_netfile_file += (stop.tv_usec - start.tv_usec);
		    stopwatch_tt_netfile_file += (stop.tv_sec  - start.tv_sec) * 1000000;
		}
#else
		newPrePend = _DtEnv_tt_host_netfile_file (targetHost, netpath);
#endif /* DTENV_PERF_HOOK */
		if ( tt_ptr_error(newPrePend) != TT_OK ) {
		    newPrePend = (char *) NULL;
		}
		ttfreeAndNull( netpath );
	    }
	}

	/*
	 * Calculate length of the new path element to the new path list.
	 */
	tmpi = strlen(envVarCache->mappedEnvVarPtr)+1;	/* current list + ... */
	if ( i != 0 )
	    tmpi += 1;					/* separator */
	if (newPrePend)
		tmpi += strlen(newPrePend);		/* new prePend or ... */
	else
		tmpi += strlen(prePend);		/* ... old prePend */
	if (postPend)
		tmpi += strlen(postPend);		/* new postPend */

	if ( tmpi > availEnvStrSize ) {
	    /*
	     * Grow new mappedEnvVar space.
	     */
	    availEnvStrSize = tmpi + 64;
	    envVarCache->mappedEnvVarPtr = (char *) realloc(
				(char *) envVarCache->mappedEnvVarPtr,
				availEnvStrSize );
	}

	/*
	 * Add the new path element.
	 */
	if ( i != 0 )
	    strcat( envVarCache->mappedEnvVarPtr, separator );

	if (newPrePend)
	    strcat( envVarCache->mappedEnvVarPtr, newPrePend );
	else
	    strcat( envVarCache->mappedEnvVarPtr, prePend );

	if (postPend)
	    strcat( envVarCache->mappedEnvVarPtr, postPend );

	freeAndNull( prePend );		/* aka pathList[i] */
	ttfreeAndNull( newPrePend );
	freeAndNull( postPend );
    }
    freeAndNull( pathList );
}

/******************************************************************************
 *
 * _DtEnvGetMapInformation()
 */
#define _DtEnv_MAX_BUF_SIZE 1024
#define _DtEnv_NULL_GUARD(s) ((s) ? (s) : "")

static char *_DtEnvGetMapInformation( void )
{
   char        *mapInfo;
   char         nameBuf[_DtEnv_MAX_BUF_SIZE];
   char         classBuf[_DtEnv_MAX_BUF_SIZE];
   XrmValue     resource_value;
   XrmDatabase  db;
   char         *rep_type;
   int          bytesNeeded;
   char        *name;
   char        *class;

   extern char    *_DtApplicationName;		/* set in DtUtil.c */
   extern char    *_DtApplicationClass;
   extern Display *_DtDisplay;


   /*
    * See if an environment variable has been set.   If so, get
    * the map info from there.
    */
   mapInfo = getenv( _DTENV_MAP_ENV_VAR );

   if (mapInfo)
	return( XtNewString( mapInfo ) );

   /*
    * Try to get map info from the resource database.
    */
   bytesNeeded = strlen(_DTENV_MAP_RESOURCE_NAME)
                       + strlen(_DtApplicationName) + 4;
   if ( bytesNeeded > _DtEnv_MAX_BUF_SIZE )
        name = XtMalloc(bytesNeeded);
   else
        name = nameBuf;

   sprintf (name, "%s*%s",
        _DtEnv_NULL_GUARD( _DtApplicationName) , _DTENV_MAP_RESOURCE_NAME);

   bytesNeeded = strlen(_DTENV_MAP_RESOURCE_CLASS)
                       + strlen(_DtApplicationClass) + 4;
   if ( bytesNeeded > _DtEnv_MAX_BUF_SIZE )
        class = XtMalloc(bytesNeeded);
   else
        class = classBuf;
   sprintf (class, "%s*%s",
        _DtEnv_NULL_GUARD(_DtApplicationClass) , _DTENV_MAP_RESOURCE_CLASS);

   db = XtDatabase (_DtDisplay);
   if (XrmGetResource (db, nameBuf, classBuf, &rep_type, &resource_value))
      mapInfo = (char *) resource_value.addr;
   else
      mapInfo = (char *) NULL;

   if ( name != nameBuf )
        XtFree(name);
   if ( class != classBuf )
        XtFree(class);

   if (mapInfo)
	return( XtNewString( mapInfo ) );
   else
	return( (char *) NULL );
}

/******************************************************************************
 *
 * _DtEnvMapForRemote()
 *
 * Perform filename mapping on the current environment so it makes
 * sense on the target host.   The original environment is saved
 * for later restoring, and caching is used to minimize mapping
 * computations.
 */
void _DtEnvMapForRemote (char *targetHost)
{
    int    i, cacheRegen;
    char  *mapListStr, *tmpPtr;
    char **mapList;

    cacheForTargetHost *targetCache;
    int ttMark = 0;

    extern char *mostRecentRemoteHostG;

    _DtSvcProcessLock();
    if (mostRecentRemoteHostG) {
	/*
	 * Warning - a _DtEnvRestoreLocal() was not called for
	 * the most recent _DtEnvMapForRemote().  Tossing.
	 */
	freeAndNull(mostRecentRemoteHostG);
    }

    if (!targetHost) {
	/*
	 * No target host to cache.
	 */
	_DtSvcProcessUnlock();
	return;
    }

    /*
     * Performance enhancement:  Check if we can map our $HOME directory 
     * to the remote host.  If we fail because the host cannot be accessed
     * then give up the attempt to map the environment now because we'll
     * run into a lot of timeouts on remote mapping failures otherwise.
     */
    ttMark = tt_mark();
    switch ( tt_ptr_error(
                _DtEnv_tt_host_netfile_file(targetHost, 
                    _DtEnv_tt_file_netfile(getenv("HOME")) ) ) )
    {
	case TT_ERR_DBEXIST:	/* cannot contact remote host */
        case TT_ERR_DBAVAIL:	/* timeouts occur trying to make contact */
        case TT_ERR_UNIMP:	/* remote server doesn't support file naming */
	    /*
             * It will do no good to attempt to map filenames to this remote host
             * So forget it -- the user may try again later.
             */
            tt_release(ttMark); /* free up tooltalk memory used for test */
	    _DtSvcProcessUnlock();
            return;  
            break;
         default:
            tt_release(ttMark); /* free up tooltalk memory used for test */
            break;
    }


#ifdef _DTENV_OPTIMIZATION_LOCALHOST
    /*
     * _DtEnvMapForRemote() is normally called from the remote execution
     * code within libDtSvc.   If by chance it gets called when spawning
     * local processes, then a test should be done here to bypass mapping
     * environment variables for a local fork/exec.
     */
    if ( <unimplemented - targetHost is local test> ) {
	mostRecentRemoteHostG = "localhost";
	_DtSvcProcessUnlock();
	return;
    }
#endif /* _DTENV_OPTIMIZATION_LOCALHOST */

    /*
     * Get list of env vars to be mapped.
     */
    mapListStr = _DtEnvGetMapInformation();

    if (!mapListStr) {
	/*
	 * Nothing to map.
	 */
	_DtSvcProcessUnlock();
	return;
    }

    /*
     * We have a targetHost that needs some mapping done.   Start
     * stashing data away.
     */
    mostRecentRemoteHostG = strdup( targetHost );
    _DtSvcProcessUnlock();

    /*
     * Find or allocate a cache entry.
     */
    targetCache = _DtEnvGetTargetCache( targetHost, 1 );

    /*
     * See if cache information for targetHost is available, and
     * if so, if it still looks valid.
     *
     * To maximize performance, an all-or-nothing regeneration of
     * the cache will be done rather than incremental regeneration
     * of some portions of the cache.   One would expect that the
     * list of variables to map and their contents would remain
     * fairly static.
     */
    if (targetCache->mapListStr) {
	if ( !strcmp( targetCache->mapListStr, mapListStr ) ) {
	    /*
	     * Atleast the list of environment variables that need
	     * to be mapped is the same as previous.
	     */
	    cacheRegen = 0;
	    for ( i = 0; i < targetCache->mapListCnt; i++ ) {
		if (( tmpPtr = getenv(targetCache->mapList[i]) )) {
		    if ( strcmp( tmpPtr,
			     targetCache->mapListDetails[i].localEnvVarCopy) ) {
			cacheRegen = 1;	/* one map entry is no longer valid */
			break;
		    }
		}
		else {
		    /*
		     * Env Var does not exist, but maybe it never did.
		     */
		    if (targetCache->mapListDetails[i].localEnvVarCopy) {
			/*
			 * Was in cache, but now no longer exists.
			 */
			cacheRegen = 1;	/* env var no longer exists */
			break;
		    }
		}
	    }
	}
	else {
	    cacheRegen = 1;	/* map list changed - need to regen cache */
	}
    }
    else {
	cacheRegen = 1;		/* need to create cache */
    }

    if (cacheRegen) {
	/*
	 * Toss out the old.
	 */
	_DtEnvCleanCacheSlot(targetCache);

	/*
	 * Bring in the new.
	 */
	targetCache->remoteHost = strdup( targetHost );
	targetCache->mapListStr = strdup( mapListStr );
	targetCache->mapList = _DtEnvGetMapList( mapListStr,
					&(targetCache->mapListCnt) );

	targetCache->mapListDetails = (cachedEnvVar *)
					malloc( sizeof(cachedEnvVar) *
					    targetCache->mapListCnt );

	for ( i = 0; i < targetCache->mapListCnt; i++ ) {
	    _DtEnvMapIt( targetCache->mapList[i],
			 &(targetCache->mapListDetails[i]),
			 targetHost );
	}
    }
    else {
	/*
	 * We can use cached information.  Even though all the
	 * environment variable "strings" match, the users
	 * (environ **) pointers may be different, so re-cache
	 * the restoration pointers.
	 */
	for ( i = 0; i < targetCache->mapListCnt; i++ ) {
	    if (( targetCache->mapListDetails[i].localEnvVarPtr =
					getenv( targetCache->mapList[i] ) )) {
		targetCache->mapListDetails[i].localEnvVarPtr -=
					strlen( targetCache->mapList[i] ) + 1;
	    }
	}
    }

    /*
     * Install the mapped environment variables.
     */
    for ( i = 0; i < targetCache->mapListCnt; i++ ) {
	putenv( targetCache->mapListDetails[i].mappedEnvVarPtr );
    }

    XtFree(mapListStr);
}


/******************************************************************************
 *
 * _DtEnvRestoreLocal()
 *
 * Presuming a _DtEnvMapForRemote() was called, _DtEnvRestoreLocal()
 * restores the original envirnment settings for a number of
 * environment variables.
 */
void _DtEnvRestoreLocal ()
{
    extern char   *mostRecentRemoteHostG;
    cacheForTargetHost  *targetCache;
    char          *tmpP;
    int            i;

    _DtSvcProcessLock();
    if (mostRecentRemoteHostG) {
#ifdef _DTENV_OPTIMIZATION_LOCALHOST
	/*
	 * See comment with the other ifdef'ed block.
	 *
	 * If localhost, then nothing to restore.
	 */
	if ( !strcmp(mostRecentRemoteHostG, "localhost") ) {
	    mostRecentRemoteHostG = (char *) NULL;
	    _DtSvcProcessUnlock();
	    return;
	}
#endif /* _DTENV_OPTIMIZATION_LOCALHOST */

	targetCache = _DtEnvGetTargetCache( mostRecentRemoteHostG, 0 );

	if (targetCache) {
	    /*
	     * Install the mapped environment variables.
	     */
	    for ( i = 0; i < targetCache->mapListCnt; i++ ) {
		tmpP = targetCache->mapListDetails[i].localEnvVarPtr;
		if ( tmpP ) {
		    putenv( targetCache->mapListDetails[i].localEnvVarPtr );
		}
	    }
	}

	freeAndNull( mostRecentRemoteHostG );
    }
    _DtSvcProcessUnlock();
}

