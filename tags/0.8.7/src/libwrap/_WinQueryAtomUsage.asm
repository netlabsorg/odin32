        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryAtomUsage:PROC
        PUBLIC  _WinQueryAtomUsage
_WinQueryAtomUsage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinQueryAtomUsage
    add    esp, 8
    pop    fs
    ret
_WinQueryAtomUsage ENDP

CODE32          ENDS

                END
