#pragma once
#include <vector>
#include "Track.h"

using namespace std;
class Pattern
{
    public:
    Pattern();
    Track *patternTracks[16]; //
    uint8_t patternBPM;
    //I'm sure more things will go here,Just not super sure yet.  
    uint16_t currentNoteIndex[16]; //This is where we currently are in the sequence.
    //for playback and realtime recording, this is our note indexing system. 
    uint8_t numBars;
    private:
};