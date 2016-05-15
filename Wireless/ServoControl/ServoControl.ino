#include <SoftwareSerial.h>

#define RX_PIN 10
#define TX_PIN 11

#define NUM_SERVOS 6 // number of servos (total - not only this Arduino)

SoftwareSerial dataLink(RX_PIN, TX_PIN);

uint8_t inputBuffer[NUM_SERVOS]; // full data packet

void setup() {
  dataLink.begin(57600);
  Serial.begin(57600);
}

void loop() {
  while (dataLink.available() >= NUM_SERVOS + 1) {
    if (dataLink.read() == '\n') {
      for (int i = 0; i < NUM_SERVOS; i++) {
        inputBuffer[i] = dataLink.read();
            
        // read packet content
        for (int i = 0; i < NUM_SERVOS; i++) {
          Serial.print(inputBuffer[i]);
        }
        
        Serial.write('\n');
      }
    }
  }
}
