        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosFreeModule:PROC
        PUBLIC  _DosFreeModule
_DosFreeModule PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosFreeModule
    add    esp, 4
    pop    fs
    ret
_DosFreeModule ENDP

CODE32          ENDS

                END
