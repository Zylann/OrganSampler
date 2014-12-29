#ifndef __HEADER_INSTRUMENT__
#define __HEADER_INSTRUMENT__

#include "InstrumentInfo.h"
#include "Sound/SoundBuffer.h"

//-----------------------------------------------------------------------------
class InstrumentNote
{
public:
	unsigned int getMemoryUse() const;
	inline void clear();

	SoundBuffer * getNextWave(int velocity);

	struct PolyphonicSound
	{
		PolyphonicSound():
			lastPlayedVariant(0)
		{}

		std::vector<SoundBuffer*> variants;
		unsigned int lastPlayedVariant;
	};

	// [velocityThreshold] => sounds
	std::map<int, PolyphonicSound> sounds; // Sound variants, by velocity

private:
	PolyphonicSound & getSoundForVelocity(int velocity);

};

//-----------------------------------------------------------------------------
class Instrument;

//-----------------------------------------------------------------------------
class InstrumentSection
{
public:
	InstrumentSection():
		enabled(false)
	{}

	~InstrumentSection()
	{
		for(unsigned int i = 0; i < m_notes.size(); ++i)
		{
			m_notes[i].clear();
		}
	}

	inline bool hasNote(int i) const { return i >= 0 && i < m_notes.size(); }
	inline const InstrumentNote & getNote(int i) const { return m_notes[i]; }

	bool enabled;

private:
	friend class Instrument;

	std::vector<InstrumentNote> m_notes;
};

//-----------------------------------------------------------------------------
class Instrument
{
public:

	~Instrument();

	void loadSectionsData(const std::string & topFolder, const std::vector<InstrumentSectionInfo> & sectionsInfo);
	int loadNext();
	void clear();
	
	inline bool isLoading() const { return !m_loadQueue.empty(); }
	bool isLoadingSection(unsigned int sectionID);

	std::vector<const SoundBuffer*> getNextNoteWaves(int noteNumber, int velocity) const;

	inline bool hasSection(unsigned int sectionID) const { return sectionID < m_sections.size() && m_sections[sectionID] != 0; }
	inline InstrumentSection & getSection(unsigned int sectionID) { return *m_sections[sectionID]; }

	unsigned int getMemoryUse() const;

private:
	std::vector< InstrumentSection* > m_sections; // [sectionID][note] => buffer

	struct LoadEntry
	{
		unsigned int sectionID;
		unsigned int noteNumber;
		unsigned int variantIndex;
		int velocity;
		double amplitudeLevel;
		std::string filePath;
	};
	std::vector<LoadEntry> m_loadQueue;
};

#endif // __HEADER_INSTRUMENT__

