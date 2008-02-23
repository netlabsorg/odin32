        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateCompatibleDC:PROC
        PUBLIC  O32_CreateCompatibleDC
O32_CreateCompatibleDC PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CreateCompatibleDC
    add    esp, 4
    pop    fs
    ret
O32_CreateCompatibleDC ENDP

CODE32          ENDS

                END
