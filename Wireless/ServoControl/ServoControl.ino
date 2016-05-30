#include <Servo.h>

// servos attached to this arduino
#define SERVO_1_PIN 9

#define REQUEST_NEXT_PIN 2

Servo servo1;

void setup() {
  Serial.begin(57600);

  servo1.attach(SERVO_1_PIN);

  // parallel input data (8 bit)
  pinMode(A0, INPUT); // LSB
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT); // MSB  

  // parallel input address (3 bit)
  pinMode(5, INPUT); // LSB
  pinMode(6, INPUT);
  pinMode(7, INPUT); // MSB

  pinMode(REQUEST_NEXT_PIN, OUTPUT);
  digitalWrite(REQUEST_NEXT_PIN, HIGH);
}

void loop() {
  uint8_t angle = requestNext();
  //Serial.println(angle);
  switch(readIndex()) {
    case 0:
      servo1.write(angle);
      Serial.println(angle);
      break;
  }
  //Serial.println(readIndex());
}

uint8_t requestNext() {
  digitalWrite(REQUEST_NEXT_PIN, LOW);
  digitalWrite(REQUEST_NEXT_PIN, HIGH);
  delay(100); // give time for parallel output
  return readAngle();
}

uint8_t readAngle() {
  uint8_t angle = 0;
  angle |= digitalRead(A0);
  angle |= digitalRead(A1) << 1;
  angle |= digitalRead(A2) << 2;
  angle |= digitalRead(A3) << 3;
  angle |= digitalRead(A4) << 4;
  angle |= digitalRead(A5) << 5;
  angle |= digitalRead(3) << 6;
  angle |= digitalRead(4) << 7;
  return angle;
}

uint8_t readIndex() {
  uint8_t index = 0;
  index |= digitalRead(5);
  index |= digitalRead(6) << 1;
  index |= digitalRead(7) << 2;
  return index;
}

