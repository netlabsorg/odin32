        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgAllocDraginfo:PROC
        PUBLIC  _DrgAllocDraginfo
_DrgAllocDraginfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgAllocDraginfo
    add    esp, 4
    pop    fs
    ret
_DrgAllocDraginfo ENDP

CODE32          ENDS

                END
