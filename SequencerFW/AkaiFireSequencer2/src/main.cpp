//Global Includes
#include <Arduino.h>
#include "USBHost_t36.h"
#include <MIDI.h>

//local Includes
#include "AkaiFireControl.h"
#include "Sequencer.h"
#include "AkaiFireState.h"

// Instatiate Global Objects
USBHost myusb;
MIDIDevice AkaiFire(myusb);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi_Out1);
IntervalTimer myTimer;

AkaiFireControl *AFControl = new AkaiFireControl();
Sequencer *sequencer = new Sequencer();
AkaiFireState *AFState = new AkaiFireState(&AkaiFire, sequencer);

uint8_t msCounter = 0;
uint8_t updateFlip = 0;

void OnNoteOnWrapper(uint8_t channel, uint8_t note, uint8_t velocity)
{
  AFState->OnNoteOn(channel,note,velocity);
}

//Setup Callbacks

void forgroundLoop()
{

  if(msCounter%16==0)
  {
    //do a screen update
    updateFlip = ~updateFlip;
    if(updateFlip)
    {
      AFState->sendPadLights();
    }else
    {
      //do a screen update 
    }
  }

  msCounter++;
}

void setup() {
  myusb.begin();
  pinMode(02, OUTPUT);
  pinMode(03, OUTPUT);
  pinMode(04, OUTPUT);
  pinMode(05, OUTPUT);
  pinMode(06, OUTPUT);
  digitalWriteFast(02, LOW);
  digitalWriteFast(03, LOW);
  digitalWriteFast(04, LOW); //set all test pins to low state. 
  digitalWriteFast(05, LOW);
  digitalWriteFast(06, LOW);


  delay(4500); //wait for akai fire controller to finish boot sequence. 
  Serial.print("Track type is: ");
  Serial.println(sequencer->currentPattern->patternTracks[0]->track_type);
  AFState->stateInit();
  AkaiFire.setHandleNoteOn(OnNoteOnWrapper);

  
  midi_Out1.begin();
  myTimer.begin(forgroundLoop, 1000); //running at 1 ms = 1000nS
  Serial.println("Setup Complete");
}

void loop() {
  myusb.Task();
  AkaiFire.read();
  //AFState->updateState();

}