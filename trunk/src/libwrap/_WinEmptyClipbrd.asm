        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinEmptyClipbrd:PROC
        PUBLIC  _WinEmptyClipbrd
_WinEmptyClipbrd PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinEmptyClipbrd
    add    esp, 4
    pop    fs
    ret
_WinEmptyClipbrd ENDP

CODE32          ENDS

                END
