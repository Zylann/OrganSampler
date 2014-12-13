#include "WaveFile.h"
#include <fstream>
#include "utility.h"


template <typename T>
T readData(std::istream & is)
{
	T i = 0;
	is.read(reinterpret_cast<char*>(&i), sizeof(T));
	return i;
}


bool WaveFile::loadFromFile(const char * filePath)
{
	clear();

	std::ifstream ifs(filePath, std::ios::in|std::ios::beg|std::ios::binary);

	if(!ifs)
	{
		// Open error
		return false;
	}

	char chunkID[5] = {0};
	ifs.read(chunkID, 4);
	if(strcmp(chunkID, "RIFF") != 0)
	{
		// Not a valid WAV file
		return false;
	}

	char chunkSize[4] = {0};
	ifs.read(chunkSize, 4);

	char format[5] = {0};
	ifs.read(format, 4);
	if(strcmp(format, "WAVE") != 0)
	{
		// Not a valid WAV file
		return false;
	}

	while(!ifs.eof())
	{
		char subchunkID[5] = {0};
		ifs.read(subchunkID, 4);

		unsigned int subchunkSize = 0;
		ifs.read(reinterpret_cast<char*>(&subchunkSize), 4);

		if(strcmp(subchunkID, "fmt ") == 0)
		{
			unsigned short audioFormat = 0;
			ifs.read(reinterpret_cast<char*>(&audioFormat), 2); // PCM = 1 (i.e. Linear quantization) Values other than 1 indicate some form of compression.
			if(audioFormat != 1)
			{
				// Non-PCM data not supported
				return false;
			}

			unsigned short numChannels = 0;
			ifs.read(reinterpret_cast<char*>(&numChannels), 2); // Mono = 1, Stereo = 2, etc
			m_channelCount = numChannels;

			unsigned int sampleRate = 0;
			ifs.read(reinterpret_cast<char*>(&sampleRate), 4); // 8000, 44100, etc.
			m_sampleRate = sampleRate;

			char byteRate[4] = {0};
			ifs.read(byteRate, 4); // == SampleRate * NumChannels * BitsPerSample/8

			char blockAlign[2] = {0}; // == NumChannels * BitsPerSample/8
			// The number of bytes for one sample including all channels.
			// I wonder what happens when this number isn't an integer?
			ifs.read(blockAlign, 2);

			short bitsPerSample = 0; // 8 bits = 8, 16 bits = 16, etc.
			ifs.read(reinterpret_cast<char*>(&bitsPerSample), 2);
			if(bitsPerSample != 16)
			{
				// Non-16-bit audio is not supported
				return false;
			}
		}
		else if(strcmp(subchunkID, "data") == 0)
		{
			if(m_channelCount == 0)
			{
				// Format not specified yet
				return false;
			}

			char * data = new char[subchunkSize];
			ifs.read(data, subchunkSize); // Read in all of the sound data

			if(m_samples == 0)
			{
				m_samples = reinterpret_cast<short*>(data);
				m_size = subchunkSize / 2;
			}
			else
			{
				// Two data chunks??
				delete[] data;
			}
		}
		else if(strcmp(subchunkID, "cue ") == 0)
		{
			unsigned int numCuePoints = readData<unsigned int>(ifs);

			for(unsigned int i = 0; i < numCuePoints; ++i)
			{
				//0x00 	4 	ID 	unique identification value
				//0x04 	4 	Position 	play order position
				//0x08 	4 	Data Chunk ID 	RIFF ID of corresponding data chunk
				//0x0c 	4 	Chunk Start 	Byte Offset of Data Chunk *
				//0x10 	4 	Block Start 	Byte Offset to sample of First Channel
				//0x14 	4 	Sample Offset 	Byte Offset to sample byte of First Channel
				//http://www.sonicspot.com/guide/wavefiles.html#cue

				WaveCue cue;

				cue.ID = readData<unsigned int>(ifs);
				cue.order = readData<unsigned int>(ifs);
				
				char targetChunkID[5] = {0};
				ifs.read(targetChunkID, 4);
				cue.chunkID = targetChunkID;

				unsigned int chunkStart = readData<unsigned int>(ifs);
				unsigned int blockStart = readData<unsigned int>(ifs);
				unsigned int sampleOffset = readData<unsigned int>(ifs);

				cue.position = sampleOffset;

				m_cues.push_back(cue);
			}
		}
		else if(strcmp(subchunkID, "smpl") == 0)
		{
			//0x08 	4 	Manufacturer 	0 - 0xFFFFFFFF
			//0x0C 	4 	Product 	0 - 0xFFFFFFFF
			//0x10 	4 	Sample Period 	0 - 0xFFFFFFFF
			//0x14 	4 	MIDI Unity Note 	0 - 127
			//0x18 	4 	MIDI Pitch Fraction 	0 - 0xFFFFFFFF
			//0x1C 	4 	SMPTE Format 	0, 24, 25, 29, 30
			//0x20 	4 	SMPTE Offset 	0 - 0xFFFFFFFF
			//0x24 	4 	Num Sample Loops 	0 - 0xFFFFFFFF
			//0x28 	4 	Sampler Data 	0 - 0xFFFFFFFF
			//0x2C 	
			//List of Sample Loops

			WaveSampleInfo smpl;

			smpl.manufacturer = readData<unsigned int>(ifs);
			smpl.product = readData<unsigned int>(ifs);
			smpl.samplePeriod = readData<unsigned int>(ifs);
			smpl.midiNote = readData<unsigned int>(ifs);
			smpl.midiPitchFraction = readData<unsigned int>(ifs);
			smpl.smpteFormat = readData<unsigned int>(ifs);
			smpl.smpteOffset = readData<unsigned int>(ifs);
			unsigned numSampleLoops = readData<unsigned int>(ifs);
			smpl.samplerData = readData<unsigned int>(ifs);

			for(unsigned int i = 0; i < numSampleLoops; ++i)
			{
				//0x00 	4 	Cue Point ID 	0 - 0xFFFFFFFF
				//0x04 	4 	Type 	0 - 0xFFFFFFFF
				//0x08 	4 	Start 	0 - 0xFFFFFFFF
				//0x0C 	4 	End 	0 - 0xFFFFFFFF
				//0x10 	4 	Fraction 	0 - 0xFFFFFFFF
				//0x14 	4 	Play Count 	0 - 0xFFFFFFFF

				WaveSampleLoop loop;

				loop.cuePointID = readData<unsigned int>(ifs);
				loop.loopType = readData<unsigned int>(ifs);
				loop.loopStart = readData<unsigned int>(ifs);
				loop.loopEnd = readData<unsigned int>(ifs);
				loop.fraction = readData<unsigned int>(ifs);
				loop.loopPlayCount = readData<unsigned int>(ifs);

				smpl.loops.push_back(loop);
			}

			m_sampleInfo = smpl;
		}
		else
		{
			// Unknown chunk, skip
			ifs.seekg(subchunkSize, std::ios::cur);
		}
	}

	ifs.close();

	return true;
}

void WaveFile::add(const WaveFile & other)
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

void WaveFile::multiply(double k)
{
	for(unsigned int i = 0; i < m_size; ++i)
	{
		double s = m_samples[i];
		m_samples[i] = static_cast<short>(s * k);
	}
}

