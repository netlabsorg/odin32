        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetClipbrdViewer:PROC
        PUBLIC  _WinSetClipbrdViewer
_WinSetClipbrdViewer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetClipbrdViewer
    add    esp, 8
    pop    fs
    ret
_WinSetClipbrdViewer ENDP

CODE32          ENDS

                END
