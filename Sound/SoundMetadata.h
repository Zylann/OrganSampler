#ifndef __HEADER_SOUND_METADATA__
#define __HEADER_SOUND_METADATA__

#include <string>
#include <vector>

class SoundMetadata
{
public:
	SoundMetadata():
		midiNote(0)
	{}

	struct Cue
	{
		Cue():
			ID(0),
			position(0)
		{}

		std::string label;
		unsigned int ID;
		unsigned int position;
	};

	struct Loop
	{
		Loop():
			begin(0),
			end(0)
		{}

		unsigned int begin;
		unsigned int end;
	};

	std::vector<Cue> cues;
	std::vector<Loop> loops;
	unsigned int midiNote;
};

#endif // __HEADER_SOUND_METADATA__

