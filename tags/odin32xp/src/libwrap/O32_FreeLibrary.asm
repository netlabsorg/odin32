        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_FreeLibrary:PROC
        PUBLIC  O32_FreeLibrary
O32_FreeLibrary PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_FreeLibrary
    add    esp, 4
    pop    fs
    ret
O32_FreeLibrary ENDP

CODE32          ENDS

                END
