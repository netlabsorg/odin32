        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_AbortPath:PROC
        PUBLIC  O32_AbortPath
O32_AbortPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_AbortPath
    add    esp, 4
    pop    fs
    ret
O32_AbortPath ENDP

CODE32          ENDS

                END
