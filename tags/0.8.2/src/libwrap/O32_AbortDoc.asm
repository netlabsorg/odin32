        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_AbortDoc:PROC
        PUBLIC  O32_AbortDoc
O32_AbortDoc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_AbortDoc
    add    esp, 4
    pop    fs
    ret
O32_AbortDoc ENDP

CODE32          ENDS

                END
