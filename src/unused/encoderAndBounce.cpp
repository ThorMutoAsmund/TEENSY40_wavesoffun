/*
#include <Bounce.h>
#include <QuadEncoder.h>

const int encoder1Pin1 = 4;
const int encoder1Pin2 = 5;

Bounce button1 = Bounce(button1Pin, 100);   
Bounce button2 = Bounce(button2Pin, 100); 
QuadEncoder encoder1(1, encoder1Pin1, encoder1Pin2, 1);

void setup(void)
{
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  encoder1.setInitConfig();
  encoder1.EncConfig.filterCount = 5;
  encoder1.EncConfig.filterSamplePeriod = 255;
  encoder1.init();
}

void loop(void)
{
  writeIntro();

  for ( ; ; )
  {
    if (button1.update())
    {
      if (button1.fallingEdge())
      {
        Serial.println("Button 1");
      }
    }
    if (button2.update())
    {
      if (button2.fallingEdge())
      {
        Serial.println("Button 2");
      }
    }
    long newPosition1 = encoder1.read() >> 2;
    if (position1 != newPosition1)
    {
      position1 = newPosition1;
      Serial.print("Position 1 = ");
      Serial.println(position1);
    }
  }
}
*/