#ifndef _MIDIHost_H
#define _MIDIHost_H

#include "gpaddon.h"

#ifndef MIDI_HOST_ENABLED
#define MIDI_HOST_ENABLED 0
#endif

// KeyboardHost Module Name
#define MIDIHostName "MIDIHost"

class MIDIHostAddon : public GPAddon {
public:
	virtual bool available();
	virtual void setup();       // MIDIHost Setup
	virtual void process() {}   // MIDIHost Process
	virtual void preprocess();
	virtual std::string name() { return MIDIHostName; }
private:
};

#endif  // _MIDIHost_H_