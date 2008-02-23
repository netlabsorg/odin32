/* $Id: PrfTstProcess-2.c,v 1.1 2003-05-01 11:20:49 bird Exp $
 *
 * Test program which checks how long it takes to execute another
 * instance of itself and run
 *
 * Copyright (c) 2001-2003 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
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
 * nmake -f prftstprocess-2.mak
 *  or
 * wcl386 -d__OS2__=1 -bt=os2v2 /los2v2 -I%WATCOM\h\os2 PrfTstProcess-2.c
 *
 * @subsection  Complation NT
 * This works from OS/2 and NT:
 * wcl386 -d__WINNT__=1 -bt=nt /lnt -I%WATCOM\h\nt PrfTstProcess-2.c   kernel32.lib
 *
 * Optimized:
 * wcl386 -Otx -d__WINNT__=1 -bt=nt /lnt -I%WATCOM\h\nt PrfTstProcess-2.c   kernel32.lib
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

    /*
     * Child process test.
     */
    if (argc != 1)
        return 0;

    /*
     * Main process.
     */
    cChilds = 0;                        /* child count */
    rdEnd = getHz() * 20.0;             /* loop for 20 seconds */
    rdStart = rdCur = gettime();
    rdEnd += gettime();
    while (rdEnd > rdCur)
    {
        #ifndef __NOTPC__
        pid = spawnl(P_WAIT, argv[0], argv[0], "child", NULL); /* pid == 0 on success */
        #else
        pid = fork();
        if (pid == 0)
        {/* child code */
            execl(argv[0], argv[0], "child", NULL);
            fprintf(stderr, "we should NEVER be here!!\n");
            return 0;
        }
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
