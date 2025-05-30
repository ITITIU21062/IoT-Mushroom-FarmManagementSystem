#include <Wire.h>
#include <BH1750.h>

#define SDA_PIN 14  // GPIO14 trên ESP32-CAM
#define SCL_PIN 15  // GPIO15

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN); // Khởi tạo I2C với chân tùy chỉnh
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 đã sẵn sàng.");
  } else {
    Serial.println("Lỗi khởi tạo BH1750. Kiểm tra dây nối!");
    while (1);
  }
}

void loop() {
  float lux = lightMeter.readLightLevel();
  if (lux >= 0) {
    Serial.print("Độ sáng: ");
    Serial.print(lux);
    Serial.println(" lux");
  } else {
    Serial.println("Không đọc được giá trị ánh sáng.");
  }

  delay(2000);
}
