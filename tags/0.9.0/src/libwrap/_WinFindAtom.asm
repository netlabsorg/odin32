        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinFindAtom:PROC
        PUBLIC  _WinFindAtom
_WinFindAtom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinFindAtom
    add    esp, 8
    pop    fs
    ret
_WinFindAtom ENDP

CODE32          ENDS

                END
