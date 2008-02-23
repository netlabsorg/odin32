        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ExitThread:PROC
        PUBLIC  O32_ExitThread
O32_ExitThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_ExitThread
    add    esp, 4
    pop    fs
    ret
O32_ExitThread ENDP

CODE32          ENDS

                END
