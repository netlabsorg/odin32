        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryClipbrdData:PROC
        PUBLIC  _WinQueryClipbrdData
_WinQueryClipbrdData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryClipbrdData
    add    esp, 8
    pop    fs
    ret
_WinQueryClipbrdData ENDP

CODE32          ENDS

                END
