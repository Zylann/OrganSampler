#ifndef __MYSAMPLER__
#define __MYSAMPLER__

#include "MIDIReceiver.h"
#include "VoiceManager.h"

//-----------------------------------------------------------------------------
enum EParams
{
	kParamDummySelectedFile = 0,

	// Parameters of a plugin are fixed, but organs have variable controls count,
	// So we have to reserve values for them.
	kParamStopsBegin = 1,
	//...
	kParamStopsEnd = 63,

	kNumParams = 64
};

//-----------------------------------------------------------------------------
enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,

	kKeybX = 1,
    kKeybY = kHeight - 66,
	kStopsBeginX = 10,
	kStopsBeginY = 40,
	kStopsMaxY = 224,
	kStopsColumnWidth = 100
	//kFrequencyX = 100,
	//kFrequencyY = 100,
	//kKnobFrames = 60
};

struct StopControl
{
	ISwitchControl * button;
};

class MySampler : public IPlug
{
public:
	MySampler(IPlugInstanceInfo instanceInfo);
	~MySampler();

	void Reset();
	void OnParamChange(int paramIdx);
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

	// to receive MIDI messages:
	void ProcessMidiMsg(IMidiMsg* pMsg);

	void LoadOrgan(const char * organFilePath);

	// Debug
	void Print(const std::string & str);

	//-----------------------------------
	// Needed for the GUI keyboard:
	//-----------------------------------

    // Should return non-zero if one or more keys are playing.
    inline int GetNumKeys() const { return m_midiReceiver.getNumKeys(); }

    // Should return true if the specified key is playing.
    inline bool GetKeyStatus(int key) const { return m_midiReceiver.getKeyStatus(key); }

    int lastVirtualKeyboardNoteNumber;

	static const int VIRTUAL_KEYBOARD_MINIUMUM_NOTE_NUMBER = 36;

private:
	void CreateParams();
	void CreateGraphics();
	void CreatePresets();
	
	void CreateOrganControls(const OrganInfo & info);
	void ClearOrganControls();

	void processVirtualKeyboard();

	OrganInfo m_organInfo;

	MIDIReceiver m_midiReceiver;
	VoiceManager m_voiceManager;

	IGraphics * m_graphics;
	//IText * m_text;
	ITextControl * m_textControl;
	IControl* m_virtualKeyboard;
	IFileSelectorControl* m_fileSelector;
	std::vector<StopControl> m_stopControls;

	//double m_frequency;
};

#endif
