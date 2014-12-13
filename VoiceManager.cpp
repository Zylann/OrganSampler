#include "VoiceManager.h"
#include <stdlib.h> // For NULL

//-----------------------------------------------------------------------------
void VoiceManager::onNoteOn(int noteNumber, int velocity)
{
	int basedNoteNumber = noteNumber - 32;

	std::vector<const WaveFile*> waves = organData.getNoteDatas(basedNoteNumber);
	for(unsigned int i = 0; i < waves.size(); ++i)
	{
		Voice* voice = findFreeVoice();
		if (!voice)
			break;
		voice->noteOn(noteNumber, waves[i], velocity);
	}
}

//-----------------------------------------------------------------------------
Voice* VoiceManager::findFreeVoice()
{
	Voice* freeVoice = NULL;
	for (unsigned int i = 0; i < VOICE_COUNT; ++i)
	{
		if (!m_voices[i].isActive())
		{
			freeVoice = &(m_voices[i]);
			break;
		}
	}
	return freeVoice;
}

//-----------------------------------------------------------------------------
void VoiceManager::onNoteOff(int noteNumber, int velocity)
{
	// Find the voice(s) with the given noteNumber:
	for (int i = 0; i < VOICE_COUNT; ++i)
	{
		Voice& voice = m_voices[i];
		if (voice.isActive() && voice.getNoteNumber() == noteNumber)
		{
			voice.noteOff();
		}
	}
}

//-----------------------------------------------------------------------------
void VoiceManager::nextSample(double & out_l, double & out_r)
{
	out_l = 0;
	out_r = 0;
	for (unsigned int i = 0; i < VOICE_COUNT; ++i)
	{
		Voice& voice = m_voices[i];
		if(voice.isActive())
		{
			double l = 0, r = 0;
			voice.nextSample(l, r);
			out_l += l;
			out_r += r;
		}
	}
}

//-----------------------------------------------------------------------------
void VoiceManager::reset()
{
	for(unsigned int i = 0; i < VOICE_COUNT; ++i)
	{
		m_voices[i].reset();
	}
}

