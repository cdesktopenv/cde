#
# file: msg.awk
#
#
BEGIN{ 
}
   /^\$set/ { SET_NAME = $2 }
   # if a line does not start with $, and the first character of the second
   # argument is a double quote, then delete the first argument from $0,
   # then delete $1 from $0, concatenate TXT_,SET_NAME,_, and  $1, and print
   # $0 (w/t $1)  
   /^[^\$]/ { if (substr($2,1,1) == "\"") 
	       { arg1=$1;
		 sub($1,"",$0);
		 print "#define TXT_" SET_NAME "_" arg1 " " $0
	       }
              else
		 print $0
            }   
   /^$/ { print $0 }  # print line if blank
  /^\$/ { sub(/^\$/ ,"/*",$0);
	  sub( /$/," */",$0);
	  print $0; # C style comments
	}
   
END {

}
