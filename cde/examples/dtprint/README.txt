/* $XConsortium: README.txt /main/3 1996/07/15 14:00:43 drk $ */
-*- text -*-

OVERVIEW

    This directory contains example source demonstrating application
    printing using the X Print Service, the Motif printing functions,
    and the DtPrintSetupBox widget.

FILES

    Main.c
        Contains main(), etc.

    Print.c
        Contains printing callbacks and functions that are
        conceptually application-independent.

    MainWindow.c
        Creates a simple top-level main window.
            
    PrintDemo.h
        Contains declarations exported by Main, Print, and MainWindow.

    AppSpecific.[ch]
        Contains application-specific logic.

    Imakefile
        For use within the CDEnext build tree.

USAGE

    dtprint [-fileName <filename>] [-print]

    where:

        -fileName <filename> is application-specific data to print.

        -print indicates that the application should perform the print
        operation, then exit when printing is complete. This option is
        used for "Print" actions.

STATUS (12/4/95)

    What the demo currently *can* do:

        The demo currently demostrates how an application that prints
        using the X Print Service can incorporate the DtPrintSetupBox
        to be used with a "Print..." menu button, a "Quick-Print"
        toolbar button, or a "-print" option intended for use within a
        "Print" action for an application-specific data type.

        The demo can establish connections to X print servers.

        The demo recognizes the XPRINTER environment variable and the
        XpServerList and XpPrinterNameMode resources. 

    What the demo currently *does not* do:

	The demo does not actally print at this time.

        Printing where no video display is available is not
        demonstrated at this time. This mode will conceivably be used
        for batch printing.

        Print rendering in a separate process is not implemented.

        The demo currently does not recognize LPDEST, PRINTER,
        PDPRINTER, or XpPrinter.

