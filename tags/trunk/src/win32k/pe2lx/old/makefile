

all: filter3 pe2lx


pe2lx:
	@nmake /nologo -f pe2lx.mak

filter3:
	@nmake /nologo -f filter3.mak

bitmap.exe: bitmap.cpp ..\include\icon.h ..\include\bitmap.h
	icc -I..\include -DBITMAPCONVERTER bitmap.cpp



clean:
	@nmake /nologo -f filter3.mak clean
	@nmake /nologo -f pe2lx.mak clean
#	@del *.obj *.s *.exe *.map *.lst ole* err* *.err *.lnk *.pch
