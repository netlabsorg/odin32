/*
 *  Kevin Langman
 *
 *  The Dart functions are here to avoid the conficts with the
 *  the OS/2 mcios2.h, meerror.h, os2medef.h and mmsystem.h
 *
 */

#define INCL_DOS
#define INCL_OS2MM
#include <os2wrap.h>
#include <os2mewrap.h>

#include <stdlib.h>
#include <string.h>

#define DART_DSOUND
#include "dart.h"
#include "dsmixer.h"

#include <misc.h>

static MCI_MIXSETUP_PARMS *MixSetup_Global;
static MCI_MIX_BUFFER     *pMixBuffs;
static long               lLastBuff;
static char               *pDSoundBuff;
static BOOL               fIsPlaying = FALSE;

USHORT               usDeviceID;                 /* Amp Mixer device id          */
MCI_MIX_BUFFER       MixBuffers[NUM_DART_BUFFS]; /* Device buffers               */
MCI_MIXSETUP_PARMS   MixSetupParms;              /* Mixer parameters             */
MCI_BUFFER_PARMS     BufferParms;                /* Device buffer parms          */


#define ULONG_LOWD(ul)  (*(USHORT *)((ULONG *)(&ul)))     /* Low  Word */

//******************************************************************************
//******************************************************************************
LONG APIENTRY OS2_Dart_Update(ULONG ulStatus,PMCI_MIX_BUFFER pBuffer,ULONG ulFlags)
{
   //MCI_STATUS_PARMS Status;
   ULONG           rc;

   if ((ulFlags == MIX_WRITE_COMPLETE) ||
       ((ulFlags == (MIX_WRITE_COMPLETE | MIX_STREAM_ERROR))&&
       (ulStatus == ERROR_DEVICE_UNDERRUN)))
   {
      lLastBuff++;
      if (lLastBuff == NUM_DART_BUFFS)
         lLastBuff = 0;

      /* Now mix sound from all playing secondary SoundBuffers into the primary buffer */
      MixCallback(BUFFER_SIZE/NUM_DART_BUFFS);

      /* Fill The next buff from the DSound Buffer */
      memcpy( pMixBuffs[lLastBuff].pBuffer, &pDSoundBuff[lLastBuff*(BUFFER_SIZE/NUM_DART_BUFFS)], BUFFER_SIZE/NUM_DART_BUFFS );

      /* Send the NEXT Buffer to Dart for playing! */
      rc = MixSetup_Global->pmixWrite(MixSetup_Global->ulMixHandle, &pMixBuffs[lLastBuff], 1 );
      //dprintf(("DSOUND-DART: Playing Next Buffer %d", rc));
   }

   return TRUE;
}


long Dart_Open_Device(USHORT *pusDeviceID, void **vpMixBuffer, void **vpMixSetup,
                      void **vpBuffParms,  void **ppvBuffer)
{

   MCI_AMP_OPEN_PARMS  AmpOpenParms;
   //MCI_WAVE_SET_PARMS WaveSetParms;
   ULONG   rc, ulNew;
   LONG    lAdd = 5;
   short   device = 0;

   dprintf(("DSOUND-DART: Dart_Open_Device"));

   DosSetRelMaxFH(&lAdd, &ulNew);

   *vpMixBuffer    = &MixBuffers;
   *vpMixSetup     = &MixSetupParms;
   *vpBuffParms    = &BufferParms;

   pMixBuffs = &MixBuffers[0];
   lLastBuff = 0;

   /* Is there a way to avoid the use of the MixSetup_Global ????? */
   MixSetup_Global = &MixSetupParms;
   /****************************************************************/

   // If the DSOUND_DEVICE is set then use that number as the device for DART.
   // this will allow people with many sound cards to use the card of their choice
   // for an instance of a DSOUND enabled app!
   if (getenv("DSOUND_DEVICE") != NULL) {
      device = atoi(getenv("DSOUND_DEVICE"));
   }

   /* Open the AmpMix Device and start processing the buffer! */
   memset(&AmpOpenParms, 0, sizeof(MCI_AMP_OPEN_PARMS));
   AmpOpenParms.usDeviceID    = 0;
   AmpOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_AUDIO_AMPMIX;

   rc = mciSendCommand(0, MCI_OPEN, MCI_WAIT | MCI_OPEN_TYPE_ID, (PVOID)&AmpOpenParms, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_OPEN %d", rc));
      return DSERR_GENERIC;
   }
   *pusDeviceID = AmpOpenParms.usDeviceID;
   usDeviceID   = AmpOpenParms.usDeviceID;

   /* setup playback parameters */
   memset(&MixSetupParms, 0, sizeof(MCI_MIXSETUP_PARMS));
   MixSetupParms.ulBitsPerSample  = 16;
   MixSetupParms.ulFormatTag      = MCI_WAVE_FORMAT_PCM;
   MixSetupParms.ulSamplesPerSec  = 22500;
   MixSetupParms.ulChannels       = 2;
   MixSetupParms.ulFormatMode     = MCI_PLAY;
   MixSetupParms.ulDeviceType     = MCI_DEVTYPE_WAVEFORM_AUDIO;
   MixSetupParms.pmixEvent        = OS2_Dart_Update;

   rc = mciSendCommand(usDeviceID, MCI_MIXSETUP, MCI_WAIT | MCI_MIXSETUP_INIT,
                      (PVOID)&MixSetupParms, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_MIXSETUP (Constructor) %d", rc));
      return DSERR_GENERIC;
   }

   /* Create the Audio Buffer */
   // OK... Since DSound only uses 1 buffer and uses the GetPosition API to
   // figure out where it can and can't write, I have emulating this by
   // using 16 1kb buffers and do locking by tracking what buffer is currently
   // playing. This maybe less CPU friendly then other methods but it's the best
   // my little brain could come up with!!

   MixSetupParms.ulBufferSize = BUFFER_SIZE/NUM_DART_BUFFS;

//   BufferParms->ulStructLength = sizeof(MCI_BUFFER_PARMS);
   BufferParms.ulNumBuffers   = NUM_DART_BUFFS;
   BufferParms.ulBufferSize   = MixSetupParms.ulBufferSize;
   BufferParms.pBufList       = MixBuffers;

   rc = mciSendCommand(usDeviceID, MCI_BUFFER, MCI_WAIT | MCI_ALLOCATE_MEMORY,
                                (PVOID)&BufferParms, 0);
   if ( ULONG_LOWD(rc) != MCIERR_SUCCESS ) {
      dprintf(("DSOUND-DART: MCI_BUFFER (Alloc) %d", rc));
      mciSendCommand(*pusDeviceID, MCI_CLOSE, MCI_WAIT, NULL, 0);
      return DSERR_OUTOFMEMORY;
   }

   /* Clear the Buffer */
   // Todo:  I don't know if Dart expects Signed or Unsigned data... This will
   // ok until I look in to this... Also it may be that Dart uses signed for
   // 8bit data and unsigned for 16it.. Anyhow the worst that can happen by setting
   // the buffer to 0 is a click on Dart_Play and a Clink when real sound is written
   // to the buffer!
   for (device = 0; device < NUM_DART_BUFFS; device++) {
      memset(MixBuffers[device].pBuffer, 0, BUFFER_SIZE/NUM_DART_BUFFS);
   }

   // Allocate memory for the DSound "Holder" buffer.
   // TODO: Move this to the Constructor for OS2PrimBuff
   //       so that the Deconstructor can be used to free the memory!
   *(char**)ppvBuffer = (char*)malloc(BUFFER_SIZE);
   if (*ppvBuffer == NULL) {
      return DSERR_OUTOFMEMORY;
   }
   pDSoundBuff = (char*)(*ppvBuffer);

   dprintf(("DSOUND-DART: Dart_Open_Device Exiting"));

   return DS_OK;
}

long Dart_Close_Device(USHORT usDeviceID, void *vpMixBuffer, void *vpMixSetup,
                       void *vpBuffParms )
{
   MCI_MIX_BUFFER      *MixBuffer;
   MCI_MIXSETUP_PARMS  *MixSetup;
   MCI_BUFFER_PARMS    *BufferParms;
   ULONG               rc;

   dprintf(("DSOUND-DART: Dart_Close_Device"));

   MixBuffer   = (MCI_MIX_BUFFER*)vpMixBuffer;
   MixSetup    = (MCI_MIXSETUP_PARMS*)vpMixSetup;
   BufferParms = (MCI_BUFFER_PARMS*)vpBuffParms;

   rc = mciSendCommand(usDeviceID, MCI_BUFFER, MCI_WAIT | MCI_DEALLOCATE_MEMORY, BufferParms, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_BUFFER (Close) %d", rc));
   }
   rc = mciSendCommand(usDeviceID, MCI_CLOSE, MCI_WAIT, NULL, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_CLOSE (Close) %d", rc));
   }

   dprintf(("DSOUND-DART: Dart_Close_Device returning DS_OK"));
   return DS_OK;
}


long Dart_GetPosition(USHORT usDeviceID, LONG *pulPosition)
{
   dprintf(("DSOUND-DART: Dart_GetPosition"));

   *pulPosition = (lLastBuff * (BUFFER_SIZE/NUM_DART_BUFFS)) + (BUFFER_SIZE/NUM_DART_BUFFS);
   if (*pulPosition > BUFFER_SIZE)
      *pulPosition = 0;

   dprintf(("DSOUND-DART: Returning %d", *pulPosition));

   return DS_OK;
}

long Dart_SetFormat(USHORT *pusDeviceID, void *vpMixSetup, void *vpBufferParms, void *vpMixBuffer, LONG lBPS, LONG lSPS, LONG lChannels )
{
   ULONG   rc;
   MCI_MIXSETUP_PARMS  *MixSetup;
   MCI_BUFFER_PARMS    *BufferParms;
   MCI_MIX_BUFFER      *MixBuffer;
   MCI_AMP_OPEN_PARMS  AmpOpenParms;
   short               device = 0;

   MixSetup    = (MCI_MIXSETUP_PARMS*)vpMixSetup;
   BufferParms = (MCI_BUFFER_PARMS*)vpBufferParms;
   MixBuffer   = (MCI_MIX_BUFFER*)vpMixBuffer;

   dprintf(("DSOUND-DART: Dart_SetFormat"));

   // If the DSOUND_DEVICE is set then use that number as the device for DART.
   // this will allow people with many sound cards to use the card of there choice
   // for an instance of a DSOUND enabled app!
   if (getenv("DSOUND_DEVICE") != NULL) {
      device = atoi(getenv( "DSOUND_DEVICE" ));
   }

   /* Dealloc to avoid the 5511 error */
   rc = mciSendCommand(*pusDeviceID, MCI_BUFFER, MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                       BufferParms, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_DEALLOCATE_MEMORY (SetFormat) %d", rc));
      return DSERR_GENERIC;
   }

   rc = mciSendCommand(*pusDeviceID, MCI_CLOSE, MCI_WAIT, NULL, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_CLOSE (SetFormat) %d", rc));
      return(DSERR_GENERIC);
   }

   /* Reopen the MixAmp Device and start processing the buffer! */
   memset(&AmpOpenParms,0,sizeof(MCI_AMP_OPEN_PARMS));
   AmpOpenParms.usDeviceID    = 0;
   AmpOpenParms.pszDeviceType = (PSZ)MAKEULONG(MCI_DEVTYPE_AUDIO_AMPMIX, (USHORT)device);

   rc = mciSendCommand(0, MCI_OPEN, MCI_WAIT | MCI_OPEN_TYPE_ID, (PVOID)&AmpOpenParms, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_OPEN %d", rc));
      return DSERR_GENERIC;
   }
   *pusDeviceID = AmpOpenParms.usDeviceID;

   /* setup playback parameters */
   memset(MixSetup, 0, sizeof(MCI_MIXSETUP_PARMS));
   MixSetup->ulBitsPerSample  = lBPS;
   MixSetup->ulFormatTag      = MCI_WAVE_FORMAT_PCM;
   MixSetup->ulSamplesPerSec  = lSPS;
   MixSetup->ulChannels       = lChannels;
   MixSetup->ulFormatMode     = MCI_PLAY;
   MixSetup->ulDeviceType     = MCI_DEVTYPE_WAVEFORM_AUDIO;
   MixSetup->pmixEvent        = OS2_Dart_Update;

   rc = mciSendCommand(*pusDeviceID, MCI_MIXSETUP, MCI_WAIT | MCI_MIXSETUP_INIT,
                       (PVOID)MixSetup, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_MIXSETUP (SetFormat) %d", rc));
      return DSERR_GENERIC;
   }

   memset(BufferParms, 0, sizeof(MCI_BUFFER_PARMS));
   memset(MixBuffer,   0, sizeof(MCI_MIX_BUFFER) * NUM_DART_BUFFS);
   BufferParms->ulStructLength = sizeof(MCI_BUFFER_PARMS);
   BufferParms->ulNumBuffers   = NUM_DART_BUFFS;
   BufferParms->ulBufferSize   = BUFFER_SIZE/NUM_DART_BUFFS;
   BufferParms->pBufList       = MixBuffer;
   MixBuffer->pBuffer          = NULL;

   rc = mciSendCommand(*pusDeviceID, MCI_BUFFER, MCI_WAIT | MCI_ALLOCATE_MEMORY,
                       (PVOID)BufferParms, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_BUFFER_ALLOCATE_MEMORY (SetFormat) %d", rc));
      mciSendCommand(*pusDeviceID, MCI_CLOSE, MCI_WAIT, NULL, 0);
      memset(MixBuffer, 0, sizeof(MCI_MIX_BUFFER) * NUM_DART_BUFFS);
      return DSERR_OUTOFMEMORY;
   }

   /* Clear the Buffer */
   // Todo:  I don't know if Dart expects Signed or Unsigned data... This will
   // ok until I look in to this... Also it may be that Dart uses signed for
   // 8bit data and unsigned for 16it.. Anyhow the worst that can happen by setting
   // the buffer to 0 is a click on Dart_Play and a Clink when real sound is written
   // to the buffer!
   for (int i=0; i<NUM_DART_BUFFS; i++) {
      memset(MixBuffer[i].pBuffer, 0, BUFFER_SIZE/NUM_DART_BUFFS);
   }

   /* If the primary buffer was playing, we have to restart it!! */
   if (fIsPlaying) {
      dprintf(("DSOUND-DART: Restarting playback!!!!"));

      /* Mix the first buffer before playing */
      MixCallback(BUFFER_SIZE/NUM_DART_BUFFS);
      memcpy(pMixBuffs[lLastBuff].pBuffer, &pDSoundBuff[lLastBuff*(BUFFER_SIZE/NUM_DART_BUFFS)], BUFFER_SIZE/NUM_DART_BUFFS);

      USHORT  sel = RestoreOS2FS();
      /* Note: the call to pmixWrite trashes the FS selector, we have to save */
      /* and then restore FS!!! Otherwise exception handling will be broken.  */
      MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle, MixBuffers, 2);
      SetFS(sel);
      fIsPlaying = TRUE;
   }

   return DS_OK;
}


long Dart_Stop(USHORT usDeviceID)
{
   ULONG   rc;

   dprintf(("DSOUND-DART: Dart_Stop"));

   if (!fIsPlaying)
      return DS_OK;

   fIsPlaying = FALSE;
//   rc = mciSendCommand(usDeviceID, MCI_PAUSE, MCI_WAIT, NULL, 0);
   rc = mciSendCommand(usDeviceID, MCI_STOP, MCI_WAIT, NULL, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_PAUSE %d", rc));
      return DSERR_GENERIC;
   }
   return DS_OK;
}

long Dart_Play(USHORT usDeviceID, void *vpMixSetup, void *vpMixBuffer, long playing)
{
   ULONG   rc;
   MCI_MIXSETUP_PARMS  *MixSetup;
   MCI_MIX_BUFFER      *MixBuffer;

   MixSetup  = (MCI_MIXSETUP_PARMS*)vpMixSetup;
   MixBuffer = (MCI_MIX_BUFFER*)vpMixBuffer;

   dprintf(("DSOUND-DART: Dart_Play"));

   if (playing == TRUE) {
      rc = mciSendCommand(usDeviceID, MCI_RESUME, MCI_WAIT, NULL, 0);
      if (rc != MCIERR_SUCCESS) {
         dprintf(("DSOUND-DART: MCI_RESUME %d", rc));
         return DSERR_GENERIC;
      }
   } else { //if (playing==FALSE)
      dprintf(("DSOUND-DART: Playback started!!!!"));

      /* Mix the first buffer before playing */
      MixCallback(BUFFER_SIZE/NUM_DART_BUFFS);
      memcpy(pMixBuffs[lLastBuff].pBuffer, &pDSoundBuff[lLastBuff*(BUFFER_SIZE/NUM_DART_BUFFS)], BUFFER_SIZE/NUM_DART_BUFFS);

      USHORT  sel = RestoreOS2FS();
      /* Note: the call to pmixWrite trashes the FS selector, we have to save */
      /* and then restore FS!!! Otherwise exception handling will be broken.  */
      MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle, MixBuffers, 2);
      SetFS(sel);
      fIsPlaying = TRUE;
   }

   return DS_OK;
}
