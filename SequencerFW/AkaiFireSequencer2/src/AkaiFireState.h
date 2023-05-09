#pragma once
#include <stdint.h>
#include "USBHost_t36.h"
#include "AkaiFireControl.h"
#include "Sequencer.h"
#include <vector>

#define PatternMode 0
#define SongMode 1
#define noteMode 2
#define drumMode 3
#define stepMode 4

#define STEP 0x2C
#define NOTE 0x2D
#define DRUM 0x2E
#define PERFORM 0x2F
#define SHIFT 0x30
#define ALT 0x31
#define PATTERN_SONG 0x32
#define PLAY 0x33
#define STOP 0x34
#define REC 0x35
#define SOLO0 0x24
#define SOLO1 0x25
#define SOLO2 0x26
#define SOLO3 0x27
#define CC_BANK 0x1A
#define PAT_UP 0x1F
#define PAT_DOWN 0x20
#define BROWSER 0x21
#define GRID_LEFT 0x22
#define GRID_RIGHT 0x23

#define MUTE_OFF 0
#define MUTE_ON 1
#define SOLO_ON 2

class AkaiFireState
{
    public:
    AkaiFireState(MIDIDevice *AkaiFireInput, Sequencer *inSequencer); //taking in this in will allow us to send and receive midi stuff in this object. 
    void stateInit();
    void OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    void OnControlChange(uint8_t channel, uint8_t control, uint8_t value);    
    uint32_t getPadLight(uint8_t row, uint8_t column);
    void sendPadLights();
    void sendPadLightsNoClear();
    void writePadLight(uint8_t row, uint8_t column, uint32_t color);
    void writePadRow(uint8_t row, uint32_t color);
    void writePadRow(uint8_t row, uint32_t color[16]);
    bool checkNewPadLights();
    void printPadLights();


    private:
    MIDIDevice *akaiFire;
    AkaiFireControl *AFControl;
    Sequencer *sequencer;
    uint8_t padLightIndex[4][48]; //RGB for each pad. 
    //vector<uint8_t> padLightsToSend;
    uint8_t performanceMode;
    uint8_t selectedTrack;
    uint8_t selectedDrum;
    uint8_t trackMuteSolo;
    uint8_t drumMuteSolo; 
    uint16_t tracksOn; //each bit represents a mute. We can solo by inverting the mute function.
    uint16_t drumsOn;
    uint16_t stepLights;
    uint8_t stepDiv;
    uint16_t padChangeArray[4] = { 0 };



    uint32_t noteModeColorIndex[16] = 
    {
        WHITE_KEY, //A
        BLACK_KEY, //A#
        WHITE_KEY, //B
        WHITE_KEY, //C
        BLACK_KEY, //C#
        WHITE_KEY, //D
        BLACK_KEY, //D#
        WHITE_KEY, //E
        WHITE_KEY, //F
         BLACK_KEY, //F#
        WHITE_KEY, //G
        BLACK_KEY, //G#
        WHITE_KEY, //A
        BLACK_KEY, //A#
        WHITE_KEY, //B
        BLACK_KEY //C
    };

};