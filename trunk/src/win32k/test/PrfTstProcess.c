/* $Id: PrfTstProcess.c,v 1.2 2001-07-30 01:56:28 bird Exp $
 *
 * Test program which checks how long it takes to execute another
 * instance of itself and run
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
 *  Linux version 2.4.5 (SMP) Pentium III 700Mhz- GCC:
 *   0.001845 - 0.001845 sec (pid=193c)
 *   0.001832 - 0.001832 sec (pid=193e)
 *   0.001792 - 0.001792 sec (pid=1940)
 *   0.001899 - 0.001899 sec (pid=1942)
 *
 *  OS/2 WS4eB 4.5 FP 2 (SMP) Pentium III 600Mhz - Watcom -Otx:
 *   5150.000000 - 0.004316 sec (pid=0)
 *   5175.000000 - 0.004337 sec (pid=0)
 *   5143.000000 - 0.004310 sec (pid=0)
 *   5181.000000 - 0.004342 sec (pid=0)
 *
 *  OS/2 WS4eB 4.5 FP 2 (SMP) Pentium III 600Mhz - Watcom (no optimization):
 *   5510.000000 - 0.004618 sec (pid=0)
 *   5500.000000 - 0.004610 sec (pid=0)
 *   5489.000000 - 0.004600 sec (pid=0)
 *   5551.000000 - 0.004652 sec (pid=0)
 *
 *  OS/2 WS4eB 4.5 FP 2 (SMP) Pentium III 600Mhz - VAC308:
 *   6490.000000 - 0.005439 sec (pid=0)
 *   6465.000000 - 0.005418 sec (pid=0)
 *   6501.000000 - 0.005449 sec (pid=0)
 *   6496.000000 - 0.005444 sec (pid=0)
 *
 *  OS/2 WS4eB 4.5 FP 2 (SMP) Pentium III 600Mhz - VAC365:
 *   6743.000000 - 0.005651 sec (pid=0)
 *   6694.000000 - 0.005610 sec (pid=0)
 *   6705.000000 - 0.005619 sec (pid=0)
 *   7025.000000 - 0.005888 sec (pid=0)
 *
 *  OS/2 WS4eB 4.5 FP 2 (SMP) Pentium III 600Mhz - EMX -D__OS2__:
 *   15339.000000 - 0.012856 sec (pid=0)
 *   15507.000000 - 0.012997 sec (pid=0)
 *   15224.000000 - 0.012759 sec (pid=0)
 *   15714.000000 - 0.013170 sec (pid=0)
 *
 *  OS/2 WS4eB 4.5 FP 2 (SMP) Pentium III 600Mhz - EMX -D__OS2__ -D__NOTPC__:
 *   31992.000000 - 0.026813 sec (pid=1c7f)
 *   32300.000000 - 0.027071 sec (pid=1c82)
 *   31699.000000 - 0.026567 sec (pid=1c85)
 *   33570.000000 - 0.028135 sec (pid=1c88)
 *
 */

#include <stdio.h>
#include <sys/time.h>
#ifndef __NOTPC__
#include <process.h>
#endif

#ifdef __OS2__
#define INCL_DOSPROFILE
#include <os2.h>

long double gettime(void)
{
    QWORD   qw;
    DosTmrQueryTime(&qw);
    return (long double)qw.ulHi * (4294967296.00) + qw.ulLo;
}

unsigned getHz(void)
{
    ULONG ul = -1;
    DosTmrQueryFreq(&ul);
    return ul;
}


#elif defined(__WINNT__)
/*
 * Windows
 */
unsigned long __stdcall GetTickCount(void);

long double gettime(void)
{
    return (long double)GetTickCount();
}

unsigned getHz(void)
{
    return 1000;
}


#else

long double gettime(void)
{
    struct  timeval tp;
    long    sec = 0L;

    if (gettimeofday(&tp, NULL))
        return -1;
    return tp.tv_usec / 1000000.00 + tp.tv_sec;
}

unsigned getHz(void)
{
    return 1;//000000;
}

#endif


int main(int argc, char **argv)
{
    long double rdStart;
    long double rdEnd;
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
    rdStart = gettime();
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
    rdEnd = gettime();
    printf("%Lf - %Lf sec (pid=%x)\n", rdEnd - rdStart, (rdEnd - rdStart) / getHz(), pid);
    printf("(start: %Lf  end: %Lf  Hz: %d\n", rdStart, rdEnd, getHz());

    return 0;
}
