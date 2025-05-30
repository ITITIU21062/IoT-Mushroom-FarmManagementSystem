#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

// ==== BH1750 Setup ====
#define SDA_PIN 14
#define SCL_PIN 15
BH1750 lightMeter;

// ==== DHT22 Setup ====
#define DHTPIN 13         // DATA chân cảm biến
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ==== Biến đọc dữ liệu ====
float temperature, humidity, heatIndexC;
float lux = -1;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ==== DHT22 Init ====
  dht.begin();
  Serial.println("Khởi động DHT22...");

  // ==== BH1750 Init ====
  Wire.begin(SDA_PIN, SCL_PIN);
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 đã sẵn sàng.");
  } else {
    Serial.println("Lỗi khởi tạo BH1750. Kiểm tra dây nối!");
    while (1);
  }
}

void loop() {
  // Đọc nhiệt độ và độ ẩm
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();       // Độ C
  heatIndexC = dht.computeHeatIndex(temperature, humidity, false); // Độ C

  // Đọc độ sáng
  lux = lightMeter.readLightLevel();

  // Kiểm tra lỗi
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Lỗi đọc từ DHT22!");
  } else {
    Serial.print("Độ ẩm: "); Serial.print(humidity); Serial.print(" %\t");
    Serial.print("Nhiệt độ: "); Serial.print(temperature); Serial.print(" °C\t");
    Serial.print("Chỉ số nhiệt (heat index): "); Serial.print(heatIndexC); Serial.println(" °C");
  }

  if (lux >= 0) {
    Serial.print("Độ sáng (lux): ");
    Serial.println(lux);
  } else {
    Serial.println("Không đọc được độ sáng từ BH1750.");
  }

  delay(3000); // Delay giữa mỗi lần đọc
}
