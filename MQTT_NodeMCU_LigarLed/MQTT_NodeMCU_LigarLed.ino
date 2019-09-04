#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Dados do WiFi
const char* ssid = "Net Virtua 577";
const char* password = "1000160930";

// Instância das classes para uso do protocolo MQTT
WiFiClient espClient;
PubSubClient client(espClient);
const char* mqtt_server = "broker.hivemq.com";

int led = 0;

//char* topicPub = "";
char* topicSub = "led/onOff";
char* topicSubOnline = "online";

// Pino D7 na NodeMCU
#define LED 13

////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Inicia conexão com o WiFi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println(" ");
    Serial.print("WiFi conectado - IP da NoceMCU: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  estadoLed();
}

////////////////////////////////////////////////////////////////////////////////////////////

// Função chamada ao receber uma mensagem do Subscribe
void callback(char* topic, byte* payload, unsigned int length) {  
  String valor = "";
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    valor += (char)payload[i];
  }
  led = valor.toInt();
}

// Função para restabelecer a conexão MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266ClientUnoesc")) {
      Serial.println("connected");
      client.subscribe(topicSub);
      client.publish(topicSubOnline, "Online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void estadoLed() {  
  if(led == 0)
    digitalWrite(LED, LOW);  
  if(led == 1)
    digitalWrite(LED, HIGH);    
}
