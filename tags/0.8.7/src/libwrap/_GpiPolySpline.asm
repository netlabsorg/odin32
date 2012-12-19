        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPolySpline:PROC
        PUBLIC  _GpiPolySpline
_GpiPolySpline PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiPolySpline
    add    esp, 12
    pop    fs
    ret
_GpiPolySpline ENDP

CODE32          ENDS

                END
