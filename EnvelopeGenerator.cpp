#include "EnvelopeGenerator.h"
#include <algorithm>


//-----------------------------------------------------------------------------
double EnvelopeGenerator::s_sampleRate = 44100.0;

//-----------------------------------------------------------------------------
double EnvelopeGenerator::nextSample()
{
	if (m_currentStage != ENVELOPE_STAGE_OFF && m_currentStage != ENVELOPE_STAGE_SUSTAIN)
	{
		if (m_currentSampleIndex == m_nextStageSampleIndex)
		{
			EnvelopeStage newStage = static_cast<EnvelopeStage>((m_currentStage + 1) % kNumEnvelopeStages);
			enterStage(newStage);
		}
		m_currentLevel *= m_multiplier;
		++m_currentSampleIndex;
	}
	return m_currentLevel;
}

//-----------------------------------------------------------------------------
void EnvelopeGenerator::reset()
{
	m_currentStage = ENVELOPE_STAGE_OFF;
	m_currentLevel = minimumLevel;
	m_multiplier = 1.0;
	m_currentSampleIndex = 0;
	m_nextStageSampleIndex = 0;
}

//-----------------------------------------------------------------------------
void EnvelopeGenerator::calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples)
{
	m_multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}

//-----------------------------------------------------------------------------
void EnvelopeGenerator::enterStage(EnvelopeStage newStage)
{
	if (m_currentStage == newStage)
		return;
	if (m_currentStage == ENVELOPE_STAGE_OFF)
	{
		beganCycle();
	}
	if (newStage == ENVELOPE_STAGE_OFF)
	{
		finishedCycle();
	}

	m_currentStage = newStage;
	m_currentSampleIndex = 0;

	if (m_currentStage == ENVELOPE_STAGE_OFF || m_currentStage == ENVELOPE_STAGE_SUSTAIN)
	{
		m_nextStageSampleIndex = 0;
	}
	else
	{
		m_nextStageSampleIndex = m_stageValue[m_currentStage] * s_sampleRate;
	}

	switch (newStage)
	{
	case ENVELOPE_STAGE_OFF:
		m_currentLevel = 0.0;
		m_multiplier = 1.0;
		break;

	case ENVELOPE_STAGE_ATTACK:
		m_currentLevel = minimumLevel;
		calculateMultiplier(m_currentLevel, 1.0, m_nextStageSampleIndex);
		break;

	case ENVELOPE_STAGE_DECAY:
		m_currentLevel = 1.0;
		calculateMultiplier(m_currentLevel, std::max(m_stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel), m_nextStageSampleIndex);
		break;
	
	case ENVELOPE_STAGE_SUSTAIN:
		m_currentLevel = m_stageValue[ENVELOPE_STAGE_SUSTAIN];
		m_multiplier = 1.0;
		break;

	case ENVELOPE_STAGE_RELEASE:
		// We could go from ATTACK/DECAY to RELEASE,
		// so we're not changing currentLevel here.
		calculateMultiplier(m_currentLevel, minimumLevel, m_nextStageSampleIndex);
		break;

	default:
		break;
	}
}



