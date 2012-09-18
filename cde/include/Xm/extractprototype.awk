BEGIN { 
	curfile = FILENAME;
	s = ""; for (i = 0; i < 60; i++) s = s "*";
	printf("/*\n *\n * PRIVATE MOTIF FUNCTIONS\n");
	printf(" * Declarations extracted from Open Motif header files\n *\n */\n\n", s); 
	printf("#include <Xm/TextStrSoP.h>\n");
	printf("#ifdef __cplusplus\nextern \"C\" {\n#endif\n");
}
END {
	printf("#ifdef __cplusplus\n}\n#endif\n");
	printf("/*%s\n * END OF EXTRACTED DATA\n *%s/\n", s, s);
}
function proto() {
        if ($0 ~ /extern/) {
		pattern = 0;
		do { 
			line = $0;
			if (line ~ /#if/) {
				pattern = 1;
			}	
			if (curfile != FILENAME) {
				n = FILENAME;
				sub(/.*\//, "", n);
				printf("/* Extracted from %s */\n", n);
				curfile = FILENAME;
			}
			print line;
			getline;
			if ( pattern == 0) {
				terminate = (line ~ /;/);
			} else {
				terminate = (line ~ /^#.*endif/);
			}
		} while (!terminate);
	}
}
/_XmClearShadowType/ { proto(); }
/_XmShellIsExclusive/ { proto(); }
/_XmGrabKeyboard/ { proto(); }
/_XmFocusIsHere/ { proto(); }
/_XmAddGrab/ { proto(); }
/_XmGrabPointer/ { proto(); }
/_XmRemoveGrab/ { proto(); }
/_XmEnterGadget/ { proto(); }
/_XmLeaveGadget/ { proto(); }
/_XmPushWidgetExtData/ { proto(); }
/_XmPopWidgetExtData/ { proto(); }
/_XmGetWidgetExtData/ { proto(); }
/_XmGadgetImportSecondaryArgs/ { proto(); }
/_XmExtImportArgs/ { proto(); }
/_XmExtGetValuesHook/ { proto(); }
/_XmSocorro/ { proto(); }
/_XmFocusInGadget/ { proto(); }
/_XmFocusOutGadget/ { proto(); }
/_XmIsEventUnique/ { proto(); }
/_XmRecordEvent/ { proto(); }
/_XmSetInDragMode/ { proto(); }
/_XmStringSourceGetString/ { proto(); }
/_XmStringUngenerate/ { proto(); }
/_XmShellIsExclusive/ { proto(); }
/_XmTextFieldSetDestination/ { proto(); }
/_XmGetActiveTopLevelMenu/ { proto(); }
