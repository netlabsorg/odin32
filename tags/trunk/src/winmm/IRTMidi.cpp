/* $Id: IRTMidi.cpp,v 1.7 2001-02-26 20:14:38 sandervl Exp $ */

/*******************************************************************************
* FILE NAME: IRTMidi.cpp                                                       *
*                                                                              *
* DESCRIPTION:                                                                 *
*   Class implementation of the class:                                         *
*     IRTMidi- Real Time Midi Base Class                                       *
*                                                                              *
* Copyright Joel Troster                                                       *
*
* Project Odin Software License can be found in LICENSE.TXT
*
*******************************************************************************/


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#define INCL_DOS
#include <os2wrap.h>     //Odin32 OS/2 api wrappers
#include <win32type.h>
#include "IRTMidi.hpp"
#include <meerror.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#define DBG_LOCALLOG	DBG_irtmidi
#include "dbglocal.h"

IRTMidi * IRTMidi::iTheIRTMidiSingleton = NULL;

//------------------------------------------------------------------------------
// IRTMidi::IRTMidi
//------------------------------------------------------------------------------
IRTMidi::IRTMidi()
  : iHardwareClass( 0 )
  , iApplicationClass( 1 )
  , iRTMidiModule( NULL )
  , iNumInInstances( 0 )
  , iNumOutInstances( 0 )
{
  unsigned long rc = 0;

  // Remember my address
  iTheIRTMidiSingleton = this;

  // Get RTMIDI.DLL Module Handle procedures
  rc = getRTMidiProcs();
//fprintf( stderr, "Get Procs rc=%d\n", rc );

  // Initialize MIDI Setup structure and call MIDISetup
  iSetup.ulStructLength         = sizeof( MIDISETUP );
  iSetup.pulMaxRTSysexLength    = &maxRTSysexLen;
  iSetup.ppulMIDICurrentTime    = &iCurrentTime;
  iSetup.ulFlags                = 0;
  iSetup.pfnMIDI_NotifyCallback = NULL;
  iSetup.hwndCallback           = 0;
  iSetup.hqueueCallback         = 0;
  if ( rc == 0 )
    rc = MidiSetup( &iSetup, 0 );
//fprintf( stderr, "MidiSetup rc=%d\n", rc );

  // Clear all Instance slots
  unsigned int i;
  for ( i = 0; i < MAX_INSTANCES; i++ )
  {
     iInstances[ i ] = NULL;
     iInInstances[ i ] = NULL;
     iOutInstances[ i ] = NULL;
  }

  // Query Classes & Instances, filling in the slots
  if ( rc == 0 ) rc = getClasses();
  if ( rc == 0 ) rc = getInstances();
  setLastRC( rc );

  // Start up the MIDI timer
  if ( rc == 0 )
    startTimer();
}

unsigned long IRTMidi::getRTMidiProcs()
{
  char loadError[256] = "";
  unsigned long rc;
  PFN modAddr;
  rc = DosLoadModule( loadError, sizeof(loadError), "RTMIDI", &iRTMidiModule );
  if ( rc ) return rc;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDICreateInstance", &modAddr );
  if ( rc ) return rc;
  pfnMidiCreateInstance =  (ULONG(*APIENTRY )( ULONG, MINSTANCE*, PSZ, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIDeleteInstance", &modAddr );
  if ( rc ) return rc;
  pfnMidiDeleteInstance =  (ULONG(*APIENTRY )(MINSTANCE, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIEnableInstance", &modAddr );
  if ( rc ) return rc;
  pfnMidiEnableInstance =  (ULONG(*APIENTRY )(MINSTANCE, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIDisableInstance", &modAddr );
  if ( rc ) return rc;
  pfnMidiDisableInstance = (ULONG(*APIENTRY )(MINSTANCE, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIAddLink", &modAddr );
  if ( rc ) return rc;
  pfnMidiAddLink =         (ULONG(*APIENTRY )(MINSTANCE, MINSTANCE, ULONG, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIRemoveLink", &modAddr );
  if ( rc ) return rc;
  pfnMidiRemoveLink =      (ULONG(*APIENTRY )(MINSTANCE, MINSTANCE, ULONG, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIQueryClassList", &modAddr );
  if ( rc ) return rc;
  pfnMidiQueryClassList    = (ULONG(*APIENTRY )(ULONG, PMIDICLASSINFO, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIQueryInstanceList", &modAddr );
  if ( rc ) return rc;
  pfnMidiQueryInstanceList = (ULONG(*APIENTRY )(ULONG, PMIDIINSTANCEINFO, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIQueryNumClasses", &modAddr );
  if ( rc ) return rc;
  pfnMidiQueryNumClasses   = (ULONG(*APIENTRY )(PULONG, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIQueryNumInstances", &modAddr );
  if ( rc ) return rc;
  pfnMidiQueryNumInstances = (ULONG(*APIENTRY )(PULONG, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDISendMessages", &modAddr );
  if ( rc ) return rc;
  pfnMidiSendMessages      = (ULONG(*APIENTRY )(PMESSAGE, ULONG, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDISendSysexMessage", &modAddr );
  if ( rc ) return rc;
  pfnMidiSendSysexMessage  = (ULONG(*APIENTRY )(PMESSAGE, ULONG, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDIRetrieveMessages", &modAddr );
  if ( rc ) return rc;
  pfnMidiRetrieveMessages  = (ULONG(*APIENTRY )(MINSTANCE, PVOID, PULONG, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDISetup", &modAddr );
  if ( rc ) return rc;
  pfnMidiSetup             = (ULONG(*APIENTRY)(PMIDISETUP, ULONG)) modAddr;

  rc = DosQueryProcAddr( iRTMidiModule, 0L, "MIDITimer", &modAddr );
  if ( rc ) return rc;
  pfnMidiTimer             = (ULONG(*APIENTRY )(ULONG, ULONG)) modAddr;

  return 0;
}

//------------------------------------------------------------------------------
// IRTMidi::~IRTMidi
//------------------------------------------------------------------------------
IRTMidi::~IRTMidi()
{
  stopTimer();
  delInstances();
  iTheIRTMidiSingleton = NULL;
}

void IRTMidi::setLastRC(unsigned long theRC)
{
  iLastRC = theRC;
}

void IRTMidi::resetLastRC()
{
  setLastRC( 0 );
}

//------------------------------------------------------------------------------
// IRTMidi :: RCExplanation
//------------------------------------------------------------------------------
char * IRTMidi::RCExplanation() const
{
   char * DebugString;
   switch( iLastRC )
   {
   case 0:
      DebugString = "OK";
      break;

   // Timer error messages
   case TIMERERR_INVALID_PARAMETER:
      DebugString = "MIDI Timer: Invalid Parameter";
      break;
   case TIMERERR_INTERNAL_SYSTEM:
      DebugString = "MIDI Timer: Internal System Error";
      break;

   // RTMIDI messages
   case MIDIERR_DUPLICATE_INSTANCE_NAME:
      DebugString = "MIDIERR_DUPLICATE_INSTANCE_NAME";
      break;
   case MIDIERR_HARDWARE_FAILED:
      DebugString    = "MIDIERR_HARDWARE_FAILED";
      break;
   case MIDIERR_INTERNAL_SYSTEM:
      DebugString = "MIDIERR_INTERNAL_SYSTEM";
      break;
   case MIDIERR_INVALID_BUFFER_LENGTH:
      DebugString = "MIDIERR_INVALID_BUFFER_LENGTH";
      break;
   case MIDIERR_INVALID_CLASS_NUMBER:
      DebugString    = "MIDIERR_INVALID_CLASS_NUMBER";
      break;
   case MIDIERR_INVALID_CONFIG_DATA:
      DebugString    = "MIDIERR_INVALID_CONFIG_DATA";
      break;
   case MIDIERR_INVALID_FLAG:
      DebugString    = "MIDIERR_INVALID_FLAG";
      break;
   case MIDIERR_INVALID_INSTANCE_NAME:
      DebugString    = "MIDIERR_INVALID_INSTANCE_NAME";
      break;
   case MIDIERR_INVALID_INSTANCE_NUMBER:
      DebugString    = "MIDIERR_INVALID_INSTANCE_NUMBER";
      break;
   case MIDIERR_INVALID_PARAMETER:
      DebugString = "MIDIERR_INVALID_PARAMETER";
      break;
   case MIDIERR_INVALID_SETUP:
      DebugString = "MIDIERR_INVALID_SETUP";
      break;
   case MIDIERR_NO_DRIVER:
      DebugString    = "MIDIERR_NO_DRIVER";
      break;
   case MIDIERR_NO_DEFAULT_HW_NODE:
      DebugString    = "MIDIERR_NO_DEFAULT_HW_NODE";
      break;
   case MIDIERR_NOT_ALLOWED:
      DebugString    = "MIDIERR_NOT_ALLOWED";
      break;
   case MIDIERR_NOTIFY_MISSED:
      DebugString    = "MIDIERR_NOTIFY_MISSED";
      break;
   case MIDIERR_RESOURCE_NOT_AVAILABLE:
      DebugString    = "MIDIERR_RESOURCE_NOT_AVAILABLE";
      break;
   case MIDIERR_SENDONLY:
      DebugString    = "MIDIERR_SENDONLY";
      break;
   case MIDIERR_RECEIVEONLY:
      DebugString    = "MIDIERR_RECEIVEONLY";
      break;

   default:
      DebugString    = "Beats Me!";
      break;
   }
   return DebugString;
}

//------------------------------------------------------------------------------
// IRTMidi :: getClasses (private)
//------------------------------------------------------------------------------
unsigned long IRTMidi::getClasses()
{
   unsigned long rc;
   unsigned long nc = 0;
   PMIDICLASSINFO ci = NULL;

   // Query classes and get the Hardware & Application class numbers
   rc = MidiQueryNumClasses( &nc, 0 );
   if ( rc == 0 && nc > 0 )
   {
      ci = new MIDICLASSINFO[ nc ];
      rc = MidiQueryClassList( nc, ci, 0 );
   }
   if ( rc == 0 && ci )
   {
      for ( int i = 0; i < nc; i++ )
      {
         if ( strcmp( ci[i].szmClassName, "Application" ) == 0 )
            iApplicationClass = ci[i].ulClassNumber;
         if ( strcmp( ci[i].szmClassName, "Hardware" )    == 0 )
            iHardwareClass    = ci[i].ulClassNumber;
      }
   }
   if ( ci )
   {
      delete[] ci;
   }
   return rc;
}

unsigned long IRTMidi::findFreeInstance() const
{
  unsigned long i;
  for ( i = 0; i < MAX_INSTANCES; i++ )
  {
    if ( iInstances[ i ] == NULL )
      return i;
  }
  return MAX_INSTANCES; // should not happen to a dog
}

//------------------------------------------------------------------------------
// IRTMidi :: addInstance
//------------------------------------------------------------------------------
MINSTANCE IRTMidi::addInstance( IMidiInstance * theInstance,
                                ULONG classNum, char * instanceName )
{
   unsigned long rc;
   MINSTANCE newInstance;
   unsigned long ni = 0;
   PMIDIINSTANCEINFO ci = NULL;

   // Add new instance
   rc = MidiCreateInstance( classNum,
                            &newInstance,
                            instanceName,
                            0 );
   // Now find it to initialize the Instance Object
   if ( rc == 0 )
   {
      rc = MidiQueryNumInstances( &ni, 0 );
      if ( rc == 0 && ni > 0 )
      {
         ci = new MIDIINSTANCEINFO[ ni ];
         rc = MidiQueryInstanceList( ni, ci, 0 );
      }
   }
   if ( rc == 0 && ci )
   {
      for ( int i = 0; i < ni; i++ )
      {
         if ( ci[i].minstance == newInstance )
         {
            theInstance->setInstanceInfo( &ci[i] );
            iInstances[ findFreeInstance() ] = theInstance;
         }
      }
   }
   if ( ci )
   {
      delete[] ci;
   }
   setLastRC( rc );
   return newInstance;
}

//------------------------------------------------------------------------------
// IRTMidi :: delInstance
//------------------------------------------------------------------------------
void IRTMidi::delInstance( IMidiInstance * theInstance )
{
   unsigned long rc;

   // Find instance in array and null it
   for( unsigned int i = 0; i < MAX_INSTANCES; i++ )
   {
     if ( iInstances[ i ] == theInstance )
     {
        iInstances[ i ] = NULL;
     }
   }

   // Delete MIDI Instance
   rc = MidiDeleteInstance( theInstance->instance(), 0 );
   setLastRC( rc );
}

//------------------------------------------------------------------------------
// IRTMidi :: delInstances (private)
//------------------------------------------------------------------------------
void IRTMidi::delInstances()
{
   // Delete all instances this way as delete of an instance removes
   // it from the instance list through the miracle of double
   // dispatching.  See above.
   for( unsigned int i = 0; i < MAX_INSTANCES; i++ )
   {
      IMidiInstance * ins = iInstances[ i ];
      if ( ins )
         delete ins;
   }
}

//------------------------------------------------------------------------------
// IRTMidi :: getInstances (hardware only)
//------------------------------------------------------------------------------
unsigned long IRTMidi::getInstances()
{
  // Query all Instances
  unsigned long rc;
  unsigned long ni = 0;
  PMIDIINSTANCEINFO ci = NULL;

  iNumInInstances  = 0;
  iNumOutInstances = 0;

  // Query instances and post them
  rc = MidiQueryNumInstances( &ni, 0 );
  if ( rc == 0 && ni > 0 )
  {
    ci = new MIDIINSTANCEINFO[ ni ];
    rc = MidiQueryInstanceList( ni, ci, 0 );
  }
  if ( rc == 0 && ci )
  {
    for ( int i = 0; i < ni; i++ )
    {
      if ( ci[i].ulClassNumber == hardwareClass() )
      {
        IMidiInstance * ins = new IMidiInstance();
        ins ->setInstanceInfo( &ci[i] );
        iInstances[ i ] = ins;
        if ( ins->canSend() )
        {
          iInInstances[ iNumInInstances ] = ins;
          iNumInInstances++;
        }
        if ( ins->canReceive() )
        {
          iOutInstances[ iNumOutInstances ] = ins;
          iNumOutInstances++;
        }
      }
    }
  }
  if ( ci )
  {
    delete[] ci;
  }
  return rc;
}

void IRTMidi::startTimer() 
{
  ((IRTMidi*)this)->setLastRC( MidiTimer( MIDI_START_TIMER, 0 ) );
}

void IRTMidi::stopTimer() 
{
  ((IRTMidi*)this)->setLastRC( MidiTimer( MIDI_STOP_TIMER, 0 ) );
}

IRTMidi* IRTMidi::instance()
{
  if ( iTheIRTMidiSingleton == NULL )
     iTheIRTMidiSingleton = new IRTMidi();
  return iTheIRTMidiSingleton;
}

void IRTMidiShutdown()
{
  IRTMidi::shutdown();
}

void IRTMidi::shutdown()
{
  if ( iTheIRTMidiSingleton != NULL )
    delete iTheIRTMidiSingleton;
}

ULONG IRTMidi::MidiCreateInstance( ULONG a, MINSTANCE* b, PSZ c, ULONG d)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiCreateInstance)(a,b,c,d);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiDeleteInstance( MINSTANCE a, ULONG b)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiDeleteInstance)(a,b);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiEnableInstance( MINSTANCE a, ULONG b )
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiEnableInstance)(a,b);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiDisableInstance( MINSTANCE a, ULONG b)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiDisableInstance)(a,b);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiAddLink( MINSTANCE a, MINSTANCE b, ULONG c, ULONG d )
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiAddLink)(a,b,c,d);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiRemoveLink( MINSTANCE a, MINSTANCE b, ULONG c, ULONG d )
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiRemoveLink)(a,b,c,d);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiQueryClassList( ULONG a, PMIDICLASSINFO b, ULONG c)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiQueryClassList)(a,b,c);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiQueryInstanceList ( ULONG a, PMIDIINSTANCEINFO b, ULONG c)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiQueryInstanceList)(a,b,c);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiQueryNumClasses   ( PULONG a, ULONG b)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiQueryNumClasses)(a,b);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiQueryNumInstances ( PULONG a, ULONG b)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiQueryNumInstances)(a,b);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiSendMessages( PMESSAGE a, ULONG b, ULONG c )
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiSendMessages)(a,b,c);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiSendSysexMessage( PMESSAGE a, ULONG b, ULONG c )
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiSendSysexMessage)(a,b,c);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiRetrieveMessages( MINSTANCE a, PVOID b, PULONG c, ULONG d)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiRetrieveMessages)(a,b,c,d);
  SetFS(sel);
  return ret;
}
ULONG IRTMidi::MidiSetup( PMIDISETUP a, ULONG b)
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiSetup)(a,b);
  SetFS(sel);
  return ret;
}

ULONG IRTMidi::MidiTimer( ULONG a, ULONG b )
{
  ULONG ret;
  USHORT sel = GetFS();
  ret = (*pfnMidiTimer)(a,b);
  SetFS(sel);
  return ret;
}

//------------------------------------------------------------------------------
// IMidiInstance::IMidiInstance
//------------------------------------------------------------------------------
IMidiInstance::IMidiInstance()
{
}

//------------------------------------------------------------------------------
// IMidiInstance::~IMidiInstance
//------------------------------------------------------------------------------
IMidiInstance::~IMidiInstance()
{
  // Disable send and receive if set
  if ( isReceive() ) enableReceive( FALSE );
  if ( isSend() )     enableSend( FALSE );
  // Remove from RTMIDI
  IRTMIDI->delInstance( this );
}

//------------------------------------------------------------------------------
// IMidiInstance :: setInfo
//------------------------------------------------------------------------------
void IMidiInstance::setInstanceInfo( MIDIINSTANCEINFO* i )
{
   memcpy( &iInfo, i, sizeof( MIDIINSTANCEINFO ) );
}

MINSTANCE IMidiInstance::instance() const
{
  return iInfo.minstance;
}

unsigned long IMidiInstance::classNum() const
{
  return iInfo.ulClassNumber;
}

char * IMidiInstance::name()
{
  return iInfo.szmInstanceName;
}

unsigned long IMidiInstance::numLinks() const
{
  return iInfo.ulNumLinks;
}

unsigned long IMidiInstance::attributes() const
{
  return iInfo.ulAttributes;
}

BOOL IMidiInstance::isSend() const
{
  return ( iInfo.ulAttributes & MIDI_INST_ATTR_ENABLE_S );
}

IMidiInstance& IMidiInstance::enableSend(BOOL enable)
{
  if (!(isSend() == enable))
  {
    unsigned long rc;
    if ( enable )
    {
      // Enable send
      rc = IRTMIDI->MidiEnableInstance( instance(), MIDI_ENABLE_SEND );
      if ( rc == 0 )
         iInfo.ulAttributes |= MIDI_INST_ATTR_ENABLE_S;
    }
    else
    {
      // Disable send
      rc = IRTMIDI->MidiDisableInstance( instance(), MIDI_DISABLE_SEND );
      if ( rc == 0 )
         iInfo.ulAttributes &= ~MIDI_INST_ATTR_ENABLE_S;
    }
    IRTMIDI->setLastRC( rc );
  }
  return *this;
}

BOOL IMidiInstance::isReceive() const
{
  return ( iInfo.ulAttributes & MIDI_INST_ATTR_ENABLE_R );
}

IMidiInstance& IMidiInstance::enableReceive(BOOL enable)
{
  if (!(isReceive() == enable))
  {
    unsigned long rc;
    if ( enable )
    {
      // Enable receive
      rc = IRTMIDI->MidiEnableInstance( instance(), MIDI_ENABLE_RECEIVE );
      if ( rc == 0 )
         iInfo.ulAttributes |= MIDI_INST_ATTR_ENABLE_R;
    }
    else
    {
      // Disable receive
      rc = IRTMIDI->MidiDisableInstance( instance(), MIDI_DISABLE_RECEIVE );
      if ( rc == 0 )
         iInfo.ulAttributes &= ~MIDI_INST_ATTR_ENABLE_R;
    }
    IRTMIDI->setLastRC( rc );
  }
  return *this;
}

BOOL IMidiInstance::canReceive() const
{
  return ( iInfo.ulAttributes & MIDI_INST_ATTR_CAN_RECV );
}

BOOL IMidiInstance::canSend() const
{
  return ( iInfo.ulAttributes & MIDI_INST_ATTR_CAN_SEND );
}

IMidiInstance& IMidiInstance::removeLink(IMidiInstance* toLink)
{
  if ( canSend() && toLink->canReceive() )
  {
    IRTMIDI->setLastRC(
         IRTMIDI->MidiRemoveLink( instance(), toLink->instance(),
                      0,       // slot?
                      0 ) );
  }
  return *this;
}

IMidiInstance& IMidiInstance::addLink(IMidiInstance* toLink)
{
  if ( canSend() && toLink->canReceive() )
  {
    if ( !isSend() )
    {
      enableSend();
    }
    if ( !toLink->isReceive() )
    {
      toLink->enableReceive();
    }
    IRTMIDI->setLastRC(
       IRTMIDI->MidiAddLink( instance(), toLink->instance(),
                    0,       // slot ?
                    0 ) );
   }
  return *this;
}

//------------------------------------------------------------------------------
// IMidiInstance :: sendMessage
//------------------------------------------------------------------------------
void IMidiInstance::sendMessage( UCHAR b1, UCHAR b2, UCHAR b3, UCHAR b4 ) const
{
   // Build message
   MESSAGE msg;
   msg.ulSourceInstance = instance();
   msg.ulTime           = 0;
   msg.ulTrack          = 0;
   msg.msg.abData[0]    = b1;
   msg.msg.abData[1]    = b2;
   msg.msg.abData[2]    = b3;
   msg.msg.abData[3]    = b4;
   IRTMIDI->setLastRC( IRTMIDI->MidiSendMessages( &msg, 1, 0 ) );
}

//------------------------------------------------------------------------------
// IMidiInstance :: sendMessage
//------------------------------------------------------------------------------
void IMidiInstance::sendMessage( ULONG inMsg ) const
{
   // Build message
   MESSAGE msg;
   msg.ulSourceInstance = instance();
   msg.ulTime           = 0;
   msg.ulTrack          = 0;
   msg.msg.ulMessage    = inMsg;
   IRTMIDI->setLastRC( IRTMIDI->MidiSendMessages( &msg, 1, 0 ) );
}

//------------------------------------------------------------------------------
// IMidiInstance :: sendSysexMessage
//------------------------------------------------------------------------------
void IMidiInstance::sendSysexMessage( UCHAR* theMsg, ULONG msgLen ) const
{
   // Build message
   MESSAGE* msg;
   ULONG outLen = sizeof(MESSAGE) - 4 + msgLen;
   msg = (MESSAGE*)malloc( outLen );

   msg->ulSourceInstance = instance();
   msg->ulTime           = 0;
   msg->ulTrack          = 0;
   memcpy( msg->msg.abData, theMsg, outLen );
   IRTMIDI->setLastRC( IRTMIDI->MidiSendSysexMessage( msg, outLen, 0 ) );
   free(msg);
}

unsigned long IMidiInstance::getMessage( ULONG * pTime, ULONG * pMsg )
{
   MESSAGE theMessage;
   unsigned long numUCHARs;
   unsigned long rc = 0;
   numUCHARs = sizeof( theMessage );
   rc = IRTMIDI->MidiRetrieveMessages( instance(), &theMessage, &numUCHARs, 0 );
   if ( rc == 0 )
   {
      *pTime = theMessage.ulTime;
      *pMsg  = theMessage.msg.ulMessage;
   }
   IRTMIDI->setLastRC( rc );
   return rc;
}

//------------------------------------------------------------------------------
// IAppMidiInstance::IAppMidiInstance
//------------------------------------------------------------------------------
unsigned long IAppMidiInstance::appNum = 0;

IAppMidiInstance::IAppMidiInstance( unsigned long attrs )
{
   iInfo.ulAttributes = attrs;
   char name[32];
   appNum++;
   sprintf( name, "MIDI App(%d)", appNum );

   // Create application instance
   MINSTANCE AppInstance = IRTMIDI->addInstance(
                       this,
                       IRTMIDI->applicationClass(),
                       name );
}

IAppMidiInstance::~IAppMidiInstance()
{
}


