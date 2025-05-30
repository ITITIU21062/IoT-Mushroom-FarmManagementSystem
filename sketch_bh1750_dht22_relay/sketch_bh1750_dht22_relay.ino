#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

// ==== BH1750 Setup ====
#define SDA_PIN 14
#define SCL_PIN 15
BH1750 lightMeter;

// ==== DHT22 Setup ====
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ==== Relay quạt ====
#define FAN_RELAY_PIN 2  // Chân điều khiển relay

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

  // ==== Relay quạt Init ====
  pinMode(FAN_RELAY_PIN, OUTPUT);
  digitalWrite(FAN_RELAY_PIN, HIGH); // Tắt quạt lúc đầu
}

void loop() {
  // Đọc nhiệt độ và độ ẩm
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  heatIndexC = dht.computeHeatIndex(temperature, humidity, false);

  // Đọc độ sáng
  lux = lightMeter.readLightLevel();

  // Hiển thị DHT
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Lỗi đọc từ DHT22!");
  } else {
    Serial.print("Độ ẩm: "); Serial.print(humidity); Serial.print(" %\t");
    Serial.print("Nhiệt độ: "); Serial.print(temperature); Serial.print(" °C\t");
    Serial.print("Chỉ số nhiệt: "); Serial.print(heatIndexC); Serial.println(" °C");
  }

  // Hiển thị BH1750
  if (lux >= 0) {
    Serial.print("Độ sáng: "); Serial.print(lux); Serial.println(" lux");
  } else {
    Serial.println("Không đọc được độ sáng từ BH1750.");
  }

  // ==== Điều khiển quạt ====
  if (!isnan(heatIndexC)) {
    if (heatIndexC > 50) {
      digitalWrite(FAN_RELAY_PIN, LOW);  // Bật quạt
      Serial.println("🔥 Nhiệt độ cao! Đã bật quạt.");
    } else {
      digitalWrite(FAN_RELAY_PIN, HIGH);   // Tắt quạt
      Serial.println("✅ Nhiệt độ ổn. Tắt quạt.");
    }
  }

  delay(3000);
}
