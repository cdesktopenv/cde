/* $XConsortium: GraphicsP.h /main/4 1996/05/09 03:43:09 drk $ */
#ifndef _DtGraphicsP_h
#define _DtGraphicsP_h

#include <stdio.h>
#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif
/* 
 * Input stream data structures
 */

/* Stream source types */ 
enum _DtGrStreamType { _DtGrNONE, _DtGrBUFFER, _DtGrFILE };

/* Stream file data */
typedef struct
{
    FILE *fileptr;
    char *filename;
    char *uncompressed_filename;
} _DtGrFile;

/* Stream buffer data */
typedef struct
{
    const char *base;
    long       size;
    char       *current;
    char       *end;
} _DtGrBuffer;

/* Stream data */
typedef struct
{
    enum _DtGrStreamType  type;
    union
    {
	_DtGrFile    file;
	_DtGrBuffer  buffer;
    } source;
} _DtGrStream;

/*
 * Input stream function prototypes
 */

/* Open a file stream */
int _DtGrOpenFile(
    _DtGrStream *stream,
    const char  *path
);

/* Open a buffer stream */
int _DtGrOpenBuffer(
    _DtGrStream *stream,
    const char  *buffer,
    int         buffer_size
);

/* Close a stream */
int _DtGrCloseStream(
    _DtGrStream *stream
);

/* Reads data from a stream into a buffer */
size_t _DtGrRead(
    void        *buffer,
    size_t      size,
    size_t      num_items,
    _DtGrStream *stream
);

/* Sets the position of the next input operation on a stream */
int _DtGrSeek(
    _DtGrStream *stream,
    long        offset,
    int         whence
);

/* Reads a character from a stream and advances the stream position */
int _DtGrGetChar(
    _DtGrStream *stream
);

/* Reads a string from a stream and advances the stream position */
char *_DtGrGetString(
    char        *buffer,
    int         num_bytes,
    _DtGrStream *stream
);

/*
 * Inline graphics data structures
 */

enum _DtGrColorModel { _DtGrCOLOR, _DtGrGRAY_SCALE, _DtGrBITONAL };
enum _DtGrLoadStatus { _DtGrSUCCESS, _DtGrCOLOR_REDUCE,_DtGrCONVERT_FAILURE, 
                       _DtGrOPEN_FAILED, _DtGrFILE_INVALID, _DtGrNO_MEMORY, 
                       _DtGrCOLOR_FAILED };

/* Context structure used by image type converters */
typedef struct {
    char *image_type;
    XPointer context;
} _DtGrContext;

typedef enum _DtGrLoadStatus (*_DtGrLoadProc)(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context);

typedef void (*_DtGrDestroyContextProc)(
    _DtGrContext *context);

/*
 * Inline graphics function prototypes
 */

/* Loads an image into a pixmap */
enum _DtGrLoadStatus _DtGrLoad(
    _DtGrStream           *stream,
    char                  **image_type,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context);

/* Destroys a context */
void _DtGrDestroyContext(
    _DtGrContext *context);

/* Registers a converter for a graphic type */
void _DtGrRegisterConverter(
    char                    *image_type,
    _DtGrLoadProc           convert_proc,
    _DtGrDestroyContextProc destroy_context_proc,
    _DtGrLoadProc           *current_convert_proc,
    _DtGrDestroyContextProc *current_destroy_proc);

#ifdef __cplusplus
}
#endif
#endif /* _DtGraphicsP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
