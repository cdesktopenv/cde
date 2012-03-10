/* $XConsortium: RFCMIME.h /main/1 1995/11/03 10:17:13 rswiston $ */
/*
 *   COMPONENT_NAME: desktop
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 119
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */

typedef enum {
	MIME_7BIT,
	MIME_8BIT,
	MIME_QPRINT,
	MIME_BASE64
	} Encoding;

typedef enum {
	CURRENT_TO_INTERNET,
	INTERNET_TO_CURRENT
	} Direction;
