#ifndef SYNTH_H
#define SYNTH_H

#include <Arduino.h>
#include <AudioStream.h>
#include "wave.h"
#include "patch.h"
#include "globals.h"

#define NUM_WAVES 4

#define PRESET_SINE 0
#define PRESET_TRI 1
#define PRESET_SAW 2
#define PRESET_SQUARE 3
#define PRESET_CIR 4
#define PRESET_STEPSAW 5

class Synth : public AudioStream
{
public:
	Synth(void) : AudioStream(0, NULL), 
        wave0(AUDIO_BLOCK_SAMPLES, (int) AUDIO_SAMPLE_RATE_EXACT),
        wave1(AUDIO_BLOCK_SAMPLES, (int) AUDIO_SAMPLE_RATE_EXACT),
        wave2(AUDIO_BLOCK_SAMPLES, (int) AUDIO_SAMPLE_RATE_EXACT),
        wave3(AUDIO_BLOCK_SAMPLES, (int) AUDIO_SAMPLE_RATE_EXACT)
    { 	
        execPreset(PRESET_SINE);
        // patch.ot_type = PATCH_OT_BASE;
        // patch.ot_phase_type = 0;
        // patch.ot_amp_type = PATCH_OT_AMP_1_T;
        // patch.ahdsr_attack_time = 50;
        // patch.ahdsr_attack_shape = PATCH_AHDSR_SHAPE_LIN;
        // patch.ahdsr_hold_time = 0;
        // patch.ahdsr_decay_time = 100;
        // patch.ahdsr_decay_shape = PATCH_AHDSR_SHAPE_LIN;
        // patch.ahdsr_sustain_level = 1000;
        // patch.ahdsr_release_time = 2000;
        // patch.ahdsr_release_shape = PATCH_AHDSR_SHAPE_LIN;
        // patchChanged();
	}
    Patch patch;
	void noteOn(byte channel, byte note, byte velocity);
    void noteOff(byte channel, byte note);
    void execPreset(uint8_t type);
    void patchChanged();
    virtual void update(void);
private:
    audio_block_t *inputQueueArray[1];
    Wave wave0;
    Wave wave1;
    Wave wave2;
    Wave wave3;
    Wave* wave[NUM_WAVES] = { &wave0, &wave1, &wave2, &wave3 };
    byte waveOn[NUM_WAVES] = { 0, 0, 0, 0 };
    uint64_t scaleF[12] = { 118111600633LL, 125134881887LL, 132575789176LL, 140459155838LL, 148811291875LL, 157660071765LL, 167035027489LL, 176967447090LL, 187490479095LL, 198639243145LL, 210450947204LL, 222965011735LL };
    byte someWaveOn = 0;
    byte next = 0;
};

#endif
