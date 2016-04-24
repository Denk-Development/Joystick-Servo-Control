#include <Servo.h>

#define TASTE_1 2
#define TASTE_2 4
#define Taste_3 7
#define DREHGEBER_1 A0
#define DREHGEBER_2 A1
#define DREHGEBER_3 A2

#define SERVO_1_PIN 12
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
  CustomServo(unsigned pin, unsigned analogInputPin, unsigned* activePins, unsigned numActivePins) : Servo() {
    Servo::attach(pin);
    pinMode(pin, OUTPUT);
    pinMode(analogInputPin, INPUT);
    _pin = pin;
    _activePins = activePins;
    _numActivePins = numActivePins;
    _analogInputPin = analogInputPin;
  }

  void update() {
    addNewValue();
    if (!_init && isActive()) { // active
      Servo::write(CustomServo::transferFunction(getAverageOfLastValues()));
    }
    else { // not active 
      // go back to zero position
      Servo::write(90);
    }
  };

private:
  bool _init = true;
  unsigned _pin;
  unsigned _analogInputPin;
  unsigned* _activePins;
  unsigned _numActivePins;
  unsigned _lastValues[AVERAGE_SAMPLES_NUM];
  unsigned _lastValuesPtr = 0;

  // converts the analog value into the servo rotation
  // 
  // @param x = [0;1023]
  // @return y = [0; 180]
  static unsigned transferFunction(unsigned x) { 
    return 6.6822581E-7 * x*x*x - .001026855 * x*x + .5263598 * x;
  };

  bool isActive() {
    for (unsigned i = 0; i < _numActivePins; i++) {
      if (!digitalRead(_activePins[i])) {
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


void setup() {
  // serial connection for debugging
  Serial.begin(9600);
  Serial.println("running...");

  
  // initialize servos

  // servo 1
  unsigned servo1ActivePins[] = { TASTE_1 };
  CustomServo servo1(SERVO_1_PIN, DREHGEBER_1, servo1ActivePins, sizeof(servo1ActivePins) / sizeof(unsigned));

  // servo 2
  unsigned servo2ActivePins[] = { TASTE_1 };
  CustomServo servo2(SERVO_2_PIN, DREHGEBER_2, servo2ActivePins, sizeof(servo2ActivePins) / sizeof(unsigned));

  // servo 3
  unsigned servo3ActivePins[] = { TASTE_1 };
  CustomServo servo3(SERVO_3_PIN, DREHGEBER_3, servo3ActivePins, sizeof(servo3ActivePins) / sizeof(unsigned));

  // servo 4
  unsigned servo4ActivePins[] = { TASTE_1, TASTE_2 };
  CustomServo servo4(SERVO_4_PIN, DREHGEBER_1, servo4ActivePins, sizeof(servo4ActivePins) / sizeof(unsigned));

  // servo 5
  unsigned servo5ActivePins[] = { TASTE_1, TASTE_2 };
  CustomServo servo5(SERVO_5_PIN, DREHGEBER_2, servo5ActivePins, sizeof(servo5ActivePins) / sizeof(unsigned));

  // servo 6
  unsigned servo6ActivePins[] = { TASTE_1, TASTE_2 };
  CustomServo servo6(SERVO_6_PIN, DREHGEBER_3, servo6ActivePins, sizeof(servo6ActivePins) / sizeof(unsigned));

  CustomServo servos[NUM_SERVOS] = { servo1, servo2, servo3, servo4, servo5, servo6 };

  while (true) {
    for (unsigned i = 0; i < NUM_SERVOS; i++) {
      servos[i].update();
    }
  }
}

void loop() {}
