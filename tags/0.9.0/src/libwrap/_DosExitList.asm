        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosExitList:PROC
        PUBLIC  _DosExitList
_DosExitList PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosExitList
    add    esp, 8
    pop    fs
    ret
_DosExitList ENDP

CODE32          ENDS

                END
