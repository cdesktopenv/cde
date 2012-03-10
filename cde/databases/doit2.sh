#!/bin/ksh
# $XConsortium: doit2.sh /main/2 1995/07/19 18:04:38 drk $

#
# creates .udb entries given source and dest
#
files="
hp/rivers/config.ive/vuemail.ddf /usr/vue/config/dialogs/ivemail.ddf 0444
hp/rivers/config.ive/pvmail.vf /usr/vue/config/ivemail.vf 0444
hp/rivers/ivemail/elmconfig/elmrc  /usr/vue/ivemail/elmconfig/elmrc 0444
hp/rivers/ivemail/elmconfig/elmheaders /usr/vue/ivemail/elmconfig/elmheaders 0444
hp/rivers/ivemail/collapse.mc /usr/vue/ivemail/collapse.mc 0444
hp/rivers/ivemail/cvt2mime.ph /usr/vue/ivemail/cvt2mime 0555
hp/rivers/ivemail/expandSkel.ph /usr/vue/ivemail/expandSkel 0555
hp/rivers/ivemail/getHeadVal.ph /usr/vue/ivemail/getHeadVal 0555
hp/rivers/ivemail/getMailHd.ph /usr/vue/ivemail/getMailHd 0555
hp/rivers/ivemail/ivepad.ph /usr/vue/ivemail/ivepad 0555
hp/rivers/ivemail/ivepage.ph /usr/vue/ivemail/ivepage 0555
hp/rivers/ivemail/mailFiles /usr/vue/ivemail/mailFiles 0555
hp/rivers/ivemail/mimeImplode /usr/vue/ivemail/mimeImplode 0555
hp/rivers/ivemail/mimeXplode /usr/vue/ivemail/mimeXplode 0555
hp/rivers/ivemail/mimetypes.ph /usr/vue/ivemail/mimetypes 0555
hp/rivers/ivemail/vpMimeClo.ph /usr/vue/ivemail/vpMimeClo 0555
hp/rivers/ivemail/mimetypes.ph /usr/vue/ivemail/mimetypes 0555
hp/rivers/ivemail/vpMimeIns.ph /usr/vue/ivemail/vpMimeIns 0555
hp/rivers/ivemail/vuemailsend /usr/vue/ivemail/vuemailsend 0555
hp/rivers/ivemail/mimetypes.h /usr/vue/ivemail/mimetypes.h 0444
"

set - $files

while [ $# -ne 0 ]; do

     echo $1
     echo "{ default"
     echo "	install_target = $2"
     if [ $3 != "0444" ]; then
	 echo "	mode = $3"
     fi
     echo "}"
     echo "#"
     shift
     shift
     shift
done

