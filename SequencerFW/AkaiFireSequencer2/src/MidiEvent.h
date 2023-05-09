#pragma once
#include <stdint.h>

typedef enum
{
    NoteOn,
    NoteOff,
    CC,
    ProgramChange,
    PitchBend,
    AfterTouch,
    polyAftertouch
}midiEventType;


class MidiEvent
{
    public:
    MidiEvent(uint32_t inTimestamp, midiEventType inType, uint8_t inChannel, uint8_t inV1, uint8_t inV2);
    uint32_t msStamp; //this is when we play this event. 
    midiEventType eventType;
    uint8_t channel;
    uint8_t value1;
    uint8_t value2;
    //uint8_t port; //for later development
    private:
};