: $Id: dbg.cmd,v 1.1 1999-05-24 20:19:05 ktk Exp $ 

SET PMDPATH=f:\source\win32os2\src\peldr;f:\source\win32os2\src\advapi32;f:\source\win32os2\src\comdlg32;f:\source\win32os2\src\gdi32;f:\source\win32os2\src\user32;f:\source\win32os2\src\kernel32;f:\source\win32os2\src\winmm;f:\source\win32os2\src\version;f:\source\win32os2\src\shell32;f:\source\win32os2\src\ole32;f:\source\win32os2\src\ddraw;f:\source\win32os2\src\dsound;
del \os2\*.@*
ipmd %1 %2 %3 %4 %5

