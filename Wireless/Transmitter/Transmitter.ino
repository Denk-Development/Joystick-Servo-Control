#include <VirtualWire.h> // wireless module library

#define DEBUG

#define TASTE_1 2
#define TASTE_2 4
#define TASTE_3 7
#define TASTE_4 8
#define TASTE_7 13

#define DREHGEBER_1 A0
#define DREHGEBER_2 A1
#define DREHGEBER_3 A2

#define TRANSMITTER_MODULE_DATA_PIN 12

#define NUM_SERVOS 6 // number of servos

#define AVERAGE_SAMPLES_NUM 10


// transmission
#define SENDER_ID 0x5A14 // secret id (2 byte)

#define BPS 8000 // transmission rate (bits per second)

#define MESSAGE_HEADER 2 // packet header size in bytes
#define MESSAGE_LENGTH (NUM_SERVOS + MESSAGE_HEADER) // packet size in bytes

#define MILLIS_IDLE_BETWEEN_TRANSMISSION 80 // recommended approx. MESSAGE_LENGTH * 8 / BPS * 1,000 * 3


#define TASTE_1_HIGH_OUTPUT_DELAY 1000 // 1 second
#define TASTE_1_HIGH_OUTPUT_PIN 13

unsigned long millisLastTaste1 = 0;
bool taste1PushedPreviously = false;

/** Virutal Servo **/

class VirtualServo
{
public:
  VirtualServo(unsigned analogInputPin, unsigned* highPins, unsigned numHighPins, unsigned* lowPins, unsigned numLowPins, bool inverted = false) {
    pinMode(analogInputPin, INPUT);
    _highPins = highPins;
    _numHighPins = numHighPins;
    _lowPins = lowPins;
    _numLowPins = numLowPins;
    _analogInputPin = analogInputPin;
  }

  void update() {
    addNewValue();
    if (!_init && isActive()) { // active
      _state = VirtualServo::transferFunction(getAverageOfLastValues(), _inverted);
    }
    else { // not active 
      // go back to zero position
      _state = 90;
    }
  };

  uint8_t read() {
    return _state;
  }

private:
  uint8_t _state = 90;
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


uint8_t outputBuffer[MESSAGE_LENGTH]; // output buffer for transmitted data


void softwareReset() {
  asm volatile ("jmp 0"); // reset software
}

void broadcastServoStates(VirtualServo servos[]) {
  // fill output buffer with data
  outputBuffer[0] = (uint8_t)(SENDER_ID >> 8); // transmitter secret high byte
  outputBuffer[1] = (uint8_t)(SENDER_ID & 255); // transmitter secret low byte
  for (unsigned i = 0; i < NUM_SERVOS; i++) {
    outputBuffer[i + MESSAGE_HEADER] = (uint8_t)servos[i].read();
    #ifdef DEBUG
      Serial.print(outputBuffer[MESSAGE_HEADER + i]);
    #endif
  }
  
  #ifdef DEBUG
    Serial.println();
  #endif
  
  // transmitt data
  vw_send(outputBuffer, MESSAGE_LENGTH);
  vw_wait_tx(); // wait until the whole message is gone
  delay(MILLIS_IDLE_BETWEEN_TRANSMISSION); // wait some more time
}


void setup() {
  pinMode(TASTE_1, INPUT);
  pinMode(TASTE_2, INPUT);
  pinMode(TASTE_3, INPUT);
  pinMode(TASTE_4, INPUT);
  pinMode(TASTE_7, INPUT);
  pinMode(TASTE_1_HIGH_OUTPUT_PIN, OUTPUT);
  
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  
  // initialize transmitter
  vw_set_tx_pin(TRANSMITTER_MODULE_DATA_PIN); // transmitter module data pin
  vw_setup(BPS); // transmission rate

  
  // initialize servos
  // servo 1
  unsigned servo1HighPins[] = { TASTE_1 }, servo1LowPins[] = { TASTE_2 };
  VirtualServo servo1(DREHGEBER_1, servo1HighPins, sizeof(servo1HighPins) / sizeof(unsigned), servo1LowPins, sizeof(servo1LowPins) / sizeof(unsigned), true);

  // servo 2
  unsigned servo2HighPins[] = { TASTE_1 }, servo2LowPins[] = { TASTE_2 };
  VirtualServo servo2(DREHGEBER_2, servo2HighPins, sizeof(servo2HighPins) / sizeof(unsigned), servo2LowPins, sizeof(servo2LowPins) / sizeof(unsigned));

  // servo 3
  unsigned servo3HighPins[] = { TASTE_1 }, servo3LowPins[] = { TASTE_2 };
  VirtualServo servo3(DREHGEBER_3, servo3HighPins, sizeof(servo3HighPins) / sizeof(unsigned), servo3LowPins, sizeof(servo3LowPins) / sizeof(unsigned));

  // servo 4
  unsigned servo4HighPins[] = { TASTE_1, TASTE_2 }, servo4LowPins[] = {};
  VirtualServo servo4(DREHGEBER_1, servo4HighPins, sizeof(servo4HighPins) / sizeof(unsigned), servo4LowPins, sizeof(servo4LowPins) / sizeof(unsigned));

  // servo 5
  unsigned servo5HighPins[] = { TASTE_1, TASTE_2 }, servo5LowPins[] = {};
  VirtualServo servo5(DREHGEBER_2, servo5HighPins, sizeof(servo5HighPins) / sizeof(unsigned), servo5LowPins, sizeof(servo5LowPins) / sizeof(unsigned));

  // servo 6
  unsigned servo6HighPins[] = { TASTE_1, TASTE_2 }, servo6LowPins[] = {};
  VirtualServo servo6(DREHGEBER_3, servo6HighPins, sizeof(servo6HighPins) / sizeof(unsigned), servo6LowPins, sizeof(servo6LowPins) / sizeof(unsigned));

  VirtualServo servos[NUM_SERVOS] = { servo1, servo2, servo3, servo4, servo5, servo6 };


  // inifinite loop
  while (true) {
    #ifdef DEBUG
      Serial.println(digitalRead(TASTE_1));
    #endif
    
    // update each servo (read analog pins, etc.)
    for (unsigned i = 0; i < NUM_SERVOS; i++) {
      servos[i].update();
    }

    // check for reset combination
    if (digitalRead(TASTE_4) && digitalRead(TASTE_7)) {
      softwareReset();
    }

    // transmit servo states
    broadcastServoStates(servos);

    
    if (digitalRead(TASTE_1)) {
      taste1PushedPreviously = true;
      millisLastTaste1 = millis();
    }
    digitalWrite(TASTE_1_HIGH_OUTPUT_PIN, (taste1PushedPreviously && millisLastTaste1 + TASTE_1_HIGH_OUTPUT_DELAY >= millis()) ? HIGH : LOW);
  }
}

void loop() {}
