/* $Id: OS2KOwn.h,v 1.1 2001-09-24 02:12:13 bird Exp $
 *
 * System Object Owner Identifiers from sg24-4640-00 (#0.7a).
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _OS2KOwn_h_
#define _OS2KOwn_h_

#define OWNER_lielist     0xff2d        /* LDR LieLists                  */
#define OWNER_demversion  0xff2e        /* DEM fake version entries      */
#define OWNER_vmbmapd     0xff2f        /* VM Arena Bitmap Directory     */
#define OWNER_npipenpn    0xff30        /* Named pipe NPN segment        */
#define OWNER_npipenp     0xff31        /* Named pipe NP segment         */
#define OWNER_reqpkttcb   0xff32        /* DD TCB request packets        */
#define OWNER_reqpkt2     0xff33        /* DD strat2 request packets     */
#define OWNER_spldevrmp   0xff34        /* Spool Dev RMP segment         */
#define OWNER_chardevrmp  0xff35        /* Char Dev RMP segment          */
#define OWNER_syssemrmp   0xff36        /* System Semaphore RMP segment  */
#define OWNER_romdata     0xff37        /* ROM data                      */
#define OWNER_libpath     0xff38        /* LDR LibPath                   */
#define OWNER_jfnflags    0xff39        /* JFN flags                     */
#define OWNER_jfntable    0xff3a        /* JFN table                     */
#define OWNER_ptouvirt    0xff3b        /* PhysToUVirt                   */
#define OWNER_tkr3stack   0xff3c        /* Ring 3 stack                  */
#define OWNER_tkr2stack   0xff3d        /* Ring 2 stack                  */
#define OWNER_tkenv       0xff3e        /* User Environment              */
#define OWNER_tktib       0xff3f        /* Thread Information Block      */
#define OWNER_reqpkt1     0xff40        /* DD strat1 request packets     */
#define OWNER_allocphys   0xff41        /* Allocated via DevHlp AllocPhys*/
#define OWNER_khbdon      0xff42        /* Unusable donated heap page    */
                                        /* owner                         */
#define OWNER_krhrw1m     0xff43        /* Resident R/W 1Meg mem heap    */
                                        /* owner                         */
#define OWNER_krhro1m     0xff44        /* Resident R/W 1Meg mem heap    */
                                        /* owner                         */
#define OWNER_mmph        0xff45        /* dekko mapped memory           */
#define OWNER_pageio      0xff46        /* pageio per-swap-file save     */
                                        /* block                         */
#define OWNER_fsreclok    0xff47        /* record lock record owner      */
                                        /* File System Drivers           */
#define OWNER_fsd1        0xff48        /* FSD 1                         */
#define OWNER_fsd2        0xff49        /* FSD 2                         */
#define OWNER_fsd3        0xff4a        /* FSD 3                         */
#define OWNER_fsd4        0xff4b        /* FSD 4                         */
#define OWNER_fsd5        0xff4c        /* FSD 5                         */
#define OWNER_fsd6        0xff4d        /* FSD 6                         */
#define OWNER_fsd7        0xff4e        /* FSD 7                         */
#define OWNER_fsd8        0xff4f        /* FSD 8 and subsequent          */
#define OWNER_dd1         0xff50        /* device driver 1               */
#define OWNER_dd2         0xff51        /* device driver 2               */
#define OWNER_dd3         0xff52        /* device driver 3               */
#define OWNER_dd4         0xff53        /* device driver 4               */
#define OWNER_dd5         0xff54        /* device driver 5               */
#define OWNER_dd6         0xff55        /* device driver 6               */
#define OWNER_dd7         0xff56        /* device driver 7               */
#define OWNER_dd8         0xff57        /* device driver 8               */
#define OWNER_dd9         0xff58        /* device driver 9               */
#define OWNER_dd10        0xff59        /* device driver 10              */
#define OWNER_dd11        0xff5a        /* device driver 11              */
#define OWNER_dd12        0xff5b        /* device driver 12              */
#define OWNER_dd13        0xff5c        /* device driver 13              */
#define OWNER_dd14        0xff5d        /* device driver 14              */
#define OWNER_dd15        0xff5e        /* device driver 15              */
#define OWNER_dd16        0xff5f        /* device driver 16 and subsequent */

#define OWNER_fsclmap     0xff60        /* cluster map owner             */
#define OWNER_cdsrmp      0xff61        /* Current Directory Structure   */
                                        /* RMP seg                       */
#define OWNER_tom         0xff62        /* Timeout Manager               */
#define OWNER_abios       0xff63        /* Advanced BIOS                 */
#define OWNER_cache       0xff64        /* cache                         */
#define OWNER_dbgdcb      0xff65        /* DBG Debug Control Block       */
#define OWNER_dbgkdb      0xff66        /* DBG Kernel Debug Block        */
#define OWNER_dbgwpcb     0xff67        /* DBP Watch Point Control Block */
#define OWNER_demsft      0xff68        /* DEM SFT array (for FCBs)      */
#define OWNER_demfonto    0xff69        /* DEM font offsets              */
#define OWNER_demfont     0xff6a        /* DEM font data                 */
#define OWNER_devhlp      0xff6b        /* allocated via devhlp AllocPhys*/
#define OWNER_discard     0xff6c        /* discardable, zero fill object */
#define OWNER_doshlp      0xff6d        /* DosHelp segment                  */
#define OWNER_dyndtgp     0xff6e        /* DYN tracepoint parm block       */
#define OWNER_dyndto      0xff6f        /* dynamic tracepoint               */
#define OWNER_dyndtot     0xff70        /* tmp dynamic trace info          */
#define OWNER_dynmtel     0xff71        /* DYN MTE dynamic   trace   link      */
#define OWNER_emalloc     0xff72        /* EM86 malloc ( )                    */
#define OWNER_emtss       0xff73        /* EM86 TSS                          */
#define OWNER_device      0xff74        /* installed   device   driver         */
#define OWNER_infoseg     0xff75        /* infoseg   ( local   or   global )       */
#define OWNER_initmsg     0xff76        /* INIT   saved   message               */
#define OWNER_init        0xff77        /* generic   init - time   only          */
#define OWNER_intdirq     0xff78        /* INT   IRQ   info                     */
#define OWNER_intstack    0xff79        /* interrupt   stack                  */
#define OWNER_iopllist    0xff7a        /* List   of   modules   with   IOPL       */
#define OWNER_kdbalias    0xff7b        /* Kernel   debugger   alias           */
#define OWNER_kdbsym      0xff7c        /* Kernel   debugger   symbol          */
#define OWNER_kmhook      0xff7d        /* KM   hook   info                     */
#define OWNER_ksem        0xff7e        /* KSEM   semaphore                   */
#define OWNER_lbdd        0xff7f        /* loadable   base   device   driver     */
#define OWNER_lid         0xff80        /* ABIOS   logical   identifier        */
#define OWNER_monitor     0xff81        /* monitor   segment                  */
#define OWNER_mshare      0xff82        /* named - shared                     */
#define OWNER_mshrmp      0xff83        /* RMP   having   mshare   records       */
#define OWNER_nmi         0xff84        /* non   maskable   interrupt          */
#define OWNER_npx         0xff85        /* 287 / 387   save   area                */
#define OWNER_orphan      0xff86        /* orphaned   segment            */
#define OWNER_prof        0xff87        /* profile   support             */
#define OWNER_ptogdt      0xff88        /* Allocated   via               */
                                        /* dh _ allocateGDTSelector      */
#define OWNER_ptovirt     0xff89        /* PhysToVirt                       */
#define OWNER_puse        0xff8a        /* Page   Usage                       */
#define OWNER_pusetmp     0xff8b        /* tmp   Page   Usage                   */
#define OWNER_perfview    0xff8c        /* Perfview                          */
#define OWNER_qscache     0xff8d        /* QuerySysInfo   cache               */
#define OWNER_ras         0xff8e        /* RAS   segment                      */
#define OWNER_resource    0xff8f        /* Resource   BMP   segment            */
#define OWNER_sysserv     0xff90        /* system   service                   */
#define OWNER_timer       0xff91        /* timer   services   segment          */
#define OWNER_traphe      0xff92        /* TRAP   Hard   Error                  */
                                        /* File   System   Owners               */
#define OWNER_fsbuf       0xff93        /* file   system   buffer               */
#define OWNER_cdevtmp     0xff94        /* Char   DEV   TMP                     */
#define OWNER_fsc         0xff95        /* FSC   segment                      */
#define OWNER_dpb         0xff96        /* DPB                               */
#define OWNER_eatmp       0xff97        /* fat   EA   TMP                       */
#define OWNER_fatsrch     0xff98        /* fat   search   segment               */
#define OWNER_gnotify     0xff99        /* FindNotify   global   segment       */
#define OWNER_pnotify     0xff9a        /* FindNotify   private   segment      */
#define OWNER_fsh         0xff9b        /* installable   file   sys   helper     */
#define OWNER_ifs         0xff9c        /* installable   file   system         */
#define OWNER_mfsd        0xff9d        /* mini   file   system                 */
#define OWNER_mft         0xff9e        /* master   file   table                */
#define OWNER_npipebuf    0xff9f        /* Named   pipe   I / O   buffer   segment   */
#define OWNER_pipe        0xffa0        /* pipe                              */
#define OWNER_sft         0xffa1        /* system   file   table                */
#define OWNER_vpb         0xffa2        /* volume   parameter   block          */
                                        /* Loader   Owners                    */
#define OWNER_ldcache     0xffa3        /* Loader   Instance   Data   Cache      */
#define OWNER_ldrdld      0xffa4        /* LDR   Dynamic   Load   record         */
#define OWNER_invalid     0xffa5        /* Cache   being   made                 */
#define OWNER_ldrmte      0xffa6        /* mte                               */
#define OWNER_ldrpath     0xffa7        /* LDR   MTE   path                     */
#define OWNER_ldrnres     0xffa8        /* LDR   non - resident   names          */
#define OWNER_prot16      0xffa9        /* Protect   16   list                  */
                                        /* Boot   Loader   and   Kernel   Owners   */
#define OWNER_os2krnl     0xffaa        /* os2krnl   load   image               */
#define OWNER_os2ldr      0xffab        /* os2ldr   load   image                */
#define OWNER_ripl        0xffac        /* Remote   IPL   ( remote   boot )        */
                                        /* Page   Manager   Owners              */
#define OWNER_pgalias     0xffad        /* Temporary   page   manager   aliases */
#define OWNER_pgbuf       0xffae        /* PG   loader   and   swapper   buffer    */
#define OWNER_pgcrpte     0xffaf        /* PG   Compat .   region   page   table    */
#define OWNER_dbgalias    0xffb0        /* debugger   alias   pte               */
#define OWNER_pgdir       0xffb1        /* PG   Page   directory                */
#define OWNER_pgkstack    0xffb2        /* kernel   stack   region              */
#define OWNER_pgvp        0xffb3        /* VP   array                          */
#define OWNER_pgpf        0xffb4        /* PF   array                          */
#define OWNER_pgprt       0xffb5        /* Page   Range   Table                 */
#define OWNER_pgsyspte    0xffb6        /* PG   System   page   tables           */
                                        /* Selector   Manager   Owners         */
#define OWNER_gdt         0xffb7        /* SEL   GDT                           */
#define OWNER_selheap     0xffb8        /* Selector - mapped   heap   block      */
#define OWNER_ldt         0xffb9        /* SEL   LDT                           */
#define OWNER_lock        0xffba        /* SEL   Lock                          */
#define OWNER_selnop      0xffbb        /* NO - OP   Locks                      */
#define OWNER_seluvirt    0xffbc        /* SEL   UVIRT   mapping                */
                                        /* Semaphore   Owners                 */
#define OWNER_semmisc     0xffbd        /* SEM   Miscellaneous                */
#define OWNER_semmuxq     0xffbe        /* SEM   Mux   Queue                    */
#define OWNER_semopenq    0xffbf        /* SEM   Open   Queue                   */
#define OWNER_semrec      0xffc0        /* SEM   SemRecord                    */
#define OWNER_semstr      0xffc1        /* SEM   string                       */
#define OWNER_semstruc    0xffc2        /* SEM   Main   structure               */
#define OWNER_semtable    0xffc3        /* SEM   Private / Shared   table        */
                                        /* Swapper   Owners                   */
#define OWNER_smdfh       0xffc4        /* SM   Disk   Frame   Heap               */
#define OWNER_smsfn       0xffc5        /* SM   SFN   array                     */
#define OWNER_smsf        0xffc6        /* SM   Swap   Frame                    */
                                        /* Tasking   Owners                   */
#define OWNER_tkextlst    0xffc7        /* TK   Exit   List   record              */
#define OWNER_tkkmreg     0xffc8        /* TK   dispatch   ( KM )   registers      */
#define OWNER_tklibif     0xffc9        /* TK   LibInit   Free   Notification    */
                                        /* record                            */
#define OWNER_tklibi      0xffca        /* TK   LibInit   record                */
#define OWNER_ptda        0xffcb        /* TK   PTDA                           */
#define OWNER_tcb         0xffcc        /* TK   TCB                            */
#define OWNER_tsd         0xffcd        /* TK   TSD                            */
                                        /* VDD ,   VDH ,   VDM   Owners            */
#define OWNER_vddblkh     0xffce        /* VDD   block   header                 */
#define OWNER_vddblk      0xffcf        /* VDD   memory   block                 */
#define OWNER_vddcfstr    0xffd0        /* VDD   config . sys   string           */
#define OWNER_vddctmp     0xffd1        /* VDD   creation   tmp   allocation     */
#define OWNER_vddep       0xffd2        /* VDD   Entry   Point                  */
#define OWNER_vddheaph    0xffd3        /* VDD   heap   header                  */
#define OWNER_vddheap     0xffd4        /* heap   objects   to   load   VDDs       */
#define OWNER_vddhook     0xffd5        /* VDD   hook                          */
#define OWNER_vddla       0xffd6        /* VDD   Linear   Arena   header         */
#define OWNER_vddlr       0xffd7        /* VDD   Linear   arena   Record         */
#define OWNER_vddmod      0xffd8        /* VDD   module   record                */
#define OWNER_vddopen     0xffd9        /* open   VDD   record                  */
#define OWNER_vddpddep    0xffda        /* VDD   PDD   Entry   Point              */
#define OWNER_vddproc     0xffdb        /* VDD   procedure   record            */
#define OWNER_vddstr      0xffdc        /* VDD   string                       */
#define OWNER_vdhfhook    0xffdd        /* VDH   fault   hook                   */
#define OWNER_vdhalloc    0xffde        /* VDH   services   resident   memory    */
#define OWNER_vdhswap     0xffdf        /* VDH   services   swappable   memory   */
#define OWNER_vdmalias    0xffe0        /* VDM   Alias                         */
                                        /* Virtual   Memory   Manager   Owners   */
#define OWNER_vmah        0xffe1        /* VM   arena   header                  */
#define OWNER_vmal        0xffe2        /* VM   Alias   Record                  */
#define OWNER_vmar        0xffe3        /* VM   Arena   Record                  */
#define OWNER_vmbmap      0xffe4        /* VM   Location   Bitmap               */
#define OWNER_vmco        0xffe5        /* VM   Context   Record                */
#define OWNER_vmdead      0xffe6        /* VM   Dead   Object                   */
#define OWNER_vmhsh       0xffe7        /* VM   Location   Hash   Table          */
#define OWNER_vmkrhb      0xffe8        /* VM   * UNKNOWN *   busy   KRHB          */
#define OWNER_vmkrhf      0xffe9        /* VM   free   KRHB                     */
#define OWNER_vmkrhl      0xffea        /* VM   end   KRHB                      */
#define OWNER_vmkrhro     0xffeb        /* VM   Public   Kernel   Resident   R / O   */
                                        /* Heap                              */
#define OWNER_vmkrhrw     0xffec        /* VM   Public   Kernel   Resident   R / W   */
                                        /* Heap                              */
#define OWNER_vmkshd      0xffed        /* VM   Swappable   Heap   Descriptor    */
#define OWNER_vmkshro     0xffee        /* VM   Public   Kernel   Swappable   R / O */
                                        /* Heap                              */
#define OWNER_vmkshrw     0xffef        /* VM   Public   Kernel   Swappable   R / W */
                                        /* Heap                              */
#define OWNER_vmllock     0xfff0        /* VM   long   term   lock   manager       */
#define OWNER_vmob        0xfff1        /* VM   Object   Record                 */
#define OWNER_vmsgs       0xfff2        /* VM   Screen   Group   Switch   record   */
#define OWNER_vmbmp16     0xfff3        /* VM   Temp   buf   ( BMP16 )              */
#define OWNER_shrind      0xfff4        /* reserved   for   shared   indicator   */
#define OWNER_give        0xfff5        /* giveable   segment                 */
#define OWNER_get         0xfff6        /* gettable   segment                 */
#define OWNER_giveget     0xfff7        /* giveable   and   gettable   segment   */
#define OWNER_preload     0xfff8        /* Loader ' s   preload   object         */

#endif