        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryBackColor:PROC
        PUBLIC  _GpiQueryBackColor
_GpiQueryBackColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryBackColor
    add    esp, 4
    pop    fs
    ret
_GpiQueryBackColor ENDP

CODE32          ENDS

                END
