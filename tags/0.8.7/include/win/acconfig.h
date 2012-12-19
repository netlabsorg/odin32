/* $Id: acconfig.h,v 1.1 1999-05-24 20:19:08 ktk Exp $ */

/*
 * This file is used by 'autoheader' to generate the list of symbols
 * defined in the 'configure' script.
 */

/* Define if symbols declared in assembly code need an underscore prefix */
#undef NEED_UNDERSCORE_PREFIX

/* Define to use .string instead of .ascii */
#undef HAVE_ASM_STRING

/* Define if struct msghdr contains msg_accrights */
#undef HAVE_MSGHDR_ACCRIGHTS

/* Define if you have the Xxf86dga library (-lXxf86dga).  */
#undef HAVE_LIBXXF86DGA

/* Define if you have the X Shm extension */
#undef HAVE_LIBXXSHM

/* Define if you have the Xxf86vm library */
#undef HAVE_LIBXXF86VM

/* Define if you have the Xpm library */
#undef HAVE_LIBXXPM

/* Define if you have the Open Sound system.  */
#undef HAVE_OSS

/* Define if X libraries are not reentrant (compiled without -D_REENTRANT).  */
#undef NO_REENTRANT_X11

/* Define if libc is not reentrant  */
#undef NO_REENTRANT_LIBC

/* Define if libc uses __errno_location for reentrant errno */
#undef HAVE__ERRNO_LOCATION

/* Define if libc uses __error for reentrant errno */
#undef HAVE__ERROR

/* Define if libc uses ___errno for reentrant errno */
#undef HAVE___ERRNO

/* Define if all debug messages are to be compiled out */
#undef NO_DEBUG_MSGS

/* Define if TRACE messages are to be compiled out */
#undef NO_TRACE_MSGS

/* Define if the struct statfs has the member bavail */
#undef STATFS_HAS_BAVAIL

/* Define if the struct statfs has the member bfree */
#undef STATFS_HAS_BFREE

/* Define if we have a working sigaltstack */
#undef HAVE_WORKING_SIGALTSTACK

/* Define if the struct statfs is defined by <sys/vfs.h> */
#undef STATFS_DEFINED_BY_SYS_VFS

/* Define if the struct statfs is defined by <sys/statfs.h> */
#undef STATFS_DEFINED_BY_SYS_STATFS

/* Define if the struct statfs is defined by <sys/mount.h> */
#undef STATFS_DEFINED_BY_SYS_MOUNT

/* Define if we can use ncurses for full-screen access */
#undef HAVE_LIBNCURSES

/* Define if we can use curses (if no ncurses) for full-screen access */
#undef HAVE_LIBCURSES

/* Define if ncurses have the new resizeterm function */
#undef HAVE_RESIZETERM

/* Define if ncurses have the new getbkgd function */
#undef HAVE_GETBKGD

/* Define if we can a compatible xterm program */
#undef XTERM_PROGRAM

/* Define if IPX should use netipx/ipx.h from libc */
#undef HAVE_IPX_GNU

/* Define if IPX includes are taken from Linux kernel */
#undef HAVE_IPX_LINUX

/* Define if Mesa is present on the system or not */
#undef HAVE_LIBMESAGL

/* Define if the system has dynamic link library support with the dl* API */
#undef HAVE_DL_API
