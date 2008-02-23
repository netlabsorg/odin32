/* $Id: PrfTstProcess-4.c,v 1.1 2003-05-01 14:00:32 bird Exp $
 *
 * Test program which checks how long it takes to execute another
 * user specified process.
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
 * wcl386 -d__OS2__=1 -bt=os2v2 /los2v2 -I%WATCOM\h\os2 PrfTstProcess-4.c
 *
 * @subsection  Complation NT
 * This works from OS/2 and NT:
 * wcl386 -d__WINNT__=1 -bt=nt /lnt -I%WATCOM\h\nt PrfTstProcess-4.c   kernel32.lib
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

    if (    argc == 1
        ||  argv[1][0] == '-'
        ||  argv[1][0] == '/')
    {
        printf("syntax: %s <executable> [args]\n"
               "\n"
               "The executable is started executed repeatedly for 20 seconds.\n",
               argv[0]);
        return 8;
    }

    /*
     * Mainloop.
     */
    cChilds = 0;                        /* child count */
    rdEnd = getHz() * 20.0;             /* loop for 20 seconds */
    rdStart = rdCur = gettime();
    rdEnd += gettime();
    while (rdEnd > rdCur)
    {
        #ifndef __NOTPC__
        pid = spawnv(P_WAIT, argv[1], &argv[1]); /* pid == 0 on success */
        #else
        pid = fork();
        if (pid == 0)
        {/* child code */
            execv(argv[1], &argv[1]);
            fprintf(stderr, "we should NEVER be here!!\n");
            return 0;
        }
        #endif

        if (pid < 0)
        {
            fprintf(stderr, "Failed to spawn child\n");
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
