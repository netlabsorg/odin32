/* $Id: misc.cpp,v 1.1 1999-09-06 02:20:07 bird Exp $ */

/*
 * PE2LX ascii to unicode conversion
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "pe2lx.h"
#ifndef RING0
    #include <uniconv.h>
#endif

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#ifndef RING0
    static BOOL prepareCP(int cp);
    static void convertCP(int cp, char *str);
#endif


//******************************************************************************
//******************************************************************************
char *UnicodeToAscii(int length, WCHAR *wstring, int cp)
{
    static char *astring = NULL;
    static int lenas = 0;

    int i;
    int len;

    if (length < 0x10000) //one have to set some limits
    {
        if ((ULONG)wstring > MINPTR && (ULONG)wstring < MAXPTR)
        {
            if (length >= lenas)
            {
                char *p = (char*)realloc(astring,lenas = max(length+0x20,0x100));
                if (p == NULL)
                    return "";
                astring = p;
            }

            for (i=0;i < length;i++)
                astring[i] = (UCHAR)wstring[i];

            if (lenas-i > 0)
                astring[i] = '\0';
            #ifndef RING0
                convertCP(cp, asciistring);
            #else
                cp = cp;
            #endif
        }
        else
        {
            kprintf(("UnicodeToAscii: wstring is an invalid pointer, %p\n", wstring));
            return "";
        }
    }
    else
    {
        kprintf(("UnicodeToAscii: String requested is too long.(%d)\n",length));
        return "";
    }
    return (astring);
}
//******************************************************************************
//******************************************************************************
char *UnicodeToAscii(WCHAR *wstring, int cp /*=0*/)
{
    static char *astring = NULL;
    static int lenas = 0;

    int i;
    int len;

    //astring[0] = '\0'; //returns an empty string on error
    if ((ULONG)wstring > MINPTR && (ULONG)wstring < MAXPTR)
    {
        len = UniStrlen(wstring);
        if (len+1 >= lenas)
        {
            char *p = (char*)realloc(astring, lenas = max(len+0x20,0x100));
            if (p == NULL)
                return "";
            astring = p;
        }

        for (i=0; i < len; i++)
            astring[i] = (UCHAR)wstring[i];

        if (lenas-i > 0)
            astring[i] = '\0';
        #ifndef RING0
            convertCP(cp, astring);
        #endif
    }
    else
    {
        kprintf(("UnicodeToAscii: wstring is an invalid pointer, %p\n", wstring));
        return ("");
    }

    return (astring);
}
//******************************************************************************
//******************************************************************************
int UniStrlen(WCHAR *wstring)
{
 int i = 0;
  ltassert((ULONG)wstring > MINPTR && (ULONG) wstring < MAXPTR);
  while(wstring[i] != 0)    i++;
  return(i);
}
//******************************************************************************
//******************************************************************************
void UpCase(char *mixedcase)
{
 int i;
  vltassert((ULONG)mixedcase > MINPTR && (ULONG) mixedcase < MAXPTR);

  for(i=0;i<strlen(mixedcase);i++) {
    if(mixedcase[i] >= 'a' && mixedcase[i] <= 'z') {
        mixedcase[i] += 'A' - 'a';
    }
  }
}
//******************************************************************************
#ifndef RING0
char transCP[256];
int CodePage=-1;
//******************************************************************************
static BOOL prepareCP(int cp)
{
  #define SIZE_ucs_code_page 20

  static  UconvObject uconv_objFrom = NULL;
  static  UconvObject uconv_objTo = NULL;

  UniChar ucs_code_page[SIZE_ucs_code_page];
  int     rc, i, j;
  BOOL    ret;
  size_t  uni_chars_left;
  size_t  in_bytes_left;
  size_t  num_subs;
  UniChar *pout_uni_str;
  char    *pin_char_str;
  char    table[256];
  UniChar fromCP[256];
  UniChar toCP[256];

  if(cp != 0)
  {
    if(uconv_objTo == NULL)
    {
      rc = UniCreateUconvObject( (UniChar*)L"", &uconv_objTo);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniCreateUconvObject, return code = %u\n", rc);
        return FALSE;
      }
    }

    if(cp != CodePage)
    {
      if(uconv_objFrom != NULL)
      {
        rc = UniFreeUconvObject(uconv_objFrom);
        if (rc != ULS_SUCCESS)
        {
          printf("ERROR UniFreeUconvObject error: return code = %u\n", rc);
          return FALSE;
        }
      }
      rc = UniMapCpToUcsCp(cp, ucs_code_page, SIZE_ucs_code_page);
      if (rc != ULS_SUCCESS)
      {
        printf("ERROR couldn't translate Codepage ID to unistring, return code = %u\n", rc);
        return FALSE;
      }
      rc = UniCreateUconvObject( ucs_code_page, &uconv_objFrom);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniCreateUconvObject, return code = %u\n", rc);
        return FALSE;
      }
      for(i=0;i<256;i++)
        table[i] = i;

      in_bytes_left = 256;
      uni_chars_left = 256;
      pout_uni_str = fromCP;
      pin_char_str = table;
      rc = UniUconvToUcs(uconv_objFrom, (void**)&pin_char_str, &in_bytes_left,
                         &pout_uni_str, &uni_chars_left, &num_subs);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniUconvToUcs, return code = %u\n", rc);
        return FALSE;
      }

      in_bytes_left = 256;
      uni_chars_left = 256;
      pout_uni_str = toCP;
      pin_char_str = table;
      rc = UniUconvToUcs(uconv_objTo, (void**)&pin_char_str, &in_bytes_left,
                         &pout_uni_str, &uni_chars_left, &num_subs);
      if ( rc != ULS_SUCCESS )
      {
        printf("ERROR UniUconvToUcs, return code = %u\n", rc);
        return FALSE;
      }

      CodePage = cp;

      for(i=0;i<256;i++)
      {
        transCP[i] = i;
        for(j=0;j<256;j++)
        {
          if(fromCP[i] == toCP[j])
          {
            transCP[i] = j;
            break;
          }
        }
      }
    }
  }
  else  return FALSE;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
void convertCP(int cp, char *str)
{
  if(str == NULL)
    return;

  if(prepareCP(cp) == TRUE && CodePage > 0)
    while(*str != 0)
    {
      *str = transCP[*str];
      str++;
    }
}
//******************************************************************************
//******************************************************************************
#endif /* ifndef RING 0 */
