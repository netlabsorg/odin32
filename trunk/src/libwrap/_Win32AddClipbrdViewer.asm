        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   Win32AddClipbrdViewer:PROC
        PUBLIC  _Win32AddClipbrdViewer
_Win32AddClipbrdViewer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   Win32AddClipbrdViewer
    add    esp, 4
    pop    fs
    ret
_Win32AddClipbrdViewer ENDP

CODE32          ENDS

                END
