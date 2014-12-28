#ifndef __HEADER_VOICE_MANAGER__
#define __HEADER_VOICE_MANAGER__

#include "Voice.h"
//#include "OrganData.h"
#include "Instrument.h"

class VoiceManager
{
public:
	VoiceManager() {}

	void onNoteOn(int noteNumber, int velocity);
	void onNoteOff(int noteNumber, int velocity);

	void nextSample(double & out_l, double & out_r);

	void reset();

	Instrument instrument;

private:
	//static const unsigned int NOTE_COUNT = 256;
#ifdef _DEBUG
	static const unsigned int VOICE_COUNT = 64;
#else
	static const unsigned int VOICE_COUNT = 1024;
#endif

	Voice* findFreeVoice();

	Voice m_voices[VOICE_COUNT];
	//std::vector<unsigned int> m_freeVoices;
	//std::vector<unsigned int> m_activeVoices;

};

#endif // __HEADER_VOICE_MANAGER__

