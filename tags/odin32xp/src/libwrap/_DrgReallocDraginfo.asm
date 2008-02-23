        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgReallocDraginfo:PROC
        PUBLIC  _DrgReallocDraginfo
_DrgReallocDraginfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DrgReallocDraginfo
    add    esp, 8
    pop    fs
    ret
_DrgReallocDraginfo ENDP

CODE32          ENDS

                END
