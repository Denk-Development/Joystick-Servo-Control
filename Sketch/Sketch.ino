#include <Servo.h>

#define TASTE_1 2
#define TASTE_2 4
#define TASTE_3 7
#define TASTE_4 8
#define TASTE_7 12

#define DREHGEBER_1 A0
#define DREHGEBER_2 A1
#define DREHGEBER_3 A2

#define SERVO_1_PIN 3
#define SERVO_2_PIN 5
#define SERVO_3_PIN 6
#define SERVO_4_PIN 9
#define SERVO_5_PIN 10
#define SERVO_6_PIN 11

#define NUM_SERVOS 6

#define AVERAGE_SAMPLES_NUM 10

class CustomServo : public Servo
{
public:
  CustomServo(unsigned pin, unsigned analogInputPin, unsigned* highPins, unsigned numHighPins, unsigned* lowPins, unsigned numLowPins, bool inverted = false) : Servo() {
    Servo::attach(pin);
    pinMode(pin, OUTPUT);
    pinMode(analogInputPin, INPUT);
    _pin = pin;
    _highPins = highPins;
    _numHighPins = numHighPins;
    _lowPins = lowPins;
    _numLowPins = numLowPins;
    _analogInputPin = analogInputPin;
    _inverted = inverted;
  }

  void update() {
    addNewValue();
    if (!_init && isActive()) { // active
      Servo::write(CustomServo::transferFunction(getAverageOfLastValues(), _inverted));
    }
    else { // not active 
      // go back to zero position
      Servo::write(90);
    }
  };

private:
  bool _init = true;
  bool _inverted;
  unsigned _pin;
  unsigned _analogInputPin;
  
  unsigned* _highPins;
  unsigned _numHighPins;
  unsigned* _lowPins;
  unsigned _numLowPins;
  
  unsigned _lastValues[AVERAGE_SAMPLES_NUM];
  unsigned _lastValuesPtr = 0;

  // converts the analog value into the servo rotation
  // 
  // @param x = [0;1023]
  // @return y = [0; 180]
  static unsigned transferFunction(unsigned x, bool inverted) { 
    unsigned y = 6.6822581E-7 * x*x*x - .001026855 * x*x + .5263598 * x;
    if (inverted) {
      y = 180 - y;
    }
    return y;
  };

  bool isActive() {
    unsigned i;
    // check high pins
    for (i = 0; i < _numHighPins; i++) {
      if (!digitalRead(_highPins[i])) {
        return false;
      }
    }

    // check low pins
    for (i = 0; i < _numLowPins; i++) {
      if (digitalRead(_lowPins[i])) {
        return false;
      }
    }
    
    return true;
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
};


void softwareReset() {
  asm volatile ("jmp 0"); // reset software
}


void setup() {
  // initialize servos
  // servo 1
  unsigned servo1HighPins[] = { TASTE_1 }, servo1LowPins[] = { TASTE_2 };
  CustomServo servo1(SERVO_1_PIN, DREHGEBER_1, servo1HighPins, sizeof(servo1HighPins) / sizeof(unsigned), servo1LowPins, sizeof(servo1LowPins) / sizeof(unsigned), true);

  // servo 2
  unsigned servo2HighPins[] = { TASTE_1 }, servo2LowPins[] = { TASTE_2 };
  CustomServo servo2(SERVO_2_PIN, DREHGEBER_2, servo2HighPins, sizeof(servo2HighPins) / sizeof(unsigned), servo2LowPins, sizeof(servo2LowPins) / sizeof(unsigned));

  // servo 3
  unsigned servo3HighPins[] = { TASTE_1 }, servo3LowPins[] = { TASTE_2 };
  CustomServo servo3(SERVO_3_PIN, DREHGEBER_3, servo3HighPins, sizeof(servo3HighPins) / sizeof(unsigned), servo3LowPins, sizeof(servo3LowPins) / sizeof(unsigned));

  // servo 4
  unsigned servo4HighPins[] = { TASTE_1, TASTE_2 }, servo4LowPins[] = {};
  CustomServo servo4(SERVO_4_PIN, DREHGEBER_1, servo4HighPins, sizeof(servo4HighPins) / sizeof(unsigned), servo4LowPins, sizeof(servo4LowPins) / sizeof(unsigned));

  // servo 5
  unsigned servo5HighPins[] = { TASTE_1, TASTE_2 }, servo5LowPins[] = {};
  CustomServo servo5(SERVO_5_PIN, DREHGEBER_2, servo5HighPins, sizeof(servo5HighPins) / sizeof(unsigned), servo5LowPins, sizeof(servo5LowPins) / sizeof(unsigned));

  // servo 6
  unsigned servo6HighPins[] = { TASTE_1, TASTE_2 }, servo6LowPins[] = {};
  CustomServo servo6(SERVO_6_PIN, DREHGEBER_3, servo6HighPins, sizeof(servo6HighPins) / sizeof(unsigned), servo6LowPins, sizeof(servo6LowPins) / sizeof(unsigned));

  CustomServo servos[NUM_SERVOS] = { servo1, servo2, servo3, servo4, servo5, servo6 };

  while (true) {
    for (unsigned i = 0; i < NUM_SERVOS; i++) {
      servos[i].update();
    }
    
    if (digitalRead(TASTE_4) && digitalRead(TASTE_7)) {
      softwareReset();
    }
  }
}

void loop() {}
