/* $Id: crtdll.h,v 1.2 1999-11-30 14:19:02 sandervl Exp $ */

#ifndef __WINE_CRTDLL_H
#define __WINE_CRTDLL_H

#define CRTDLL_LC_ALL		0
#define CRTDLL_LC_COLLATE	1
#define CRTDLL_LC_CTYPE		2
#define CRTDLL_LC_MONETARY	3
#define CRTDLL_LC_NUMERIC	4
#define CRTDLL_LC_TIME		5
#define CRTDLL_LC_MIN		LC_ALL
#define CRTDLL_LC_MAX		LC_TIME

/* ctype defines */
#define CRTDLL_UPPER		0x1
#define CRTDLL_LOWER		0x2
#define CRTDLL_DIGIT		0x4
#define CRTDLL_SPACE		0x8
#define CRTDLL_PUNCT		0x10
#define CRTDLL_CONTROL		0x20
#define CRTDLL_BLANK		0x40
#define CRTDLL_HEX		0x80
#define CRTDLL_LEADBYTE		0x8000
#define CRTDLL_ALPHA		(0x0100|CRTDLL_UPPER|CRTDLL_LOWER)

/* syserr defines */
char __syserr00[] = "No Error";
char __syserr01[] = "Input to function out of range (EDOM)";
char __syserr02[] = "Output of function out of range (ERANGE)";
char __syserr03[] = "Argument list too long (E2BIG)";
char __syserr04[] = "Permission denied (EACCES)";
char __syserr05[] = "Resource temporarily unavailable (EAGAIN)";
char __syserr06[] = "Bad file descriptor (EBADF)";
char __syserr07[] = "Resource busy (EBUSY)";
char __syserr08[] = "No child processes (ECHILD)";
char __syserr09[] = "Resource deadlock avoided (EDEADLK)";
char __syserr10[] = "File exists (EEXIST)";
char __syserr11[] = "Bad address (EFAULT)";
char __syserr12[] = "File too large (EFBIG)";
char __syserr13[] = "Interrupted system call (EINTR)";
char __syserr14[] = "Invalid argument (EINVAL)";
char __syserr15[] = "Input or output error (EIO)";
char __syserr16[] = "Is a directory (EISDIR)";
char __syserr17[] = "Too many open files (EMFILE)";
char __syserr18[] = "Too many links (EMLINK)";
char __syserr19[] = "File name too long (ENAMETOOLONG)";
char __syserr20[] = "Too many open files in system (ENFILE)";
char __syserr21[] = "No such device (ENODEV)";
char __syserr22[] = "No such file or directory (ENOENT)";
char __syserr23[] = "Unable to execute file (ENOEXEC)";
char __syserr24[] = "No locks available (ENOLCK)";
char __syserr25[] = "Not enough memory (ENOMEM)";
char __syserr26[] = "No space left on drive (ENOSPC)";
char __syserr27[] = "Function not implemented (ENOSYS)";
char __syserr28[] = "Not a directory (ENOTDIR)";
char __syserr29[] = "Directory not empty (ENOTEMPTY)";
char __syserr30[] = "Inappropriate I/O control operation (ENOTTY)";
char __syserr31[] = "No such device or address (ENXIO)";
char __syserr32[] = "Operation not permitted (EPERM)";
char __syserr33[] = "Broken pipe (EPIPE)";
char __syserr34[] = "Read-only file system (EROFS)";
char __syserr35[] = "Invalid seek (ESPIPE)";
char __syserr36[] = "No such process (ESRCH)";
char __syserr37[] = "Improper link (EXDEV)";
char __syserr38[] = "No more files (ENMFILE)";

/* function prototypes used in crtdll.c */
extern int LastErrorToErrno(DWORD);

#endif /* __WINE_CRTDLL_H */
