/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: xdm_msg.h /main/3 1995/10/27 16:18:41 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef _H_XDM_MSG 
#define _H_XDM_MSG 
#include <limits.h>
#include <nl_types.h>
#define MF_XDM "xdm.cat"



/* The following was generated from ../../../../../../../src/gos/2d/XTOP/aixclients/xdm/xdm.msg. */


/* definitions for set MS_XDM */
#define MS_XDM 1

#define MSG_XDM_1 1
#define MSG_XDM_2 2
#define MSG_XDM_3 3
#define MSG_XDM_4 4
#define MSG_XDM_5 5
#define MSG_XDM_6 6
#define MSG_XDM_7 7
#define MSG_XDM_8 8

/* definitions for set MS_xdmshell */
#define MS_xdmshell 2

#define M_MSG_1 1
#define M_MSG_2 2
#define M_MSG_3 3
#define M_MSG_4 4
#define M_MSG_5 5
#define M_MSG_6 6
#define M_MSG_7 7
#define M_MSG_8 8
#define M_MSG_9 9

/* definitions for set MS_access */
#define MS_access 3

#define M_MSG_10 1
#define E_MSG_11 2
#define E_MSG_12 3
#define E_MSG_13 4

/* definitions for set MS_auth */
#define MS_auth 4

#define M_MSG_14 1
#define M_MSG_15 2
#define M_MSG_16 3
#define M_MSG_17 4
#define M_MSG_19 5
#define E_MSG_20 6
#define M_MSG_21 7
#define E_MSG_22 8
#define M_MSG_23 9
#define M_MSG_24 10
#define M_MSG_25 11
#define M_MSG_26 12
#define M_MSG_27 13
#define M_MSG_28 14
#define M_MSG_29 15
#define M_MSG_30 16
#define E_MSG_31 17
#define M_MSG_32 18
#define M_MSG_33 19
#define M_MSG_34 20
#define M_MSG_35 21
#define M_MSG_36 22
#define M_MSG_37 23
#define M_MSG_38 24
#define M_MSG_39 25
#define E_MSG_40 26
#define M_MSG_41 27
#define M_MSG_42 28
#define M_MSG_43 29
#define M_MSG_44 30
#define E_MSG_45 31
#define M_MSG_46 32
#define M_MSG_47 33

/* definitions for set MS_choose */
#define MS_choose 5

#define E_MSG_48 1
#define M_MSG_49 2
#define M_MSG_50 3
#define M_MSG_51 4
#define M_MSG_52 5
#define M_MSG_53 6
#define E_MSG_54 7
#define M_MSG_55 8
#define M_MSG_56 9
#define M_MSG_57 10
#define E_MSG_58 11
#define M_MSG_59 12
#define M_MSG_60 13
#define E_MSG_61 14

/* definitions for set MS_dm */
#define MS_dm 6

#define M_MSG_62 1
#define E_MSG_63 2
#define E_MSG_64 3
#define M_MSG_65 4
#define M_MSG_66 5
#define M_MSG_67 6
#define E_MSG_68 7
#define M_MSG_69 8
#define M_MSG_70 9
#define M_MSG_71 10
#define M_MSG_72 11
#define M_MSG_73 12
#define M_MSG_74 13
#define M_MSG_75 14
#define M_MSG_76 15
#define M_MSG_77 16
#define M_MSG_78 17
#define M_MSG_79 18
#define M_MSG_80 19
#define M_MSG_81 20
#define M_MSG_82 21
#define E_MSG_83 22
#define M_MSG_84 23
#define E_MSG_85 24
#define E_MSG_86 25
#define M_MSG_87 26
#define M_MSG_88 27
#define M_MSG_89 28
#define M_MSG_90 29
#define M_MSG_91 30
#define M_MSG_92 31
#define E_MSG_93 32
#define M_MSG_94 33
#define M_MSG_95 34
#define M_MSG_96 35
#define M_MSG_98 36
#define E_MSG_99 37
#define E_MSG_100 38
#define E_MSG_101 39

/* definitions for set MS_error */
#define MS_error 7

#define E_MSG_102 1
#define M_MSG_103 2
#define M_MSG_104 3
#define E_MSG_105 4
#define M_MSG_106 5

/* definitions for set MS_file */
#define MS_file 8

#define E_MSG_107 1
#define E_MSG_108 2
#define E_MSG_109 3
#define M_MSG_110 4
#define M_MSG_111 5

/* definitions for set MS_greet */
#define MS_greet 9

#define M_MSG_112 1
#define M_MSG_113 2
#define M_MSG_114 3
#define M_MSG_115 4

/* definitions for set MS_netaddr */
#define MS_netaddr 10

#define M_MSG_116 1
#define M_MSG_117 2
#define M_MSG_118 3

/* definitions for set MS_policy */
#define MS_policy 11

#define M_MSG_119 1
#define M_MSG_120 2

/* definitions for set MS_protodpy */
#define MS_protodpy 12

#define M_MSG_121 1
#define M_MSG_122 2
#define M_MSG_123 3
#define M_MSG_124 4

/* definitions for set MS_reset */
#define MS_reset 13

#define M_MSG_125 1
#define E_MSG_126 2
#define M_MSG_127 3
#define M_MSG_128 4
#define M_MSG_129 5

/* definitions for set MS_resource */
#define MS_resource 14

#define M_MSG_130 1
#define E_MSG_131 2
#define E_MSG_132 3
#define E_MSG_133 4

/* definitions for set MS_rpcauth */
#define MS_rpcauth 15

#define M_MSG_134 1

/* definitions for set MS_server */
#define MS_server 16

#define M_MSG_135 1
#define M_MSG_136 2
#define E_MSG_137 3
#define E_MSG_138 4
#define E_MSG_139 5
#define M_MSG_140 6
#define M_MSG_141 7
#define M_MSG_142 8
#define M_MSG_143 9
#define M_MSG_144 10
#define M_MSG_145 11
#define E_MSG_146 12
#define M_MSG_147 13
#define E_MSG_148 14
#define M_MSG_149 15
#define M_MSG_150 16
#define M_MSG_151 17
#define M_MSG_152 18
#define M_MSG_153 19
#define M_MSG_154 20
#define E_MSG_155 21
#define M_MSG_156 22
#define E_MSG_157 23
#define M_MSG_158 24
#define M_MSG_160 25

/* definitions for set MS_socket */
#define MS_socket 17

#define M_MSG_161 1
#define E_MSG_162 2
#define E_MSG_163 3
#define M_MSG_164 4
#define E_MSG_165 5

/* definitions for set MS_streams */
#define MS_streams 18

#define M_MSG_166 1
#define E_MSG_167 2
#define E_MSG_168 3
#define M_MSG_169 4
#define E_MSG_170 5

/* definitions for set MS_verify */
#define MS_verify 19

#define M_MSG_171 1
#define M_MSG_172 2
#define M_MSG_173 3
#define M_MSG_174 4
#define M_MSG_175 5
#define M_MSG_176 6
#define M_MSG_177 7
#define E_MSG_178 8

/* definitions for set MS_xdmauth */
#define MS_xdmauth 20

#define M_MSG_179 1
#define M_MSG_180 2

/* definitions for set MS_xdmcp */
#define MS_xdmcp 21

#define M_MSG_181 1
#define M_MSG_182 2
#define M_MSG_183 3
#define M_MSG_184 4
#define M_MSG_185 5
#define M_MSG_186 6
#define M_MSG_187 7
#define M_MSG_188 8
#define M_MSG_189 9
#define M_MSG_190 10
#define M_MSG_191 11
#define M_MSG_192 12
#define M_MSG_193 13
#define M_MSG_194 14
#define M_MSG_195 15
#define M_MSG_196 16
#define M_MSG_197 17
#define M_MSG_198 18
#define M_MSG_199 19
#define M_MSG_200 20
#define M_MSG_201 21
#define M_MSG_202 22
#define M_MSG_203 23
#define M_MSG_204 24
#define M_MSG_205 25
#define M_MSG_206 26
#define M_MSG_207 27
#define M_MSG_208 28
#define M_MSG_209 29
#define M_MSG_210 30
#define M_MSG_211 31
#define M_MSG_212 32
#define M_MSG_213 33
#define M_MSG_214 34

/* definitions for set MS_sessreg */
#define MS_sessreg 22

#define M_MSG_215 1
#define M_MSG_216 2
#define M_MSG_217 3

/* definitions for set MS_chooser */
#define MS_chooser 23

#define M_MSG_218 1
#define M_MSG_219 2
#define M_MSG_220 3
#define M_MSG_221 4
#define M_MSG_222 5
#define M_MSG_223 6
#define M_MSG_224 7
#define M_MSG_225 8
#endif 
