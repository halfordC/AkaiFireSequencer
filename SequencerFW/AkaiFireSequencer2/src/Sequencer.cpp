#include "Sequencer.h"

Sequencer::Sequencer()
{

    //we may need to instatiate all patterns, not just one. 
    Pattern *defaultPattern = new Pattern();
    generatemS();
    allPatterns[0] = defaultPattern;
    currentPattern = allPatterns[0];
    playState = stopped;
}

void Sequencer::inrecementmS()
{
    if(playState ==playing||playState == recording)
    {
        currentmS++;
        if(currentmS > msMax || currentmS > currentPattern->numBars*4*msPB)
        {
            currentmS = 0;
        }
    }else
    {
        currentmS = 1;
    }
}

uint16_t Sequencer::readyToPlay()
{
    if(playState == stopped || playState == armed)
    {
        return 0; //not ready to play.
    }
    //this is where we need to find the notes in the pattern that match our notes in 
    for(int i = 0; i<16; i++)
    {
        if(!currentPattern->patternTracks[i]->trackEvents.empty()) //we should also check our Mutes here. 
        {
            uint32_t noteTime = currentPattern->patternTracks[i]->trackEvents[currentPattern->currentNoteIndex[i]].msStamp;
            if(noteTime >= currentmS)
            {
                notesToPlay = notesToPlay | (1<<i); //shift in what current note tracks we need to play.
            }
        }
    }
    return notesToPlay; 
}

MidiEvent *Sequencer::playNotes(midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> midiObject)
{
    //we need to play the note, 
    MidiEvent *currentNote;
    for(int i = 0; i<16; i++)
    {
        if(notesToPlay & (1 << i))
        {
            //TODO
            //Play Current Note here!:
            currentNote = &(currentPattern->patternTracks[i]->trackEvents[currentPattern->currentNoteIndex[i]]);

            switch(currentNote->eventType)
            {
                case NoteOn:
                    midiObject.sendNoteOn(currentNote->value1, currentNote->value2, currentNote->channel);
                break;

                case NoteOff:
                    midiObject.sendNoteOff(currentNote->value1, currentNote->value2, currentNote->channel);
                break;

                default:
                break;
            }
            

            // cleanup, and prepare for next note. 
            currentPattern->currentNoteIndex[i]++;
            notesToPlay = notesToPlay & (~(1<<i)); //turn off bit
        }
    }
    return currentNote;
}

void Sequencer::addEvent(uint16_t trackNumber, MidiEvent inEvent)
{
    currentPattern->patternTracks[trackNumber]->trackEvents.push_back(inEvent);
}

uint32_t Sequencer::getmsPB()
{
    return msPB;
}

uint64_t Sequencer::getCurrentMs()
{
    return currentmS;
}

void Sequencer::generatemS()
{
    float BPS = currentPattern->patternBPM / 60; //this is our beats per second. We don't expect it to be very large. 400 / 60 = 6.66666
    msPB = BPS*1000; //millisecond per beat. This 
    msMax = msPB*MAX_BARS;
}
