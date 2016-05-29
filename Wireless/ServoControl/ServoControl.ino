#include <SoftwareSerial.h>
#include <Servo.h>

#define RX_PIN 10
#define TX_PIN 11
#define DATA_LINK_ENABLE_PIN 13 // high active

#define NUM_SERVOS 6 // number of servos (total - not only this Arduino)


SoftwareSerial dataLink(RX_PIN, TX_PIN);

uint8_t inputBuffer[NUM_SERVOS]; // full data packet

// servos attached to this arduino
#define SERVO_1_PIN 9

Servo servo1;

void setup() {
  pinMode(DATA_LINK_ENABLE_PIN, OUTPUT);
  dataLink.begin(57600);
  Serial.begin(57600);

  servo1.attach(SERVO_1_PIN);
}

void loop() {
  digitalWrite(DATA_LINK_ENABLE_PIN, HIGH);
  
  while (dataLink.available() >= NUM_SERVOS + 1) {
    if (dataLink.read() == 0xFF) { // 0xFF marks the end of a packet because it's no valid servo state
      digitalWrite(DATA_LINK_ENABLE_PIN, LOW);
      for (int i = 0; i < NUM_SERVOS; i++) {
        inputBuffer[i] = dataLink.read();
        uint8_t servoAngle = inputBuffer[i];

        switch(i) {
          case 0:
            Serial.print(servoAngle);
            servo1.write(servoAngle);
            break;
        }
      }
      delay(100);
      Serial.write('\n');
    }
  }
}
