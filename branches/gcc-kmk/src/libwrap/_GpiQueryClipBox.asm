        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryClipBox:PROC
        PUBLIC  _GpiQueryClipBox
_GpiQueryClipBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryClipBox
    add    esp, 8
    pop    fs
    ret
_GpiQueryClipBox ENDP

CODE32          ENDS

                END
