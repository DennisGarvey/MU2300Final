/*
  Based on Sketch built by Gustavo Silveira and Dolce Wang
  Modified by Dennis Garvey for MU2300 Final Project
*/



// LIBRARY

#include "MIDIUSB.h"  

// BUTTONS
const int NButtons = 12;
const int buttonPin[NButtons] = {16, 4, 7, 10, 2, 5, 8, 11, 3, 6, 9, 12};
                                            
int buttonCState[NButtons] = {};        
int buttonPState[NButtons] = {};        
      
// debounce
unsigned long lastDebounceTime[NButtons] = {0};  
unsigned long debounceDelay = 50;    


// POTENTIOMETERS
const int NPots = 4; 
const int potPin[NPots] = { A3, A2, A1, A0}; 
int potCState[NPots] = {0}; 
int potPState[NPots] = {0}; 
int potVar = 0; // Difference between the current and previous state of the pot

int midiCState[NPots] = {0}; // Current state of the midi value; delete 0 if 0 pots
int midiPState[NPots] = {0}; // Previous state of the midi value; delete 0 if 0 pots

const int TIMEOUT = 300; //* Amount of time the potentiometer will be read after it exceeds the varThreshold
const int varThreshold = 10; //* Threshold for the potentiometer signal variation
boolean potMoving = true; // If the potentiometer is moving
unsigned long PTime[NPots] = {0}; // Previously stored time; delete 0 if 0 pots
unsigned long timer[NPots] = {0}; // Stores the time that has elapsed since the timer was reset; delete 0 if 0 pots


// MIDI Assignments 
byte midiCh = 1; //* MIDI channel to be used
byte note = 60; //* Lowest note to be used; 36 = C2; 60 = Middle C
byte cc = 1; //* Lowest MIDI CC to be used


// SETUP
void setup() {
  for (int i = 0; i < NButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

}

// LOOP
void loop() {

  buttons();
  potentiometers();

}

// BUTTONS
void buttons() 
{

  for (int i = 0; i < NButtons; i++) 
  {
    buttonCState[i] = digitalRead(buttonPin[i]);  
    if ((millis() - lastDebounceTime[i]) > debounceDelay) 
    {
      if (buttonPState[i] != buttonCState[i]) 
      {
        lastDebounceTime[i] = millis();
        if (buttonCState[i] == LOW) 
        {
          noteOn(midiCh, note + i, 127);
          MidiUSB.flush();
        }
        else 
        {
          noteOn(midiCh, note + i, 0);
          MidiUSB.flush();
        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

// POTENTIOMETERS
void potentiometers() 
{
  for (int i = 0; i < NPots; i++) 
  { 
    potCState[i] = 1023-analogRead(potPin[i]); 
    midiCState[i] = map(potCState[i], 0, 1023, 0, 127); 
    potVar = abs(potCState[i] - potPState[i]);
    if (potVar > varThreshold) 
    { 
      PTime[i] = millis();
    }
    timer[i] = millis() - PTime[i]; 
    if (timer[i] < TIMEOUT) 
    {
      potMoving = true;
    }
    else 
    {
      potMoving = false;
    }
    if (potMoving == true) 
    {
      if (midiPState[i] != midiCState[i]) 
      {
        controlChange(midiCh, cc + i, midiCState[i]);
        MidiUSB.flush();
        potPState[i] = potCState[i]; 
        midiPState[i] = midiCState[i];
      }
    }
  }
}

// Arduino MIDI functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) 
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) 
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) 
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
