#line 1 "d:\\Project\\LedPanel\\ESP32_LedPanel\\Led_Panel\\BTSerial.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define Rx_Buffer_Size 15
#define StartByte 0xAA
#define EndByte 0xEE

class BTSerial : public BluetoothSerial
{
  private:
    uint8_t Buffer[Rx_Buffer_Size];
    uint8_t Data[Rx_Buffer_Size];
    uint8_t Len;
    uint8_t Index;
  public:
    BTSerial();
    ~BTSerial();
    uint8_t* Read_CMD();
};