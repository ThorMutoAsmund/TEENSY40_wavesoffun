#include "tm1638.h"
#include "globals.h"

#define CMD_WRITE 0x40
#define CMD_READ 0x42
#define CMD_ADDRESS 0xC0
#define CMD_BRIGHTNESS 0x80

#ifdef DEBUG
#define LONG_PRESS_DELAY 5
#else
#define LONG_PRESS_DELAY 10
#endif

#define SW1 32
#define SW2 2
#define SW3 32
#define SW4 2
#define SW5 32
#define SW6 2
#define SW7 32
#define SW8 2
#define SW9 64
#define SW10 4
#define SW11 64
#define SW12 4
#define SW13 64
#define SW14 4
#define SW15 64
#define SW16 4
#define SWB1 0
#define SWB2 0
#define SWB3 1
#define SWB4 1
#define SWB5 2
#define SWB6 2
#define SWB7 3
#define SWB8 3
#define SWB9 0
#define SWB10 0
#define SWB11 1
#define SWB12 1
#define SWB13 2
#define SWB14 2
#define SWB15 3
#define SWB16 3

#define DATA 16
#define CLK  17
#define STB  18

TM1638 tm1638;

uint32_t lastState = 0L;
uint32_t btnState = 0L;
void (*buttonClick)(bool longPress);
void (*blink)(void);
void (*fastBlink)(void);

uint8_t blinkCnt = 0;
uint8_t longPressCnt = 0;

void sendByte(uint8_t cmd);
uint8_t receiveByte();

void buttonInterrupt() 
{    
    digitalWrite(STB, LOW);
    sendByte(CMD_READ);

    pinMode(DATA, INPUT);

    btnState = 0L;
    for (uint8_t i = 0; i < 4; ++i)
    {
        btnState <<= 8;
        btnState |= receiveByte();
    }

    digitalWrite(STB, HIGH);  
    pinMode(DATA, OUTPUT);

    if (lastState != btnState)
    {
        lastState = btnState;
        if (buttonClick)
        {
            buttonClick(false);
        }
        // Setting longPressCnt to resets and enables the longPress counter. Setting it to 0 disables it
        longPressCnt = btnState ? 1 : 0;
    }
    else
    {
        if (btnState && longPressCnt)
        {
            longPressCnt++;
            if (longPressCnt == LONG_PRESS_DELAY)
            {
                longPressCnt = 0;
                if (buttonClick)
                {
                    buttonClick(true);
                }
            }
        }
    }

    blinkCnt = (blinkCnt+1)%5;
    if (!blinkCnt && blink)
    {
        blink();
    }
    if (fastBlink)
    {
        fastBlink();
    }
}

void TM1638::setup()
{
    pinMode(DATA, OUTPUT);
    pinMode(CLK, OUTPUT);
    pinMode(STB, OUTPUT);
    digitalWrite(CLK, HIGH);
    digitalWrite(STB, HIGH);
    delayMicroseconds(1);

    myTimer.begin(buttonInterrupt, 100000);
}

void TM1638::onButtonClick(void (*fn)(bool longPress))
{
    buttonClick = fn;
}

void TM1638::onBlink(void (*fn)(void))
{
    blink = fn;
}

void TM1638::onFastBlink(void (*fn)(void))
{
    fastBlink = fn;
}

void TM1638::toggle(uint8_t pos)
{
    toggled[pos] = !toggled[pos];
    setDisplay(seg, 8);
}

void TM1638::clearToggle(uint8_t pos)
{
    toggled[pos] = false;
}

void TM1638::write(const char symbol, uint8_t offset)
{
    uint8_t idx = symbol - ' ';
    if (idx < NUM_ASCII)
    {
        seg[offset] = ascii[idx];
    }
}

void TM1638::write(const char *txt, uint8_t offset)
{
    uint8_t len = strlen(txt);
    for (uint8_t i=0; i<len; ++i)
    {
        uint8_t idx = txt[i] - ' ';
        bool dec = false;
        if (idx & 0x80)
        {
            idx &= 0x7F;
            dec = true;
        }
        if (idx < NUM_ASCII)
        {
            seg[offset+i] = ascii[idx];
            if (dec)
            {
                seg[offset+i] |= 0x80;
            }
        }
    }
}

void TM1638::writeln(const char *txt, uint8_t offset)
{
    write(txt, offset);
    setDisplay(seg, 8);
}

void TM1638::clearDisplay()
{
    digitalWrite(STB, LOW);
    sendByte(CMD_WRITE);
    digitalWrite(STB, HIGH);
    delayMicroseconds(1);

    digitalWrite(STB, LOW);
    sendByte(CMD_ADDRESS);

    for (uint8_t i = 0; i < 16; ++i)
    {
        sendByte(0);
    }
  
    digitalWrite(STB, HIGH);
    delayMicroseconds(1);
}

void TM1638::setBrightness(uint8_t brg)
{
    digitalWrite(STB, LOW);
    sendByte(CMD_BRIGHTNESS | 0x08 | brg);
    digitalWrite(STB, HIGH);
    delayMicroseconds(1);
}

void TM1638::setDisplay(uint8_t seg[], uint8_t n)
{
    digitalWrite(STB, LOW);
    sendByte(CMD_WRITE);
    digitalWrite(STB, HIGH);
    delayMicroseconds(1);

    digitalWrite(STB, LOW);
    sendByte(CMD_ADDRESS);

    uint8_t b = 1;
    uint8_t v = 0;
    uint8_t sg;
    for (uint8_t a = 0; a < 8; ++a)
    {
        for (uint8_t i = 0; i < n; ++i)
        {
            sg = toggled[i] ? 0 : seg[i];
            v <<= 1;
            v |= (sg & b) ? 1 : 0;
        }
        sendByte(v);
        v = 0;
        sendByte(v);
        b <<= 1;
    }
    
    digitalWrite(STB, HIGH);
    delayMicroseconds(1);
}

bool TM1638::isPressed(uint8_t b)
{
    uint8_t mask = 2<<(b/8 + ( ((b+1) % 2) <<2)); 
    uint8_t idx = (b>>1)%4;
    return (btnState >> ((3-idx)*8)) & mask;  
}

uint8_t TM1638::pressedButton()
{
    uint8_t btn = 6;
    uint8_t val = 0;
    //32 2 64 4
    if (btnState & 0xff000000)
    {
        btn = 0;
        val = (btnState & 0xff000000) >> 24;
    }
    else if (btnState & 0xff0000)
    {
        btn = 2;
        val = (btnState & 0xff0000) >> 16;
    }
    else if (btnState & 0xff00)
    {
        btn = 4;
        val = (btnState & 0xff00) >> 8;
    }
    else if (btnState & 0xff)
    {
        val = (btnState & 0xff);
    }
    else
    {
        return 0xff;
    }
    
    btn |= (val & 0x06 ? 1 : 0) | (val & 0x44 ? 8 : 0);
    
    return btn;
}

void sendByte(uint8_t cmd)
{
    for (uint8_t i = 0; i < 8; ++i)
    {
        digitalWrite(DATA, cmd&1);
        digitalWrite(CLK, LOW);
        delayMicroseconds(1);
        digitalWrite(CLK, HIGH);
        delayMicroseconds(1);
        cmd >>= 1;
    }
}

uint8_t receiveByte()
{
    uint8_t b;
    uint8_t v = 0;
    for (uint8_t i = 0; i < 8; ++i)
    {
        v <<= 1;
        digitalWrite(CLK, LOW);
        delayMicroseconds(1);
        b = digitalRead(DATA);
        digitalWrite(CLK, HIGH);
        delayMicroseconds(1);
        v |= b;
    }

    return v;
}
