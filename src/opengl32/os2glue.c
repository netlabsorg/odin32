/* opengl32 - native OS/2 module loading helpers (kept apart from the Win32 headers) */
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#include <os2wrap.h>
#include <string.h>

unsigned long glw_loadmesa(char *failname, unsigned long cb, unsigned long *hmod)
{
    return DosLoadModule(failname, cb, "MesaGL7", (PHMODULE)hmod);
}

void *glw_proc(unsigned long hmod, const char *name)
{
    PFN pf = 0;
    if(DosQueryProcAddr((HMODULE)hmod, 0, (PSZ)name, &pf))
        return 0;
    return (void *)pf;
}
