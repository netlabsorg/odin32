/* $Id: lz32.cpp,v 1.10 2005-01-15 22:17:58 sao2l02 Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * LZ Decompression functions
 *
 * Copyright 1996 Marcus Meissner
 * Copyright 1999 Patrick Haller
 *
 * FIXME: return values might be wrong
 */

#include <string.h>
#include <ctype.h>
#include <odin.h>
#include <os2sel.h>
#include <odinwrap.h>
#include <windef.h>
#include <winbase.h>
#include <heap.h>
#include <heapstring.h>
#include <lz32.h>
#include "debugtools.h"
#include <misc.h>

ODINDEBUGCHANNEL(LZ32)

#define SystemHeap GetProcessHeap()


/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

/* The readahead length of the decompressor. Reading single bytes
 * using _lread() would be SLOW.
 */
#define  GETLEN   2048


/****************************************************************************
 * Structures                                                               *
 ****************************************************************************/

/* Format of first 14 byte of LZ compressed file */
struct lzfileheader {
   BYTE  magic[8];
   BYTE  compressiontype;
   CHAR  lastchar;
   DWORD reallength;
};
static BYTE LZMagic[8]={'S','Z','D','D',0x88,0xf0,0x27,0x33};

struct lzstate {
   HFILE realfd;                         /* the real filedescriptor */
   CHAR  lastchar;                       /* the last char of the filename */

   DWORD reallength;                     /* the decompressed length of the file */
   DWORD realcurrent;                    /* the position the decompressor currently is */
   DWORD realwanted;                     /* the position the user wants to read from */

   BYTE  table[0x1000];                  /* the rotating LZ table */
   UINT  curtabent;                      /* CURrent TABle ENTry */

   BYTE  stringlen;                      /* length and position of current string */
   DWORD stringpos;                      /* from stringtable */


   WORD  bytetype;                       /* bitmask within blocks */

   BYTE  *get;    /* GETLEN bytes */
   DWORD getcur;                         /* current read */
   DWORD getlen;                         /* length last got */
};

#define MAX_LZSTATES 16
static struct lzstate *lzstates[MAX_LZSTATES];

#define IS_LZ_HANDLE(h) (((h) >= 0x400) && ((h) < 0x400+MAX_LZSTATES))
#define GET_LZ_STATE(h) (IS_LZ_HANDLE(h) ? lzstates[(h)-0x400] : NULL)

/* reads one compressed byte, including buffering */
#define GET(lzs,b)                       _lzget(lzs,&b)
#define GET_FLUSH(lzs)                   lzs->getcur=lzs->getlen;


/****************************************************************************
 * Internal Prototypes                                                      *
 ****************************************************************************/

static int _lzget(struct lzstate *lzs,BYTE *b);
static INT read_header(HFILE fd,struct lzfileheader *head);


/****************************************************************************
 * Implementation                                                           *
 ****************************************************************************/

static int _lzget(struct lzstate *lzs,BYTE *b)
{
   if (lzs->getcur<lzs->getlen) {
      *b    = lzs->get[lzs->getcur++];
      return      1;
   } else {
      int ret = _lread(lzs->realfd,lzs->get,GETLEN);
      if (ret==HFILE_ERROR)
         return HFILE_ERROR;
      if (ret==0)
         return 0;
      lzs->getlen = ret;
      lzs->getcur = 1;
      *b    = *(lzs->get);
      return 1;
   }
}


/* internal function, reads lzheader
 * returns BADINHANDLE for non filedescriptors
 * return 0 for file not compressed using LZ
 * return UNKNOWNALG for unknown algorithm
 * returns lzfileheader in *head
 */
static INT read_header(HFILE fd,struct lzfileheader *head)
{
   BYTE  buf[14];

   if (_llseek(fd,0,SEEK_SET)==-1)
      return LZERROR_BADINHANDLE;

   /* We can't directly read the lzfileheader struct due to
    * structure element alignment
    */
   if (_lread(fd,buf,14)<14)
      return 0;
   memcpy(head->magic,buf,8);
   memcpy(&(head->compressiontype),buf+8,1);
   memcpy(&(head->lastchar),buf+9,1);

   /* FIXME: consider endianess on non-intel architectures */
   memcpy(&(head->reallength),buf+10,4);

   if (memcmp(head->magic,LZMagic,8))
      return 0;
   if (head->compressiontype!='A')
      return LZERROR_UNKNOWNALG;
   return 1;
}


/***********************************************************************
 *           LZStart32   (LZ32.6)
 */

ODINFUNCTION0(INT,LZStart)
{
  dprintf(("LZ32: LZStart32()\n"));
  return 1;
}


/***********************************************************************
 *           LZInit32   (LZ32.2)
 *
 * initializes internal decompression buffers, returns lzfiledescriptor.
 * (return value the same as hfSrc, if hfSrc is not compressed)
 * on failure, returns error code <0
 * lzfiledescriptors range from 0x400 to 0x410 (only 16 open files per process)
 *
 * since _llseek uses the same types as libc.lseek, we just use the macros of
 *  libc
 */
ODINFUNCTION1(HFILE,LZInit,HFILE,hfSrc)
{
   struct   lzfileheader                 head;
   struct   lzstate                         *lzs;
   DWORD ret;
        int i;

   dprintf(("LZ32: LZInit(%08xh)\n",
            hfSrc));

   ret=read_header(hfSrc,&head);
   if (ret<=0) {
      _llseek(hfSrc,0,SEEK_SET);
      return ret?ret:hfSrc;
   }
        for (i = 0; i < MAX_LZSTATES; i++) if (!lzstates[i]) break;
        if (i == MAX_LZSTATES) return LZERROR_GLOBALLOC;

   lzstates[i] = lzs = (lzstate*)HeapAlloc( GetProcessHeap(), 0, sizeof(struct lzstate) );
   if (lzs == NULL) return LZERROR_GLOBALLOC;
  
   memset(lzs,'\0',sizeof(*lzs));
   lzs->realfd = hfSrc;
   lzs->lastchar  = head.lastchar;
   lzs->reallength = head.reallength;

   lzs->get = (BYTE*)HeapAlloc( GetProcessHeap(), 0, GETLEN );
   lzs->getlen = 0;
   lzs->getcur = 0;
  
   if (lzs->get == NULL) {
       HeapFree(GetProcessHeap(), 0, lzs);
       lzstates[i] = NULL;
       return LZERROR_GLOBALLOC;
   }
  
   /* Yes, preinitialize with spaces */
   memset(lzs->table,' ',0x1000);
   /* Yes, start 16 byte from the END of the table */
   lzs->curtabent = 0xff0;
   return 0x400 + i;
}


/***********************************************************************
 *           LZDone   (LZEXPAND.9) (LZ32.8)
 */
ODINPROCEDURE0(LZDone)
{
    TRACE("(void)\n");
}

/***********************************************************************
 *           GetExpandedName32A   (LZ32.9)
 *
 * gets the full filename of the compressed file 'in' by opening it
 * and reading the header
 *
 * "file." is being translated to "file"
 * "file.bl_" (with lastchar 'a') is being translated to "file.bla"
 * "FILE.BL_" (with lastchar 'a') is being translated to "FILE.BLA"
 */

ODINFUNCTION2(INT,GetExpandedNameA,LPCSTR,in,LPSTR,out)
{
   struct lzfileheader                   head;
   HFILE    fd;
   OFSTRUCT ofs;
   INT      fnislowercased,ret,len;
   LPSTR    s,t;

   dprintf(("LZ32: GetExpandedNameA(%s,%08xh)\n",
            in,
            out));

   fd=OpenFile(in,&ofs,OF_READ);
   if (fd==HFILE_ERROR)
      return (INT)(INT16)LZERROR_BADINHANDLE;
   strcpy(out,in);
   ret=read_header(fd,&head);
   if (ret<=0) {
      /* not a LZ compressed file, so the expanded name is the same
       * as the input name */
      _lclose(fd);
      return 1;
   }


   /* look for directory prefix and skip it. */
   s=out;
   while (NULL!=(t=strpbrk(s,"/\\:")))
      s=t+1;

   /* now mangle the basename */
   if (!*s) {
      /* FIXME: hmm. shouldn't happen? */
      WARN("Specified a directory or what? (%s)\n",in);
      _lclose(fd);
      return 1;
   }
   /* see if we should use lowercase or uppercase on the last char */
   fnislowercased=1;
   t=s+strlen(s)-1;
   while (t>=out) {
      if (!isalpha(*t)) {
         t--;
         continue;
      }
      fnislowercased=islower(*t);
      break;
   }
   if (isalpha(head.lastchar)) {
      if (fnislowercased)
         head.lastchar=tolower(head.lastchar);
      else
         head.lastchar=toupper(head.lastchar);
   }

   /* now look where to replace the last character */
   if (NULL!=(t=strchr(s,'.'))) {
      if (t[1]=='\0') {
         t[0]='\0';
      } else {
         len=strlen(t)-1;
         if (t[len]=='_')
            t[len]=head.lastchar;
      }
   } /* else no modification necessary */
   _lclose(fd);
   return 1;
}


/***********************************************************************
 *           GetExpandedName32W   (LZ32.11)
 */
ODINFUNCTION2(INT,GetExpandedNameW,LPCWSTR,in,LPWSTR,out)
{
   char  *xin,*xout;
   INT   ret;

   dprintf(("LZ32: GetExpandedNameW(%08xh,%08xh)\n",
            in,
            out));

   xout  = (char*)HeapAlloc( GetProcessHeap(), 0, lstrlenW(in)+3 );
   xin   = HEAP_strdupWtoA( GetProcessHeap(), 0, in );
   ret   = GetExpandedNameA(xin,xout);
   if (ret>0) lstrcpyAtoW(out,xout);
   HeapFree( GetProcessHeap(), 0, xin );
   HeapFree( GetProcessHeap(), 0, xout );
   return   ret;
}


/***********************************************************************
 *           LZRead32   (LZ32.4)
 */
ODINFUNCTION3(INT,LZRead,HFILE,fd,LPVOID,vbuf,UINT,toread)
{
   int   howmuch;
   BYTE  b,*buf;
   struct   lzstate                      *lzs;

   dprintf(("LZ32: LZRead(%08xh,%08xh,%08h)\n",
            fd,
            vbuf,
            toread));

   buf=(LPBYTE)vbuf;
   howmuch=toread;
   if (!(lzs = GET_LZ_STATE(fd))) return _lread(fd,buf,toread);

/* The decompressor itself is in a define, cause we need it twice
 * in this function. (the decompressed byte will be in b)
 */
#define DECOMPRESS_ONE_BYTE                             \
      if (lzs->stringlen) {                          \
         b     = lzs->table[lzs->stringpos];   \
         lzs->stringpos                  = (lzs->stringpos+1)&0xFFF;   \
         lzs->stringlen--;                        \
      } else {                                    \
         if (!(lzs->bytetype&0x100)) {         \
            if (1!=GET(lzs,b))                 \
               return toread-howmuch;       \
            lzs->bytetype = b|0xFF00;       \
         }                                     \
         if (lzs->bytetype & 1) {              \
            if (1!=GET(lzs,b))                 \
               return toread-howmuch;       \
         } else {                                 \
            BYTE  b1,b2;                          \
                                               \
            if (1!=GET(lzs,b1))                \
               return toread-howmuch;       \
            if (1!=GET(lzs,b2))                \
               return toread-howmuch;       \
            /* Format:                            \
             * b1 b2                              \
             * AB CD                              \
             * where CAB is the stringoffset in the table\
             * and D+3 is the len of the string   \
             */                                   \
            lzs->stringpos               = b1|((b2&0xf0)<<4); \
            lzs->stringlen               = (b2&0xf)+2;     \
            /* 3, but we use a  byte already below ... */\
            b     = lzs->table[lzs->stringpos];\
            lzs->stringpos               = (lzs->stringpos+1)&0xFFF;\
         }                                     \
         lzs->bytetype>>=1;                       \
      }                                        \
      /* store b in table */                         \
      lzs->table[lzs->curtabent++]= b;         \
      lzs->curtabent                     &= 0xFFF;            \
      lzs->realcurrent++;

   /* if someone has seeked, we have to bring the decompressor
    * to that position
    */
   if (lzs->realcurrent!=lzs->realwanted) {
      /* if the wanted position is before the current position
       * I see no easy way to unroll ... We have to restart at
       * the beginning. *sigh*
       */
      if (lzs->realcurrent>lzs->realwanted) {
         /* flush decompressor state */
         _llseek(lzs->realfd,14,SEEK_SET);
         GET_FLUSH(lzs);
         lzs->realcurrent= 0;
         lzs->bytetype                   = 0;
         lzs->stringlen                  = 0;
         memset(lzs->table,' ',0x1000);
         lzs->curtabent                  = 0xFF0;
      }
      while (lzs->realcurrent<lzs->realwanted) {
         DECOMPRESS_ONE_BYTE;
      }
   }

   while (howmuch) {
      DECOMPRESS_ONE_BYTE;
      lzs->realwanted++;
      *buf++      = b;
      howmuch--;
   }
   return   toread;
#undef DECOMPRESS_ONE_BYTE
}


/***********************************************************************
 *           LZSeek32   (LZ32.3)
 */

ODINFUNCTION3(LONG,LZSeek,HFILE,fd,LONG,off,INT,type)
{
   struct   lzstate                      *lzs;
   LONG  newwanted;

   dprintf(("LZ32: LZSeek(%08xh,%08xh,%08xh)\n",
            fd,
            off,
            type));

   /* not compressed? just use normal _llseek() */
        if (!(lzs = GET_LZ_STATE(fd))) return _llseek(fd,off,type);
   newwanted = lzs->realwanted;
   switch (type) {
   case 1:  /* SEEK_CUR */
      newwanted      += off;
      break;
   case 2:  /* SEEK_END */
      newwanted   = lzs->reallength-off;
      break;
   default:/* SEEK_SET */
      newwanted   = off;
      break;
   }
   if (newwanted>lzs->reallength)
      return LZERROR_BADVALUE;
   if (newwanted<0)
      return LZERROR_BADVALUE;
   lzs->realwanted                       = newwanted;
   return newwanted;
}


/***********************************************************************
 *           LZCopy32   (LZ32.0)
 *
 * Copies everything from src to dest
 * if src is a LZ compressed file, it will be uncompressed.
 * will return the number of bytes written to dest or errors.
 */

typedef  UINT (WINAPI *_readfun)(HFILE,LPVOID,UINT);

ODINFUNCTION2(LONG,LZCopy,HFILE,src,HFILE,dest)
{
   int   usedlzinit=0,ret,wret;
   LONG  len;
   HFILE oldsrc = src;
#define BUFLEN 1000
   BYTE  buf[BUFLEN];
   /* we need that weird typedef, for i can't seem to get function pointer
    * casts right. (Or they probably just do not like WINAPI in general)
    */
   _readfun xread;

   dprintf(("LZ32: LZCopy(%08x,h%08xh)\n",
            src,
            dest));

   if (!IS_LZ_HANDLE(src)) {
      src = LZInit(src);
                if ((INT)src <= 0) return 0;
      if (src != oldsrc) usedlzinit=1;
   }

   /* not compressed? just copy */
        if (!IS_LZ_HANDLE(src))
      xread=_lread;
   else
      xread=(_readfun)LZRead;
   len=0;
   while (1) {
      ret=xread(src,buf,BUFLEN);
      if (ret<=0) {
         if (ret==0)
            break;
         if (ret==-1)
            return LZERROR_READ;
         return ret;
      }
      len    += ret;
      wret  = _lwrite(dest,(LPCSTR)buf,ret);
      if (wret!=ret)
         return LZERROR_WRITE;
   }
   if (usedlzinit)
      LZClose(src);
   return len;
#undef BUFLEN
}

/* reverses GetExpandedPathname */
static LPSTR LZEXPAND_MangleName( LPCSTR fn )
{
    char *p;
    char *mfn = (char *)HeapAlloc( GetProcessHeap(), 0,
                                    strlen(fn) + 3 ); /* "._" and \0 */
    if (mfn == NULL) return NULL;
    strcpy( mfn, fn );
    if (!(p = strrchr( mfn, '\\' ))) p = mfn;
    if ((p = strchr( p, '.' )) != NULL)
    {
        p++;
        if (strlen(p) < 3) strcat( p, "_" );  /* append '_' */
        else p[strlen(p)-1] = '_';  /* replace last character */
    }
    else strcat( mfn, "._" );            /* append "._" */
    return mfn;
}


/***********************************************************************
 *           LZOpenFile32A   (LZ32.1)
 *
 * Opens a file. If not compressed, open it as a normal file.
 */

ODINFUNCTION3(HFILE,LZOpenFileA,LPCSTR,fn,LPOFSTRUCT,ofs,UINT,mode)
{
   HFILE fd,cfd;

   dprintf(("LZ32: LZOpenFileA(%s,%08xh,%08xh)\n",
            fn,
            ofs,
            mode));

   /* 0x70 represents all OF_SHARE_* flags, ignore them for the check */
   fd=OpenFile(fn,ofs,mode);
   if (fd==HFILE_ERROR)
        {
            LPSTR mfn = LZEXPAND_MangleName(fn);
            fd = OpenFile(mfn,ofs,mode);
            HeapFree( GetProcessHeap(), 0, mfn );
   }
   if ((mode&~0x70)!=OF_READ)
      return fd;
   if (fd==HFILE_ERROR)
      return HFILE_ERROR;
   cfd=LZInit(fd);
   if ((INT)cfd <= 0) return fd;
   return cfd;
}


/***********************************************************************
 *           LZOpenFile32W   (LZ32.10)
 */
ODINFUNCTION3(HFILE,LZOpenFileW,LPCWSTR,fn,LPOFSTRUCT,ofs,UINT,mode)
{
   LPSTR xfn;
   LPWSTR   yfn;
   HFILE ret;

   dprintf(("LZ32: LZOpenFileW(%08xh,%08xh,%08xh)\n",
            fn,
            ofs,
            mode));

   xfn   = HEAP_strdupWtoA( GetProcessHeap(), 0, fn);
   ret   = LZOpenFileA(xfn,ofs,mode);
   HeapFree( GetProcessHeap(), 0, xfn );
   if (ret!=HFILE_ERROR) {
      /* ofs->szPathName is an array with the OFSTRUCT */
                yfn = HEAP_strdupAtoW( GetProcessHeap(), 0, (LPCSTR)ofs->szPathName );
      memcpy(ofs->szPathName,yfn,lstrlenW(yfn)*2+2);
                HeapFree( GetProcessHeap(), 0, yfn );
   }
   return   ret;
}


/***********************************************************************
 *           LZClose32   (LZ32.5)
 */

ODINPROCEDURE1(LZClose,HFILE,fd)
{
   struct lzstate *lzs;

   dprintf(("LZ32: LZClose(%08xh)\n",
            fd));

        if (!(lzs = GET_LZ_STATE(fd))) _lclose(fd);
        else
        {
            HeapFree( GetProcessHeap(), 0, lzs->get );
            CloseHandle(lzs->realfd);
            lzstates[fd - 0x400] = NULL;
            HeapFree( GetProcessHeap(), 0, lzs );
        }
}


/***********************************************************************
 *           CopyLZFile32  (LZ32.7)
 *
 * Copy src to dest (including uncompressing src).
 * NOTE: Yes. This is exactly the same function as LZCopy.
 */

ODINFUNCTION2(LONG,CopyLZFile,HFILE,src,HFILE,dest)
{
  dprintf(("LZ32: CopyLZFile(%08xh,%08xh)\n",
           src,
           dest));

  return LZCopy(src,dest);
}
