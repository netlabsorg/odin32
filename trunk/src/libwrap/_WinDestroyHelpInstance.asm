        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDestroyHelpInstance:PROC
        PUBLIC  _WinDestroyHelpInstance
_WinDestroyHelpInstance PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinDestroyHelpInstance
    add    esp, 4
    pop    fs
    ret
_WinDestroyHelpInstance ENDP

CODE32          ENDS

                END
