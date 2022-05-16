#include <Arduino.h>
#line 1 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
#include "BTSerial.h"
#include <DMD32.h>

BTSerial SerialBT;

#define Height  6
#define Len   8

#define Led_Channel     2
#define Led_Freq        5000
#define Led_Resolution  10

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 3
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
hw_timer_t * timer = NULL;

void DMD_setBrightness(uint8_t percent);

#line 28 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
void setup();
#line 52 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
void loop();
#line 72 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
void Draw_Box(uint8_t PosX, uint8_t PosY);
#line 76 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
void WriteLine(uint8_t val, uint8_t Line);
#line 21 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
void IRAM_ATTR triggerScan()
{
  // Serial.println("Scan");
  dmd.scanDisplayBySPI();
}


void setup()
{

  Serial.begin(115200);

  Serial.println("The device started, now you can pair it with bluetooth!");
  SerialBT.begin("Led_Panel");

  ledcSetup(Led_Channel, Led_Freq, Led_Resolution);
  ledcAttachPin(PIN_DMD_nOE, Led_Channel);
  DMD_setBrightness(30);

  uint8_t cpuClock = ESP.getCpuFreqMHz();

  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);

  timerAlarmEnable(timer);

  dmd.clearScreen( true ); 
  
}

void loop()
{
  uint8_t* Rcv = SerialBT.Read_CMD();
  if(Rcv != nullptr)
  {
    dmd.clearScreen( true ); 
    uint8_t size = sizeof(Rcv);
    for(int i = 0; i < Len; i++)
    {
      char buff[50];
      sprintf(buff, "%.2x ", Rcv[i]);
      Serial.print(buff);
      SerialBT.print(buff);
      WriteLine(Rcv[i], i); 
    }
    Serial.println();
  }

}

void Draw_Box(uint8_t PosX, uint8_t PosY)
{
  dmd.drawFilledBox( PosX + 1, PosY + 1, PosX + Len - 2, PosY + Height - 2, GRAPHICS_NORMAL );
}
void WriteLine(uint8_t val, uint8_t Line)
{
  for(uint8_t i = 0; i< 8; ++i)
  {
    if((val&1) == 1)
    {
      Draw_Box(i * Len, Line* Height);
    }
    val = val >> 1;
  }
}

void DMD_setBrightness(uint8_t percent)
{
  ledcWrite(Led_Channel ,percent * 10);
}
