/* $Id: glucb32.cpp,v 1.1 2000-01-27 21:49:12 sandervl Exp $ */
/*****************************************************************************/
/*                                                                           */
/* GLU/GLUT callback handler                                                 */
/*                                                                           */
/* Both GLU and GLUT make heavy use of callbacks to notify the application   */
/* of certain events. The event is registered for an object, and only an     */
/* event code is passed back to the application. Since *no* object or other  */
/* handle is passed back, we have no way of determining where to dispatch    */
/* the event to! Directly calling into the application wreaks havoc, since   */
/* the calling convention is different, and hence we'll nuke the stack!!     */
/*                                                                           */
/* The code herein allocates a memory page and dynamically generates machine */
/* code to push the parameters on the stack, call the registered EP and      */
/* return.                                                                   */
/*                                                                           */
/*****************************************************************************/

#define INCL_BASE
#include <os2.h>
#include <os2wrap.h>
#include <misc.h>
#include <malloc.h>
#include "glos2.h"
#include "gluos2.h"
#include "glucb32.h"

/*****************************************************************************/
/*                                                                           */
/* Globals - exported                                                        */
/*                                                                           */
/*****************************************************************************/

HMTX   hmtxGlucb;
GLUCB *Glucbs = NULL;

/*****************************************************************************/
/*                                                                           */
/* Callback Handler - Pass on the call to the registered functions           */
/*                                                                           */
/*****************************************************************************/

GLUCB *OS2glucbQueryGlucb(GLbyte objtype,void *object,GLenum which)
{
  GLUCB *glucb=Glucbs;

  while(glucb)
    {
      if(glucb->objtype==objtype &&
         glucb->object==object &&
         glucb->which==which)
        return glucb;

      glucb=glucb->Next;
    }

  return NULL;
}

GLUCB *OS2glucbCreateGlucb(GLenum objtype,void *object,GLenum which,GLint num_parms)
{
  GLUCB *glucb;
  int    i;

  glucb=(GLUCB *)malloc(sizeof(GLUCB));

  glucb->objtype=objtype;
  glucb->object=object;
  glucb->which=which;
  glucb->parms_on_stack=num_parms;

  DosAllocMem((PPVOID)(&glucb->entryAddress),
              4096,
              PAG_COMMIT | PAG_EXECUTE | PAG_READ | PAG_WRITE);

  /* Setup the actual machine-code in this page... */
  /* Insert a number of 'PUSH DWORD PTR [ESP+08H]' to set the stack */
  for(i=0; i<num_parms; i++)
    ((ULONG *)((BYTE *)glucb->entryAddress))[i]=0x082474FF;

  /* Insert a CALL instruction */
  ((BYTE *)glucb->entryAddress)[num_parms*4]=0xE8;
  /* Next 4 bytes will eventually hold return address! */
  /* Finally insert a RET instruction */
  ((BYTE *)glucb->entryAddress)[num_parms*4+5]=0xC3;

  glucb->Next=Glucbs;

  Glucbs=glucb;

  return glucb;
}

void OS2glucbDestroyGlucb(GLUCB *glucb)
{
  /* Destroy a glucb! */
  GLUCB *glucbPrev;

  if(glucb==Glucbs)
    glucbPrev=NULL;
  else
    {
      /* Don't check pointer - it must exist so there is at least one Glucbs member */
      glucbPrev=Glucbs;

      while(glucbPrev->Next!=glucb)
        glucbPrev=glucbPrev->Next;
    }

  DosFreeMem(glucb->entryAddress);

  if(glucbPrev)
    glucbPrev->Next=glucb->Next;
  else
    Glucbs=glucb->Next;

  free(glucb);
}

void OS2glucbSetGlucbAddress(GLUCB *glucb,GLvoid (* WIN32API Address)())
{
  ULONG  addr=(ULONG)Address;
  ULONG  addr_e=(ULONG)glucb->entryAddress+4*glucb->parms_on_stack;
  ULONG  addr_offset=addr_e-addr-5;
  BYTE  *code=(BYTE *)glucb->entryAddress;

  *((ULONG *)code[addr_e+1])=addr_offset;
}

GLUOS2CALLBACK OS2glucbGenerateCallbackFunc(GLbyte  objtype,
                                            void   *object,
                                            GLenum  which,
                                            GLUWINCALLBACK CallBackFunc,
                                            int     NumParameters)
{
  /* NOTE: All parms are 4-byte in size            */
  GLUCB *glucb;

  DosRequestMutexSem(hmtxGlucb,SEM_INDEFINITE_WAIT);

  glucb=OS2glucbQueryGlucb(objtype,object,which);

  if(glucb)
    {
      if(CallBackFunc)
        {
          OS2glucbSetGlucbAddress(glucb,CallBackFunc);

          DosReleaseMutexSem(hmtxGlucb);

          return glucb->entryAddress;
        }
      else
        {
          OS2glucbDestroyGlucb(glucb);

          DosReleaseMutexSem(hmtxGlucb);

          return 0;
        }
    }
  else
    {
      if(CallBackFunc)
        {
          /* Setup a page in memory with the required code */
          glucb=OS2glucbCreateGlucb(objtype,object,which,NumParameters);

          OS2glucbSetGlucbAddress(glucb,CallBackFunc);

          DosReleaseMutexSem(hmtxGlucb);

          return glucb->entryAddress;
        }
      else
        dprintf(("GLUCALLBACK (ERROR): Callback remove, but no callback registered!\n"));
    }

  DosReleaseMutexSem(hmtxGlucb);

  return 0;
}

/*****************************************************************************/
/*                                                                           */
/* OS2glucbRegisterCallback sets up a memory page with machine code.         */
/*                                                                           */
/* When called from GLU the object type is GLUCB_TESS, GLUCB_NURBS or        */
/* GLUCB_QUADRIC and the parameters are self explanatory. The number of      */
/* params on the stack is determined by this code. If the function is called */
/* by GLUT the objtype is GLUTCB_xxxx, and the object points to the WINDOW   */
/* the resistration was required for.                                        */
/*                                                                           */
/*****************************************************************************/

GLUOS2CALLBACK OS2glucbRegisterCallback(GLbyte  objtype,
                                        void   *object,
                                        GLenum  which,
                                        GLUWINCALLBACK CallBackFunc)
{
  dprintf(("GLUCALLBACK: Registering Callback!\n"));

  switch(objtype)
    {
      case GLUCB_NURBS:
        switch (which)
          {
            case GLU_ERROR:
              return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,1);

            default:                  /* All legal callback values defined!*/
              return 0;
          }
        break;

      case GLUCB_QUADRIC:
        switch (which)
          {
            case GLU_ERROR:
              return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,1);

            default:                  /* All legal callback values defined!*/
              return 0;
          }
        break;

      case GLUCB_TESS:
        switch (which)
          {
            case GLU_TESS_BEGIN:
            case GLU_TESS_VERTEX:
            case GLU_TESS_ERROR:
            case GLU_TESS_EDGE_FLAG:
            case GLU_TESS_COMBINE:
            case GLU_TESS_BEGIN_DATA:
            case GLU_TESS_VERTEX_DATA:
            case GLU_TESS_END_DATA:
            case GLU_TESS_ERROR_DATA:
            case GLU_TESS_EDGE_FLAG_DATA:
            case GLU_TESS_COMBINE_DATA:
              return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,1);

            case GLU_TESS_END:
              return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,0);

            default:                  /* All legal callback values defined!*/
              return 0;
          }
        break;

      case GLUTCB_DISPLAY:
      case GLUTCB_IDLE:
        return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,0);

      case GLUTCB_MENUSTATE:
      case GLUTCB_VISIBILITY:
        return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,1);

      case GLUTCB_PASSIVEMOTION:
      case GLUTCB_RESHAPE:
      case GLUTCB_MOTION:
        return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,2);

      case GLUTCB_KEYBOARD:
      case GLUTCB_SPECIAL:
        return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,3);

      case GLUTCB_MOUSE:
        return OS2glucbGenerateCallbackFunc(objtype,object,which,CallBackFunc,4);

      case GLUTCB_MENU:  /* Special case - do not deregister existing ones!*/
      case GLUTCB_TIMER:/* Timers cannot be deregistered - simply add one !*/
        {
          GLUCB *glucb;

          DosRequestMutexSem(hmtxGlucb,SEM_INDEFINITE_WAIT);

          glucb=OS2glucbCreateGlucb(GLUTCB_TIMER,0,0,1);

          OS2glucbSetGlucbAddress(glucb,CallBackFunc);

          DosReleaseMutexSem(hmtxGlucb);

          return glucb->entryAddress;
        }

      default:
        dprintf(("GLUCALLBACK: Default action in SWITCH!\n"));
        break;
    }

  return 0;
}

void OS2glucbDestroyCallbackForObject(GLbyte  objtype,
                                      void   *object)
{
  GLUCB *glucb,*glucbNext;

  DosRequestMutexSem(hmtxGlucb,SEM_INDEFINITE_WAIT);

  glucb=Glucbs;

  if(!glucb)
    return;

  do{
      glucbNext=glucb->Next;

      if(objtype==GLUT_OBJECT)
        {
          if(glucb->object==object)
            OS2glucbDestroyGlucb(glucb);
        }
      else
        if(glucb->objtype==objtype &&
           glucb->object==object)
          OS2glucbDestroyGlucb(glucb);

      glucb=glucbNext;
    }while(glucb);

  DosReleaseMutexSem(hmtxGlucb);
}

void OS2glucbDestroyCallbackForObjectWhich(GLbyte  objtype,
                                           void   *object,
                                           GLenum  which)
{
  GLUCB *glucb,*glucbNext;

  DosRequestMutexSem(hmtxGlucb,SEM_INDEFINITE_WAIT);

  glucb=Glucbs;

  if(!glucb)
    return;

  do{
      glucbNext=glucb->Next;

      if(glucb->objtype==objtype &&
         glucb->object==object &&
         glucb->which==which)
        OS2glucbDestroyGlucb(glucb);

      glucb=glucbNext;
    }while(glucb);

  DosReleaseMutexSem(hmtxGlucb);
}

void OS2glucbSetWhich(GLUOS2CALLBACK pfn,GLenum which)
{
  GLUCB *glucb;

  DosRequestMutexSem(hmtxGlucb,SEM_INDEFINITE_WAIT);

  glucb=Glucbs;

  while(glucb)
    {
      if(glucb->entryAddress==pfn)

        {
          glucb->which=which;

          DosReleaseMutexSem(hmtxGlucb);
          return;
        }

      glucb=glucb->Next;
    }

  DosReleaseMutexSem(hmtxGlucb);
}
