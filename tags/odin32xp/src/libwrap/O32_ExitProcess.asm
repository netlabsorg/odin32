        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ExitProcess:PROC
        PUBLIC  O32_ExitProcess
O32_ExitProcess PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_ExitProcess
    add    esp, 4
    pop    fs
    ret
O32_ExitProcess ENDP

CODE32          ENDS

                END
