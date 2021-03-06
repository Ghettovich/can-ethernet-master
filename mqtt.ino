#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <EthernetENC.h>
#include <EthernetClient.h>
//#include <Dns.h>
//#include <Dhcp.h>

const long pingInterval = 30000;
unsigned long delayStart = 0;
bool flagPublishProximity = false, delayPingStart = false;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

#define SERVER   "192.168.178.251"
#define PORT     1883

IPAddress iotIP (192, 168, 178, 21);
//Uncomment the following, and set to your preference if you don't have automatic dns.
IPAddress dnsIP (192, 168, 178, 1);
//If you uncommented either of the above lines, make sure to change "Ethernet.begin(mac)" to "Ethernet.begin(mac, iotIP)" or "Ethernet.begin(mac, iotIP, dnsIP)"

const char proximityLiftTopic[] = "/proximity/lift";
const char relayStatesTopic[] = "/relay/states";
const char toggleRelaySub[] = "/toggle/relay";

const int ethernetCS = 22;

//Set up the ethernet client
EthernetClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER, PORT);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }

// Setup a feed for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish proximityPublish = Adafruit_MQTT_Publish(&mqtt, proximityLiftTopic, MQTT_QOS_1, 1);
Adafruit_MQTT_Publish toggleRelayPublish = Adafruit_MQTT_Publish(&mqtt, relayStatesTopic, MQTT_QOS_1, 1);

// Setup a feed for subscribing to changes.
Adafruit_MQTT_Subscribe toggleRelaySubcription = Adafruit_MQTT_Subscribe(&mqtt, toggleRelaySub, MQTT_QOS_1);

void setupMqttClient() {
  // Initialise the Client
  Ethernet.init(ethernetCS);
  
  Serial.print(F("\nInit the Client..."));
  Ethernet.begin(mac, iotIP, dnsIP);
  delay(1000); //give the ethernet a second to initialize

  //delayStart = millis();
  delayPingStart = true;

  toggleRelaySubcription.setCallback(toggleRelayCallback);

  mqtt.subscribe(&toggleRelaySubcription);
}

uint32_t x = 0;

void mqttLoop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(1000);

  if (delayPingStart &&
      (millis() - delayStart) >= pingInterval) {
    // ping the server to keep the mqtt connection alive
    if (! mqtt.ping()) {
      mqtt.disconnect();
    }

    delayStart = millis();
    delayPingStart = true;
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}


void toggleRelayCallback(char *data, uint16_t len) {
  Serial.println("hoi");
  Serial.println(data);
}
