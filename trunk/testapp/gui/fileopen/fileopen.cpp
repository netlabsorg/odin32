#include <stdio.h>
#include <windows.h>
#include <Commdlg.h>
#include <tchar.h>

#ifndef _MSC_VER

#include <odinlx.h>

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR  lpCmdLine,
                   int    nCmdShow);

int main(int argc, char **argv)
{
#ifdef ODIN_FORCE_WIN32_TIB
    ForceWin32TIB();
#endif
    RegisterLxExe(WinMain, NULL);
}

#endif

//
// Gobal Variables and declarations.
//
OPENFILENAME ofn ;

// a another memory buffer to contain the file name
TCHAR szFile[100] ;

UINT_PTR CALLBACK OFNHookProc(
    HWND hdlg,
    UINT uiMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    printf("*** OFNHookProc\n");
    return 1;
}


DWORD WINAPI MyThreadFunc( LPVOID lpParam )
{
    // open a file name
    ZeroMemory( &ofn , sizeof( ofn));
    ofn.lStructSize = sizeof ( ofn );
    ofn.hwndOwner = NULL  ;
    ofn.lpstrFile = szFile ;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof( szFile );
    ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0");
    ofn.nFilterIndex =1;
    ofn.lpstrFileTitle = NULL ;
    ofn.nMaxFileTitle = 0 ;
    ofn.lpstrInitialDir=NULL ;
//    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ENABLEHOOK|OFN_EXPLORER;
    ofn.Flags =  OFN_ENABLESIZING|OFN_LONGNAMES|OFN_EXPLORER|OFN_ENABLEHOOK|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
    ofn.lpfnHook = OFNHookProc;
    GetOpenFileName( &ofn );

    // Now simpley display the file name
    MessageBox ( NULL , ofn.lpstrFile , _T("File Name") , MB_OK);
    return 0;
}

int WINAPI WinMain( HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdLine , int nCmdShow )
{
    DWORD dwThread;
    HANDLE hThread = CreateThread( NULL, 0, MyThreadFunc, NULL, 0, &dwThread);

    WaitForSingleObject( hThread , INFINITE );

    return 0;
}
