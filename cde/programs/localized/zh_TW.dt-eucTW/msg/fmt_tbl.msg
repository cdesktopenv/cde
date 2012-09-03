$ $XConsortium: fmt_tbl.msg /main/3 1995/11/08 14:09:19 rswiston $
$set 1
$ 
$ This file is to specify special formatting characteristics of a
$ language.  It defines which characters of the language can not end a
$ line of text, begin a line of text or whether to replace internal
$ newlines with spaces.

$ This file is ONLY necessary for languages with MULTIBYTE character
$ sets.  For single byte character sets (I.E.  English, German, French,
$ etc.), the system has a built in default list of characters that can
$ not begin and end a line.  For single byte languages, the system will
$ also always replace newlines with spaces.
$ 
$ This table is for <???>
$ 
$ message #1 indicates the list of 2byte punctuation, special characters
$ and double consonants that cannot start a line.
$ 
1 ¡¡ÙòÇÄÏÐåôÍõÈºÝ¨>

$ 
$ message #2 indicates the list of 2byte punctuation, special characters
$ and double consonants that cannot end a line.
$ 
2 ¡¡ÙòÇÄÏÐåôÍõÈºÝ¨>

$ 
$ message #3 indicates whether the language wants all end-of-lines in
$ text to be changed into spaces.  I.E.  in english if you had
$ 
$          'the quick brown fox'
$          'jumps over the lazy dog'
$ 
$ would be output as 'the quick brown fox jumps....'.  If this was
$ translated into Japanese but leaving the break where it appeared in the
$ sentence, the newline between 'fox' and 'jumps' would be compressed out
$ and no space would be put between the two words.  But if 'fox' was in
$ Japanese and 'jump' was in english, the newline would be turned into a
$ space. The same (newline -> space) would occur if 'fox' was in english
$ and 'jumps' was in Japanese.
$ 
$ Therefore, the values for message #3 should be
$        1   - means that newlines are always turned into spaces.
$        0   - means that newlines are turned into space only if they
$              occur between a multibyte character and a single byte
$              character.
$ Example:
$     For Japanese, the 'value' of message #3 would be '0'
$ 
3 1

