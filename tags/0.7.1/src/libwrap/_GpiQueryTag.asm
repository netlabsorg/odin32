        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryTag:PROC
        PUBLIC  _GpiQueryTag
_GpiQueryTag PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryTag
    add    esp, 8
    pop    fs
    ret
_GpiQueryTag ENDP

CODE32          ENDS

                END
