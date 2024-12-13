#include <WiFi.h> 
#include <WiFiClientSecure.h> 
#include <PubSubClient.h> 
#include <DHTesp.h> 

char SSIDName[] = "Wokwi-GUEST"; 
char SSIDPass[] = "";

const int DHT_PIN = 15; 

char BrokerURL[] = "964e80a2593d41cd80bf0518aaeb7473.s1.eu.hivemq.cloud"; 
char BrokerUserName[] = "teste"; 
char BrokerPassword[] = "Teste123456"; 
char MQTTClientName[] = "DHT22-ESTUFA"; 
int BrokerPort = 8883; 

char Topico_01[] = "temperature_humidity"; 

WiFiClientSecure espClient;
PubSubClient clienteMQTT(espClient);
DHTesp dhtSensor; 


void mqttReconnect() {
  while (!clienteMQTT.connected()) {
    Serial.println("Conectando-se ao broker MQTT...");
    Serial.println(MQTTClientName);
    if (clienteMQTT.connect(MQTTClientName, BrokerUserName, BrokerPassword)) {
      Serial.print(MQTTClientName);
      Serial.println(" conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(clienteMQTT.state());
      Serial.println(" Tente novamente em 5 segundos.");
      delay(5000);
    }
  }
}

// Setup
void setup() {
  Serial.begin(9600);

  Serial.print("Conectando-se ao Wi-Fi");
  WiFi.begin(SSIDName, SSIDPass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  espClient.setInsecure(); 
  clienteMQTT.setServer(BrokerURL, BrokerPort);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22); 
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() {
  if (!clienteMQTT.connected()) {
    mqttReconnect(); 
  }
  clienteMQTT.loop();

  TempAndHumidity temp_umid = dhtSensor.getTempAndHumidity(); 
  float temperatura = temp_umid.temperature;
  float umidade = temp_umid.humidity;

  // Criar o JSON com temperatura e umidade
  String jsonPayload = "{\"t\":\"" + String(temperatura, 1) + "\",\"u\":\"" + String(umidade, 1) + "\"}";

  clienteMQTT.publish(Topico_01, jsonPayload.c_str());
  Serial.print("Dados publicados: ");
  Serial.println(jsonPayload);

  delay(1000);
}
