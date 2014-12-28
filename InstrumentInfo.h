#ifndef __HEADER_INSTRUMENT_PRESET__
#define __HEADER_INSTRUMENT_PRESET__

#include <string>
#include <vector>
#include <map>
#include <JsonBox.h>

//-----------------------------------------------------------------------------
struct InstrumentMetadata
{
	InstrumentMetadata():
		name("Untitled"),
		author("Unknown author"),
		version(0)
	{}

	std::string name;
	std::string author;
	std::string description;
	std::string url;
	int version;
};

//-----------------------------------------------------------------------------
struct InstrumentSoundInfo
{
	InstrumentSoundInfo():
		amplitudeFactor(1.0)
	{}

	std::string file; // Audio file
	float amplitudeFactor; // Post-loading amplitude
};

//-----------------------------------------------------------------------------
struct InstrumentNoteInfo
{
	int midiNumber;
	// [maxVelocity0-100] => polyphony (list of files)
	std::map<int, std::vector<InstrumentSoundInfo> > sounds;
};

//-----------------------------------------------------------------------------
struct InstrumentSectionInfo
{
	InstrumentSectionInfo():
		ID(0),
		name("Untitled"),
		loadingFormatStr("json")
	{}

	unsigned int ID;
	std::string name;
	std::string loadingFormatStr;

	// [midiNote] => info
	std::map<int, InstrumentNoteInfo> notes;
};

//-----------------------------------------------------------------------------
class InstrumentInfo
{
public:

	bool loadFromFile(const char * pfilePath, std::string * out_errorMsg=NULL);

	void clear();

	std::string filePath;
	InstrumentMetadata metadata;
	std::vector<InstrumentSectionInfo> sections;

private:

	void loadFromJSONFile(const char * filePath);
	void loadFromGrandOrgueFile(const char * filePath);

	// Json reading

	void readMetadata(JsonBox::Value & o);
	void readSections(JsonBox::Value & o);
	void readSection(JsonBox::Value & o);
	void readNote(JsonBox::Value & o, InstrumentNoteInfo & info);
	void readNoteSounds(JsonBox::Value & o, InstrumentNoteInfo & noteInfo);
	void readNoteSound(JsonBox::Value & o, InstrumentSoundInfo & info);

};

#endif // __HEADER_INSTRUMENT_PRESET__


