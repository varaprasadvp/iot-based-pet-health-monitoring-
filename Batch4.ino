#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "MAX30105.h"   // SparkFun MAX3010x library (works for MAX30102)

// Objects
Adafruit_MPU6050 mpu;
MAX30105 particleSensor;

// I2C objects for different buses
TwoWire I2C_MPU = TwoWire(0);   // For MPU6050
TwoWire I2C_MAX = TwoWire(1);   // For MAX30102

// Temperature sensor pin (LM35 on ESP32)
#define TEMP_PIN 25   // Analog input pin

void setup() {
  Serial.begin(115200);
  delay(1000);

  // === Initialize MPU6050 ===
  I2C_MPU.begin(18, 19, 400000);   // SDA=18, SCL=19
  if (!mpu.begin(0x68, &I2C_MPU)) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("MPU6050 Initialized");

  // === Initialize MAX30102 ===
  I2C_MAX.begin(22, 21, 400000);   // SDA=22, SCL=21
  if (!particleSensor.begin(I2C_MAX)) {  
    Serial.println("MAX30102 not found. Check wiring/power.");
    while (1) { delay(10); }
  }
  particleSensor.setup(); // Default configuration
  Serial.println("MAX30102 Initialized");

  Serial.println("ESP32 Sensors Ready");
}

void loop() {
  // === MPU6050 Readings ===
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Accel X: "); Serial.print(a.acceleration.x);
  Serial.print(" Y: "); Serial.print(a.acceleration.y);
  Serial.print(" Z: "); Serial.print(a.acceleration.z); Serial.print(" m/s^2 ");

  Serial.print(" | Gyro X: "); Serial.print(g.gyro.x);
  Serial.print(" Y: "); Serial.print(g.gyro.y);
  Serial.print(" Z: "); Serial.print(g.gyro.z); Serial.println(" rad/s");

  // === MAX30102 Readings ===
  long irValue = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  Serial.print("MAX30102 IR: "); Serial.print(irValue);
  Serial.print(" | RED: "); Serial.println(redValue);

  // === Analog Temperature Sensor (LM35) ===
  int rawValue = analogRead(TEMP_PIN);
  float voltage = (rawValue / 4095.0) * 3.3; // ESP32 ADC: 0-4095 → 0-3.3V
  float temperatureC = voltage * 100.0;      // LM35: 10mV per °C

  Serial.print("Temperature Sensor: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  Serial.println("--------------------------------------------------");
  delay(1000);
}
