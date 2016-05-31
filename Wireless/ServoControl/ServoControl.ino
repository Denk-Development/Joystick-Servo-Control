#include <SoftwareSerial.h>
#include <Servo.h>

#define RX_PIN 10
#define TX_PIN 11
#define DATA_LINK_ENABLE_PIN 13 // high active

#define NUM_SERVOS 6 // number of servos (total - not only this Arduino)


SoftwareSerial dataLink(RX_PIN, TX_PIN);
bool dataLinkRunning = true;

uint8_t inputBuffer[NUM_SERVOS + 1]; // full data packet
int inputPtr = 0;

// servos attached to this arduino
#define SERVO_1_PIN 9

Servo servo1;

void setup() {
  pinMode(DATA_LINK_ENABLE_PIN, OUTPUT);
  digitalWrite(DATA_LINK_ENABLE_PIN, HIGH);
  
  dataLink.begin(57600);
  Serial.begin(57600);

  servo1.attach(SERVO_1_PIN);
}

void loop() {
  while (dataLink.available()) {
    if (inputPtr > NUM_SERVOS) {
      inputPtr = 0;
    }
    uint8_t lastChar = dataLink.read();
    inputBuffer[inputPtr++] = lastChar;
    if (lastChar == 0xFF) { // 0xFF marks the end of a packet because it's no valid servo state
      Serial.println(inputPtr);
      if (inputPtr == NUM_SERVOS + 1) {
        for (int i = 0; i < NUM_SERVOS; i++) {
          uint8_t servoAngle = (uint8_t)inputBuffer[i];
          Serial.print(servoAngle);
  
          switch(i) {
            case 0:
              servoWrite(servo1, servoAngle);
              break;
          }
        }
      }
      Serial.write('\n');
      inputPtr = 0;
    }
  }
}

void servoWrite(Servo servo, byte angle) {
  if (servo.read() != angle) {
    servo.write(angle);
  }
}

