/* $Id: versionos2.h,v 1.5 1999-12-28 19:17:09 sandervl Exp $ */

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
#define PE2LX_VERSION 4

#ifndef SYSTEM
#define SYSTEM _System
#endif

//in kernel32.dll/wprocess.cpp, used by version.dll
BOOL  SYSTEM GetVersionStruct(char *modname, char *verstruct, ULONG bufLength);
ULONG SYSTEM GetVersionSize(char *modname);

//Used by GetVersion and GetProcessVersion
#define WIN32OS2_VERSION  (4)

#define ODINNT_VERSION          4	//returned by GetVersion

//Used by GetVersionEx:
#define ODINNT_MAJOR_VERSION	4
#define ODINNT_MINOR_VERSION	0
#define ODINNT_BUILD_NR		1381
#define ODINNT_CSDVERSION	"Service Pack 3"
#define ODINNT_CSDVERSION_W	(LPWSTR)L"Service Pack 3"
#define ODINNT_SOFTWARE_TYPE    "SYSTEM"
#define ODINNT_OSTYPE_UNI       "Uniprocessor Free"
#define ODINNT_OSTYPE_SMP       "SMPprocessor Free" //??

#endif