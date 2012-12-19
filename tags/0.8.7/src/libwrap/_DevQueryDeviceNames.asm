        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DevQueryDeviceNames:PROC
        PUBLIC  _DevQueryDeviceNames
_DevQueryDeviceNames PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   DevQueryDeviceNames
    add    esp, 28
    pop    fs
    ret
_DevQueryDeviceNames ENDP

CODE32          ENDS

                END
