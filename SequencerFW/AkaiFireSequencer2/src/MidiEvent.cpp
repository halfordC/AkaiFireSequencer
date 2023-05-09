#include "MidiEvent.h"

MidiEvent::MidiEvent(uint32_t inTimestamp, midiEventType inType, uint8_t inChannel, uint8_t inV1, uint8_t inV2)
{
    msStamp = inTimestamp;
    eventType = inType;
    channel = inChannel;
    value1 = inV1;
    value2 = inV2;
}