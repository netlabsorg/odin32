: $Id: ziptree.cmd,v 1.1 1999-05-24 20:19:05 ktk Exp $ 

@del win32os2.zip
zip -r win32os2.zip ChangeLog Readme.txt LICENSE.TXT include Makefile *.bugs *.cmd
zip win32os2.zip bin\*.cmd src\* src\advapi32\* src\capi2032\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\comdlg32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\ddraw\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\dsound\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\gdi32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\kernel32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\mpr\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\ntdll\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\ole32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\pe2lx\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\peldr\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\shell32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\tapi32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\user32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\user32\new\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\version\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\winmm\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\winspool\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\wnetap32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip src\wsock32\* -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
zip win32os2.zip lib\pmwinx.lib lib\pmwinx.def
rem zip -r win32os2.zip src\win32k -x *.obj *.sys *.map *.dll *.sym *.pch *.lib *.exe *.res
