#ifndef __HEADER_ORGAN_DATA__
#define __HEADER_ORGAN_DATA__

#include "OrganInfo.h"
#include <vector>

//-----------------------------------------------------------------------------
class OrganData;

//-----------------------------------------------------------------------------
class OrganStopData
{
public:
	~OrganStopData()
	{
		for(unsigned int i = 0; i < m_notes.size(); ++i)
			delete m_notes[i];
	}

	inline bool hasNote(int i) const { return i >= 0 && i < m_notes.size() && m_notes[i] != 0; }
	inline const WaveFile & getNoteBuffer(int i) const { return *m_notes[i]; }

private:
	friend class OrganData;

	std::vector<WaveFile*> m_notes;
};

//-----------------------------------------------------------------------------
class OrganData
{
public:

	~OrganData();

	void loadStopsData(const std::string & top, const std::vector<OrganStopInfo> & stops);
	int loadNext();
	void clear();
	
	inline bool isLoading() const { return !m_loadQueue.empty(); }

	std::vector<const WaveFile*> getNoteDatas(int note) const;

	inline bool hasStop(unsigned int stopID) const { return stopID < m_stops.size(); }
	inline const OrganStopData & getStop(unsigned int stopID) const { return *m_stops[stopID]; }

private:
	std::vector< OrganStopData* > m_stops; // [stopID][note] => buffer

	struct LoadEntry
	{
		unsigned int stopID;
		unsigned int note;
		std::string filePath;
	};
	std::vector<LoadEntry> m_loadQueue;
};

#endif // __HEADER_ORGAN_DATA__

