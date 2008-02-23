        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_Sleep:PROC
        PUBLIC  O32_Sleep
O32_Sleep PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_Sleep
    add    esp, 4
    pop    fs
    ret
O32_Sleep ENDP

CODE32          ENDS

                END
