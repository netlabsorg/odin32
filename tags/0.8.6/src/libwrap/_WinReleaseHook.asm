        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinReleaseHook:PROC
        PUBLIC  _WinReleaseHook
_WinReleaseHook PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinReleaseHook
    add    esp, 20
    pop    fs
    ret
_WinReleaseHook ENDP

CODE32          ENDS

                END
