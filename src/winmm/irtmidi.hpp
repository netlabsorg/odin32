/* $Id: irtmidi.hpp,v 1.1 1999-05-24 20:20:06 ktk Exp $ */

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
  void            startTimer() const;
  void            stopTimer()  const;
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
  ULONG (*APIENTRY MidiCreateInstance)    ( ULONG, MINSTANCE*, PSZ, ULONG );
  ULONG (*APIENTRY MidiDeleteInstance)    ( MINSTANCE, ULONG );
  ULONG (*APIENTRY MidiEnableInstance)    ( MINSTANCE, ULONG );
  ULONG (*APIENTRY MidiDisableInstance)   ( MINSTANCE, ULONG );
  ULONG (*APIENTRY MidiAddLink)           ( MINSTANCE, MINSTANCE, ULONG, ULONG );
  ULONG (*APIENTRY MidiRemoveLink)        ( MINSTANCE, MINSTANCE, ULONG, ULONG );
  ULONG (*APIENTRY MidiQueryClassList)    ( ULONG, PMIDICLASSINFO, ULONG );
  ULONG (*APIENTRY MidiQueryInstanceList) ( ULONG, PMIDIINSTANCEINFO, ULONG );
  ULONG (*APIENTRY MidiQueryNumClasses)   ( PULONG, ULONG );
  ULONG (*APIENTRY MidiQueryNumInstances) ( PULONG, ULONG );
  ULONG (*APIENTRY MidiSendMessages)      ( PMESSAGE, ULONG, ULONG );
  ULONG (*APIENTRY MidiSendSysexMessage)  ( PMESSAGE, ULONG, ULONG );
  ULONG (*APIENTRY MidiRetrieveMessages)  ( MINSTANCE, PVOID, PULONG, ULONG );
  ULONG (*APIENTRY MidiSetup)             ( PMIDISETUP, ULONG );
  ULONG (*APIENTRY MidiTimer)             ( ULONG, ULONG );

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

