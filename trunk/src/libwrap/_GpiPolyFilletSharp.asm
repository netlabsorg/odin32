        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPolyFilletSharp:PROC
        PUBLIC  _GpiPolyFilletSharp
_GpiPolyFilletSharp PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiPolyFilletSharp
    add    esp, 16
    pop    fs
    ret
_GpiPolyFilletSharp ENDP

CODE32          ENDS

                END
