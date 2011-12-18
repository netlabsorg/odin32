        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryDragitemPtr:PROC
        PUBLIC  _DrgQueryDragitemPtr
_DrgQueryDragitemPtr PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DrgQueryDragitemPtr
    add    esp, 8
    pop    fs
    ret
_DrgQueryDragitemPtr ENDP

CODE32          ENDS

                END
