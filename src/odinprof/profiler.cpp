/* $Id: profiler.cpp,v 1.4 2001-12-03 13:13:06 phaller Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Execution Trace Profiler
 *
 * Copyright 2001 Patrick Haller <patrick.haller@innotek.de>
 *
 */


/****************************************************************************
 * To Do
 ****************************************************************************
 
 - write result in java.prof style
 - find tool to nicely work on the output files
 - fix wrong timestamps (use 64-bit arithmetic and recalculate to ms)
 */


/****************************************************************************
 * includes
 ****************************************************************************/

#define INCL_DOSERRORS
#define INCL_DOSPROCESS
#include <os2.h>

#include <os2sel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <process.h>

#include "symfile.h"


/****************************************************************************
 * local class definitions
 ****************************************************************************/


typedef struct
{
    ULONG ulID;
    void* pObject;
} HASHTABLEENTRYID, *PHASHTABLEENTRYID;


class CProfilerHashtableLookup : public CHashtableLookup
{
  public:
        CProfilerHashtableLookup(int iInitialSize);
        PHASHTABLEENTRYID addElement(ULONG ulID, void *pObject);
        void* removeElement(ULONG ulID);
        void* getElement(ULONG ulID);
        int   getElementMap(PHASHTABLEENTRYID pBuffer);

  protected:
        void  setSize0(int iNewSize);
};


/****************************************************************************
 * definitions
 ****************************************************************************/

typedef struct tagThreadProfileEntry
{
  unsigned long      EIP;          /* currently only 32-bit segments are traced */
  char*              pszModule;                   /* name of the current module */
                        /* name of the current function or (caller;callee pair) */
  char*              pszFunction;
  unsigned long      ulCalls;
  unsigned long      ulTimeMinimum;
  unsigned long      ulTimeMaximum;
  unsigned long      ulTimeTotal;
} PROFILEENTRY, *PPROFILEENTRY;


#define NUM_RECORDS 256
typedef struct tagThreadProfilerDataBlock
{
  unsigned long ulCalldepth;         // current call depth
  unsigned long ret[NUM_RECORDS];            // last saved return address
  unsigned long callStack[NUM_RECORDS];      // call stack (CS:) EIP records
  unsigned long enterTimeHi[NUM_RECORDS];    // machine timestamps of function entry
  unsigned long enterTimeLo[NUM_RECORDS];
  unsigned long overheadTimeHi[NUM_RECORDS]; // machine timestamps of function entry
  unsigned long overheadTimeLo[NUM_RECORDS];
  
  // Hashtable to record the PROFILEENTRY records
  CProfilerHashtableLookup* pHashtable;
  
} PROFILERBLOCK, *PPROFILERBLOCK;


/****************************************************************************
 * module local variables
 ****************************************************************************/

static SymbolFilePool*           pSymPool    = new SymbolFilePool();
static CProfilerHashtableLookup* pProfileMap = new CProfilerHashtableLookup(1021);
static PPROFILERBLOCK* ppTLS              = 0;      /* thread local storage */
static BOOL            fIsProfilerEnabled = FALSE;  /* enable / disable hook*/
static CHAR            szWorkingDirectory[260] = ".";

extern void _System odin_ProfileHook32Bottom();
extern void _System _ProfileGetTimestamp(PULONG hi, PULONG lo);


/****************************************************************************
 * local class definitions
 ****************************************************************************/

static inline unsigned long _Optlink hashcodeEIP(int iRing, ULONG eip)
{
  return eip % iRing;
}


CProfilerHashtableLookup::CProfilerHashtableLookup(int iInitialSize)
  : CHashtableLookup(iInitialSize)
{
}


PHASHTABLEENTRYID CProfilerHashtableLookup::addElement(ULONG ulID, void *pObject)
{
    // get slot number
    unsigned long ulHash = hashcodeEIP(iSize, ulID);

    // create entry
    PHASHTABLEENTRYID pHTE = (PHASHTABLEENTRYID)malloc(sizeof(HASHTABLEENTRYID));
    pHTE->ulID    = ulID;
    pHTE->pObject = pObject;

    // check if slot has a list object already
    if (parrLists[ulHash] == NULL)
        parrLists[ulHash] = new CLinearList();

    parrLists[ulHash]->addLast(pHTE);

    // count allocated elements
    iElements++;

    return pHTE;
}


void* CProfilerHashtableLookup::removeElement(ULONG ulID)
{
    // get slot number
    unsigned long ulHash = hashcodeEIP(iSize, ulID);

    // check if slot is occupied
    if (parrLists[ulHash] == NULL)
        // signal not found
        return NULL;

    // search the list
    PLINEARLISTENTRY pLLE = parrLists[ulHash]->getFirst();
    if (pLLE == NULL)
        // signal not found
        return NULL;

    // iterate over the list
    while(pLLE)
    {
        PHASHTABLEENTRYID pHTE = (PHASHTABLEENTRYID)pLLE->pObject;

        if (pHTE->ulID == ulID)
        {
            // save old object pointer
            void* pTemp = pHTE->pObject;
            free(pHTE);

            // found the correct entry
            parrLists[ulHash]->removeElement(pLLE);

            // count allocated elements
            iElements--;

            // return old object pointer to signal success
            return pTemp;
        }

        pLLE = pLLE->pNext;
    }

    // failed
    return NULL;
}


void* CProfilerHashtableLookup::getElement(ULONG ulID)
{
    // get slot number
    unsigned long ulHash = hashcodeEIP(iSize, ulID);

    CLinearList *pLL = parrLists[ulHash];

    // check if slot is occupied
    if (pLL == NULL)
        // signal not found
        return NULL;

    // search the list
    PLINEARLISTENTRY pLLE = pLL->getFirst();
    if (pLLE == NULL)
        // signal not found
        return NULL;

    // iterate over the list
    while(pLLE)
    {
        PHASHTABLEENTRYID pHTE = (PHASHTABLEENTRYID)pLLE->pObject;

        // quickly compare 1st character for equality
        // before doing the strcmp call
        if (pHTE->ulID == ulID)
        {
            // return result
            return pHTE->pObject;
        }

        pLLE = pLLE->pNext;
    }

    // failed
    return NULL;
}


int CProfilerHashtableLookup::getElementMap(PHASHTABLEENTRYID pBuffer)
{
  int iIndex = 0;
  
  // iterate over all registered entries and dump them to the buffer
  // giving the caller direct access to the hashtable internals.
  for (int i = 0;
       i < iSize;
       i++)
  {
    // check if slot was occupied
    if (parrLists[i] != NULL)
    {
      // walk along any entry in that linear list
      PLINEARLISTENTRY pLLE = parrLists[i]->getFirst();
      
      while (pLLE)
      {
        PHASHTABLEENTRYID pHTE = (PHASHTABLEENTRYID)pLLE->pObject;
        memcpy(&pBuffer[iIndex], pHTE, sizeof( HASHTABLEENTRYID ) );
        iIndex++;
        
        pLLE = parrLists[i]->getNext(pLLE);
      }
    }
  }
  
  // return number of elements copied
  return iIndex;
}


void  CProfilerHashtableLookup::setSize0(int iNewSize)
{
    // check if rehashing is necessary at all
    if (iSize == iNewSize)
        return;

    // save old array, allocate new array
    CLinearList** parrNew = new CLinearList* [iNewSize];
    memset(parrNew,
           0,
           sizeof(CLinearList*) * iNewSize);

    // convert all the slot lists
    for (int i = 0;
         i < iSize;
         i++)
    {
        // check if slot was occupied
        if (parrLists[i] != NULL)
        {
            // iterate over the slot
            PLINEARLISTENTRY pLLE = parrLists[i]->getFirst();
            PHASHTABLEENTRYID pHTE;
            unsigned long ulHash;

            while(pLLE)
            {
                // calculate new hashcode for the entry
                pHTE = (PHASHTABLEENTRYID)pLLE->pObject;
                ulHash = hashcodeEIP(iNewSize, pHTE->ulID);

                // reinsert the pHTE into new slot
                if (parrNew[ulHash] == NULL)
                    parrNew[ulHash] = new CLinearList();

                parrNew[ulHash]->addLast( (void*)pHTE );

                pLLE=pLLE->pNext;
            }

            // kill the slot
            delete parrLists[i];
            parrLists[i] = NULL;
        }
    }

    // swap the tables
    iSize = iNewSize;
    delete [] parrLists;

    parrLists = parrNew;
}


/****************************************************************************
 * implementation
 ****************************************************************************/
 
static PPROFILERBLOCK inline getProfilerBlock(void)
{
  if ( ( (ULONG)ppTLS != 0) && ( (ULONG)ppTLS != 0xffffffff) )
    return *ppTLS;
  
  // is TLS already available?
  if (ppTLS == NULL)
  {
    // allocate thread local storage
    USHORT sel = RestoreOS2FS();
    APIRET rc = DosAllocThreadLocalMemory(1, (PULONG*)&ppTLS);
    SetFS(sel);
    
    if (rc != NO_ERROR)
      ppTLS = (PPROFILERBLOCK*)0xffffffff; // mark as invalid
  }
  
  if ((ULONG)ppTLS == 0xffffffff)
    // profiling is not available
    return NULL;

  // allocate data structure
  {
    USHORT sel = RestoreOS2FS();
    
    // Note: must be DosAllocMem because the CRT would automatically close
    // all heap objects on termination
    //    *ppTLS = (PPROFILERBLOCK)malloc( sizeof(PROFILERBLOCK) );
    int iSize = 4096 * (1 + sizeof(PROFILERBLOCK) / 4096);
    APIRET rc = DosAllocMem((PPVOID)ppTLS, iSize, PAG_COMMIT | PAG_READ | PAG_WRITE);
    SetFS(sel);
    
    if (rc == NO_ERROR)
    {
      memset(*ppTLS, 0, sizeof(PROFILERBLOCK) );
      
      PPROFILERBLOCK pPB = (PPROFILERBLOCK)*ppTLS;
      
      // experiment: thread-local profile
      // pPB->pHashtable = new CProfilerHashtableLookup(1021);
      pPB->pHashtable = pProfileMap;
    }
  }
  
  return (PPROFILERBLOCK)*ppTLS;
}



/***********************************************************************
* Name      : APIRET QueryModuleFromEIP
* Funktion  : Determine which module resides at 0:EIP
* Parameter :
* Variablen :
* Ergebnis  : Rckgabewert ans Betriebssystem
* Bemerkung :
*
* Autor     : Patrick Haller [Samstag, 28.10.1995 14.55.13]
***********************************************************************/

APIRET APIENTRY DosQueryModFromEIP (PULONG pulModule,
                                    PULONG pulObject,
                                    ULONG  ulBufferLength,
                                    PSZ    pszBuffer,
                                    PULONG pulOffset,
                                    ULONG  ulEIP);

static void i_ProfileQuerySymbol(ULONG eip_function,
                                 char **ppszModule,
                                 char **ppszFunction)
{
  APIRET rc;                                               /* API returncode */
  ULONG  ulModule;                                          /* module number */
  ULONG  ulObject;                        /* object number within the module */
  CHAR   szModule[260];                        /* buffer for the module name */
  ULONG  ulOffset;             /* offset within the object within the module */
  ULONG  ulSymbolOffset;
  
  rc = DosQueryModFromEIP(&ulModule,
                          &ulObject,
                          sizeof(szModule),
                          szModule,
                          &ulOffset,
                          eip_function);
  if (rc == NO_ERROR)
  {
    CHAR szFunction[128];
    *ppszModule = strdup(szModule);
    
    // @@@PH replace by symbol file lookup
    if (FALSE == pSymPool->getSymbolName(szModule,
                                         ulObject,
                                         ulOffset,
                                         szFunction,
                                         sizeof(szFunction),
                                         &ulSymbolOffset) )
    {
      sprintf(szFunction,
              "func_#%08xh",
              eip_function);
    }
    *ppszFunction = strdup(szFunction);
  }
  else
  {
    // neither module name nor function could be determined, so
    // we can just provide these values here.
    CHAR szFunction[40];
    *ppszModule = "<unknown module>";
    sprintf(szFunction,
            "func_#%08xh",
            eip_function);
    *ppszFunction = strdup(szFunction);
  }
}



//
// @@@PH
// Note: prevent compiler from generating hooks in these functions here!
// -> is there any pragma to prevent this?
// -> alternative: place it in the makefile accordingly w/ "/Gh-"

ULONG _System C_ProfileHook32Enter(ULONG eip_function, ULONG ret, ULONG time_hi, ULONG time_lo)
{
  if (!fIsProfilerEnabled)
    // tell low level procedure not to replace the return address
    // so bottom half will never be called
    return ret;
  
  USHORT sel = RestoreOS2FS();
  // DosEnterCritSec();
  
  
  // "call _ProfileHook32" has 5 bytes, adjust here
  eip_function -= 5;
  
  // 1 - get profiler per-thread-data-block
  PPROFILERBLOCK pPB = getProfilerBlock();
  if (NULL == pPB)
    // tell low level procedure not to replace the return address
    // so bottom half will never be called
    return ret;
  
  // 2 - save return address and call stack entry
  int i               = pPB->ulCalldepth;
  pPB->ret[i]         = ret;
  pPB->callStack[i]   = eip_function;
  
  // 3 - check if the database has an entry with the current EIP
  // @@@PH need to synchronize access!
  PPROFILEENTRY pPE = (PPROFILEENTRY) pPB->pHashtable->getElement(eip_function);
  
  // 3.1 - if not, create new entry
  if (NULL == pPE)
  {
    pPE = (PPROFILEENTRY)malloc( sizeof(PROFILEENTRY) );
    
    i_ProfileQuerySymbol(eip_function,
                         &pPE->pszModule,
                         &pPE->pszFunction);
    
    pPE->EIP           = eip_function;
    // pPE->pszModule     = NULL; // @@@PH
    // pPE->pszFunction   = NULL; // @@@PH
    pPE->ulCalls       = 0;
    pPE->ulTimeMaximum = 0;
    pPE->ulTimeMinimum = 0xffffffff;
    pPE->ulTimeTotal   = 0;
    
    // add to the hashtable
    // @@@PH need to synchronize access!
    pPB->pHashtable->addElement(eip_function, pPE);
  }
  
  // update available statistical data
  pPE->ulCalls++;
  
  // register call and skip to the next empty slot
  pPB->ulCalldepth++;
  
  // DosExitCritSec();
  SetFS(sel);
  
  // save overhead "timestamp"
  pPB->overheadTimeHi[i] = time_hi;
  pPB->overheadTimeLo[i] = time_lo;
  
  // get "compensated" timestamp
  // -> keep the time between entering the function and
  // exiting it as low as possible, yet the error will definately
  // accumulate for the "higher" callers
  _ProfileGetTimestamp(&pPB->enterTimeHi[i],
                       &pPB->enterTimeLo[i]);
  
  // tell low level procedure to replace the return address
  return (ULONG)&odin_ProfileHook32Bottom;
}


ULONG _System C_ProfileHook32Exit(ULONG time_hi, ULONG time_lo)
{
  // 1 - get profiler per-thread-data-block
  PPROFILERBLOCK pPB = getProfilerBlock();
  if (NULL == pPB)
    // Oops, we're now in deep trouble! The return address will not be
    // available, so we're definately gonna crash!
    return NULL;
  
  USHORT sel = RestoreOS2FS();
  // DosEnterCritSec();
  
  // register call exit
  pPB->ulCalldepth--;
  
  // 3.2 - add gathered statistical values to the entry
  int   i = pPB->ulCalldepth;
  ULONG eip_function = pPB->callStack[i];
  PPROFILEENTRY pPE = (PPROFILEENTRY) pPB->pHashtable->getElement(eip_function);
  
  // get another overhead timestamp
  ULONG ulOverheadHi;
  ULONG ulOverheadLo;
  _ProfileGetTimestamp(&ulOverheadHi,
                       &ulOverheadLo);
  
  // If we're the lowest method on the call stack, the timing is accurate.
  // If we came back from a subsequent call, we have to compensate the
  // acumulated time of the lower-level calls and pass it up to the
  // previous caller.
  
  // lowest caller?
  if (pPB->overheadTimeLo[i] != 0)
  {
    // No -> got to compensate
  }
  
  // calculate and save time difference spent in profiler
  
  if (pPE)
  {
    ULONG ulDiffHi = time_hi - pPB->enterTimeHi[i];
    ULONG ulDiffLo = time_lo - pPB->enterTimeLo[i];
    
    // test for wrap around
    if (ulDiffHi)
      ulDiffLo = ~ulDiffLo;
    
    // timestamps are in "cpu cycles"
    // ULONG ulTime = (ulDiffHi >> 4) | (ulDiffLo << 28);
    ULONG ulTime = ulDiffLo;
    
    if (pPE->ulTimeMinimum > ulTime)
      pPE->ulTimeMinimum = ulTime;
    
    if (pPE->ulTimeMaximum < ulTime)
      pPE->ulTimeMaximum = ulTime;
    
    pPE->ulTimeTotal += ulTime;
  }
  
  // DosExitCritSec();
  SetFS(sel);
  
  // 2 - return saved return address
  return pPB->ret[pPB->ulCalldepth];
}


// query if the profiler is enabled
BOOL _System ProfilerIsEnabled(void)
{
  return fIsProfilerEnabled;
}


// enable / disable the profiler hook
void _System ProfilerEnable(BOOL fState)
{
  fIsProfilerEnabled = fState;
}


// initialize the profiler (i. e. working directory)
void _System ProfilerInitialize(void)
{
  USHORT sel = RestoreOS2FS();
  
  // determine where to write the profile data to
  // (get the current working directory)
  getcwd(szWorkingDirectory, sizeof(szWorkingDirectory) );
  
  // @@@PH
  // try to determine the measurement overhead
  // (difficult due to non-deterministic behaviour
  // of the hashtable lookups and dynamic symbol loading, etc.)
  // We'd have to split the hook function into three parts:
  // - lookup and prepare the call
  // - enter call
  // - exit call
  // This however, still leaves us with wrong timing for the parent functions.
  
  SetFS(sel);
}


// terminate the profiler
void _System ProfilerTerminate(void)
{
  USHORT sel = RestoreOS2FS();
  
  // disable the profiler
  ProfilerEnable(FALSE);
  
  SetFS(sel);
}


int _Optlink sortHashtableEntries0(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRYID pHTE1 = (PHASHTABLEENTRYID)arg1;
  PHASHTABLEENTRYID pHTE2 = (PHASHTABLEENTRYID)arg2;
  
  PPROFILEENTRY p1 = (PPROFILEENTRY)pHTE1->pObject;
  PPROFILEENTRY p2 = (PPROFILEENTRY)pHTE2->pObject;
  
  return strcmp(p1->pszFunction, p2->pszFunction);
}

int _Optlink sortHashtableEntries1(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRYID pHTE1 = (PHASHTABLEENTRYID)arg1;
  PHASHTABLEENTRYID pHTE2 = (PHASHTABLEENTRYID)arg2;
  
  PPROFILEENTRY p1 = (PPROFILEENTRY)pHTE1->pObject;
  PPROFILEENTRY p2 = (PPROFILEENTRY)pHTE2->pObject;
  
  return p1->ulTimeTotal - p2->ulTimeTotal;
}

int _Optlink sortHashtableEntries2(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRYID pHTE1 = (PHASHTABLEENTRYID)arg1;
  PHASHTABLEENTRYID pHTE2 = (PHASHTABLEENTRYID)arg2;
  
  PPROFILEENTRY p1 = (PPROFILEENTRY)pHTE1->pObject;
  PPROFILEENTRY p2 = (PPROFILEENTRY)pHTE2->pObject;
  
  return p1->ulCalls - p2->ulCalls;
}

int _Optlink sortHashtableEntries3(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRYID pHTE1 = (PHASHTABLEENTRYID)arg1;
  PHASHTABLEENTRYID pHTE2 = (PHASHTABLEENTRYID)arg2;
  
  PPROFILEENTRY p1 = (PPROFILEENTRY)pHTE1->pObject;
  PPROFILEENTRY p2 = (PPROFILEENTRY)pHTE2->pObject;
  
  unsigned long int iAvg1 = p1->ulTimeTotal / p1->ulCalls;
  unsigned long int iAvg2 = p2->ulTimeTotal / p2->ulCalls;
  
  return iAvg1 - iAvg2;
}

int _Optlink sortHashtableEntries4(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRYID pHTE1 = (PHASHTABLEENTRYID)arg1;
  PHASHTABLEENTRYID pHTE2 = (PHASHTABLEENTRYID)arg2;
  
  PPROFILEENTRY p1 = (PPROFILEENTRY)pHTE1->pObject;
  PPROFILEENTRY p2 = (PPROFILEENTRY)pHTE2->pObject;
  
  return p1->EIP - p2->EIP;
}


// dump the collected profile to the specified file
void _Optlink Profiler_DumpProfile(FILE *file)
{
  fprintf(file,
          "ODIN Performance Analysis\n");
  
  BOOL flagLock = ProfilerIsEnabled();  // lock recording
  ProfilerEnable(FALSE);
  
  int iEntries = pProfileMap->getNumberOfElements();
  
  // get a list of all entries of the hashtable
  PHASHTABLEENTRYID arrEntries = (PHASHTABLEENTRYID)malloc( iEntries * sizeof(HASHTABLEENTRYID) );
  iEntries = pProfileMap->getElementMap(arrEntries);
  
  fprintf(file,
          "%d entries available.\n\n",
          iEntries);
  
  // sort the list by function name
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRYID ),
        sortHashtableEntries0);
  
  // write to file
  fprintf(file,
          "Sorted by function name\n"
          "Ticks ---- Called --- Average -- Minimum -- Maximum -- Function -----------\n");
  for(int i = 0;
      i < iEntries;
      i++)
  {
    PPROFILEENTRY p = (PPROFILEENTRY)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %10d %10d %s(%s)\n",
            p->ulTimeTotal,
            p->ulCalls,
            p->ulTimeTotal / p->ulCalls,
            p->ulTimeMinimum,
            p->ulTimeMaximum,
            p->pszFunction,
            p->pszModule);
  }
  
  
  // sort the list by ulTimeTotal
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRYID ),
        sortHashtableEntries1);
  
  // write to file
  fprintf(file,
          "\nSorted by total call time\n"
          "Ticks ---- Called --- Average -- Function ---------------------------------\n");
  for(i = 0;
      i < iEntries;
      i++)
  {
    PPROFILEENTRY p = (PPROFILEENTRY)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %s(%s)\n",
            p->ulTimeTotal,
            p->ulCalls,
            p->ulTimeTotal / p->ulCalls,
            p->pszFunction,
            p->pszModule);
  }
  
  
  // sort the list by ulCalls
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRYID ),
        sortHashtableEntries2);
  
  // write to file
  fprintf(file,
          "\nSorted by total calls\n"
          "Called --- Ticks ---- Average -- Function ---------------------------------\n");
  for(i = 0;
      i < iEntries;
      i++)
  {
    PPROFILEENTRY p = (PPROFILEENTRY)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %s(%s)\n",
            p->ulCalls,
            p->ulTimeTotal,
            p->ulTimeTotal / p->ulCalls,
            p->pszFunction,
            p->pszModule);
  }
  
  
  // sort the list by average call time
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRYID ),
        sortHashtableEntries3);
  
  // write to file
  fprintf(file,
          "\nSorted by average call time\n"
          "Average -- Calls ---- Ticks ---- Function ---------------------------------\n");
  for(i = 0;
      i < iEntries;
      i++)
  {
    PPROFILEENTRY p = (PPROFILEENTRY)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %s(%s)\n",
            p->ulTimeTotal / p->ulCalls,
            p->ulCalls,
            p->ulTimeTotal,
            p->pszFunction,
            p->pszModule);
  }
  
  
  // sort the list by address
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRYID ),
        sortHashtableEntries4);
  
  
  // write to file
  fprintf(file,
          "\nFunctions / symbols sorted by address\n"
          "Address ----- Function / Symbol -------------------------------------------\n");
  for(i = 0;
      i < iEntries;
      i++)
  {
    PPROFILEENTRY p = (PPROFILEENTRY)arrEntries[i].pObject;
    fprintf(file,
            "#%08xh %-9s %s\n",
            p->EIP,
            p->pszModule,
            p->pszFunction);
  }
  
  // at last print the sym map
  pSymPool->printSYMs(file);

  ProfilerEnable(flagLock);
}


void _System ProfilerWrite()
{
  FILE* _privateLogFile;
  char szFilename[260];
  USHORT sel = RestoreOS2FS();
  int pid = _getpid();

  sprintf(szFilename, "%s\\%d.prof", szWorkingDirectory, pid);
  _privateLogFile = fopen(szFilename, "w");
  
  if(_privateLogFile == NULL) 
  {
    DosBeep(500,500);
  }
  else
  {
    // dump the gathered data
    Profiler_DumpProfile(_privateLogFile);
  
    if(_privateLogFile)
    {
      fclose(_privateLogFile);
      _privateLogFile = NULL;
    }
  }
  
  SetFS(sel);
}

