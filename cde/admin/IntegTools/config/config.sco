####################################
### Instructions for SCO Systems ###
####################################

System Requirements
===================

    * SCO Open Desktop or SCO Open Server Version 3.0
    * 386-33, 486, or Pentium based processor
    * 16MB memory
    * 1024x768, 256 color graphics system
    * 70MB free disk space
    * 30MB Swap Space


Before You Start the Desktop
============================

If your system has one of these special configurations, you may need to
edit certain Login Manager files before starting the desktop. See
chapter 2 of the Getting Started Guide:


	* If  the system console is a non-bitmap device.

	* If the system is an X terminal or a host for X terminals.

	* If the system has more than one display.




To Start and Stop the Desktop
============================

1. Log in as the user who will use the desktop and be running the Korn or
   Bourne shells (ksh or sh).  

2. First, add the following two lines to your .profile:

	PATH=$PATH:/opt/dt/bin
        . /opt/dt/bin/dtsearchpath

   These lines add the desktop search path to your PATH variable.

3. If you have a .startxrc file in your home directory, move it to a
   backup location or name by entering:

	 mv .startxrc old.startxrc

4. Copy /etc/opt/dt/dt.startxrc to your home directory and rename it 
   to .startxrc by entering: 

	cp /etc/opt/dt/dt.startxrc .startxrc

5. Do one of the following:

       * Log out of your ODT session and log in via Scologin.

       * Or, at a shell prompt on a multiscreen not running the X
         server, run startx by entering:

	 startx

         Be sure to log out to reinvoke your .profile correctly.














