        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryDragitemCount:PROC
        PUBLIC  _DrgQueryDragitemCount
_DrgQueryDragitemCount PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgQueryDragitemCount
    add    esp, 4
    pop    fs
    ret
_DrgQueryDragitemCount ENDP

CODE32          ENDS

                END
