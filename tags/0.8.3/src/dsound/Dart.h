
#define INVALID_PARMS 1
#define OUT_OF_MEMORY 2


#ifdef DART_DSOUND

  #define E_NOTIMPL                   0x80004001
  #define CLASS_E_NOAGGREGATION       0x80040110
  #define E_OUTOFMEMORY               0x8007000E
  #define E_INVALIDARG                0x80070057
  #define E_FAIL                      0x80000008

  #define DS_OK                       0
  #define DSERR_ALLOCATED             10
  #define DSERR_CONTROLUNAVAIL        30
  #define DSERR_INVALIDPARAM          E_INVALIDARG
  #define DSERR_INVALIDCALL           50
  #define DSERR_GENERIC               E_FAIL
  #define DSERR_PRIOLEVELNEEDED       70
  #define DSERR_OUTOFMEMORY           E_OUTOFMEMORY
  #define DSERR_BADFORMAT             100
  #define DSERR_UNSUPPORTED           E_NOTIMPL
  #define DSERR_NODRIVER              120
  #define DSERR_ALREADYINITIALIZED    130
  #define DSERR_NOAGGREGATION         CLASS_E_NOAGGREGATION
  #define DSERR_BUFFERLOST            150
  #define DSERR_OTHERAPPHASPRIO       160
  #define DSERR_UNINITIALIZED         170

#endif


#define BUFFER_SIZE 32768

extern  ULONG ulNumDartBuffs;

long Dart_Open_Device(USHORT *pusDeviceID, void **vpMixBuffer, void **vpMixSetup,
                      void  **vpBuffParms, void **ppvBuffer);


long Dart_Close_Device(USHORT usDeviceID, void *vpMixBuffer, void *vpMixSetup,
                       void *vpBuffParms );

long Dart_GetPosition(USHORT usDeviceID, LONG *pulPosition);
long Dart_Stop(USHORT usDeviceID);
long Dart_Play(USHORT usDeviceID, void *vpMixSetup, void *vpMixBuffer, long playing);
long Dart_SetFormat(USHORT *pusDeviceID, void *vpMixSetup, void *vpBufferParms,
                    void **vpMixBuffer, LONG lBPS, LONG lSPS, LONG lChannels );

