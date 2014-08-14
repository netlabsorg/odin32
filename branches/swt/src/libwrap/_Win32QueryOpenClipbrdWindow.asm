        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   Win32QueryOpenClipbrdWindow:PROC
        PUBLIC  _Win32QueryOpenClipbrdWindow
_Win32QueryOpenClipbrdWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   Win32QueryOpenClipbrdWindow
    pop    fs
    ret
_Win32QueryOpenClipbrdWindow ENDP

CODE32          ENDS

                END
