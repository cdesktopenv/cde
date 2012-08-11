/*
 *+SNOTICE
 *
 *
 *      $XConsortium: Buffer.hh /main/4 1996/04/21 19:44:23 drk $
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement bertween
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *      Sun's specific written approval.  This documment and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
 
#ifndef _DTMAILBUFFER_HH
#define _DTMAILBUFFER_HH

#include <stdarg.h>

// these classes are provided in order to have a logical array whose
// size is not known at creation time.  The DtMailBuffer class
// holds the data, and for the moment holds the only "write" operation
// (which is append).  In addition, to be thread safe, there is
// a separate class for reading, so that you can have separate
// "seek points" in the read buffer in different procedures.

class BufReader;

class Buffer {


    protected:
	static const int defaultchunksize;

    public:

	Buffer();

	// actually add data.  result is # of bytes written.  Can
	// fail if underlying write fails, or if out of memory.
	virtual int appendData(const char *user_buffer, int length) = 0;

	// an interator function that gets called so that eventually
	// the entire buffer has been passed through
	typedef unsigned long (*CallBack)(const char *buffer, int len, va_list);
	virtual unsigned long iterate(CallBack, ...) = 0;

	// get a new reader object
	virtual BufReader *getReader(void) = 0;

	virtual int getSize(void) = 0;	// get total size of the buffer


    protected:
	virtual ~Buffer(void);
    private:
	Buffer(const Buffer&);		// try and avoid copies


};

// subsidiary classes
class BufReader {
    public:
	// actually read data into a user specified buffer
	virtual int getData(char *user_buffer, int length) = 0;
	virtual ~BufReader(void);

    protected:
	BufReader();
    private:
	BufReader(const BufReader &);
};


class BufReaderMemory;
class BufferMemory : public Buffer {

    public:
	class Chunk;
	friend class BufReaderMemory;

	BufferMemory(int chunksize);
	BufferMemory();
	virtual ~BufferMemory(void);

	// actually add data.  result is # of bytes written.  Can
	// fail if underlying write fails, or if out of memory.
	virtual int appendData(const char *user_buffer, int length);

	virtual unsigned long iterate(Buffer::CallBack, ...);

	// get a new reader object
	virtual BufReader *getReader(void);

	virtual int getSize(void);      // get total size of the buffer

    private:
#if !defined(linux) && !defined(CSRG_BASED)
	class Chunk;
#endif

	BufferMemory(const Buffer&);	// try and avoid copies
	void initBuffer(int size);	// common constructor
	int newChunk(int size);		// get a new data chunk

	Chunk *_firstchunk;		// first chunk
	Chunk *_lastchunk;		// last chunk
	int _totalsize;			// total size in all buffers
	int _chunksize;			// size to allocate new buffers

	void *_mutex;

	BufReaderMemory *_firstreader;	// linked list of readers


    public:
	// aux structure to hold data; no active functions
	struct Chunk {
	    struct Chunk *_nextchunk;	// next element in linked list
	    char *_buffer;		// data buffer
	    int _chunksize;		// allocated size of buffer
	    int _currentend;		// current used space
	};

    // subsidiary classes
};

class BufReaderMemory : protected BufReader {
    friend class BufferMemory;

    public:
	// actually read data into a user specified buffer
	virtual int getData(char *user_buffer, int length);
	virtual ~BufReaderMemory(void);

    private:
	BufReaderMemory(const BufReaderMemory &);
	BufReaderMemory(BufferMemory *);

	BufferMemory::Chunk *_currentchunk;
	int _currentoffset;

	BufferMemory *_buffer;
	BufReaderMemory *_nextreader;	// maintain linked list of readers
	BufReaderMemory *_prevreader;	// maintain linked list of readers
};

#endif // _DTMAILBUFFER_HH









