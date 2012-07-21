        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryBoundaryData:PROC
        PUBLIC  _GpiQueryBoundaryData
_GpiQueryBoundaryData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryBoundaryData
    add    esp, 8
    pop    fs
    ret
_GpiQueryBoundaryData ENDP

CODE32          ENDS

                END
