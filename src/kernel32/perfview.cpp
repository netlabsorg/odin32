/* $Id: perfview.cpp,v 1.3 2001-10-12 03:48:06 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 performance measurement and self-profiling API functions
 *
 * Copyright 2001 Patrick Haller <patrick.haller@innotek.de>
 */

/****************************************************************************
 * includes
 ****************************************************************************/
#include "perfview.h"


#ifndef PROFILE

// insert "nullified" dummies here to save space in the executable image
void PerfView_Initialize(void) {}
void PerfView_RegisterCall(char* pszFunctionName, 
                           unsigned long int nTicks) {}

void PerfView_DumpProfile(FILE *file) {}
void PerfView_Write() {}

#else

#include <ccollection.h>
#include <winbase.h>

// imported from the kernel loader (initterm)
extern int loadNr;
extern char  kernel32Path[];



typedef struct tagFUNCTION
{
  char* pszFunctionName;
  unsigned long int nCalled;
  unsigned long int nTotalTicks;
  unsigned long int nMinimumTicks;
  unsigned long int nMaximumTicks;
} PERFVIEW_FUNCTION, *PPERFVIEW_FUNCTION;


// the map keeps track of all called methods and functions
static CHashtableLookup* pProfileMap = new CHashtableLookup(1021);
static BOOL flagLock = FALSE;
static unsigned long int tsCompensation = 0;


// measure the measurement overhead itself
void PerfView_Initialize(void) 
{
#define CALIBRATION_RUNS 100
  
  LARGE_INTEGER liStart;
  LARGE_INTEGER liEnd;
  unsigned long ulElapsed;
  
  // initialize this
  tsCompensation = 0;
  
  for (int i = 0;
       i < CALIBRATION_RUNS;
       i++)
  {
    QueryPerformanceCounter(&liStart);
    QueryPerformanceCounter(&liEnd);

    if (liStart.LowPart > liEnd.LowPart)
      ulElapsed = 0xFFFFFFFF - liStart.LowPart + liEnd.LowPart;
    else
      ulElapsed = liEnd.LowPart - liStart.LowPart;
  
    // save the determined amount of elapsed ticks
    // as compensatory factor
    tsCompensation += ulElapsed;
  }

  // now calculate back to real value
  tsCompensation /= CALIBRATION_RUNS;
}


// register a call to a function
void _Optlink PerfView_RegisterCall(char* pszFunctionName, 
                                    unsigned long int nTicks)
{
  // don't record call if currently locked
  if (flagLock)
    return;
  
  // subtract the measurement overhead factor.
  // Note: this should rather be done where the times are taken,
  // however this would spread the code just too far.
  if (tsCompensation < nTicks)
    nTicks -= tsCompensation;
  else
    nTicks = 0;
  
  // check if that particular function is registered already
  PPERFVIEW_FUNCTION p = (PPERFVIEW_FUNCTION)pProfileMap->getElement(pszFunctionName);
  if (NULL == p)
  {
    // new function call
    p = (PPERFVIEW_FUNCTION)malloc( sizeof( PERFVIEW_FUNCTION ) );
    p->pszFunctionName = strdup( pszFunctionName );
    p->nCalled = 0;
    p->nTotalTicks = 0;
    p->nMinimumTicks = 0xffffffff;
    p->nMaximumTicks = 0;
    
    // add to the hashtable
    pProfileMap->addElement(pszFunctionName, p);
  }
  
  // update statistical data
  p->nCalled++;
  p->nTotalTicks += nTicks;
  
  if (nTicks < p->nMinimumTicks)
    p->nMinimumTicks = nTicks;
  
  if (nTicks > p->nMaximumTicks)
    p->nMaximumTicks = nTicks;
}


int _Optlink sortHashtableEntries0(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRY pHTE1 = (PHASHTABLEENTRY)arg1;
  PHASHTABLEENTRY pHTE2 = (PHASHTABLEENTRY)arg2;
  
  PPERFVIEW_FUNCTION p1 = (PPERFVIEW_FUNCTION)pHTE1->pObject;
  PPERFVIEW_FUNCTION p2 = (PPERFVIEW_FUNCTION)pHTE2->pObject;
  
  return strcmp(p1->pszFunctionName, p2->pszFunctionName);
}

int _Optlink sortHashtableEntries1(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRY pHTE1 = (PHASHTABLEENTRY)arg1;
  PHASHTABLEENTRY pHTE2 = (PHASHTABLEENTRY)arg2;
  
  PPERFVIEW_FUNCTION p1 = (PPERFVIEW_FUNCTION)pHTE1->pObject;
  PPERFVIEW_FUNCTION p2 = (PPERFVIEW_FUNCTION)pHTE2->pObject;
  
  // return strcmp(pHTE1->pszName, pHTE2->pszName);
  return p1->nTotalTicks - p2->nTotalTicks;
}

int _Optlink sortHashtableEntries2(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRY pHTE1 = (PHASHTABLEENTRY)arg1;
  PHASHTABLEENTRY pHTE2 = (PHASHTABLEENTRY)arg2;
  
  PPERFVIEW_FUNCTION p1 = (PPERFVIEW_FUNCTION)pHTE1->pObject;
  PPERFVIEW_FUNCTION p2 = (PPERFVIEW_FUNCTION)pHTE2->pObject;
  
  return p1->nCalled - p2->nCalled;
}

int _Optlink sortHashtableEntries3(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRY pHTE1 = (PHASHTABLEENTRY)arg1;
  PHASHTABLEENTRY pHTE2 = (PHASHTABLEENTRY)arg2;
  
  PPERFVIEW_FUNCTION p1 = (PPERFVIEW_FUNCTION)pHTE1->pObject;
  PPERFVIEW_FUNCTION p2 = (PPERFVIEW_FUNCTION)pHTE2->pObject;
  
  unsigned long int iAvg1 = p1->nTotalTicks / p1->nCalled;
  unsigned long int iAvg2 = p2->nTotalTicks / p2->nCalled;
  
  return iAvg1 - iAvg2;
}


// dump the collected profile to the specified file
void _Optlink PerfView_DumpProfile(FILE *file)
{
  flagLock = TRUE;  // lock recording
  
  int iEntries = pProfileMap->getSize();
  
  // get a list of all entries of the hashtable
  PHASHTABLEENTRY arrEntries = (PHASHTABLEENTRY)malloc( iEntries * sizeof(HASHTABLEENTRY) );
  iEntries = pProfileMap->getElementMap(arrEntries);
  
  fprintf(file,
          "ODIN Performance Analysis\n"
          "%d entries available, compensated measure overhead is %d ticks.\n\n",
          iEntries,
          tsCompensation);
  
  
  // sort the list by function name
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRY ),
        sortHashtableEntries0);
  
  // write to file
  fprintf(file,
          "Sorted by function name\n"
          "Ticks ---- Called --- Average -- Minimum -- Maximum -- Function -----------\n");
  for(int i = 0;
      i < iEntries;
      i++)
  {
    PPERFVIEW_FUNCTION p = (PPERFVIEW_FUNCTION)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %10d %10d %s\n",
            p->nTotalTicks,
            p->nCalled,
            p->nTotalTicks / p->nCalled,
            p->nMinimumTicks,
            p->nMaximumTicks,
            p->pszFunctionName);
  }
  
  
  // sort the list by nTotalTicks
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRY ),
        sortHashtableEntries1);
  
  // write to file
  fprintf(file,
          "\nSorted by total call time\n"
          "Ticks ---- Called --- Average -- Function ---------------------------------\n");
  for(i = 0;
      i < iEntries;
      i++)
  {
    PPERFVIEW_FUNCTION p = (PPERFVIEW_FUNCTION)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %s\n",
            p->nTotalTicks,
            p->nCalled,
            p->nTotalTicks / p->nCalled,
            p->pszFunctionName);
  }
  
  
  // sort the list by nCalled
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRY ),
        sortHashtableEntries2);
  
  // write to file
  fprintf(file,
          "\nSorted by total calls\n"
          "Called --- Ticks ---- Average -- Function ---------------------------------\n");
  for(i = 0;
      i < iEntries;
      i++)
  {
    PPERFVIEW_FUNCTION p = (PPERFVIEW_FUNCTION)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %s\n",
            p->nCalled,
            p->nTotalTicks,
            p->nTotalTicks / p->nCalled,
            p->pszFunctionName);
  }
  
  
  // sort the list by average call time
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRY ),
        sortHashtableEntries3);
  
  // write to file
  fprintf(file,
          "\nSorted by average call time\n"
          "Average -- Calls ---- Ticks ---- Function ---------------------------------\n");
  for(i = 0;
      i < iEntries;
      i++)
  {
    PPERFVIEW_FUNCTION p = (PPERFVIEW_FUNCTION)arrEntries[i].pObject;
    fprintf(file,
            "%10d %10d %10d %s\n",
            p->nTotalTicks / p->nCalled,
            p->nCalled,
            p->nTotalTicks,
            p->pszFunctionName);
  }
  
  
  flagLock = FALSE;  // lock recording
}


//use a different logfile
#define PRIVATE_LOGGING
#include <misc.h>


void PerfView_Write()
{
  FILE* _privateLogFile;
  char szFilename[260];

  sprintf(szFilename, "perf_%d.log", loadNr);
  _privateLogFile = fopen(szFilename, "w");
  
  if(_privateLogFile == NULL) 
  {
    sprintf(szFilename, "%sperf_%d.log", kernel32Path, loadNr);
    _privateLogFile = fopen(szFilename, "w");
  }
  
  dprintfGlobal(("PE PERFFILE : %s", szFilename));

  // dump the gathered data
  PerfView_DumpProfile(_privateLogFile);
  
  if(_privateLogFile) 
  {
    fclose(_privateLogFile);
    _privateLogFile = NULL;
  }
}



#endif /* PROFILE */
