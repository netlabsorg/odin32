        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_TlsGetValue:PROC
        PUBLIC  O32_TlsGetValue
O32_TlsGetValue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_TlsGetValue
    add    esp, 4
    pop    fs
    ret
O32_TlsGetValue ENDP

CODE32          ENDS

                END
