#include <ldr.h>

int LDRCALL myldrOpenNewExe(PCHAR pszName, USHORT lenName, ULONG p3, ULONG p4)
{
	char szName[CCHMAXPATH];
	int i;

	for (i = 0; i < lenName; i++)
		szName[i] = pszName[i];
	szName[i] = '\0';

//	kernel_printf(("_ldrOpenNewExe: pszName(p1):%s lenName(p2):0x%x param3:0x%x param4:0x%x", __StackToFlat(szName), lenName, p3, p4));

	return _ldrOpenNewExe(pszName, lenName, p3, p4);
}



