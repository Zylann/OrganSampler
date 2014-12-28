#ifndef __HEADER_ORGAN_INFO__
#define __HEADER_ORGAN_INFO__

#include <string>
#include <vector>

struct OrganStopInfo
{
	unsigned int ID;
	std::string name;
	int amplitudeLevel;
	std::vector<std::string> noteFiles;
};

class OrganInfo
{
public:

	bool loadFromFile(const char * filePath);
	void clear();

	inline const OrganStopInfo & getStop(unsigned int i) const { return m_stops[i]; }
	inline const unsigned int getStopCount() const { return m_stops.size(); }

	inline const std::string & getFilePath() const { return m_filePath; }

private:
	std::vector<OrganStopInfo> m_stops;
	std::string m_filePath;

};


#endif // __HEADER_ORGAN_INFO__

