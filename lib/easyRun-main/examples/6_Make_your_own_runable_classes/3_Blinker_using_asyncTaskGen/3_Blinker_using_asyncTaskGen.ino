#include "easyRun.h"

class blinker : public asyncTaskGen
{
  private :
    int _pin, _state;

  public :
    blinker(int pin, long delay_ms)
    {
      _pin = pin;
      _state = LOW;
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, _state);
      set(&blinker::high, delay_ms);
    }

  protected :
    void println(const char *msg)
    {
      Serial.print(millis());
      Serial.print(", pin");
      Serial.print(_pin);
      Serial.print(' ');
      Serial.println(msg);
    }

    void high()
    {
      println("HIGH");
      _state = HIGH;
      digitalWrite(_pin, _state);
      set(&blinker::low);
    }

    void low()
    {
      println("LOW");
      _state = LOW;
      digitalWrite(_pin, _state);
      set(&blinker::high);
    }
};

//Using a class allows to handle several blinkers 
blinker a(3, 2000);
blinker b(LED_BUILTIN, 1000);

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  easyRun();
}

