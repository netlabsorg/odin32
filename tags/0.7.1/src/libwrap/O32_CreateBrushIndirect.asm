        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateBrushIndirect:PROC
        PUBLIC  O32_CreateBrushIndirect
O32_CreateBrushIndirect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CreateBrushIndirect
    add    esp, 4
    pop    fs
    ret
O32_CreateBrushIndirect ENDP

CODE32          ENDS

                END
