        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiResetBoundaryData:PROC
        PUBLIC  _GpiResetBoundaryData
_GpiResetBoundaryData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiResetBoundaryData
    add    esp, 4
    pop    fs
    ret
_GpiResetBoundaryData ENDP

CODE32          ENDS

                END
