#ifndef TM1638_H
#define TM1638_H

#include <Arduino.h>

#define LBL_MODE 0
#define LBL_AMOUNT 1
#define LBL_GAIN 2
#define LBL_TREBLE 3
#define LBL_BASS 4

#define NUM_ASCII 65

class TM1638
{
public:
	TM1638()
    {
	}
    void setup();
    void clearDisplay();
    void setBrightness(uint8_t brg);
    void write(const char *txt, uint8_t offset);
    void writeln(const char *txt, uint8_t offset);
    void write(const char symbol, uint8_t offset);
    bool isPressed(uint8_t b);
    uint8_t pressedButton();
    void onButtonClick(void (*fn)(bool longPress));
    void onBlink(void (*fn)(void));
    void onFastBlink(void (*fn)(void));
    void toggle(uint8_t pos);
    void clearToggle(uint8_t pos);
private:
    void setDisplay(uint8_t seg[], uint8_t n);
    uint8_t receiveByte();
    IntervalTimer myTimer;    
    
    const uint8_t ascii[NUM_ASCII] = { 
        B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0111001, B0001111, B0000000, B0000000, B0000000, // s!"#$%&'()*+,
        B1000000, B0000000, B0000000, // -./
        B0111111, B0000110, B1011011, B1001111, B1100110,  // 0-4
        B1101101, B1111101, B0000111, B1111111, B1101111, // 5-9
        B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000,  // :;<=>?@
        B1110111, B1111100, B0111001, B1011110, B1111001,  // A-E  //
        B1110001, B0111101, B1110110, B0000110, B0011110,  // F-J // alt G B1011110 alt H B0010111  alt I B0000110
        B1110101, B0111000, B0010101, B0110111, B0111111,  // K-O
        B1110011, B1100111, B0110011, B1101101, B1111000,  // P-T
        B0111110, B0111110, B0101010, B1100100, B1101110,  // U-Y  alt X B0110111  alt V B0101110
        B1011011, // Z alt Z B1101001
        B0000000, B0000000, B0000000, B0000000, B0001000, B0000000 // [\]^_`xyz{|}~⌂Çü((<210/))
        }; 
    uint8_t digits[10] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };
    uint8_t seg[8] = { 0,0,0,0,0,0,0,0 };
    bool toggled[8] = { false,false,false,false,false,false,false,false };
};

extern TM1638 tm1638;


// void writeLabel(uint8_t textId);
// void writeNumber(uint8_t val);
// void writeLabelAndNumber(uint8_t textId, uint8_t val);


#endif