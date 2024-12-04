#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions for Arduino Nano
#define SS 10      // Chip Select (NSS)
#define RST 9      // Reset
#define DIO0 2     // IRQ (DIO0)
#define LED_PIN 8  // LED indicator
#define BUTTON_PIN 7 // Push-button

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address to 0x27 (change if necessary)

void setup() {
  Serial.begin(9600); // Initialize Serial Monitor at 9600 baud rate
  while (!Serial);

  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set button pin as input with pull-up resistor
  digitalWrite(LED_PIN, LOW); // Ensure LED is off initially

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  // Initialize LoRa module
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) { // Start LoRa at 433 MHz
    lcd.clear();
    lcd.print("LoRa Init Failed!");
    Serial.println("LoRa initialization failed!");
    while (true) {
      digitalWrite(LED_PIN, HIGH); // Blink LED to indicate failure
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }
  }

  // Configure LoRa settings
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN); // Set transmission power
  LoRa.setSpreadingFactor(12);                // Set spreading factor
  LoRa.setSignalBandwidth(125E3);             // Set signal bandwidth
  LoRa.setCodingRate4(8);                     // Set coding rate

  lcd.clear();
  lcd.print("LoRa Ready!");
  Serial.println("LoRa initialized successfully!");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Check if button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    LoRa.beginPacket();         // Start a new LoRa packet
    LoRa.print("toggle");       // Send "toggle" message
    LoRa.endPacket();           // End LoRa packet
    Serial.println("Sent: toggle");
    lcd.clear();
    lcd.print("Sent: toggle");
    delay(200);                 // Debounce delay
  }

  // Check if there's a received packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received_message = "";
    while (LoRa.available()) {
      received_message += (char)LoRa.read();  // Read incoming message
    }

    Serial.println("Received: " + received_message);

    // Check if the received message is "toggle"
    if (received_message == "toggle") {
      digitalWrite(LED_PIN, HIGH); // Turn LED on
      lcd.clear();
      lcd.print("Received:");
      lcd.setCursor(0, 1);
      lcd.print(received_message); // Display received message
      delay(5000);                 // Display message for 5 seconds
      lcd.clear();                 // Clear LCD
      digitalWrite(LED_PIN, LOW);  // Turn LED off
    }
  }
}
