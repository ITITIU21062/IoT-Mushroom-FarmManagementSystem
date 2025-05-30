#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ==== WiFi Info ====
const char* ssid = "Redmi K50";        // Wi-Fi của bạn
const char* password = "khoaaaaa";

// ==== MQTT Broker Info ====
const char* mqtt_server = "192.168.219.210"; // IP của Raspberry Pi
WiFiClient espClient;
PubSubClient client(espClient);

// ==== BH1750 Setup ====
#define SDA_PIN 14
#define SCL_PIN 15
BH1750 lightMeter;

// ==== DHT22 Setup ====
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ==== Sensor IDs (lấy từ DB sensors) ====
#define SENSOR_ID_TEMP     26
#define SENSOR_ID_HUMI     27
#define SENSOR_ID_HEAT     28
#define SENSOR_ID_LUX      25

// ==== Biến đọc dữ liệu ====
float temperature, humidity, heatIndexC;
float lux = -1;

// ==== MQTT reconnect ====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Kết nối MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("✓ kết nối thành công!");
      client.publish("esp32/status", "ESP32 đã online");
    } else {
      Serial.print("Thất bại. Lỗi = ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ==== DHT Init ====
  dht.begin();
  Serial.println("Khởi động DHT22...");

  // ==== BH1750 Init ====
  Wire.begin(SDA_PIN, SCL_PIN);
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 đã sẵn sàng.");
  } else {
    Serial.println("Lỗi BH1750. Kiểm tra dây nối!");
    while (1);
  }

  // ==== WiFi ====
  WiFi.begin(ssid, password);
  Serial.print("Kết nối Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWi-Fi kết nối thành công!");
  Serial.println(WiFi.localIP());

  // ==== MQTT ====
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // ==== Đọc cảm biến ====
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  heatIndexC = dht.computeHeatIndex(temperature, humidity, false);
  lux = lightMeter.readLightLevel();

  // ==== Gửi DHT22 ====
  if (!isnan(humidity) && !isnan(temperature)) {
    Serial.print("Độ ẩm: "); Serial.print(humidity); Serial.print(" %\t");
    Serial.print("Nhiệt độ: "); Serial.print(temperature); Serial.print(" °C\t");
    Serial.print("Chỉ số nhiệt: "); Serial.print(heatIndexC); Serial.println(" °C");

    char jsonTemp[100];
    sprintf(jsonTemp, "{\"sensor_id\": %d, \"value\": %.2f}", SENSOR_ID_TEMP, temperature);
    client.publish("sensor/readings", jsonTemp);

    char jsonHumi[100];
    sprintf(jsonHumi, "{\"sensor_id\": %d, \"value\": %.2f}", SENSOR_ID_HUMI, humidity);
    client.publish("sensor/readings", jsonHumi);

    char jsonHeat[100];
    sprintf(jsonHeat, "{\"sensor_id\": %d, \"value\": %.2f}", SENSOR_ID_HEAT, heatIndexC);
    client.publish("sensor/readings", jsonHeat);
  }

  // ==== Gửi độ sáng ====
  if (lux >= 0) {
    Serial.print("Độ sáng: "); Serial.println(lux);
    char jsonLux[100];
    sprintf(jsonLux, "{\"sensor_id\": %d, \"value\": %.2f}", SENSOR_ID_LUX, lux);
    client.publish("sensor/readings", jsonLux);
  } else {
    Serial.println("Không đọc được lux!");
  }

  delay(3000);
}
