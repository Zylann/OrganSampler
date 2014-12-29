#include "Voice.h"

//-----------------------------------------------------------------------------
void Voice::nextSample(double & out_l, double & out_r)
{
	out_l = 0;
	out_r = 0;

	if (!m_isActive)
		return;

	if(m_loopable)
		nextSample_loop(out_l, out_r);
	else
		nextSample_oneShot(out_l, out_r);
}

//-----------------------------------------------------------------------------
void Voice::nextSample_loop(double & out_l, double & out_r)
{
	unsigned int loopDataPos = m_loopPos * 2;

	// TODO Support mono sounds here too

	if(loopDataPos < m_wave->size())
	{
		out_l = m_wave->getAsDouble(loopDataPos) * m_velocity;
		out_r = m_wave->getAsDouble(loopDataPos+1) * m_velocity;
		++m_loopPos;

		double out_l_before = out_l;
		double out_r_before = out_r;

		const SoundMetadata & sampleInfo = m_wave->metadata;
		const SoundMetadata::Loop & loop = sampleInfo.loops[0];
		if(m_loopPos == loop.end)
		{
			m_loopPos = loop.begin;
		}

		if(m_isReleasing)
		{
			unsigned int releaseDataPos = m_releasePos * 2;
			if(releaseDataPos < m_wave->size())
			{
				//double rel_l = 0;
				//double rel_r = 0;
				double rel_l = m_wave->getAsDouble(releaseDataPos) * m_velocity;
				double rel_r = m_wave->getAsDouble(releaseDataPos+1) * m_velocity;
				++m_releasePos;

				// Mix with the looping part
				out_l = lerp(rel_l, out_l, m_loopAmount);
				out_r = lerp(rel_r, out_r, m_loopAmount);
				m_loopAmount *= m_releaseFactor;
			}
			else
			{
				out_l = 0;
				out_r = 0;
				m_isActive = false;
			}
		}
	}
	else
	{
		m_isActive = false;
	}
}

//-----------------------------------------------------------------------------
void Voice::nextSample_oneShot(double & out_l, double & out_r)
{
	unsigned int channelCount = m_wave->getChannelCount();
	unsigned int dataPos = m_releasePos * channelCount;

	if(dataPos < m_wave->size())
	{
		double rel_l = m_wave->getAsDouble(dataPos) * m_velocity;
		double rel_r = channelCount == 1 ? rel_l : m_wave->getAsDouble(dataPos+1) * m_velocity;
		++m_releasePos;
		out_l = rel_l;
		out_r = rel_r;
	}
	else
	{
		out_l = 0;
		out_r = 0;
		m_isActive = false;
	}
}

//-----------------------------------------------------------------------------
void Voice::reset()
{
	m_noteNumber = -1;
	m_velocity = 0;
	m_wave = 0;
	m_releasePos = 0;
	m_isActive = false;
	m_isReleasing = false;
	m_loopPos = 0;
	m_loopable = false;
	//m_oscillator.reset();
	//m_volumeEnvelope.reset();
}

//-----------------------------------------------------------------------------
void Voice::noteOn(int noteNumber, const SoundBuffer * wave, int velocity)
{
	reset();

	m_noteNumber = noteNumber;
	m_wave = wave;
	m_velocity = static_cast<double>(velocity) / 127.0;
	m_isActive = true;
	m_loopAmount = 1.0;
	m_loopable = wave->metadata.loops.size() > 0;

	m_releaseFactor = calcExponentialDecreaseFactor(1.0, 0.001, 44100/2 /* arbitrary */);

	//m_oscillator.setFrequency(noteNumberToFrequency(noteNumber));

	//m_volumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
}

//-----------------------------------------------------------------------------
void Voice::noteOff()
{
	if(!m_isReleasing)
	{
		if(m_loopable)
		{
			//unsigned int releasePos = m_wave->getCues()[0].position;
			unsigned int releasePos = m_wave->metadata.loops[0].end;
			if(!m_wave->metadata.cues.empty())
			{
				releasePos = m_wave->metadata.cues[0].position;
			}
			m_releasePos = releasePos;
			m_isReleasing = true;
			m_loopAmount = 1.0;
		}
	}

	//m_playbackPos = 
	//m_volumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
}



