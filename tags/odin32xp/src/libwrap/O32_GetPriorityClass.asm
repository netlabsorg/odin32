        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetPriorityClass:PROC
        PUBLIC  O32_GetPriorityClass
O32_GetPriorityClass PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetPriorityClass
    add    esp, 4
    pop    fs
    ret
O32_GetPriorityClass ENDP

CODE32          ENDS

                END
