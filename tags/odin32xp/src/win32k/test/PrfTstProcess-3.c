/* $Id: PrfTstProcess-3.c,v 1.1 2003-05-01 11:20:50 bird Exp $
 *
 * Test program which checks how long it takes to execute another process.
 *
 * Copyright (c) 2001-2003 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/** @design Process Startup and Termination cost.
 *
 * The purpose is to compare the cost of creating a child process on different
 * platforms; revealing which is the best ones...
 *
 * Later analysis on why will be done I hope...
 *
 *
 * @subsection Test Results
 *
 *
 * @subsection  Compilation OS/2
 * Just as normal odin apps:<br>
 * wcl386 -d__OS2__=1 -bt=os2v2 /los2v2 -I%WATCOM\h\os2 PrfTstProcess-3.c
 * wcl386 -d__OS2__=1 -bt=os2v2 /los2v2 -I%WATCOM\h\os2 PrfTstProcess-3-child.c
 *
 * @subsection  Complation NT
 * This works from OS/2 and NT:
 * wcl386 -d__WINNT__=1 -bt=nt /lnt -I%WATCOM\h\nt PrfTstProcess-3.c   kernel32.lib
 * wcl386 -d__WINNT__=1 -bt=nt /lnt -I%WATCOM\h\nt PrfTstProcess-3-child.c
 *
 * Optimized:
 *  Add -Otx
 *
 */

#include "PrfTiming.h"

int main(int argc, char **argv)
{
    long double rdCur;
    long double rdStart;
    long double rdEnd;
    unsigned    cChilds;
    int         pid;
    #ifdef __NOTPC__
    int         status;
    #endif
    char        szChild[256];


    /*
     * Mainloop.
     */
    strcpy(szChild, argv[0]);
    #ifdef __NOTPC__
    strcat(szChild, "-child");
    #else
    if (!stricmp(szChild + strlen(szChild) - 4, ".exe"))
        szChild[strlen(szChild) - 4] = '\0';
    strcat(szChild, "-child.exe");
    #endif

    cChilds = 0;                        /* child count */
    rdEnd = getHz() * 20.0;             /* loop for 20 seconds */
    rdStart = rdCur = gettime();
    rdEnd += gettime();
    while (rdEnd > rdCur)
    {
        #ifndef __NOTPC__
        pid = spawnl(P_WAIT, szChild, szChild, "child", NULL); /* pid == 0 on success */
        #else
        pid = fork();
        if (pid == 0)
        {/* child code */
            execl(szChild, szChild, "child", NULL);
            fprintf(stderr, "we should NEVER be here!!\n");
            return 0;
        }
        #endif

        if (pid < 0)
        {
            fprintf(stderr, "Failed to spawn child '%s'\n", szChild);
            return 0;
        }

        #ifdef __NOTPC__
        if (pid > 0)
            pid = wait(&status);
        #endif
        cChilds++;
        rdCur = gettime();
    }
    printf("Spawned %d childs in %Lf seconds\n", cChilds, (rdCur - rdStart) / getHz());
    printSystemInfo();
    return 0;
}
