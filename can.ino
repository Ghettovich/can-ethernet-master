#include <SPI.h>
#include <mcp_can.h>

const int SPI_CS_PIN = 23;
const int CAN_INT_PIN = 20;
const int LED        = 8;
boolean ledON        = 1;

MCP_CAN CAN(SPI_CS_PIN);

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];


void setupCAN() {
  pinMode(LED, OUTPUT);

  while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");

  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), MCP2515_ISR, FALLING); // start interrupt
}

void MCP2515_ISR() {
  flagRecv = 1;
}

void canLoop() {

  if (flagRecv) {
    // check if get data

    flagRecv = 0;                   // clear flag

    // iterate over all pending messages
    // If either the bus is saturated or the MCU is busy,
    // both RX buffers may be in use and reading a single
    // message does not clear the IRQ conditon.
    while (CAN_MSGAVAIL == CAN.checkReceive()) {
      // read data,  len: data length, buf: data buf
      CAN.readMsgBuf(&len, buf);

      // print the data
      for (int i = 0; i < len; i++) {
        Serial.print(buf[i]);
        Serial.print("\t");
      }
      Serial.println();
    }
  }
}
