#include "synth.h"
#include "gui.h"

#define NOTE_A0 21

void Synth::update()
{
    cpuLoadReset = true;
    
    audio_block_t *block;
    int16_t *bp;
    bool clear = true;
    int i;

    if (this->someWaveOn)
    {
        block = allocate();
        if (block)
        {
            bp = block->data;

            for (i=0; i < NUM_WAVES; ++i)
            {
                if (waveOn[i])
                {
                    if (this->wave[i]->ahdsr_stage == OFF_STAGE)
                    {
                        this->someWaveOn--;
                        this->waveOn[i] = 0;
                    }
                    else
                    {
                        this->wave[i]->update(bp, clear);
                        clear = false;
                    }
                }
            }

            // send the samples to the left channel            
            transmit(block, 0);
            release(block);
        }
    }
}

void Synth::noteOn(byte channel, byte note, byte velocity)
{
    if (note < NOTE_A0)
    {
        return;
    }

    byte nextWave = this->queue[this->qptr];
    if (this->waveOn[nextWave] && this->someWaveOn != NUM_WAVES)
    {        
        byte found = 0;
        byte q = this->qptr;
        for (byte i = 0; i < NUM_WAVES-2; ++i)
        {
            q = (q + 1) % NUM_WAVES;
            if (!found && !this->waveOn[this->queue[q]])
            {
                nextWave = this->queue[q];
                found = 1;
            }
            if (found)
            {
                this->queue[q] = this->queue[(q + 1) % NUM_WAVES];
            }
        }
        if (found)
        {
            this->queue[(this->qptr + NUM_WAVES - 1) % NUM_WAVES] = nextWave;
        }
        else
        {
            nextWave = this->queue[(this->qptr + NUM_WAVES - 1) % NUM_WAVES];
        }        
    }
    else
    {
        this->qptr = (this->qptr + 1) % NUM_WAVES;
    }

    this->wave[nextWave]->reset(this->scaleF[(note - NOTE_A0) % 12] << ((note - NOTE_A0) / 12));
    this->someWaveOn += this->waveOn[nextWave] ? 0 : 1;
    this->waveOn[nextWave] = note;    
}

void Synth::noteOff(byte channel, byte note)
{
    for (int w=0; w < NUM_WAVES; ++w)
    {
        if (this->waveOn[w] == note)
        {
            this->wave[w]->release();
        }
    }
}

void Synth::patchChanged()
{
    wave0.setPatch(&this->patch);
    wave1.setPatch(&this->patch);
    wave2.setPatch(&this->patch);
    wave3.setPatch(&this->patch);
    this->someWaveOn = 0;
    for (uint8_t i=0; i < NUM_WAVES; ++i)
    {
        waveOn[i] = 0;
    }
}

void Synth::execPreset(uint8_t type)
{
    switch (type)
    {
        case PRESET_SINE:
            patch.ot_type = PATCH_OT_BASE;
            patch.ot_phase_type = 0;
            patch.ot_amp_type = PATCH_OT_AMP_1_T;
            break;
        case PRESET_TRI:
            patch.ot_type = PATCH_OT_HARMONIC;
            patch.ot_phase_type = 0b00000110; // odd=-1, altOdd=1, even=0, altEven=0
            patch.ot_amp_type = PATCH_OT_AMP_1_TT;
            break;
        case PRESET_SAW:
            patch.ot_type = PATCH_OT_HARMONIC;
            patch.ot_phase_type = 0b01010101;
            patch.ot_amp_type = PATCH_OT_AMP_1_T;
            break;
        case PRESET_SQUARE:
            patch.ot_type = PATCH_OT_HARMONIC;
            patch.ot_phase_type = 0b00000101;
            patch.ot_amp_type = PATCH_OT_AMP_1_T;
            break;
        case PRESET_CIR:
            patch.ot_type = PATCH_OT_HARMONIC;
            patch.ot_phase_type = 0b00000101;
            patch.ot_amp_type = PATCH_OT_AMP_1_TT;
            break;
        case PRESET_STEPSAW:
            patch.ot_type = PATCH_OT_HARMONIC;
            patch.ot_phase_type = 0b10010101;
            patch.ot_amp_type = PATCH_OT_AMP_1_T;
            break;
    }

    patch.ahdsr_attack_time = 20;
    patch.ahdsr_attack_shape = PATCH_AHDSR_SHAPE_LIN;
    patch.ahdsr_hold_time = 0;
    patch.ahdsr_decay_time = 20;
    patch.ahdsr_decay_shape = PATCH_AHDSR_SHAPE_LIN;
    patch.ahdsr_sustain_level = 1000;
    patch.ahdsr_release_time = 1500;
    patch.ahdsr_release_shape = PATCH_AHDSR_SHAPE_LIN;

    patchChanged();
}

