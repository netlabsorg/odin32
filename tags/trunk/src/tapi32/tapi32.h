/* $Id: tapi32.h,v 1.3 1999-06-10 16:54:47 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * TAPI2032 stubs
 *
 * Copyright 1998 Felix Maschek
 *
 */
#ifndef __TAPI32_H__
#define __TAPI32_H__

typedef ULONG*       HLINEAPP;
typedef ULONG*       LPLINETRANSLATEOUTPUT;
typedef ULONG*       LPLINETRANSLATECAPS;
typedef ULONG*       HCALL, LPHCALL;
typedef ULONG*       HLINE;
typedef ULONG*       LPVARSTRING;

typedef ULONG*       LPLINEFORWARDLIST;
typedef ULONG*       LPLINEGENERATETONE;
typedef ULONG*       LPLINEADDRESSCAPS;
typedef ULONG*       LPLINEADDRESSSTATUS;
typedef ULONG*       LPLINECALLINFO;
typedef ULONG*       LPLINECALLSTATUS;
typedef ULONG*       LPLINECALLLIST;

typedef ULONG*       LPLINEDEVCAPS;
typedef ULONG*       LPLINECALLLIST;
typedef ULONG*       LPHICON;
typedef ULONG*       LPLINEDEVSTATUS;
typedef ULONG*       LPLINECALLPARAMS;
typedef ULONG*       LPLINEMONITORTONE;
typedef ULONG*       LPLINEEXTENSIONID;
typedef ULONG*       LPHLINE;
typedef ULONG*       LPLINECALLPARAMS;
typedef ULONG*       LPLINEDIALPARAMS;

typedef ULONG*       LPLINEMEDIACONTROLDIGIT;
typedef ULONG*       LPLINECALLPARAMS;
typedef ULONG*       LPLINECALLPARAMS;

typedef ULONG*       LPHLINEAPP;
typedef ULONG*       LPLINEMEDIACONTROLMEDIA;
typedef ULONG*       HPHONE;

typedef ULONG*       LINECALLBACK;
typedef ULONG*       LPLINEMEDIACONTROLTONE;
typedef ULONG*       LPPHONEBUTTONINFO;
typedef ULONG*       LPPHONESTATUS;
typedef ULONG*       LPLINEMEDIACONTROLCALLSTATE;

typedef ULONG*       HPHONEAPP;
typedef ULONG*       LPLINECOUNTRYLIST;

typedef ULONG*       LPPHONECAPS;
typedef ULONG*       LPPHONEEXTENSIONID;
typedef ULONG*       LPHPHONE;
typedef ULONG*       LPLINEPROVIDERLIST;

typedef ULONG*       LPHPHONEAPP;
typedef ULONG*       PHONECALLBACK;

#define LINEERR_ALLOCATED                       0x80000001
#define LINEERR_BADDEVICEID                     0x80000002
#define LINEERR_BEARERMODEUNAVAIL               0x80000003
#define LINEERR_CALLUNAVAIL                     0x80000005
#define LINEERR_COMPLETIONOVERRUN               0x80000006
#define LINEERR_CONFERENCEFULL                  0x80000007
#define LINEERR_DIALBILLING                     0x80000008
#define LINEERR_DIALDIALTONE                    0x80000009
#define LINEERR_DIALPROMPT                      0x8000000A
#define LINEERR_DIALQUIET                       0x8000000B
#define LINEERR_INCOMPATIBLEAPIVERSION          0x8000000C
#define LINEERR_INCOMPATIBLEEXTVERSION          0x8000000D
#define LINEERR_INIFILECORRUPT                  0x8000000E
#define LINEERR_INUSE                           0x8000000F
#define LINEERR_INVALADDRESS                    0x80000010
#define LINEERR_INVALADDRESSID                  0x80000011
#define LINEERR_INVALADDRESSMODE                0x80000012
#define LINEERR_INVALADDRESSSTATE               0x80000013
#define LINEERR_INVALAPPHANDLE                  0x80000014
#define LINEERR_INVALAPPNAME                    0x80000015
#define LINEERR_INVALBEARERMODE                 0x80000016
#define LINEERR_INVALCALLCOMPLMODE              0x80000017
#define LINEERR_INVALCALLHANDLE                 0x80000018
#define LINEERR_INVALCALLPARAMS                 0x80000019
#define LINEERR_INVALCALLPRIVILEGE              0x8000001A
#define LINEERR_INVALCALLSELECT                 0x8000001B
#define LINEERR_INVALCALLSTATE                  0x8000001C
#define LINEERR_INVALCALLSTATELIST              0x8000001D
#define LINEERR_INVALCARD                       0x8000001E
#define LINEERR_INVALCOMPLETIONID               0x8000001F
#define LINEERR_INVALCONFCALLHANDLE             0x80000020
#define LINEERR_INVALCONSULTCALLHANDLE          0x80000021
#define LINEERR_INVALCOUNTRYCODE                0x80000022
#define LINEERR_INVALDEVICECLASS                0x80000023
#define LINEERR_INVALDEVICEHANDLE               0x80000024
#define LINEERR_INVALDIALPARAMS                 0x80000025
#define LINEERR_INVALDIGITLIST                  0x80000026
#define LINEERR_INVALDIGITMODE                  0x80000027
#define LINEERR_INVALDIGITS                     0x80000028
#define LINEERR_INVALEXTVERSION                 0x80000029
#define LINEERR_INVALGROUPID                    0x8000002A
#define LINEERR_INVALLINEHANDLE                 0x8000002B
#define LINEERR_INVALLINESTATE                  0x8000002C
#define LINEERR_INVALLOCATION                   0x8000002D
#define LINEERR_INVALMEDIALIST                  0x8000002E
#define LINEERR_INVALMEDIAMODE                  0x8000002F
#define LINEERR_INVALMESSAGEID                  0x80000030
#define LINEERR_INVALPARAM                      0x80000032
#define LINEERR_INVALPARKID                     0x80000033
#define LINEERR_INVALPARKMODE                   0x80000034
#define LINEERR_INVALPOINTER                    0x80000035
#define LINEERR_INVALPRIVSELECT                 0x80000036
#define LINEERR_INVALRATE                       0x80000037
#define LINEERR_INVALREQUESTMODE                0x80000038
#define LINEERR_INVALTERMINALID                 0x80000039
#define LINEERR_INVALTERMINALMODE               0x8000003A
#define LINEERR_INVALTIMEOUT                    0x8000003B
#define LINEERR_INVALTONE                       0x8000003C
#define LINEERR_INVALTONELIST                   0x8000003D
#define LINEERR_INVALTONEMODE                   0x8000003E
#define LINEERR_INVALTRANSFERMODE               0x8000003F
#define LINEERR_LINEMAPPERFAILED                0x80000040
#define LINEERR_NOCONFERENCE                    0x80000041
#define LINEERR_NODEVICE                        0x80000042
#define LINEERR_NODRIVER                        0x80000043
#define LINEERR_NOMEM                           0x80000044
#define LINEERR_NOREQUEST                       0x80000045
#define LINEERR_NOTOWNER                        0x80000046
#define LINEERR_NOTREGISTERED                   0x80000047
#define LINEERR_OPERATIONFAILED                 0x80000048
#define LINEERR_OPERATIONUNAVAIL                0x80000049
#define LINEERR_RATEUNAVAIL                     0x8000004A
#define LINEERR_RESOURCEUNAVAIL                 0x8000004B
#define LINEERR_REQUESTOVERRUN                  0x8000004C
#define LINEERR_STRUCTURETOOSMALL               0x8000004D
#define LINEERR_TARGETNOTFOUND                  0x8000004E
#define LINEERR_TARGETSELF                      0x8000004F
#define LINEERR_UNINITIALIZED                   0x80000050
#define LINEERR_USERUSERINFOTOOBIG              0x80000051
#define LINEERR_REINIT                          0x80000052
#define LINEERR_ADDRESSBLOCKED                  0x80000053
#define LINEERR_BILLINGREJECTED                 0x80000054
#define LINEERR_INVALFEATURE                    0x80000055
#define LINEERR_NOMULTIPLEINSTANCE              0x80000056

#define PHONEERR_ALLOCATED                      0x90000001
#define PHONEERR_BADDEVICEID                    0x90000002
#define PHONEERR_INCOMPATIBLEAPIVERSION         0x90000003
#define PHONEERR_INCOMPATIBLEEXTVERSION         0x90000004
#define PHONEERR_INIFILECORRUPT                 0x90000005
#define PHONEERR_INUSE                          0x90000006
#define PHONEERR_INVALAPPHANDLE                 0x90000007
#define PHONEERR_INVALAPPNAME                   0x90000008
#define PHONEERR_INVALBUTTONLAMPID              0x90000009
#define PHONEERR_INVALBUTTONMODE                0x9000000A
#define PHONEERR_INVALBUTTONSTATE               0x9000000B
#define PHONEERR_INVALDATAID                    0x9000000C
#define PHONEERR_INVALDEVICECLASS               0x9000000D
#define PHONEERR_INVALEXTVERSION                0x9000000E
#define PHONEERR_INVALHOOKSWITCHDEV             0x9000000F
#define PHONEERR_INVALHOOKSWITCHMODE            0x90000010
#define PHONEERR_INVALLAMPMODE                  0x90000011
#define PHONEERR_INVALPARAM                     0x90000012
#define PHONEERR_INVALPHONEHANDLE               0x90000013
#define PHONEERR_INVALPHONESTATE                0x90000014
#define PHONEERR_INVALPOINTER                   0x90000015
#define PHONEERR_INVALPRIVILEGE                 0x90000016
#define PHONEERR_INVALRINGMODE                  0x90000017
#define PHONEERR_NODEVICE                       0x90000018
#define PHONEERR_NODRIVER                       0x90000019
#define PHONEERR_NOMEM                          0x9000001A
#define PHONEERR_NOTOWNER                       0x9000001B
#define PHONEERR_OPERATIONFAILED                0x9000001C
#define PHONEERR_OPERATIONUNAVAIL               0x9000001D
#define PHONEERR_RESOURCEUNAVAIL                0x9000001F
#define PHONEERR_REQUESTOVERRUN                 0x90000020
#define PHONEERR_STRUCTURETOOSMALL              0x90000021
#define PHONEERR_UNINITIALIZED                  0x90000022
#define PHONEERR_REINIT                         0x90000023

#endif
