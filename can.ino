#include <SPI.h>
#include <mcp_can.h>

const int SPI_CS_PIN = 23;
const int CAN_INT_PIN = 20;

MCP_CAN CAN(SPI_CS_PIN);

canMsgBuffer* canBuffer;
const int CAN_MSG_SIZE = 3;
volatile bool flagRecv = false;
unsigned char len = 0;
unsigned char buf[8];
char str[20];


void setupCAN() {
  while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");

  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), MCP2515_ISR, FALLING); // start interrupt
  
  // init can buffer message
  canBuffer = canBufferInit(CAN_MSG_SIZE);
}

void MCP2515_ISR() {
  flagRecv = true;
}

void canLoop() {

  if (flagRecv) {
    // check if get data

    flagRecv = false;                   // clear flag

    // iterate over all pending messages
    // If either the bus is saturated or the MCU is busy,
    // both RX buffers may be in use and reading a single
    // message does not clear the IRQ conditon.
    while (CAN_MSGAVAIL == CAN.checkReceive()) {
      
      // read data,  len: data length, buf: data buf
      CAN.readMsgBuf(&len, canBuffer->data);
      unsigned long canId = CAN.getCanId();


      unsigned short value = getValueFromBuffer(canBuffer, len);
      Serial.println(value);
    }
  }
}
