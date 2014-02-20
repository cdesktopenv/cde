# feed this into perl
eval '(exit $?0)' && eval 'exec PERL $0 ${1+"$@"}' & eval 'exec PERL $0 $argv'
   if 0;

# treeres - resource file preprocessor
# by Dave Brennan (brennan@hal.com)  and RF Starr (starr@wg2.waii.com)
# This is Public Domain software.
#
# This script converts a tree format resource file to an X Resource file.
# The tree format uses spaces to represent the level of a widget in the
# hierarchy.  Currently two spaces represent a level.  For example, if
# the following was the input (ignoring the '# ' prefix):
#
# Toplevel
#   title: Hello World
#   label
#     labelString: Hello
#     foreground: red; background: black
#   *borderWidth: 0
#
# The output would be:
#
# Toplevel.title: Hello World
# Toplevel.label.labelString: Hello
# Toplevel.label.foreground: red
# Toplevel.label.background: black
# Toplevel*borderWidth: 0
#
# The character '.' is the default separator.  If no separator is specified
# it will be used.  Basically this means that only '*' need be sepcified when
# needed.  In general resources specified using the '.' for tight binding
# can be matched faster than those with '*' for lose binding.

*IN = STDIN;
$file = "<stdin>";
if ($#ARGV >= 0)
  {
    open(IN,$ARGV[0]) || die "Can't open file $ARGV[0].\n";
    $file = $ARGV[0];
  }
$continuation = 0;

$line = 0;

while (<IN>)
  {
    # Snag CPP line and file info.
    if (/^# ([0-9]+) "([^"]*)"/)
      {
        $line = $1 - 1; $file = $2;
        #DEBUG print STDERR "Switching to file ", $file, " Line ", $line, "\n";
        next;
      }
    $line++;
    # If previous line ended with \, echo this one as-is.
    if ($continuation)
      {
	 # First unescape single quotes.
	 s/\\'/'/g;
	 $continuation = /[^\\](\\\\)*\\$/;
         print; next;
      }

    # Check for continuation, handling quoted backslashes.
    $continuation = /[^\\](\\\\)*\\$/;

    # Multiple blank lines are compressed to one
    if (/^([ \t]*)(!x.*)?$/) {
      if ($blankcount++ == 0) { print; }
      next;
    }
    $blankcount = 0;
    # Echo comments and blank lines without change.
    if (/^([ \t]*)!/) { print; next; }
    chop;
    #DEBUG $saved = $_;

    # Strip off and count depth character.
    # NOTE: Need cmd line option to alter.
    s/^([ ]*)//;
    $level = length($1) / 2;
    if ($level > $oldlevel + 1) {
      print STDERR $file, ":", $line, ": level increased by more than 1\n";
      exit (1);
    }

    #die "treeres: level increased by more than 1" if ($level > $oldlevel + 1);

    # Nuke trailing space.
    s/([ \t]*)$//;

    # Make base array length match current level
    #DEBUG print "! oldlevel = $oldlevel, level = $level\n";
    if ($oldlevel > 0 && $level <= $oldlevel && ! $did_gen)
      { print STDERR $file, ":", $line,
                     ": Level <= previous with no generation.\n"; }
    $#base = $level - 1;

    # See if a separator has been specified.  If not, use '.'.
    # NOTE: Again, cmd line option.
    if (/^[.*]/ || $level == 0) { $sep = ''; } else { $sep = '.'; }

    # Lines with colons represent resources, so print the line.
    if (/:/)
      {
	# First unescape quotes.
	s/\\'/'/g;
        # Also add readability breaks for `\n' lines.
        # s/\\n/\\n\\\n/g;
        # Then print a line for each semi-colon separated resource.
	foreach $i (split(/[ \t]*;[ \t]*/)) { print @base, $sep, $i, "\n"; }
        $did_gen = 1;
      }
    # Intermediate widget otherwise, so add to current base.
    else
      {
	$oldlevel = $level;
        $base[$level] = $sep . $_;
        $did_gen = 0;
      }
    #DEBUG print "! input = '$saved'\n";
    #DEBUG print "! level = $level, oldlevel = $oldlevel, base = '@base'\n";
  }
