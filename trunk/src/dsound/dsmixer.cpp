/* $Id: dsmixer.cpp,v 1.4 2001-03-06 20:11:17 mike Exp $ */
/*
 * DirectSound Software Mixer
 *
 * Copyright 2000 Michal Necasek
 * Copyright 2000 Ondrej Necasek
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSMISC
#include <os2win.h>

#include <stdlib.h>
#include <string.h>

#define INITGUID
#include <dsound.h>

#include "os2dsound.h"
#include "os2sndbuffer.h"
#include "os2notify.h"
#include <misc.h>

signed long mixbuf[2300*2]; /* enough for 50 ms at 44100Hz stereo */

void MixCallback(ULONG cbMix) {
   // Check for priority level here; for DSSCL_WRITEPRIMARY just play directly
   // from the primary buffer; for all others mix the secondary buffers
   if (OS2IDirectSoundBuffer::primary->parentDS->GetCoopLevel() != DSSCL_WRITEPRIMARY)
      MixFunc(OS2IDirectSoundBuffer::dsbroot, OS2IDirectSoundBuffer::primary, cbMix);
   else {
   }
}

void MixOneBuffer(OS2IDirectSoundBuffer *inBuf, int tomix, int outrate)
{
   unsigned char *data8b;
   signed short *data16b;
   int inpos, spd, i, j, sample, len, bytespersample, vol1, vol2;
   int oldpos;

   bytespersample = inBuf->lpfxFormat->wBitsPerSample *
      inBuf->lpfxFormat->nChannels / 8;
   inpos = inBuf->playpos / bytespersample * 1024 + inBuf->frac;
   spd = 1024 * inBuf->frequency / outrate;
   if (inBuf->pan <= 0)
      vol1 = inBuf->volume;
   else
      vol1 = inBuf->volume * (256 - inBuf->pan) / 256;
   if (inBuf->pan >= 0)
      vol2 = inBuf->volume;
   else
      vol2 = inBuf->volume * (256 + inBuf->pan) / 256;

   len = inBuf->bufferdesc.dwBufferBytes / bytespersample * 1024;
   if ((inBuf->lpfxFormat->nChannels == 2) && (inBuf->lpfxFormat->wBitsPerSample == 16)) {
      data16b = (signed short *) inBuf->lpBuffer;
      for (i = 0; i < tomix; i++) {
         j = inpos / 1024 * 2;
         mixbuf[i*2] += data16b[j] * vol1 / 256;
         mixbuf[i*2+1] += data16b[j+1] * vol2 / 256;
         inpos += spd;
         if (inpos >= len) {
            if (inBuf->fLoop) inpos -= len;
            else {
               inBuf->fPlaying = FALSE;
               if (inBuf->notify != NULL)
                  inBuf->notify->CheckStop();
               break;
            }
         }
      }
   }
   if ((inBuf->lpfxFormat->nChannels == 1) && (inBuf->lpfxFormat->wBitsPerSample == 16)) {
      data16b = (signed short *) inBuf->lpBuffer;
      for (i = 0; i < tomix; i++) {
         j = inpos / 1024;
         sample = data16b[j];
         mixbuf[i*2] += sample * vol1 / 256;
         mixbuf[i*2+1] += sample * vol2 / 256;
         inpos += spd;
         if (inpos >= len) {
            if (inBuf->fLoop) inpos -= len;
            else {
               inBuf->fPlaying = FALSE;
               if (inBuf->notify != NULL)
                  inBuf->notify->CheckStop();
               break;
            }
         }
      }
   }
   if ((inBuf->lpfxFormat->nChannels == 2) && (inBuf->lpfxFormat->wBitsPerSample == 8)) {
      data8b = (unsigned char *) inBuf->lpBuffer;
      for (i = 0; i < tomix; i++) {
         j = inpos / 1024 * 2;
         sample = ((int) data8b[j] - 128) * vol1;
         mixbuf[i*2] += sample;
         sample = ((int) data8b[j+1] - 128) * vol2;
         mixbuf[i*2+1] += sample;
         inpos += spd;
         if (inpos >= len) {
            if (inBuf->fLoop) inpos -= len;
            else {
               inBuf->fPlaying = FALSE;
               if (inBuf->notify != NULL)
                  inBuf->notify->CheckStop();
               break;
            }
         }
      }
   }
   if ((inBuf->lpfxFormat->nChannels == 1) && (inBuf->lpfxFormat->wBitsPerSample == 8)) {
      data8b = (unsigned char *) inBuf->lpBuffer;
      for (i = 0; i < tomix; i++) {
         j = inpos / 1024;
         sample = (int) data8b[j] - 128;
         mixbuf[i*2] += sample * vol1;
         mixbuf[i*2+1] += sample * vol2;
         inpos += spd;
         if (inpos >= len) {
            if (inBuf->fLoop) inpos -= len;
            else {
               inBuf->fPlaying = FALSE;
               if (inBuf->notify != NULL)
                  inBuf->notify->CheckStop();
               break;
            }
         }
      }
   }
   oldpos         = inBuf->playpos;
   inBuf->playpos = inpos / 1024 * bytespersample;
   inBuf->frac    = inpos % 1024;

   // Check if any notifications are to be signaled
   if (inBuf->notify != NULL)
      inBuf->notify->CheckPos(oldpos, inBuf->playpos);

   // keep the write cursor about 15ms ahead of the play cursor
   inBuf->writepos  = inBuf->playpos + inBuf->frequency * bytespersample / 67;
   inBuf->writepos %= inBuf->bufferdesc.dwBufferBytes;
}

void MixFunc (OS2IDirectSoundBuffer *firstBuf, OS2IDirectSoundBuffer *outBuf,
   ULONG cbMix) {
   OS2IDirectSoundBuffer *inBuf = firstBuf;
   int i, outbits, outrate, outnch, tomix, outpos, outlen;
   unsigned char *data8b;
   signed short *data16b;

   outbits = outBuf->lpfxFormat->wBitsPerSample;
   outrate = outBuf->lpfxFormat->nSamplesPerSec;
   outnch = outBuf->lpfxFormat->nChannels;
   tomix = cbMix * 8 / outbits / outnch;

   memset(&mixbuf[0], 0, tomix * 2 * sizeof(mixbuf[0]));

   while (inBuf != NULL) {
      if (inBuf->fPlaying) {
         MixOneBuffer(inBuf, tomix, outrate);
      }
      inBuf = inBuf->next;
   }

   outpos = outBuf->playpos * 8 / outbits;
   outlen = outBuf->bufferdesc.dwBufferBytes * 8 / outbits;
   if (outbits == 16) {
      data16b = (signed short *) outBuf->lpBuffer;
      for (i = 0; i < tomix * outnch; i++) {
         if (mixbuf[i] <= -32768) data16b[outpos] = -32768;
         else if (mixbuf[i] >= 32767) data16b[outpos] = 32767;
         else data16b[outpos] = (signed short)mixbuf[i];
         outpos++;
         if (outpos >= outlen) outpos = 0;
      }
   } else {
      data8b = (unsigned char *) outBuf->lpBuffer;
      for (i = 0; i < tomix * outnch; i++) {
         if (mixbuf[i] <= -32768) data8b[outpos] = 0;
         else if (mixbuf[i] >= 32767) data8b[outpos] = 255;
         else data8b[outpos] = (signed short)mixbuf[i] / 256 + 128;
         outpos++;
         if (outpos >= outlen) outpos = 0;
      }
   }
   outBuf->playpos = outpos * outbits / 8;
}

