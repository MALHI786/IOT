// Assignment 1 - Embedded IoT Systems
// Student: SALMAN ASHRAF
// Registration: 1272

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Hardware connections - UPDATED WITH YOUR COLORS
#define WHITE_LED 2       // Changed from Red to White
#define YELLOW_LED 4      // Changed from Green to Yellow  
#define DARK_GREEN_LED 5  // Changed from Blue to Dark Green
#define MODE_BUTTON 26    // Button for changing modes
#define RESET_BUTTON 27   // Button for resetting to OFF
#define BUZZER_PIN 15     // Buzzer pin

// Display configuration
Adafruit_SSD1306 oled_display(128, 64, &Wire, -1);

// Program variables
int current_state = 1;
unsigned long last_time = 0;
bool light_status = false;

// Show message on screen
void display_message(const String &text) {
  oled_display.clearDisplay();
  oled_display.setTextSize(1);
  oled_display.setTextColor(WHITE);
  oled_display.setCursor(5, 28);
  oled_display.print("Status: ");
  oled_display.println(text);
  oled_display.display();
}

// Sound feedback
void sound_beep(int pitch, int length) {
  tone(BUZZER_PIN, pitch, length);
  delay(length);
  noTone(BUZZER_PIN);
}

// Initial boot sequence
void boot_sequence() {
  oled_display.clearDisplay();
  oled_display.setTextSize(1);
  oled_display.setTextColor(WHITE);
  oled_display.setCursor(10, 30);
  oled_display.print("System Starting");
  oled_display.display();

  // Flashing lights sequence with your colors
  for (int count = 0; count < 3; count++) {
    digitalWrite(WHITE_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    sound_beep(600 + (count * 300), 80);
    delay(100);
    digitalWrite(WHITE_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    delay(100);
  }

  digitalWrite(WHITE_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(DARK_GREEN_LED, LOW);
  display_message("Ready");
  delay(500);
}

void setup() {
  // Configure pins
  pinMode(WHITE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(DARK_GREEN_LED, OUTPUT);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize display
  oled_display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled_display.clearDisplay();
  oled_display.display();

  boot_sequence();
  display_message("All LEDs Off");
}

void loop() {
  // Handle mode selection
  if (digitalRead(MODE_BUTTON) == LOW) {
    delay(150);
    current_state++;
    if (current_state > 4) current_state = 1;

    switch (current_state) {
      case 1:
        digitalWrite(WHITE_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        analogWrite(DARK_GREEN_LED, 0);
        display_message("All Off");
        sound_beep(750, 100);
        break;

      case 2:
        display_message("Alternate Blink");
        sound_beep(950, 100);
        break;

      case 3:
        digitalWrite(WHITE_LED, HIGH);
        digitalWrite(YELLOW_LED, HIGH);
        analogWrite(DARK_GREEN_LED, 0);
        display_message("All On");
        sound_beep(1150, 100);
        break;

      case 4:
        display_message("Green Fade");
        sound_beep(1400, 100);
        break;
    }
  }

  // Handle reset function
  if (digitalRead(RESET_BUTTON) == LOW) {
    delay(150);
    current_state = 1;
    digitalWrite(WHITE_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    analogWrite(DARK_GREEN_LED, 0);
    display_message("Reset Complete");
    sound_beep(450, 150);
  }

  // Execute current mode behavior
  if (current_state == 2) {
    // White and Yellow alternate blinking
    if (millis() - last_time >= 600) {
      last_time = millis();
      light_status = !light_status;
      digitalWrite(WHITE_LED, light_status);
      digitalWrite(YELLOW_LED, !light_status);
    }
  }

  if (current_state == 4) {
    // Dark Green LED fading (PWM)
    // Brightness increase
    for (int level = 0; level <= 255; level++) {
      analogWrite(DARK_GREEN_LED, level);
      delay(5);
    }
    // Brightness decrease
    for (int level = 255; level >= 0; level--) {
      analogWrite(DARK_GREEN_LED, level);
      delay(5);
    }
  }
}