/* $Id: CmdQd.c,v 1.2 2001-08-16 04:27:44 bird Exp $
 *
 * Command Queue Daemon / Client.
 *
 * Designed to execute commands asyncronus using multiple workers,
 * and when all commands are submitted wait for them to complete.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * GPL
 *
 */


/** @design Command Queue Daemon.
 * This command daemon orginated as tool to exploit SMP and UNI systems better
 * building large programs, but also when building one specific component of
 * that program. It is gonna work just like the gnu make -j option.
 *
 * @subsection Work flow
 *
 * 1. Init daemon process. Creates a daemon process with a given number of
 *    workers. This is a detached process.
 * 2. Submit jobs to the daemon. The daemon will queue the jobs and the
 *    workers will start working at once there is a job for them.
 * 3. The nmake script will issue a wait command. We will now wait for all
 *    jobs to finish and in the mean time we'll display output from the jobs.
 *    Failing jobs will be queued up and show when all jobs are finished.
 * 4. Two options: kill the daemon or start at step 2 again.
 *
 *
 * @subsection Client <-> Daemon communication
 *
 * Client and daemon is one and the same executable. This has some advantages
 * like implicit preloading of the client, fewer source files and fewer programs
 * to install.
 *
 * The communication between the client and the daemon will use shared memory
 * and an mutex semaphore which directs the conversation. The shared memory
 * block is allocated by the daemon and will have a quite simple layout:
 *      Mutex Semaphore.
 *      Message Type.
 *      Message specific data:
 *       - Submit job:
 *              Returcode ignore. (4 bytes)
 *              Command to execute. (1 Kb)
 *              Current directory. (260 bytes)
 *              Environment block. (about 62KB)
 *       - Submit job response:
 *              Success/failure indicator.
 *
 *       - Wait:
 *              Nothing.
 *       - Wait response:
 *              More output indicator.
 *              Success/failure indicator.
 *              Job output (about 63KB)
 *
 *       - Kill:
 *              Nothing.
 *       - Kill response:
 *              Success/failure indicator.
 *
 * The shared memory block is 64KB.
 *
 *
 * @subsection The Workers
 *
 * The workers is individual threads which waits for a job to be submitted to
 * execution. Each worker is two threads. The job is executed thru a spawnvpe
 * call and all stdout is read by the 1st worker thread, stderr is read by the
 * 2nd thread. (Initially we've merged the two pipes and used one thread.)
 * The output will be buffered (up to 4 MB). When the job is completed we'll
 * put the output into either the success queue or the failure queue
 * depending on the result.
 *
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <direct.h>
#include <signal.h>
#include <process.h>

#define INCL_BASE
#include <os2.h>


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define SHARED_MEM_NAME     "\\SHAREMEM\\CmdQd"
#define SHARED_MEM_SIZE     65536
#define IDLE_TIMEOUT_MS     -1 //(60*1000*3)
#define OUTPUT_CHUNK        (8192-8)

#define HF_STDIN    0
#define HF_STDOUT   1
#define HF_STDERR   2

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct SharedMem
{
    HEV         hevClient;              /* Client will wait on this. */
    HEV         hevDaemon;              /* Daemon will wait on this.  */
    HMTX        hmtx;                   /* Owner of the shared memory. */
    HMTX        hmtxClient;             /* Take and release this for each  */
                                        /* client -> server -> client talk. */
    enum
    {
        msgUnknown = 0,
        msgSubmit = 1,
        msgSubmitResponse = 2,
        msgWait = 3,
        msgWaitResponse = 4,
        msgKill = 5,
        msgKillResponse = 6
    }           enmMsgType;

    union
    {
        struct Submit
        {
            unsigned rcIgnore;          /* max return code to accept as good. */
            char    szCommand[1024];    /* job command. */
            char    szCurrentDir[CCHMAXPATH]; /* current directory. */
            int     cchEnv;             /* Size of the environment block */
            char    szzEnv[SHARED_MEM_SIZE - CCHMAXPATH - 1024 - 4 - 4 - 4 - 4 - 4 - 4];
                                        /* Environment block. */
        } Submit;

        struct SubmitResponse
        {
            BOOL    fRc;                /* Success idicator. */
        } SubmitResponse;


        struct Wait
        {
            int     iNothing;           /* Dummy. */
        } Wait;

        struct WaitResponse
        {
            BOOL    fMore;              /* More data. */
            int     rc;                 /* return code of first failing job. */
                                        /* only valid if fMore == FALSE. */
            char    szOutput[SHARED_MEM_SIZE- 4 - 4 - 4 - 4 - 4 - 4 - 4];
                                        /* The output of one or more jobs. */
        } WaitResponse;


        struct Kill
        {
            int     iNothing;           /* dummy. */
        } Kill;

        struct KillResponse
        {
            BOOL    fRc;                /* Success idicator. */
        } KillResponse;

    } u1;

} SHAREDMEM, *PSHAREDMEM;


typedef struct JobOutput
{
    struct JobOutput *  pNext;          /* Pointer to next output chunk. */
    int     cchOutput;                  /* Bytes used of the szOutput member. */
    char    szOutput[OUTPUT_CHUNK];     /* Output. */
} JOBOUTPUT, *PJOBOUTPUT;


typedef struct Job
{
    struct Job *    pNext;              /* Pointer to next job. */
    int             rc;                 /* Result. */
    PJOBOUTPUT      pJobOutput;         /* Output. */
    struct Submit   JobInfo;            /* Job. */
} JOB, *PJOB;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
PSHAREDMEM  pShrMem;                    /* Pointer to the shared memory.      */

PJOB        pJobQueue;                  /* Linked list of jobs. */
PJOB        pJobQueueEnd;               /* Last job entry. */
HMTX        hmtxJobQueue;               /* Read/Write mutex. */
HEV         hevJobQueue;                /* Incomming job event sem. */
ULONG       cJobs;                      /* Count of jobs submitted. */

HMTX        hmtxJobQueueFine;           /* Read/Write mutex for the next two queues. */
HEV         hevJobQueueFine;            /* Posted when there is more output. */
PJOB        pJobCompleted;              /* Linked list of successful jobs. */
PJOB        pJobCompletedLast;          /* Last successful job entry. */
PJOB        pJobFailed;                 /* Linked list of failed jobs. */
PJOB        pJobFailedLast;             /* Last failed job entry. */
ULONG       cJobsFinished;              /* Count of jobs finished (failed or completed). */

HMTX        hmtxExec;                   /* Execute childs mutex sem. Required */
                                        /* since we redirect standard files handles */
                                        /* and changes the currentdirectory. */

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void syntax(void);

/* operations */
int  Init(const char *arg0, int cWorkers);
int  Daemon(int cWorkers);
int  DaemonInit(int cWorkers);
void signalhandler(int sig);
void Worker(void * iWorkerId);
int  Submit(int rcIgnore);
int  Wait(void);
int  Kill(void);

/* shared memory helpers */
int  shrmemCreate(void);
int  shrmemOpen(void);
void shrmemFree(void);
int  shrmemSendDaemon(BOOL fWait);
int  shrmemSendClient(int enmMsgTypeResponse);

/* error handling */
void _Optlink Error(const char *pszFormat, ...);


int main(int argc, char **argv)
{

    /*
     * Display help.
     */
    if (argc < 2 || (argv[1][0] == '-'))
    {
        syntax();
        if (argc < 2)
        {
            printf("\n!syntax error!");
            return -1;
        }
        return 0;
    }

    /*
     * String switch on command.
     */
    if (!stricmp(argv[1], "submit"))
    {
        int rcIgnore = 0;
        if (argc == 2)
        {
            printf("fatal error: There is no job to submit...\n");
            return -1;
        }
        if (argv[2][0] == '-' && (rcIgnore = atoi(argv[2]+1)) <= 0)
        {
            printf("syntax error: Invalid ignore return code number...\n");
            return -1;
        }
        return Submit(rcIgnore);
    }
    else if (!stricmp(argv[1], "wait"))
    {
        return Wait();
    }
    else if (!strcmp(argv[1], "kill"))
    {
        return Kill();
    }
    else if (!strcmp(argv[1], "init"))
    {
        if (argc != 3 || atoi(argv[2]) <= 0 || atoi(argv[2]) >= 256)
        {
            printf("fatal error: invalid/missing number of workers.\n");
            return -1;
        }
        return Init(argv[0], atoi(argv[2]));
    }
    else if (!strcmp(argv[1], "!Daemon!"))
    {
        if (argc != 3 || atoi(argv[2]) <= 0)
        {
            printf("fatal error: no worker count specified or to many parameters.\n");
            return -2;
        }

        return Daemon(atoi(argv[2]));
    }
    else
    {
        syntax();
        printf("\n!invalid command '%s'.\n", argv[1]);
        return -1;
    }

    return 0;
}


/**
 * Display syntax.
 */
void syntax(void)
{
    printf(
        "Command Queue Daemon v0.0.1\n"
        "---------------------------\n"
        "syntax: CmdQd.exe <command> [args]\n"
        "\n"
        "commands:\n"
        "    init <workers>\n"
        "        Initiates the command queue daemon with the given number of workers.\n"
        "\n"
        "    submit [-<n>] <command> [args]\n"
        "        Submits a command to the daemon.\n"
        "        Use '-<n>' to tell use to ignore return code 0-n.\n"
        "    \n"
        "    wait\n"
        "        Wait for all commands which are queued up to complete.\n"
        "        rc = count of failing commands.\n"
        "    \n"
        "    kill\n"
        "        Kills the daemon. Daemon will automatically die after\n"
        "        idling for some time.\n"
        "\n"
        "Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)\n"
        );
}


/**
 * Starts a daemon process.
 * @returns 0 on success.
 *          -4 on error.
 * @param   arg0        Executable filename.
 * @param   cWorkers    Number of workers to start.
 */
int Init(const char *arg0, int cWorkers)
{
    int             rc;
    RESULTCODES     Res;                /* dummy, unused */
    char            szArg[CCHMAXPATH + 32];

    if (!getenv("COMSPEC"))
    {
        Error("Fatal error: env. var. COMSPEC not found!\n");
        return 0;
    }

    sprintf(&szArg[0], "%s\t!Daemon! %d", arg0, cWorkers);
    szArg[strlen(arg0)] = '\0';
    rc = DosExecPgm(NULL, 0, EXEC_BACKGROUND, &szArg[0], NULL, &Res, &szArg[0]);
    if (rc)
        Error("Fatal error: Failed to start daemon. rc=%d\n");
    return rc;
}


/**
 * This process is to be a daemon with a given number of works.
 * @returns 0 on success.
 *          -4 on error.
 * @param   cWorkers    Number of workers to start.
 * @sketch
 */
int Daemon(int cWorkers)
{
    int     rc;

    /*
     * Init Shared memory
     */
    rc = shrmemCreate();
    if (rc)
        return rc;

    /*
     * Init queues and semaphores.
     */
    rc = DaemonInit(cWorkers);
    if (rc)
    {
        shrmemFree();
        return rc;
    }

    /*
     * Do work!
     */
    rc = shrmemSendDaemon(TRUE);
    while (!rc)
    {
        switch (pShrMem->enmMsgType)
        {
            case msgSubmit:
            {
                PJOB    pJob;

                /*
                 * Make job entry.
                 */
                _heap_check();
                pJob = malloc((int)&((PJOB)0)->JobInfo.szzEnv[pShrMem->u1.Submit.cchEnv]);
                if (pJob)
                {
                    _heap_check();
                    memcpy(&pJob->JobInfo, &pShrMem->u1.Submit,
                           (int)&((struct Submit *)0)->szzEnv[pShrMem->u1.Submit.cchEnv]);
                    _heap_check();
                    pJob->rc = -1;
                    pJob->pNext = NULL;
                    pJob->pJobOutput = NULL;

                    /*
                     * Insert the entry.
                     */
                    rc = DosRequestMutexSem(hmtxJobQueue, SEM_INDEFINITE_WAIT);
                    if (rc)
                        break;
                    if (!pJobQueue)
                        pJobQueueEnd = pJobQueue = pJob;
                    else
                    {
                        pJobQueueEnd->pNext = pJob;
                        pJobQueueEnd = pJob;
                    }
                    cJobs++;
                    DosReleaseMutexSem(hmtxJobQueue);

                    /*
                     * Post the queue to wake up workers.
                     */
                    DosPostEventSem(hevJobQueue);
                    pShrMem->u1.SubmitResponse.fRc = TRUE;
                }
                else
                {
                    Error("Internal Error: Out of memory (line=%d)\n", __LINE__);
                    pShrMem->u1.SubmitResponse.fRc = FALSE;
                }
                pShrMem->enmMsgType = msgSubmitResponse;
                rc = shrmemSendDaemon(TRUE);
                break;
            }


            case msgWait:
            {
                PJOB        pJob = NULL;
                PJOBOUTPUT  pJobOutput = NULL;
                char *      psz;
                int         cch = 0;
                char *      pszOutput;
                int         cchOutput;
                int         rcFailure = 0;
                BOOL        fMore = TRUE;
                ULONG       ulIgnore;
                void *      pv;

                DosPostEventSem(hevJobQueueFine); /* just so we don't get stuck in the loop... */
                do
                {
                    /* init response message */
                    pShrMem->enmMsgType = msgWaitResponse;
                    pShrMem->u1.WaitResponse.szOutput[0] = '\0';
                    pszOutput = &pShrMem->u1.WaitResponse.szOutput[0];
                    cchOutput = sizeof(pShrMem->u1.WaitResponse.szOutput) - 1;

                    /*
                     * Wait for output.
                     */
                    rc = DosWaitEventSem(hevJobQueueFine, SEM_INDEFINITE_WAIT);
                    if (rc)
                        break;

                    /*
                     * Copy output - Optimized so we don't cause to many context switches.
                     */
                    do
                    {
                        /*
                         * Next job.
                         */
                        if (!pJobOutput)
                        {
                            rc = DosRequestMutexSem(hmtxJobQueueFine, SEM_INDEFINITE_WAIT);
                            if (rc)
                                break;
                            pv = pJob;
                            pJob = pJobCompleted;
                            if (pJob)
                            {
                                pJobCompleted = pJob->pNext;
                                if (!pJobCompleted)
                                    pJobCompletedLast = NULL;
                            }

                            if (!pJob && cJobs == cJobsFinished)
                            {   /* all jobs finished, we may start output failures. */
                                pJob = pJobFailed;
                                if (pJob)
                                {
                                    if (rcFailure == 0)
                                         rcFailure = pJob->rc;

                                    pJobFailed = pJob->pNext;
                                    if (!pJobFailed)
                                        pJobFailedLast = NULL;
                                }
                                else
                                    fMore = FALSE;
                            }
                            else
                                DosResetEventSem(hevJobQueueFine, &ulIgnore); /* No more output, prepare wait. */
                            DosReleaseMutexSem(hmtxJobQueueFine);

                            if (pJob && pJob->pJobOutput)
                            {
                                pJobOutput = pJob->pJobOutput;
                                psz = pJobOutput->szOutput;
                                cch = pJobOutput->cchOutput;
                            }
                            if (pv)
                                free(pv);
                        }

                        /*
                         * Anything to output?
                         */
                        if (pJobOutput)
                        {
                            /*
                             * Copy output.
                             */
                            do
                            {
                                if (cch)
                                {   /* copy */
                                    int cchCopy = min(cch, cchOutput);
                                    memcpy(pszOutput, psz, cchCopy);
                                    psz       += cchCopy; cch       -= cchCopy;
                                    pszOutput += cchCopy; cchOutput -= cchCopy;
                                }
                                if (!cch)
                                {   /* next chunk */
                                    pv = pJobOutput;
                                    pJobOutput = pJobOutput->pNext;
                                    if (pJobOutput)
                                    {
                                        psz = &pJobOutput->szOutput[0];
                                        cch = pJobOutput->cchOutput;
                                    }
                                    free(pv);
                                }
                            } while (cch && cchOutput);
                        }
                        else
                            break;      /* no more output, let's send what we got. */

                    } while (!rc && fMore && cchOutput);

                    /*
                     * We've got a message to send.
                     */
                    if (rc)
                        break;
                    *pszOutput = '\0';
                    pShrMem->u1.WaitResponse.rc = rcFailure;
                    pShrMem->u1.WaitResponse.fMore = fMore;
                    rc = shrmemSendDaemon(TRUE);
                } while (!rc && fMore);
                break;
            }


            case msgKill:
            {
                pShrMem->enmMsgType = msgKillResponse;
                pShrMem->u1.KillResponse.fRc = TRUE;
                shrmemSendDaemon(FALSE);
                rc = -1;
                break;
            }

            default:
                Error("Internal error: Invalid message id %d\n", pShrMem->enmMsgType);
                pShrMem->enmMsgType = msgUnknown;
                rc = shrmemSendDaemon(TRUE);
        }
    }

    /*
     * Cleanup.
     */
    shrmemFree();
    DosCloseMutexSem(hmtxJobQueue);
    DosCloseMutexSem(hmtxJobQueueFine);
    DosCloseEventSem(hevJobQueueFine);
    DosCloseMutexSem(hmtxExec);
    DosCloseEventSem(hevJobQueue);

    _dump_allocated(16);

    return 0;
}


/**
 * Help which does most of the daemon init stuff.
 * @returns 0 on success.
 * @param   cWorkers    Number of worker threads to start.
 */
int DaemonInit(int cWorkers)
{
    int     rc;
    int     i;

    /*
     * Init queues and semaphores.
     */
    rc = DosCreateEventSem(NULL, &hevJobQueue, 0, FALSE);
    if (!rc)
    {
        rc = DosCreateMutexSem(NULL, &hmtxJobQueue, 0, FALSE);
        if (!rc)
        {
            rc = DosCreateMutexSem(NULL, &hmtxJobQueueFine, 0, FALSE);
            if (!rc)
            {
                rc = DosCreateEventSem(NULL, &hevJobQueueFine, 0, FALSE);
                if (!rc)
                {
                    rc = DosCreateMutexSem(NULL, &hmtxExec, 0, FALSE);
                    if (!rc)
                    {
                        /*
                         * Start workers.
                         */
                        rc = 0;
                        for (i = 0; i < cWorkers; i++)
                            if (_beginthread(Worker, NULL, 64*1024, (void*)i) == -1)
                            {
                                Error("Fatal error: failed to create worker thread no. %d\n", i);
                                rc = -1;
                                break;
                            }
                         if (!rc)
                         {
                             DosSetMaxFH(cWorkers * 3 + 20);
                             return 0;      /* success! */
                         }

                         /* failure */
                         DosCloseMutexSem(hmtxExec);
                    }
                    else
                        Error("Fatal error: failed to create exec mutex. rc=%d", rc);
                    DosCloseEventSem(hevJobQueueFine);
                }
                else
                    Error("Fatal error: failed to create job queue fine event sem. rc=%d", rc);
                DosCloseMutexSem(hmtxJobQueueFine);
            }
            else
                Error("Fatal error: failed to create job queue fine mutex. rc=%d", rc);
            DosCloseMutexSem(hmtxJobQueue);
        }
        else
            Error("Fatal error: failed to create job queue mutex. rc=%d", rc);
        DosCloseEventSem(hevJobQueue);
    }
    else
        Error("Fatal error: failed to create job queue event sem. rc=%d", rc);

    return rc;
}


/**
 * Daemon signal handler.
 */
void signalhandler(int sig)
{
     shrmemFree();
     exit(-42);
}


/**
 * Worker thread.
 * @param   iWorkerId   The worker process id.
 * @sketch
 */
void Worker(void * iWorkerId)
{
    while (!DosWaitEventSem(hevJobQueue, SEM_INDEFINITE_WAIT))
    {
        PJOB    pJob;

        /*
         * Get job.
         */
        if (DosRequestMutexSem(hmtxJobQueue, SEM_INDEFINITE_WAIT))
            return;
        pJob = pJobQueue;
        if (pJob)
        {
            if (pJob != pJobQueueEnd)
                pJobQueue = pJob->pNext;
            else
            {
                ULONG ulIgnore;
                pJobQueue = pJobQueueEnd = NULL;
                DosResetEventSem(hevJobQueue, &ulIgnore);
            }
        }
        DosReleaseMutexSem(hmtxJobQueue);

        /*
         * Execute job.
         */
        if (pJob)
        {
            int         rc;
            char        szArg[4096];
            char        szObj[256];
            PJOBOUTPUT  pJobOutput = NULL;
            PJOBOUTPUT  pJobOutputLast = NULL;
            RESULTCODES Res;
            PID         pid;
            HFILE       hStdOut = HF_STDOUT;
            HFILE       hStdErr = HF_STDERR;
            HFILE       hStdOutSave = -1;
            HFILE       hStdErrSave = -1;
            HPIPE       hPipeR = NULLHANDLE;
            HPIPE       hPipeW = NULLHANDLE;

            //printf("debug-%d: start %s\n", iWorkerId, pJob->JobInfo.szCommand);

            /*
             * Redirect output and start process.
             */
            sprintf(szArg, "%s\t /C \"%s\"\n", getenv("COMSPEC"), pJob->JobInfo.szCommand);
            *strchr(szArg, '\t') = '\0';
            rc = DosCreatePipe(&hPipeR, &hPipeW, sizeof(pJobOutput->szOutput) - 1);
            if (rc)
            {
                Error("Internal Error: Failed to create pipe! rc=%d\n", rc);
                return;
            }

            if (DosRequestMutexSem(hmtxExec, SEM_INDEFINITE_WAIT))
            {
                DosClose(hPipeR);
                DosClose(hPipeW);
                return;
            }

            pJob->pJobOutput = pJobOutput = pJobOutputLast = malloc(sizeof(JOBOUTPUT));
            pJobOutput->pNext = NULL;
            pJobOutput->cchOutput = sprintf(pJobOutput->szOutput, "Job: %s\n", pJob->JobInfo.szCommand);

            rc = DosSetDefaultDisk(  pJob->JobInfo.szCurrentDir[0] >= 'a'
                                   ? pJob->JobInfo.szCurrentDir[0] - 'a' + 1
                                   : pJob->JobInfo.szCurrentDir[0] - 'A' + 1);
            rc += DosSetCurrentDir(pJob->JobInfo.szCurrentDir);
            if (!rc)
            {
                assert(   pJob->JobInfo.szzEnv[pJob->JobInfo.cchEnv-1] == '\0'
                       && pJob->JobInfo.szzEnv[pJob->JobInfo.cchEnv-2] == '\0');
                DosDupHandle(HF_STDOUT, &hStdOutSave);
                DosDupHandle(HF_STDERR, &hStdErrSave);
                DosDupHandle(hPipeW, &hStdOut);
                DosDupHandle(hPipeW, &hStdErr);
                rc = DosExecPgm(szObj, sizeof(szObj), EXEC_ASYNCRESULT,
                                szArg, pJob->JobInfo.szzEnv, &Res, szArg);
                DosClose(hStdOut); hStdOut = HF_STDOUT;
                DosClose(hStdErr); hStdErr = HF_STDERR;
                DosDupHandle(hStdOutSave, &hStdOut);
                DosDupHandle(hStdErrSave, &hStdErr);
                DosClose(hStdOutSave);
                DosClose(hStdErrSave);
                DosReleaseMutexSem(hmtxExec);
                DosClose(hPipeW);


                /*
                 * Read Output.
                 */
                if (!rc)
                {
                    ULONG       cchRead;
                    ULONG       cchRead2 = 0;

                    cchRead = sizeof(pJobOutput->szOutput) - 1;
                    while (((rc = DosRead(hPipeR,
                                         &pJobOutput->szOutput[pJobOutput->cchOutput],
                                         cchRead, &cchRead2)) == NO_ERROR
                            || rc == ERROR_MORE_DATA)
                           && cchRead2 != 0)
                    {
                        pJobOutput->cchOutput += cchRead2;
                        pJobOutput->szOutput[pJobOutput->cchOutput] = '\0';

                        /* prepare next read */
                        cchRead = sizeof(pJobOutput->szOutput) - pJobOutput->cchOutput - 1;
                        if (cchRead < 16)
                        {
                            pJobOutput = pJobOutput->pNext = malloc(sizeof(JOBOUTPUT));
                            pJobOutput->pNext = NULL;
                            pJobOutput->cchOutput = 0;
                            cchRead = sizeof(pJobOutput->szOutput) - 1;
                        }
                        cchRead2 = 0;
                    }
                    rc = 0;
                }

                /* finished reading */
                DosClose(hPipeR);

                /*
                 * Get result.
                 */
                if (!rc)
                {
                    DosWaitChild(DCWA_PROCESS, DCWW_WAIT, &Res, &pid, Res.codeTerminate);
                    if (    Res.codeResult <= pJob->JobInfo.rcIgnore
                        &&  Res.codeTerminate == TC_EXIT)
                        pJob->rc = 0;
                    else
                    {
                        pJob->rc = -1;
                        rc = sprintf(szArg, "failed with rc=%d term=%d\n", Res.codeResult, Res.codeTerminate);
                        if (rc + pJobOutput->cchOutput + 1 >= sizeof(pJobOutput->szOutput))
                        {
                            pJobOutput = pJobOutput->pNext = malloc(sizeof(JOBOUTPUT));
                            pJobOutput->pNext = NULL;
                            pJobOutput->cchOutput = 0;
                        }
                        strcpy(&pJobOutput->szOutput[pJobOutput->cchOutput], szArg);
                        pJobOutput->cchOutput += rc;
                    }
                }
                else
                {
                    pJobOutput->cchOutput += sprintf(&pJobOutput->szOutput[pJobOutput->cchOutput],
                                                     "DosExecPgm failed with rc=%d for command %s %s\n"
                                                     "    obj=%s\n",
                                                     rc, szArg, pJob->JobInfo.szCommand, szObj);
                    pJob->rc = -1;
                }
            }
            else
            {
                /*
                 * ChDir failed.
                 */
                DosReleaseMutexSem(hmtxExec);
                pJobOutput->cchOutput += sprintf(&pJobOutput->szOutput[pJobOutput->cchOutput ],
                                                 "Failed to set current directory to: %s\n",
                                                 pJob->JobInfo.szCurrentDir);
                pJob->rc = -1;
                DosClose(hPipeR);
            }


            /*
             * Insert result in result queue.
             */
            if (DosRequestMutexSem(hmtxJobQueueFine, SEM_INDEFINITE_WAIT))
                return;
            pJob->pNext = NULL;
            if (!pJob->rc)              /* 0 on success. */
            {
                if (pJobCompletedLast)
                    pJobCompletedLast->pNext = pJob;
                else
                    pJobCompleted = pJob;
                pJobCompletedLast = pJob;
            }
            else
            {
                if (pJobFailedLast)
                    pJobFailedLast->pNext = pJob;
                else
                    pJobFailed = pJob;
                pJobFailedLast = pJob;
            }
            cJobsFinished++;
            DosReleaseMutexSem(hmtxJobQueueFine);
            /* wake up Wait. */
            DosPostEventSem(hevJobQueueFine);
            //printf("debug-%d: fine\n", iWorkerId);
        }
    }
}


/**
 * Submits a command to the daemon.
 * @returns 0 on success.
 *          -3 on failure.
 * @param   rcIgnore   Ignores returcodes ranging from 0 to rcIgnore.
 */
int Submit(int rcIgnore)
{
    int     i;
    int     cch;
    int     rc;
    char *  psz;
    PPIB    ppib;
    PTIB    ptib;

    DosGetInfoBlocks(&ptib, &ppib);
    rc = shrmemOpen();
    if (rc)
        return rc;

    /*
     * Build message.
     */
    pShrMem->enmMsgType = msgSubmit;
    pShrMem->u1.Submit.rcIgnore = rcIgnore;
    _getcwd(pShrMem->u1.Submit.szCurrentDir, sizeof(pShrMem->u1.Submit.szCurrentDir));

    /* command */
    psz = ppib->pib_pchcmd;
    psz += strlen(psz) + 1 + 7; /* 7 = strlen("submit ")*/
    while (*psz == ' ' || *psz == '\t')
        psz++;
    if (*psz == '-')
    {
        while (*psz != ' ' && *psz != '\t')
            psz++;
        while (*psz == ' ' || *psz == '\t')
            psz++;
    }
    cch = strlen(psz) + 1;
    if (cch > sizeof(pShrMem->u1.Submit.szCommand))
    {
        Error("Fatal error: Command too long.\n");
        shrmemFree();
        return -1;
    }
    memcpy(&pShrMem->u1.Submit.szCommand[0], psz, cch);

    /* environment */
    for (cch = 1, psz = ppib->pib_pchenv; *psz != '\0';)
    {
        int cchVar = strlen(psz) + 1;
        cch += cchVar;
        psz += cchVar;
    }
    if (    ppib->pib_pchenv[cch-2] != '\0'
        ||  ppib->pib_pchenv[cch-1] != '\0')
    {
        Error("internal error\n");
        return -1;
    }
    if (cch > sizeof(pShrMem->u1.Submit.szzEnv))
    {
        Error("Fatal error: environment is to bit, cchEnv=%d\n", cch);
        shrmemFree();
        return -ERROR_BAD_ENVIRONMENT;
    }
    pShrMem->u1.Submit.cchEnv = cch;
    memcpy(&pShrMem->u1.Submit.szzEnv[0], ppib->pib_pchenv, cch);


    /*
     * Send message and get respons.
     */
    rc = shrmemSendClient(msgSubmitResponse);
    if (rc)
    {
        shrmemFree();
        return rc;
    }

    rc = !pShrMem->u1.SubmitResponse.fRc;
    shrmemFree();
    return rc;
}


/**
 * Waits for the commands to complete.
 * Will write all output from completed command to stdout.
 * Will write failing commands last.
 * @returns Count of failing commands.
 */
int Wait(void)
{
    int     rc;

    rc = shrmemOpen();
    if (rc)
        return rc;
    do
    {
        pShrMem->enmMsgType = msgWait;
        pShrMem->u1.Wait.iNothing = 0;
        rc = shrmemSendClient(msgWaitResponse);
        if (rc)
        {
            shrmemFree();
            return -1;
        }
        printf("%s", pShrMem->u1.WaitResponse.szOutput);
        /*
         * Release the client mutex if more data and yield the CPU.
         * So we can submit more work. (Odin nmake lib...)
         */
        if (pShrMem->u1.WaitResponse.fMore)
        {
            DosReleaseMutexSem(pShrMem->hmtxClient);
            DosSleep(0);
            rc = DosRequestMutexSem(pShrMem->hmtxClient, SEM_INDEFINITE_WAIT);
            if (rc)
            {
                Error("Fatal error: failed to get client mutex. rc=%d\n", rc);
                shrmemFree();
                return -1;
            }
        }
    } while (pShrMem->u1.WaitResponse.fMore);

    rc = pShrMem->u1.WaitResponse.rc;
    shrmemFree();
    return rc;
}


/**
 * Sends a kill command to the daemon to kill it and its workers.
 * @returns 0.
 */
int Kill(void)
{
    int     rc;

    rc = shrmemOpen();
    if (rc)
        return rc;

    pShrMem->enmMsgType = msgKill;
    pShrMem->u1.Kill.iNothing = 0;
    rc = shrmemSendClient(msgKillResponse);
    if (!rc)
        rc = !pShrMem->u1.KillResponse.fRc;

    shrmemFree();
    return rc;
}


/**
 * Creates the shared memory area.
 * The creator owns the memory when created.
 * @returns 0 on success. Error code on error.
 */
int shrmemCreate(void)
{
    int rc;
    rc = DosAllocSharedMem((PPVOID)&pShrMem,
                           SHARED_MEM_NAME,
                           SHARED_MEM_SIZE,
                           PAG_COMMIT | PAG_READ | PAG_WRITE);
    if (rc)
    {
        Error("Fatal error: Failed to create shared memory object. rc=%d\n", rc);
        return rc;
    }

    rc = DosCreateEventSem(NULL, &pShrMem->hevDaemon, DC_SEM_SHARED, FALSE);
    if (rc)
    {
        Error("Fatal error: Failed to create daemon event semaphore. rc=%d\n", rc);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosCreateEventSem(NULL, &pShrMem->hevClient, DC_SEM_SHARED, FALSE);
    if (rc)
    {
        Error("Fatal error: Failed to create client event semaphore. rc=%d\n", rc);
        DosCloseEventSem(pShrMem->hevDaemon);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosCreateMutexSem(NULL, &pShrMem->hmtx, DC_SEM_SHARED, TRUE);
    if (rc)
    {
        Error("Fatal error: Failed to create mutex semaphore. rc=%d\n", rc);
        DosCloseEventSem(pShrMem->hevClient);
        DosCloseEventSem(pShrMem->hevDaemon);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosCreateMutexSem(NULL, &pShrMem->hmtxClient, DC_SEM_SHARED, FALSE);
    if (rc)
    {
        Error("Fatal error: Failed to create client mutex semaphore. rc=%d\n", rc);
        DosCloseEventSem(pShrMem->hevClient);
        DosCloseEventSem(pShrMem->hevClient);
        DosCloseEventSem(pShrMem->hevDaemon);
        DosFreeMem(pShrMem);
        return rc;
    }


    /*
     * Install signal handlers.
     */
    signal(SIGSEGV, signalhandler);
    signal(SIGTERM, signalhandler);
    signal(SIGABRT, signalhandler);
    signal(SIGINT,  signalhandler);
    signal(SIGBREAK,signalhandler);

    return rc;
}


/**
 * Opens the shared memory and the semaphores.
 * The caller is owner of the memory upon successful return.
 * @returns 0 on success. Error code on error.
 */
int shrmemOpen(void)
{
    int     rc;
    ULONG   ulDummy;
    rc = DosGetNamedSharedMem((PPVOID)&pShrMem,
                              SHARED_MEM_NAME,
                              PAG_READ | PAG_WRITE);
    if (rc)
    {
        Error("Fatal error: Failed to open shared memory. rc=%d\n");
        return rc;
    }

    rc = DosOpenEventSem(NULL, &pShrMem->hevClient);
    if (rc)
    {
        Error("Fatal error: Failed to open client event semaphore. rc=%d\n");
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosOpenEventSem(NULL, &pShrMem->hevDaemon);
    if (rc)
    {
        Error("Fatal error: Failed to open daemon event semaphore. rc=%d\n");
        DosCloseEventSem(pShrMem->hevClient);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosOpenMutexSem(NULL, &pShrMem->hmtx);
    if (rc)
    {
        Error("Fatal error: Failed to open mutex semaphore. rc=%d\n");
        DosCloseEventSem(pShrMem->hevClient);
        DosCloseEventSem(pShrMem->hevDaemon);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosOpenMutexSem(NULL, &pShrMem->hmtxClient);
    if (rc)
    {
        Error("Fatal error: Failed to open client mutex semaphore. rc=%d\n");
        DosCloseEventSem(pShrMem->hevClient);
        DosCloseEventSem(pShrMem->hevDaemon);
        DosCloseMutexSem(pShrMem->hmtx);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosRequestMutexSem(pShrMem->hmtxClient, SEM_INDEFINITE_WAIT);
    if (rc)
    {
        Error("Fatal error: Failed to open aquire ownership of client mutex semaphore. rc=%d\n");
        shrmemFree();
        return rc;
    }

    rc = DosRequestMutexSem(pShrMem->hmtx, SEM_INDEFINITE_WAIT);
    if (rc)
    {
        Error("Fatal error: Failed to open aquire ownership of mutex semaphore. rc=%d\n");
        shrmemFree();
        return rc;
    }


    /*
     * Install signal handlers.
     */
    signal(SIGSEGV, signalhandler);
    signal(SIGTERM, signalhandler);
    signal(SIGABRT, signalhandler);
    signal(SIGINT,  signalhandler);
    signal(SIGBREAK,signalhandler);

    return rc;
}


/**
 * Frees the shared memory and the associated semaphores.
 */
void shrmemFree(void)
{
    if (!pShrMem)
        return;
    DosReleaseMutexSem(pShrMem->hmtxClient);
    DosReleaseMutexSem(pShrMem->hmtx);
    DosCloseMutexSem(pShrMem->hmtxClient);
    DosCloseMutexSem(pShrMem->hmtx);
    DosCloseEventSem(pShrMem->hevClient);
    DosCloseEventSem(pShrMem->hevDaemon);
    DosFreeMem(pShrMem);
    pShrMem = NULL;
}


/**
 * Daemon sends a message.
 * Upon we don't own the shared memory any longer.
 * @returns 0 on success. Error code on error.
 *          -1 on timeout.
 * @param   fWait   Wait for new message.
 */
int  shrmemSendDaemon(BOOL fWait)
{
    ULONG   ulDummy;
    int     rc;

    /* send message */
    DosResetEventSem(pShrMem->hevDaemon, &ulDummy);
    rc = DosReleaseMutexSem(pShrMem->hmtx);
    if (!rc)
        rc = DosPostEventSem(pShrMem->hevClient);

    /* wait for next message */
    if (!rc && fWait)
    {
        do
        {
            rc = DosWaitEventSem(pShrMem->hevDaemon, IDLE_TIMEOUT_MS);
        } while (rc == ERROR_TIMEOUT && pJobQueue);

        if (rc == ERROR_TIMEOUT)
        {
            DosRequestMutexSem(pShrMem->hmtx, SEM_INDEFINITE_WAIT);
            shrmemFree();
            return -1;
        }

        if (!rc)
            rc = DosRequestMutexSem(pShrMem->hmtx, SEM_INDEFINITE_WAIT);
        if (rc && rc != ERROR_INTERRUPT)
            Error("Internal error: failed to get next message from daemon, rc=%d\n", rc);
    }
    else
        Error("Internal error: failed to send message from daemon, rc=%d\n", rc);
    return rc;
}


/**
 * Client sends a message.
 * Upon we don't own the shared memory any longer.
 * @returns 0 on success. Error code on error.
 * @param   enmMsgTypeResponse  The expected response on this message.
 */
int  shrmemSendClient(int enmMsgTypeResponse)
{
    ULONG   ulDummy;
    int     rc;

    /* send message */
    DosResetEventSem(pShrMem->hevClient, &ulDummy);
    rc = DosReleaseMutexSem(pShrMem->hmtx);
    if (!rc)
        rc = DosPostEventSem(pShrMem->hevDaemon);

    /* wait for response */
    if (!rc)
    {
        rc = DosWaitEventSem(pShrMem->hevClient, SEM_INDEFINITE_WAIT);
        if (!rc)
        {
            rc = DosRequestMutexSem(pShrMem->hmtx, SEM_INDEFINITE_WAIT);
            if (!rc && pShrMem->enmMsgType != enmMsgTypeResponse)
            {
                Error("Internal error: Invalid response message. response=%d  expected=%d\n",
                      pShrMem->enmMsgType, enmMsgTypeResponse);
                return -1;
            }
        }
        if (rc && rc != ERROR_INTERRUPT)
            Error("Internal error: failed to get response message from daemon, rc=%d\n", rc);
    }
    else
        Error("Internal error: failed to send message to daemon, rc=%d\n", rc);

    return rc;
}


/**
 * printf lookalike used to print all run-tim errors.
 * @param   pszFormat   Format string.
 * @param   ...         Arguments (optional).
 */
void Error(const char *pszFormat, ...)
{
    va_list arg;

    va_start(arg, pszFormat);
    vfprintf(stdout, pszFormat, arg);
    va_end(arg);
}

