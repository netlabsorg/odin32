/*******************************************************************************
* FILE NAME: IRTMidi.hpp                                                       *
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
#ifndef _IRMIDI_
#define _IRMIDI_

#ifdef _OS2WIN_H
#include <winos2def.h>
#endif

#include <mididll.h>

#define MAX_INSTANCES 32

class IMidiInstance {
public:
  IMidiInstance();

   virtual  ~IMidiInstance();

   void setInstanceInfo( MIDIINSTANCEINFO * theInstanceInfo );
   MINSTANCE instance() const;
   unsigned long classNum() const;
   char * name();
   unsigned long numLinks() const;
   unsigned long attributes() const;
   BOOL isSend() const;
   IMidiInstance& enableSend(BOOL enable = TRUE);
   BOOL isReceive() const;
   IMidiInstance& enableReceive(BOOL enable = TRUE);
   BOOL canReceive() const;
   BOOL canSend() const;
   IMidiInstance& removeLink(IMidiInstance* toInstance);
   IMidiInstance& addLink(IMidiInstance* toInstance);

   unsigned long getMessage( ULONG * pTime, ULONG * pMsg );
   void sendMessage( UCHAR b1, UCHAR b2, UCHAR b3, UCHAR b4 ) const;
   void sendMessage( ULONG theMsg  ) const;
   void sendSysexMessage( UCHAR* theMsg, ULONG msgLen  ) const;

   MIDIINSTANCEINFO iInfo;

};  // IMidiInstance

class IAppMidiInstance : public IMidiInstance
{
public:
  IAppMidiInstance( unsigned long attrs );

  virtual  ~IAppMidiInstance();
private:
  static unsigned long appNum;
};  // IAppMidiInstance

// *****************************************************************************
// Class declaration for IRTMidi
// *****************************************************************************
class IRTMidi {
public:
  //----------------------------------------------------------------------------
  // Constructors / destructors
  //----------------------------------------------------------------------------
  IRTMidi();

  virtual  ~IRTMidi();

public:
  void            startTimer();
  void            stopTimer();
  MINSTANCE       addInstance(IMidiInstance* theInstance, ULONG classNum, char * );
  void            delInstance(IMidiInstance* theInstance);
  void            resetLastRC();
  unsigned long   lastRC() const
                  { return iLastRC; }
  void            setLastRC(unsigned long theRC);
  unsigned long   applicationClass() const
                  { return iApplicationClass; };
  unsigned long   hardwareClass() const
                  { return iHardwareClass; };
  char *          RCExplanation() const;
  static IRTMidi* instance();
  static void     shutdown();

  unsigned int    numInInstances() const
                  { return iNumInInstances; };
  IMidiInstance*  inInstance( unsigned int i ) const
                  { return iInInstances[i]; };
  unsigned int    numOutInstances() const
                  { return iNumOutInstances; };
  IMidiInstance*  outInstance( unsigned int i ) const
                  { return iOutInstances[i]; };
  unsigned long   currentTime() const
                  { return *iCurrentTime; };

  // Methods for calls to RTMIDI functions
  ULONG (*APIENTRY pfnMidiCreateInstance)    ( ULONG, MINSTANCE*, PSZ, ULONG );
  ULONG (*APIENTRY pfnMidiDeleteInstance)    ( MINSTANCE, ULONG );
  ULONG (*APIENTRY pfnMidiEnableInstance)    ( MINSTANCE, ULONG );
  ULONG (*APIENTRY pfnMidiDisableInstance)   ( MINSTANCE, ULONG );
  ULONG (*APIENTRY pfnMidiAddLink)           ( MINSTANCE, MINSTANCE, ULONG, ULONG );
  ULONG (*APIENTRY pfnMidiRemoveLink)        ( MINSTANCE, MINSTANCE, ULONG, ULONG );
  ULONG (*APIENTRY pfnMidiQueryClassList)    ( ULONG, PMIDICLASSINFO, ULONG );
  ULONG (*APIENTRY pfnMidiQueryInstanceList) ( ULONG, PMIDIINSTANCEINFO, ULONG );
  ULONG (*APIENTRY pfnMidiQueryNumClasses)   ( PULONG, ULONG );
  ULONG (*APIENTRY pfnMidiQueryNumInstances) ( PULONG, ULONG );
  ULONG (*APIENTRY pfnMidiSendMessages)      ( PMESSAGE, ULONG, ULONG );
  ULONG (*APIENTRY pfnMidiSendSysexMessage)  ( PMESSAGE, ULONG, ULONG );
  ULONG (*APIENTRY pfnMidiRetrieveMessages)  ( MINSTANCE, PVOID, PULONG, ULONG );
  ULONG (*APIENTRY pfnMidiSetup)             ( PMIDISETUP, ULONG );
  ULONG (*APIENTRY pfnMidiTimer)             ( ULONG, ULONG );

  ULONG MidiCreateInstance    ( ULONG, MINSTANCE*, PSZ, ULONG );
  ULONG MidiDeleteInstance    ( MINSTANCE, ULONG );
  ULONG MidiEnableInstance    ( MINSTANCE, ULONG );
  ULONG MidiDisableInstance   ( MINSTANCE, ULONG );
  ULONG MidiAddLink           ( MINSTANCE, MINSTANCE, ULONG, ULONG );
  ULONG MidiRemoveLink        ( MINSTANCE, MINSTANCE, ULONG, ULONG );
  ULONG MidiQueryClassList    ( ULONG, PMIDICLASSINFO, ULONG );
  ULONG MidiQueryInstanceList ( ULONG, PMIDIINSTANCEINFO, ULONG );
  ULONG MidiQueryNumClasses   ( PULONG, ULONG );
  ULONG MidiQueryNumInstances ( PULONG, ULONG );
  ULONG MidiSendMessages      ( PMESSAGE, ULONG, ULONG );
  ULONG MidiSendSysexMessage  ( PMESSAGE, ULONG, ULONG );
  ULONG MidiRetrieveMessages  ( MINSTANCE, PVOID, PULONG, ULONG );
  ULONG MidiSetup             ( PMIDISETUP, ULONG );
  ULONG MidiTimer             ( ULONG, ULONG );

private:
  unsigned long              getRTMidiProcs();
  unsigned long              getClasses();
  unsigned long              getInstances();
  void                       delInstances();
  unsigned long              findFreeInstance() const;

  unsigned long              iLastRC;

  IMidiInstance*             iInstances[MAX_INSTANCES+1];
  unsigned int               iNumInInstances;
  IMidiInstance*             iInInstances[MAX_INSTANCES+1];
  unsigned int               iNumOutInstances;
  IMidiInstance*             iOutInstances[MAX_INSTANCES+1];
  unsigned long              iApplicationClass;
  unsigned long              iHardwareClass;
  static IRTMidi*            iTheIRTMidiSingleton;
  MIDISETUP                  iSetup;
  unsigned long              maxRTSysexLen;
  unsigned long *            iCurrentTime;
  HMODULE                    iRTMidiModule;
};

#define IRTMIDI IRTMidi::instance()

extern "C"
{
  void IRTMidiShutdown();
}

#endif

