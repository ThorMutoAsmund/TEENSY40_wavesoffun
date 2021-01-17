#include <arm_math.h>
#include "wave.h"

// If set to 0x2000 the TRI wave will distort when C-D-E-F are pressed
#define MAX_AMP 0x1800LL

void Wave::update(int16_t *bp, bool clear)
{
    // 61290 med AHDSR 61525 med smart mult 61700 uden AHDSR
    int32_t v;
    for (int i = 0; i < audioBlockSamples; i++)
    {
        switch (ahdsr_stage)
        {
            case ATK_STAGE:
                amp = (atk_ampls20 * atk_t) >> 20;
                if (atk_t >= atk_tmax)
                {
                    ahdsr_stage = HLD_STAGE;
                    if (hld_t == hld_tmax)
                    {
                        ahdsr_stage = DEC_STAGE;
                        if (dec_t == dec_tmax)
                        {
                            ahdsr_stage = SUS_STAGE;
                        }
                    }
                }
                break;
            case HLD_STAGE:
                amp = hld_amp;
                if (hld_t >= hld_tmax)
                {
                    ahdsr_stage = DEC_STAGE;
                    if (dec_t == dec_tmax)
                    {
                        ahdsr_stage = SUS_STAGE;
                    }
                }
                break;
            case DEC_STAGE:
                amp = hld_amp - ((dec_ampls20 * dec_t) >> 20);
                if (dec_t >= dec_tmax)
                {
                    ahdsr_stage = SUS_STAGE;
                }
                break;
            case SUS_STAGE:
                amp = sus_amp;
                break;
            case REL_STAGE:
                amp = ((dec_ampls20 * (rel_tmax-rel_t)) >> 20);
                if (rel_t >= rel_tmax)
                {
                    ahdsr_stage = OFF_STAGE;
                }
                break;
        }

        if (ahdsr_stage == OFF_STAGE)
        {
            for (int j = i; j < audioBlockSamples; j++)
            {
                *bp++ = 0;
            }
            break;
        }
        
        v = 0L;
        for (int o=0; o<num_ots; ++o)
        {        
            q31_t a = arm_sin_q31((uint32_t)((tone_phase[o] >> 15) & 0x7fffffff));    
            v += ((a>>16) * tone_amp[o] * amp) >> 10;
            tone_phase[o] += tone_rate[o];
            if (tone_phase[o] & 0x800000000000LL)
            {
                tone_phase[o] &= 0x7fffffffffffLL;
            }
        }
        
        *bp = clear ? (uint16_t)(v >> 15) : *bp + (uint16_t)(v >> 15);
        bp++;

        switch (ahdsr_stage)
        {
            case ATK_STAGE:
                atk_t++;
                break;
            case HLD_STAGE:
                hld_t++;
                break;
            case DEC_STAGE:
                dec_t++;
                break;
            case REL_STAGE:
                rel_t++;
                break;
        }
    }
}

// void Wave::update(int16_t *bp, bool clear)
// {
//     for (int i = 0; i < audioBlockSamples; i++)
//     {
//         uint64_t amp = 1;//atk_ampls10;// * (atk_t+i) / atk_tmax;

//         int32_t v = 0L;
//         for (int o=0; o<=NUM_OTS; ++o)
//         {        
//             q31_t w = arm_sin_q31((uint32_t)((tone_phase[o] >> 15) & 0x7fffffff));    
//             v += (int16_t) (((w>>16) * tone_amp[o] * amp) ); // >> 10
//             tone_phase[o] += tone_rate[o];
//             if (tone_phase[o] & 0x800000000000LL)
//             {
//                 tone_phase[o] &= 0x7fffffffffffLL;
//             }
//         }
        
//         *bp = clear ? (uint16_t)(v >> 15) : *bp + (uint16_t)(v >> 15);
//         bp++;
//     }

//     atk_t += audioBlockSamples;
// }


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
    this->base_freq = base_freq;

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
    atk_t = 0L;
    atk_tmax = patch->ahdsr_attack_time * sampleRate / 1000L;
    atk_ampls20 = (1 << 30) / atk_tmax;

    hld_t = 0L;
    hld_tmax = patch->ahdsr_hold_time * sampleRate / 1000L;
    hld_amp = (1 << 10);

    dec_t = 0L;
    dec_tmax = patch->ahdsr_decay_time * sampleRate / 1000L;

    dec_ampls20 = ((1000-(uint64_t)patch->ahdsr_sustain_level) << 30) / (1000LL * dec_tmax);

    sus_amp = (((uint64_t)patch->ahdsr_sustain_level) << 10) / 1000L;

    rel_t = 0L;
    rel_tmax = patch->ahdsr_release_time * sampleRate / 1000L;

    ahdsr_stage = ATK_STAGE;
}

void Wave::release()
{
    // Fade out from current amp level
    dec_ampls20 = (amp << 20) / rel_tmax;
    ahdsr_stage = REL_STAGE;
}
