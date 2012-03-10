#!/bin/ksh
# $XConsortium: catalog.sh /main/3 1996/06/19 17:12:50 drk $

# This file may be "sourced" into your current shell (i.e., invoked
# as ". env.sh") or executed in its own shell (i.e., invoked
# as "env.sh".  It's purpose is to set up symbolic links from munged
# PUBLIC identifiers in the SGML declaration to the actual files on
# the system.  PUBLIC identifiers get munged by replacing spaces with
# underscores and slashes with percent signs.

# If all the things you need are based on a common path with entities
# in a directory named "Entities" and the DocBook SGML declaration
# and document type definition in a directory named SgmlStuff, simply
# setting PATH_TOP to that path will make the necessary
# customization.  Otherwise, the path to each directory will have
# to be edited individually.

PATH_TOP=/projects/help/sgml/OSF/tpt/DtDocBook

# the DocBook DTD
ln -s ${PATH_TOP}/SgmlStuff/docbook.2.2.1.dtd \
    -%%HaL_and_O\'Reilly%%DTD_DocBook%%EN

# ISO Numeric and Special Graphic entities
ln -s ${PATH_TOP}/Entities/iso-num.gml        \
    ISO_8879-1986%%ENTITIES_Numeric_and_Special_Graphic%%EN

# ISO Publishing entities
ln -s ${PATH_TOP}/Entities/iso-pub.gml        \
    ISO_8879-1986%%ENTITIES_Publishing%%EN

# ISO Diacritical Mark entities
ln -s ${PATH_TOP}/Entities/iso-dia.gml        \
    ISO_8879-1986%%ENTITIES_Diacritical_Marks%%EN

# ISO General Technical entities
ln -s ${PATH_TOP}/Entities/iso-tech.gml       \
    ISO_8879-1986%%ENTITIES_General_Technical%%EN

# ISO Latin 1 entities
ln -s ${PATH_TOP}/Entities/iso-lat1.gml       \
    ISO_8879-1986%%ENTITIES_Added_Latin_1%%EN

# ISO Latin 2 entities
ln -s ${PATH_TOP}/Entities/iso-lat2.gml       \
    ISO_8879-1986%%ENTITIES_Added_Latin_2%%EN

# ISO Greek 3 entities
ln -s ${PATH_TOP}/Entities/iso-grk3.gml       \
    ISO_8879-1986%%ENTITIES_Greek_Symbols%%EN

# ISO Box and Line entities
ln -s ${PATH_TOP}/Entities/iso-box.gml        \
    ISO_8879-1986%%ENTITIES_Box_and_Line_Drawing%%EN
