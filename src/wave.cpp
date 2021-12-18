#include <arm_math.h>
#include "wave.h"
#include "gui.h"

// If set to 0x2000 the TRI wave will distort when C-D-E-F are pressed
#define MAX_AMP 0x1800LL

void Wave::update(int16_t *bp, bool clear)
{
    // 61290 med AHDSR 61525 med smart mult 61700 uden AHDSR
    int32_t shiftedVal;
    int16_t newValue;
    for (uint16_t i = 0; i < audioBlockSamples; i++)
    {
        // recalc:
        switch (ahdsr_stage)
        {
            case ATK_STAGE:
                amp = atk >> 22;
                switch (patch->ahdsr_attack_shape)
                {
                    case PATCH_AHDSR_SHAPE_LIN:
                        atk += atk_d;
                        // atk -= atk_d;
                        break;
                    case PATCH_AHDSR_SHAPE_SQR:
                        atk = atk + 2 * atk_d - ((atk_idx * atk_dd)>>16);
                        atk_idx += 2;
                        break;
                }
                if (atk >= atk_max)
                {
                    amp = max;
                    ahdsr_stage = HLD_STAGE;
                    if (hld >= hld_max)
                    {
                        ahdsr_stage = DEC_STAGE;
                        if (dec <= dec_min)
                        {
                            amp = sus_amp;
                            ahdsr_stage = SUS_STAGE;
                        }
                    }
                }
                break;

            case HLD_STAGE:                
                hld++;
                if (hld >= hld_max)
                {
                    ahdsr_stage = DEC_STAGE;
                    if (dec <= dec_min)
                    {
                        amp = sus_amp;
                        ahdsr_stage = SUS_STAGE;
                    }
                }
                break;

            case DEC_STAGE:
                amp = dec >> 22;
                switch (patch->ahdsr_decay_shape)
                {
                    case PATCH_AHDSR_SHAPE_LIN:
                        dec -= dec_d;
                        break;
                    case PATCH_AHDSR_SHAPE_SQR:
                        dec = dec - 2 * dec_d + ((dec_idx * dec_dd)>>16);
                        dec_idx += 2;
                        break;
                }
                if (dec <= dec_min)
                {
                    amp = sus_amp;
                    ahdsr_stage = SUS_STAGE;
                }
                break;

            case SUS_STAGE:
                break;

            case REL_STAGE:
                amp = rel >> 22;

                switch (patch->ahdsr_release_shape)
                {
                    case PATCH_AHDSR_SHAPE_LIN:
                        rel -= rel_d;
                        break;
                    case PATCH_AHDSR_SHAPE_SQR:
                        rel = rel - 2 * rel_d + ((rel_idx * rel_dd)>>16);
                        rel_idx += 2;
                        break;
                }
                    
                if (rel & 0x8000000000000000)
                {
                    amp = 0L;
                    ahdsr_stage = OFF_STAGE;
                }
                break;
        }

        if (ahdsr_stage == OFF_STAGE)
        {
            if (clear)
            {
                for (int j = i; j < audioBlockSamples; j++)
                {
                    *bp++ = 0;
                }
            }
            break;
        }
        
        shiftedVal = 0;
        for (int o = 0; o < num_ots; ++o)
        {        
            q31_t a = arm_sin_q31((uint32_t)((tone_phase[o] >> 15) & 0x7fffffff));    
            shiftedVal += ((a>>16) * tone_amp[o] * amp) >> 10;
            tone_phase[o] += tone_rate[o];
            if (tone_phase[o] & 0x800000000000LL)
            {
                tone_phase[o] &= 0x7fffffffffffLL;
            }
        }

        newValue = (int16_t)(shiftedVal >> 15);

        // if (next_base_freq)
        // {
        //     if  (lastValue < 0 && newValue >= 0)
        //     {
        //         actualReset(false);
        //         next_base_freq = 0;
        //         goto recalc;
        //     }
        // }

        *bp = clear ? newValue : *bp + newValue;
        lastValue = newValue;       

        bp++;
    }
    
}

void Wave::actualReset(bool releaseOnly)
{
    uint32_t freq_div;
    uint32_t final_level;

    if (!releaseOnly)
    {
        base_freq = next_base_freq;

        uint8_t o = 0;
        uint64_t tone_freq = 0LL;

        // OT
        while (o <= NUM_OTS)
        {
            switch (this->patch->ot_type)
            {
                case PATCH_OT_BASE:
                case PATCH_OT_HARMONIC:
                    tone_freq = ((uint64_t)base_freq*tone_ot[o]);
                    break;
                break;
            }
            tone_rate[o] = (tone_freq<<14) / (int) sampleRate;
            tone_phase[o] = 0LL;

            o++;
            
            if (this->patch->ot_type == PATCH_OT_BASE || !this->patch->ot_phase_type)
            {
                while (o <= NUM_OTS)
                {
                    tone_rate[o] = 0LL;
                    tone_phase[o] = 0LL;
                    o++;
                }
            }
        }

        // AHDSR
        max = 1L << 10;
        atk_idx = 1L;
        dec_idx = 1L;
        rel_idx = 1L;

        // ATK
        atk = 0LL;
        atk_max = 1LL<<32;
        freq_div = patch->ahdsr_attack_time * sampleRate / 1000L;
        atk_d = atk_max/freq_div;
        atk_dd = (atk_d*atk_d)>>16;

        // HLD
        hld = 0L;
        hld_max = patch->ahdsr_hold_time * sampleRate / 1000L;

        // DEC
        final_level = (((1000LL-patch->ahdsr_sustain_level)<<24)/1000);// == 0.8 << 24 for 20% sustainlevel
        freq_div = patch->ahdsr_decay_time * sampleRate / 1000L;  // == 44100 for 1 sec decay

        dec = atk_max;                                       // == 1.0 << 32
        dec_d = (((uint64_t)final_level)<<8)/freq_div;                           // == 0.8/44100 << 32
        dec_dd = (dec_d*dec_d)>>16;
        dec_min = ((((uint64_t)patch->ahdsr_sustain_level)<<32)/1000);        

        // SUS
        sus_amp = (((uint64_t)patch->ahdsr_sustain_level) << 10) / 1000L;

        ahdsr_stage = ATK_STAGE;
    }
    else
    {
        // Fade out from current amp level
        freq_div = patch->ahdsr_release_time * sampleRate / 1000L;
        rel = ((uint64_t)amp)<<22;
        rel_d = rel/freq_div;
        rel_dd = (rel_d*rel_d)>>16;
        ahdsr_stage = REL_STAGE;
    }
}


void Wave::setPatch(Patch *patch)
{
    this->ahdsr_stage = OFF_STAGE;
    this->patch = patch;
    
    uint16_t ot=1;
    num_ots = 0;
    uint8_t sign = 0;
    
    while (num_ots <= NUM_OTS)
    {
        if (ot > 1)
        {
            if ((ot-2)%4 == 0 && this->patch->ot_phase_type & 0b00110000)
            {
                sign = (this->patch->ot_phase_type & 0b00110000)>>4;
            }
            else if ((ot-2)%4 == 1 && this->patch->ot_phase_type & 0b00000011)
            {
                sign = this->patch->ot_phase_type & 0b00000011;
            }
            else if ((ot-2)%4 == 2 && this->patch->ot_phase_type & 0b11000000)
            {
                sign = (this->patch->ot_phase_type & 0b11000000)>>6;
            }
            else if ((ot-2)%4 == 3 && this->patch->ot_phase_type & 0b00001100)
            {
                sign = (this->patch->ot_phase_type & 0b00001100)>>2;
            }
            else
            {
                ot++;
                continue;
            }
        }
        
        tone_ot[num_ots] = ot;

        // 0x8000LL amounts to a sine of amp 1
        switch (this->patch->ot_amp_type)
        {
            case PATCH_OT_AMP_1_T:
                tone_amp[num_ots] = (MAX_AMP * 0x10000LL / ot) >> 16;
                break;
            case PATCH_OT_AMP_1_TT:
                tone_amp[num_ots] = (MAX_AMP * 0x10000LL / ot / ot) >> 16;
                break;
        }
        if (sign == 0b10)
        {
            tone_amp[num_ots] = -tone_amp[num_ots];
        }

        num_ots++;
        ot++;
        
        if (this->patch->ot_type == PATCH_OT_BASE || !this->patch->ot_phase_type)
        {
            break;
        }
    }
}

void Wave::reset(uint64_t base_freq)
{
    next_base_freq = base_freq;
    actualReset(false);
}
