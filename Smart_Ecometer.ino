#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Define the I2C LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16x2 LCD

// Define sensor pins
#define LM35_PIN A1
#define MQ2_PIN A0
#define POT_PIN A2

// Define LED and Buzzer pins
#define GREEN_LED_PIN 6
#define RED_LED_PIN 4
#define BUZZER_PIN 5

// Initialize SIM800 serial on pins 2 (RX) and 3 (TX)
SoftwareSerial sim800(2, 3); // RX, TX

// Define thresholds
const float TEMP_THRESHOLD = 30.0; // Celsius
const int GAS_THRESHOLD = 400;     // Analog value, adjust based on calibration
const int NOISE_THRESHOLD = 512;   // Midpoint value for potentiometer

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  sim800.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Initialize LED and buzzer pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Display startup message with project details
  lcd.setCursor(0, 0);
  lcd.print("Smart Ecometer ");
  lcd.setCursor(0, 1);
  lcd.print("by Buya Kalicha");
  delay(300); // Display the message for 3 seconds

  // Clear the LCD and show ready status
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ecometer Ready");

  // Display startup message on Serial Monitor
  Serial.println("Ecometer V1.0 - Presented by Deftec");
  delay(100);
}

void loop() {
  // Read sensor values
  float temperature = analogRead(LM35_PIN) * 0.48828125; // LM35: 10mV per degree Celsius
  int gasLevel = analogRead(MQ2_PIN); // MQ2 gas sensor
  int noiseLevel = analogRead(POT_PIN); // Potentiometer as noise sensor

  // Display sensor labels on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas  Noise Temp");

  // Display sensor values on the LCD
  lcd.setCursor(0, 1);
  lcd.print(gasLevel); // Gas level
  lcd.setCursor(5, 1);
  lcd.print(noiseLevel); // Noise level
  lcd.setCursor(11, 1);
  lcd.print(temperature, 1); // Temperature with one decimal place
  lcd.print("C");

  // Print values to Serial Monitor for debugging
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Gas Level: ");
  Serial.print(gasLevel);
  Serial.print(", Noise Level: ");
  Serial.println(noiseLevel);

  // Check if any value exceeds the threshold
  bool alarm = false;
  String alertMessage = "Alert! ";

  if (temperature > TEMP_THRESHOLD) {
    alertMessage += "Temp High! ";
    alarm = true;
  }

  if (gasLevel > GAS_THRESHOLD) {
    alertMessage += "Gas High! ";
    alarm = true;
  }

  if (noiseLevel > NOISE_THRESHOLD) {
    alertMessage += "Noise High! ";
    alarm = true;
  }

  // LED and Buzzer control logic
  if (alarm) {
    // Turn on the red LED and buzzer
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH);

    // Send SMS alert
    sendSMS(alertMessage);
  } else {
    // Normal condition, turn on the green LED
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Add a delay between readings
  delay(200);
}

void sendSMS(String message) {
  sim800.println("AT"); // Check if module is ready
  delay(1000);
  sim800.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  sim800.println("AT+CMGS=\"+1234567890\""); // Replace with your phone number
  delay(1000);
  sim800.println(message); // The SMS message content
  delay(1000);
  sim800.println((char)26); // ASCII code for CTRL+Z to send SMS
  delay(1000);
}
