#include "MIDIReceiver.h"
#include "utility.h"

//-----------------------------------------------------------------------------
void MIDIReceiver::onMessageReceived(IMidiMsg* midiMessage)
{
	IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
	// We're only interested in Note On/Off messages (not CC, pitch, etc.)
	if(status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff)
	{
		m_midiQueue.Add(midiMessage);
	}
}

//-----------------------------------------------------------------------------
void MIDIReceiver::advance()
{
	while (!m_midiQueue.Empty())
	{
		IMidiMsg* midiMessage = m_midiQueue.Peek();
		if (midiMessage->mOffset > m_offset)
			break;

		IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
		int noteNumber = midiMessage->NoteNumber();
		int velocity = midiMessage->Velocity();

		// There are only note on/off messages in the queue, see ::onMessageReceived
		if (status == IMidiMsg::kNoteOn && velocity)
		{
			if(m_keyStatus[noteNumber] == false)
			{
				m_keyStatus[noteNumber] = true;
				m_numKeys += 1;
				// Emit a "note on" signal
				noteOn(noteNumber, velocity);
			}
		}
		else
		{
			if(m_keyStatus[noteNumber])
			{
				m_keyStatus[noteNumber] = false;
				m_numKeys -= 1;
				// Emit a "note off" signal
				noteOff(noteNumber, velocity);
			}
		}
		m_midiQueue.Remove();
	}

	++m_offset;
}


