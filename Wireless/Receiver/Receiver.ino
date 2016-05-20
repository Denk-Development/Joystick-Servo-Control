#include <VirtualWire.h>
#include <SoftwareSerial.h>

// software serial
#define SS_RX_PIN 10
#define SS_TX_PIN 11

#define BPS 8000 // transmission rate (bits per second)

#define SENDER_ID 0x5A14 // secret id (2 byte)

#define NUM_SERVOS 6 // number of servos (total - not only this Arduino)

#define MESSAGE_HEADER 2 // packet header size in bytes
#define MESSAGE_LENGTH (NUM_SERVOS + MESSAGE_HEADER) // packet size in bytes
#define MILLIS_IDLE_BETWEEN_TRANSMISSION 24 // min. time the transmitter waits before sending the next message

#define RX_PIN 12 // wireless module receiver data pin

SoftwareSerial dataLink(SS_RX_PIN, SS_TX_PIN);

uint8_t inputBuffer[MESSAGE_LENGTH]; // full data packet

byte rawData[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming data (can contain parts of a data packet)
byte dataLength = VW_MAX_MESSAGE_LEN; // the size of the data

unsigned long lastDataReceived = 0; // time (millis) of last data reception
byte bytesReceived = 0; // number of bytes of current packet already received

void setup() {
  //Serial.begin(9600);
  dataLink.begin(57600);

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
            //Serial.print(inputBuffer[MESSAGE_HEADER + i]);
            dataLink.write(inputBuffer[MESSAGE_HEADER + i]);
          }

          //Serial.println();
          dataLink.write(0xFF); // 0xFF marks the end of a packet because it's no valid servo state
        }

        flushInputBuffer(); // clear input buffer
      }
    }
  }

  if (bytesReceived && lastDataReceived + MILLIS_IDLE_BETWEEN_TRANSMISSION / 2 < millis()) {
    // a broken packet has been received 
    flushInputBuffer();
  }
}

void flushInputBuffer() {
  for (byte i = 0; i < MESSAGE_LENGTH; i++) {
    inputBuffer[i] = 0;  
  }
  bytesReceived = 0;
}
