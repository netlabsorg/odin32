#ifndef __SHLFILEOP_H__
#define __SHLFILEOP_H__
#define IsAttribFile(x) (!(x == -1) && !(x & FILE_ATTRIBUTE_DIRECTORY))
#define IsAttribDir(x)  (!(x == -1) && (x & FILE_ATTRIBUTE_DIRECTORY))
#endif

#undef A_W
#undef I_SHFileStrCpyCatA_W

#ifdef I_SHFileStrCpyCatA
#undef I_SHFileStrCpyCatA
#define I_SHFileStrCpyCatA_W SHFileStrCpyCatA
#define A_W 1
#endif

#ifdef I_SHFileStrCpyCatW
#undef I_SHFileStrCpyCatW
#define I_SHFileStrCpyCatA_W SHFileStrCpyCatW
#define A_W 0
#endif

#undef LPSTR_A_W
#undef LPCSTR_A_W
#undef lstrcpy_A_W
#undef strrchr_A_W
#undef lstrlen_A_W

#if A_W
#define LPSTR_A_W LPSTR
#define LPCSTR_A_W LPCSTR
#define lstrcpy_A_W lstrcpyA
#define strrchr_A_W strrchr
#define lstrlen_A_W lstrlenA
#else
#define LPSTR_A_W LPWSTR
#define LPCSTR_A_W LPCWSTR
#define lstrcpy_A_W lstrcpyW
#define strrchr_A_W strrchrW
#define lstrlen_A_W lstrlenW
#endif

#undef A_W

#ifdef I_SHFileStrCpyCatA_W
LPSTR_A_W I_SHFileStrCpyCatA_W(LPSTR_A_W pTo, LPCSTR_A_W pFrom, LPCSTR_A_W pCatStr)
{
#undef I_SHFileStrCpyCatA_W
  LPSTR_A_W pToFile = NULL;
  int  i_len;
  if (pTo)
  {
    if (pFrom)
      lstrcpy_A_W(pTo, pFrom);
    if (pCatStr)
    {
      i_len = lstrlen_A_W(pTo);
      if ((i_len) && (pTo[--i_len] != '\\'))
        i_len++;
      pTo[i_len] = '\\';
      if (pCatStr[0] == '\\')
        pCatStr++; \
      lstrcpy_A_W(&pTo[i_len+1], pCatStr);
    }
    pToFile = strrchr_A_W(pTo,'\\');
/* !! termination of the new string-group */
    pTo[(lstrlen_A_W(pTo)) + 1] = '\0';
  }
  return pToFile;
}
#undef SHFileStrCpyCatA_W
#endif
#if 0
#define SHFileStrCpyCat(A) LPSTR_##A SHFileStrCpyCat##A(LPSTR_##A pTo, LPCSTR_##A pFrom, LPCSTR_##A pCatStr) \
{  \
  LPSTR_##A pToFile = NULL;  \
  int  i_len; \
  if (pTo)    \
  {           \
    if (pFrom)    \
      lstrcpy##A(pTo, pFrom);   \
    if (pCatStr) \
    {  \
      i_len = lstrlen##A(pTo); \
      if ((i_len) && (pTo[--i_len] != '\\')) \
        i_len++;  \
      pTo[i_len] = '\\'; \
      if (pCatStr[0] == '\\') \
        pCatStr++; \
      lstrcpy##A(&pTo[i_len+1], pCatStr); \
    }  \
    pToFile = strrchr##A(pTo,'\\');   \
/* !! termination of the new string-group */   \
    pTo[(lstrlen##A(pTo)) + 1] = '\0';   \
  }  \
  return pToFile; \
}
#endif

