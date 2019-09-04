/*
  Implementação de um timer que irá atívar a saida de um pino, através de uma ativação
  vindo remotamente pelo protocolo MQTT
  Desenvolvido por:
  Willian
  Eduardo
  Cassiano
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// instancia das classes para uso do Wifi e do protocolo MQTT
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "Net Virtua 577";
const char* password = "1000160930";
const char* mqtt_server = "broker.hivemq.com";

int tempo_saida_ativa = 0;
char msg[50];
long lastMsg = 0;
long lastMsgTopic = 0;

char* topicPub = "led/status";
char* topicSub = "led/timer";
char* topicSubOnline = "online";

bool avisou_ativado;
bool avisou_desativado;

// Saida do PIN
#define LED 13 // pino D7 na NodeMCU

// Conectar ao wifi e ao Broker
void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// função que realiza a conexão wifi
void setup_wifi() {
  delay(10);
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

// Função chamada ao receber uma mensagem do Subscribe
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String tempo = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    tempo += (char)payload[i];
  }
  Serial.println();

  tempo_saida_ativa = tempo.toInt() * 1000;
  avisou_ativado = avisou_desativado = true;
}

// Função para restabelecer a conexão
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

void loop() {

  if (!client.connected())
    reconnect();

  client.loop();
  timer();
}

// Função ativada pelo subscribe para realização do timer
void timer() {
  long now = millis();
  int interval = 100;
  int intervalTopic = 1000;

  if (now - lastMsg > interval && tempo_saida_ativa != 0) {
    lastMsg = now;
    subtrairTempo(interval);
    ativarSaida();
    Serial.println(tempo_saida_ativa);
  }

  if (now - lastMsgTopic > intervalTopic && tempo_saida_ativa != 0) {
    lastMsgTopic = now;
    publicarSaidaAtivada();
  }

  if (tempo_saida_ativa == 0) {
    destivarSaida();
    publicarSaidaDesativada();
  }

}

void ativarSaida() {
  digitalWrite(LED, HIGH);
  Serial.println("Saída ativada");
}

void destivarSaida() {
  digitalWrite(LED, LOW);
  if (avisou_desativado)
    Serial.println("Saída desativada");
}

// enviar informação via mqtt sobre quantos segundos faltam
void publicarSaidaAtivada() {
  snprintf (msg, 75, "Faltam %ld s", tempo_saida_ativa);
  client.publish(topicPub, msg);
}

// enviar informação via mqtt sobre o término do timer
void publicarSaidaDesativada() {
  if (avisou_desativado) {
    client.publish(topicPub, "Saída Desativada");
    avisou_desativado = false;
  }
}

int subtrairTempo(int interval) {
  tempo_saida_ativa -= interval;
  if (tempo_saida_ativa < 0)
    tempo_saida_ativa = 0;
}
