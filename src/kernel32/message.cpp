/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 message API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (ported WINE code)
 *
 * Original WINE code (loader\resource.c)
 *
 * Resources
 *
 * Copyright 1993 Robert J. Amstadt
 * Copyright 1995 Alexandre Julliard
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "unicode.h"

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
    struct      _stringentry {
        WORD    len;
        WORD    unknown;
        CHAR    str[1];
    } *stre;

    /*FIXME: I am not sure about the '1' ... But I've only seen those entries*/
    hrsrc = FindResourceA(instance,(LPCSTR)1,(LPCSTR)RT_MESSAGELISTA);
    if (!hrsrc) return 0;
    hmem = LoadResource(instance, hrsrc);
    if (!hmem) return 0;

    p = (BYTE *)LockResource(hmem);
    nrofentries = *(DWORD*)p;
    stre = NULL;
    se = (struct _subentry*)(p+4);
    for (i=nrofentries;i--;) {
        if ((id>=se->firstentry) && (id<=se->lastentry)) {
            stre = (struct _stringentry*)(p+se->offset);
            id  -= se->firstentry;
            break;
        }
        se++;
    }
    if (!stre)
        return 0;
    for (i=id;i--;) {
        if (!(slen=stre->len))
                return 0;
        stre = (struct _stringentry*)(((char*)stre)+slen);
    }
    slen=stre->len;

    i = min(buflen - 1, slen);
    if (buffer == NULL)
        return slen; /* different to LoadString */
    if (i>0) {
        strncpy(buffer,stre->str,i);
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

#if 1
/***********************************************************************
 *           FormatMessage32A   (KERNEL32.138)
 * FIXME: missing wrap,FROM_SYSTEM message-loading,
 */
DWORD WIN32API FormatMessageA(
        DWORD   dwFlags,
        LPCVOID lpSource,
        DWORD   dwMessageId,
        DWORD   dwLanguageId,
        LPSTR   lpBuffer,
        DWORD   nSize,
        LPDWORD args /* va_list *args */
) {
        LPSTR   target,t;
        DWORD   talloced;
        LPSTR   from,f;
        DWORD   width = dwFlags & FORMAT_MESSAGE_MAX_WIDTH_MASK;
        DWORD   nolinefeed = 0;

        dprintf(("FormatMessageA\n"));

        from = NULL;
        if (dwFlags & FORMAT_MESSAGE_FROM_STRING) {
                from = (char *)HeapAlloc(GetProcessHeap(),0, strlen((char *)lpSource)+1);
                UnicodeToAscii((LPWSTR)lpSource, from);
        }
        if (dwFlags & FORMAT_MESSAGE_FROM_SYSTEM) {
                from = (char *)HeapAlloc(GetProcessHeap(),0,200 );
                sprintf(from,"Systemmessage, messageid = 0x%08lx\n",dwMessageId);
        }
        if (dwFlags & FORMAT_MESSAGE_FROM_HMODULE) {
                int bufsize;

                dwMessageId &= 0xFFFF;
                bufsize=LoadMessageA(0,dwMessageId,dwLanguageId,NULL,100);
                if (bufsize) {
                        from = (char *)HeapAlloc(GetProcessHeap(), 0, bufsize + 1 );
                        LoadMessageA(0,dwMessageId,dwLanguageId,from,bufsize+1);
                }
        }
        target  = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 100);
        t       = target;
        talloced= 100;

#define ADD_TO_T(c) \
        *t++=c;\
        if (t-target == talloced) {\
                target  = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,talloced*2);\
                t       = target+talloced;\
                talloced*=2;\
        }

        if (from) {
                f=from;
                while (*f) {
                        if (*f=='%') {
                                int     insertnr;
                                char    *fmtstr,*sprintfbuf,*x,*lastf;
                                DWORD   *argliststart;

                                fmtstr = NULL;
                                lastf = f;
                                f++;
                                if (!*f) {
                                        ADD_TO_T('%');
                                        continue;
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
                                                if (NULL!=(x=strchr(f,'!'))) {
                                                        *x='\0';
                                                        fmtstr=(char *)HeapAlloc(GetProcessHeap(),0,strlen(f)+2);
                                                        sprintf(fmtstr,"%%%s",f);
                                                        f=x+1;
                                                } else {
                                                        fmtstr=(char *)HeapAlloc(GetProcessHeap(),0,strlen(f));
                                                        sprintf(fmtstr,"%%%s",f);
                                                        f+=strlen(f); /*at \0*/
                                                }
                                        } else
                                                fmtstr=strdup("%s");
                                        if (args) {
                                                if (dwFlags & FORMAT_MESSAGE_ARGUMENT_ARRAY)
                                                        argliststart=args+insertnr-1;
                                                else
                                                    argliststart=(*(DWORD**)args)+insertnr-1;

                                                if (fmtstr[strlen(fmtstr)]=='s')
                                                        sprintfbuf=(char *)HeapAlloc(GetProcessHeap(),0,strlen((LPSTR)argliststart[0])+1);
                                                else
                                                        sprintfbuf=(char *)HeapAlloc(GetProcessHeap(),0,100);

                                                /* CMF - This makes a BIG assumption about va_list */
                                                vsprintf(sprintfbuf, fmtstr, (va_list) argliststart);
                                                x=sprintfbuf;
                                                while (*x) {
                                                        ADD_TO_T(*x++);
                                                }
                                                HeapFree(GetProcessHeap(),0,sprintfbuf);
                                        } else {
                                                /* NULL args - copy formatstr
                                                 * (probably wrong)
                                                 */
                                                while (lastf<f) {
                                                        ADD_TO_T(*lastf++);
                                                }
                                        }
                                        HeapFree(GetProcessHeap(),0,fmtstr);
                                        break;
                                case 'n':
                                        /* FIXME: perhaps add \r too? */
                                        ADD_TO_T('\n');
                                        f++;
                                        break;
                                case '0':
                                        nolinefeed=1;
                                        f++;
                                        break;
                                default:ADD_TO_T(*f++)
                                        break;

                                }
                        } else {
                                ADD_TO_T(*f++)
                        }
                }
                *t='\0';
        }
        if (!nolinefeed && t[-1]!='\n')
                ADD_TO_T('\n');
        talloced = strlen(target)+1;
        if (nSize && talloced<nSize) {
                target = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,nSize);
        }
        if (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) {
                /* nSize is the MINIMUM size */
                *((LPVOID*)lpBuffer) = (LPVOID)LocalAlloc(GMEM_ZEROINIT,talloced);
                memcpy(*(LPSTR*)lpBuffer,target,talloced);
        } else
                strncpy(lpBuffer,target,nSize);
        HeapFree(GetProcessHeap(),0,target);
        if (from) HeapFree(GetProcessHeap(),0,from);
        dprintf(("FormatMessageA returned %s\n",
                                (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ?
                                *(LPSTR*)lpBuffer:
                                lpBuffer));
        return (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ?
                        strlen(*(LPSTR*)lpBuffer):
                        strlen(lpBuffer);
}
#undef ADD_TO_T

/***********************************************************************
 *           FormatMessage32W   (KERNEL32.138)
 */
DWORD WIN32API FormatMessageW(
        DWORD   dwFlags,
        LPCVOID lpSource,
        DWORD   dwMessageId,
        DWORD   dwLanguageId,
        LPWSTR  lpBuffer,
        DWORD   nSize,
        LPDWORD args /* va_list *args */
) {
        LPSTR   target,t;
        DWORD   talloced;
        LPSTR   from,f;
        DWORD   width = dwFlags & FORMAT_MESSAGE_MAX_WIDTH_MASK;
        DWORD   nolinefeed = 0;

        dprintf(("FormatMessageW\n"));

        from = NULL;
        if (dwFlags & FORMAT_MESSAGE_FROM_STRING) {
                from = (char *)HeapAlloc(GetProcessHeap(),0, UniStrlen((UniChar*)lpSource)+1);
                UnicodeToAscii((LPWSTR)lpSource, from);
        }
        if (dwFlags & FORMAT_MESSAGE_FROM_SYSTEM) {
                /* gather information from system message tables ... */
                from = (char *)HeapAlloc( GetProcessHeap(),0,200 );
                sprintf(from,"Systemmessage, messageid = 0x%08lx\n",dwMessageId);
        }
        if (dwFlags & FORMAT_MESSAGE_FROM_HMODULE) {
                int bufsize;

                dwMessageId &= 0xFFFF;
                bufsize=LoadMessageW(0,dwMessageId,dwLanguageId,NULL,100);
                if (bufsize)
                {
                    from = (char *)HeapAlloc( GetProcessHeap(), 0, bufsize + 1 );
                    LoadMessageW(0,dwMessageId,dwLanguageId,(LPWSTR)from,bufsize+1);
                }
        }
        target  = (char *)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, 100 );
        t       = target;
        talloced= 100;

#define ADD_TO_T(c) \
        *t++=c;\
        if (t-target == talloced) {\
                target  = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,talloced*2);\
                t       = target+talloced;\
                talloced*=2;\
        }

        if (from) {
                f=from;
                while (*f) {
                        if (*f=='%') {
                                int     insertnr;
                                char    *fmtstr,*sprintfbuf,*x;
                                DWORD   *argliststart;

                                fmtstr = NULL;
                                f++;
                                if (!*f) {
                                        ADD_TO_T('%');
                                        continue;
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
                                                    fmtstr=(char *)HeapAlloc( GetProcessHeap(), 0, strlen(f)+2);
                                                        sprintf(fmtstr,"%%%s",f);
                                                        f=x+1;
                                                } else {
                                                        fmtstr=(char *)HeapAlloc(GetProcessHeap(),0,strlen(f));
                                                        sprintf(fmtstr,"%%%s",f);
                                                        f+=strlen(f); /*at \0*/
                                                }
                                        } else
                                                fmtstr=strdup("%s");
                                        if (dwFlags & FORMAT_MESSAGE_ARGUMENT_ARRAY)
                                                argliststart=args+insertnr-1;
                                        else
                                                argliststart=(*(DWORD**)args)+insertnr-1;

                                        if (fmtstr[strlen(fmtstr)]=='s') {
                                                DWORD   xarr[3];

                                                xarr[0]=(DWORD)strdup((char *)(*(argliststart+0)));
                                                /* possible invalid pointers */
                                                xarr[1]=*(argliststart+1);
                                                xarr[2]=*(argliststart+2);
                                                sprintfbuf = (char *)(char *)HeapAlloc(GetProcessHeap(),0,UniStrlen((UniChar*)argliststart[0])*2+1);

                                                /* CMF - This makes a BIG assumption about va_list */
                                                vsprintf(sprintfbuf, fmtstr, (va_list) xarr);
                                        } else {
                                                sprintfbuf = (char *)HeapAlloc(GetProcessHeap(),0,100);

                                                /* CMF - This makes a BIG assumption about va_list */
                                                vsprintf(sprintfbuf, fmtstr, (va_list) argliststart);
                                        }
                                        x=sprintfbuf;
                                        while (*x) {
                                                ADD_TO_T(*x++);
                                        }
                                        HeapFree(GetProcessHeap(),0,sprintfbuf);
                                        HeapFree(GetProcessHeap(),0,fmtstr);
                                        break;
                                case 'n':
                                        /* FIXME: perhaps add \r too? */
                                        ADD_TO_T('\n');
                                        f++;
                                        break;
                                case '0':
                                        nolinefeed=1;
                                        f++;
                                        break;
                                default:ADD_TO_T(*f++)
                                        break;

                                }
                        } else {
                                ADD_TO_T(*f++)
                        }
                }
                *t='\0';
        }
        if (!nolinefeed && t[-1]!='\n')
                ADD_TO_T('\n');
        talloced = strlen(target)+1;
        if (nSize && talloced<nSize)
                target = (char*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,target,nSize);
        if (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) {
                /* nSize is the MINIMUM size */
                *((LPVOID*)lpBuffer) = (LPVOID)LocalAlloc(GMEM_ZEROINIT,talloced*2+2);
                AsciiToUnicode(target, *(LPWSTR*)lpBuffer);
        }
        else    AsciiToUnicode(target, lpBuffer);

        dprintf(("FormatMessageW returned %s\n", target));

        HeapFree(GetProcessHeap(),0,target);
        if (from) HeapFree(GetProcessHeap(),0,from);
        return (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ?
                        UniStrlen((UniChar*)*lpBuffer):
                        UniStrlen((UniChar*)lpBuffer);
}
#else
//******************************************************************************
//DWORD dwFlags;        /* source and processing options        */
//LPCVOID lpSource;     /* address of message source    */
//DWORD dwMessageId;    /* requested message identifier */
//DWORD dwLanguageId;   /* language identifier for requested message    */
//LPTSTR lpBuffer;      /* address of message buffer    */
//DWORD nSize;          /* maximum size of message buffer       */
//va_list * Arguments;  /* address of array of message inserts  */
//******************************************************************************
DWORD WIN32API FormatMessageA(DWORD  dwFlags, LPCVOID lpSource,
                                 DWORD  dwMessageId, DWORD dwLanguageId,
                                 LPTSTR lpBuffer, DWORD nSize, va_list *Arguments)
{
 char *msgBuffer;

  dprintf(("FormatMessageA Not Implemented\n"));

  if(dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) {
        msgBuffer          = (char *)LocalAlloc(LMEM_FIXED, nSize);
        *(DWORD *)lpBuffer = (DWORD)msgBuffer;
  }
  else  msgBuffer = lpBuffer;

  if(nSize >= 40) {
        //SvL: I always wanted to do this :)
        strncpy(msgBuffer, "Windows Error 666: Reserved for future mistakes\n", 40);
  }
  else  *msgBuffer = 0;
  return(strlen(msgBuffer));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API FormatMessageW(DWORD  dwFlags, LPCVOID lpSource,
                                 DWORD  dwMessageId, DWORD dwLanguageId,
                                 LPWSTR lpBuffer, DWORD nSize, va_list *Arguments)
{
 WCHAR *msgBuffer;

  dprintf(("FormatMessageW Not Implemented\n"));

  if(dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) {
        msgBuffer          = (WCHAR *)LocalAlloc(LMEM_FIXED, nSize*sizeof(USHORT));
        *(DWORD *)lpBuffer = (DWORD)msgBuffer;
  }
  else  msgBuffer = lpBuffer;

  if(nSize >= 40) {
        //SvL: I always wanted to do this :)
        AsciiToUnicode("Windows Error 666: Reserved for future mistakes\n", msgBuffer);
  }
  else  *msgBuffer = 0;
  return(UniStrlen((UniChar*)msgBuffer));
}
//******************************************************************************
//******************************************************************************
#endif
