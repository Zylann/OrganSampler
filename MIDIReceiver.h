#ifndef __HEADER_MIDI_RECEIVER__
#define __HEADER_MIDI_RECEIVER__

#include "IPlug_include_in_plug_hdr.h"
#include "IMidiQueue.h"

#include <math.h>

#include "GallantSignal.h"
using Gallant::Signal2;

class MIDIReceiver
{
public:
	MIDIReceiver() :
		m_numKeys(0),
		m_offset(0)
	{
		for (int i = 0; i < KEY_COUNT; ++i)
		{
			m_keyStatus[i] = false;
		}
	}

	Signal2< int, int > noteOn;
	Signal2< int, int > noteOff;

	// Returns true if the key with a given index is currently pressed
	inline bool getKeyStatus(int keyIndex) const { return m_keyStatus[keyIndex]; }

	// Returns the number of keys currently pressed
	inline int getNumKeys() const { return m_numKeys; }

	void advance();

	void onMessageReceived(IMidiMsg* midiMessage);

	inline void flush(int nFrames) { m_midiQueue.Flush(nFrames); m_offset = 0; }
	inline void resize(int blockSize) { m_midiQueue.Resize(blockSize); }


private:
	static const int KEY_COUNT = 128;

	IMidiQueue m_midiQueue;

	int m_numKeys; // how many keys are being played at the moment (via midi)
	bool m_keyStatus[KEY_COUNT]; // array of on/off for each key (index is note number)
	int m_offset;

};

#endif // __HEADER_MIDI_RECEIVER__

