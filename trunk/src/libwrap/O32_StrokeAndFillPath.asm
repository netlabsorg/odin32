        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_StrokeAndFillPath:PROC
        PUBLIC  O32_StrokeAndFillPath
O32_StrokeAndFillPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_StrokeAndFillPath
    add    esp, 4
    pop    fs
    ret
O32_StrokeAndFillPath ENDP

CODE32          ENDS

                END
