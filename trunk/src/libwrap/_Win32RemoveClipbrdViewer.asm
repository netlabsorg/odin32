        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   Win32RemoveClipbrdViewer:PROC
        PUBLIC  _Win32RemoveClipbrdViewer
_Win32RemoveClipbrdViewer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   Win32RemoveClipbrdViewer
    add    esp, 4
    pop    fs
    ret
_Win32RemoveClipbrdViewer ENDP

CODE32          ENDS

                END
