#pragma once
#include "MidiEvent.h"
#include <vector>


using namespace std;
typedef enum
{
    Step,
    Note,
    Drum
}trackType;

class Track
{
    public:
    Track(trackType inTrackType);
    vector<MidiEvent> trackEvents;
    trackType track_type;
    bool muteState; 
    uint16_t drumMuteState; //This only applies to drum tracks. 
    private:
};