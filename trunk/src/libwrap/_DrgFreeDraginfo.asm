        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgFreeDraginfo:PROC
        PUBLIC  _DrgFreeDraginfo
_DrgFreeDraginfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgFreeDraginfo
    add    esp, 4
    pop    fs
    ret
_DrgFreeDraginfo ENDP

CODE32          ENDS

                END
