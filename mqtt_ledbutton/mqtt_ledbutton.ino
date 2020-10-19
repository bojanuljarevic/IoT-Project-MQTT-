
#include <WiFi.h> // Enables the ESP32 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker


// WiFi
const char* ssid = "mreza";
const char* wifi_password = "12345678";

// MQTT
const char* mqtt_server = "192.168.4.1";
const char* mqtt_topic = "light";
const char* clientID = "ESP32";

//GPIO
// We assigned a name LED pin to pin number 22
const int LEDPIN = 2; 

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void ReceivedMessage(char* topic, byte* payload, unsigned int length) {
  // Output the first character of the message to serial (debug)
  if(payload[0] == '0') {
    digitalWrite(LEDPIN, LOW);
  } else {
    digitalWrite(LEDPIN, HIGH);
  }

}

bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID)) {
      client.subscribe(mqtt_topic);
      return true;
    }
    else {
      return false;
  }
}

void setup() {

  // Begin Serial on 115200
  // Remember to choose the correct Baudrate on the Serial monitor!
  // This is just for debugging purposes
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP32
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // This statement will declare pin 22 as digital output 
  pinMode(LEDPIN, OUTPUT);
  // This statement will declare pin 15 as digital input 

  // setCallback sets the function to be called when a message is received.
  client.setCallback(ReceivedMessage);
  
  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  if (Connect()) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
  
}

void loop() {

  // If the connection is lost, try to connect again
  if (!client.connected()) {
    Connect();
  }
  // client.loop() just tells the MQTT client code to do what it needs to do itself (i.e. check for messages, etc.)
  client.loop();


    delay(200);
}

