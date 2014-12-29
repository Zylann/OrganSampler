#ifndef __HEADER_SOUND_BUFFER__
#define __HEADER_SOUND_BUFFER__

#include "SoundMetadata.h"

#define MAX_SHORT_D 32768.0

class SoundBuffer
{
public:
	SoundBuffer():
		m_samples(NULL),
		m_size(0),
		m_sampleRate(0),
		m_channelCount(0)
	{}

	~SoundBuffer()
	{
		clear();
	}

	void clear();

	void loadFromFile(const std::string & filePath);

	inline unsigned int size() const { return m_size; }
	//inline short operator[](unsigned int i) const { return m_samples[i]; }

	inline double getAsDouble(unsigned int i) const { return static_cast<double>(m_samples[i]) / MAX_SHORT_D; }

	void getLinear(double x, double * out) const;

	inline unsigned int getChannelCount() const { return m_channelCount; }
	inline unsigned int getSampleRate() const { return m_sampleRate; }

	void add(const SoundBuffer & other);
	void multiply(double k);

	unsigned int getMemoryUse() const;

	SoundMetadata metadata;

private:

	void loadFromWaveFile(const std::string & filePath);

	short * m_samples; // 16-bit samples only
	unsigned int m_size;
	unsigned int m_sampleRate;
	unsigned int m_channelCount;

};

#endif // __HEADER_SOUND_BUFFER__

