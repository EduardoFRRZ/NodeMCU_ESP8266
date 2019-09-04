#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Dados do WiFi
const char* ssid = "Net Virtua 577";
const char* password = "1000160930";
 
void setup() {
  Serial.begin(115200);
  
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
} 
