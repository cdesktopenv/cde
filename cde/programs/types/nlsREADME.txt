# $XConsortium: nlsREADME.txt /main/2 1996/11/11 11:32:25 drk $
#############################################################################
#
# Component:  types - The DT 3.0 action and filetype definitions.
#
############################################################################

Introduction:
------------

   The files in this directory contain the DT 3.0 action and 
   filetype definitions.

   The syntax for the DT 3.0 definitions is much different than the
   DT 2.0 syntax.  The most significant change is that the each 
   field in a definition is on a separate line and each field is
   identified by a keyword.  Other important changes are:  filetype
   and action definitions may be in the same file; some fields are
   optional and each definition may have a "DESCRIPTION" field.

   See the file "uxstd.vf" for an example of a DT database file
   that has both filetype and action definitions in the same file
   and uses the "DESCRIPTION" field.

   There are NO screendumps for this component.

   There is NO message catalog for this component.

   There is NO app-defaults file for this component.

   This component contains a Dialog Description File (DDF).  See 
   the section on "Localizing Dtdialog DDF Files" in the NLS
   Cookbook for general information.


Files to be localized:  
---------------------


           File Name                     Target Name
	   ---------                     -----------
   1.     laserrom.vf                 /usr/dt/types/%L/laserrom.vf
   2.     print.vf	              /usr/dt/types/%L/print.vf
   3.     printerNN.vf	              /usr/dt/types/%L/printerNN.vf
   4.     unsupported.vf	      /usr/dt/types/%L/unsupported.vf
   5.     user-prefs.vf		      /usr/dt/types/%L/user-prefs.vf
   6.     uxstd.vf                    /usr/dt/types/%L/uxstd.vf
   7.     dt.vf                      /usr/dt/types/%L/dt.vf
   8.     vhelp.vf	              /usr/dt/types/%L/vhelp.vf
   9.     dt20.vf                    /usr/dt/types/%L/dt20.vf
  10.     dtfile.vf                  /usr/dt/types/%L/dtfile.vf
  11.     dtpad.vf                   /usr/dt/types/%L/dtpad.vf
  12.     xclients.vf                 /usr/dt/types/%L/xclients.vf
  13.	  actions.ddf		      /usr/dt/dialogs/%L/actions.ddf


What to localize in the .vf files:
---------------------------------

   NOTE: do NOT localize action and filetype names.

   1. The "DESCRIPTION" field in every action and filetype definition
      should be localized.  The information in this field can be
      displayed to the user by selecting a file in the File Manager and
      then pressing the "F1" key.

   2. Some action definitions contain "prompt strings" that ask 
      the user for input.  The prompt strings should be localized.

      Prompt strings are always embedded in a pair of '"' characters
      which are embedded in a pair of '%' characters.  For example, in
      the following EXEC-STRING field, the prompt string is "Some
      String:":

	 EXEC-STRING    %(File)Arg_1"Some String:"% 

      To change the prompt to "String Some:", use:

	 EXEC-STRING    %(File)Arg_1"String Some:"% 

      Prompt strings are only used in the "EXEC-STRING" and "EXEC-HOST"
      fields of action definitions.

      To find the actions which contain prompts, 

      The EXEC-STRING and EXEC-HOST field may contain "keywords" that
      should NOT be localized.  These keywords are the same keywords
      that were used with DT 2.*, so for more infomation, see the
      reference "HP Visual User Environment User's Guide".

      Do NOT localize the following keywords:

	 (File)Arg_1

	 (File)Args

	 %LocalHost%

	 Arg_1

	 Args

   3. The "L-ICON" and "S-ICON" fields may be localized.  These fields
      are for the large and small icons, respectively.


How to test the localization changes:
------------------------------------

  1. To test the DESCRIPTION field changes:
  
    To test the action definition changes, select the action and
    then press the "F1" key.  The localized description should get
    displayed.

    To test the filetype definition changes, create a file of that
    filetype, select that file and then press the "F1" key.  The
    localized description should get displayed.

  2. To test the prompt string changes:

     For every action definition that was changed in step #2 above,
     double click the action in the File Manager.  The prompt string
     that is displayed should be localized.

  3. To test the icon changes:

     For every action whose icon was changed, display the action in the
     File Manager.  The icon displayed should be the localized icon.

     For every filetype whose icon was changed, create a file of that
     filetype and display the file in the File Manager.  The icon
     displayed should be the localized icon.


#
#


