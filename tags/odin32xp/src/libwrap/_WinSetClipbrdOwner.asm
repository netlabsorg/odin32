        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetClipbrdOwner:PROC
        PUBLIC  _WinSetClipbrdOwner
_WinSetClipbrdOwner PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetClipbrdOwner
    add    esp, 8
    pop    fs
    ret
_WinSetClipbrdOwner ENDP

CODE32          ENDS

                END
