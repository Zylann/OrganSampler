#ifndef __HEADER_OSCILLATOR__
#define __HEADER_OSCILLATOR__

#include <math.h>

enum OscillatorMode
{
	OM_SINE = 0,
	OM_SAW,
	OM_SQUARE,
	OM_TRIANGLE,
	OM_NOISE
};

class Oscillator
{
public:
	Oscillator() :
		m_mode(OM_SINE),
		m_pi(2*acos(0.0)),
		m_tau(2*m_pi),
		m_frequency(440.0),
		m_phase(0.0)
	{
		updateIncrement();
	}

	void setMode(OscillatorMode mode);
	void setFrequency(double frequency);
	//void setSampleRate(double sampleRate);
	//void generate(double* buffer, int nFrames);
	double nextSample();
	void reset();

	inline static void setSampleRate(double sr) { s_sampleRate = sr; }

private:
	static double s_sampleRate;

	void updateIncrement();

	OscillatorMode m_mode;
	const double m_pi;
	const double m_tau;
	double m_frequency;
	double m_phase;
	double m_phaseIncrement;

};

#endif // __HEADER_OSCILLATOR__

