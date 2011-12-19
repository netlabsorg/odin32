        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiStrokeInkPath:PROC
        PUBLIC  _GpiStrokeInkPath
_GpiStrokeInkPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiStrokeInkPath
    add    esp, 20
    pop    fs
    ret
_GpiStrokeInkPath ENDP

CODE32          ENDS

                END
