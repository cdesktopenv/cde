%{
/* $XConsortium: gr_type.l /main/3 1996/11/19 16:55:02 drk $ */

#include <assert.h>
#include <stream.h>

#ifdef DEBUG
#include "oliasdb/olias_consts.h"
GR_TYPE gtype;
#else
#include "GraphicsTask.h"
extern GR_TYPE gtype;
#endif

%}

%%

^"GIF8"[79]"a"	{
             gtype = GR_TYPE_GIF;
	     return gtype;
	 }

^\xff\xd8\xff\xe0	{
             gtype = GR_TYPE_JPEG;
	     return gtype;
	 }

^"P"[14]    {
             gtype = GR_TYPE_PBM;
	     return gtype;
	 }

^"P"[25]    {
             gtype = GR_TYPE_PGM;
	     return gtype;
	 }

^"%!"     {
             gtype = GR_TYPE_POSTSCRIPT;
	     return gtype;
          }

^"P"[36]    {
             gtype = GR_TYPE_PPM;
	     return gtype;
	 }

^\x49\x49\x2a\x00	{
             gtype = GR_TYPE_TIFF;
	     return gtype;
	 }

^\x4d\x4d\x00\x2a	{
             gtype = GR_TYPE_TIFF;
	     return gtype;
	 }

^"#define "[^_ \n]+("_width"|"_x_hot")    {
             gtype = GR_TYPE_XBM;
	     return gtype;
	 }

"XPM"    {
             gtype = GR_TYPE_XPM;
	     return gtype;
	 }

\n       {
             return gtype;
         }
.        ;

         
%%

#ifndef DEBUG
void
get_type()
{
  graphics_tasklex();

  BEGIN INITIAL;
  graphics_taskrestart(graphics_taskin);
}

#else
#include <X11/XWDFile.h>

GR_TYPE
try_xwd(FILE *fp)
{
  // Create header container
  CARD32 header[sz_XWDheader];
  XWDFileHeader *hdrptr;

  hdrptr = (XWDFileHeader *)header;
  for (CARD32 ndx = 0; ndx < sz_XWDheader; ndx++) {
    header[ndx] = (CARD32) 0;
  }

  if ( fread( &header[0], sizeof(*hdrptr), 1, fp ) != 1 ) {
    return( GR_TYPE_UNKNOWN );
  }
  if ((hdrptr->file_version == XWD_FILE_VERSION) &&
      (hdrptr->header_size > 0) &&
      (hdrptr->pixmap_width > 0) &&
      (hdrptr->pixmap_height > 0)) {
    return( GR_TYPE_XWD );
  } else {
    return( GR_TYPE_UNKNOWN );
  }
}

int
main(int argc, char *argv[])
{
   int
      retcode;
   
   /*
    * This procedure attempts to open a file passed as an argument,
    * and exits on failure.  If no argument is available, stdin is
    * assumed.
    */

   if (argc > 1)
   {
      if ((yyin = fopen(argv[1], "r")) == NULL)
      {
         (void) fprintf(stderr, "Bad input file name: %s \n", argv[1]);
         exit (1);
      }
   }

   switch ( yylex() ){
     case GR_TYPE_UNKNOWN:
       {
	 cerr << "UNKNOWN: " << gtype << endl;
	 // try xwd before giving up
	 fclose(yyin);
	 if ((yyin = fopen(argv[1], "r")) == NULL) {
	   (void) fprintf(stderr, "Bad input file name: %s \n", argv[1]);
	   exit (1);
	 }
	 gtype = try_xwd(yyin);
	 if (gtype == GR_TYPE_UNKNOWN) {
	   cerr << "UNKNOWN: " << gtype << endl;
	 } else {
	   cerr << "XWD: " << gtype << endl;
	 }
       }
       break;
     case GR_TYPE_CGM:
       {
	 cerr << "CGM: " << gtype << endl;
       }
       break;
     case GR_TYPE_GIF:
       {
	 cerr << "GIF: " << gtype << endl;
       }
       break;
     case GR_TYPE_JPEG:
       {
	 cerr << "JPEG: " << gtype << endl;
       }
       break;
     case GR_TYPE_PBM:
       {
	 cerr << "PBM: " << gtype << endl;
       }
       break;
     case GR_TYPE_PGM:
       {
	 cerr << "PGM: " << gtype << endl;
       }
       break;
     case GR_TYPE_POSTSCRIPT:
       {
	 cerr << "POSTSCRIPT: " << gtype << endl;
       }
       break;
     case GR_TYPE_PPM:
       {
	 cerr << "PPM: " << gtype << endl;
       }
       break;
     case GR_TYPE_TIFF:
       {
	 cerr << "TIFF: " << gtype << endl;
       }
       break;
     case GR_TYPE_XBM:
       {
	 cerr << "XBM: " << gtype << endl;
       }
       break;
     case GR_TYPE_XPM:
       {
	 cerr << "XPM: " << gtype << endl;
       }
       break;
     case GR_TYPE_XWD:
       {
	 cerr << "XWD: " << gtype << endl;
       }
       break;
   }
   exit(0);
}
#endif
