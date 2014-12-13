#include "Oscillator.h"

#include <stdlib.h>
#include <limits.h>

//-----------------------------------------------------------------------------
double Oscillator::s_sampleRate = 44100.0;

//-----------------------------------------------------------------------------
void Oscillator::setMode(OscillatorMode mode)
{
	m_mode = mode;
}

//-----------------------------------------------------------------------------
void Oscillator::setFrequency(double frequency)
{
	m_frequency = frequency;
	updateIncrement();
}

//-----------------------------------------------------------------------------
void Oscillator::updateIncrement()
{
	m_phaseIncrement = m_frequency * 2.0 * m_pi / s_sampleRate;
}

//-----------------------------------------------------------------------------
double Oscillator::nextSample()
{
	double value = 0;

	switch (m_mode)
	{
	case OM_SINE:
		value = sin(m_phase);
		break;

	case OM_SAW:
		value = 1.0 - (2.0 * m_phase / m_tau);
		break;

	case OM_SQUARE:
		if (m_phase <= m_pi)
		{
			value = 1.0;
		}
		else
		{
			value = -1.0;
		}
		break;

	case OM_TRIANGLE:
		value = -1.0 + (2.0 * m_phase / m_tau);
		value = 2.0 * (fabs(value) - 0.5);
		break;

	default: // Noise
		value = ((double)rand()) / (double)RAND_MAX;
		break;
	}

	m_phase += m_phaseIncrement;
	while (m_phase >= m_tau)
	{
		m_phase -= m_tau;
	}

	return value;
}

//-----------------------------------------------------------------------------
void Oscillator::reset()
{
	m_phase = 0;
}

//-----------------------------------------------------------------------------
//void Oscillator::generate(double* buffer, int nFrames)
//{
//
//}

