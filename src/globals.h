#ifndef GLOBALS_H
#define GLOBALS_H

#define ADC_MAX 4095.f
#define ADC_PEAK 2048.f

#define M_PI_2	1.57079632679489661923
#define M_PI    3.14159265358979323846
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __clamp(a,min,max) (__min(__max(min, a), max))

// extern uint32_t input;
// extern uint32_t output;

extern bool cpuLoadReset;
extern uint32_t lastCPULoadCount;

#endif