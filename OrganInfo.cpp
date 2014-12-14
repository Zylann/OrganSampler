#include "OrganInfo.h"
#include "utility.h"
#include <fstream>
#include <map>
#include <sstream>
#include <iostream>

//-----------------------------------------------------------------------------
void nextLine(std::istream & is)
{
	char last_c = 0;
	while(!is.eof())
	{
		if(isEOL(last_c))
		{
			char c = is.peek();
			if(!isEOL(c))
				return;
		}
		last_c = is.get();
	}
}

//-----------------------------------------------------------------------------
std::string getUntil(std::istream & is, std::string delims)
{
	std::string str;
	while(!is.eof())
	{
		char c = is.get();
		if(delims.find(c) == std::string::npos)
			str += c;
		else
			break;
	}
	return str;
}

//-----------------------------------------------------------------------------
void OrganInfo::clear()
{
	m_stops.clear();
}

//-----------------------------------------------------------------------------
bool OrganInfo::loadFromFile(const char * filePath)
{
	clear();

	m_filePath = filePath;

	std::ifstream ifs(filePath);
	if(!ifs.good())
	{
		// Failed to open file
		return false;
	}

	typedef std::map< std::string, std::string > Section;
	typedef std::map< std::string, Section > SectionMap;

	SectionMap sections;
	std::string sectionName;

	// Parse file
	while(!ifs.eof())
	{
		char c = ifs.peek();
		if(c == ';')
		{
			nextLine(ifs);
		}
		else if(c == '[')
		{
			ifs.get();
			sectionName = getUntil(ifs, "]");
		}
		else if(isalpha(c))
		{
			std::pair<std::string, std::string> p;
			p.first = getUntil(ifs, "=");
			//ifs.get();
			p.second = getUntil(ifs, "\r\n");
			sections[sectionName].insert(p);
			//nextLine(ifs);
		}
		else
		{
			nextLine(ifs);
		}
	}

	ifs.close();

	// Interpret data
	for(SectionMap::iterator it = sections.begin(); it != sections.end(); ++it)
	{
		// If the section contains stop info
		if(it->first.find("Stop") == 0)
		{
			Section & section = it->second;
			OrganStopInfo organStop;

			organStop.name = section["Name"];

			// TODO Handle AmplitudeLevel (0-100)
			organStop.amplitudeLevel = atoi(section["AmplitudeLevel"].c_str());

			// Find notes (= pipes)
			for(Section::iterator it2 = section.begin(); it2 != section.end(); ++it2)
			{
				size_t findPos = it2->first.find("Pipe");
				if(findPos != std::string::npos)
				{
					std::string path = it2->second;
					normalizePath(path);
					// Note: pipes are in order because they are indexed in a map
					organStop.noteFiles.push_back(path);
				}
			}

			std::cout << "OrganStop \"" << organStop.name << "\" has " << organStop.noteFiles.size() << " pipes" << std::endl;
			organStop.ID = m_stops.size();
			m_stops.push_back(organStop);
		}
	}

	return true;
}



