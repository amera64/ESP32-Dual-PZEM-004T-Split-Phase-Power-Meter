//  PZEM004t_dual_meter_mqtt_v3
//  Get voltage, Amps, Watts and Wh from two PZEM-004T power meter modules, one for each phase.
//  
//  by Alan Mera  
//  Dec, 27, 2019
//

#include <PZEM004T.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <math.h> 

// Replace the next variables with your SSID/Password combination
const char* ssid = "AMnetgear";
const char* password = "malibu2515";

// Add your MQTT Broker IP address
const char* mqtt_server = "192.168.1.20";
char voltString[8];
char currentString[8];
char wattsString[8];
char energyString[8];
float totalCurrent;
float totalWatts;
float totalEnergy;
char volts;
float amps;
float watts;
float Energy;




//PZEM004T pzem(10, 11);
PZEM004T pzem(&Serial2);
IPAddress ip[2];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  ip[0] = IPAddress(192,168,1,1);
  ip[1] = IPAddress(192,168,1,2);
  //pzem.setAddress(ip[1]); ** Use to set device address on PZEM-004T modules
}

void loop() {
  
  if (!client.connected()) {
      reconnect();
     }
  client.loop();

  //Get volts reading from each device and publish to MQTT
    volts = getVolts(0);
    client.publish("power/volts", voltString);
    
    

  //Get current reading from each device and publish to MQTT
    amps = getCurrent(0);
    totalCurrent = totalCurrent + amps;
    amps = getCurrent(1);
    totalCurrent = totalCurrent + amps;
    client.publish("power/amps", dtostrf(totalCurrent, 2, 2, currentString));
    totalCurrent = 0;
    
  //Get power reading from each device and publish to MQTT
    watts= getPower(0);
    totalWatts = totalWatts + watts;
    watts= getPower(1);
    totalWatts = totalWatts + watts;
    client.publish("power/watts", dtostrf(totalWatts, 2, 2, wattsString));
    totalWatts = 0;

  //Get energy reading from each device and publish to MQTT
    Energy = getEnergy(0);
    totalEnergy = totalEnergy + Energy;
    Energy = getEnergy(1);
    totalEnergy = totalEnergy + Energy;
    client.publish("power/energy", dtostrf(totalEnergy, 2, 2, energyString));
    totalEnergy = 0;
    
    delay(1000);
}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
   // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    
    
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



float getVolts(int deviceNum) {
    float v;
    Serial.print(ip[deviceNum]);
    v = pzem.voltage(ip[deviceNum]);
    if (v < 0.0)
      v = 0.0;
    Serial.print(": ");
    Serial.print(v);
    Serial.print("V; ");
    Serial.println("");
    dtostrf(v, 1, 2, voltString);
    delay(1000);
    return v;  
}

float getCurrent(int deviceNum) {
    float i;
    Serial.print(ip[deviceNum]);
    i = pzem.current(ip[deviceNum]);
    if (i < 0.0)
      i = 0.0;
    Serial.print(": ");
    Serial.print(i);
    Serial.print("A; ");
    Serial.println("");
    delay(1000);
    return i;  
}

float getPower(int deviceNum) {
    float p;
    Serial.print(ip[deviceNum]);
    p = pzem.power(ip[deviceNum]);
    if (p < 0.0)
      p = 0.0;
    Serial.print(": ");
    Serial.print(p);
    Serial.print("W; ");
    Serial.println("");
    delay(1000);
    return p;  
}

float getEnergy(int deviceNum) {
    float e;
    Serial.print(ip[deviceNum]);
    e = pzem.energy(ip[deviceNum]);
    if (e < 0.0)
      e = 0.0;
    Serial.print(": ");
    Serial.print(e);
    Serial.print("Wh; ");
    Serial.println("");
    delay(1000);
    return e;  
}
