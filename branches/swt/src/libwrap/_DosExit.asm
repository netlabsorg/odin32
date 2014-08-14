        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosExit:PROC
        PUBLIC  _DosExit
_DosExit PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosExit
    add    esp, 8
    pop    fs
    ret
_DosExit ENDP

CODE32          ENDS

                END
