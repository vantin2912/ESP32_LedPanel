#line 1 "d:\\Project\\LedPanel\\Led_Panel\\BTSerial.cpp"
#include "BTSerial.h"
#include "string.h"
BTSerial::BTSerial()
{
  memset(Buffer, 0, Rx_Buffer_Size);
  memset(Data, 0, Rx_Buffer_Size);
  Len = 0;
  Index = 0;
}
BTSerial::~BTSerial()
{
  Len = 0;
  Index = 0;
}
uint8_t* BTSerial::Read_CMD()
{
  
  if(this->available()>0)
  {
    // Serial.println("Hello");
    uint8_t Rcv = this->read();
    if(Index == 0)
    {
      if(Rcv == StartByte)
      {
        Buffer[Index] = Rcv;
        Index++;
      }
      memset(Data, 0, Rx_Buffer_Size);
    }
    else if(Index == 1)
    {
      Len = Rcv;
      Buffer[Index] = Rcv;
      Index++;

    }
    else if(Index < Len + 2)
    {
      if(Len <=8)
      {
        Data[Index-2] = Rcv;
        Buffer[Index] = Rcv;
        Index++;
      }
      else
      {
        this->println("Len bigger 8");
        memset(Buffer, 0, Rx_Buffer_Size);
        memset(Data, 0, Rx_Buffer_Size);
        Len = 0;
        Index = 0;
      }

    }
    else if(Index == Len + 2)
    {

      if(Rcv == EndByte)
      {
        // Serial.println("Endbyte");
        Index = 0;
        Len = 0;
        memset(Buffer, 0, Rx_Buffer_Size);
        return Data;
      }
      else
      {
        memset(Buffer, 0, Rx_Buffer_Size);
        memset(Data, 0, Rx_Buffer_Size);
        Len = 0;
        Index = 0;
        this->println("Wrong format");
      }
      
    }
    else
    {
      memset(Buffer, 0, Rx_Buffer_Size);
      memset(Data, 0, Rx_Buffer_Size);
      Len = 0;
      Index = 0;
      this->println("Wrong format");
    }
  }
  return nullptr;
}

