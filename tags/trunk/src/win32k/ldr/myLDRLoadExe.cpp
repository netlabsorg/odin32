#include <ldr.h>

int LDRCALL myLDRLoadExe(PCHAR pExeName,PULONG p2)
{
	//kernel_printf(("_LDRLoadExe: %s - param2:0x%x", pExeName, p2));
	return _LDRLoadExe(pExeName,p2);
}


