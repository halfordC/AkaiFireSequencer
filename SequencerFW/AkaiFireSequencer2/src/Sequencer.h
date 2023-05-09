#pragma once
#include <stdint.h>
#include <vector>
#include "Pattern.h"
#include "MidiEvent.h"
#include <MIDI.h>

using namespace std;

#define MAX_PATTERNS 256
#define MAX_BARS 256

typedef enum{
    playing,
    stopped,
    recording,
    armed,
}play_State;

class Sequencer
{
    public:
    Sequencer();
    void inrecementmS();
    uint16_t readyToPlay();
    MidiEvent* playNotes(midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> midiObject);
    void addEvent(uint16_t trackNumber, MidiEvent inEvent);
    uint32_t getmsPB();
    uint64_t getCurrentMs();

    Pattern *allPatterns[MAX_PATTERNS]; 
    Pattern *currentPattern;
    play_State playState;
    
    
    private:
    void generatemS(); //this is how we get new BPMs
    uint32_t msPB;
    uint64_t msMax; //this is the maximum number of MS in a 256 bar loop
    uint64_t currentmS; //this gets incremented every super loop. It's max value is detirmed by msMax.
    uint16_t notesToPlay;

};