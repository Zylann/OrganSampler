#include "Instrument.h"
#include <assert.h>

//-----------------------------------------------------------------------------
unsigned int InstrumentNote::getMemoryUse() const
{
	unsigned int sum = 0;
	for(std::map<int,PolyphonicSound>::const_iterator it = sounds.begin(); it != sounds.end(); ++it)
	{
		const PolyphonicSound & s = it->second;
		for(unsigned int i = 0; i < s.variants.size(); ++i)
		{
			if(s.variants[i])
				sum += s.variants[i]->getMemoryUse();
		}
	}
	return sum;
}

//-----------------------------------------------------------------------------
inline void InstrumentNote::clear()
{
	for(std::map<int,PolyphonicSound>::iterator it = sounds.begin(); it != sounds.end(); ++it)
	{
		PolyphonicSound & s = it->second;
		for(unsigned int i = 0; i < s.variants.size(); ++i)
		{
			delete s.variants[i];
		}
	}
	sounds.clear();
}

//-----------------------------------------------------------------------------
InstrumentNote::PolyphonicSound & InstrumentNote::getSoundForVelocity(int velocity)
{
	if(velocity < 0)
		velocity = 0;
	if(velocity > 100)
		velocity = 100;

	std::map<int,PolyphonicSound>::iterator it = sounds.upper_bound(velocity);
	if(it == sounds.end())
	{
		it = sounds.lower_bound(velocity);
	}

	return it->second;
}

//-----------------------------------------------------------------------------
WaveFile * InstrumentNote::getNextWave(int velocity)
{
	if(sounds.empty())
		return NULL;
	PolyphonicSound & s = getSoundForVelocity(velocity);
	s.lastPlayedVariant = (s.lastPlayedVariant+1) % s.variants.size();
	if(s.variants[s.lastPlayedVariant])
		return s.variants[s.lastPlayedVariant];
	return NULL;
}

//-----------------------------------------------------------------------------
Instrument::~Instrument()
{
	clear();
}

//-----------------------------------------------------------------------------
void Instrument::clear()
{
	for(unsigned int i = 0; i < m_sections.size(); ++i)
	{
		delete m_sections[i];
	}
	m_sections.clear();
	m_loadQueue.clear();
}

//-----------------------------------------------------------------------------
void Instrument::loadSectionsData(const std::string & topFolder, const std::vector<InstrumentSectionInfo> & sectionsInfo)
{
	//clear();

	for(unsigned int i = 0; i < sectionsInfo.size(); ++i)
	{
		const InstrumentSectionInfo & sectionInfo = sectionsInfo[i];
		unsigned int sectionID = sectionInfo.ID;

		if(!hasSection(sectionID))
		{
			// Ensure the stop data struct is created now
			if(m_sections.size() <= sectionID)
				m_sections.resize(sectionID+1, 0);
			assert(m_sections[sectionID] == 0);
			m_sections[sectionID] = new InstrumentSection();

			// Schedule note files loading
			for(std::map<int,InstrumentNoteInfo>::const_iterator it = sectionInfo.notes.begin(); it != sectionInfo.notes.end(); ++it)
			{
				const InstrumentNoteInfo & noteInfo = it->second;

				for(std::map<int,std::vector<InstrumentSoundInfo> >::const_iterator it = noteInfo.sounds.begin(); it != noteInfo.sounds.end(); ++it)
				{
					int velocity = it->first;
					const std::vector<InstrumentSoundInfo> & soundInfos = it->second;

					for(unsigned int k = 0; k < soundInfos.size(); ++k)
					{
						const InstrumentSoundInfo & soundInfo = soundInfos[k];

						LoadEntry e;
						e.filePath = topFolder + "/" + soundInfo.file;
						e.sectionID = sectionID;
						e.noteNumber = noteInfo.midiNumber;
						e.velocity = velocity;
						e.variantIndex = k;
						e.amplitudeLevel = soundInfo.amplitudeFactor;
						m_loadQueue.push_back(e);
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
bool Instrument::isLoadingSection(unsigned int sectionID)
{
	for(unsigned int i = 0; i < m_loadQueue.size(); ++i)
	{
		if(m_loadQueue[i].sectionID == sectionID)
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
int Instrument::loadNext()
{
	if(m_loadQueue.empty())
		return 0;

	LoadEntry e = m_loadQueue.back();
	m_loadQueue.pop_back();

	if(m_sections.size() <= e.sectionID)
		m_sections.resize(e.sectionID+1, 0);

	if(m_sections[e.sectionID] == 0)
	{
		InstrumentSection * section = new InstrumentSection();
		m_sections[e.sectionID] = section;
	}

	std::vector<InstrumentNote> & notes = m_sections[e.sectionID]->m_notes;

	WaveFile * wave = new WaveFile();
	if(wave->loadFromFile(e.filePath.c_str()))
	{
		int noteNumber = e.noteNumber;

		if(notes.size() <= noteNumber)
		{
			notes.resize(noteNumber + 1);
		}

		InstrumentNote & note = notes[noteNumber];
		InstrumentNote::PolyphonicSound & sound = note.sounds[e.velocity];

		if(e.variantIndex >= sound.variants.size())
		{
			sound.variants.resize(e.variantIndex+1, NULL);
		}
			
		if(sound.variants[e.variantIndex])
		{
			delete sound.variants[e.variantIndex];
		}

		wave->multiply(e.amplitudeLevel);
		sound.variants[e.variantIndex] = wave;
	}
	else
	{
		delete wave;
		return -1;
	}

	return m_loadQueue.size();
}

//-----------------------------------------------------------------------------
std::vector<const WaveFile*> Instrument::getNextNoteWaves(int noteNumber, int velocity) const
{
	std::vector<const WaveFile*> notes;

	if(noteNumber < 0)
		return notes;

	for(unsigned int i = 0; i < m_sections.size(); ++i)
	{
		if(m_sections[i])
		{
			InstrumentSection & section = *m_sections[i];
			if(section.enabled && section.hasNote(noteNumber))
			{
				InstrumentNote & note = section.m_notes[noteNumber];
				WaveFile * wave = note.getNextWave(velocity);
				if(wave)
					notes.push_back(wave);
			}
		}
	}

	return notes;
}

//-----------------------------------------------------------------------------
unsigned int Instrument::getMemoryUse() const
{
	unsigned int sum = 0;
	for(unsigned int i = 0; i < m_sections.size(); ++i)
	{
		if(m_sections[i])
		{
			const InstrumentSection & section = *m_sections[i];
			for(unsigned int j = 0; j < section.m_notes.size(); ++j)
			{
				sum += section.m_notes[j].getMemoryUse();
			}
		}
	}
	return sum;
}
