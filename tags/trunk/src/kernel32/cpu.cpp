/* $Id: cpu.cpp,v 1.8 2000-04-29 18:26:57 sandervl Exp $ */
/*
 * Odin win32 CPU apis
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Based on Wine Code (991031: misc\cpu.c)
 *
 * Copyright 1995,1997 Morten Welinder
 * Copyright 1997-1998 Marcus Meissner
 */

#include <os2win.h>
#include <ctype.h>
#include <string.h>
#include "winreg.h"
#include "global.h"
#include "winnt.h"
#include "winerror.h"
#include "winreg.h"
#include "debugtools.h"
#include <cpuhlp.h>

#define DBG_LOCALLOG	DBG_cpu
#include "dbglocal.h"

DEFAULT_DEBUG_CHANNEL(CPU)

static BYTE PF[64]          = {0,};
static nrCPUs               = 1;
static SYSTEM_INFO cachedsi = {0};

//******************************************************************************
//******************************************************************************
void InitSystemInfo(int nrcpus)
{
  SYSTEM_INFO si;

   nrCPUs = nrcpus;
   GetSystemInfo(&si);
}
/***********************************************************************
 * 			GetSystemInfo            	[KERNELL32.404]
 *
 * Gets the current system information.
 *
 * On the first call it reads cached values, so it doesn't have to determine
 * them repeatedly. On Linux, the /proc/cpuinfo special file is used.
 *
 * It creates a registry subhierarchy, looking like:
 * \HARDWARE\DESCRIPTION\System\CentralProcessor\<processornumber>\
 *							Identifier (CPU x86)
 * Note that there is a hierarchy for every processor installed, so this
 * supports multiprocessor systems. This is done like Win95 does it, I think.
 *							
 * It also creates a cached flag array for IsProcessorFeaturePresent().
 *
 * RETURNS
 *	nothing, really
 */
VOID WINAPI GetSystemInfo(LPSYSTEM_INFO si)	/* [out] system information */
{
 HKEY   xhkey=0,hkey;
 HKEY   fpukey=0, xhfpukey;
 char   buf[20];
 DWORD  features, signature;
 static int cache = 0;

	if(!si) {
		dprintf(("GetSystemInfo -> si == NULL!!"));
		SetLastError(ERROR_INVALID_PARAMETER);
		return;
	}
	dprintf(("GetSystemInfo"));
	if (cache) {
		memcpy(si,&cachedsi,sizeof(*si));
		return;
	}
	memset(PF,0,sizeof(PF));

	/* choose sensible defaults ...
	 * FIXME: perhaps overrideable with precompiler flags?
	 */
	cachedsi.u.x.wProcessorArchitecture     = PROCESSOR_ARCHITECTURE_INTEL;
	cachedsi.dwPageSize 			= 4096;

	/* FIXME: better values for the two entries below... */
	cachedsi.lpMinimumApplicationAddress	= (void *)0x40000000;
	cachedsi.lpMaximumApplicationAddress	= (void *)0x7FFFFFFF;
	cachedsi.dwActiveProcessorMask		= 1;
	cachedsi.dwNumberOfProcessors		= nrCPUs;
	cachedsi.dwProcessorType		= PROCESSOR_INTEL_386;
	cachedsi.dwAllocationGranularity	= 0x10000;
	cachedsi.wProcessorLevel		= 3; /* 386 */
	cachedsi.wProcessorRevision		= 0;

	cache = 1; /* even if there is no more info, we now have a cacheentry */
	memcpy(si,&cachedsi,sizeof(*si));

	/* Hmm, reasonable processor feature defaults? */

        /* Create this registry key for all systems */
	if (RegCreateKeyA(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System\\CentralProcessor",&hkey)!=ERROR_SUCCESS) {
            dprintf(("Unable to register CPU information\n"));
        }

	if(SupportsCPUID()) 
	{
	    for(int i=0;i<cachedsi.dwNumberOfProcessors;i++) 
	    {
		// Create a new processor subkey
		sprintf(buf,"%d",i);
		if (xhkey)
			RegCloseKey(xhkey);
		RegCreateKeyA(hkey,buf,&xhkey);

	 	signature = GetCPUSignature();

		switch ((signature >> 8)&0xf) {
		case 3: cachedsi.dwProcessorType = PROCESSOR_INTEL_386;
			cachedsi.wProcessorLevel= 3;
			break;
		case 4: cachedsi.dwProcessorType = PROCESSOR_INTEL_486;
			cachedsi.wProcessorLevel= 4;
			break;
		case 5: cachedsi.dwProcessorType = PROCESSOR_INTEL_PENTIUM;
			cachedsi.wProcessorLevel= 5;
			break;
		case 6: cachedsi.dwProcessorType = PROCESSOR_INTEL_PENTIUM;
			cachedsi.wProcessorLevel= 5;
			break;
		default:
			break;
		}
		/* set the CPU type of the current processor */
		sprintf(buf,"CPU %ld",cachedsi.dwProcessorType);
		if (xhkey) {
			RegSetValueExA(xhkey,"Identifier",0,REG_SZ,(LPBYTE)buf,strlen(buf));
			memset(buf, 0, sizeof(buf));
			GetCPUVendorString(buf);
			RegSetValueExA(xhkey,"VendorIdentifier",0,REG_SZ,(LPBYTE)buf,strlen(buf));
		}
		cachedsi.wProcessorRevision = signature & 0xf;

//TODO: FPU fdiv bug
#if 0
		if (!lstrncmpiA(line,"fdiv_bug",strlen("fdiv_bug"))) {
			if (!lstrncmpiA(value,"yes",3))
				PF[PF_FLOATING_POINT_PRECISION_ERRATA] = TRUE;

			continue;
		}
#endif
		features = GetCPUFeatures();
		if (features & CPUID_CMPXCHG8B_INSTRUCTION)
			PF[PF_COMPARE_EXCHANGE_DOUBLE] = TRUE;
		if (features & CPUID_MMX)
			PF[PF_MMX_INSTRUCTIONS_AVAILABLE] = TRUE;

		//Create FPU key if one is present
		if (features & CPUID_FPU_PRESENT) {
			if (i == 0) {
				if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System\\FloatingPointProcessor",&fpukey)!=ERROR_SUCCESS) 
            				dprintf(("Unable to register FPU information\n"));
        		}
			// Create a new processor subkey
			if(fpukey) {
				sprintf(buf,"%d",i);
				RegCreateKeyA(fpukey,buf,&xhfpukey);
			}
		}
	
	   } //for each cpu
	}
	memcpy(si,&cachedsi,sizeof(*si));

	if (xhkey)
		RegCloseKey(xhkey);
	if (hkey)
		RegCloseKey(hkey);
	if (xhfpukey)
		RegCloseKey(xhfpukey);
	if (fpukey)
		RegCloseKey(fpukey);

}


/***********************************************************************
 * 			IsProcessorFeaturePresent	[KERNELL32.880]
 * RETURNS:
 *	TRUE if processorfeature present
 *	FALSE otherwise
 */
BOOL WINAPI IsProcessorFeaturePresent (DWORD feature)	/* [in] feature number, see PF_ defines */
{
  SYSTEM_INFO si;
  GetSystemInfo (&si); /* To ensure the information is loaded and cached */

  if (feature < 64)
    return PF[feature];
  else
    return FALSE;
}
//******************************************************************************
//******************************************************************************
