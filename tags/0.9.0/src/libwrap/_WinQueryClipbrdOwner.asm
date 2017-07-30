        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryClipbrdOwner:PROC
        PUBLIC  _WinQueryClipbrdOwner
_WinQueryClipbrdOwner PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryClipbrdOwner
    add    esp, 4
    pop    fs
    ret
_WinQueryClipbrdOwner ENDP

CODE32          ENDS

                END
