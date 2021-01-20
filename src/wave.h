#ifndef WAVE_H
#define WAVE_H

#include <Arduino.h>
#include "patch.h"

#define NUM_OTS 36

#define ATK_STAGE 0
#define HLD_STAGE 1
#define DEC_STAGE 2
#define SUS_STAGE 3
#define REL_STAGE 4
#define OFF_STAGE 5

class Wave
{
public:
	Wave(uint16_t audioBlockSamples, int sampleRate) : audioBlockSamples(audioBlockSamples), sampleRate(sampleRate), base_freq(440LL << 32)
    { 		
	}
	void update(int16_t *bp, bool clear);
	void setPatch(Patch *patch);
	void reset(uint64_t base_freq);
	void release();
	uint8_t ahdsr_stage = OFF_STAGE;
private:
	Patch *patch;
	uint16_t audioBlockSamples;
	int sampleRate;
	uint8_t num_ots = 0;
	uint64_t base_freq;
	uint16_t tone_ot[NUM_OTS+1];
	uint64_t tone_phase[NUM_OTS+1];
	uint64_t tone_amp[NUM_OTS+1];
	uint64_t tone_rate[NUM_OTS+1];	
	uint64_t amp = 0;
	uint64_t next_base_freq = 0;
	int16_t lastValue;
	uint32_t atk_t;
	uint32_t atk_tmax;
	uint64_t atk_ampls20;
	uint32_t hld_t;
	uint32_t hld_tmax;
	uint64_t hld_amp;
	uint32_t dec_t;
	uint32_t dec_tmax;
	uint64_t dec_ampls20;
	uint64_t sus_amp;
	uint32_t rel_t;
	uint32_t rel_tmax;
	void actualReset();
};

#endif
