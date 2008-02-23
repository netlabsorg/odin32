        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDeleteSetId:PROC
        PUBLIC  _GpiDeleteSetId
_GpiDeleteSetId PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiDeleteSetId
    add    esp, 8
    pop    fs
    ret
_GpiDeleteSetId ENDP

CODE32          ENDS

                END
