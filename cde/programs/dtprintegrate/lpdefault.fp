##################################################################
# 
#   Control Definition for Default Printer
# 
#   Common Desktop Environment 1.0
# 
#  (c) Copyright 1993, 1994 Hewlett-Packard Company
#  (c) Copyright 1993, 1994 International Business Machines Corp.
#  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
#
##################################################################
 
CONTROL		PRINTER_default
{
  TYPE			icon
  ICON			djet
  LABEL			Default
  PUSH_ACTION		Manage_default
  DROP_ACTION		Print_default
  HELP_STRING		This is the default printer.
}
