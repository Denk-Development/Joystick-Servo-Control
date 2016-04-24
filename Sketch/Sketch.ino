#include <Servo.h>

#define TASTE_1 2
#define TASTE_2 3
#define Taste_3 4
#define DREHGEBER_1 A0
#define DREHGEBER_2 A1
#define DREHGEBER_3 A2

#define NUM_SERVOS 6

#define AVERAGE_SAMPLES_NUM 10

class CustomServo : public Servo
{
public:
  CustomServo(unsigned pin, unsigned analogInputPin, unsigned activePins[]) : Servo() {
    Servo::attach(pin);
    _pin = pin;
    _activePins = activePins;
    _analogInputPin = analogInputPin;
  }

  void update() {
    addNewValue();
    if (!_init) {
      Servo::write(CustomServo::transferFunction(getAverageOfLastValues()));
    }
  };

private:
  // @param x = [0;1023]
  // @return y = [0; 180]
  static unsigned transferFunction(unsigned x) { 
    return map(x, 0, 1023, 0, 180);
  };
  
  void addNewValue() {
    _lastValues[_lastValuesPtr] = analogRead(_analogInputPin);
    _lastValuesPtr++;

    // overflow
    if (_lastValuesPtr >= AVERAGE_SAMPLES_NUM) {
      _lastValuesPtr = 0;
      _init = false;
    }
  };

  float getAverageOfLastValues() {
    float sum = 0;
    for (unsigned i = 0; i < AVERAGE_SAMPLES_NUM; i++) {
      sum += _lastValues[i];
    }
    return sum / (float)AVERAGE_SAMPLES_NUM;
  };

  bool _init = true;
  unsigned _pin;
  unsigned _analogInputPin;
  unsigned* _activePins;
  unsigned _lastValues[AVERAGE_SAMPLES_NUM];
  unsigned _lastValuesPtr = 0;
};


void setup() {
  // initialize servos

  // servo 1
  unsigned servo1ActivePins[] = { TASTE_1 };
  CustomServo servo1(5, DREHGEBER_1, servo1ActivePins);

  // servo 2
  unsigned servo2ActivePins[] = { TASTE_1 };
  CustomServo servo2(6, DREHGEBER_2, servo2ActivePins);

  // servo 3
  unsigned servo3ActivePins[] = { TASTE_1 };
  CustomServo servo3(7, DREHGEBER_3, servo3ActivePins);

  // servo 4
  unsigned servo4ActivePins[] = { TASTE_1, TASTE_2 };
  CustomServo servo4(8, DREHGEBER_1, servo4ActivePins);

  // servo 5
  unsigned servo5ActivePins[] = { TASTE_1, TASTE_2 };
  CustomServo servo5(9, DREHGEBER_2, servo5ActivePins);

  // servo 6
  unsigned servo6ActivePins[] = { TASTE_1, TASTE_2 };
  CustomServo servo6(10, DREHGEBER_3, servo6ActivePins);

  CustomServo servos[NUM_SERVOS] = { servo1, servo2, servo3, servo4, servo5, servo6 };

  while (true) {
    for (unsigned i = 0; i < NUM_SERVOS; i++) {
      servos[i].update();
    }
  }
}

void loop() {}
