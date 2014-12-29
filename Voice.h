#ifndef __HEADER_VOICE__
#define __HEADER_VOICE__

//#include "Oscillator.h"
//#include "EnvelopeGenerator.h"
#include "utility.h"
#include "Sound/SoundBuffer.h"

class Voice
{
public:
	Voice():
		m_noteNumber(-1),
		m_velocity(0),
		m_isActive(false),
		m_isReleasing(false),
		m_releasePos(0),
		m_loopPos(0),
		m_releaseFactor(0),
		m_loopAmount(0)
	{
		// Set myself free everytime my volume envelope has fully faded out of RELEASE stage:
		//m_volumeEnvelope.finishedCycle.Connect(this, &Voice::setFree);
	}

	void nextSample(double & out_l, double & out_r);
	void reset();

	void noteOn(int noteNumber, const SoundBuffer * wave, int velocity);
	void noteOff();

	inline bool isActive() const { return m_isActive; }
	inline int getNoteNumber() const { return m_noteNumber; }

private:

	void nextSample_loop(double & out_l, double & out_r);
	void nextSample_oneShot(double & out_l, double & out_r);

	//inline void setFree()
	//{
	//	m_isActive = false;
	//}

	//Oscillator m_oscillator;
	//EnvelopeGenerator m_volumeEnvelope;
	int m_noteNumber;
	const SoundBuffer * m_wave;
    double m_velocity;
	bool m_isActive;
	bool m_isReleasing;
	unsigned int m_releasePos;
	unsigned int m_loopPos;
	double m_loopAmount;
	double m_releaseFactor;

	bool m_loopable;
};

#endif // __HEADER_VOICE__

