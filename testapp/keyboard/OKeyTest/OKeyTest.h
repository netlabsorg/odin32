// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#if !defined(AFX_OKEYTEST_H__05D06965_C6DD_11D5_B6AE_004854D1C794__INCLUDED_)
#define AFX_OKEYTEST_H__05D06965_C6DD_11D5_B6AE_004854D1C794__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"


//
// exported functions
//
void APIENTRY       logString(LPSTR lpstrOrigin, LPSTR lpstrText);
void APIENTRY       logMessage(LPSTR lpstrOrigin, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


#endif // !defined(AFX_OKEYTEST_H__05D06965_C6DD_11D5_B6AE_004854D1C794__INCLUDED_)
