/* $XConsortium: StyleValidate.h /main/2 1996/07/18 16:50:17 drk $ */

// Usage :
//          int status = validate_stylesheet( style_sheet, style_sheet_size );

//          if (status) { fprintf(stderr, "failed"); }

enum RENDERER_ENGINE_T { ONLINE, PRINT };
extern int validate_stylesheet( const char *buf,int buf_size, enum RENDERER_ENGINE_T ); 

