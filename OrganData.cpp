#include "OrganData.h"
#include <assert.h>

//-----------------------------------------------------------------------------
OrganData::~OrganData()
{
	clear();
}

//-----------------------------------------------------------------------------
void OrganData::clear()
{
	for(unsigned int i = 0; i < m_stops.size(); ++i)
	{
		delete m_stops[i];
	}
	m_stops.clear();
	m_loadQueue.clear();
}

//-----------------------------------------------------------------------------
void OrganData::loadStopsData(const std::string & top, const std::vector<OrganStopInfo> & stops)
{
	//clear();

	for(unsigned int i = 0; i < stops.size(); ++i)
	{
		unsigned int stopID = stops[i].ID;

		if(!hasStop(stopID))
		{
			// Ensure the stop data struct is created now
			if(m_stops.size() <= stopID)
				m_stops.resize(stopID+1, 0);
			assert(m_stops[stopID] == 0);
			m_stops[stopID] = new OrganStopData();

			// Schedule note files loading
			for(unsigned int j = 0; j < stops[i].noteFiles.size(); ++j)
			{
				LoadEntry e;
				e.filePath = top + "/" + stops[i].noteFiles[j];
				e.stopID = stopID;
				e.note = j;
				m_loadQueue.push_back(e);
			}
		}
	}
}

//-----------------------------------------------------------------------------
bool OrganData::isLoadingStop(unsigned int stopID)
{
	for(unsigned int i = 0; i < m_loadQueue.size(); ++i)
	{
		if(m_loadQueue[i].stopID == stopID)
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
int OrganData::loadNext()
{
	if(m_loadQueue.empty())
		return 0;

	LoadEntry e = m_loadQueue.back();
	m_loadQueue.pop_back();

	if(m_stops.size() <= e.stopID)
		m_stops.resize(e.stopID+1, 0);

	if(m_stops[e.stopID] == 0)
	{
		OrganStopData * stopData = new OrganStopData();
		m_stops[e.stopID] = stopData;
	}

	std::vector<WaveFile*> & notes = m_stops[e.stopID]->m_notes;

	WaveFile * wave = new WaveFile();
	if(wave->loadFromFile(e.filePath.c_str()))
	{
		int note = e.note;

		if(notes.size() <= note)
		{
			notes.resize(note + 1, 0);
		}
		if(notes[note])
		{
			delete notes[note];
		}

		notes[note] = wave;
	}
	else
	{
		delete wave;
		return -1;
	}

	return m_loadQueue.size();
}

//-----------------------------------------------------------------------------
std::vector<const WaveFile*> OrganData::getNoteDatas(int note) const
{
	std::vector<const WaveFile*> notes;

	if(note < 0)
		return notes;

	for(unsigned int i = 0; i < m_stops.size(); ++i)
	{
		if(m_stops[i])
		{
			const OrganStopData & stopData = *m_stops[i];
			if(stopData.enabled && stopData.hasNote(note))
			{
				notes.push_back(&stopData.getNoteBuffer(note));
			}
		}
	}

	return notes;
}

//-----------------------------------------------------------------------------
unsigned int OrganData::getMemoryUse() const
{
	unsigned int sum = 0;
	for(unsigned int i = 0; i < m_stops.size(); ++i)
	{
		if(m_stops[i])
		{
			const OrganStopData & stopData = *m_stops[i];
			for(unsigned int j = 0; j < stopData.m_notes.size(); ++j)
			{
				if(stopData.m_notes[j])
					sum += stopData.m_notes[j]->getMemoryUse();
			}
		}
	}
	return sum;
}

