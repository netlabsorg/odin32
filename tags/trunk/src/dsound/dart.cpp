/* $Id: dart.cpp,v 1.4 2001-03-06 20:11:16 mike Exp $ */
/*
 *  Dart Interface..
 *
 *  Kevin Langman
 *
 *  Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_DOS
#define INCL_OS2MM
#include <os2wrap.h>
#include <os2mewrap.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DART_DSOUND
#include "dart.h"
#include "dsmixer.h"

#include <misc.h>

static MCI_MIXSETUP_PARMS *MixSetup_Global;
static long               lLastBuff;
static char               *pDSoundBuff;
static BOOL               fIsPlaying = FALSE;

USHORT               usDeviceID;                 /* Amp Mixer device id          */

/* TODO: scrap this variable! */
MCI_MIX_BUFFER       *pMixBuffers;               /* Device buffers               */

MCI_MIXSETUP_PARMS   MixSetupParms;              /* Mixer parameters             */
MCI_BUFFER_PARMS     BufferParms;                /* Device buffer parms          */
ULONG                ulNumDartBuffs;             /* # of DART buffers            */

#define ULONG_LOWD(ul)  (*(USHORT *)((ULONG *)(&ul)))     /* Low  Word */

//******************************************************************************
//******************************************************************************
LONG APIENTRY OS2_Dart_Update(ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
   ULONG           rc;

   if( ( (ulFlags == MIX_WRITE_COMPLETE) ||
         ((ulFlags == (MIX_WRITE_COMPLETE | MIX_STREAM_ERROR))&&
         (ulStatus == ERROR_DEVICE_UNDERRUN)) )
     )
   {
      lLastBuff++;
      if (lLastBuff == ulNumDartBuffs){
         lLastBuff = 0;
      }

      if( fIsPlaying == FALSE /*&& lLastBuff == 0*/ ){
         mciSendCommand(usDeviceID, MCI_STOP, MCI_WAIT, NULL, 0);
         return TRUE;
      }

      /* Now mix sound from all playing secondary SoundBuffers into the primary buffer */
      MixCallback(BUFFER_SIZE/ulNumDartBuffs);

      /* Fill The next buff from the DSound Buffer */
      memcpy( pMixBuffers[lLastBuff].pBuffer, &pDSoundBuff[lLastBuff*(BUFFER_SIZE/ulNumDartBuffs)], BUFFER_SIZE/ulNumDartBuffs );

      /* Send the NEXT Buffer to Dart for playing! */
      rc = MixSetup_Global->pmixWrite(MixSetup_Global->ulMixHandle, &pMixBuffers[lLastBuff], 1 );
   }

   return TRUE;
}


long Dart_Open_Device(USHORT *pusDeviceID, void **vpMixBuffer, void **vpMixSetup,
                      void **vpBuffParms,  void **ppvBuffer)
{

   MCI_AMP_OPEN_PARMS  AmpOpenParms;
   ULONG   rc, ulNew;
   LONG    lAdd = 5;
   short   device = 0;

   dprintf(("DSOUND-DART: Dart_Open_Device"));

   /* TODO: remove eventually... */
   DosSetRelMaxFH(&lAdd, &ulNew);

   ulNumDartBuffs = BUFFER_SIZE * 44100 / (2 * 2 * 2048 * 22050);

   pMixBuffers = (MCI_MIX_BUFFER*)malloc(sizeof(MCI_MIX_BUFFER) * ulNumDartBuffs);

   *vpMixBuffer    = pMixBuffers;
   *vpMixSetup     = &MixSetupParms;
   *vpBuffParms    = &BufferParms;

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
   //AmpOpenParms.pszDeviceType = (PSZ)MCI_DEVTYPE_AUDIO_AMPMIX;
   AmpOpenParms.pszDeviceType = (PSZ)MAKEULONG(MCI_DEVTYPE_AUDIO_AMPMIX, (USHORT)device);

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
   // figure out where it can and can't write. I have emulating this by
   // using many smaller buffers and do locking by tracking what buffer that is
   // currently playing. This maybe less CPU friendly then other methods but
   // it's the best my little brain could come up with!!

   MixSetupParms.ulBufferSize = BUFFER_SIZE / ulNumDartBuffs;

   BufferParms.ulNumBuffers   = ulNumDartBuffs;
   BufferParms.ulBufferSize   = MixSetupParms.ulBufferSize;
   BufferParms.pBufList       = pMixBuffers;

   rc = mciSendCommand(usDeviceID, MCI_BUFFER, MCI_WAIT | MCI_ALLOCATE_MEMORY,
                                (PVOID)&BufferParms, 0);
   if ( ULONG_LOWD(rc) != MCIERR_SUCCESS ) {
      dprintf(("DSOUND-DART: MCI_BUFFER (Alloc) %d", rc));
      mciSendCommand(*pusDeviceID, MCI_CLOSE, MCI_WAIT, NULL, 0);
      return DSERR_OUTOFMEMORY;
   }

   /* Clear the Buffer */
   // Set initial values to 32767 to avoid clicks on start of playback.
   for (device = 0; device < ulNumDartBuffs; device++) {
      memset(pMixBuffers[device].pBuffer, 32767, BUFFER_SIZE/ulNumDartBuffs);
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

   *pulPosition = (lLastBuff * (BUFFER_SIZE/ulNumDartBuffs)) + (BUFFER_SIZE/ulNumDartBuffs);
   if (*pulPosition > BUFFER_SIZE)
      *pulPosition = 0;

   dprintf(("DSOUND-DART: Returning %d", *pulPosition));

   return DS_OK;
}

long Dart_SetFormat(USHORT *pusDeviceID, void *vpMixSetup, void *vpBufferParms, void **vpMixBuffer, LONG lBPS, LONG lSPS, LONG lChannels )
{
   ULONG   rc;
   MCI_MIXSETUP_PARMS  *MixSetup;
   MCI_BUFFER_PARMS    *BufferParms;
   MCI_AMP_OPEN_PARMS  AmpOpenParms;
   short               device = 0;

   /* Recalculate the number of DART buffers based on the new data rate      */
   /* Note: the factor 2048 means a 2K buffer, 1024 = 4K, 512 = 8K and so on */
   ulNumDartBuffs = BUFFER_SIZE * 44100 / ((lBPS / 8) * lChannels * 2048 * lSPS);

   /* Reallocate the MCI_MIX_BUFFER array */
   free(pMixBuffers);
   pMixBuffers = (MCI_MIX_BUFFER*)malloc(sizeof(MCI_MIX_BUFFER) * ulNumDartBuffs);

   MixSetup     = (MCI_MIXSETUP_PARMS*)vpMixSetup;
   BufferParms  = (MCI_BUFFER_PARMS*)vpBufferParms;
   *vpMixBuffer = pMixBuffers;


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
   memset(pMixBuffers, 0, sizeof(MCI_MIX_BUFFER) * ulNumDartBuffs);
   BufferParms->ulStructLength = sizeof(MCI_BUFFER_PARMS);
   BufferParms->ulNumBuffers   = ulNumDartBuffs;
   BufferParms->ulBufferSize   = BUFFER_SIZE/ulNumDartBuffs;
   BufferParms->pBufList       = pMixBuffers;
   pMixBuffers->pBuffer        = NULL;

   rc = mciSendCommand(*pusDeviceID, MCI_BUFFER, MCI_WAIT | MCI_ALLOCATE_MEMORY,
                       (PVOID)BufferParms, 0);
   if (rc != MCIERR_SUCCESS) {
      dprintf(("DSOUND-DART: MCI_BUFFER_ALLOCATE_MEMORY (SetFormat) %d", rc));
      mciSendCommand(*pusDeviceID, MCI_CLOSE, MCI_WAIT, NULL, 0);
      memset(pMixBuffers, 0, sizeof(MCI_MIX_BUFFER) * ulNumDartBuffs);
      return DSERR_OUTOFMEMORY;
   }

   /* Clear the Buffer */
   // If the data is 8bit  then set values to 127
   // If the data is 16bit then set values to 32767
   // Doing this will avoid the clicks at the beging of playback! :)
   for (int i=0; i<ulNumDartBuffs; i++) {
      memset(pMixBuffers[i].pBuffer, lBPS == 8 ? 127 : 32767, BUFFER_SIZE / ulNumDartBuffs);
   }

   lLastBuff = 0; /* we have to reset this, the number of buffers probably changed! */

   /* If the primary buffer was playing, we have to restart it!! */
   if (fIsPlaying) {
      dprintf(("DSOUND-DART: Restarting playback!!!!"));

      /* Mix the first buffer before playing */
      MixCallback(BUFFER_SIZE/ulNumDartBuffs);
      memcpy(pMixBuffers[lLastBuff].pBuffer, &pDSoundBuff[lLastBuff*(BUFFER_SIZE/ulNumDartBuffs)], BUFFER_SIZE/ulNumDartBuffs);

      USHORT  sel = RestoreOS2FS();
      /* Note: the call to pmixWrite trashes the FS selector, we have to save */
      /* and then restore FS!!! Otherwise exception handling will be broken.  */
      MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle, pMixBuffers, 2);
      SetFS(sel);
      fIsPlaying = TRUE;
   }

   return DS_OK;
}


long Dart_Stop(USHORT usDeviceID)
{
   dprintf(("DSOUND-DART: Dart_Stop"));

   if (!fIsPlaying)
      return DS_OK;

   fIsPlaying = FALSE;

   // The OS2_Dart_Update function is now used to send the MCI_STOP!!
   // Doing this fixes a bug where after a Dart_Stop call the sound would
   // continue to loop because the OS2_Dart_Update would send the next
   // buffer causing Dart to start again..

   //rc = mciSendCommand(usDeviceID, MCI_STOP, MCI_WAIT, NULL, 0);
   //if (rc != MCIERR_SUCCESS) {
   //   { FILE *dbf; dbf=fopen("log.log", "a"); fprintf( dbf, "Error in MCI_STOP...\n"); fclose(dbf); }
   //   dprintf(("DSOUND-DART: MCI_PAUSE %d", rc));
   //   return DSERR_GENERIC;
   //}


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
      MixCallback(BUFFER_SIZE/ulNumDartBuffs);
      memcpy(MixBuffer[lLastBuff].pBuffer, &pDSoundBuff[lLastBuff*(BUFFER_SIZE/ulNumDartBuffs)], BUFFER_SIZE/ulNumDartBuffs);

      USHORT  sel = RestoreOS2FS();
      /* Note: the call to pmixWrite trashes the FS selector, we have to save */
      /* and then restore FS!!! Otherwise exception handling will be broken.  */
      MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle, MixBuffer, 2);
      SetFS(sel);
      fIsPlaying = TRUE;
   }

   return DS_OK;
}
