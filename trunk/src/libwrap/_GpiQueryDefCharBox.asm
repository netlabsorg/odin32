        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryDefCharBox:PROC
        PUBLIC  _GpiQueryDefCharBox
_GpiQueryDefCharBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryDefCharBox
    add    esp, 8
    pop    fs
    ret
_GpiQueryDefCharBox ENDP

CODE32          ENDS

                END
