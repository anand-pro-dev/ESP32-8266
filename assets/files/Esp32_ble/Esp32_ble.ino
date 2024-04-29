#include <Arduino.h>
#include <BluetoothSerial.h>
// Define LED pin
const int ledPin = 2;
// Define BluetoothSerial object
BluetoothSerial SerialBT;
void setup() {
  // Initialize LED pin as an output
  pinMode(ledPin, OUTPUT);
  // Initialize SerialBT communication
  SerialBT.begin("ESP32_LED_Control"); // Bluetooth device name
  Serial.println("Bluetooth device name: ESP32_LED_Control");
  // Wait until SerialBT is initialized
  delay(1000);
}
void loop() {
  // Check if there's data available to read from SerialBT
  if (SerialBT.available()) {
    // Read the incoming byte from SerialBT
    char receivedChar = SerialBT.read();
    // Check the received character and perform corresponding actions
    switch (receivedChar) {
      case '1': // Turn on the LED
        digitalWrite(ledPin, HIGH);
        SerialBT.println("LED turned on");
        break;
      case '0': // Turn off the LED
        digitalWrite(ledPin, LOW);
        SerialBT.println("LED turned off");
        break;
      default:
        break;
    }
  }
  // Add any additional tasks or code here
}
