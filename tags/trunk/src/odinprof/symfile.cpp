/* $Id: symfile.cpp,v 1.3 2001-11-22 13:35:42 phaller Exp $ */
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
 
 - find and load symbolic debug information file
   (SYM2IDA)
   => verify against kernel32\exceptionstack.cpp
   (KERNEL32.SYM is not interpreted correctly)
 */


/****************************************************************************
 * includes
 ****************************************************************************/

#define INCL_DOSERRORS
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_DOSFILEMGR
#include <os2.h>

#include <os2sel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <demangle.h>

#include "profcollection.h"
#include "symfile.h"


/****************************************************************************
 * implementation
 ****************************************************************************/

/*****************************************************************************
 * Name      : APIRET ReadFileToBuffer
 * Funktion  : Reads a single file completely into a memory buffer
 * Parameter : PSZ pszFile, PPVOID ppBuffer, PULONG pulBuffer
 * Variablen :
 * Ergebnis  : API-Returncode
 * Bemerkung :
 *
 * Autor     : Patrick Haller [Samstag, 07.10.1995 03.22.44]
 *****************************************************************************/

APIRET ToolsReadFileToBuffer (PSZ    pszFile,
                              PPVOID ppBuffer,
                              PULONG pulBuffer)
{
  APIRET      rc;                                          /* API-Returncode */
  FILESTATUS3 fs3Status;                               /* Dateiinformationen */
  HFILE       hFileInput;                               /* input file handle */
  ULONG       ulAction;                       /* dummy parameter for DosOpen */
  ULONG       ulReadSize;                /* number of bytes to read per call */
  ULONG       ulReadTotal = 0;                 /* total number of bytes read */

  if ( (pszFile   == NULL) ||                            /* check parameters */
       (ppBuffer  == NULL) ||
       (pulBuffer == NULL) )
    return (ERROR_INVALID_PARAMETER);                        /* signal error */


  rc = DosQueryPathInfo (pszFile,                          /* Infos einholen */
                         FIL_STANDARD,
                         &fs3Status,
                         sizeof(fs3Status));
  if (rc != NO_ERROR)                           /* check if an error occured */
    return (rc);                                             /* signal error */

                                         /* allocate buffer RAM for the file */
  *pulBuffer = fs3Status.cbFile;              /* return the size of the file */
  rc = DosAllocMem(ppBuffer,                          /* allocate the buffer */
                   fs3Status.cbFile,
                   PAG_WRITE |
                   PAG_READ  |
                   PAG_COMMIT);
  if (rc != NO_ERROR)                      /* check if the allocation failed */
    return (rc);                                             /* signal error */

  ulReadSize = fs3Status.cbFile;

  rc = DosOpen(pszFile,                                    /* File path name */
               &hFileInput,                                   /* File handle */
               &ulAction,                                    /* Action taken */
               0L,                                /* File primary allocation */
               FILE_ARCHIVED |
               FILE_NORMAL,                                /* File attribute */
               OPEN_ACTION_FAIL_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,             /* Open function type */
               OPEN_FLAGS_NOINHERIT |
               OPEN_FLAGS_SEQUENTIAL|
               OPEN_SHARE_DENYNONE  |
               OPEN_ACCESS_READONLY,                /* Open mode of the file */
               0L);                                 /* No extended attribute */
  if (rc == NO_ERROR)                                    /* check for errors */
  {
    ULONG ulRead;                                    /* number of read bytes */

    rc = DosRead (hFileInput,
                  *ppBuffer,
                  ulReadSize,
                  &ulRead);

    DosClose (hFileInput);               /* close the filehandle in any case */
  }

  if (rc != NO_ERROR)                                    /* check for errors */
  {
    DosFreeMem (*ppBuffer);                     /* free the allocated memory */
    *ppBuffer  = NULL;                       /* reset the passed back values */
    *pulBuffer = 0;
  }

  return (rc);                                                /* return code */
}




LXSymbolFile::LXSymbolFile(PSZ _pszName, PSZ _pszFileName)
{
  pszName         = strdup(_pszName); // copy lookup name
  pszFileName     = strdup(_pszFileName);
  pszErrorMessage = NULL;
}


LXSymbolFile::~LXSymbolFile()
{
  if (pszName)
    free(pszName);
  
  if (pszErrorMessage)
    free(pszErrorMessage);
  
  if (pszFileName)
    free(pszFileName);
  
  if (pSymbolRawData)
    DosFreeMem (pSymbolRawData);
}


void LXSymbolFile::setErrorMessage(PSZ _pszErrorMessage)
{
  if (pszErrorMessage)
    free(pszErrorMessage);
  
  if (NULL != _pszErrorMessage)
    pszErrorMessage = strdup(_pszErrorMessage);
  else
    pszErrorMessage = NULL;
}


APIRET LXSymbolFile::parseFile()
{
  APIRET rc;
  ULONG  ulBufferSize;
  
  // 1 - read file to buffer
  rc = ToolsReadFileToBuffer(pszFileName,
                             (PPVOID)&pSymbolRawData,
                             &ulBufferSize);
  if (NO_ERROR != rc)
  {
    setErrorMessage("unable to read file");
    return rc;
  }
  
  return NO_ERROR;
}


BOOL   LXSymbolFile::isAvailable()
{
  return pszErrorMessage == NULL;
}


PSZ    LXSymbolFile::getErrorMessage()
{
  return pszErrorMessage;
}


PSZ    LXSymbolFile::getFileName()
{
  return pszFileName;
}


BOOL   LXSymbolFile::getSymbolName(ULONG  objNr,
                                   ULONG  offset,
                                   PSZ    pszNameBuffer,
                                   ULONG  ulNameBufferLength,
                                   PULONG pulSymbolOffset)
{
  PUCHAR    p = (PUCHAR)pSymbolRawData;
  ULONG     ulMapDefs;
  PMAPDEF   pMapDef;
  ULONG     SegOffset;
  ULONG     SymOffset;
  ULONG     ulSegNum;
  ULONG     ulSymNum;
  SEGDEF*   pSegDef;
  SYMDEF32* pSymDef32;
  
  // scan through the maps
  pMapDef = (PMAPDEF)p;
  for (ulMapDefs = 0;
       pMapDef->ppNextMap != 0;
       ulMapDefs++)
  {
    SegOffset = pMapDef->ppSegDef * 16;
    
    for (ulSegNum = 0;
         ulSegNum < pMapDef->cSegs;
         ulSegNum++)
    {
      pSegDef = (PSEGDEF)(p + SegOffset);
      
      // check for the segment number
      if (ulSegNum == objNr)
      {
        PSYMDEF32 pSymClosest = NULL;
        
        // lookup the symbol
        for (ulSymNum = 0;
             ulSymNum < pSegDef->cSymbols;
             ulSymNum++)
        {
          /* calculate symbol offset */
          PUCHAR pucTemp = ( (PUCHAR)pSegDef + 
                            pSegDef->pSymDef +
                            ulSymNum * sizeof(USHORT) );
          SymOffset = *(PUSHORT)pucTemp;
          pucTemp = (PUCHAR)pSegDef + SymOffset;
          
          if (pSegDef->bFlags & 0x01)
          {
            // 32-bit segment
            pSymDef32 = (PSYMDEF32)pucTemp;
            
            if (NULL == pSymClosest)
              pSymClosest = pSymDef32;
            else
            {
              int iClosest = offset - pSymClosest->wSymVal;
              int iCurrent = offset - pSymDef32->wSymVal;
              
              // find nearest symbol to the given offset
              if (iCurrent > 0)
                if (iClosest > iCurrent)
                  pSymClosest = pSymDef32;
            }
            
            // check if we've found it (or get the closest symbol)
            if (offset == pSymDef32->wSymVal)
            {
              int iLen = min(pSymDef32->cbSymName, ulNameBufferLength);
              memcpy(pszNameBuffer,
                     pSymDef32->achSymName,
                     iLen);
              
              // terminate the string in any case
              pszNameBuffer[iLen] = 0;
              *pulSymbolOffset = 0;
              
              // OK, found
              return TRUE;
            }
          }
          else
          {
            // 16-bit segment
            // @@@PH not supported
          }
        } /* symbol lookup */
        
        if (NULL != pSymClosest)
        {
          CHAR szBuf[128];
          int  iLen;
          
          // proper symbol name found?
          if ( (pSymClosest->achSymName[0] > 32) &&
               (pSymClosest->cbSymName < 255) )
          {
            memcpy(szBuf, pSymClosest->achSymName, sizeof(szBuf));
            szBuf[ min(sizeof(szBuf), pSymClosest->cbSymName) ] = 0;
            iLen = min(strlen(szBuf), ulNameBufferLength);
          }
          else
          {
            sprintf(szBuf, "%s:obj%d:%08xh", getName(), objNr, offset);
            iLen = min(strlen(szBuf), ulNameBufferLength);
          }

          memcpy(pszNameBuffer, szBuf, iLen);
          pszNameBuffer[iLen] = 0; // terminate the string in any case
          *pulSymbolOffset = offset - pSymClosest->wSymVal;
          return TRUE;            // at least something usable has been found!
        }
        else
        {
          // nothing usable was found
          return FALSE;
        }
      }
      
      SegOffset = (pSegDef->ppNextSeg * 16);
    } /* segment loop */
    
    p += (pMapDef->ppNextMap * 16);
    pMapDef = (PMAPDEF)p;
  } /* map loop */
  
  // not found
  return FALSE;
}
  




SymbolFilePool::SymbolFilePool()
{
  pHashModules = new CHashtableLookup(67);
}


SymbolFilePool::~SymbolFilePool()
{
  if (pHashModules)
    delete pHashModules;
}


APIRET SymbolFilePool::searchModule(PSZ pszModule, PBYTE pBuffer, ULONG ulBufferLength)
{
  // 1 - build symbol filename
  CHAR szSymFile[260];
  
  strcpy(szSymFile, pszModule);
  PSZ pszDot = strrchr(szSymFile, '.');
  if (NULL == pszDot)
    pszDot = szSymFile + strlen(szSymFile);
  
  strcpy(pszDot, ".sym");
  
  // 2 - search path for symbol files
  //     and copy result to pszFilename
  APIRET rc = DosSearchPath(SEARCH_IGNORENETERRS |
                            SEARCH_ENVIRONMENT |
                            SEARCH_CUR_DIRECTORY,
                            "PATH",
                            szSymFile,
                            pBuffer,
                            ulBufferLength);
  if ( (rc == ERROR_FILE_NOT_FOUND) ||
       (rc == ERROR_ENVVAR_NOT_FOUND) )
    rc = DosSearchPath(SEARCH_IGNORENETERRS |
                       SEARCH_ENVIRONMENT |
                       SEARCH_CUR_DIRECTORY,
                       "DPATH",
                       szSymFile,
                       pBuffer,
                       ulBufferLength);
  
  return rc;
}


  
BOOL   SymbolFilePool::getSymbolName(PSZ    pszModule,
                                     ULONG  objNr,
                                     ULONG  offset,
                                     PSZ    pszNameBuffer,
                                     ULONG  ulNameBufferLength,
                                     PULONG pulSymbolOffset)
{
  LXSymbolFile* pSym = (LXSymbolFile*)pHashModules->getElement(pszModule);
  if (NULL == pSym)
  {
    DosEnterCritSec();
    
    // try again since someone else could have loaded the map already
    pSym = (LXSymbolFile*)pHashModules->getElement(pszModule);
    if (NULL == pSym)
    {
      CHAR szFilename[260];
      
      // 1 - locate the file
      APIRET rc = searchModule(pszModule, szFilename, sizeof(szFilename));
      
      // create new entry
      pSym = new LXSymbolFile(pszModule, szFilename);
      
      // parse the file
      if (rc == NO_ERROR)
        pSym->parseFile();
      else
        pSym->setErrorMessage("file not found");
      
      // add to the hashtable
      PSZ pszCopyOfModuleName = strdup(pszModule);
      pHashModules->addElement(pszCopyOfModuleName, pSym);
    }
    
    DosExitCritSec();
  }
  
  BOOL rc;
  
  // if the parsed symbol table is correct, lookup the symbol
  if (pSym->isAvailable())
  {
    rc = pSym->getSymbolName(objNr,
                             offset,
                             pszNameBuffer,
                             ulNameBufferLength,
                             pulSymbolOffset);
    
    if (rc == TRUE)
    {
      // check if we can demangle a C++ name
      char* rest;
      Name* name = Demangle(pszNameBuffer, rest);
      if (name != NULL)
      {
        strncpy(pszNameBuffer,
                name->Text(),
                ulNameBufferLength);
        delete name;
      }

      // append symbol offsets
      if (*pulSymbolOffset != 0)
      {
        CHAR szBuf[256];
        sprintf(szBuf,
                "%s+%xh",
                pszNameBuffer,
                *pulSymbolOffset);
        strncpy(pszNameBuffer,
                szBuf,
                ulNameBufferLength);
      }
    }
  }
  else
    rc = FALSE;
  
  return rc;
}


static int _Optlink sortHashtableSYMs(const void *arg1,const void *arg2)
{
  PHASHTABLEENTRY pHTE1 = (PHASHTABLEENTRY)arg1;
  PHASHTABLEENTRY pHTE2 = (PHASHTABLEENTRY)arg2;
  
  LXSymbolFile* p1 = (LXSymbolFile*)pHTE1->pObject;
  LXSymbolFile* p2 = (LXSymbolFile*)pHTE2->pObject;
  
  return stricmp(p1->getName(), p2->getName());
}


void SymbolFilePool::printSYMs(FILE *file)
{
  int iEntries = pHashModules->getNumberOfElements();
  
  // get a list of all entries of the hashtable
  PHASHTABLEENTRY arrEntries = (PHASHTABLEENTRY)malloc( iEntries * sizeof(HASHTABLEENTRY) );
  iEntries = pHashModules->getElementMap(arrEntries);
  
  fprintf(file,
          "\nSymbolic debug information maps (%d maps)\n",
          iEntries);
  
  // sort the list by name
  qsort(arrEntries,
        iEntries,
        sizeof( HASHTABLEENTRY ),
        sortHashtableSYMs);
  
  // write to file
  fprintf(file,
          "Module ---- Status --------------------------------------------------------\n");
  for(int i = 0;
      i < iEntries;
      i++)
  {
    LXSymbolFile* p = (LXSymbolFile*)arrEntries[i].pObject;
    fprintf(file,
            "%-10s %s (%s)\n",
            p->getName(),
            p->getFileName(),
            (p->getErrorMessage() != NULL) ? p->getErrorMessage() : "");
  }
}
