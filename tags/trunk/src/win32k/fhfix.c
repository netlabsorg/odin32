#define INCL_BASE
#include <os2.h>
#include <string.h>
int main(int argc, char **argv)
{
    static char     szArgBuffer[32768];
    RESULTCODES     resc;
    int             argi;
    char *          psz;

    DosSetMaxFH(100);

    if (argc < 2)
    {
        /* complain */
        return -1;
    }

    /* first argument */
    psz = strcpy(szArgBuffer, argv[1]);
    psz += strlen(psz);
    psz++;

    if (argc > 2)
    {
        argi = 2;
        while (argi < argc)
        {
            strcpy(psz, argv[argi]);
            psz += strlen(psz);
            *psz++ = ' ';
            argi++;
        }
        psz[-1] = '\0';
    }
    *psz = '\0';

    if (DosExecPgm(NULL, 0, EXEC_SYNC, szArgBuffer, NULL, &resc, szArgBuffer) != NO_ERROR)
    {
        /*complain*/
        return -1;
    }
    return resc.codeResult;
}
