        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgCancelLazyDrag:PROC
        PUBLIC  _DrgCancelLazyDrag
_DrgCancelLazyDrag PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   DrgCancelLazyDrag
    pop    fs
    ret
_DrgCancelLazyDrag ENDP

CODE32          ENDS

                END
