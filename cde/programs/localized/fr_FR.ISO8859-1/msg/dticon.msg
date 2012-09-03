$ $TOG: dticon.msg /main/7 1999/09/17 12:18:48 mgreess $
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dticon.msg  
$ **  
$ **  Project:     HP DT Icon Editor  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for dticon  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1992, 1993, 1994 Hewlett-Packard Company  
$ **     (c) Copyright 1993, 1994 International Business Machines Corp.  
$ **     (c) Copyright 1993, 1994 Sun Microsystems, Inc.  
$ **     (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of  
$ **         Novell, Inc.  
$ **  
$ **     All Rights reserved  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$quote "

$set 2
$ *****************************************************************************  
$ ** _DtMessage set for fileIODialog.c  
$ **  
$ ** messages 10-16 are for the FileSelectionBox widget (default labels)  
$ *****************************************************************************  

2 "Editeur d'icônes - E-S de fichier"

4 "Editeur d'icônes - E-S de fichier"

6 "Editeur d'icônes - Sauvegarder sous"

8 "Editeur d'icônes - Ouverture de fichier"

10 "Mettre à jour"

12 "Dossiers"

14 "Fichiers"

16 "Entrez un nom de fichier:"

17 "Entrez un nom de fichier:"

18 "Mettre à jour"

20 "SANS TITRE"

22 "Ouvrir"

24 "Sauvegarder"

$set 4        Redefine set#

$ *****************************************************************************  
$ ** _DtMessage set for newIconDialog.c  
$ **  
$ *****************************************************************************  

2 "newIconDialog"

4 "Editeur d'icônes - Redimensionnement"

6 "OK"

8 "Annuler"

10 "Aide"

12 "Nouvelle largeur:"

14 "Nouvelle hauteur:"

$set 6        Redefine set#

$ *****************************************************************************  
$ ** _DtMessage set for queryDialog.c  
$ **  
$ *****************************************************************************  

2 "Editeur d'icônes - Avertissement"

6 "OK"
$ ** DO NOT LOCALIZE - the user should never see this message  

8 "Do ya really wanna?"

10 "Annuler"

$set 8        Redefine set#

$ *****************************************************************************  
$ ** _DtMessage set for stdErrDialog.c  
$ **  
$ *****************************************************************************  

2 "Editeur d'icônes - Erreur"

4 "Continuer"

$ 
$ "-NEW"
6 OK

$ 
$ "-NEW"
8 "Could not connect to ToolTalk:\n%s\nExiting ..."

$set 10        Redefine set#

$ *****************************************************************************  
$ ** _DtMessage set for utils.c  
$ **  
$ *****************************************************************************  
$ ** DO NOT LOCALIZE messages 2-48, they appear in the errorlog only  

2 "Cannot initialize button icon for circle"

4 "Cannot initialize button icon for solid circle"

6 "Cannot initialize button icon for polyline"

8 "Cannot initialize button icon for polygon"

10 "Cannot initialize button icon for solid polygon"

12 "Cannot initialize button icon for ellipse"

14 "Cannot initialize button icon for solid ellipse"

16 "Cannot initialize button icon for eraser"

18 "Cannot initialize button icon for line"

20 "Cannot initialize button icon for flood"

22 "Cannot initialize button icon for point"

24 "Cannot initialize button icon for rectangle"

26 "Cannot initialize button icon for solid rectangle"

28 "Cannot initialize button icon for select"

30 "Unable to parse static color no."

32 "Unable to allocate static color no."

38 "No memory(0)"

40 "No memory(1)"

42 "No memory(2)"

44 "No memory(3)"

46 "No memory(4)"

48 "No memory(5)"

50 "Impossible d'initialiser l'application de stockage des icônes."

52 "Utilisation du paramètre -f non valide."

54 "Utilisation incorrecte du paramètre -i."

56 "Utilisation du paramètre -session non valide."

58 "Taille non valide."

60 "La saisie de l'image a échoué."

62 "Echec de l'opération.\n\
La table des couleurs est saturée."

64 "Paramètre de dimension non valide."

66 "(SANS NOM)"

$set 12        Redefine set#

$ *****************************************************************************  
$ ** _DtMessage set for dtIconShell.c  
$ **  
$ *****************************************************************************  

1 "Editeur d'icônes"

2 "2 couleurs"

3 "Editeur d'icônes -  (SANS NOM)"

4 "Bienvenue dans l'éditeur d'icônes."

6 "Remplir"

8 "Couleurs statiques"

10 "Couleurs dynamiques"

12 "Nuances de gris"

14 "Premier plan"

16 "Arrière-plan"

18 "Ombrage du haut"

20 "Ombrage du bas"

22 "Sélection"

24 "Transparence"
$ ** message 26-198 are menu items with corresponding mnemonics  
$ ** (for set grouping information, see the comment at the end-of-file)  

26 "N"

28 "Nouveau..."

30 "O"

32 "Ouvrir..."

34 "S"

36 "Sauvegarder"

38 "a"

40 "Sauvegarder sous..."

42 "Q"

44 "Quitter"

46 "F"

48 "Fichier"

50 "A"

52 "Annuler"

54 "C"

56 "Couper zone"

58 "o"

60 "Copier zone"

62 "l"

64 "Coller zone"

68 "G"

70 "Gauche"

72 "D"

74 "Droite"

76 "P"

78 "Pivoter zone"

82 "V"

84 "Verticale"

86 "H"

88 "Horizontale"

90 "I"

92 "Inverser zone"

94 "j"

96 "Ajuster zone"

98 "e"

100 "Redimensionner l'icône..."

102 "u"

104 "Ajouter point de repère"

106 "S"

108 "Supprimer point de repère"

110 "S"

112 "Saisir image écran"

114 "E"

116 "Effacer l'icône"

118 "E"

120 "Editer"

122 "G"

124 "Grille affichée"

128 "B"

130 "XBM"

134 "P"

136 "XPM"

138 "F"

140 "Format de sortie"

144 "2x"

146 "3x"

148 "4x"

150 "5x"

152 "6x"

154 "8x"

156 "10x"

158 "12x"

160 "A"

162 "Agrandissement"

164 "O"

166 "Options"

168 "G"

170 "Généralités..."

172 "T"

174 "Tâches..."

176 "R"

178 "Référence..."

180 "u"

182 "Sur l'élément"

188 "i"

190 "Aide sur l'aide..."

192 "A"

194 "A propos de l'éditeur d'icônes..."

196 "A"

198 "Aide"
$ ** message 200-are used in the "instruction" line for current drawing mode  

200 "Mode d'édition en cours:"

202 "(Coloré)"

204 "Crayon"

206 "Coloriage"

208 "Ligne"

210 "Polyligne"

212 "Rectangle"

214 "Polygone"

216 "Cercle"

218 "Ovale"

220 "Effacer"

222 "Sélection"

224 "Non valide"

$set 14        Redefine set#

$ *****************************************************************************  
$ ** _DtMessage set for help.c  
$ **  
$ *****************************************************************************  

2 "Editeur d'icônes - Aide"

4 "Vous devez sélectionner un élément\n\
dans l'éditeur d'icônes."

$set 16        Redefine set#

$ *****************************************************************************  
$ ** _DtMessage set for process.c  
$ **  
$ *****************************************************************************  

2 "Le fichier est inaccessible ou\n\
contient des données non valides."

4 "Impossible d'écrire les données dans le fichier."

6 "Cette fonction n'est pas encore disponible."

8 "Aucune image précédente\n\
disponible."

10 "Largeur et/ou hauteur\n\
spécifiée(s) non valide(s)."

12 "Aucune zone sélectionnée."

14 "Aucune zone n'a encore été\n\
coupée ou copiée."

16 "L'icône en cours n'a pas été sauvegardée.\n\n\
Vos modifications seront perdues."

20 "Vérifiez les autorisations associées au fichier."

22 "Impossible de restaurer la session à partir du fichier."

24 "La session n'a pas pu être sauvegardée dans le fichier."

25 "Ce fichier existe déjà.\n\n\
Voulez-vous le remplacer ?"

$set 18
$ *****************************************************************************
$ ** _DtMessage set for event.c
$ **
$ *****************************************************************************
$quote

2 Taille de l'image capturée = %dx%d

$ ****************************************************************************  
$ ** End of _DtMessage Catalog File  
$ ** (Comments only below this line)  
$ ****************************************************************************  
$ **  
$ ****************************************************************************  
$ ** Unique mnemonic groupings from set 12  
$ *****************************************************************************  
$ **  
$ ** Unique Set 1:  (Main Menu Bar)  
$ **  
$ **   46 F  
$ **   48 File  
$ **  
$ **   118 E  
$ **   120 Edit  
$ **  
$ **   164 O  
$ **   166 Options  
$ **  
$ **   196 H  
$ **   198 Help  
$ **  
$ **  
$ ** Unique Set 2:  (File Menu)  
$ **  
$ **   26 N  
$ **   28 New...  
$ **   30 O  
$ **   32 Open...  
$ **   34 S  
$ **   36 Save  
$ **   38 A  
$ **   40 Save As...  
$ **   42 x  
$ **   44 Exit  
$ **  
$ **  
$ ** Unique Set 3:  (Edit Menu)  
$ **  
$ **   50 U  
$ **   52 Undo  
$ **   54 t  
$ **   56 Cut Area  
$ **   58 C  
$ **   60 Copy Area  
$ **   62 P  
$ **   64 Paste Area  
$ **  
$ **        Unique Set 3a:  (Rotate Sub-Menu)  
$ **        68 L  
$ **        70 Left  
$ **        72 R  
$ **        74 Right  
$ **   76 R  
$ **   78 Rotate Area  
$ **  
$ **        Unique Set 3b:  (Flip Sub-Menu)  
$ **        82 V  
$ **        84 Vertical  
$ **        86 H  
$ **        88 Horizontal  
$ **   90 F  
$ **   92 Flip Area  
$ **   94 S  
$ **   96 Scale Area  
$ **   98 z  
$ **   100 Resize Icon...  
$ **   102 A  
$ **   104 Add Hot Spot  
$ **   106 D  
$ **   108 Delete Hot Spot  
$ **   110 G  
$ **   112 Grab Screen Image  
$ **   114 e  
$ **   116 Clear Icon  
$ **  
$ **  
$ ** Unique Set 4:  (Options Menu)  
$ **  
$ **   122 V  
$ **   124 Visible Grid  
$ **  
$ **        Unique Set 4a:  (Output Sub-Menu)  
$ **        128 B  
$ **        130 XBM  
$ **        134 P  
$ **        136 XPM  
$ **   138 O  
$ **   140 Output Format  
$ **   160 M  
$ **   162 Magnification  
$ **  
$ **  
$ ** Unique Set 5:  (Help Menu)  
$ **  
$ **   168 I  
$ **   170 Introduction...  
$ **   172 T  
$ **   174 Tasks...  
$ **   176 R  
$ **   178 Reference...  
$ **   180 O  
$ **   182 On Item  
$ **   188 U  
$ **   190 Using Help...  
$ **   192 V  
$ **   194 Version...  
$ **  
