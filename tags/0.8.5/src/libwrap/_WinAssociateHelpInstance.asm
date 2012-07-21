        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinAssociateHelpInstance:PROC
        PUBLIC  _WinAssociateHelpInstance
_WinAssociateHelpInstance PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinAssociateHelpInstance
    add    esp, 8
    pop    fs
    ret
_WinAssociateHelpInstance ENDP

CODE32          ENDS

                END
