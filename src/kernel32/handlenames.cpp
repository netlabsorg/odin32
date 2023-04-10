/* $Id: handlenames.cpp,v 1.6 2003-04-02 12:58:29 sandervl Exp $ */

/*
 * Win32 Handle Manager Object Namespace for OS/2
 *
 * 2001/11/23 Patrick Haller <patrick.haller@innotek.de>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

//#undef DEBUG_LOCAL
//#define DEBUG_LOCAL


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include <os2wrap.h>

#define __USE_EMX
#include <stdlib.h>
#include <string.h>
#include <win32type.h>

#include <ccollection.h>
#include "handlenames.h"
#include <vmutex.h>


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct tagHandleName
{
  // the real "defined" target name (which can directly passed on
  // to the device handlers or filesystems or ...
  PSZ   pszTarget;
  ULONG ulTargetLength;

  // the alias prefix
  PSZ   pszSymbolicLink;
  ULONG ulSymbolicLinkLength;
} HANDLENAME, *PHANDLENAME;


class HandleNames
{
  protected:
    VMutex       mtxHandleNameMgr;
    CLinearList* pSymbolicLinks;

    PHANDLENAME findSymbolicLink(PCSZ pszSymbolicLink,
                                 BOOL fCaseInsensitive);

    PHANDLENAME findSymbolicLinkExact(PCSZ pszSymbolicLink);

  public:
    HandleNames(void);
    ~HandleNames();

    BOOL addSymbolicLink(PCSZ pszSymbolicLink,
                         PCSZ pszTarget);

    BOOL removeSymbolicLink(PCSZ pszSymbolicLink);

    BOOL removeTarget(PCSZ pszTarget);

    BOOL resolveName(PCSZ pszName,
                     PSZ pszTarget,
                     ULONG ulTargetLength,
                     BOOL fCaseInsensitive);
};


/*****************************************************************************
 * Process Global Structures                                                 *
 *****************************************************************************/


static HandleNames  handleNameMgr;
static HandleNames* pHandleNameMgr = &handleNameMgr;


/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/



/*****************************************************************************
 * Name      : HandleNames::HandleNames
 * Purpose   : Constructor for handle name mapper
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

HandleNames::HandleNames(void)
{
  // Note: as handleNameMgr is a static object, the
  // destructor will never be called, this memory is leaked.
  pSymbolicLinks = new CLinearList();
}


/*****************************************************************************
 * Name      : HandleNames::~HandleNames
 * Purpose   : destructor for handle name mapper
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

HandleNames::~HandleNames()
{
  delete pSymbolicLinks;
}


/*****************************************************************************
 * Name      : HandleNames::findSymbolicLink
 * Purpose   : find the table entry with the specified symbolic link
 * Parameters: PSZ  pszSymbolicLink - the link to scan for
 *             BOOL fCaseInsensitive - TRUE for a case-insensitive lookup
 * Variables :
 * Result    :
 * Remark    : The comparison here is not meant to be "identity" but
 *             "startsWith" because for the name resolver, the first
 *             symbolic link that the specified name starts with is used
 *             for name resolution.
 *             So the idea is to prevent ambiguous symbolic link names here.
 *
 *             It is expected to enter this method in "locked" state
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

PHANDLENAME HandleNames::findSymbolicLink(PCSZ pszSymbolicLink,
                                          BOOL fCaseInsensitive)
{
  int               cchSymbolicLink = strlen(pszSymbolicLink);
  PLINEARLISTENTRY pLE = pSymbolicLinks->getFirst();
  while (pLE)
  {
    PHANDLENAME pHandleName = (PHANDLENAME)pLE->pObject;
    int cch = pHandleName->ulSymbolicLinkLength; //strlen(pHandleName->pszSymbolicLink);

    /* pszSymbolicLink must end a path component at cch. */
    if (    cch <= cchSymbolicLink
        &&  (pszSymbolicLink[cch] == '\\' || pszSymbolicLink[cch] == '\0'))
    {
    if (fCaseInsensitive)
    {
            if (!strnicmp(pHandleName->pszSymbolicLink, pszSymbolicLink, cch))
          return pHandleName;
    }
    else
    {
            if (!memcmp(pHandleName->pszSymbolicLink, pszSymbolicLink, cch))
          return pHandleName;
    }
    }

    // skip to the next entry
    pLE = pSymbolicLinks->getNext(pLE);
  }

  // not found
  return NULL;
}


/*****************************************************************************
 * Name      : HandleNames::findSymbolicLinkExact
 * Purpose   : find the table entry with the specified symbolic link
 * Parameters: PSZ  pszSymbolicLink - the link to scan for
 *             BOOL fCaseInsensitive - TRUE for a case-insensitive lookup
 * Variables :
 * Result    :
 * Remark    : The comparison here is not meant to be "identity" but
 *             "startsWith" because for the name resolver, the first
 *             symbolic link that the specified name starts with is used
 *             for name resolution.
 *             So the idea is to prevent ambiguous symbolic link names here.
 *
 *             It is expected to enter this method in "locked" state
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

PHANDLENAME HandleNames::findSymbolicLinkExact(PCSZ pszSymbolicLink)
{
  PLINEARLISTENTRY pLE = pSymbolicLinks->getFirst();
  while (pLE)
  {
    PHANDLENAME pHandleName = (PHANDLENAME)pLE->pObject;

    if (strcmp(pHandleName->pszSymbolicLink, pszSymbolicLink) == 0)
      return pHandleName;

    // skip to the next entry
    pLE = pSymbolicLinks->getNext(pLE);
  }

  // not found
  return NULL;
}


/*****************************************************************************
 * Name      : HandleNames::addSymbolicLink
 * Purpose   : add a symbolic link definition to the table
 * Parameters: PSZ pszSymbolicLink - the name of the symbolic link
 *             PSZ pszTarget       - the name of the device/name to link on
 * Variables :
 * Result    : TRUE if successful
 *             FALSE if otherwise (same link already exists)
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

BOOL HandleNames::addSymbolicLink(PCSZ pszSymbolicLink,
                                  PCSZ pszTarget)
{
  BOOL rc = TRUE;

  mtxHandleNameMgr.enter();

  // 1 - find symbolic link with same name
  PHANDLENAME pHandleName = findSymbolicLinkExact(pszSymbolicLink);

  // 2 - if found
  if (NULL != pHandleName)
  {
    // 2.1 - and targets are different, return FALSE
    if (strcmp(pszTarget, pHandleName->pszTarget) == 0)
      rc = TRUE;

    // 2.2 - and targets are identical, return TRUE
    else
      rc = FALSE;
  }
  else
  {
    // 3 - add definition to table
    pHandleName = (PHANDLENAME)malloc( sizeof(HANDLENAME) );
    if (NULL == pHandleName)
      rc = FALSE;
    else
    {
      pHandleName->pszSymbolicLink = strdup(pszSymbolicLink);
      if (NULL == pHandleName->pszSymbolicLink)
      {
        free (pHandleName);
        rc = FALSE;
      }
      else
      {
        pHandleName->pszTarget = strdup(pszTarget);
        if (NULL == pHandleName->pszTarget)
        {
          free (pHandleName->pszSymbolicLink);
          free (pHandleName);
          rc = FALSE;
        }
        else
        {
          // fill in these values
          pHandleName->ulTargetLength = strlen(pszTarget);
          pHandleName->ulSymbolicLinkLength = strlen(pszSymbolicLink);

          // OK, finally add to the list
          pSymbolicLinks->addFirst(pHandleName);
        }
      }
    }
  }

  mtxHandleNameMgr.leave();

  return rc;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    : TRUE if successful, FALSE if otherwise
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

BOOL HandleNames::removeSymbolicLink(PCSZ pszSymbolicLink)
{
  BOOL rc = TRUE;

  mtxHandleNameMgr.enter();

  // 1 - find symbolic name
  PHANDLENAME pHandleName = findSymbolicLinkExact(pszSymbolicLink);
  if (NULL == pHandleName)
    rc = FALSE;
  else
  {
    // 2 - remove the link
    pSymbolicLinks->removeObject(pHandleName);

    if (NULL != pHandleName->pszSymbolicLink )
      free( pHandleName->pszSymbolicLink );

    if (NULL != pHandleName->pszTarget )
      free( pHandleName->pszTarget );

    free( pHandleName );
  }

  mtxHandleNameMgr.leave();

  return rc;
}


/*****************************************************************************
 * Name      : HandleNames::removeTarget
 * Purpose   : remove all links to the specified target
 * Parameters: PSZ pszTarget - the name of the target to remove
 * Variables :
 * Result    : TRUE if successful, FALSE if otherwise
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

BOOL HandleNames::removeTarget(PCSZ pszTarget)
{
  BOOL rc = FALSE;

  mtxHandleNameMgr.enter();

  // iterate over all registered symbolic links
  PLINEARLISTENTRY pLE = pSymbolicLinks->getFirst();
  while (pLE)
  {
    PHANDLENAME pHandleName = (PHANDLENAME)pLE->pObject;

    // check the name
    if (strcmp(pszTarget, pHandleName->pszTarget) == 0)
    {
      pSymbolicLinks->removeElement(pLE);

      // at least one removal succeeded
      rc = TRUE;
    }
  }

  mtxHandleNameMgr.leave();

  return rc;
}


/*****************************************************************************
 * Name      : HandleNames::resolveName
 * Purpose   : resolve the specified name according to the symbolic links
 *             until we reach the "final" name
 * Parameters: PSZ   pszName
 *             PSZ   pszTarget
 *             ULONG ulTargetLength
 *             BOOL  fCaseInsensitive
 * Variables :
 * Result    : FALSE if name was not modified, TRUE if name was resolved
 * Remark    : This is a very easy, cheesy implementation of a pathname
 *             cracker. Should be sufficient at the moment though.
 * Status    :
 *
 * Author    : Patrick Haller [2001/11/23]
 *****************************************************************************/

BOOL HandleNames::resolveName(PCSZ pszName,
                              PSZ pszTarget,
                              ULONG ulTargetLength,
                              BOOL fCaseInsensitive)
{
  BOOL rc = FALSE;

  mtxHandleNameMgr.enter();

  // scan through the names (case-insensitive)
  PHANDLENAME pHandleName = findSymbolicLink(pszName, fCaseInsensitive);
  if (NULL != pHandleName)
  {
    // rebuild the target name
    int iNameLength     = strlen(pszName);

    // first copy the resolved target name fragment
    strncpy(pszTarget,
            pHandleName->pszTarget,
            ulTargetLength);

    // now append the rest of the specified name with the
    // now resolved symbolic cut away
    if (ulTargetLength != pHandleName->ulTargetLength)
      strncpy(pszTarget + pHandleName->ulTargetLength,
              pszName + pHandleName->ulSymbolicLinkLength,
              ulTargetLength - pHandleName->ulTargetLength);

    // tell caller the name has been resolved
    // (is different from the source name)
    rc = TRUE;
  }

  mtxHandleNameMgr.leave();

  return rc;
}




/*****************************************************************************
 * Exported Wrapper Functions
 *****************************************************************************/

BOOL HandleNamesResolveName(PCSZ pszName,
                            PSZ pszTarget,
                            ULONG ulTargetLength,
                            BOOL fCaseInsensitive)
{
  return pHandleNameMgr->resolveName(pszName,
                                     pszTarget,
                                     ulTargetLength,
                                     fCaseInsensitive);
}


BOOL HandleNamesAddSymbolicLink(PCSZ pszSymbolicLink,
                                PCSZ pszTarget)
{
  return pHandleNameMgr->addSymbolicLink(pszSymbolicLink,
                                         pszTarget);
}


BOOL HandleNamesRemoveSymbolicLink(PCSZ pszSymbolicLink)
{
  return pHandleNameMgr->removeSymbolicLink(pszSymbolicLink);
}


BOOL HandleNamesRemoveTarget(PCSZ pszTarget)
{
  return pHandleNameMgr->removeTarget(pszTarget);
}
