#include "MySampler.h"
#include "IPlug_include_in_plug_src.h"
#include "IKeyboardControl.h" // Must be included after MySampler.h
#include "resource.h"
#include "utility.h"

#include <stdlib.h>
#include <time.h>
#include <sstream>

#include "WaveFile.h"
#include "OrganInfo.h"

const int kNumPrograms = 4;

//-----------------------------------------------------------------------------
MySampler::MySampler(IPlugInstanceInfo instanceInfo):
	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
	lastVirtualKeyboardNoteNumber(VIRTUAL_KEYBOARD_MINIUMUM_NOTE_NUMBER - 1),
	m_graphics(0),
	//m_text(0),
	m_textControl(0),
	m_virtualKeyboard(0),
	m_fileSelector(0)
{
	TRACE;

	srand(time(NULL));

	CreateParams();
	CreateGraphics();
    CreatePresets();

	m_midiReceiver.noteOn.Connect(&m_voiceManager, &VoiceManager::onNoteOn);
	m_midiReceiver.noteOff.Connect(&m_voiceManager, &VoiceManager::onNoteOff);
}

//-----------------------------------------------------------------------------
MySampler::~MySampler()
{
}

//-----------------------------------------------------------------------------
void MySampler::CreateParams()
{
	for(unsigned int i = kParamStopsBegin; i <= kParamStopsEnd; ++i)
	{
		std::stringstream ss;
		ss << "Stop" << i;
		std::string name = ss.str();
		GetParam(i)->InitBool(name.c_str(), false, name.c_str());
	}
}

//-----------------------------------------------------------------------------
void MySampler::CreateGraphics()
{
	m_graphics = MakeGraphics(this, kWidth, kHeight);
	AttachGraphics(m_graphics);

	// Background
    m_graphics->AttachBackground(KEYBOARD_BG_ID, KEYBOARD_BG_FN);

	// Virtual keyboard
    IBitmap whiteKeyImage = m_graphics->LoadIBitmap(KEYBOARD_WHITE_KEY_ID, KEYBOARD_WHITE_KEY_FN, 6);
    IBitmap blackKeyImage = m_graphics->LoadIBitmap(KEYBOARD_BLACK_KEY_ID, KEYBOARD_BLACK_KEY_FN);
	// Key coordinates for one octave
    //                            C#     D#          F#      G#      A#
    int keyCoordinates[12] = { 0, 7, 12, 20, 24, 36, 43, 48, 56, 60, 69, 72 };
    m_virtualKeyboard = new IKeyboardControl(this,
		kKeybX, kKeybY,
		VIRTUAL_KEYBOARD_MINIUMUM_NOTE_NUMBER,
		6, // Octaves
		&whiteKeyImage,
		&blackKeyImage,
		keyCoordinates
	);
	m_graphics->AttachControl(m_virtualKeyboard);

	// State info
	IText text;
	text.mColor = IColor(255,255,255,64);
	m_textControl = new ITextControl(this, IRECT(112,10,507,29), &text, "<State info>");
	m_graphics->AttachControl(m_textControl);

	// File selector
	IBitmap openBankImage = m_graphics->LoadIBitmap(BUTTON_OPEN_BANK_ID, BUTTON_OPEN_BANK_FN);
	GetParam(kParamDummySelectedFile)->InitDouble("SelectedFile", 50., 0., 100.0, 0.01, "%");
	m_fileSelector = new IFileSelectorControl(this, IRECT(10,10,100,30), kParamDummySelectedFile, &openBankImage, kFileOpen, "", "organ");
	m_graphics->AttachControl(m_fileSelector);

	//IBitmap buttonImage = m_graphics->LoadIBitmap(BUTTON_ORGAN_STOP_ID, BUTTON_ORGAN_STOP_FN, 2);
	//ISwitchControl * test = new ISwitchControl(this, kParamStopsBegin, 50, 50, &buttonImage);
	//m_graphics->AttachControl(test);
}

//-----------------------------------------------------------------------------
void MySampler::CreateOrganControls(const OrganInfo & info)
{
	// Note: bitmaps are cached so it won't be reloaded twice
	IBitmap buttonImage = m_graphics->LoadIBitmap(BUTTON_ORGAN_STOP_ID, BUTTON_ORGAN_STOP_FN, 2);

	int x = kStopsBeginX;
	int y = kStopsBeginY;

	for(unsigned int i = 0; i < info.getStopCount() && i <= kParamStopsEnd; ++i)
	{
		const OrganStopInfo & stp = info.getStop(i);

		StopControl stopControl;
		int paramIdx = kParamStopsBegin + i;
		stopControl.button = new ISwitchControl(this, x, y, paramIdx, &buttonImage);
		m_graphics->AttachControl(stopControl.button);
		m_stopControls.push_back(stopControl);

		int frameH = buttonImage.H / buttonImage.N;
		y += frameH + 4;
		if(y > kStopsMaxY - frameH)
		{
			y = kStopsBeginY;
			x += kStopsColumnWidth;

			if(x > GUI_WIDTH - 10 - buttonImage.W)
			{
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
void MySampler::ClearOrganControls()
{
	for(unsigned int i = 0; i < m_stopControls.size(); ++i)
	{
		StopControl & sc = m_stopControls[i];
		m_graphics->DeleteControl(sc.button, true);
	}
	m_stopControls.clear();
}

//-----------------------------------------------------------------------------
void MySampler::CreatePresets()
{
	//MakeDefaultPreset((char *) "-", kNumPrograms);
	//MakePreset("Clean", 440.0);
	//MakePreset("Little", 60);
	//MakePreset("Big", 30);
	//MakePreset("Awful", 0.1);
}

//-----------------------------------------------------------------------------
void MySampler::Print(const std::string & str)
{
	m_textControl->SetTextFromPlug(str.c_str());
	m_textControl->SetDirty(true);
}

//-----------------------------------------------------------------------------
void MySampler::LoadOrgan(const char * organFilePath)
{
	std::string organPath = organFilePath;
	normalizePath(organPath);

	ClearOrganControls();

	if(m_organInfo.loadFromFile(organPath.c_str()))
	{
		CreateOrganControls(m_organInfo);
	}
}

//-----------------------------------------------------------------------------
void MySampler::ProcessMidiMsg(IMidiMsg* pMsg)
{
	m_midiReceiver.onMessageReceived(pMsg);
	m_virtualKeyboard->SetDirty();
}

//-----------------------------------------------------------------------------
void MySampler::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	// Mutex is already locked for us.

	updateStatusText();

	double *leftOutput = outputs[0];
	double *rightOutput = outputs[1];

	processVirtualKeyboard();

	for (unsigned int i = 0; i < nFrames; ++i)
	{
		m_midiReceiver.advance();
		m_voiceManager.nextSample(leftOutput[i], rightOutput[i]);
	}

	m_midiReceiver.flush(nFrames);
}

//-----------------------------------------------------------------------------
void MySampler::updateStatusText()
{
	if(m_voiceManager.organData.isLoading())
	{
		int remaining = m_voiceManager.organData.loadNext();
		unsigned int memoryUseMo = m_voiceManager.organData.getMemoryUse() / 1000000;
		std::stringstream ss;
		if(remaining >= 0)
			ss << "Loading samples (" << remaining << ")... ";
		else
			ss << "Ready ";
		ss << "[" << memoryUseMo << " Mo]";
		Print(ss.str());
	}
}

//-----------------------------------------------------------------------------
void MySampler::Reset()
{
	TRACE;
	IMutexLock lock(this);
	Oscillator::setSampleRate(GetSampleRate());
	EnvelopeGenerator::setSampleRate(GetSampleRate());
}

//-----------------------------------------------------------------------------
void MySampler::OnParamChange(int paramIdx)
{
	IMutexLock lock(this);

	if(paramIdx >= kParamStopsBegin && paramIdx <= kParamStopsEnd)
	{
		unsigned int stopIndex = paramIdx - kParamStopsBegin;
		if(stopIndex < m_organInfo.getStopCount())
		{
			bool active = GetParam(paramIdx)->Bool();

			OrganData & organData = m_voiceManager.organData;

			if(!organData.hasStop(stopIndex))
			{
				// Start loading if not already done
				if(!organData.isLoadingStop(stopIndex))
				{
					std::vector<OrganStopInfo> stops;
					stops.push_back(m_organInfo.getStop(stopIndex));

					std::string organPath = m_organInfo.getFilePath();
					std::string organDir = organPath.substr(0, organPath.rfind('/'));
					organData.loadStopsData(organDir, stops);
				}
			}

			if(organData.hasStop(stopIndex))
			{
				// Set active flag
				// Note: stopData should always exist because it is created above
				OrganStopData & stopData = organData.getStop(stopIndex);
				stopData.enabled = active;
			}
		}
	}

	switch (paramIdx)
	{
	case kParamDummySelectedFile:
		{
			WDL_String str;
			m_fileSelector->GetLastSelectedFileForPlug(&str);
			LoadOrgan(str.Get());
		}
		break;

	//case kFrequency:
	//	m_oscillator.setFrequency(GetParam(kFrequency)->Value());
	//	break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
void MySampler::processVirtualKeyboard()
{
	IKeyboardControl* virtualKeyboard = (IKeyboardControl*)m_virtualKeyboard;
	const int minimalNote = VIRTUAL_KEYBOARD_MINIUMUM_NOTE_NUMBER;
	int virtualKeyboardNoteNumber = virtualKeyboard->GetKey() + minimalNote;

	if(lastVirtualKeyboardNoteNumber >= minimalNote && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber)
	{
		// The note number has changed from a valid key to something else (valid key or nothing). Release the valid key:
		IMidiMsg midiMessage;
		midiMessage.MakeNoteOffMsg(lastVirtualKeyboardNoteNumber, 0);
		m_midiReceiver.onMessageReceived(&midiMessage);
	}

	if (virtualKeyboardNoteNumber >= minimalNote && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber)
	{
		// A valid key is pressed that wasn't pressed the previous call. Send a "note on" message to the MIDI receiver:
		IMidiMsg midiMessage;
		midiMessage.MakeNoteOnMsg(virtualKeyboardNoteNumber, virtualKeyboard->GetVelocity(), 0);
		m_midiReceiver.onMessageReceived(&midiMessage);
	}

	lastVirtualKeyboardNoteNumber = virtualKeyboardNoteNumber;
}

