        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCloseVDD:PROC
        PUBLIC  _DosCloseVDD
_DosCloseVDD PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosCloseVDD
    add    esp, 4
    pop    fs
    ret
_DosCloseVDD ENDP

CODE32          ENDS

                END
