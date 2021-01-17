/*
  Pins:		Teensy 4.0

  LRCLK:	Pin 20
  BCLK:		Pin 21
  DIN:		Pin 7
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <SPI.h>

#ifdef USB_MIDI
#include <MIDIUSB.h>
#endif

#include "globals.h"
#include "synth.h"
#include "noise.h"
#include "wave.h"
#include "gui.h"
#include "tm1638.h"


Synth synth;
// Noise noise;
AudioAmplifier amp1;
AudioOutputI2S i2s1;
AudioConnection patchCord1 (synth, amp1);
AudioConnection	patchCord2 (amp1, 0, i2s1, 0);

bool cpuLoadReset = false;
uint32_t lastCPULoadCount = 0L;
uint32_t cpuLoadCount = 0L;

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    gui.flashLed();
    synth.noteOn(inChannel, inNote, inVelocity);
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
    gui.flashLed();
    synth.noteOff(inChannel, inNote);
}

void setup(void)
{
    // Wait for at least 3 seconds for the USB serial connection
    Serial.begin(9600);
    while (!Serial && millis () < 3000) ;

    pinMode(LED_BUILTIN, OUTPUT);

    // Midi
#ifdef USB_MIDI
    usbMIDI.begin();
    usbMIDI.setHandleNoteOn(handleNoteOn);
    usbMIDI.setHandleNoteOff(handleNoteOff);
#endif
    Serial.println("Midi ready!");

    // Setup QYF-TM1638
    gui.init(&synth);

    AudioMemory (2);
    amp1.gain (0.5);

    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop(void)
{
    // const uint32_t maxCPUTime = ((uint32_t)F_CPU)*AUDIO_BLOCK_SAMPLES/(uint32_t)AUDIO_SAMPLE_RATE_EXACT;
    gui.reset();
    for (;;)
    {
        if (cpuLoadReset)
        {            
            lastCPULoadCount = cpuLoadCount;
            cpuLoadCount = 0L;
            cpuLoadReset = false;
        }        
        cpuLoadCount+=2;
#ifdef USB_MIDI
       do
       {
       }
       while (usbMIDI.read());
#endif
    }
}
