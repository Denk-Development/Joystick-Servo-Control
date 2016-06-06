#include <Servo.h>

const unsigned int servoCount = 3;
const uint8_t servoPins[servoCount] = { 2, 3, 4 }; // pinning information
Servo servos[servoCount];

const int servoAngles[] = { 0, 60, 60, 40, -40, -60, -60, 60, 60, 20 };

void setup() {
  Serial.begin(9600);
  Serial.println("init...");

  initServos(); // attach to pins
  
  Serial.println("running...");
}

void loop() {
  byte servoState = (millis() / 1000) % 10;
  
  int angle = servoAngles[servoState] + 90;
  writeAllServos(angle);

  Serial.println(millis() / 1000 / 10); // counter
}

void initServos() {
  for (unsigned int i = 0; i < servoCount; i++) {
    servos[i].attach(servoPins[i]);
    Serial.println("attached Servo " + String(i) + " to pin " + String(servoPins[i]));
  }
}

void writeAllServos(int angle) {
  for (unsigned int i = 0; i < servoCount; i++) {
    if (servos[i].read() == angle) continue; // access servo only if necessary (angle changed)
    servos[i].write(angle);
  }
}

