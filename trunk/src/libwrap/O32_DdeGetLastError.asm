        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DdeGetLastError:PROC
        PUBLIC  O32_DdeGetLastError
O32_DdeGetLastError PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DdeGetLastError
    add    esp, 4
    pop    fs
    ret
O32_DdeGetLastError ENDP

CODE32          ENDS

                END
