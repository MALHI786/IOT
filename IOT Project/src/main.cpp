#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>

// --- Configuration ---
const char* ssid = "Salman Malhi";         // Apna WiFi name likhein
const char* password = "Malhi777"; // Apna WiFi password likhein
const char* deviceName = "SolarSAS-Pro-01";

const char* serverUrl = "https://solarsas.vercel.app/api/telemetry/ingest";
const char* apiKey = "sk_live_51Pz7xXSolarSAS_9921_bc64e892f10a";

// --- Pin Mapping ---
const int voltagePin = 36; // GPIO36 (VP)
const int currentPin = 39; // GPIO39 (VN)

// OLED Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  
  // OLED Initialize
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.setTextColor(WHITE);

  // WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.setCursor(0,0);
    display.print("Connecting WiFi...");
    display.display();
  }
  Serial.println("\nWiFi Connected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    
    // 1. Read Sensors
    float rawVoltage = analogRead(voltagePin);
    float rawCurrent = analogRead(currentPin);

    // 2. Calibration Logic
    // Voltage: (Raw / 4095) * 3.3 * DividerFactor (e.g., 5.0 for 25V sensor)
    float voltage = (rawVoltage / 4095.0) * 3.3 * 5.0; 
    
    // Current (ACS712): Offset 2048 is 2.5V middle point
    float current = (rawCurrent - 2048.0) * (3.3 / 4095.0) / 0.185; 
    if (current < 0.05) current = 0; // Small noise filter
    
    float power = voltage * current;

    // 3. Serial Monitor Output
    Serial.printf("V: %.2fV, I: %.2fA, P: %.2fW\n", voltage, current, power);

    // 4. HTTPS Post to Vercel
    WiFiClientSecure *client = new WiFiClientSecure;
    if(client) {
      client->setInsecure(); // Skip SSL cert check for speed
      HTTPClient https;
      
      if (https.begin(*client, serverUrl)) {
        https.addHeader("Content-Type", "application/json");
        https.addHeader("X-API-Key", apiKey);

        // JSON Payload
        String jsonPayload = "{\"deviceId\": \"" + String(deviceName) + "\", \"voltage\": " + String(voltage) + ", \"current\": " + String(current) + "}";
        
        int httpResponseCode = https.POST(jsonPayload);
        Serial.printf("[HTTPS] Response: %d\n", httpResponseCode);

        // 5. OLED Display Update
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.println("SolarSAS Live");
        
        display.setTextSize(2);
        display.setCursor(0,15);
        display.printf("%.2f V\n", voltage);
        display.printf("%.2f A\n", current);
        
        display.setTextSize(1);
        display.setCursor(0,50);
        display.printf("Power: %.2f W", power);
        display.display();

        https.end();
      }
      delete client;
    }
  } else {
    WiFi.begin(ssid, password);
  }
  
  delay(5000); // 5 second ka interval
}