        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosTmrQueryFreq:PROC
        PUBLIC  _DosTmrQueryFreq
_DosTmrQueryFreq PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosTmrQueryFreq
    add    esp, 4
    pop    fs
    ret
_DosTmrQueryFreq ENDP

CODE32          ENDS

                END
