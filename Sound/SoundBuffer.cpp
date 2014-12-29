#include "SoundBuffer.h"
#include "WaveFile.h"
#include "../utility.h"

//-----------------------------------------------------------------------------
void SoundBuffer::clear()
{
	if(m_samples)
		delete[] m_samples;
	m_samples = NULL;
	m_size = 0;
	m_sampleRate = 0;
	m_channelCount = 0;
}

//-----------------------------------------------------------------------------
void SoundBuffer::loadFromFile(const std::string & filePath)
{
	clear();

	std::string ext = filePath.substr(filePath.rfind('.'));
	toLower(ext);

	if(ext == ".wav")
	{
		loadFromWaveFile(filePath);
	}
	else
	{
		throw std::string("Unsupported audio file format '" + ext + "'");
	}
}

//-----------------------------------------------------------------------------
void SoundBuffer::loadFromWaveFile(const std::string & filePath)
{
	WaveFile wave;
	if(!wave.loadFromFile(filePath.c_str()))
	{
		throw std::string("A problem occurred loading WAV file '" + filePath + "'");
	}

	m_size = wave.size();
	m_samples = new short[m_size];
	memcpy(m_samples, wave.getSamplesPtr(), m_size * sizeof(short));
	m_sampleRate = wave.getSampleRate();
	m_channelCount = wave.getChannelCount();
		
	const WaveSampleInfo & sampleInfo = wave.getSampleInfo();

	metadata.midiNote = sampleInfo.midiNote;

	for(size_t i = 0; i < sampleInfo.loops.size(); ++i)
	{
		SoundMetadata::Loop loop;
		loop.begin = sampleInfo.loops[i].loopStart;
		loop.end = sampleInfo.loops[i].loopEnd;
		metadata.loops.push_back(loop);
	}

	for(size_t i = 0; i < wave.getCues().size(); ++i)
	{
		const WaveCue & waveCue = wave.getCues()[i];
		SoundMetadata::Cue cue;
		cue.ID = waveCue.ID;
		cue.position = waveCue.position;
		// TODO Wave cue labels
		metadata.cues.push_back(cue);
	}
}

//-----------------------------------------------------------------------------
void SoundBuffer::getLinear(double x, double * out) const
{
	int i_floor = clamp<int>(static_cast<int>(floor(x)), 0, m_size/2);
	int i_ceil = clamp<int>(static_cast<int>(ceil(x)), 0, m_size/2);
	short * s = m_samples + i_floor * m_channelCount;
	for(unsigned int i = 0; i < m_channelCount; ++i)
	{
		*out = static_cast<double>((*s) / MAX_SHORT_D);
		++s;
	}
}

//-----------------------------------------------------------------------------
void SoundBuffer::add(const SoundBuffer & other)
{
	unsigned int len = std::min(m_size, other.m_size);
	short * dst = m_samples;
	short * src = other.m_samples;
	for(unsigned int i = 0; i < len; ++i, ++dst, ++src)
	{
		int sum = *dst + *src;
		sum = clamp(sum, SHRT_MIN, SHRT_MAX);
		*dst = static_cast<short>(sum);
	}
}

//-----------------------------------------------------------------------------
void SoundBuffer::multiply(double k)
{
	for(unsigned int i = 0; i < m_size; ++i)
	{
		double s = m_samples[i];
		s = clamp(s, -MAX_SHORT_D, MAX_SHORT_D);
		m_samples[i] = static_cast<short>(s);
	}
}

//-----------------------------------------------------------------------------
unsigned int SoundBuffer::getMemoryUse() const
{
	return m_size * sizeof(short) + sizeof(SoundBuffer) + sizeof(SoundMetadata);
}

