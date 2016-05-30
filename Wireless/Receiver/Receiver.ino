#include <VirtualWire.h>

// software serial
#define BPS 8000 // transmission rate (bits per second)

#define SENDER_ID 0x5A14 // secret id (2 byte)

#define NUM_SERVOS 6 // number of servos (total - not only this Arduino)
#define NUM_LOCAL_SERVOS 1 // number of servos attached to the connected servo control Arduino

#define MESSAGE_HEADER 2 // packet header size in bytes
#define MESSAGE_LENGTH (NUM_SERVOS + MESSAGE_HEADER) // packet size in bytes
#define MILLIS_IDLE_BETWEEN_TRANSMISSION 24 // min. time the transmitter waits before sending the next message

#define RX_PIN 12 // wireless module receiver data pin

#define REQUEST_NEXT_PIN 2

uint8_t inputBuffer[MESSAGE_LENGTH]; // full data packet

byte rawData[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming data (can contain parts of a data packet)
byte dataLength = VW_MAX_MESSAGE_LEN; // the size of the data

unsigned long lastDataReceived = 0; // time (millis) of last data reception
byte bytesReceived = 0; // number of bytes of current packet already received

uint8_t servoAngles[NUM_SERVOS];

volatile uint8_t shownServo = 0; // index of the servo which is currently transmitted via parallel output

void setup() {
  // parallel output data (8 bit)
  pinMode(A0, OUTPUT); // LSB
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT); // MSB

  // parallel output address (3 bit)
  pinMode(5, OUTPUT); // LSB
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT); // MSB
  
  Serial.begin(9600);
  
  pinMode(REQUEST_NEXT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REQUEST_NEXT_PIN), nextServo, FALLING);

  vw_set_rx_pin(RX_PIN); // pin
  vw_setup(BPS); // transmission rate
  vw_rx_start(); // start receiver
}

void loop() {
  if (vw_get_message(rawData, &dataLength)) { // data incoming
    lastDataReceived = millis();

    // valid number of bytes received
    if (bytesReceived + dataLength > MESSAGE_LENGTH) {
      flushInputBuffer();
      Serial.println("flushed 1");
    }
    else {
      // append new raw data to input buffer
      for (int i = 0; i < dataLength; i++) {
        inputBuffer[bytesReceived + i] = rawData[i];
      }
      
      bytesReceived += dataLength;
      
      // check whether a full packet has been received already
      if (bytesReceived == MESSAGE_LENGTH) {
        // read sender information (first two byte)
        if (inputBuffer[0] == (uint8_t)(SENDER_ID >> 8) && inputBuffer[1] == (uint8_t)(SENDER_ID & 255)) {
          // packet is for this receiver
          
          // read packet content
          for (int i = 0; i < NUM_SERVOS; i++) {
            servoAngles[i] = inputBuffer[MESSAGE_HEADER + i];
          }
          //Serial.println(inputBuffer[MESSAGE_HEADER + 0]);
          Serial.println("received");
          //Serial.println();
        }

        flushInputBuffer(); // clear input buffer
      }
    }
  }

  if (bytesReceived && lastDataReceived + MILLIS_IDLE_BETWEEN_TRANSMISSION / 2 < millis()) {
    // a broken packet has been received 
    flushInputBuffer();
    Serial.println("flushed");
  }
}

void flushInputBuffer() {
  for (byte i = 0; i < MESSAGE_LENGTH; i++) {
    inputBuffer[i] = 0;  
  }
  bytesReceived = 0;
}


void nextServo() {
  Serial.println("sent");
  if (++shownServo >= NUM_LOCAL_SERVOS) {
    shownServo = 0;
  }
  parallelOut(servoAngles[shownServo]);
  parallelOutServoIndex(shownServo);
}

inline void parallelOut(uint8_t val) {
  PORTC = val & B00111111;
  PORTD &= 0B11110011;
  PORTD |= (val & B01000000) >> 3;
  PORTD |= (val & B10000000) >> 2;
  return;
  digitalWrite(A0, val & 1);
  digitalWrite(A1, val & 1 << 1);
  digitalWrite(A2, val & 1 << 2);
  digitalWrite(A3, val & 1 << 3);
  digitalWrite(A4, val & 1 << 4);
  digitalWrite(A5, val & 1 << 5);
}

inline void parallelOutServoIndex(uint8_t index) {
  PORTD ^= (-(index & 1) ^ PORTD) & (1 << 5);
  PORTD ^= (-(index & 2) ^ PORTD) & (1 << 6);
  PORTD ^= (-(index & 4) ^ PORTD) & (1 << 7);
  return;
  digitalWrite(5, index & B00000001);
  digitalWrite(6, index & B00000010);
  digitalWrite(7, index & B00000100);
}

