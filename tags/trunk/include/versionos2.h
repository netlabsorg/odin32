/* $Id: versionos2.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

#ifndef __VERSIONOS2__H__
#define __VERSIONOS2__H__

/*PLF
 *
 * PE2LX_VERSION is an internal version number used to verify that the
 * dll's are a matched set.  It doesn't have to be related at all to the
 * "version number of the product" we advertise to users.
 *
 * Each time a structural change in pe2lx.exe causes previous dll's to
 * be incompatible (for example,  when Sander removed the "OS2" prefix
 * from all exported names),  this constant should be incremented.
 *
 * The value is currently stored as a four byte EA (so the range is that
 * of an unsigned long).
 *
 *
 */
#define PE2LX_VERSION 2

#ifndef SYSTEM
#define SYSTEM _System
#endif

//in kernel32.dll/wprocess.cpp, used by version.dll
BOOL  SYSTEM GetVersionStruct(char *modname, char *verstruct, ULONG bufLength);
ULONG SYSTEM GetVersionSize(char *modname);

//Used by GetVersion and GetProcessVersion
//Pretend we're win95
//#define WIN32OS2_VERSION  ((1<<32)|4)
#define WIN32OS2_VERSION  (4)

#endif