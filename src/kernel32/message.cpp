/* $Id: message.cpp,v 1.14 2003-07-28 11:35:31 sandervl Exp $ */

/*
 * Win32 message API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (ported WINE code)
 *
 * Original WINE code (loader\resource.c)
 * WINE level: 991031
 *
 * Resources
 *
 * Copyright 1993 Robert J. Amstadt
 * Copyright 1995 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>
#include "unicode.h"
#include "heap.h"
#include "heapstring.h"

#define DBG_LOCALLOG    DBG_message
#include "dbglocal.h"

typedef VOID (* WIN32API WVSPRINTFAPROC)(LPSTR,LPCSTR,va_list);
WVSPRINTFAPROC wvsprintfAProc = NULL;

BOOL LoadwvsprintfA(VOID)
{
  //CB: load wvsprintfA dynamic to avoid problems with crosslinked DLL's
  if (!wvsprintfAProc)
  {
    HMODULE hUser32 = LoadLibraryA("USER32.DLL");

    wvsprintfAProc = (WVSPRINTFAPROC)GetProcAddress(hUser32,"wvsprintfA");
    FreeLibrary(hUser32);
  }

  return wvsprintfAProc != NULL;
}

//******************************************************************************
//******************************************************************************
int LoadMessageA(HINSTANCE instance, UINT id, WORD lang,
                 LPSTR buffer, int buflen )
{
    HGLOBAL     hmem;
    HRSRC       hrsrc;
    BYTE        *p;
    int         nrofentries,i,slen;
    struct      _subentry {
        DWORD   firstentry;
        DWORD   lastentry;
        DWORD   offset;
    } *se;
    MESSAGE_RESOURCE_ENTRY *stre;

    /*FIXME: I am not sure about the '1' ... But I've only seen those entries*/
    hrsrc = FindResourceW(instance,(LPWSTR)1,RT_MESSAGELISTW);
    if (!hrsrc) return 0;
    hmem = LoadResource(instance, hrsrc);
    if (!hmem) return 0;

    p = (BYTE *)LockResource(hmem);
    nrofentries = *(DWORD*)p;
    stre = NULL;
    se = (struct _subentry*)(p+4);
    for (i=nrofentries;i--;) {
        if ((id>=se->firstentry) && (id<=se->lastentry)) {
            stre = (PMESSAGE_RESOURCE_ENTRY)(p+se->offset);
            id  -= se->firstentry;
            break;
        }
        se++;
    }
    if (!stre)
        return 0;
    for (i=id;i--;) {
        if (!(slen=stre->Length))
                return 0;
        stre = (PMESSAGE_RESOURCE_ENTRY)(((char*)stre)+slen);
    }
    slen=stre->Length;

    i = (std::min<unsigned int>)(buflen - 1, slen);
    if (buffer == NULL)
        return slen; /* different to LoadString */
    if (i>0) {
	if (stre->Flags & MESSAGE_RESOURCE_UNICODE)
            WideCharToMultiByte( CP_ACP, 0, (LPWSTR)stre->Text, -1, buffer, i, NULL, NULL );
	else
	    lstrcpynA(buffer, (LPSTR)stre->Text, i);
	buffer[i]=0;
    } else {
        if (buflen>1) {
            buffer[0]=0;
            return 0;
        }
    }
    return i;
}

/**********************************************************************
 *      LoadMessage32W  (internal)
 */
int LoadMessageW(HINSTANCE instance, UINT id, WORD lang,
                 LPWSTR buffer, int buflen )
{
    INT retval;
    LPSTR buffer2 = NULL;
    if (buffer) buffer2 = (char *)HeapAlloc(GetProcessHeap(), 0, buflen );
    retval = LoadMessageA(instance,id,lang,buffer2,buflen);
    if (buffer)
    {
        AsciiToUnicode(buffer2, buffer);
        HeapFree(GetProcessHeap(), 0, buffer2);
    }
    return retval;
}


/***********************************************************************
 *           FormatMessage32A   (KERNEL32.138)
 * FIXME: missing wrap,FROM_SYSTEM message-loading,
 */
DWORD WIN32API FormatMessageA(DWORD   dwFlags,
                              LPCVOID lpSource,
                              DWORD   dwMessageId,
                              DWORD   dwLanguageId,
                              LPSTR   lpBuffer,
                              DWORD   nSize,
                              LPDWORD args /* va_list *args */)
{
/* This implementation is completely dependant on the format of the va_list on x86 CPUs */
   LPSTR target,
         t;
   DWORD talloced;
   LPSTR from,
         f;
   DWORD width = dwFlags & FORMAT_MESSAGE_MAX_WIDTH_MASK;
   BOOL  eos = FALSE;

   dprintf(("KERNEL32: FormatMessageA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
            dwFlags,
            lpSource,
            dwMessageId,
            dwLanguageId,
            lpBuffer,
            nSize,
            args));

   if (width)
      dprintf(("KERNEL32: FormatMessageA - line wrapping not supported.\n"));

   from = NULL;
   if (dwFlags & FORMAT_MESSAGE_FROM_STRING)
      from = HEAP_strdupA( GetProcessHeap(), 0, (LPSTR)lpSource);

   if (dwFlags & FORMAT_MESSAGE_FROM_SYSTEM)
   {
      INT bufsize;
      HMODULE hmodule;

      hmodule = GetModuleHandleA("kernel32");
      bufsize=LoadMessageA(hmodule,dwMessageId,dwLanguageId,NULL,100);
      if (bufsize)
      {
        from = (char*)HeapAlloc( GetProcessHeap(), 0, bufsize + 1 );
        LoadMessageA(hmodule,dwMessageId,dwLanguageId,from,bufsize+1);
      }
   }

   if (dwFlags & FORMAT_MESSAGE_FROM_HMODULE)
   {
     INT   bufsize;

//SvL/MN: VACPPWIN uses message ids > 64k!!
#ifndef __WIN32OS2__
     dwMessageId &= 0xFFFF;
#endif
     bufsize=LoadMessageA((HMODULE)lpSource,dwMessageId,dwLanguageId,NULL,100);
     if (bufsize)
     {
       from = (char*)HeapAlloc( GetProcessHeap(), 0, bufsize + 1 );
       LoadMessageA((HMODULE)lpSource,dwMessageId,dwLanguageId,from,bufsize+1);
     }
   }

   target   = (char*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, 100);
   t  = target;
   talloced= 100;

#define ADD_TO_T(c) \
   *t++=c;\
   if (t-target == talloced) {\
      target   = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,talloced*2);\
      t  = target+talloced;\
      talloced*=2;\
   }

   if (from)
   {
     f=from;

     while (*f && !eos)
     {
       if (*f=='%')
       {
         int   insertnr;
         char  *fmtstr,
               *sprintfbuf,
               *x,
               *lastf;
         DWORD *argliststart;

         fmtstr = NULL;
         lastf  = f;
         f++;

         if (!*f)
         {
           ADD_TO_T('%');
           continue;
         }

         if (dwFlags & FORMAT_MESSAGE_IGNORE_INSERTS) {
            /* Ignore inserts, except for '%0' - maybe more? */
            if (*f == '0') {
               eos = TRUE;
               f++;
               continue;
            }
            else {
               ADD_TO_T('%');
               continue;
            }
         }

         switch (*f)
         {
           case '1': case '2': case '3': case '4': case '5':
           case '6': case '7': case '8': case '9':
             insertnr=*f-'0';
             switch (f[1])
             {
               case '0':case '1':case '2':case '3':
               case '4':case '5':case '6':case '7':
               case '8':case '9':
                 f++;
                 insertnr=insertnr*10+*f-'0';
                 f++;
                 break;

               default:
                  f++;
                  break;
             }

             if (*f=='!')
             {
               f++;
               if (NULL!=(x=strchr(f,'!')))
               {
                 *x='\0';
                 fmtstr = (char*)HeapAlloc(GetProcessHeap(),0,strlen(f)+2);
                 sprintf(fmtstr,"%%%s",f);
                 f=x+1;
               }
               else
               {
                 fmtstr = (char*)HeapAlloc(GetProcessHeap(),0,strlen(f));
                 sprintf(fmtstr,"%%%s",f);
                 f+=strlen(f); /*at \0*/
               }
             }
             else
               if(!args)
                 break;
               else
                  fmtstr=HEAP_strdupA(GetProcessHeap(),0,"%s");

             if (args)
             {
               if (dwFlags & FORMAT_MESSAGE_ARGUMENT_ARRAY)
                 argliststart=args+insertnr-1;
               else
                 argliststart=(*(DWORD**)args)+insertnr-1;

               if (fmtstr[strlen(fmtstr)-1]=='s' && argliststart[0])
                 sprintfbuf = (char*)HeapAlloc(GetProcessHeap(),0,strlen((LPSTR)argliststart[0])+1);
               else
                 sprintfbuf = (char*)HeapAlloc(GetProcessHeap(),0,100);

               /* CMF - This makes a BIG assumption about va_list */
               if (LoadwvsprintfA()) wvsprintfAProc(sprintfbuf,fmtstr,(va_list)argliststart);
               x=sprintfbuf;
               while (*x)
               {
                 ADD_TO_T(*x++);
               }

               HeapFree(GetProcessHeap(),0,sprintfbuf);
             }
             else
             {
               /* NULL args - copy formatstr
                * (probably wrong)
                */
               while ((lastf<f)&&(*lastf))
               {
                 ADD_TO_T(*lastf++);
               }
             }

             HeapFree(GetProcessHeap(),0,fmtstr);
             break;

           case 'n':
             ADD_TO_T('\r');
             ADD_TO_T('\n');
             f++;
             break;

           case '0':
             eos = TRUE;
             f++;
             break;

           default:ADD_TO_T(*f++)
             break;
         }
       } else
       { /* '\n' or '\r' gets mapped to "\r\n" */
         if(*f == '\n' || *f == '\r')
         {
           ADD_TO_T('\r');
           ADD_TO_T('\n');
           if(*f++ == '\r' && *f == '\n')
             f++;
         } else
         {
           ADD_TO_T(*f++);
         }
       }
     }
     *t='\0';
  }

  talloced = strlen(target)+1;
  if (nSize && talloced<nSize)
  {
    target = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,nSize);
  }

  //TRACE("-- %s\n",debugstr_a(target));

  if (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER)
  {
    /* nSize is the MINIMUM size */
    *((LPVOID*)lpBuffer) = (LPVOID)LocalAlloc(GMEM_ZEROINIT,talloced);
    memcpy(*(LPSTR*)lpBuffer,target,talloced);
  }
  else {
    strncpy(lpBuffer,target,nSize);
    lpBuffer[nSize-1] = 0;
  }

  HeapFree(GetProcessHeap(),0,target);
  if (from)
    HeapFree(GetProcessHeap(),0,from);

  dprintf(("gives: %s", (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ?
           *(LPSTR*)lpBuffer : lpBuffer));

  return (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ?
         strlen(*(LPSTR*)lpBuffer):
         strlen(lpBuffer);
}
#undef ADD_TO_T


/***********************************************************************
 *           FormatMessage32W   (KERNEL32.138)
 */
DWORD WINAPI FormatMessageW(DWORD   dwFlags,
                            LPCVOID lpSource,
                            DWORD   dwMessageId,
                            DWORD   dwLanguageId,
                            LPWSTR  lpBuffer,
                            DWORD   nSize,
                            LPDWORD args /* va_list *args */ )
{
/* This implementation is completely dependant on the format of the va_list on x86 CPUs */
   LPSTR target,t;
   DWORD talloced;
   LPSTR from,f;
   DWORD width = dwFlags & FORMAT_MESSAGE_MAX_WIDTH_MASK;
   BOOL  eos = FALSE;

   dprintf(("KERNEL32: FormatMessageW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
            dwFlags,
            lpSource,
            dwMessageId,
            dwLanguageId,
            lpBuffer,
            nSize,
            args));

   if (width)
     dprintf(("KERNEL32: FormatMessageW - line wrapping not supported.\n"));

   from = NULL;
   if (dwFlags & FORMAT_MESSAGE_FROM_STRING)
      from = HEAP_strdupWtoA(GetProcessHeap(),0,(LPWSTR)lpSource);

   if (dwFlags & FORMAT_MESSAGE_FROM_SYSTEM)
   {
      INT bufsize;
      HMODULE hmodule;

      hmodule = GetModuleHandleA("kernel32");
      bufsize=LoadMessageA(hmodule,dwMessageId,dwLanguageId,NULL,100);
      if (bufsize)
      {
        from = (char*)HeapAlloc( GetProcessHeap(), 0, bufsize + 1 );
        LoadMessageA(hmodule,dwMessageId,dwLanguageId,from,bufsize+1);
      }
   }

   if (dwFlags & FORMAT_MESSAGE_FROM_HMODULE)
   {
      INT   bufsize;

//SvL/MN: VACPPWIN uses message ids > 64k!!
#ifndef __WIN32OS2__
      dwMessageId &= 0xFFFF;
#endif
      bufsize=LoadMessageA((HMODULE)lpSource,dwMessageId,dwLanguageId,NULL,100);
      if (bufsize)
      {
        from = (char*)HeapAlloc( GetProcessHeap(), 0, bufsize + 1 );
        LoadMessageA((HMODULE)lpSource,dwMessageId,dwLanguageId,from,bufsize+1);
      }
   }

   target   = (char*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, 100 );
   t  = target;
   talloced= 100;

#define ADD_TO_T(c) \
   *t++=c;\
   if (t-target == talloced) {\
      target   = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,talloced*2);\
      t  = target+talloced;\
      talloced*=2;\
   }

   if (from) {
      f=from;
      while (*f && !eos) {
         if (*f=='%') {
            int   insertnr;
            char  *fmtstr,*sprintfbuf,*x;
            DWORD *argliststart;

            fmtstr = NULL;
            f++;
            if (!*f) {
               ADD_TO_T('%');
               continue;
            }

            if (dwFlags & FORMAT_MESSAGE_IGNORE_INSERTS) {
               /* Ignore inserts, except for '%0' - maybe more? */
               if (*f == '0') {
                  eos = TRUE;
                  f++;
                  continue;
               }
               else {
                  ADD_TO_T('%');
                  continue;
               }
            }

            switch (*f) {
            case '1':case '2':case '3':case '4':case '5':
            case '6':case '7':case '8':case '9':
               insertnr=*f-'0';
               switch (f[1]) {
               case '0':case '1':case '2':case '3':
               case '4':case '5':case '6':case '7':
               case '8':case '9':
                  f++;
                  insertnr=insertnr*10+*f-'0';
                  f++;
                  break;
               default:
                  f++;
                  break;
               }
               if (*f=='!') {
                  f++;
                  if (NULL!=(x=strchr(f,'!')))
                                                {
                                                    *x='\0';
                                                    fmtstr=(char*)HeapAlloc( GetProcessHeap(), 0, strlen(f)+2);
                                         sprintf(fmtstr,"%%%s",f);
                                         f=x+1;
                  } else {
                                         fmtstr=(char*)HeapAlloc(GetProcessHeap(),0,strlen(f));
                                         sprintf(fmtstr,"%%%s",f);
                                         f+=strlen(f); /*at \0*/
                  }
               } else
                       if(!args)
                    break;
               else
                  fmtstr=HEAP_strdupA( GetProcessHeap(),0,"%s");
               if (dwFlags & FORMAT_MESSAGE_ARGUMENT_ARRAY)
                  argliststart=args+insertnr-1;
               else
                  argliststart=(*(DWORD**)args)+insertnr-1;

               if (fmtstr[strlen(fmtstr)-1]=='s' && argliststart[0]) {
                  DWORD                  xarr[3];

                  xarr[0]=(DWORD)HEAP_strdupWtoA(GetProcessHeap(),0,(LPWSTR)(*(argliststart+0)));
                  /* possible invalid pointers */
                  xarr[1]=*(argliststart+1);
                  xarr[2]=*(argliststart+2);
                  sprintfbuf=(char*)HeapAlloc(GetProcessHeap(),0,lstrlenW((LPWSTR)argliststart[0])*2+1);

                  /* CMF - This makes a BIG assumption about va_list */
                  vsprintf(sprintfbuf, fmtstr, (va_list) xarr);
               } else {
                  sprintfbuf=(char*)HeapAlloc(GetProcessHeap(),0,100);

                  /* CMF - This makes a BIG assumption about va_list */
                  if (LoadwvsprintfA()) wvsprintfAProc(sprintfbuf,fmtstr,(va_list)argliststart);
               }
               x=sprintfbuf;
               while (*x) {
                  ADD_TO_T(*x++);
               }
               HeapFree(GetProcessHeap(),0,sprintfbuf);
               HeapFree(GetProcessHeap(),0,fmtstr);
               break;
            case 'n':
               ADD_TO_T('\r');
               ADD_TO_T('\n');
               f++;
               break;
            case '0':
               eos = TRUE;
               f++;
               break;
            default:ADD_TO_T(*f++)
               break;

          }
        } else
        { /* '\n' or '\r' gets mapped to "\r\n" */
          if(*f == '\n' || *f == '\r')
          {
            ADD_TO_T('\r');
            ADD_TO_T('\n');
            if(*f++ == '\r' && *f == '\n')
              f++;
          } else
          {
            ADD_TO_T(*f++);
          }
        }
      }
      *t='\0';
   }
   talloced = strlen(target)+1;
   if (nSize && talloced<nSize)
      target = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,nSize);
   if (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) {
      /* nSize is the MINIMUM size */
      *((LPVOID*)lpBuffer) = (LPVOID)LocalAlloc(GMEM_ZEROINIT,talloced*2+2);
      lstrcpynAtoW(*(LPWSTR*)lpBuffer,target,talloced);
   } else
      lstrcpynAtoW(lpBuffer,target,nSize);
   HeapFree(GetProcessHeap(),0,target);
   if (from) HeapFree(GetProcessHeap(),0,from);
   return (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ?
         lstrlenW(*(LPWSTR*)lpBuffer):
         lstrlenW(lpBuffer);
}
#undef ADD_TO_T


