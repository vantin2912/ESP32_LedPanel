# 1 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
# 2 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 2
# 3 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 2

# 5 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 2
# 6 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 2
# 7 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 2


BTSerial SerialBT;
# 18 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
uint8_t pairedDeviceBtAddr[20][6];
char bda_str[18];
# 29 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
//Fire up the DMD library as dmd


DMD dmd(2, 3);
hw_timer_t * timer = 
# 33 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 3 4
                    __null
# 33 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
                        ;

bool BT_isConnected = false;

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


  BT_RemovePaired();

  SerialBT.register_callback(BT_Callback);

  ledcSetup(2, 5000, 10);
  ledcAttachPin(22 /* D22 active low Output Enable, setting this low lights all the LEDs in the selected rows. Can pwm it at very high frequency for brightness control.*/, 2);
  DMD_setBrightness(30);

  SerialBT.begin("Led_Panel_v2");

  uint8_t cpuClock = ESP.getCpuFreqMHz();

  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 200, true);

  // timerAlarmEnable(timer);

  dmd.clearScreen( true );

}

void loop()
{
  uint8_t* Rcv = nullptr;
  if(BT_isConnected)
  {
    Rcv = SerialBT.Read_CMD();
  }

  if(Rcv != nullptr)
  {
    dmd.clearScreen( true );
    uint8_t size = sizeof(Rcv);
    for(int i = 0; i < 8; i++)
    {
      char buff[50];
      sprintf(buff, "%.2x ", Rcv[i]);
      // SerialBT.print(buff);
      Serial.print(buff);
      WriteLine(Rcv[i], i);
    }
    Serial.println();
    // SerialBT.println();
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

void BT_Callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if(event == ESP_SPP_SRV_OPEN_EVT)
  {
    BT_isConnected = true;
    timerAlarmEnable(timer);
    Serial.println("Connected");
  }
  if(event == ESP_SPP_CLOSE_EVT)
  {
    BT_isConnected = false;
    dmd.clearScreen(true);
    timerAlarmDisable(timer);
    Serial.println("Disconnected");
  }
}

bool initBluetooth()
{
  if(!btStart()) {
    Serial.println("Failed to initialize controller");
    return false;
  }

  if(esp_bluedroid_init() != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }

  if(esp_bluedroid_enable() != 0 /*!< esp_err_t value indicating success (no error) */) {
    Serial.println("Failed to enable bluedroid");
    return false;
  }
  return true;
}
void BT_Deinit()
{
  btStop();
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
}

char *bda2str(const uint8_t* bda, char *str, size_t size)
{
  if (bda == 
# 167 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 3 4
            __null 
# 167 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
                 || str == 
# 167 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 3 4
                           __null 
# 167 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
                                || size < 18) {
    return 
# 168 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino" 3 4
          __null
# 168 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
              ;
  }
  sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
          bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
  return str;
}

void BT_RemovePaired()
{
  initBluetooth();
  Serial.print("ESP32 bluetooth address: "); Serial.println(bda2str(esp_bt_dev_get_address(), bda_str, 18));
  // Get the numbers of bonded/paired devices in the BT module
  int count = esp_bt_gap_get_bond_device_num();
  if(!count) {
    Serial.println("No bonded device found.");
  } else {
    Serial.print("Bonded device count: "); Serial.println(count);
    if(20 < count) {
      count = 20;
      Serial.print("Reset bonded device count: "); Serial.println(count);
    }
    esp_err_t tError = esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
    if(0 /*!< esp_err_t value indicating success (no error) */ == tError) {
      for(int i = 0; i < count; i++) {
        Serial.print("Found bonded device # "); Serial.print(i); Serial.print(" -> ");
        Serial.println(bda2str(pairedDeviceBtAddr[i], bda_str, 18));
        if(1 /* <- Set to 0 to view all bonded devices addresses, set to 1 to remove*/) {
          esp_err_t tError = esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
          if(0 /*!< esp_err_t value indicating success (no error) */ == tError) {
            Serial.print("Removed bonded device # ");
          } else {
            Serial.print("Failed to remove bonded device # ");
          }
          Serial.println(i);
        }
      }
    }
  }
  BT_Deinit();
}
