        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_StrokePath:PROC
        PUBLIC  O32_StrokePath
O32_StrokePath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_StrokePath
    add    esp, 4
    pop    fs
    ret
O32_StrokePath ENDP

CODE32          ENDS

                END
