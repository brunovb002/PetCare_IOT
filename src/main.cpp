#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const char* MQTT_HOST     = "0c79b338958a441080e6b89fdb8a8015.s1.eu.hivemq.cloud";
const int   MQTT_PORT     = 8883;
const char* MQTT_USER     = "petvital";
const char* MQTT_PASSWORD = "PetVital123";
const char* CLIENT_ID     = "petvital-esp32";

const char* TOPIC_TEMP_CORPORAL  = "petvital/temperatura_corporal";
const char* TOPIC_FREQ_CARDIACA  = "petvital/frequencia_cardiaca";
const char* TOPIC_HIDRATACAO     = "petvital/hidratacao";
const char* TOPIC_ATIVIDADE      = "petvital/atividade";
const char* TOPIC_TEMP_AMBIENTE  = "petvital/temperatura_ambiente";
const char* TOPIC_UMID_AMBIENTE  = "petvital/umidade_ambiente";
const char* TOPIC_COMEU          = "petvital/comeu";
const char* TOPIC_BEBEU_AGUA     = "petvital/bebeu_agua";
const char* TOPIC_VASILHA_COMIDA = "petvital/vasilha_comida";
const char* TOPIC_VASILHA_AGUA   = "petvital/vasilha_agua";

#define DHT_CORPORAL_PIN        15
#define DHT_AMBIENTE_PIN         4
#define DHT_TYPE              DHT22
#define POT_FREQ_PIN            34
#define POT_HIDRAT_PIN          35
#define POT_ATIVID_PIN          32
#define POT_VASILHA_COMIDA_PIN  36
#define POT_VASILHA_AGUA_PIN    39
#define BTN_COMEU_PIN           18
#define BTN_BEBEU_PIN           19

DHT dhtCorporal(DHT_CORPORAL_PIN, DHT_TYPE);
DHT dhtAmbiente(DHT_AMBIENTE_PIN, DHT_TYPE);
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

const long INTERVALO_MS = 5000;
unsigned long ultimoEnvio = 0;
unsigned long ultimoBtnComeu = 0;
unsigned long ultimoBtnBebeu = 0;
const long DEBOUNCE_MS = 500;

void conectarWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void conectarMQTT() {
  espClient.setInsecure();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setKeepAlive(60);
  while (!mqttClient.connected()) {
    if (!mqttClient.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) delay(3000);
  }
}

float mapFloat(int valor, float outMin, float outMax) {
  return outMin + ((float)valor / 4095.0f) * (outMax - outMin);
}

void publicar(const char* topico, float valor, int casas = 1) {
  char buffer[16];
  dtostrf(valor, 4, casas, buffer);
  mqttClient.publish(topico, buffer);
}

void setup() {
  Serial.begin(115200);
  dhtCorporal.begin();
  dhtAmbiente.begin();
  pinMode(POT_FREQ_PIN,           INPUT);
  pinMode(POT_HIDRAT_PIN,         INPUT);
  pinMode(POT_ATIVID_PIN,         INPUT);
  pinMode(POT_VASILHA_COMIDA_PIN, INPUT);
  pinMode(POT_VASILHA_AGUA_PIN,   INPUT);
  pinMode(BTN_COMEU_PIN, INPUT_PULLUP);
  pinMode(BTN_BEBEU_PIN, INPUT_PULLUP);
  conectarWifi();
  conectarMQTT();
}

void loop() {
  if (!mqttClient.connected()) conectarMQTT();
  mqttClient.loop();

  if (digitalRead(BTN_COMEU_PIN) == LOW) {
    unsigned long agora = millis();
    if (agora - ultimoBtnComeu > DEBOUNCE_MS) {
      ultimoBtnComeu = agora;
      mqttClient.publish(TOPIC_COMEU, "1");
    }
  }

  if (digitalRead(BTN_BEBEU_PIN) == LOW) {
    unsigned long agora = millis();
    if (agora - ultimoBtnBebeu > DEBOUNCE_MS) {
      ultimoBtnBebeu = agora;
      mqttClient.publish(TOPIC_BEBEU_AGUA, "1");
    }
  }

  unsigned long agora = millis();
  if (agora - ultimoEnvio >= INTERVALO_MS) {
    ultimoEnvio = agora;

    float tempCorporal = dhtCorporal.readTemperature();
    if (!isnan(tempCorporal)) publicar(TOPIC_TEMP_CORPORAL, tempCorporal);

    float tempAmbiente = dhtAmbiente.readTemperature();
    float umidAmbiente = dhtAmbiente.readHumidity();
    if (!isnan(tempAmbiente)) publicar(TOPIC_TEMP_AMBIENTE, tempAmbiente);
    if (!isnan(umidAmbiente)) publicar(TOPIC_UMID_AMBIENTE, umidAmbiente);

    publicar(TOPIC_FREQ_CARDIACA, mapFloat(analogRead(POT_FREQ_PIN),   60.0f, 180.0f), 0);
    publicar(TOPIC_HIDRATACAO,    mapFloat(analogRead(POT_HIDRAT_PIN),  0.0f, 100.0f), 1);
    publicar(TOPIC_ATIVIDADE,     mapFloat(analogRead(POT_ATIVID_PIN),  0.0f, 100.0f), 1);
    publicar(TOPIC_VASILHA_COMIDA, mapFloat(analogRead(POT_VASILHA_COMIDA_PIN), 0.0f, 100.0f), 1);
    publicar(TOPIC_VASILHA_AGUA,   mapFloat(analogRead(POT_VASILHA_AGUA_PIN),   0.0f, 100.0f), 1);
  }
}
