        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDestroyPS:PROC
        PUBLIC  _GpiDestroyPS
_GpiDestroyPS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiDestroyPS
    add    esp, 4
    pop    fs
    ret
_GpiDestroyPS ENDP

CODE32          ENDS

                END
