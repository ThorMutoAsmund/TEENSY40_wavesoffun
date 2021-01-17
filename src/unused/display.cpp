/*
#include <SPI.h>
#include "display.h"
#include "globals.h"

#define max7219_reg_noop        0x00
#define max7219_reg_digit0      0x01
#define max7219_reg_digit1      0x02
#define max7219_reg_digit2      0x03
#define max7219_reg_digit3      0x04
#define max7219_reg_digit4      0x05
#define max7219_reg_digit5      0x06
#define max7219_reg_digit6      0x07
#define max7219_reg_digit7      0x08
#define max7219_reg_decodeMode  0x09
#define max7219_reg_intensity   0x0a
#define max7219_reg_scanLimit   0x0b
#define max7219_reg_shutdown    0x0c
#define max7219_reg_displayTest 0x0f

#define MAX7219_PIN 10

#define DATA D0
#define CLK D1
#define STB D2

SPISettings spiSettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);

const uint8_t ascii[43] = { 
    B1111110, B0110000, B1101101, B1111001, B0110011,  // 0-4
    B1011011, B1011111, B1110000, B1111111, B1111011, // 5-9
    B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000,  // :;<=>?@
    B1110111, B0011111, B1001110, B0111101, B1001111,  // A-E  //
    B1000111, B1011111, B0110111, B0110000, B0111100,  // F-J // alt G B1011110 alt H B0010111  alt I B0000110
    B1010111, B0001110, B1010100, B1110110, B1111110,  // K-O
    B1100111, B1110011, B1100110, B1011011, B0001111,  // P-T
    B0111110, B0111010, B0101010, B0010011, B0111011,  // U-Y  alt X B0110111
    B1101101 }; // Z  // alt Z B1101001

const char* title1 = "  WAVES ";
const char* title2 = "   OF   ";
const char* title3 = "  FUN   ";

const char* labels[5] = {"MODE", "AMNT", "GAIN", "TREB", "BASS" };

void sendCommand(uint8_t command, uint8_t value)
{
    SPI.transfer16((command << 8) | value);
    delayMicroseconds(1);
    digitalWrite(MAX7219_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(MAX7219_PIN, LOW);
    delayMicroseconds(1);
}

void send(uint8_t pos, char letter)
{
    sendCommand(8 - pos,ascii[(letter == ' ' ? ':' : letter) - '0']);
}

void send(uint8_t pos, const char* letter, int count)
{
    for (int p = 0; p < __clamp(count, 0, 8); ++p)
    {
        send(pos + p, *letter++);
    }
}

void setupDisplay()
{
    pinMode(MAX7219_PIN, OUTPUT);
    SPI.begin();
    digitalWrite(MAX7219_PIN, LOW);
    delayMicroseconds(1);

    sendCommand(max7219_reg_scanLimit, 0x07);
    sendCommand(max7219_reg_decodeMode, 0x00);
    sendCommand(max7219_reg_shutdown, 0x01);
    sendCommand(max7219_reg_displayTest, 0x00);
    sendCommand(max7219_reg_intensity, 0x08);

    clear();
}

void clear()
{
    for (uint8_t p = 1; p<=8; ++p)
        sendCommand(p, 0);
}

void writeIntro()
{
    send(0, title1, 8);
    delay(1000);
    send(0, title2, 8);
    delay(1000);
    send(0, title3, 8);
    delay(1000);
}

void writeLabel(uint8_t textId)
{
    send(0, labels[textId], 4);
}

void writeNumber(uint8_t val)
{
    send(4, ' ');
    send(5, val >= 100 ? '1' : '0');
    val %= 100;
    send(6, (val / 10) + '0');
    send(7, (val % 10) + '0');
}

void writeLabelAndNumber(uint8_t textId, uint8_t val)
{
    writeLabel(textId);
    writeNumber(val);
}
*/