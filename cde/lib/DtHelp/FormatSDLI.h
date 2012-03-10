/* $XConsortium: FormatSDLI.h /main/5 1995/10/26 12:22:04 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **  File:  FormatSDLI.h
 **  Project:  Common Desktop Environment
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpFormatSDLI_h
#define _DtHelpFormatSDLI_h

#include <sys/stat.h>

/********    Semi-Private Function Declarations    ********/
extern	int			 _DtHelpCeFrmtSDLTitleToAscii(
					_DtHelpVolumeHdl volume_handle,
					int		 offset,
					char		**ret_title,
					char		**ret_abbrev);
extern	char			*_DtHelpCeFrmtSdlVolumeAbstractToAscii(
					_DtHelpVolumeHdl volume);
extern	int			 _DtHelpCeFrmtSdlPathAndChildren(
					_DtHelpVolumeHdl volume,
					_FrmtUiInfo	*ui_info,
					int		 fd,
					char		*target_id,
					_DtCvTopicPtr	*ret_handle);
extern	int			 _DtHelpCeGetSdlTitleChunks(
					_DtHelpVolumeHdl volume,
					char		*loc_id,
					_FrmtUiInfo	*ui_info,
					void		***ret_chunks);
extern	int			 _DtHelpCeGetSdlVolIds(
					_DtHelpVolumeHdl volume,
					int		 fd,
					_DtCvSegment	**ret_loids);
extern	int			 _DtHelpCeGetSdlVolIndex(
					_DtHelpVolumeHdl	 volume);
extern	int			 _DtHelpCeGetSdlVolTitleChunks(
					_DtHelpVolumeHdl volume,
					_FrmtUiInfo	*ui_info,
					void		***ret_chunks);
extern	int			 _DtHelpCeFrmtSDLVolTitleToAscii(
					_DtHelpVolumeHdl volume,
					_FrmtUiInfo	*ui_info,
					char		**ret_title);
extern	_DtCvSegment		*_DtHelpCeGetSdlVolToss(
					_DtHelpVolumeHdl volume,
					int		 fd);
extern  _DtCvSegment		*_DtHelpCeMapIdToLoidEntry(
					_DtCvSegment       *id_segs,
					char            *target_id);
extern	int			 _DtHelpCeFrmtSdlVolumeInfo(
					char		*filename,
					_DtHelpVolumeHdl volume,
					time_t		*ret_time);
extern	const SDLAttribute	*_DtHelpCeGetSdlAttributeList(void);
extern	int			 _DtHelpCeParseSdlTopic(
					_DtHelpVolumeHdl volume,
					_FrmtUiInfo	*ui_info,
					int		 fd,
					int		 offset,
					char		*id_string,
					int		 rich_text,
					_DtCvTopicPtr	*ret_handle);

/********    End Public Function Declarations    ********/

#endif /* _DtHelpFormatSDLI_h */
