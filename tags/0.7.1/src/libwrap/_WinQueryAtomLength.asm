        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryAtomLength:PROC
        PUBLIC  _WinQueryAtomLength
_WinQueryAtomLength PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryAtomLength
    add    esp, 8
    pop    fs
    ret
_WinQueryAtomLength ENDP

CODE32          ENDS

                END
