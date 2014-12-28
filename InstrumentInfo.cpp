#include "InstrumentInfo.h"
#include "GrandOrgue/OrganInfo.h"
#include <fstream>
#include <cassert>
#include <sstream>


//------------------------------------------------------------------------------
// Helper
int loadFromFile(JsonBox::Value & document, const std::string & filePath, int checkVersion)
{
    std::ifstream ifs(filePath.c_str(), std::ios::in|std::ios::binary);
    if(!ifs.good())
    {
        return -1;
    }

    // Parse stream

    document.loadFromStream(ifs);
    ifs.close();

    // Check document

    assert(document.isObject());

    if(checkVersion >= 0)
    {
        int version = document["formatversion"].getInt();
        if(version != checkVersion)
        {
            return -2;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
bool InstrumentInfo::loadFromFile(const char * pfilePath, std::string * out_errorMsg)
{
	clear();

	filePath = pfilePath;

	try
	{
		if(strstr(pfilePath, ".json"))
		{
			loadFromJSONFile(pfilePath);
		}
		else if(strstr(pfilePath, ".organ"))
		{
			loadFromGrandOrgueFile(pfilePath);
		}
		else
		{
			throw "Could not determine file format";
		}
	}
	catch(std::string errorMsg)
	{
		// Error reading file
		if(out_errorMsg)
			*out_errorMsg = errorMsg;
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
void InstrumentInfo::clear()
{
	metadata = InstrumentMetadata();
	sections.clear();
}

//-----------------------------------------------------------------------------
void InstrumentInfo::loadFromJSONFile(const char * filePath)
{
	JsonBox::Value document;
	if(!::loadFromFile(document, filePath, 1))
	{
		throw "Version mismatch";
	}

	readMetadata(document["metadata"]);
	readSections(document["sections"]);
}

//-----------------------------------------------------------------------------
void InstrumentInfo::readMetadata(JsonBox::Value & o)
{
	InstrumentMetadata meta;

	if(o["name"].isString())         meta.name = o["name"].getString();
	if(o["author"].isString())       meta.author = o["author"].getString();
	if(o["description"].isString())  meta.description = o["description"].getString();
	if(o["url"].isString())          meta.url = o["url"].getString();
	if(o["version"].isInteger())     meta.version = o["version"].getInt();

	metadata = meta;
}

//-----------------------------------------------------------------------------
void InstrumentInfo::readSections(JsonBox::Value & o)
{
	size_t count = o.getArray().size();
	for(size_t i = 0; i < count; ++i)
	{
		readSection(o[i]);
	}
}

//-----------------------------------------------------------------------------
void InstrumentInfo::readSection(JsonBox::Value & o)
{
	InstrumentSectionInfo sectionInfo;

	if(o["name"].isString())
		sectionInfo.name = o["name"].getString();

	if(o["format"].isString())
		sectionInfo.loadingFormatStr = o["format"].getString();

	if(sectionInfo.loadingFormatStr == "json")
	{
		// Notes are defined in JSON

		if(o["notes"].isArray())
		{
			size_t noteCount = o["notes"].getArray().size();
			JsonBox::Value & a = o["notes"];
			for(size_t i = 0; i < noteCount; ++i)
			{
				InstrumentNoteInfo noteInfo;
				readNote(a[i], noteInfo);
				if(sectionInfo.notes.find(noteInfo.midiNumber) == sectionInfo.notes.end())
				{
					sectionInfo.notes[noteInfo.midiNumber] = noteInfo;
				}
				else
				{
					std::stringstream ss;
					ss << "Midi note " << noteInfo.midiNumber << " declared twice";
					throw ss.str();
				}
			}
		}
	}
	else
	{
		throw "Unknown or unsupported section format '" + sectionInfo.loadingFormatStr + "'";
	}

	sectionInfo.ID = sections.size();
	sections.push_back(sectionInfo);
}

//-----------------------------------------------------------------------------
void InstrumentInfo::readNote(JsonBox::Value & o, InstrumentNoteInfo & info)
{
	if(o["midi"].isInteger())
	{
		info.midiNumber = o["midi"].getInt();
		if(info.midiNumber < 0 || info.midiNumber > 127)
		{
			std::stringstream ss;
			ss << "Note midi number " << info.midiNumber << " is invalid";
			throw ss.str();
		}
	}
	else
	{
		throw "Note midi number not found";
	}

	if(o["sounds"].isArray())
	{
		JsonBox::Value & a = o["sounds"];
		size_t soundsCount = a.getArray().size();
		for(size_t i = 0; i < soundsCount; ++i)
		{
			JsonBox::Value s = a[i];
			readNoteSounds(s, info);
		}
	}
}

//-----------------------------------------------------------------------------
void InstrumentInfo::readNoteSounds(JsonBox::Value & o, InstrumentNoteInfo & noteInfo)
{
	if(o.isObject())
	{
		int vel = 100;
		if(o["vel"].isInteger())
			vel = o["vel"].getInt();

		if(vel < 0 || vel > 100)
		{
			std::stringstream ss;
			ss << "Sound velocity " << vel << " is out of bounds";
			throw ss.str();
		}

		std::vector<InstrumentSoundInfo> sounds;
		if(o["files"].isArray())
		{
			JsonBox::Value & fl = o["files"];
			size_t filesCount = fl.getArray().size();
			for(size_t i = 0; i < filesCount; ++i)
			{
				InstrumentSoundInfo soundInfo;
				readNoteSound(fl[i], soundInfo);
				sounds.push_back(soundInfo);
			}
		}
		else if(!o["file"].isNull())
		{
			InstrumentSoundInfo soundInfo;
			readNoteSound(o["file"], soundInfo);
			sounds.push_back(soundInfo);
		}
		else
		{
			throw "No files given in note sounds";
		}

		std::vector<InstrumentSoundInfo> & storedFiles = noteInfo.sounds[vel];
		for(size_t i = 0; i < sounds.size(); ++i)
		{
			storedFiles.push_back(sounds[i]);
		}
	}
	else
	{
		throw "Invalid JSON note sound format";
	}
}

//-----------------------------------------------------------------------------
void InstrumentInfo::readNoteSound(JsonBox::Value & o, InstrumentSoundInfo & info)
{
	if(o.isString())
	{
		info.file = o.getString();
	}
	else if(o.isObject())
	{
		if(o["amp"].isInteger())
			info.amplitudeFactor = static_cast<float>(o["amp"].getInt()) / 100.f;
		else if(o["amp"].isDouble())
			info.amplitudeFactor = static_cast<float>(o["amp"].getDouble());

		if(o["file"].isString())
			info.file = o["file"].getString();
		else
			throw "No file given in extended note sound info";
	}
	else
	{
		throw "Invalid JSON note file format";
	}
}

//-----------------------------------------------------------------------------
void InstrumentInfo::loadFromGrandOrgueFile(const char * filePath)
{
	// Load 

	OrganInfo organInfo;
	if(!organInfo.loadFromFile(filePath))
	{
		throw "Could not load organ file";
	}

	// Translate

	for(unsigned int i = 0; i < organInfo.getStopCount(); ++i)
	{
		InstrumentSectionInfo sectionInfo;
		const OrganStopInfo & organStopInfo = organInfo.getStop(i);

		sectionInfo.name = organStopInfo.name;

		for(unsigned int j = 0; j < organStopInfo.noteFiles.size(); ++j)
		{
			InstrumentNoteInfo noteInfo;
			noteInfo.midiNumber = j + 32; // TODO FIXME First note seems not available in GrandOrgue?

			InstrumentSoundInfo soundInfo;
			soundInfo.amplitudeFactor = static_cast<float>(organStopInfo.amplitudeLevel) / 100.f;
			soundInfo.file = organStopInfo.noteFiles[j];

			noteInfo.sounds[100].push_back(soundInfo);

			sectionInfo.notes[noteInfo.midiNumber] = noteInfo;
		}

		sectionInfo.ID = sections.size();
		sections.push_back(sectionInfo);
	}

}



