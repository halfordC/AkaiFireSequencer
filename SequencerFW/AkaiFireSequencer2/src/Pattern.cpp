#include "Pattern.h"

Pattern::Pattern()
{
    patternBPM = 120;
    for(int i = 0; i<16; i++)
    {
        Track *inTrack = new Track(Step);
        patternTracks[i] = inTrack;
        currentNoteIndex[i] = 0;
    }
    numBars = 4; //4 bars = 16 steps
    //This will need to be different when loading tracks
    //just need to get the basic object structure for now. 
}