        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_FlushFileBuffers:PROC
        PUBLIC  O32_FlushFileBuffers
O32_FlushFileBuffers PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_FlushFileBuffers
    add    esp, 4
    pop    fs
    ret
O32_FlushFileBuffers ENDP

CODE32          ENDS

                END
