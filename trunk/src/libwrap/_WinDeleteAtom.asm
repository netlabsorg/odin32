        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDeleteAtom:PROC
        PUBLIC  _WinDeleteAtom
_WinDeleteAtom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinDeleteAtom
    add    esp, 8
    pop    fs
    ret
_WinDeleteAtom ENDP

CODE32          ENDS

                END
