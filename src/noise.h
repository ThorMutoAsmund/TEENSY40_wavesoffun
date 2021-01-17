#ifndef NOISE_H
#define NOISE_H

#include <Arduino.h>
#include <AudioStream.h>

class Noise : public AudioStream
{
public:
	Noise(void) : AudioStream(0, NULL), seed(123456789LL)
    { }
	virtual void update(void);
private:
	uint32_t seed;
    uint32_t a = 1103515245LL;
    uint32_t c = 12345LL;
    uint32_t m = 2 << 30;
	audio_block_t *inputQueueArray[1];
    boolean up = false;
};

#endif
