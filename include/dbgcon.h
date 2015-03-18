/*
 * Simple Console Logging
 */


#ifndef __DBGCON_H__
#define __DBGCON_H__


#ifdef __cplusplus
      extern "C" {
#endif

// Comment out the #undef below to enable simple console logging.
#undef DBG_CON

#ifdef DBG_CON
#define PFXFMT  "**__con_debug(%d)** "
#define __con_debug(lvl, fmt, ...)\
switch (lvl) {\
  case 0:\
    break;\
  case 2:\
    printf(PFXFMT, lvl);\
    printf(fmt, __VA_ARGS__);\
    break;\
  case 3:\
    printf(PFXFMT"FUNCTION:%s ", lvl, __FUNCTION__);\
    printf(fmt, __VA_ARGS__);\
    break;\
  case 4:\
    printf(PFXFMT"FILE:%s FUNCTION:%s ", lvl, __FILE__, __FUNCTION__);\
    printf(fmt, __VA_ARGS__);\
    break;\
  case 5:\
    printf(PFXFMT, lvl);\
    printf(fmt, __VA_ARGS__);\
    break;\
  default:\
    printf(fmt, __VA_ARGS__);\
    break;\
}\
fflush(stdout)
#else
#define __con_debug(lvl, fmt, ...)
#endif

#ifdef __cplusplus
        }
#endif


#endif //__DBGCON_H__

