/* $Id: cpuhlp.h,v 1.2 2000-02-15 19:04:43 sandervl Exp $ */

#ifndef _CPUHLP_H_
#define _CPUHLP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(a)	(1<<a)

#define CPUID_FPU_PRESENT		BIT(0)
#define CPUID_VMMODE_EXTENSIONS		BIT(1)
#define CPUID_DBG_EXTENSIONS		BIT(2)
#define CPUID_PAGE_SIZE_EXTENSIONS	BIT(3)
#define CPUID_TIME_STAMP_COUNTER	BIT(4)
#define CPUID_K86_MODEL_REGS		BIT(5)
#define CPUID_MACHINE_CHECK_EXCEPTION	BIT(7)
#define CPUID_CMPXCHG8B_INSTRUCTION	BIT(8)
#define CPUID_APIC			BIT(9)
#define CPUID_MEMORY_TYPE_RANGE_REGS	BIT(12)
#define CPUID_GLOBAL_PAGING_EXTENSIONS	BIT(13)
#define CPUID_CONDITIONAL_MOVE		BIT(15)
#define CPUID_MMX			BIT(23)

BOOL  _cdecl SupportsCPUID();

void  _cdecl GetCPUVendorString(char *vendor);
DWORD _cdecl GetCPUFeatures();
DWORD _cdecl GetCPUSignature();

void  InitSystemInfo(int nrcpus);

extern DWORD CPUFeatures;

#ifdef __cplusplus
}
#endif

#endif /* _CPUHLP_H_ */

