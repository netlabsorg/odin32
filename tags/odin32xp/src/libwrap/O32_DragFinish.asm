        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DragFinish:PROC
        PUBLIC  O32_DragFinish
O32_DragFinish PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DragFinish
    add    esp, 4
    pop    fs
    ret
O32_DragFinish ENDP

CODE32          ENDS

                END
