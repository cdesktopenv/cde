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
 *+SNOTICE
 *
 *
 *  $XConsortium: Buffer.C /main/6 1996/04/21 19:47:10 drk $
 *
 *  RESTRICTED CONFIDENTIAL INFORMATION:
 *  
 *  The information in this document is subject to special
 *  restrictions in a confidential disclosure agreement between
 *  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *  document outside HP, IBM, Sun, USL, SCO, or Univel without
 *  Sun's specific written approval.  This document and all copies
 *  and derivative works thereof must be returned or destroyed at
 *  Sun's request.
 *
 *  Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <DtMail/Buffer.hh>

#include <string.h>
#ifdef __osf__
#include <stdlib.h>
#else
#if defined(USL) && (OSMAJORVERSION == 2)
extern "C" {
#endif
#if defined(USL) && (OSMAJORVERSION == 2)
};
#endif
#endif // __osf__
#include <stdio.h>
#include <stdlib.h>
#include <DtMail/Threads.hh>

const int Buffer::defaultchunksize = 16 * 1024;


// buffer.C -- implement buffer class


// initialization
void
BufferMemory::initBuffer(int chunksize)
{
    _firstreader = NULL;
    _firstchunk = NULL;
    _lastchunk = NULL;
    _totalsize = 0;
    _chunksize = chunksize;

    _mutex = MutexInit();
}

BufferMemory::BufferMemory()
{
    initBuffer(defaultchunksize);
}

BufferMemory::BufferMemory(int chunksize)
{
    initBuffer(chunksize);
}


BufferMemory::~BufferMemory()
{
	MutexLock mutex(_mutex);

	// free the data in the buffer
	Chunk *c = _firstchunk;
	while (c) {
		Chunk *next;

		next = c->_nextchunk;
		free(c->_buffer);
		free(c);
		c = next;
	}


	// disassociate us from all the readers
	BufReaderMemory *r = _firstreader;
	while (r) {
		r->_buffer = NULL;
		r = r->_nextreader;
	}

	// free the mutex
	mutex.unlock_and_destroy();
}




Buffer::Buffer() { }
Buffer::~Buffer() { }
BufReader::~BufReader() { }


// buffer memory class functions
int
BufferMemory::appendData(const char *user_buffer, int length)
{
    int numwritten = 0;
    int numbytes;

    MutexLock mutex(_mutex);

    while (numwritten < length) {
	if (! _lastchunk || _lastchunk->_chunksize <= _lastchunk->_currentend) {
	    // we need to get a new data chunk
	    if (newChunk(length - numwritten) < 0) {

		// we had an allocation error
		return (numwritten);
	    }
	}

	// number of free bytes in data chunk
	numbytes = _lastchunk->_chunksize - _lastchunk->_currentend;

	// don't write more than remaining bytes in user's request
	if (numbytes > length - numwritten) {
	    numbytes = length - numwritten;
	}

	// do the copy
	(void) memcpy(&_lastchunk->_buffer[_lastchunk->_currentend],
		user_buffer, numbytes);

	// update the counters
	numwritten += numbytes;
	_totalsize += numbytes;
	_lastchunk->_currentend += numbytes;
	user_buffer += numbytes;

    }

    return (numwritten);
}


// add a new data chunk to the buffer
int
BufferMemory::newChunk(int size)
{
    // make sure we get at least the default chunksize
    if (size < _chunksize) size = _chunksize;

    // allocate the data regions
    Chunk *d = (Chunk *) malloc(sizeof (Chunk));
    if (! d) return (-1);

    d->_buffer = (char *) malloc(size);
    if (! d->_buffer) {
	free (d);
	return (-1);
    }

    d->_chunksize = size;
    d->_currentend = 0;

    // now splice it into the list
    // lock should already have been called

    d->_nextchunk = NULL;
    if (_firstchunk == NULL) _firstchunk = d;

    if (_lastchunk) {
	_lastchunk->_nextchunk = d;
    }
    _lastchunk = d;

    return (0);
}


BufReader *
BufferMemory::getReader(void)
{
    BufReader *r;

    MutexLock mutex(_mutex);

    r = new BufReaderMemory(this);

    return (r);
}


unsigned long
BufferMemory::iterate(Buffer::CallBack callback, ...)
{
    va_list va;
    MutexLock mutex(_mutex);
    Chunk *c;
    unsigned long ret = 0;
 
    va_start(va, callback);
 
    // walk through the chunk list, calling the callback for each one
    for (c = _firstchunk; c; c = c->_nextchunk) {

	// don't bother with null length chunks
	if (c->_currentend == 0) continue;

	// do the callback
        ret = (*callback)(c->_buffer, c->_currentend, va);

	// check for problems
        if (ret) break;
    }
 
    return (ret);
}
 



//------------- beginning of bufreader code

BufReader::BufReader() {}

BufReaderMemory::BufReaderMemory(BufferMemory *b)
{

    MutexLock mutex(b->_mutex);

    _buffer = b;
    _nextreader = b->_firstreader;
    _prevreader = NULL;

    if (b->_firstreader) {
	b->_firstreader->_prevreader = this;
    }
    b->_firstreader = this;

    _currentoffset = 0;
    _currentchunk = NULL;
}


int
BufferMemory::getSize()
{
    return (_totalsize);
}


int
BufReaderMemory::getData(char *user_buffer, int length)
{
    int numread = 0;
    int numbytes;

    if (! _buffer) {
	// the buffer has already been freed
	return (0);
    }

    MutexLock mutex(_buffer->_mutex);

    // handle starup case
    if (_currentchunk == NULL) {
	_currentchunk = _buffer->_firstchunk;

	// see if buffer is still empty
	if (!_currentchunk) {
	    return (0);
	}
    }

    do {
	// space in buffer
	numbytes = _currentchunk->_currentend - _currentoffset;

	if (numbytes <= 0) {
	    // we are at the end of this data chunk
	    if (_currentchunk->_nextchunk == NULL) {
		// we are at the end of the valid data
		break;
	    }

	    // advance to the next chunk
	    _currentchunk = _currentchunk->_nextchunk;
	    _currentoffset = 0;

	}

	// don't read more than user requested
	if (numbytes > length - numread) {
		numbytes = length - numread;
	}

	// copy the data
	memcpy(user_buffer, &_currentchunk->_buffer[_currentoffset], numbytes);

	// update all the pointers
	numread += numbytes;
	user_buffer += numbytes;
	_currentoffset += numbytes;

    } while (numread < length);

    return (numread);
}


BufReaderMemory::~BufReaderMemory()
{

    if (! _buffer) return;
    MutexLock mutex(_buffer->_mutex);

    // unlink us from the buffer's list
    if (_nextreader) {
	_nextreader->_prevreader = _prevreader;
    }

    if (_prevreader) {
	    _prevreader->_nextreader = _nextreader;
    } else {
	// ASSERT(_buffer->_firstreader == this);
	_buffer->_firstreader = _nextreader;
    }
}














