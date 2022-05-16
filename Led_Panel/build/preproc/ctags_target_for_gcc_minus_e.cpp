# 1 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
# 2 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino" 2
# 3 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino" 2

BTSerial SerialBT;
# 13 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
//Fire up the DMD library as dmd


DMD dmd(2, 3);
hw_timer_t * timer = 
# 17 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino" 3 4
                    __null
# 17 "d:\\Project\\LedPanel\\Led_Panel\\Led_Panel.ino"
                        ;

void DMD_setBrightness(uint8_t percent);

void __attribute__((section(".iram1" "." "27"))) triggerScan()
{
  // Serial.println("Scan");
  dmd.scanDisplayBySPI();
}


void setup()
{

  Serial.begin(115200);

  Serial.println("The device started, now you can pair it with bluetooth!");
  SerialBT.begin("Led_Panel");

  ledcSetup(2, 5000, 10);
  ledcAttachPin(22 /* D22 active low Output Enable, setting this low lights all the LEDs in the selected rows. Can pwm it at very high frequency for brightness control.*/, 2);
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
    for(int i = 0; i < 8; i++)
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
  dmd.drawFilledBox( PosX + 1, PosY + 1, PosX + 8 - 2, PosY + 6 - 2, 0 );
}
void WriteLine(uint8_t val, uint8_t Line)
{
  for(uint8_t i = 0; i< 8; ++i)
  {
    if((val&1) == 1)
    {
      Draw_Box(i * 8, Line* 6);
    }
    val = val >> 1;
  }
}

void DMD_setBrightness(uint8_t percent)
{
  ledcWrite(2 ,percent * 10);
}
