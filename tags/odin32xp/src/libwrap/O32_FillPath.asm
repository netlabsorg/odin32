        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_FillPath:PROC
        PUBLIC  O32_FillPath
O32_FillPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_FillPath
    add    esp, 4
    pop    fs
    ret
O32_FillPath ENDP

CODE32          ENDS

                END
