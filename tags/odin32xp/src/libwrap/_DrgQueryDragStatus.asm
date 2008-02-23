        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryDragStatus:PROC
        PUBLIC  _DrgQueryDragStatus
_DrgQueryDragStatus PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   DrgQueryDragStatus
    pop    fs
    ret
_DrgQueryDragStatus ENDP

CODE32          ENDS

                END
