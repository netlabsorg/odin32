        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryDraginfoPtrFromDragitem:PROC
        PUBLIC  _DrgQueryDraginfoPtrFromDragitem
_DrgQueryDraginfoPtrFromDragitem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgQueryDraginfoPtrFromDragitem
    add    esp, 4
    pop    fs
    ret
_DrgQueryDraginfoPtrFromDragitem ENDP

CODE32          ENDS

                END
