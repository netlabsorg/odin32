; $Id: locks.asm,v 1.1 2001-01-20 23:48:53 bird Exp $
;
; My own R/W lock implementation.
;
; Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;

    .486p

;
;   Include files
;
    include devsegdf.inc
    include locks.inc

;
;   Exported symbols
;
    public RWLockAquireRead
    public RWLockAquireWrite
    public RWLockReleaseRead
    public RWLockReleaseWrite



