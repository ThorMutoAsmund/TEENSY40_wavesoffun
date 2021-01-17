#ifndef PATCH_H
#define PATCH_H

#include <Arduino.h>

#define PATCH_OT_BASE       0
#define PATCH_OT_HARMONIC   1
#define MOD_PATCH_OT        2

//#define PATCH_OT_PHASE 0

#define PATCH_OT_AMP_1_T  0
#define PATCH_OT_AMP_1_TT 1
#define MOD_PATCH_OT_AMP  2

#define PATCH_AHDSR_SHAPE_LIN 0
#define PATCH_AHDSR_SHAPE_EASEIN_CUBIC 1
#define PATCH_AHDSR_SHAPE_EASEIN_QUAD 2
#define PATCH_AHDSR_SHAPE_EASEOUT_CUBIC 3
#define PATCH_AHDSR_SHAPE_EASEOUT_QUAD 4
#define MOD_PATCH_AHDSR_SHAPE_OPTIONS 5

#define NUMTYPE_ENUM 0
#define NUMTYPE_UINT16 1
#define NUMTYPE_FLOAT 2
#define NUMTYPE_PHASE 3


//Teensy 4.1	4284 bytes
//Teensy 4.0	1080 bytes


// Noter
//
// Freq midt mellem 20 hz og 20000 hz er 632 hz.
//
// Sustain level bør være x^2, d.v.s.:
// Sustain på max (midi CC 127) = 0 dB
// Sustain på mid (midi CC 63) = -12 dB
// Sustain på min (midi CC 0) = -inf dB (helt slukket)
// evt. x^3 er også okay
// Så bliver midterniveauet (63) nemlig -18 dB.  Faktisk er alt derimellem okay, f.eks. x^2.5 også fin for den giver -15 dB
//
// Decay/release kunne i teorien også være en variation af 1/x faktisk.
// Men om det så skal være exp, x^4 eller 1/x, det ved jeg faktisk ikke.

// #pragma pack(1)
struct Patch
{
    uint16_t ot_type;
    uint16_t ot_phase_type;
    uint16_t ot_amp_type;
    uint16_t ahdsr_attack_time; // 0ms to 3s  -log - midterpunkt ca 0.3
    uint16_t ahdsr_attack_shape;
    uint16_t ahdsr_hold_time;
    uint16_t ahdsr_decay_time; // 0ms to 6s
    uint16_t ahdsr_decay_shape;
    uint16_t ahdsr_sustain_level; // % or value 0.0 - 1.0
    uint16_t ahdsr_release_time; // 0ms to 6s
    uint16_t ahdsr_release_shape;

    // uint8_t ot_type;
    // uint8_t ot_phase_type;
    // uint8_t ot_amp_type;
    // uint16_t ahdsr_attack_time; // 0ms to 3s  -log - midterpunkt ca 0.3
    // uint8_t ahdsr_attack_shape;
    // uint16_t ahdsr_hold_time;
    // uint16_t ahdsr_decay_time; // 0ms to 6s
    // uint8_t ahdsr_decay_shape;
    // uint16_t ahdsr_sustain_level; // % or value 0.0 - 1.0
    // uint16_t ahdsr_release_time; // 0ms to 6s
    // uint8_t ahdsr_release_shape;
    //23 bytes so far. 23*64 = 1472 bytes
};  

struct PatchValueDef
{
    uint8_t offset;
    uint8_t numType;
    uint16_t min;
    uint16_t max;
    const char **options;
};

#endif
