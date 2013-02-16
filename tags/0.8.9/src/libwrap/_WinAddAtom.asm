        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinAddAtom:PROC
        PUBLIC  _WinAddAtom
_WinAddAtom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinAddAtom
    add    esp, 8
    pop    fs
    ret
_WinAddAtom ENDP

CODE32          ENDS

                END
