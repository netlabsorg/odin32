        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosTmrQueryTime:PROC
        PUBLIC  _DosTmrQueryTime
_DosTmrQueryTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosTmrQueryTime
    add    esp, 4
    pop    fs
    ret
_DosTmrQueryTime ENDP

CODE32          ENDS

                END
