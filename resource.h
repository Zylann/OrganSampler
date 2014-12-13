#define PLUG_MFR "Zylann"
#define PLUG_NAME "MySampler"

#define PLUG_CLASS_NAME MySampler

#define BUNDLE_MFR "Zylann"
#define BUNDLE_NAME "MySampler"

#define PLUG_ENTRY MySampler_Entry
#define PLUG_VIEW_ENTRY MySampler_ViewEntry

#define PLUG_ENTRY_STR "MySampler_Entry"
#define PLUG_VIEW_ENTRY_STR "MySampler_ViewEntry"

#define VIEW_CLASS MySampler_View
#define VIEW_CLASS_STR "MySampler_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'Zt01'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'Acme'

// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'}; // AudioSuite
#endif

#define PLUG_MFR_PT "Zylann\nZylann\nAcme"
#define PLUG_NAME_PT "MySampler\nIPEF"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

//#define PLUG_CHANNEL_IO "0-1 0-2"
#if (defined(AAX_API) || defined(RTAS_API)) 
#define PLUG_CHANNEL_IO "1-1 2-2"
#else
// no audio input. mono or stereo output
#define PLUG_CHANNEL_IO "0-1 0-2"
#endif

#define PLUG_LATENCY 0
#define PLUG_IS_INST 1

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 1

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.
#define KNOB_ID 101
#define KEYBOARD_BG_ID         102
#define KEYBOARD_WHITE_KEY_ID  103
#define KEYBOARD_BLACK_KEY_ID  104
#define BUTTON_OPEN_BANK_ID    105
#define BUTTON_ORGAN_STOP_ID   106

// Image resource locations for this plug.
#define KNOB_FN "resources/img/knob.png"
#define KEYBOARD_BG_FN         "resources/img/bg.png"
#define KEYBOARD_WHITE_KEY_FN  "resources/img/keyboard_white_pressed.png"
#define KEYBOARD_BLACK_KEY_FN  "resources/img/keyboard_black_pressed.png"
#define BUTTON_OPEN_BANK_FN    "resources/img/open_organ_button.png"
#define BUTTON_ORGAN_STOP_FN   "resources/img/stop_button.png"

// GUI default dimensions
#define GUI_WIDTH 518
#define GUI_HEIGHT 300

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Instrument|Synth"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
