#include <Arduino.h>
#include <arm_math.h>
#include "noise.h"

void Noise::update(void)
{
    audio_block_t *block;
    short *bp;
    int i;

    //          L E F T  C H A N N E L  O N L Y
    block = allocate();
    if (block)
    {
        bp = block->data;

        for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            this->seed = (this->a * this->seed + this->c) % this->m;
            *bp++ = (short)this->seed;
        }
        this->up = !this->up;
        while (i < AUDIO_BLOCK_SAMPLES)
        {
            *bp++ = 0;
            i++;
        }    

        // send the samples to the left channel
        transmit(block, 0);
        release(block);
    }
}

/*
    // uint32_t tmp  = tone_freq >> 32; 
    // uint64_t tone_tmp = (0x400000000000LL * (int)(tmp&0x7fffffff)) / (int) AUDIO_SAMPLE_RATE_EXACT;
    // // Generate the sweep
    // for(i = 0;i < AUDIO_BLOCK_SAMPLES;i++) {
    //   *bp++ = (short)(( (short)(arm_sin_q31((uint32_t)((tone_phase >> 15)&0x7fffffff))>>16) *tone_amp) >> 15);

    //   tone_phase +=  tone_tmp;
    //   if(tone_phase & 0x800000000000LL)tone_phase &= 0x7fffffffffffLL;

    //   if(tone_sign > 0) {
    //     if(tmp > tone_hi) {
    //       sweep_busy = 0;
    //       break;
    //     }
    //     tone_freq += tone_incr;
    //   } else {
    //     if(tmp < tone_hi || tone_freq < tone_incr) {
    //       sweep_busy = 0;

    //       break;
    //     }
    //     tone_freq -= tone_incr;        
    //   }
    // }

*/
