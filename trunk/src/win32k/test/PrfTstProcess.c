/* $Id: PrfTstProcess.c,v 1.1 2001-07-30 00:43:19 bird Exp $
 *
 * Test program which checks how long it takes to execute another
 * instance of itself and run
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <stdio.h>
#include <sys\time.h>
#include <process.h>


#ifdef __OS2__
#define INCL_DOSMISC
#define INCL_DOSPROFILE
#include <os2.h>

long double gettime(void)
{
#if 0
    ULONG ul = 0;
    DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, &ul, sizeof(ul));
#else
    QWORD   qw;
    DosTmrQueryTime(&qw);
    return (long double)qw.ulHi * (4294967296.00) + qw.ulLo;
#endif
}

unsigned getHz(void)
{
#if 0
    return 1000;
#else
    ULONG ul = -1;
    DosTmrQueryFreq(&ul);
    return ul;
#endif
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
    long double  rdStart;
    long double  rdEnd;
    int     pid;
    #ifndef UNIX
    #else
    int     status;
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
    #ifndef UNIX
    pid = spawnl(P_WAIT, argv[0], argv[0], "child", NULL);
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
