#include "AkaiFireState.h"



AkaiFireState::AkaiFireState(MIDIDevice *AkaiFireInput, Sequencer *inSequencer)
{
    akaiFire = AkaiFireInput;
    AFControl = new AkaiFireControl();
    sequencer = inSequencer;

    drumsOn = 0xFFFF; //start up with no mutes. 
    tracksOn = 0xFFFF;
    selectedDrum = 0;
    selectedTrack = 0;
    trackMuteSolo = 0;
    stepLights = 0;
    stepDiv = 4; //our default is 4 steps ber beat. 
}

void AkaiFireState::stateInit()
{
    //set default mode
    performanceMode = PatternMode;

    //send control changes to light all the non-pad buttons
    akaiFire->sendControlChange(STEP,0x04,0x01);
    akaiFire->sendControlChange(NOTE,0x02,0x01);
    akaiFire->sendControlChange(DRUM,0x02,0x01);
    akaiFire->sendControlChange(PERFORM,0x04,0x01);
    akaiFire->sendControlChange(ALT,0x01,0x01);

    //have the first option from each selected
    writePadLight(0,0,PATTERN_ROW_COLOR*4);
    writePadLight(1,0,TRACK_ROW_COLOR*4);
    writePadLight(2,0,DRUM_ROW_COLOR*4);
    //writePadLight(3,0,STEP_ROW_COLOR);

    //now we need to set each pad in our array
    for(int i = 1; i<16; i++)
    {
        writePadLight(0,i,PATTERN_ROW_COLOR);
        writePadLight(1,i,TRACK_ROW_COLOR);
        writePadLight(2,i,DRUM_ROW_COLOR);
        //writePadLight(3,i,STEP_ROW_COLOR);
    } 
    switch(sequencer->currentPattern->patternTracks[0]->track_type)
    {
        case Step:
        writePadRow(3,STEP_ROW_COLOR);
        break;

        case Note:
        writePadRow(3,noteModeColorIndex);
        break;

        case Drum:
        writePadRow(3,DRUM_ROW_COLOR);
        break;
    }
    printPadLights();
    sendPadLights(); //send the lights

}

void AkaiFireState::OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if(note >= 0x36 && note <= 0x75)
    {
        uint8_t Row = (note - 0x36)>>4;
        uint8_t Column = (note-0x36)&0x0F;

        switch(Row)
        {
            case 0:
            break;

            case 1:
            {
                uint8_t isNotMuted = (tracksOn>>Column)&0x01;
                if(trackMuteSolo == MUTE_ON && isNotMuted) //if the mute button is held down, and the track is un-muted.
                {
                    writePadLight(Row, Column, TRACK_MUTED);
                    tracksOn = tracksOn & (~(0x01<<Column));

                }else if(trackMuteSolo == MUTE_ON) //this means that the track is already muted, so we just need to un-mute it. 
                {
                    if(selectedTrack == Column)
                    {
                        writePadLight(Row, Column, TRACK_SELECTED_COLOR);
                    }else
                    {
                        writePadLight(Row, Column, TRACK_ROW_COLOR);
                    }
                    
                    tracksOn = tracksOn | (1<<Column);
                }else{ 
                    if(isNotMuted) //if the track isn't muted, we can select it. 
                    {
                        writePadLight(Row, Column, TRACK_SELECTED_COLOR);
                        
                        if(sequencer->currentPattern->patternTracks[Column]->track_type != sequencer->currentPattern->patternTracks[selectedTrack]->track_type)
                        {
                            //this is where we update row 3 based on the new track selection. 
                        }
                    }

                    if((tracksOn >> selectedTrack)&0x01 && selectedTrack != Column)
                    {
                        //unlight previously selected track.
                        writePadLight(Row, selectedTrack, TRACK_ROW_COLOR);
                    }
                
                    //Ideally, we make this a funtion where we give cords to un-light the previous lightm and light the new one,
                    //and just send the sysex onces, instead of twice. But we can implement that later. 
                    //The lighting system will probably get more involved soon. 
                    selectedTrack = Column;
                }
            }
            break;

            case 2:
            {
                uint8_t isNotMuted = (drumsOn>>Column)&0x01;
                if(drumMuteSolo == MUTE_ON && isNotMuted) //if the mute button is held down, and the track is un-muted.
                {
                    writePadLight(Row, Column, DRUM_MUTED);
                    drumsOn = drumsOn & (~(0x01<<Column));
                }else if(drumMuteSolo == MUTE_ON)
                {
                    if(selectedTrack == Column)
                    {
                        writePadLight(Row, Column, DRUM_SELECTED_COLOR);
                    }else
                    {
                        writePadLight(Row, Column, DRUM_ROW_COLOR);
                    }
                    
                    drumsOn = drumsOn | (1<<Column);

                }else
                {
                    if(isNotMuted)
                    {
                        writePadLight(Row, Column, DRUM_SELECTED_COLOR);
                    }

                    if((drumsOn >> selectedDrum)&0x01 && selectedDrum != Column)
                    {           
                        //unlight previously selected track. 
                        writePadLight(Row, selectedDrum, DRUM_ROW_COLOR);
                    }
                    //Ideally, we make this a funtion where we give cords to un-light the previous lightm and light the new one,
                    //and just send the sysex onces, instead of twice. But we can implement that later. 
                    selectedDrum = Column;

                }
            }
            break;

            case 3:
            switch(sequencer->currentPattern->patternTracks[selectedTrack]->track_type)
                {
                    case Step:
                        if(!(stepLights & (1<<Column))) //if the light is off, turn it on
                        {
                            writePadLight(Row, Column, STEP_SELECTED_COLOR);

                            stepLights = stepLights | (1<<Column);

                            //turn on step, and add step to relevant track. 
                            uint64_t insertMs = sequencer->getmsPB()*Column*stepDiv;
                            MidiEvent *insertMidiEvent = new MidiEvent(insertMs,NoteOn, selectedTrack, 0x32, 255); //currently, track = channel. 
                            //this will probably change with settings. 

                            sequencer->addEvent(selectedTrack, *insertMidiEvent);
                        }else 
                        {
                            //step is already on, so turn it off. 
                            writePadLight(Row, Column, STEP_ROW_COLOR);
                            stepLights = stepLights & ~(1<<Column);

                            //we also want to delete the event from the track list. I wonder if there's a really fast way to do this?
                        }

                    break;

                    case Note:
                        //if record button is on, then we want to record played in notes
                        //if record button is not on, we just want to pass notes through.
                    break;

                    case Drum:
                        //Same as nte mode, but we need a different structure for storing notes
                        //since each midi button is not necssarily in chromatic order. 
                    break;

                }
            break;


        }

    }
    else if(note < 0x36)
    {
        switch(note)
        {
            case STEP:
                sequencer->currentPattern->patternTracks[selectedTrack]->track_type = Step;
                writePadRow(3, STEP_ROW_COLOR);
                akaiFire->sendControlChange(STEP,0x04, 0x01);
                akaiFire->sendControlChange(NOTE,0x02, 0x01);
                akaiFire->sendControlChange(DRUM,0x02, 0x01);
            break;

            case NOTE:
                sequencer->currentPattern->patternTracks[selectedTrack]->track_type = Note;
                writePadRow(3,noteModeColorIndex);
                akaiFire->sendControlChange(STEP,0x02, 0x01);
                akaiFire->sendControlChange(NOTE,0x04, 0x01);
                akaiFire->sendControlChange(DRUM,0x02, 0x01);
            break;

            case DRUM:
                sequencer->currentPattern->patternTracks[selectedTrack]->track_type = Drum;
                writePadRow(3,DRUM_IN_ROW_COLOR);
                akaiFire->sendControlChange(STEP,0x02, 0x01);
                akaiFire->sendControlChange(NOTE,0x02, 0x01);
                akaiFire->sendControlChange(DRUM,0x04, 0x01);
            break;


            case PERFORM:
                performanceMode = PatternMode;
                writePadRow(0,PATTERN_ROW_COLOR);
                switch(sequencer->currentPattern->patternTracks[selectedTrack]->track_type)
                {
                    case Note:
                        writePadRow(3,noteModeColorIndex);
                    break;

                    case Drum:
                        writePadRow(3,DRUM_IN_ROW_COLOR);
                    break;

                    case Step:
                        writePadRow(3,STEP_ROW_COLOR); //this also needs to populate step lights. Should probably make an array. 
                    break;

                }
                
                akaiFire->sendControlChange(PERFORM, 0x04, 0x01);
                akaiFire->sendControlChange(PATTERN_SONG, 0x02, 0x01);

            break;

            case PATTERN_SONG:
                //digitalWriteFast(06,HIGH);
                writePadRow(0,SONG_ROW_COLOR);
                writePadRow(3,PATTERN_ROW_COLOR);
                akaiFire->sendControlChange(PERFORM, 0x02, 0x01);
                akaiFire->sendControlChange(PATTERN_SONG, 0x04, 0x01);
                performanceMode = SongMode;
            break;

            case SOLO1: //Track Mute / solo
                trackMuteSolo = MUTE_ON; //we'll do this for now until we have Shift figured out. 
            break;
            
            case SOLO2:
                drumMuteSolo = MUTE_ON;
            break;

            case PLAY:
                if(sequencer->playState == armed)
                {
                    sequencer->playState = recording;
                    akaiFire->sendControlChange(PLAY,0x03, 0x01); //set play to dull yellow
                    akaiFire->sendControlChange(STOP,0x01,0x01); //set stop to bright yellow
                    akaiFire->sendControlChange(REC, 0x03, 0x01); //set recording to high red
                    
                }else if(sequencer->playState == stopped)
                {
                    sequencer->playState = playing;
                    akaiFire->sendControlChange(PLAY,0x03, 0x01); //set play to bright green
                    akaiFire->sendControlChange(STOP,0x01,0x01); //set stop to dull
                    akaiFire->sendControlChange(REC, 0x02, 0x01); //set recording to dull yellow
                    
                } //if already playing, do nothing. if already recording, also do nothing. 
            break;

            case STOP:
                sequencer->playState = stopped;
                akaiFire->sendControlChange(PLAY,0x01, 0x01); //set play to dull yellow
                akaiFire->sendControlChange(STOP,0x02,0x01); //set stop to bright yellow
                akaiFire->sendControlChange(REC, 0x02, 0x01); //set recording to Dull yellow
            break;

            case REC:
                if(sequencer->playState == stopped)
                {
                    sequencer->playState = armed;
                    akaiFire->sendControlChange(PLAY,0x01, 0x01); //set play to dull yellow
                    akaiFire->sendControlChange(STOP,0x02,0x01); //set stop to bright yellow
                    akaiFire->sendControlChange(REC, 0x03, 0x01); //set recording to Bright Red
                }else if(sequencer->playState == playing)
                {
                    sequencer->playState = recording;
                    akaiFire->sendControlChange(PLAY,0x03, 0x01); //set play to dull yellow
                    akaiFire->sendControlChange(STOP,0x01,0x01); //set stop to bright yellow
                    akaiFire->sendControlChange(REC, 0x03, 0x01); //set recording to high red
                }
            break;


        }
    }

}


uint32_t AkaiFireState::getPadLight(uint8_t row, uint8_t column)
{
    uint8_t red = padLightIndex[row*3][column];
    uint8_t green = padLightIndex[(row*3)+1][column];
    uint8_t blue = padLightIndex[(row*3)+2][column];

    uint32_t color = (uint32_t) (red << 16) | (green << 8) | blue;
    return color;
}

void AkaiFireState::sendPadLights()
{
    //if we have no updates, do not send. 
    if(!checkNewPadLights())
    {
        return;
    }


    AFControl->sendPadUpdate(padChangeArray, padLightIndex);
    akaiFire->sendSysEx(AFControl->sysexBuffer.size(), AFControl->sysexBuffer.data(), true);

}

void AkaiFireState::writePadLight(uint8_t row, uint8_t column, uint32_t color)
{

    padLightIndex[row][column*3] = (uint8_t)(color>>16); //red
    padLightIndex[row][(column*3)+1] = (uint8_t)(color>>8); //green
    padLightIndex[row][(column*3)+2] = (uint8_t)(color); //blue

    padChangeArray[row] = padChangeArray[row] | (1 << column); //shift in a changed value, so we know that pad has been updated.
}

void AkaiFireState::writePadRow(uint8_t row, uint32_t color)
{
    for(int i = 0; i<16; i++)
    {
        writePadLight(row, i, color);
    }
}

void AkaiFireState::writePadRow(uint8_t row, uint32_t color[16])
{
    for(int i = 0; i<16; i++)
    {
        writePadLight(row, i, color[i]);
    }   
}

bool AkaiFireState::checkNewPadLights()
{
    for(uint8_t i = 0; i<4; i++)
    {
        if(padChangeArray[i])
        {
            return true;
        }
    }
    return false;
}

void AkaiFireState::printPadLights()
{
    for(uint8_t row = 0; row<4; row++)
    {
        for(uint8_t col = 0; col<16; col++)
        {
            Serial.print(padLightIndex[row][col*3], HEX);
            Serial.print(padLightIndex[row][(col*3)+1], HEX);
            Serial.print(padLightIndex[row][(col*3)+2], HEX);
            Serial.print(",");
        }
        Serial.println("");
    }
    Serial.println("Pad Changes:");
    Serial.println(padChangeArray[0], HEX);
    Serial.println(padChangeArray[1], HEX);
    Serial.println(padChangeArray[2], HEX);
    Serial.println(padChangeArray[3], HEX);
}
