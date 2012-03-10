#!/bin/ksh
# $XConsortium: doit.test.sh /main/2 1995/07/19 18:04:31 drk $

#
# creates .udb entries  but checks first to see if they 
# are already there
#
files="
hp/rivers/bitmaps/Vuefile/Color/action.l.pm
hp/rivers/bitmaps/Vuefile/Color/action.s.pm
hp/rivers/bitmaps/Vuefile/Color/actionC.l.pm
hp/rivers/bitmaps/Vuefile/Color/annotatr.l.pm
hp/rivers/bitmaps/Vuefile/Color/aplcore.l.pm
hp/rivers/bitmaps/Vuefile/Color/arizona.l.pm
hp/rivers/bitmaps/Vuefile/Color/binary.l.pm
hp/rivers/bitmaps/Vuefile/Color/binmulti.l.pm
hp/rivers/bitmaps/Vuefile/Color/bitmap.l.pm
hp/rivers/bitmaps/Vuefile/Color/clock.l.pm
hp/rivers/bitmaps/Vuefile/Color/codering.l.pm
hp/rivers/bitmaps/Vuefile/Color/comprsd.l.pm
hp/rivers/bitmaps/Vuefile/Color/data.l.pm
hp/rivers/bitmaps/Vuefile/Color/data.s.pm
hp/rivers/bitmaps/Vuefile/Color/deliver.l.pm
hp/rivers/bitmaps/Vuefile/Color/deliver.s.pm
hp/rivers/bitmaps/Vuefile/Color/dirRed.l.pm
hp/rivers/bitmaps/Vuefile/Color/dirRed.s.pm
hp/rivers/bitmaps/Vuefile/Color/dirblue.l.pm
hp/rivers/bitmaps/Vuefile/Color/dirblue.pm
hp/rivers/bitmaps/Vuefile/Color/dirblue.s.pm
hp/rivers/bitmaps/Vuefile/Color/direye.l.pm
hp/rivers/bitmaps/Vuefile/Color/dirlock.l.pm
hp/rivers/bitmaps/Vuefile/Color/dirlockd.l.pm
hp/rivers/bitmaps/Vuefile/Color/dirup.l.pm
hp/rivers/bitmaps/Vuefile/Color/diryello.l.pm
hp/rivers/bitmaps/Vuefile/Color/diryello.s.pm
hp/rivers/bitmaps/Vuefile/Color/dotC.l.pm
hp/rivers/bitmaps/Vuefile/Color/dotH.l.pm
hp/rivers/bitmaps/Vuefile/Color/envelope.l.pm
hp/rivers/bitmaps/Vuefile/Color/envelope.s.pm
hp/rivers/bitmaps/Vuefile/Color/envopen.l.pm
hp/rivers/bitmaps/Vuefile/Color/envopen.s.pm
hp/rivers/bitmaps/Vuefile/Color/execute.l.pm
hp/rivers/bitmaps/Vuefile/Color/execute.s.pm
hp/rivers/bitmaps/Vuefile/Color/firecrkr.l.pm
hp/rivers/bitmaps/Vuefile/Color/hpterm.l.pm
hp/rivers/bitmaps/Vuefile/Color/intray.l.pm
hp/rivers/bitmaps/Vuefile/Color/intray.pm
hp/rivers/bitmaps/Vuefile/Color/key.l.pm
hp/rivers/bitmaps/Vuefile/Color/magtape.l.pm
hp/rivers/bitmaps/Vuefile/Color/mailrnr.l.pm
hp/rivers/bitmaps/Vuefile/Color/mailrnr.s.pm
hp/rivers/bitmaps/Vuefile/Color/opentbox.l.pm
hp/rivers/bitmaps/Vuefile/Color/opentbox.s.pm
hp/rivers/bitmaps/Vuefile/Color/penpad.l.pm
hp/rivers/bitmaps/Vuefile/Color/pixmap.l.pm
hp/rivers/bitmaps/Vuefile/Color/pkg.l.pm
hp/rivers/bitmaps/Vuefile/Color/pkgopen.l.pm
hp/rivers/bitmaps/Vuefile/Color/qdoc.l.pm
hp/rivers/bitmaps/Vuefile/Color/question.l.pm
hp/rivers/bitmaps/Vuefile/Color/shell.l.pm
hp/rivers/bitmaps/Vuefile/Color/vfdata.l.pm
hp/rivers/bitmaps/Vuefile/Color/xterm.l.pm
"

set - $files

while [ $# -ne 0 ]; do

 if [ `grep -c $1 VUE-RUN.udb` -eq 0 ]; then
    echo $1
    echo "{ default"
    echo "	install_target = /usr/vue/icons/${1##*/}"
    echo "}"
    echo "#"
 fi
 shift
 
done

