        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetEndOfFile:PROC
        PUBLIC  O32_SetEndOfFile
O32_SetEndOfFile PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetEndOfFile
    add    esp, 4
    pop    fs
    ret
O32_SetEndOfFile ENDP

CODE32          ENDS

                END
