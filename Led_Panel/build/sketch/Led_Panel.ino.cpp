#include <Arduino.h>
#line 1 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
#include "BTSerial.h"
#include <DMD32.h>

#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include"esp_gap_bt_api.h"
#include "esp_err.h"

BTSerial SerialBT;

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define REMOVE_BONDED_DEVICES 1   // <- Set to 0 to view all bonded devices addresses, set to 1 to remove

#define PAIR_MAX_DEVICES 20
uint8_t pairedDeviceBtAddr[PAIR_MAX_DEVICES][6];
char bda_str[18];


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

bool BT_isConnected = false;

void DMD_setBrightness(uint8_t percent);

#line 46 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void setup();
#line 76 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void loop();
#line 102 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void Draw_Box(uint8_t PosX, uint8_t PosY);
#line 106 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void WriteLine(uint8_t val, uint8_t Line);
#line 123 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void BT_Callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
#line 140 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
bool initBluetooth();
#line 158 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void BT_Deinit();
#line 165 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
char * bda2str(const uint8_t* bda, char *str, size_t size);
#line 175 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void BT_RemovePaired();
#line 39 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\Led_Panel.ino"
void IRAM_ATTR triggerScan()
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
  
  ledcSetup(Led_Channel, Led_Freq, Led_Resolution);
  ledcAttachPin(PIN_DMD_nOE, Led_Channel);
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
    for(int i = 0; i < Len; i++)
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
 
  if(esp_bluedroid_init() != ESP_OK) {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }
 
  if(esp_bluedroid_enable() != ESP_OK) {
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
  if (bda == NULL || str == NULL || size < 18) {
    return NULL;
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
    if(PAIR_MAX_DEVICES < count) {
      count = PAIR_MAX_DEVICES; 
      Serial.print("Reset bonded device count: "); Serial.println(count);
    }
    esp_err_t tError =  esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
    if(ESP_OK == tError) {
      for(int i = 0; i < count; i++) {
        Serial.print("Found bonded device # "); Serial.print(i); Serial.print(" -> ");
        Serial.println(bda2str(pairedDeviceBtAddr[i], bda_str, 18));     
        if(REMOVE_BONDED_DEVICES) {
          esp_err_t tError = esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
          if(ESP_OK == tError) {
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
