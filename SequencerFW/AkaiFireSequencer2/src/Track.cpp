#include "Track.h"

Track::Track(trackType inTracktype)
{
    track_type = inTracktype;
    muteState = false; //created tracks start un-muted
    drumMuteState = 0; //this is a bitwise value.
}