
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("[Master CAN+Ethernet]");

  setupCAN();
  Serial.println("CAN Ready.");
  
  delay(100);
  
  setupMqttClient();
  Serial.println("ETHERNET Ready.");  
}

void loop() {
  // put your main code here, to run repeatedly:
  canLoop();
  mqttLoop();
}
