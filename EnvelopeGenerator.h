#ifndef __HEADER_ENVELOPE_GENERATOR__
#define __HEADER_ENVELOPE_GENERATOR__

#include "Signal/GallantSignal.h"
using Gallant::Signal0;

class EnvelopeGenerator
{
public:
	enum EnvelopeStage
	{
		ENVELOPE_STAGE_OFF = 0,
		ENVELOPE_STAGE_ATTACK,
		ENVELOPE_STAGE_DECAY,
		ENVELOPE_STAGE_SUSTAIN,
		ENVELOPE_STAGE_RELEASE,
		kNumEnvelopeStages
	};

	EnvelopeGenerator():
		minimumLevel(0.0001),
		m_currentStage(ENVELOPE_STAGE_OFF),
		m_currentLevel(minimumLevel),
		m_multiplier(1.0),
		m_currentSampleIndex(0),
		m_nextStageSampleIndex(0)
	{
		m_stageValue[ENVELOPE_STAGE_OFF] = 0.0;
		m_stageValue[ENVELOPE_STAGE_ATTACK] = 0.01;
		m_stageValue[ENVELOPE_STAGE_DECAY] = 0.5;
		m_stageValue[ENVELOPE_STAGE_SUSTAIN] = 0.1;
		m_stageValue[ENVELOPE_STAGE_RELEASE] = 1.0;
	};

	Signal0<> beganCycle;
	Signal0<> finishedCycle;

	void enterStage(EnvelopeStage newStage);
	double nextSample();
	void reset();
	inline EnvelopeStage getCurrentStage() const { return m_currentStage; };

	const double minimumLevel;

	static inline void setSampleRate(double sr) { s_sampleRate = 44100.0; }

private:
	static double s_sampleRate;

	void calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);

	EnvelopeStage m_currentStage;
	double m_currentLevel;
	double m_multiplier;
	double m_stageValue[kNumEnvelopeStages];
	unsigned long long m_currentSampleIndex;
	unsigned long long m_nextStageSampleIndex;
};

#endif // __HEADER_ENVELOPE_GENERATOR__

