#include <Wire.h>
#include <Adafruit_INA219.h>
#include <DHT.h>

// INA219 object
Adafruit_INA219 ina219;

// ACS712 pin
const int ACS712_PIN = 34;  // GPIO pin connected to ACS712 OUT

// DHT11 sensor setup
#define DHTPIN 4        // Pin where the DHT11 is connected
#define DHTTYPE DHT11   // Define sensor type

DHT dht(DHTPIN, DHTTYPE); // Create DHT object

// Voltage measurement (from ACS712)
float voltage = 0.0;  // ACS712 output (V)

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize INA219 sensor
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 sensor");
    while (1);
  }
  
  // Calibrate the INA219 (optional, depends on your system)
  ina219.setCalibration_32V_2A();
  
  // Set up ACS712 pin
  pinMode(ACS712_PIN, INPUT);

  // Initialize DHT11 sensor
  dht.begin();

  Serial.println("System initialized");
}

void loop() {
  // Read voltage from INA219 (Solar panel voltage)
  float panelVoltage = ina219.getBusVoltage_V();  // Voltage (V)
  
  // Read current from ACS712 (analog value)
  int adcValue = analogRead(ACS712_PIN);
  voltage = (adcValue * (3.3 / 4095.0));  // Convert to voltage (0-3.3V)
  
  // Convert the ACS712 voltage to current (ACS712 5A range - 2.5V at 0A)
  // Adjust with calibration value if necessary
  float currentACS712 = (voltage - 2) * (5.0 / 2.5); // Convert to Amps
  
  // Calculate power using INA219 voltage and ACS712 current
  float power = panelVoltage * currentACS712;  // Power (W)

  // Read temperature and humidity from DHT11
  float temperature = dht.readTemperature(); // Temperature in Celsius
  float humidity = dht.readHumidity(); // Humidity in percentage
  
  // Check if DHT11 read failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Display readings
  Serial.print(" Panel Voltage : ");
  Serial.print(panelVoltage);
  Serial.println(" V");
  
  Serial.print("Panel Current: ");
  Serial.print(currentACS712);
  Serial.println(" A");

  // Display power calculated using INA219 voltage and ACS712 current
  Serial.print("Power (W): ");
  Serial.print(power);
  Serial.println(" W");

  // Display DHT11 readings
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Delay before next reading
  delay(2000); // Delay for 2 seconds
}
