# $XConsortium: nlsREADME.txt /main/2 1996/11/11 11:32:08 drk $
#############################################################################
#
# Component:  dtstyle
#
############################################################################


_DtMessage catalog source:

   File name:  	dtstyle.msg

   Target:	/usr/dt/nls/%L/dtstyle.cat

App-defaults file:

   File name:  	Dtstyle

   Target:	/usr/dt/app-defaults/%L/Dtstyle
   
    NOTE:  ONLY localize the font and accelerator resources in the 
    Dtstyle app-defaults file.  DO NOT localize any of the 
    following resources:

        Dtstyle*componentList:
        Dtstyle*mainRC.orientation:
        Dtstyle*toneScale.maximum: 9000
        Dtstyle*toneScale.minimum: 82
        Dtstyle*durationScale.maximum: 25
    


    The Font resources may need to be localized if a character set
    other than the default hp-roman8 is needed.  A set of iso8859-1 
    Dt fonts are delivered with Dt.  The iso8859-1 fonts can be 
    used by replacing the last field in the font resource xlfd 
    strings from "hp-roman8" to "iso8859-1" in the following files:

        /usr/dt/app-defaults/%L/Dtstyle
        /usr/dt/config/%L/sys.fonts

    Sample versions of these files localized for iso8859-1 are provided
    in:

        /usr/dt/examples/%L/Dtstyle
        /usr/dt/examples/%L/sys.fonts

    
    To specify a localized set of fonts other than the delivered
    dt iso8859-1 fonts,  modify the following resources:

    NumFonts: 	

	Specifies the number of different font sizes available.
	Each font size in the Dtstyle Font list actually represents 
	2 fonts (a system and user font) for that size.
	
    SystemFont[1-NumFonts]:   
    UserFont[1-NumFonts]:

	Specify xlfd strings for NumFont pairs of SystemFont and UserFont.

	To allow the font dialog to present the user with an accurate
	point size for each font pair, the resource values must specify all 
	14 field delimiters of the xlfd string.  In addition, 

        	Scalable font - must specify point size field
	        Bitmap font - must specify pixel size field
	
	If the point size cannot be calculated from the font resource, the
	font sizes will appear numbered 1,2,3,etc.... in the size list in 
	the Font dialog.
	
	Font resource values must match exactly the default font resources
	specified in /usr/dt/config/%L/sys.fonts.  When choosing a default
	font to specify in the sys.fonts file, the font pair must be chosen 
	from the list specified in the Dtstyle app-defaults file and the
	font strings must be identical.


    Example:  localize fonts for Japanese using four font sizes.  
        (There are probably the actual fonts that will be used - its just
        an example.)

	- There is no separate user and system fonts so SystemFont# and UserFont#
	refer to the same font.

	- The xlfd strings are used with all delimiters ("-")
	

	Dtstyle*numFonts: 4

	Dtstyle*SystemFont1: -hp-fixed-medium-r-normal--14-140-75-75-c-70-hp-japanese15
	Dtstyle*SystemFont2: -hp-gothic-medium-r-normal--18-180-75-75-c-80-hp-japanese15
	Dtstyle*SystemFont3: -hp-mincho-medium-r-normal--24-240-75-75-c-120-hp-japanese15
	Dtstyle*SystemFont4: -hp-mincho-medium-r-normal--32-320-75-75-c-160-hp-japanese15

	Dtstyle*UserFont1: -hp-fixed-medium-r-normal--14-140-75-75-c-70-hp-japanese15
	Dtstyle*UserFont2: -hp-gothic-medium-r-normal--18-180-75-75-c-80-hp-japanese15
	Dtstyle*UserFont3: -hp-mincho-medium-r-normal--24-240-75-75-c-120-hp-japanese15
	Dtstyle*UserFont4: -hp-mincho-medium-r-normal--32-320-75-75-c-160-hp-japanese15


