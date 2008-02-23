        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SaveDC:PROC
        PUBLIC  O32_SaveDC
O32_SaveDC PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SaveDC
    add    esp, 4
    pop    fs
    ret
O32_SaveDC ENDP

CODE32          ENDS

                END
