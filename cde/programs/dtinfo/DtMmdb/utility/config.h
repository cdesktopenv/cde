/* $XConsortium: config.h /main/4 1996/06/11 17:36:26 cde-hal $ */

#ifndef _config_h
#define _config_h 1

#ifdef OLIAS_LITTLE_ENDIAN
# define MMDB_LITTLE_ENDIAN  // i386
#else
# ifdef OLIAS_BIG_ENDIAN
#  define MMDB_BIG_ENDIAN     // sun, rs/6000, hp, uxpds
# else
#  ifdef OLIAS_DEFAULT_ENDIAN
#   ifdef MMDB_BIG_ENDIAN
#   undef MMDB_BIG_ENDIAN
#   endif
#   ifdef MMDB_LITTLE_ENDIAN
#   undef MMDB_LITTLE_ENDIAN
#   endif
#  endif
# endif
#endif

#endif

