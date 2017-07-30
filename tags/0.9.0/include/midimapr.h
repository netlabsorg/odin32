/* $Id: midimapr.h,v 1.2 1999-05-27 15:17:58 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _MIDIMAPR_H
#define _MIDIMAPR_H

// Defines for the WIN32OS2 MIDI Mapper

#define MIDI_MAPPER_QUEUE "\\QUEUES\\MIDIMAPPER"

// Message types
#define WINOS2_MIDI_QUIT      0
#define WINOS2_MIDI_SHORT_MSG 1

// Message Priorities

#define WINOS2_MIDI_PRIORITY_HIGH 15
#define WINOS2_MIDI_PRIORITY_MED  13
#define WINOS2_MIDI_PRIORITY_LOW  11

#endif
