; $Id: OS2Krnl.inc,v 1.1 2000-02-10 01:37:32 bird Exp $
OTE		STRUC
ote_size		DD		?
ote_base		DD		?
ote_flags		DD		?
ote_pagemap		DD		?
ote_mapsize		DD		?
ote_hob		DW		?
ote_sel		DW		?
OTE		ENDS

SMTE		STRUC
smte_mpages		DD		?
smte_startobj		DD		?
smte_eip		DD		?
smte_stackobj		DD		?
smte_esp		DD		?
smte_pageshift		DD		?
smte_fixupsize		DD		?
smte_objtab		DD		?
smte_objcnt		DD		?
smte_objmap		DD		?
smte_itermap		DD		?
smte_rsrctab		DD		?
smte_rsrccnt		DD		?
smte_restab		DD		?
smte_enttab		DD		?
smte_fpagetab		DD		?
smte_frectab		DD		?
smte_impmod		DD		?
smte_impproc		DD		?
smte_datapage		DD		?
smte_nrestab		DD		?
smte_cbnrestab		DD		?
smte_autods		DD		?
smte_debuginfo		DD		?
smte_debuglen		DD		?
smte_heapsize		DD		?
smte_path		DD		?
smte_semcount		DW		?
smte_semowner		DW		?
smte_pfilecache		DD		?
smte_stacksize		DD		?
smte_alignshift		DW		?
smte_NEexpver		DW		?
smte_pathlen		DW		?
smte_NEexetype		DW		?
smte_csegpack		DW		?
SMTE		ENDS

MTE		STRUC
mte_flags2		DW		?
mte_handle		DW		?
mte_swapmte		DD ?
mte_link		DD	?
mte_flags1		DD		?
mte_impmodcnt		DD		?
mte_sfn		DW ?
mte_usecnt		DW		?
mte_modname		DB 8 DUP (?)
MTE		ENDS
