# $XConsortium: nlsREADME.txt /main/2 1996/11/11 11:23:55 drk $
#############################################################################
#
# Component:  dtcreate
#
############################################################################

Introduction
------------

	This directory contains the files for dtcreate.  Dtcreate is
	the HP DT component which provides the CreateAction dialog
	box.  Dtcreate is implemented as a ksh script which uses
	dtdialog for all of its dialogs.

	Because dtcreate uses dtdialog, all of the dtcreate dialog
	boxes are described in Dialog Description Files (DDF).  For
	general information on localizing DDF's, see the "Localizing
	Dtdialog DDF Files" section in the NLS Cookbook.

	Each dialog that is defined in the dtcreate DDF is
	accompanied by comments describing how to cause the dialog to
	be posted in normal usage.

Files to be localized
---------------------

		File Name		Target Name
		---------		-----------
		dtcreate.ddf		/usr/dt/dialogs/%L/dtcreate.ddf
