        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_TlsAlloc:PROC
        PUBLIC  O32_TlsAlloc
O32_TlsAlloc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_TlsAlloc
    pop    fs
    ret
O32_TlsAlloc ENDP

CODE32          ENDS

                END
