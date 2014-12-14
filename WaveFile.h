#ifndef __HEADER_WAVE_FILE__
#define __HEADER_WAVE_FILE__

#include <map>
#include <vector>

#define MAX_SHORT_D 32768.0

//-----------------------------------------------------------------------------
struct WaveCue
{
	WaveCue():
		ID(0),
		order(0),
		position(0)
	{}

	unsigned int ID;
	unsigned int order;
	unsigned int position;
	std::string chunkID;
};

//-----------------------------------------------------------------------------
struct WaveSampleLoop
{
	WaveSampleLoop()
	{
		memset(this, 0, sizeof(WaveSampleLoop));
	}

	unsigned int cuePointID;
	unsigned int loopType;
	unsigned int loopStart;
	unsigned int loopEnd;
	unsigned int fraction;
	unsigned int loopPlayCount;
};

//-----------------------------------------------------------------------------
struct WaveSampleInfo
{
	WaveSampleInfo()
	{
		memset(this, 0, sizeof(WaveSampleInfo));
	}

	unsigned int manufacturer;
	unsigned int product;
	unsigned int samplePeriod;
	unsigned int midiNote;
	unsigned int midiPitchFraction;
	unsigned int smpteFormat;
	unsigned int smpteOffset;
	unsigned int samplerData;

	std::vector<WaveSampleLoop> loops;
};

//-----------------------------------------------------------------------------
class WaveFile
{
public:
	WaveFile():
		m_samples(0),
		m_size(0),
		m_channelCount(0),
		m_sampleRate(0)
	{}

	~WaveFile()
	{
		clear();
	}

	bool loadFromFile(const char * filePath);

	void clear()
	{
		if(m_samples)
			delete[] m_samples;
		m_samples = 0;
	}

	inline unsigned int size() const { return m_size; }
	inline short operator[](unsigned int i) const { return m_samples[i]; }

	inline double getAsDouble(unsigned int i) const { return static_cast<double>(m_samples[i]) / MAX_SHORT_D; }
	//{
	//	short s = m_samples[i];
	//	return static_cast<double>(s) / 16384.0;
	//}

	inline unsigned int getChannelCount() const { return m_channelCount; }
	inline unsigned int getSampleRate() const { return m_sampleRate; }
	inline const WaveSampleInfo & getSampleInfo() const { return m_sampleInfo; }

	inline const std::vector<WaveCue> & getCues() const { return m_cues; }

	void add(const WaveFile & other);
	void multiply(double k);

	unsigned int getMemoryUse() const;

private:
	short * m_samples;
	std::vector<WaveCue> m_cues; // [order] => position
	WaveSampleInfo m_sampleInfo;
	unsigned int m_size; // Size of the m_samples array
	unsigned int m_channelCount;
	unsigned int m_sampleRate;

};

#endif // __HEADER_WAVE_FILE__

