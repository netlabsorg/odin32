        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosAllocMem:PROC
        PUBLIC  _DosAllocMem
_DosAllocMem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosAllocMem
    add    esp, 12
    pop    fs
    ret
_DosAllocMem ENDP

CODE32          ENDS

                END
