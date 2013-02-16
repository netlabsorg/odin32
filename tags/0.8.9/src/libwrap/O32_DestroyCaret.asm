        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DestroyCaret:PROC
        PUBLIC  O32_DestroyCaret
O32_DestroyCaret PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_DestroyCaret
    pop    fs
    ret
O32_DestroyCaret ENDP

CODE32          ENDS

                END
