        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CloseHandle:PROC
        PUBLIC  O32_CloseHandle
O32_CloseHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CloseHandle
    add    esp, 4
    pop    fs
    ret
O32_CloseHandle ENDP

CODE32          ENDS

                END
