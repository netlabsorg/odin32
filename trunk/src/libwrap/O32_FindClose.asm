        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_FindClose:PROC
        PUBLIC  O32_FindClose
O32_FindClose PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_FindClose
    add    esp, 4
    pop    fs
    ret
O32_FindClose ENDP

CODE32          ENDS

                END
