/* $Id: CmdQd.c,v 1.19 2003-09-30 13:29:07 bird Exp $
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
 *
 * This command daemon orginated as tool to exploit SMP and UNI systems better
 * when building large programs, but also when building one specific component of
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
 * with an mutex semaphore and two event sems to direct the conversation. The
 * shared memory block is allocated by the daemon and will have a quite simple
 * layout:
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
 *       - Show jobs:
 *              Nothing.
 *       - Show jobs reponse:
 *              More output indicator.
 *              Job listing (about 63KB)
 *
 *       - Show failed jobs:
 *              Nothing.
 *       - Show failed jobs reponse:
 *              More output indicator.
 *              Job listing (about 63KB)
 *
 *       - Show (successfully) completed jobs:
 *              Nothing.
 *       - Show completed jobs reponse:
 *              More output indicator.
 *              Job listing (about 63KB)
 *
 *       - Show running jobs:
 *              Nothing.
 *       - Show running jobs reponse:
 *              More output indicator.
 *              Job listing (about 63KB)
 *
 *       - Kill:
 *              Nothing.
 *       - Kill response:
 *              Success/failure indicator.
 *
 *       - Dies:
 *              Nothing. This is a message to the client saying that the
 *              daemon is dying or allready dead.
 *
 * The shared memory block is 64KB.
 *
 *
 * @subsection The Workers
 *
 * The workers is individual threads which waits for a job to be submitted to
 * execution. If the job only contains a single executable program to execute
 * (no & or &&) it will be executed using DosExecPgm. If it's a multi program
 * or command job it will be executed by CMD.EXE.
 *
 * Output will be read using unamed pipe and buffered. When the job is
 * completed we'll put the output into either the success queue or the failure
 * queue depending on the result.
 *
 * Note. Process startup needs to be serialized in order to be able to redirect
 * stdout. We're using a mutex for this.
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <assert.h>
#include <direct.h>
#include <signal.h>
#include <process.h>

#define INCL_BASE
#include <os2.h>


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#undef assert
//#define assert(expr) ((expr) ? (void)0, (void)0 \
//                             : Error("assert: %s\n  CmdQd.c line %d  function %s\n",\
//                                    #expr, __FUNCTION__, __LINE__), __interrupt(3))
#ifdef DEBUG
#define assert(expr)                                                           \
    do { if (!(expr)) { Error("assert: %s\n  CmdQd.c line %d  function %s\n",  \
                              #expr, __LINE__, __FUNCTION__);                  \
                       __interrupt(3); }                                       \
    } while (0)
#else
#define assert(expr) do { } while(0)
#endif

/*
 * Memory debugging.
 */
#ifdef DEBUGMEMORY
void my_free(void *);
void *my_malloc(size_t);

#undef free
#undef malloc

#define free(pv) my_free(pv)
#define malloc(cb) my_malloc(cb)

#endif

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
        msgKillResponse = 6,
        msgShowJobs = 7,
        msgShowJobsResponse = 8,
        msgShowRunningJobs = 9,
        msgShowRunningJobsResponse = 10,
        msgShowCompletedJobs = 11,
        msgShowCompletedJobsResponse = 12,
        msgShowFailedJobs = 13,
        msgShowFailedJobsResponse = 14,
        msgSharedMemOwnerDied = 0xfd,
        msgClientOwnerDied = 0xfe,
        msgDying = 0xff
    } enmMsgType;

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


        struct ShowJobs
        {
            int     iNothing;           /* Dummy. */
        } ShowJobs;
        struct ShowJobsResponse
        {
            BOOL    fMore;              /* More data. */
            char    szOutput[SHARED_MEM_SIZE- 4 - 4 - 4 - 4 - 4 - 4];
                                        /* The listing of jobs. */
        } ShowJobsResponse;


        struct ShowRunningJobs
        {
            int     iNothing;           /* Dummy. */
        } ShowRunningJobs;
        struct ShowRunningJobsResponse
        {
            BOOL    fMore;              /* More data. */
            char    szOutput[SHARED_MEM_SIZE- 4 - 4 - 4 - 4 - 4 - 4];
                                        /* The listing of jobs. */
        } ShowRunningJobsResponse;


        struct ShowCompletedJobs
        {
            int     iNothing;           /* Dummy. */
        } ShowCompletedJobs;
        struct ShowCompletedJobsResponse
        {
            BOOL    fMore;              /* More data. */
            char    szOutput[SHARED_MEM_SIZE- 4 - 4 - 4 - 4 - 4 - 4];
                                        /* The listing of jobs. */
        } ShowCompletedJobsResponse;


        struct ShowFailedJobs
        {
            int     iNothing;           /* Dummy. */
        } ShowFailedJobs;
        struct ShowFailedJobsResponse
        {
            BOOL    fMore;              /* More data. */
            char    szOutput[SHARED_MEM_SIZE- 4 - 4 - 4 - 4 - 4 - 4];
                                        /* The listing of jobs. */
        } ShowFailedJobsResponse;

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
    int             iJobId;             /* JobId. */
    int             rc;                 /* Result. */
    PJOBOUTPUT      pJobOutput;         /* Output. */
    struct Submit   JobInfo;            /* Job. */
} JOB, *PJOB;


typedef struct PathCache
{
    char    szPath[4096 - CCHMAXPATH * 3]; /* The path which this is valid for. */
    char    szCurDir[CCHMAXPATH];       /* The current dir this is valid for. */
    char    szProgram[CCHMAXPATH];      /* The program. */
    char    szResult[CCHMAXPATH];       /* The result. */
} PATHCACHE, *PPATHCACHE;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
PSHAREDMEM  pShrMem;                    /* Pointer to the shared memory.      */

HMTX        hmtxJobQueue;               /* Read/Write mutex for the two jobs queues below. */
HEV         hevJobQueue;                /* Incomming job event sem. */
PJOB        pJobQueue;                  /* Linked list of jobs. */
PJOB        pJobQueueEnd;               /* Last job entry. */
ULONG       cJobs;                      /* Count of jobs submitted. */
PJOB        pJobRunning;                /* Linked list of jobs. */
PJOB        pJobRunningEnd;             /* Last job entry. */

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

PSZ         pszSharedMem = SHARED_MEM_NAME; /* Default shared memname */
                                        /* Could be overridden by env.var. CMDQD_MEM_NAME. */
BOOL        fDaemon = FALSE;            /* Set if we're the daemon. */

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void syntax(void);

/* operations */
int  Init(const char *arg0, int cWorkers);
int  Daemon(int cWorkers);
int  DaemonInit(int cWorkers);
void signalhandlerDaemon(int sig);
void signalhandlerClient(int sig);
void Worker(void * iWorkerId);
char*WorkerArguments(char *pszArg, const char *pszzEnv, const char *pszCommand, char *pszCurDir, PPATHCACHE pPathCache);
char*fileNormalize(char *pszFilename, char *pszCurDir);
APIRET fileExist(const char *pszFilename);
int  Submit(int rcIgnore);
int  Wait(void);
int  QueryRunning(void);
int  Kill(void);
int  ShowJobs(void);
int  ShowRunningJobs(void);
int  ShowCompletedJobs(void);
int  ShowFailedJobs(void);
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
    char *  psz;
    char    szShrMemName[CCHMAXPATH];

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
     * Check for environment variable which gives us
     * the alternate shared mem name.
     */
    if ((psz = getenv("CMDQD_MEM_NAME")) != NULL)
    {
        if (strlen(psz) >= CCHMAXPATH - sizeof("\\SHAREMEM\\"))
           {
           printf("fatal error: CMDQD_MEM_NAME is is too long.\n");
           return -1;
           }
        strcpy(pszSharedMem = &szShrMemName[0], "\\SHAREMEM\\");
        strcat(pszSharedMem, psz);
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
    else if (!strcmp(argv[1], "queryrunning"))
    {
        return QueryRunning();
    }
    else if (!strcmp(argv[1], "kill"))
    {
        return Kill();
    }
    else if (!strcmp(argv[1], "showjobs"))
    {
        return ShowJobs();
    }
    else if (!strcmp(argv[1], "showrunningjobs"))
    {
        return ShowRunningJobs();
    }
    else if (!strcmp(argv[1], "showcompletedjobs"))
    {
        return ShowCompletedJobs();
    }
    else if (!strcmp(argv[1], "showfailedjobs"))
    {
        return ShowFailedJobs();
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

    //return 0;
}


/**
 * Display syntax.
 */
void syntax(void)
{
    printf(
        "Command Queue Daemon v0.0.2\n"
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
        "\n"
        "    wait\n"
        "        Wait for all commands which are queued up to complete.\n"
        "        rc = count of failing commands.\n"
        "\n"
        "    kill\n"
        "        Kills the daemon. Daemon will automatically die after\n"
        "        idling for some time.\n"
        "\n"
        "    queryrunning\n"
        "        Checks if the daemon is running.\n"
        "        rc = 0 if running; rc != 0 if not running.\n"
        "\n"
        "    showjobs           - shows jobs queued for execution.\n"
        "    showrunningjobs    - shows jobs currently running.\n"
        "    showcompletedjobs  - shows jobs succesfully executed.\n"
        "    showfailedjobs     - shows jobs which failed.\n"
        "\n"
        "   To use multiple daemons for different purposed assing different\n"
        "   values to CMDQD_MEM_NAME (env.var.) for the sessions.\n"
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

    DosSetFHState((HFILE)HF_STDIN, OPEN_FLAGS_NOINHERIT);
    DosSetFHState((HFILE)HF_STDOUT, OPEN_FLAGS_NOINHERIT);
    DosSetFHState((HFILE)HF_STDERR, OPEN_FLAGS_NOINHERIT);

    sprintf(&szArg[0], "%s\t!Daemon! %d", arg0, cWorkers);
    szArg[strlen(arg0)] = '\0';
    rc = DosExecPgm(NULL, 0, EXEC_BACKGROUND, &szArg[0], NULL, &Res, &szArg[0]);
    if (rc)
        Error("Fatal error: Failed to start daemon. rc=%d\n", rc);
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
    fDaemon = TRUE;

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
                pJob = malloc((int)&((PJOB)0)->JobInfo.szzEnv[pShrMem->u1.Submit.cchEnv]);
                if (pJob)
                {
                    memcpy(&pJob->JobInfo, &pShrMem->u1.Submit,
                           (int)&((struct Submit *)0)->szzEnv[pShrMem->u1.Submit.cchEnv]);
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
                    pJob->iJobId = cJobs++;
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
                    /*rc = DosWaitEventSem(hevJobQueueFine, SEM_INDEFINITE_WAIT); - there is some timing problem here,  */
                    rc = DosWaitEventSem(hevJobQueueFine, 1000); /* timeout after 1 second. */
                    if (rc && rc != ERROR_TIMEOUT)
                        break;
                    rc = NO_ERROR;      /* in case of TIMEOUT */

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

                /*
                 * Check if the wait client died.
                 */
                if (rc == ERROR_ALREADY_POSTED) /* seems like this is the rc we get. */
                {
                    /*
                     * BUGBUG: This code is really fishy, but I'm to tired to make a real fix now.
                     *         Hopefully this solves my current problem.
                     */
                    ULONG   ulDummy;
                    rc = DosRequestMutexSem(pShrMem->hmtx, 500);
                    rc = DosResetEventSem(pShrMem->hevClient, &ulDummy);
                    pShrMem->enmMsgType = msgUnknown;
                    rc = shrmemSendDaemon(TRUE);
                }
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


            case msgShowJobs:
            {
                /*
                 * Gain access to the job list.
                 */
                rc = DosRequestMutexSem(hmtxJobQueue, SEM_INDEFINITE_WAIT);
                if (!rc)
                {
                    int     iJob = 0;
                    PJOB    pJob = pJobQueue;

                    /*
                     * Big loop making and sending all messages.
                     */
                    do
                    {
                        int         cch;
                        char *      pszOutput;
                        int         cchOutput;

                        /*
                         * Make one message.
                         */
                        pShrMem->enmMsgType = msgShowJobsResponse;
                        pszOutput = &pShrMem->u1.ShowJobsResponse.szOutput[0];
                        cchOutput = sizeof(pShrMem->u1.ShowJobsResponse.szOutput) - 1;

                        /*
                         * Insert job info.
                         */
                        while (pJob)
                        {
                            char    szTmp[8192]; /* this is sufficient for one job. */

                            /*
                             * Format output in temporary buffer and check if
                             * it's space left in the share buffer.
                             */
                            cch = sprintf(szTmp,
                                          "------------------ JobId %d - %d\n"
                                          " command:  %s\n"
                                          " curdir:   %s\n"
                                          " rcIgnore: %d\n",
                                          pJob->iJobId,
                                          iJob,
                                          pJob->JobInfo.szCommand,
                                          pJob->JobInfo.szCurrentDir,
                                          pJob->JobInfo.rcIgnore);
                            if (cch > cchOutput)
                                break;

                            /*
                             * Copy from temporary to shared buffer.
                             */
                            memcpy(pszOutput, szTmp, cch);
                            pszOutput += cch;
                            cchOutput -= cch;

                            /*
                             * Next job.
                             */
                            pJob = pJob->pNext;
                            iJob++;
                        }

                        /*
                         * Send the message.
                         */
                        *pszOutput = '\0';
                        pShrMem->u1.ShowJobsResponse.fMore = pJob != NULL;
                        if (!pJob)
                           DosReleaseMutexSem(hmtxJobQueue);
                        rc = shrmemSendDaemon(TRUE);

                    } while (!rc && pJob);


                    /*
                     * Release the job list.
                     */
                    DosReleaseMutexSem(hmtxJobQueue);
                }
                else
                {
                    /* init response message */
                    pShrMem->enmMsgType = msgShowJobsResponse;
                    sprintf(&pShrMem->u1.ShowJobsResponse.szOutput[0],
                            "Internal Error. Requesting of hmtxJobQueue failed with rc=%d\n",
                            rc);
                    rc = shrmemSendDaemon(TRUE);
                }


                /*
                 * Check if the waiting client died.
                 */
                if (rc == ERROR_ALREADY_POSTED) /* seems like this is the rc we get. */
                {
                    /*
                     * BUGBUG: This code is really fishy, but I'm to tired to make a real fix now.
                     *         Hopefully this solves my current problem.
                     */
                    ULONG   ulDummy;
                    rc = DosRequestMutexSem(pShrMem->hmtx, 500);
                    rc = DosResetEventSem(pShrMem->hevClient, &ulDummy);
                    pShrMem->enmMsgType = msgUnknown;
                    rc = shrmemSendDaemon(TRUE);
                }
                break;
            }


            case msgShowFailedJobs:
            {
                /*
                 * Gain access to the finished job list.
                 */
                rc = DosRequestMutexSem(hmtxJobQueueFine, SEM_INDEFINITE_WAIT);
                if (!rc)
                {
                    int     iJob = 0;
                    PJOB    pJob = pJobFailed;

                    /*
                     * Big loop making and sending all messages.
                     */
                    do
                    {
                        int         cch;
                        char *      pszOutput;
                        int         cchOutput;

                        /*
                         * Make one message.
                         */
                        pShrMem->enmMsgType = msgShowFailedJobsResponse;
                        pszOutput = &pShrMem->u1.ShowFailedJobsResponse.szOutput[0];
                        cchOutput = sizeof(pShrMem->u1.ShowFailedJobsResponse.szOutput) - 1;

                        /*
                         * Insert job info.
                         */
                        while (pJob)
                        {
                            char    szTmp[8192]; /* this is sufficient for one job. */

                            /*
                             * Format output in temporary buffer and check if
                             * it's space left in the share buffer.
                             */
                            cch = sprintf(szTmp,
                                          "------------------ Failed JobId %d - %d\n"
                                          " command:  %s\n"
                                          " curdir:   %s\n"
                                          " rc:       %d  (rcIgnore=%d)\n",
                                          pJob->iJobId,
                                          iJob,
                                          pJob->JobInfo.szCommand,
                                          pJob->JobInfo.szCurrentDir,
                                          pJob->rc,
                                          pJob->JobInfo.rcIgnore);
                            if (cch > cchOutput)
                                break;

                            /*
                             * Copy from temporary to shared buffer.
                             */
                            memcpy(pszOutput, szTmp, cch);
                            pszOutput += cch;
                            cchOutput -= cch;

                            /*
                             * Next job.
                             */
                            pJob = pJob->pNext;
                            iJob++;
                        }

                        /*
                         * Send the message.
                         */
                        *pszOutput = '\0';
                        pShrMem->u1.ShowFailedJobsResponse.fMore = pJob != NULL;
                        if (!pJob)
                           DosReleaseMutexSem(hmtxJobQueueFine);
                        rc = shrmemSendDaemon(TRUE);

                    } while (!rc && pJob);


                    /*
                     * Release the job list.
                     */
                    DosReleaseMutexSem(hmtxJobQueueFine);
                }
                else
                {
                    /* init response message */
                    pShrMem->enmMsgType = msgShowFailedJobsResponse;
                    sprintf(&pShrMem->u1.ShowFailedJobsResponse.szOutput[0],
                            "Internal Error. Requesting of hmtxJobQueue failed with rc=%d\n",
                            rc);
                    rc = shrmemSendDaemon(TRUE);
                }


                /*
                 * Check if the waiting client died.
                 */
                if (rc == ERROR_ALREADY_POSTED) /* seems like this is the rc we get. */
                {
                    /*
                     * BUGBUG: This code is really fishy, but I'm to tired to make a real fix now.
                     *         Hopefully this solves my current problem.
                     */
                    ULONG   ulDummy;
                    rc = DosRequestMutexSem(pShrMem->hmtx, 500);
                    rc = DosResetEventSem(pShrMem->hevClient, &ulDummy);
                    pShrMem->enmMsgType = msgUnknown;
                    rc = shrmemSendDaemon(TRUE);
                }
                break;
            }


            case msgShowRunningJobs:
            {
                /*
                 * Gain access to the job list.
                 */
                rc = DosRequestMutexSem(hmtxJobQueue, SEM_INDEFINITE_WAIT);
                if (!rc)
                {
                    int     iJob = 0;
                    PJOB    pJob = pJobRunning;

                    /*
                     * Big loop making and sending all messages.
                     */
                    do
                    {
                        int         cch;
                        char *      pszOutput;
                        int         cchOutput;

                        /*
                         * Make one message.
                         */
                        pShrMem->enmMsgType = msgShowRunningJobsResponse;
                        pszOutput = &pShrMem->u1.ShowRunningJobsResponse.szOutput[0];
                        cchOutput = sizeof(pShrMem->u1.ShowRunningJobsResponse.szOutput) - 1;

                        /*
                         * Insert job info.
                         */
                        while (pJob)
                        {
                            char    szTmp[8192]; /* this is sufficient for one job. */

                            /*
                             * Format output in temporary buffer and check if
                             * it's space left in the share buffer.
                             */
                            cch = sprintf(szTmp,
                                          "------------------ Running JobId %d - %d\n"
                                          " command:  %s\n"
                                          " curdir:   %s\n"
                                          " rcIgnore: %d\n",
                                          pJob->iJobId,
                                          iJob,
                                          pJob->JobInfo.szCommand,
                                          pJob->JobInfo.szCurrentDir,
                                          pJob->JobInfo.rcIgnore);
                            if (cch > cchOutput)
                                break;

                            /*
                             * Copy from temporary to shared buffer.
                             */
                            memcpy(pszOutput, szTmp, cch);
                            pszOutput += cch;
                            cchOutput -= cch;

                            /*
                             * Next job.
                             */
                            pJob = pJob->pNext;
                            iJob++;
                        }

                        /*
                         * Send the message.
                         */
                        *pszOutput = '\0';
                        pShrMem->u1.ShowRunningJobsResponse.fMore = pJob != NULL;
                        if (!pJob)
                           DosReleaseMutexSem(hmtxJobQueue);
                        rc = shrmemSendDaemon(TRUE);

                    } while (!rc && pJob);


                    /*
                     * Release the job list.
                     */
                    DosReleaseMutexSem(hmtxJobQueue);
                }
                else
                {
                    /* init response message */
                    pShrMem->enmMsgType = msgShowRunningJobsResponse;
                    sprintf(&pShrMem->u1.ShowRunningJobsResponse.szOutput[0],
                            "Internal Error. Requesting of hmtxJobQueue failed with rc=%d\n",
                            rc);
                    rc = shrmemSendDaemon(TRUE);
                }


                /*
                 * Check if the waiting client died.
                 */
                if (rc == ERROR_ALREADY_POSTED) /* seems like this is the rc we get. */
                {
                    /*
                     * BUGBUG: This code is really fishy, but I'm to tired to make a real fix now.
                     *         Hopefully this solves my current problem.
                     */
                    ULONG   ulDummy;
                    rc = DosRequestMutexSem(pShrMem->hmtx, 500);
                    rc = DosResetEventSem(pShrMem->hevClient, &ulDummy);
                    pShrMem->enmMsgType = msgUnknown;
                    rc = shrmemSendDaemon(TRUE);
                }
                break;
            }



            case msgShowCompletedJobs:
            {
                /*
                 * Gain access to the finished job list.
                 */
                rc = DosRequestMutexSem(hmtxJobQueueFine, SEM_INDEFINITE_WAIT);
                if (!rc)
                {
                    int     iJob = 0;
                    PJOB    pJob = pJobCompleted;

                    /*
                     * Big loop making and sending all messages.
                     */
                    do
                    {
                        int         cch;
                        char *      pszOutput;
                        int         cchOutput;

                        /*
                         * Make one message.
                         */
                        pShrMem->enmMsgType = msgShowCompletedJobsResponse;
                        pszOutput = &pShrMem->u1.ShowCompletedJobsResponse.szOutput[0];
                        cchOutput = sizeof(pShrMem->u1.ShowCompletedJobsResponse.szOutput) - 1;

                        /*
                         * Insert job info.
                         */
                        while (pJob)
                        {
                            char    szTmp[8192]; /* this is sufficient for one job. */

                            /*
                             * Format output in temporary buffer and check if
                             * it's space left in the share buffer.
                             */
                            cch = sprintf(szTmp,
                                          "------------------ Completed JobId %d - %d\n"
                                          " command:  %s\n"
                                          " curdir:   %s\n"
                                          " rcIgnore: %d\n",
                                          pJob->iJobId,
                                          iJob,
                                          pJob->JobInfo.szCommand,
                                          pJob->JobInfo.szCurrentDir,
                                          pJob->JobInfo.rcIgnore);
                            if (cch > cchOutput)
                                break;

                            /*
                             * Copy from temporary to shared buffer.
                             */
                            memcpy(pszOutput, szTmp, cch);
                            pszOutput += cch;
                            cchOutput -= cch;

                            /*
                             * Next job.
                             */
                            pJob = pJob->pNext;
                            iJob++;
                        }

                        /*
                         * Send the message.
                         */
                        *pszOutput = '\0';
                        pShrMem->u1.ShowCompletedJobsResponse.fMore = pJob != NULL;
                        if (!pJob)
                           DosReleaseMutexSem(hmtxJobQueueFine);
                        rc = shrmemSendDaemon(TRUE);

                    } while (!rc && pJob);


                    /*
                     * Release the finished job list.
                     */
                    DosReleaseMutexSem(hmtxJobQueueFine);
                }
                else
                {
                    /* init response message */
                    pShrMem->enmMsgType = msgShowCompletedJobsResponse;
                    sprintf(&pShrMem->u1.ShowCompletedJobsResponse.szOutput[0],
                            "Internal Error. Requesting of hmtxJobQueue failed with rc=%d\n",
                            rc);
                    rc = shrmemSendDaemon(TRUE);
                }


                /*
                 * Check if the waiting client died.
                 */
                if (rc == ERROR_ALREADY_POSTED) /* seems like this is the rc we get. */
                {
                    /*
                     * BUGBUG: This code is really fishy, but I'm to tired to make a real fix now.
                     *         Hopefully this solves my current problem.
                     */
                    ULONG   ulDummy;
                    rc = DosRequestMutexSem(pShrMem->hmtx, 500);
                    rc = DosResetEventSem(pShrMem->hevClient, &ulDummy);
                    pShrMem->enmMsgType = msgUnknown;
                    rc = shrmemSendDaemon(TRUE);
                }
                break;
            }


            case msgClientOwnerDied:
            {
                DosCloseMutexSem(pShrMem->hmtxClient);
                rc = DosCreateMutexSem(NULL, &pShrMem->hmtxClient, DC_SEM_SHARED, FALSE);
                if (rc)
                    Error("Failed to restore dead client semaphore\n");
                pShrMem->enmMsgType = msgUnknown;
                rc = shrmemSendDaemon(TRUE);
                break;
            }


            case msgSharedMemOwnerDied:
            {
                DosCloseMutexSem(pShrMem->hmtx);
                rc = DosCreateMutexSem(NULL, &pShrMem->hmtx, DC_SEM_SHARED, TRUE);
                if (rc)
                    Error("Failed to restore dead shared mem semaphore\n");
                pShrMem->enmMsgType = msgUnknown;
                rc = shrmemSendDaemon(TRUE);
                break;
            }


            default:
                Error("Internal error: Invalid message id %d\n", pShrMem->enmMsgType, rc);
                pShrMem->enmMsgType = msgUnknown;
                rc = shrmemSendDaemon(TRUE);
        }
    }

    /*
     * Set dying msg type. shrmemFree posts the hevClient so clients
     * waiting for the daemon to respond will quit.
     */
    pShrMem->enmMsgType = msgDying;

    /*
     * Cleanup.
     */
    shrmemFree();
    DosCloseMutexSem(hmtxJobQueue);
    DosCloseMutexSem(hmtxJobQueueFine);
    DosCloseEventSem(hevJobQueueFine);
    DosCloseMutexSem(hmtxExec);
    DosCloseEventSem(hevJobQueue);

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
                             DosSetMaxFH(cWorkers * 6 + 20);
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
void signalhandlerDaemon(int sig)
{
    /*
     * Set dying msg type. shrmemFree posts the hevClient so clients
     * waiting for the daemon to respond will quit.
     */
    pShrMem->enmMsgType = msgDying;

    /*
     * Free and exit.
     */
    shrmemFree();
    exit(-42);
    sig = sig;
}


/**
 * Client signal handler.
 */
void signalhandlerClient(int sig)
{
    shrmemFree();
    exit(-42);
    sig = sig;
}



/**
 * Worker thread.
 * @param   iWorkerId   The worker process id.
 * @sketch
 */
void Worker(void * iWorkerId)
{
    PATHCACHE   PathCache;
    memset(&PathCache, 0, sizeof(PathCache));

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
            /* remove from input queue */
            if (pJob != pJobQueueEnd)
                pJobQueue = pJob->pNext;
            else
            {
                ULONG ulIgnore;
                pJobQueue = pJobQueueEnd = NULL;
                DosResetEventSem(hevJobQueue, &ulIgnore);
            }

            /* insert into running */
            pJob->pNext = NULL;
            if (pJobRunningEnd)
                pJobRunningEnd = pJobRunningEnd->pNext = pJob;
            else
                pJobRunning = pJobRunningEnd = pJob;
        }
        DosReleaseMutexSem(hmtxJobQueue);

        /*
         * Execute job.
         */
        if (pJob)
        {
            static HFILE hDummy;
            static BOOL fStdClosed = FALSE;
            int         rc;
            int         rcDbg;
            char        szArg[4096];
            char        szObj[256];
            PJOBOUTPUT  pJobOutput = NULL;
            PJOBOUTPUT  pJobOutputLast = NULL;
            RESULTCODES Res;
            PID         pid;
            HPIPE       hPipeR = -1;
            HPIPE       hPipeW = -1;
            HFILE       hStdErr;
            HFILE       hStdOut;

            //printf("debug-%d: start %s\n", iWorkerId, pJob->JobInfo.szCommand);

            /*
             * Redirect output and start process.
             */
            WorkerArguments(&szArg[0], &pJob->JobInfo.szzEnv[0], &pJob->JobInfo.szCommand[0],
                            &pJob->JobInfo.szCurrentDir[0], &PathCache);
            pJob->pJobOutput = pJobOutput = pJobOutputLast = malloc(sizeof(JOBOUTPUT));
            pJobOutput->pNext = NULL;
            pJobOutput->cchOutput = sprintf(pJobOutput->szOutput, "Job: %s\n", pJob->JobInfo.szCommand);

            if (DosRequestMutexSem(hmtxExec, SEM_INDEFINITE_WAIT))
            {
                Error("Internal Error: Failed to take exec mutex! rc=%d\n", rc);
                return;
            }

            if (!fStdClosed)
            {   /* only do this once! */
                HFILE   hf;
                ULONG   ul = 0;

                fclose(stdin);
                close(HF_STDIN);
                DosClose(HF_STDIN);
                fclose(stdout);
                close(HF_STDOUT);
                DosClose(HF_STDOUT);
                fclose(stderr);
                close(HF_STDERR);
                DosClose(HF_STDERR);

                hStdOut = HF_STDOUT;
                hStdErr = HF_STDERR;
                hDummy = -1;
                rcDbg = DosOpen("\\dev\\nul", &hDummy, &ul, 0, 0,
                                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                OPEN_SHARE_DENYNONE | OPEN_ACCESS_WRITEONLY, NULL);
                assert(!rcDbg);
                rcDbg = DosSetFHState(hDummy, OPEN_FLAGS_NOINHERIT);
                for (hf = HF_STDIN; hf <= HF_STDERR; hf++)
                    if (hDummy != hf)
                    {
                        rcDbg = DosDupHandle(hDummy, &hf);
                        assert(!rcDbg);
                        rcDbg = DosSetFHState(hf, OPEN_FLAGS_NOINHERIT);
                        assert(!rcDbg);
                    }
                fStdClosed = TRUE;
            }

            rc = DosCreatePipe(&hPipeR, &hPipeW, sizeof(pJobOutput->szOutput) - 1);
            if (rc)
            {
                DosReleaseMutexSem(hmtxExec);
                Error("Internal Error: Failed to create pipe! rc=%d\n", rc);
                return;
            }
            assert(hPipeW != HF_STDOUT && hPipeW != HF_STDERR);
            assert(hPipeR != HF_STDOUT && hPipeR != HF_STDERR);

            rc = DosSetDefaultDisk(  pJob->JobInfo.szCurrentDir[0] >= 'a'
                                   ? pJob->JobInfo.szCurrentDir[0] - 'a' + 1
                                   : pJob->JobInfo.szCurrentDir[0] - 'A' + 1);
            rc += DosSetCurrentDir(pJob->JobInfo.szCurrentDir);
            if (!rc)
            {
                hStdErr = HF_STDERR;
                hStdOut = HF_STDOUT;
                assert(   pJob->JobInfo.szzEnv[pJob->JobInfo.cchEnv-1] == '\0'
                       && pJob->JobInfo.szzEnv[pJob->JobInfo.cchEnv-2] == '\0');
                rcDbg = DosDupHandle(hPipeW, &hStdOut);                     assert(!rcDbg);
                rcDbg = DosDupHandle(hPipeW, &hStdErr);                     assert(!rcDbg);
                rcDbg = DosClose(hPipeW);                                   assert(!rcDbg);
                rcDbg = DosSetFHState(hPipeR, OPEN_FLAGS_NOINHERIT);        assert(!rcDbg);
                rc = DosExecPgm(szObj, sizeof(szObj), EXEC_ASYNCRESULT,
                                szArg, pJob->JobInfo.szzEnv, &Res, szArg);
                /* Placeholders, prevents clashes... */
                hStdOut = HF_STDOUT;
                rcDbg = DosDupHandle(hDummy, &hStdOut);                     assert(!rcDbg);
                hStdErr = HF_STDERR;
                rcDbg = DosDupHandle(hDummy, &hStdErr);                     assert(!rcDbg);
                DosReleaseMutexSem(hmtxExec);


                /*
                 * Read Output.
                 */
                if (!rc)
                {
                    ULONG       cchRead;
                    ULONG       cchRead2 = 0;

                    cchRead = sizeof(pJobOutput->szOutput) - pJobOutput->cchOutput - 1;
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
                                                 "Failed to set current directory to: %s (rc=%d)\n",
                                                 pJob->JobInfo.szCurrentDir, rc);
                pJob->rc = -1;
                DosClose(hPipeR);
                DosClose(hPipeW);
            }


            /*
             * Remove from the running queue.
             */
            if (DosRequestMutexSem(hmtxJobQueue, SEM_INDEFINITE_WAIT))
                return;

            if (pJobRunning != pJob)
            {
                PJOB pJobCur = pJobRunning;
                while (pJobCur)
                {
                    if (pJobCur->pNext == pJob)
                    {
                        pJobCur->pNext = pJob->pNext;
                        if (pJob == pJobRunningEnd)
                            pJobRunningEnd = pJobCur;
                        break;
                    }
                    pJobCur = pJobCur->pNext;
                }
            }
            else
                pJobRunning = pJobRunningEnd = NULL;

            DosReleaseMutexSem(hmtxJobQueue);


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
    iWorkerId = iWorkerId;
}


/**
 * Builds the input to DosExecPgm.
 * Will execute programs directly and command thru the shell.
 *
 * @returns pszArg.
 * @param   pszArg          Arguments to DosExecPgm.(output)
 *                          Assumes that the buffer is large enought.
 * @param   pszzEnv         Pointer to environment block.
 * @param   pszCommand      Command to execute.
 * @param   pszCurDir       From where the command is to executed.
 * @param   pPathCache      Used to cache the last path, executable, and the search result.
 */
char *WorkerArguments(char *pszArg, const char *pszzEnv, const char *pszCommand, char *pszCurDir, PPATHCACHE pPathCache)
{
    BOOL        fCMD = FALSE;
    const char *psz;
    const char *psz2;
    char *      pszW;
    char        ch;
    int         cch;
    APIRET      rc;

    /*
     * Check if this is multiple command separated by either &, && or |.
     * Currently ignoring quotes for this test.
     */
    if (    strchr(pszCommand, '&')
        ||  strchr(pszCommand, '|')
        ||  strchr(pszCommand, '@'))
    {
        strcpy(pszArg, "cmd.exe");      /* doesn't use comspec, just defaults to cmd.exe in all cases. */
        fCMD = TRUE;
        psz2 = pszCommand;              /* start of arguments. */
    }
    else
    {
        char chEnd = ' ';

        /*
         * Parse out the first name.
         */
        for (psz = pszCommand; *psz == '\t' || *psz == ' ';) //strip(,'L');
            psz++;
        if (*psz == '"' || *psz == '\'')
            chEnd = *psz++;
        psz2 = psz;
        if (chEnd == ' ')
        {
            while ((ch = *psz) != '\0' && ch != ' ' && ch != '\t')
                psz++;
        }
        else
        {
            while ((ch = *psz) != '\0' && ch != chEnd)
                psz++;
        }
        *pszArg = '\0';
        strncat(pszArg, psz2, psz - psz2);
        psz2 = psz+1;                   /* start of arguments. */
    }


    /*
     * Resolve the executable name if not qualified.
     * NB! We doesn't fully support references to other driveletters yet. (TODO/BUGBUG)
     */
    /* correct slashes */
    pszW = pszArg;
    while ((pszW = strchr(pszW, '//')) != NULL)
        *pszW++ = '\\';

    /* make sure it ends with .exe */
    pszW = pszArg + strlen(pszArg) - 1;
    while (pszW > pszArg && *pszW != '.' && *pszW != '\\')
        pszW--;
    if (*pszW != '.')
        strcat(pszArg, ".exe");

    if (pszArg[1] != ':' || *pszArg == *pszCurDir)
    {
        rc = -1;                        /* indicate that we've not found the file. */

        /* relative path? - expand it */
        if (strchr(pszArg, '\\') || pszArg[1] == ':')
        {   /* relative path - expand it and check for file existence */
            fileNormalize(pszArg, pszCurDir);
            pszCurDir[strlen(pszCurDir)-1] = '\0'; /* remove slash */
            rc = fileExist(pszArg);
        }
        else
        {   /* Search path. */
            const char *pszPath = pszzEnv;
            while (*pszPath != '\0' && strncmp(pszPath, "PATH=", 5))
                pszPath += strlen(pszPath) + 1;

            if (pszPath && *pszPath != '\0')
            {
                /* check cache */
                if (   !strcmp(pPathCache->szProgram, pszArg)
                    && !strcmp(pPathCache->szPath, pszPath)
                    && !strcmp(pPathCache->szCurDir, pszCurDir)
                       )
                {
                    strcpy(pszArg, pPathCache->szResult);
                    rc = fileExist(pszArg);
                }

                if (rc)
                {   /* search path */
                    char    szResult[CCHMAXPATH];
                    rc = DosSearchPath(SEARCH_IGNORENETERRS, (PSZ)pszPath, pszArg, &szResult[0] , sizeof(szResult));
                    if (!rc)
                    {
                        strcpy(pszArg, szResult);

                        /* update cache */
                        strcpy(pPathCache->szProgram, pszArg);
                        strcpy(pPathCache->szPath, pszPath);
                        strcpy(pPathCache->szCurDir, pszCurDir);
                        strcpy(pPathCache->szResult, szResult);
                    }
                }
            }
        }
    }
    /* else nothing to do - assume full path (btw. we don't have the current dir for other drives anyway :-) */
    else
        rc = !fCMD ? fileExist(pszArg) : NO_ERROR;

    /* In case of error use CMD */
    if (rc && !fCMD)
    {
        strcpy(pszArg, "cmd.exe");      /* doesn't use comspec, just defaults to cmd.exe in all cases. */
        fCMD = TRUE;
        psz2 = pszCommand;              /* start of arguments. */
    }


    /*
     * Complete the argument string.
     * ---
     * szArg current holds the command.
     * psz2 points to the first parameter. (needs strip(,'L'))
     */
    while ((ch = *psz2) != '\0' && (ch == '\t' || ch == ' '))
        psz2++;

    pszW = pszArg + strlen(pszArg) + 1;
    cch = strlen(psz2);
    if (!fCMD)
    {
        memcpy(pszW, psz2, ++cch);
        pszW[cch] = '\0';
    }
    else
    {
        strcpy(pszW, "/C \"");
        pszW += strlen(pszW);
        memcpy(pszW, psz2, cch);
        memcpy(pszW + cch, "\"\0", 3);
    }

    return pszArg;
}



/**
 * Normalizes the path slashes for the filename. It will partially expand paths too.
 * @returns pszFilename
 * @param   pszFilename  Pointer to filename string. Not empty string!
 *                       Much space to play with.
 * @remark  (From fastdep.)
 * @remark  BOGUS CODE! Recheck it please!
 */
char *fileNormalize(char *pszFilename, char *pszCurDir)
{
    char *  pszRet = pszFilename;
    int     aiSlashes[CCHMAXPATH/2];
    int     cSlashes;
    int     i;

    /*
     * Init stuff.
     */
    for (i = 1, cSlashes = 0; pszCurDir[i] != '\0'; i++)
    {
        if (pszCurDir[i] == '/')
            pszCurDir[i] = '\\';
        if (pszCurDir[i] == '\\')
            aiSlashes[cSlashes++] = i;
    }
    if (pszCurDir[i-1] != '\\')
    {
        aiSlashes[cSlashes] = i;
        pszCurDir[i++] = '\\';
        pszCurDir[i] = '\0';
    }


    /* expand path? */
    if (pszFilename[1] != ':')
    {   /* relative path */
        int     iSlash;
        char    szFile[CCHMAXPATH];
        char *  psz = szFile;

        strcpy(szFile, pszFilename);
        iSlash = *psz == '\\' ? 1 : cSlashes;
        while (*psz != '\0')
        {
            if (*psz == '.' && psz[1] == '.'  && psz[2] == '\\')
            {   /* up one directory */
                if (iSlash > 0)
                    iSlash--;
                psz += 3;
            }
            else if (*psz == '.' && psz[1] == '\\')
            {   /* no change */
                psz += 2;
            }
            else
            {   /* completed expantion! */
                strncpy(pszFilename, pszCurDir, aiSlashes[iSlash]+1);
                strcpy(pszFilename + aiSlashes[iSlash]+1, psz);
                break;
            }
        }
    }
    /* else: assume full path */

    return pszRet;
}

/**
 * Checks if a given file exist.
 * @returns 0 if the file exists.   (NO_ERROR)
 *          2 if the file doesn't exist. (ERROR_FILE_NOT_FOUND)
 * @param   pszFilename     Name of the file to check existance for.
 */
APIRET fileExist(const char *pszFilename)
{
    FILESTATUS3     fsts3;
    return DosQueryPathInfo((PSZ)pszFilename, FIL_STANDARD, &fsts3, sizeof(fsts3));
}


/**
 * Submits a command to the daemon.
 * @returns 0 on success.
 *          -3 on failure.
 * @param   rcIgnore   Ignores returcodes ranging from 0 to rcIgnore.
 */
int Submit(int rcIgnore)
{
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
        Error("Fatal error: Command too long.\n", rc);
        shrmemFree();
        return -1;
    }
    if (*psz == '"' && psz[cch-2] == '"')    /* remove start & end quotes if any */
    {
        cch--;
        psz++;
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
 * Checks if the daemon is running.
 */
int QueryRunning(void)
{
    APIRET rc;
    rc = DosGetNamedSharedMem((PPVOID)(PVOID)&pShrMem,
                              pszSharedMem,
                              PAG_READ | PAG_WRITE);
    if (!rc)
        DosFreeMem(pShrMem);

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
 * Shows the current queued commands.
 * Will write to stdout.
 * @returns 0 or -1 usually.
 */
int  ShowJobs(void)
{
    int     rc;

    rc = shrmemOpen();
    if (rc)
        return rc;
    do
    {
        pShrMem->enmMsgType = msgShowJobs;
        pShrMem->u1.ShowJobs.iNothing = 0;
        rc = shrmemSendClient(msgShowJobsResponse);
        if (rc)
        {
            shrmemFree();
            return -1;
        }
        printf("%s", pShrMem->u1.ShowJobsResponse.szOutput);
        /*
         * Release the client mutex if more data and yield the CPU.
         * So we can submit more work. (Odin nmake lib...)
         */
        if (pShrMem->u1.ShowJobsResponse.fMore)
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
    } while (pShrMem->u1.ShowJobsResponse.fMore);

    shrmemFree();
    return rc;
}


/**
 * Shows the current running jobs (not the output).
 * Will write to stdout.
 * @returns 0 or -1 usually.
 */
int  ShowRunningJobs(void)
{
    int     rc;

    rc = shrmemOpen();
    if (rc)
        return rc;
    do
    {
        pShrMem->enmMsgType = msgShowRunningJobs;
        pShrMem->u1.ShowRunningJobs.iNothing = 0;
        rc = shrmemSendClient(msgShowRunningJobsResponse);
        if (rc)
        {
            shrmemFree();
            return -1;
        }
        printf("%s", pShrMem->u1.ShowRunningJobsResponse.szOutput);
        /*
         * Release the client mutex if more data and yield the CPU.
         * So we can submit more work. (Odin nmake lib...)
         */
        if (pShrMem->u1.ShowRunningJobsResponse.fMore)
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
    } while (pShrMem->u1.ShowRunningJobsResponse.fMore);

    shrmemFree();
    return rc;
}


/**
 * Shows the current queue of successfully completed jobs (not the output).
 * Will write to stdout.
 * @returns 0 or -1 usually.
 */
int  ShowCompletedJobs(void)
{
    int     rc;

    rc = shrmemOpen();
    if (rc)
        return rc;
    do
    {
        pShrMem->enmMsgType = msgShowCompletedJobs;
        pShrMem->u1.ShowCompletedJobs.iNothing = 0;
        rc = shrmemSendClient(msgShowCompletedJobsResponse);
        if (rc)
        {
            shrmemFree();
            return -1;
        }
        printf("%s", pShrMem->u1.ShowCompletedJobsResponse.szOutput);
        /*
         * Release the client mutex if more data and yield the CPU.
         * So we can submit more work. (Odin nmake lib...)
         */
        if (pShrMem->u1.ShowCompletedJobsResponse.fMore)
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
    } while (pShrMem->u1.ShowCompletedJobsResponse.fMore);

    shrmemFree();
    return rc;
}


/**
 * Shows the current queue of failed jobs (not the output).
 * Will write to stdout.
 * @returns 0 or -1 usually.
 */
int  ShowFailedJobs(void)
{
    int     rc;

    rc = shrmemOpen();
    if (rc)
        return rc;
    do
    {
        pShrMem->enmMsgType = msgShowFailedJobs;
        pShrMem->u1.ShowFailedJobs.iNothing = 0;
        rc = shrmemSendClient(msgShowFailedJobsResponse);
        if (rc)
        {
            shrmemFree();
            return -1;
        }
        printf("%s", pShrMem->u1.ShowFailedJobsResponse.szOutput);
        /*
         * Release the client mutex if more data and yield the CPU.
         * So we can submit more work. (Odin nmake lib...)
         */
        if (pShrMem->u1.ShowFailedJobsResponse.fMore)
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
    } while (pShrMem->u1.ShowFailedJobsResponse.fMore);

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
    rc = DosAllocSharedMem((PPVOID)(PVOID)&pShrMem,
                           pszSharedMem,
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
    signal(SIGSEGV, signalhandlerDaemon);
    signal(SIGTERM, signalhandlerDaemon);
    signal(SIGABRT, signalhandlerDaemon);
    signal(SIGINT,  signalhandlerDaemon);
    signal(SIGBREAK,signalhandlerDaemon);

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
    ULONG   ulIgnore;

    /*
     * Get memory.
     */
    rc = DosGetNamedSharedMem((PPVOID)(PVOID)&pShrMem,
                              pszSharedMem,
                              PAG_READ | PAG_WRITE);
    if (rc)
    {
        Error("Fatal error: Failed to open shared memory. rc=%d\n", rc);
        return rc;
    }


    /*
     * Open semaphores.
     */
    rc = DosOpenEventSem(NULL, &pShrMem->hevClient);
    if (rc)
    {
        Error("Fatal error: Failed to open client event semaphore. rc=%d\n", rc);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosOpenEventSem(NULL, &pShrMem->hevDaemon);
    if (rc)
    {
        Error("Fatal error: Failed to open daemon event semaphore. rc=%d\n", rc);
        DosCloseEventSem(pShrMem->hevClient);
        DosFreeMem(pShrMem);
        return rc;
    }

    rc = DosOpenMutexSem(NULL, &pShrMem->hmtx);
    if (rc)
    {
        /* try correct client died situation */
        if (rc == ERROR_SEM_OWNER_DIED)
        {
            pShrMem->enmMsgType = msgSharedMemOwnerDied;
            DosResetEventSem(pShrMem->hevClient, &ulIgnore);
            DosPostEventSem(pShrMem->hevDaemon);
            if (DosWaitEventSem(pShrMem->hevClient, 2000))
            {
                Error("Fatal error: Failed to open mutex semaphore. (owner dead) rc=%d\n", rc);
                shrmemFree();
                return rc;
            }
            rc = DosOpenMutexSem(NULL, &pShrMem->hmtx);
        }

        if (rc)
        {
            Error("Fatal error: Failed to open mutex semaphore. rc=%d\n", rc);
            DosCloseEventSem(pShrMem->hevClient);
            DosCloseEventSem(pShrMem->hevDaemon);
            DosFreeMem(pShrMem);
            return rc;
        }
    }

    rc = DosOpenMutexSem(NULL, &pShrMem->hmtxClient);
    if (rc)
    {
        /* try correct client died situation */
        if (rc == ERROR_SEM_OWNER_DIED)
        {
            pShrMem->enmMsgType = msgClientOwnerDied;
            DosResetEventSem(pShrMem->hevClient, &ulIgnore);
            DosPostEventSem(pShrMem->hevDaemon);
            if (DosWaitEventSem(pShrMem->hevClient, 2000))
            {
                Error("Fatal error: Failed to open client mutex semaphore. (owner dead) rc=%d\n", rc);
                shrmemFree();
                return rc;
            }
            rc = DosOpenMutexSem(NULL, &pShrMem->hmtxClient);
        }

        if (rc)
        {
            Error("Fatal error: Failed to open client mutex semaphore. rc=%d\n", rc);
            DosCloseEventSem(pShrMem->hevClient);
            DosCloseEventSem(pShrMem->hevDaemon);
            DosCloseMutexSem(pShrMem->hmtx);
            DosFreeMem(pShrMem);
            return rc;
        }
    }


    /*
     * Before we request semaphores we need to have signal handlers installed.
     */
    signal(SIGSEGV, signalhandlerClient);
    signal(SIGTERM, signalhandlerClient);
    signal(SIGABRT, signalhandlerClient);
    signal(SIGINT,  signalhandlerClient);
    signal(SIGBREAK,signalhandlerClient);


    /*
     * Request the necessary semaphores to be able to talk to the daemon.
     */
    rc = DosRequestMutexSem(pShrMem->hmtxClient, SEM_INDEFINITE_WAIT);
    if (rc)
    {
        /* try correct client died situation */
        if (rc == ERROR_SEM_OWNER_DIED)
        {
            pShrMem->enmMsgType = msgClientOwnerDied;
            DosResetEventSem(pShrMem->hevClient, &ulIgnore);
            DosPostEventSem(pShrMem->hevDaemon);
            if (DosWaitEventSem(pShrMem->hevClient, 2000))
            {
                Error("Fatal error: Failed to take ownership of client mutex semaphore. (owner dead) rc=%d\n", rc);
                shrmemFree();
                return rc;
            }
            rc = DosRequestMutexSem(pShrMem->hmtxClient, SEM_INDEFINITE_WAIT);
        }

        if (rc)
        {
            Error("Fatal error: Failed to take ownership of client mutex semaphore. rc=%d\n", rc);
            shrmemFree();
            return rc;
        }
    }

    rc = DosRequestMutexSem(pShrMem->hmtx, SEM_INDEFINITE_WAIT);
    if (rc)
    {
        /* try correct client died situation */
        if (rc == ERROR_SEM_OWNER_DIED)
        {
            pShrMem->enmMsgType = msgSharedMemOwnerDied;
            DosResetEventSem(pShrMem->hevClient, &ulIgnore);
            DosPostEventSem(pShrMem->hevDaemon);
            if (DosWaitEventSem(pShrMem->hevClient, 2000))
            {
                Error("Fatal error: Failed to take ownership of mutex mutex semaphore. (owner dead) rc=%d\n", rc);
                shrmemFree();
                return rc;
            }
            rc = DosRequestMutexSem(pShrMem->hmtx, SEM_INDEFINITE_WAIT);
        }

        if (rc)
        {
            Error("Fatal error: Failed to take ownership of mutex semaphore. rc=%d\n", rc);
            shrmemFree();
            return rc;
        }
    }


    return rc;
}


/**
 * Frees the shared memory and the associated semaphores.
 */
void shrmemFree(void)
{
    if (!pShrMem)
        return;
    /* wakeup any clients */
    DosPostEventSem(pShrMem->hevClient);
    /* free stuff */
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
        {
            rc = DosRequestMutexSem(pShrMem->hmtx, SEM_INDEFINITE_WAIT);
            if (rc == ERROR_SEM_OWNER_DIED)
            {
                DosCloseMutexSem(pShrMem->hmtx);
                pShrMem->hmtx = NULLHANDLE;
                rc = DosCreateMutexSem(NULL, &pShrMem->hmtx, DC_SEM_SHARED, TRUE);
            }
        }

        if (rc && rc != ERROR_INTERRUPT)
            Error("Internal error: failed to get next message from daemon, rc=%d\n", rc);
    }
    else if (!fWait)
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
            if (rc == ERROR_SEM_OWNER_DIED)
            {
                Error("Internal error: shared mem mutex owner died.\n");
                return -1;
            }

            if (!rc && pShrMem->enmMsgType != enmMsgTypeResponse)
            {
                if (pShrMem->enmMsgType != msgDying)
                    Error("Internal error: Invalid response message. response=%d  expected=%d\n",
                          pShrMem->enmMsgType, enmMsgTypeResponse);
                else
                    Error("Fatal error: daemon just died!\n");
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
    /* won't workin in daemon mode... */
    va_start(arg, pszFormat);
    if (!fDaemon)
        vfprintf(stdout, pszFormat, arg);
    else
    {
        FILE *phFile = fopen(".\\cmdqd.log", "a+");
        if (phFile)
        {
            fprintf(phFile, "%d:", getpid());
            vfprintf(phFile, pszFormat, arg);
            fclose(phFile);
        }
    }
    va_end(arg);
}


#ifdef DEBUGMEMORY
void my_free(void *pv)
{
    DosFreeMem((PVOID)((unsigned)pv & 0xffff0000));
}

void *my_malloc(size_t cb)
{
    APIRET  rc;
    PVOID   pv;
    ULONG   cbAlloc;
    char    szMsg[200];

    cbAlloc = (cb + 0x1fff) & (~0x0fff);

    rc = DosAllocMem(&pv, cbAlloc, PAG_READ | PAG_WRITE);
    if (!rc)
    {
        rc = DosSetMem(pv, cbAlloc - 0x1000, PAG_READ | PAG_WRITE | PAG_COMMIT);
        if (rc)
            __interrupt(3);
        if (cb & 0xfff)
            pv = (PVOID)((unsigned)pv + 0x1000 - (cb & 0x0fff));
    }

    strcpy(szMsg, "malloc(");
    _itoa(cb, szMsg + strlen(szMsg), 16);
    strcat(szMsg, ") -> ");
    _itoa(pv, szMsg + strlen(szMsg), 16);
    strcat(szMsg, "\r\n");

    DosPutMessage(1, strlen(szMsg), szMsg);

    return rc ? NULL : pv;
}
#endif
