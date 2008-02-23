        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQuerySystemAtomTable:PROC
        PUBLIC  _WinQuerySystemAtomTable
_WinQuerySystemAtomTable PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   WinQuerySystemAtomTable
    pop    fs
    ret
_WinQuerySystemAtomTable ENDP

CODE32          ENDS

                END
