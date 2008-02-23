        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_LocalDiscard:PROC
        PUBLIC  O32_LocalDiscard
O32_LocalDiscard PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_LocalDiscard
    add    esp, 4
    pop    fs
    ret
O32_LocalDiscard ENDP

CODE32          ENDS

                END
