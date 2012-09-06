 
# Awk Library file for parsing UDB files
#
function parseUdb() {
	# nawk has already read the initial line.  
	# Tokenize it before doing anything else.
	tokenize() 
	readDefaults(defaults)
	# assign hp-ux ".db" file src and link defaults
	# if none were designated.
		if ( SrcDefault == "" )
			SrcDefault = "a_out_location"
		if ( LnkDefault == "" )
			LnkDefault = "link_source"
		if ( TypDefault == "" )
			TypDefault = "type"
		if ( DestDefault == "" )
			DestDefault = "install_target"
		if ( ModeDefault == "" )
			ModeDefault = "mode"
		if ( OwnerDefault == "" )
			OwnerDefault = "owner"
		if ( GroupDefault == "" )
			GroupDefault = "group"
		readData()
}

# -------------------------------------------------------------
#  readDefaults
#    This routine reads the defaults at the front section 
#  of the universal database and salts the defaults away.
#
# -------------------------------------------------------------
function readDefaults(dflts)
{
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function readDefaults" > DeBugFile
	}
	do {
		os = getOS()
		if ( osQual != "defaults" )
			syntaxError("No defaults for: " BlockToken)
		if ( os == BlockToken || os == "default" )
			break
		skipToToken("}")
	} while ( 1 )
	fillDefaults()
	if ( DeBug > 1 )
		print "Skipping remaining defaults" > DeBugFile
	# skip remaining default specs
	while ( lookAhead() == "{" ) {
		# This should be another default spec
		# skip it. (watch out for syntax errors)
		os = getOS()
		if ( osQual != "defaults" )
			syntaxError("Expected os:defaults found: \"" os ":" osQual "\"")
		if ( os == BlockToken && fileName == FILENAME )
			syntaxError("Only one \"defaults\" record allowed per os" )

		skipToToken("}");
	}
	if ( DeBug > 0 ) Depth--
}
# -------------------------------------------------------------
#  syntaxError
#      bail out
#
# (optionally) mail a message to an administrator if a syntax 
# error occurs in a database.
#
# -------------------------------------------------------------
function syntaxError(reason) {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function syntaxError:" > DeBugFile
	}
	print "Syntax ERROR line: " NR " of file: " FILENAME 
	if (reason)
		print "	" reason
	system( "rm -f /tmp/SyntaxError" )
	system( "touch /tmp/SyntaxError" )
	print "Syntax ERROR line: " NR " of file: " FILENAME > "/tmp/SyntaxError"
	if (reason)
		print "	" reason >> "/tmp/SyntaxError"
	close( "/tmp/SyntaxError" )
	if ( mailTo != "" ) {
		system( "mailx -s \"database syntax error\"  "mailTo"  < /tmp/SyntaxError" )
	}
	system( "rm -f /tmp/SyntaxError" )
	exit 1
}
# -------------------------------------------------------------
#  fillDefaults
#    This routine reads the defaults in the OS
#  defaults section of the database.  It saves the defaults
#  in the "defaults" awk-style string array.
#
# -------------------------------------------------------------
function fillDefaults() {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function fillDefaults:" > DeBugFile
	}
	tempDflt = ""
	NumEntries = 1
	do {
		if ( tempDflt != "" ) {
			keyword = tempDflt
			tempDflt = ""
		}
		else
			keyword = nextToken()
	
		if ( keyword == "}" )
			break;
		if ( "=" != nextToken())
			syntaxError("Keyword: " keyword " not followed by \"=\" ");
		tempDflt = nextToken();
		if ( lookAhead() == "=" )
			defaults[keyword]=""
		else {
			if ( tempDflt == "<SRC>" ) {
				SrcDefault = keyword;
				tempDflt = ""
			}
			if ( tempDflt == "<LNK>" ) {
				LnkDefault = keyword;
				tempDflt = ""
			}
			if ( tempDflt == "<TYPE>" ) {
				TypDefault = keyword;
				tempDflt = "file"
			}
			if ( tempDflt == "<DEST>" ) {
				DestDefault = keyword;
				tempDflt = ""
			}
			if ( tempDflt == "<MODE>" ) {
				ModeDefault = keyword;
				tempDflt = "0444"
			}
			if ( tempDflt == "<OWNER>" ) {
				OwnerDefault = keyword;
				tempDflt = "bin"
			}
			if ( tempDflt == "<GROUP>" ) {
				GroupDefault = keyword;
				tempDflt = "bin"
			}
			defaults[keyword]= tempDflt

			tempDflt = ""
		}
		defOrder[NumEntries++] = keyword;
	} while ( 1 )
	if ( DeBug > 3 ) {
		DBGprintArray(defaults,"defaults") 
		print "SrcDefault =" SrcDefault  > DeBugFile
		print "LnkDefault =" LnkDefault  > DeBugFile
		print "TypDefault =" TypDefault  > DeBugFile
	}
	if ( DeBug > 0 ) Depth--
}
# -------------------------------------------------------------
#  getOS
#    This routine scans the database for an 
#  open brace, then a token, then a ":" indicating 
#  the start of an OS defaults section.
#
# -------------------------------------------------------------
function getOS()
{
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function getOS:" > DeBugFile
	}
	osQual = ""
	gotOS = 0

	if ( "{" != nextToken() )
		syntaxError("Missing initial {")
	os = nextToken();
	if ( lookAhead() == ":" ) {
		nextToken();
		osQual= nextToken();
	} else 
		osQual= ""
	if ( DeBug > 0 ) Depth--
	return os
}
# -------------------------------------------------------------
#  nextToken
#    parse the incoming data stream into tokens.
#
# -------------------------------------------------------------
function nextToken() {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function nextToken:" > DeBugFile
	}
	if ( EOF_Reached == 1 )
		syntaxError("Premature EOF");
	tmpToken=tokens[TK++]
	while ( TK > Ntokens || tokens[TK] == ";" ) {
		TK++
		if ( TK > Ntokens )
			if ( newLine() <= 0 ) {
				EOF_Reached = 1;
				break;
			}
	}
	if ( DeBug > 2 )
		print "Returning token: " tmpToken > DeBugFile
	if ( DeBug > 0 ) Depth--
	return tmpToken
}
# -------------------------------------------------------------
#  lookAhead
#     return the token at the head of the current list of
#  tokens, but do not bump the token count in TK
#
# -------------------------------------------------------------
function lookAhead() {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function lookAhead" > DeBugFile
	}
	if ( DeBug > 0 ) Depth--
	return tokens[TK];
}
# -------------------------------------------------------------
#  newLine, tokenize
#    read a new line of input and tokenize it.
#
# -------------------------------------------------------------
function newLine() {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function newLine:" > DeBugFile
	}
	if ( (retval = getline) <= 0 ) {
		if ( DeBug > 0 ) Depth--
		return retval
	}
	retval =  tokenize()
	if ( DeBug > 0 ) Depth--
	return retval
}
function tokenize() {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function tokenize:" > DeBugFile
	}
	# Workaround for a strange awk bug, seen on FreeBSD
	# and results in .db files generated with
	#   Syntax ERROR line: 10 of file: CDE-INC.udb
	#           Missing initial {
	#
	DUMMY = $0

	# Skip blank/comment lines
	while ( NF == 0 || $0 ~ /^[ 	]*#/  ) {
		if ( (getline) <= 0 ) {
			if ( DeBug > 0 ) Depth--
			return 0
		}
	}
	

	#
	# Make sure syntactically meaningful characters are surrounded by
	# white space.  (I gave up on gsub for this purpose).
	#
	last=1
	Str=""			# temp string for modified input line
	tstStr=$0		# part of input line being tested
	newStr=$0		# current input line image with modifications
##########################################################################
# REPLACE THE FOLLOWING LINE WITH A WORK_AROUND FOR A PROBLEM WITH
# THE MATCH FUNCTION FOR THE SUN VERSION OF "nawk"
#
#	while ( match(tstStr,"[^\\\][:=}{;]") ) {
#
	while ( match(tstStr,"[:=}{;]") ) {
		if ( RSTART-1 > 0 && substr(tstStr,RSTART-1,1) != "\\") {
			RSTART=RSTART-1
			LENGTH=LENGTH+1
		} else {
			#
			# The character was escaped with a backslash.
			# Patch things up -- continue testing the rest
			# of the line.
			# 
			Str=Str substr($0,last,RSTART+1) 
			last = last + RSTART + 1
			tstStr =substr($0,last)
			newStr = Str tstStr
			continue;	
		}
#######################   end of workaround ################################
############################################################################
		if ( DeBug > 1 ) {
			print "Tokenize: Match found in: " tstStr
			print "RSTART= " RSTART " ; RLENGTH = " RLENGTH
		}
                # match found -- 
		# the temp string is now modified to contain:
		# 1) all characters up to the match and the first char of match
		# 2) blank before the  syntactically significant char
		# 3) the significant character
		# 4) blank following the significant character

                Str=Str substr($0,last,RSTART) " " substr($0,last+RSTART,1) " "
                last = last + RSTART + 1;
		#
		# Test remaining part of input line for additional occurances
		# of syntactically significant characters.
		#
                tstStr=substr($0,last)
		#
		# Our best guess for the new string is the part of the
		# input line already tested plus the part yet to be tested.
		#
                newStr=Str tstStr
        }
	#
	# Check for any instances of syntax chars at the start of the line
	#
        sub("^[:=}{;]","& ",newStr);
        $0 = newStr

	#
        # allow escaping of significant syntax characters
        #
        gsub("[\\][{]","{")
        gsub("\\:",":")
        gsub("\\;",";")
        gsub("\\=","=")
        gsub("[\\][}]","}")

	#
	# Having insured that interesting chars are surrounded by blanks
	# now tokenize the input line.
	#

	Ntokens = split($0,tokens)
	TK = 1
	if ( DeBug > 3 )
		DBGprintTokens()
	if ( DeBug > 0 ) Depth--
	return Ntokens
}
function DBGprintTokens()
{
	for ( i = 1; i <= Ntokens ; i++ )
		print "tokens[" i "] = " tokens[i] > DeBugFile
	return 0
}
function DBGprintArray(array,name) {
	for ( i in array) {
		print name "[" i "] = " array[i]  > DeBugFile
	}
}
# -------------------------------------------------------------
#  skipToToken
#     read until the passed in token is encountered
#
# -------------------------------------------------------------
function skipToToken(tok)
{
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function skipToToken:" > DeBugFile
	}
	while ( nextToken() != tok )
		;
	if ( DeBug > 0 ) Depth--
}
# -------------------------------------------------------------
#  readData
#
# -------------------------------------------------------------
function readData() {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function readData" > DeBugFile
	}
	while ( EOF_Reached == 0 ) {
		if ( fileName != FILENAME ) {
			if ( DeBug > 1 ) {
				print "====>Files Changed" > DeBugFile
				print "fileName= " fileName > DeBugFile
				print "FILENAME= " FILENAME > DeBugFile
			}
			fileName = FILENAME
			# skip over defaults section of the new file
			while ( lookAhead() == "{" ) {
				# This should be another default spec
				# skip it. (watch out for syntax errors)
				os = getOS()
				if ( osQual != "defaults" )
					syntaxError("Expected os:defaults found: \"" os ":" osQual "\"")
				#
				# Relax this restriction since we are
				# ignoring this defaults record
				#if ( os == BlockToken )
				#	syntaxError("Only one \"defaults\" record allowed per os" )

				skipToToken("}");
			}
		}
		if ( getNextRecord(record) > 0 )
			PRTREC(record);
		# skip remaining os entries for this source
		# sorry no error checking.
		while ( EOF_Reached == 0 && lookAhead() == "{" )
			skipToToken("}")
	if ( DeBug > 1 )
		print "EOF_Reached = " EOF_Reached > DeBugFile
	}
	if ( DeBug > 0 ) Depth--
}

# -------------------------------------------------------------
#  getNextRecord
#
#    this function fills the rec[] array with defaults
#
#    then it scans for a block that has a token maching
#    BlockToken, or accepts a block with the "default" 
#    token.  The "default" token is not accepted if 
#    defaults are disallowed.
#
#    finally fillRecord is called to read in the lines
#    in the block and override the entries in the rec[] array.
#
# -------------------------------------------------------------
function getNextRecord(rec) {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering function getNextRecord:" > DeBugFile
	}
	# fill with defaults
	for ( i in defaults ) 
		rec[i] = defaults[i];
	do {
		src = nextToken()
		if ( DeBug > 2 )
			print "src=" src > DeBugFile
# Allow special characters to appear in src names if they have been backslashed
#			if ( src ~ /[{:=}]/ )
#				syntaxError("Invalid source: \"" src "\"");
		do {
			os = getOS()
			if ( DeBug > 1 ) {
				print "Got os " os " and qual= " osQual > DeBugFile
				print "NR= " NR " : " $0 > DeBugFile
			}
			if (( os != BlockToken || osQual == "not" ) \
				&& ( os != "default" || UseDefaultBlocks != "Y" ) ) {

				if ( DeBug > 2)
					print "Skipping to end of os rec" > DeBugFile
				skipToToken("}");
			}
			if ( EOF_Reached == 1 || fileName != FILENAME ){
				if ( DeBug > 0 ) Depth--
				return 0
			}
			if ( DeBug > 2 )
				print "Look Ahead is: " tokens[TK] > DeBugFile
		} while ( lookAhead() == "{" )
	} while (( os != BlockToken ) && ( os != "default" || UseDefaultBlocks != "Y"))
		if ( DeBug > 2)
			print "About to call fillRecord" > DeBugFile
	fillRecord(rec)
	if ( DeBug > 0 ) Depth--
	return 1
}
function fillRecord(rec) {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering fillRecord:" > DeBugFile
	}
	tempEntry = ""
	do {
		if ( tempEntry != "" ) {
			keyword = tempEntry;
			tempEntry = ""
		} else
			keyword = nextToken();
		if ( keyword == "}" )
			break;
		if ( "=" != nextToken())
			syntaxError("Keyword: " keyword " not followed by \"=\"");
		tempEntry = nextToken();
		if ( lookAhead() == "=" )
			rec[keyword] = ""
		else {
			rec[keyword] = tempEntry
			tempEntry = ""
		}
	} while (1)
	#
	# check for source entry
	# THIS IMPLIES KNOWLEDGE OF .db FILE ENTRIES!!
	if ( DeBug > 2)
		print "TYPE= " rec[TypDefault]	> DeBugFile
	if ( src == "-" )
		if ( rec[TypDefault]=="directory" || rec[TypDefault]=="empty_dir")
		{
			# no source required for a directory
			if ( rec[SrcDefault] != "" )
				syntaxError(SrcDefault " \"" rec[SrcDefault] "\" specified for a directory.")
			if ( rec[LnkDefault] != "" )
				syntaxError(LnkDefault " \"" rec[LnkDefault] "\" specfied for a directory.")
   
			rec[SrcDefault] = src;
		} else if ( rec["status"] == "---cu-" ) {
			# This is used for some reason (X11-SERV.db)
			if ( rec[SrcDefault] != "" )
				syntaxError( "File: \"" rec["install_target"] "\" with special status: \"---cu-\" should have no source.");
		} else
			syntaxError("Invalid source: \"" src "\" for type: \"" rec[TypDefault] )
	else if ( rec[TypDefault] ~ /link/ )
		if ( src ~ /^\// || src ~ /^\./ ) {
			if ( rec[SrcDefault] != "")
				syntaxError( SrcDefault ": \""	rec[SrcDefault] "\" specified for link: \"" src "\"")
			if ( rec[LnkDefault] == "" )
				rec[LnkDefault]=src;
		} else
			syntaxError("Invalid source: \"" src "\" for type: \"" rec[TypDefault] "\"")
	else if ( rec[TypDefault] == "file" || rec[TypDefault] == "control" )
		rec[SrcDefault] = src;
	else
		syntaxError("Unrecognized type:\"" rec[TypDefault] "\"")

	if ( DeBug > 0 ) Depth--
	
}

# -------------------------------------------------------------
#  printDB
#	Print records in ".db" format
# -------------------------------------------------------------
function printDb(rec) {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering printDb:"	 > DeBugFile
	}
	# NumEntries should be one greater than the number of defaults
	# read in.
	for ( i = 1; i< NumEntries; i++ ) {
		printf("%-40s %s %s\n",defOrder[i], ":",rec[defOrder[i]])
	}		
	print "#"
	if ( DeBug > 0 ) Depth--
}


# -------------------------------------------------------------
#  printLst
#	Print records in ".lst" format
# -------------------------------------------------------------
function printLst(rec) {
	if ( DeBug > 0 ) {
		Depth++
		for ( i=1; i < Depth; i++ )
			printf(" ") > DeBugFile
		print "Entering printLst:"	 > DeBugFile
	}
	if ( rec[TypDefault] ~ /link/ ) 
		Source = LnkDefault
	else
		Source = SrcDefault

	printf("%s %s %s %s %s %s %s %s %s\n",
				   rec[ DestDefault],
				   rec[ ModeDefault ],
				   rec[ Source ],
				   rec[ TypDefault ],
				   rec[ OwnerDefault ],
				   rec[ GroupDefault ],
				   rec[ "status" ],
				   rec[ "processor" ],
				   rec[ "responsible_project" ] ) 
				
	if ( DeBug > 0 ) Depth--
}

# -------------------------------------------------------------
#  printGather
#	print records in one of the formats expected by Gather.ksh
#	(Jim Andreas print routine).
# -------------------------------------------------------------
function printGather(rec) {
# print "Entering printRecord:  "

    if (( BlockToken == "hp-ux" ) && ( rec[ "processor" ] != "378" ))
    {
	if ( index( rec[ "processor" ], Machine ) == 0 )
	{
#printf( "skipping %s, Machine %s machines %s\n", src, Machine, rec[ "processor" ] );
	    return
	}
    }
    if ( action == "toSDD" )
    {
	if ( rec[ "type" ] == "file" )
	{
	    printf("%s:F:%s:%s:%s:*::\n", 
		rec[ "install_target" ], rec[ "owner" ],
		rec[ "group" ], rec[ "mode" ])
	}
    }
    else if ( action == "toReleaseTree" )
    {
      if ( ( rec[ "type" ] == "hard_link" ) ||
	     ( rec[ "type" ] == "sym_link" )  || 
	     ( rec[ "type" ] == "file" )   ) 
      {

#
# if this is a link, then fudge a source file for Gather.ksh
# to check on.  Really we are linking two dest files together
# so the hack is to get around the check in Gather.ksh
#

	if ( ( rec[ "type" ] == "hard_link" ) ||
	     ( rec[ "type" ] == "sym_link" ) )
	{
	    printf( "   {s}%s {d}%s\n", "-", rec[ "install_target" ] );
	}
	else if ( length( src ) > 34 )
		printf( "   {s}%s {d}%s\n",   src, rec[ "install_target" ] );
	    else
		printf( "   {s}%-34s {d}%s\n", src, rec[ "install_target" ] );

	if ( rec[ "install_rule_name" ] == "c-" )
	{
	    printf( "compress -c < {s}%s > {d}%s\n", src, 
		rec[ "install_target" ] );
	}
	else if ( rec[ "type" ] == "sym_link" )
	{
	    printf( "ln -s %s {d}%s\n", src, 
		rec[ "install_target" ] );
	}
	else if ( rec[ "type" ] == "hard_link" )
	{
	    printf( "ln {d}%s {d}%s\n", src, 
		rec[ "install_target" ] );
	}
	else if ( rec[ "uncompress" ] == "true" )
	{
	    printf( "uncompress -c < {s}%s > {d}%s\n", src, 
		rec[ "install_target" ] );
	}
	else if ( length( src ) > 34 )
	{
	    printf( "cp {s}%s {d}%s\n", src, 
		rec[ "install_target" ] );
	}
	else
	{
	    printf( "cp {s}%-34s {d}%s\n", src, 
		rec[ "install_target" ] );
	}
	printf( "%s %s %s\n", rec[ "owner" ], rec[ "group" ], rec[ "mode" ])
	rec[ "install_rule_name" ] = "";
	rec[ "uncompress" ] = "";
      }
    }
    else if ( action == "toDeliverArgs" )
    {
	temp = rec[ "install_target" ];
	m = n = index( temp, "/" );
	while ( n != 0 )
	{
	    temp = substr( temp, n+1 );
	    n = index( temp, "/" );
	    m += n;
	}
	dirnametarget = substr( rec[ "install_target" ], 1, m-1 );

	if ( length( rec[ "install_target" ] ) > 40 )
	{
	    printf("%s -d .%s\n", rec[ "install_target" ], dirnametarget );
	}
	else
	{
	    printf("%-40s -d .%s\n", rec[ "install_target" ], dirnametarget );
	}
    }
    else if ( action == "toCheckBuild" )
    {
# print "Entering printRecord - toCheckBuild:  "
	#
	# skip any link info
	#
	if ( rec[ "type" ] == "file" )
	{
	    #
	    # just print the source path for the checker tool
	    #
	    printf("%s\n", src );
	}
    }
    else if ( action == "toFileList" )
    {
	#
	# skip any link info
	#
	if ( rec[ "type" ] == "file" )
	{
	    #
	    # print the source and install_target for the human person
	    #
	    if ( length( src ) > 40 || length( rec[ "install_target" ] ) > 40 )
	    {
		printf("%s -> %s %s\n", src,
		   rec[ "install_target" ], rec[ "mode" ] );
	    }
	    else
	    {
		printf("%-40s -> %-40s %s\n", src,
		   rec[ "install_target" ], rec[ "mode" ] );
	    }
	}
    }
    else if ( action == "toTargetList" )
    {
	#
	# skip any link info
	#
	if ( rec[ "type" ] == "file" )
	{
	    #
	    # just print the install_target
	    #
	    printf("%s\n", rec[ "install_target" ] );
	}
    }
}

